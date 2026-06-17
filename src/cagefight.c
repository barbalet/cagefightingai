#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cagefight.h"


#define MAX_RULES 256
#define MAX_NAME 80
#define MAX_LINE 256
#define MAX_TOKENS 24
#define MAX_PROGRAMS 32
#define MAX_TURNS 240
#define TOURNAMENT_BOUTS 3

#define PI 3.14159265358979323846
#define ARENA_RADIUS_M 3.00
#define ROBOT_RADIUS_M 0.34
#define ROBOT_MASS_KG 118.0
#define FRICTION 0.72
#define CLINCH_BREAK_GAP_M 0.22

typedef enum {
    PART_HEAD = 0,
    PART_TORSO,
    PART_L_ARM,
    PART_R_ARM,
    PART_L_LEG,
    PART_R_LEG,
    PART_COUNT,
    PART_INVALID = -1
} BodyPart;

typedef enum {
    USE_NONE = 0,
    USE_L_ARM,
    USE_R_ARM,
    USE_ANY_ARM,
    USE_L_LEG,
    USE_R_LEG,
    USE_ANY_LEG,
    USE_BOTH_ARMS,
    USE_BOTH_LEGS
} UseMode;

typedef enum {
    CMD_GUARD = 0,
    CMD_ADVANCE,
    CMD_RETREAT,
    CMD_STRAFE_L,
    CMD_STRAFE_R,
    CMD_CIRCLE_L,
    CMD_CIRCLE_R,
    CMD_RESET,
    CMD_STAND,
    CMD_L_JAB,
    CMD_R_CROSS,
    CMD_L_HOOK,
    CMD_R_HOOK,
    CMD_UPPERCUT,
    CMD_LOW_KICK,
    CMD_HIGH_KICK,
    CMD_KNEE,
    CMD_ELBOW,
    CMD_CLINCH,
    CMD_THROW,
    CMD_STOMP,
    CMD_COUNT,
    CMD_INVALID = -1
} CommandId;

typedef enum {
    OP_LT = 0,
    OP_LE,
    OP_GT,
    OP_GE,
    OP_EQ,
    OP_NE
} CompareOp;

typedef enum {
    METRIC_ALWAYS = 0,
    METRIC_SELF_PART,
    METRIC_OPP_PART,
    METRIC_SELF_PROCESSOR,
    METRIC_OPP_PROCESSOR,
    METRIC_SELF_HEAT,
    METRIC_OPP_HEAT,
    METRIC_SELF_SHOCK,
    METRIC_OPP_SHOCK,
    METRIC_SELF_STABILITY,
    METRIC_OPP_STABILITY,
    METRIC_SELF_DOWN,
    METRIC_OPP_DOWN,
    METRIC_DISTANCE,
    METRIC_CENTER_DISTANCE,
    METRIC_WALL
} MetricId;

typedef struct {
    CommandId id;
    BodyPart target;
    int line;
} Command;

typedef struct {
    int enabled;
    MetricId metric;
    BodyPart part;
    CompareOp op;
    double value;
} Condition;

typedef struct {
    Condition condition;
    Command command;
    int line;
} Rule;

typedef struct {
    char name[MAX_NAME];
    Rule rules[MAX_RULES];
    size_t count;
} Program;

typedef struct {
    CommandId id;
    const char *name;
    int is_attack;
    int damage;
    int accuracy;
    int speed;
    double min_gap_m;
    double max_gap_m;
    double move_impulse_m;
    double strike_impulse_m;
    BodyPart default_target;
    UseMode use_mode;
    int stability_cost;
    int heat_cost;
    const char *description;
} MoveSpec;

typedef struct {
    int integrity[PART_COUNT];
    int detached[PART_COUNT];
    int processor;
    int shock;
    int stability;
    int heat;
    int guard;
    int retreating;
    int advancing;
    int circling;
    int down;
    int down_ticks;
    int defeated;
    double x;
    double y;
    double vx;
    double vy;
    double facing;
    char method[96];
} RobotState;

typedef struct {
    RobotState robot[2];
    int clinch;
    uint32_t rng;
} Fight;

typedef struct {
    Command command;
    const MoveSpec *spec;
    int active;
    char note[160];
} Intent;

typedef struct {
    int winner;
    int turns;
    char method[96];
    int score[2];
} BoutResult;

static const int part_initial[PART_COUNT] = {
    100, 160, 90, 90, 110, 110
};

static const int part_armor[PART_COUNT] = {
    12, 18, 9, 9, 11, 11
};

static const MoveSpec move_specs[CMD_COUNT] = {
    { CMD_GUARD, "GUARD", 0, 0, 0, 7, 0.00, 0.00, 0.00, 0.00, PART_INVALID, USE_NONE, -3, -4,
      "Brace the armored frame and raise arms around head and torso." },
    { CMD_ADVANCE, "ADVANCE", 0, 0, 0, 8, 0.00, 0.00, 0.22, 0.00, PART_INVALID, USE_BOTH_LEGS, 2, 2,
      "Drive the body footprint toward the opponent." },
    { CMD_RETREAT, "RETREAT", 0, 0, 0, 8, 0.00, 0.00, 0.20, 0.00, PART_INVALID, USE_BOTH_LEGS, 2, 2,
      "Drive the body footprint away from the opponent." },
    { CMD_STRAFE_L, "STRAFE_L", 0, 0, 0, 8, 0.00, 0.00, 0.18, 0.00, PART_INVALID, USE_BOTH_LEGS, 3, 2,
      "Move laterally left while facing the opponent." },
    { CMD_STRAFE_R, "STRAFE_R", 0, 0, 0, 8, 0.00, 0.00, 0.18, 0.00, PART_INVALID, USE_BOTH_LEGS, 3, 2,
      "Move laterally right while facing the opponent." },
    { CMD_CIRCLE_L, "CIRCLE_L", 0, 0, 0, 8, 0.00, 0.00, 0.19, 0.00, PART_INVALID, USE_BOTH_LEGS, 3, 3,
      "Circle left with a shallow inward component." },
    { CMD_CIRCLE_R, "CIRCLE_R", 0, 0, 0, 8, 0.00, 0.00, 0.19, 0.00, PART_INVALID, USE_BOTH_LEGS, 3, 3,
      "Circle right with a shallow inward component." },
    { CMD_RESET, "RESET", 0, 0, 0, 6, 0.00, 0.00, 0.30, 0.00, PART_INVALID, USE_ANY_LEG, 5, 3,
      "Break clinch state and shove the body footprint backward." },
    { CMD_STAND, "STAND", 0, 0, 0, 4, 0.00, 0.00, 0.00, 0.00, PART_INVALID, USE_ANY_LEG, 10, 6,
      "Recover from a downed state if the lower frame can carry load." },
    { CMD_L_JAB, "L_JAB", 1, 10, 82, 10, 0.15, 0.95, 0.03, 0.04, PART_HEAD, USE_L_ARM, 3, 3,
      "Fast left linear strike used for range finding and processor shock." },
    { CMD_R_CROSS, "R_CROSS", 1, 18, 72, 8, 0.12, 0.88, 0.04, 0.09, PART_HEAD, USE_R_ARM, 5, 5,
      "Rear straight punch with stronger momentum transfer." },
    { CMD_L_HOOK, "L_HOOK", 1, 21, 66, 7, 0.00, 0.48, 0.03, 0.11, PART_HEAD, USE_L_ARM, 7, 6,
      "Short arc strike for pocket and collision range." },
    { CMD_R_HOOK, "R_HOOK", 1, 23, 64, 7, 0.00, 0.48, 0.03, 0.12, PART_HEAD, USE_R_ARM, 7, 7,
      "Heavy right arc strike with elevated knockdown pressure." },
    { CMD_UPPERCUT, "UPPERCUT", 1, 24, 61, 6, 0.00, 0.38, 0.02, 0.13, PART_HEAD, USE_ANY_ARM, 9, 8,
      "Vertical close-range strike against the head module." },
    { CMD_LOW_KICK, "LOW_KICK", 1, 20, 70, 7, 0.18, 0.78, 0.02, 0.12, PART_L_LEG, USE_ANY_LEG, 8, 7,
      "Low-line strike against knee, hip, and ankle load paths." },
    { CMD_HIGH_KICK, "HIGH_KICK", 1, 31, 48, 5, 0.22, 0.88, 0.02, 0.18, PART_HEAD, USE_R_LEG, 16, 12,
      "High-energy head strike with high fall risk and thermal cost." },
    { CMD_KNEE, "KNEE", 1, 26, 63, 6, 0.00, 0.30, 0.02, 0.15, PART_TORSO, USE_ANY_LEG, 10, 8,
      "Short piston strike to torso or head from contact range." },
    { CMD_ELBOW, "ELBOW", 1, 25, 67, 7, 0.00, 0.28, 0.01, 0.14, PART_HEAD, USE_ANY_ARM, 7, 6,
      "Compact close strike using the forearm hinge and elbow hardpoint." },
    { CMD_CLINCH, "CLINCH", 1, 4, 74, 8, 0.00, 0.18, 0.03, 0.02, PART_TORSO, USE_BOTH_ARMS, 6, 5,
      "Attach both arms to the opponent frame and constrain separation." },
    { CMD_THROW, "THROW", 1, 34, 52, 4, 0.00, 0.10, 0.00, 0.34, PART_TORSO, USE_BOTH_ARMS, 18, 14,
      "Clinch-only rotational takedown with high knockback and fall pressure." },
    { CMD_STOMP, "STOMP", 1, 29, 55, 4, 0.00, 0.28, 0.00, 0.16, PART_R_LEG, USE_ANY_LEG, 15, 11,
      "Downward strike against a close or downed opponent." }
};

static void append_text(char *buffer, size_t size, const char *fmt, ...)
{
    size_t len;
    va_list args;

    if (size == 0) {
        return;
    }

    len = strlen(buffer);
    if (len >= size - 1) {
        return;
    }

    va_start(args, fmt);
    vsnprintf(buffer + len, size - len, fmt, args);
    va_end(args);
}

static void copy_text(char *dst, size_t dst_size, const char *src)
{
    if (dst_size == 0) {
        return;
    }
    snprintf(dst, dst_size, "%s", src ? src : "");
}

static int equals_ci(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int starts_with_ci(const char *text, const char *prefix)
{
    while (*prefix != '\0') {
        if (*text == '\0') {
            return 0;
        }
        if (toupper((unsigned char)*text) != toupper((unsigned char)*prefix)) {
            return 0;
        }
        text++;
        prefix++;
    }
    return 1;
}

static char *trim(char *text)
{
    char *end;

    while (isspace((unsigned char)*text)) {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    end = text + strlen(text) - 1;
    while (end > text && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return text;
}

static const char *base_name(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static double clamp_double(double value, double min_value, double max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static uint32_t rng_next(uint32_t *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static int rng_range(uint32_t *state, int min_value, int max_value)
{
    uint32_t value;
    int span;

    if (max_value <= min_value) {
        return min_value;
    }

    value = rng_next(state);
    span = max_value - min_value + 1;
    return min_value + (int)(value % (uint32_t)span);
}

static const char *part_name(BodyPart part)
{
    switch (part) {
    case PART_HEAD:
        return "HEAD";
    case PART_TORSO:
        return "TORSO";
    case PART_L_ARM:
        return "L_ARM";
    case PART_R_ARM:
        return "R_ARM";
    case PART_L_LEG:
        return "L_LEG";
    case PART_R_LEG:
        return "R_LEG";
    default:
        return "NONE";
    }
}

static int is_limb(BodyPart part)
{
    return part == PART_L_ARM || part == PART_R_ARM ||
           part == PART_L_LEG || part == PART_R_LEG;
}

static int tokenize(char *text, char **tokens, int max_tokens)
{
    int count = 0;
    char *token = strtok(text, " \t\r\n");

    while (token != NULL && count < max_tokens) {
        tokens[count++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    return count;
}

static CommandId command_from_token(const char *token)
{
    int i;

    for (i = 0; i < CMD_COUNT; i++) {
        if (equals_ci(token, move_specs[i].name)) {
            return move_specs[i].id;
        }
    }

    if (equals_ci(token, "JAB")) {
        return CMD_L_JAB;
    }
    if (equals_ci(token, "CROSS")) {
        return CMD_R_CROSS;
    }
    if (equals_ci(token, "HOOK")) {
        return CMD_L_HOOK;
    }
    if (equals_ci(token, "KICK")) {
        return CMD_LOW_KICK;
    }
    if (equals_ci(token, "LEFT") || equals_ci(token, "STRAFE_LEFT")) {
        return CMD_STRAFE_L;
    }
    if (equals_ci(token, "RIGHT") || equals_ci(token, "STRAFE_RIGHT")) {
        return CMD_STRAFE_R;
    }
    if (equals_ci(token, "CIRCLE_LEFT")) {
        return CMD_CIRCLE_L;
    }
    if (equals_ci(token, "CIRCLE_RIGHT")) {
        return CMD_CIRCLE_R;
    }
    if (equals_ci(token, "GET_UP")) {
        return CMD_STAND;
    }

    return CMD_INVALID;
}

static BodyPart part_from_token(const char *token)
{
    if (equals_ci(token, "HEAD") || equals_ci(token, "PROCESSOR") ||
        equals_ci(token, "CRANIUM")) {
        return PART_HEAD;
    }
    if (equals_ci(token, "TORSO") || equals_ci(token, "BODY") ||
        equals_ci(token, "CORE")) {
        return PART_TORSO;
    }
    if (equals_ci(token, "L_ARM") || equals_ci(token, "LEFT_ARM")) {
        return PART_L_ARM;
    }
    if (equals_ci(token, "R_ARM") || equals_ci(token, "RIGHT_ARM")) {
        return PART_R_ARM;
    }
    if (equals_ci(token, "L_LEG") || equals_ci(token, "LEFT_LEG")) {
        return PART_L_LEG;
    }
    if (equals_ci(token, "R_LEG") || equals_ci(token, "RIGHT_LEG")) {
        return PART_R_LEG;
    }
    return PART_INVALID;
}

static int parse_operator(const char *token, CompareOp *op)
{
    if (strcmp(token, "<") == 0) {
        *op = OP_LT;
        return 1;
    }
    if (strcmp(token, "<=") == 0) {
        *op = OP_LE;
        return 1;
    }
    if (strcmp(token, ">") == 0) {
        *op = OP_GT;
        return 1;
    }
    if (strcmp(token, ">=") == 0) {
        *op = OP_GE;
        return 1;
    }
    if (strcmp(token, "==") == 0 || strcmp(token, "=") == 0) {
        *op = OP_EQ;
        return 1;
    }
    if (strcmp(token, "!=") == 0) {
        *op = OP_NE;
        return 1;
    }
    return 0;
}

static int parse_command_tokens(char **tokens, int start, int count,
                                Command *command, char *error,
                                size_t error_size, const char *path,
                                int line_no)
{
    CommandId command_id;
    BodyPart target = PART_INVALID;

    if (start >= count) {
        snprintf(error, error_size, "%s:%d missing command", path, line_no);
        return 0;
    }

    command_id = command_from_token(tokens[start]);
    if (command_id == CMD_INVALID) {
        snprintf(error, error_size, "%s:%d unknown command '%s'",
                 path, line_no, tokens[start]);
        return 0;
    }

    if (start + 1 < count) {
        target = part_from_token(tokens[start + 1]);
        if (target == PART_INVALID) {
            snprintf(error, error_size, "%s:%d unknown target '%s'",
                     path, line_no, tokens[start + 1]);
            return 0;
        }
    }

    command->id = command_id;
    command->target = target;
    command->line = line_no;
    return 1;
}

static int parse_condition_tokens(char **tokens, int start, int end,
                                  Condition *condition, char *error,
                                  size_t error_size, const char *path,
                                  int line_no)
{
    int op_index = -1;
    int i;
    char *endptr;

    memset(condition, 0, sizeof(*condition));
    condition->enabled = 1;
    condition->part = PART_INVALID;

    for (i = start; i < end; i++) {
        CompareOp op;
        if (parse_operator(tokens[i], &op)) {
            condition->op = op;
            op_index = i;
            break;
        }
    }

    if (op_index < 0 || op_index + 1 >= end) {
        snprintf(error, error_size, "%s:%d invalid IF condition", path, line_no);
        return 0;
    }

    condition->value = strtod(tokens[op_index + 1], &endptr);
    if (endptr == tokens[op_index + 1] || *endptr != '\0') {
        snprintf(error, error_size, "%s:%d invalid condition value '%s'",
                 path, line_no, tokens[op_index + 1]);
        return 0;
    }

    if (op_index == start + 1) {
        if (equals_ci(tokens[start], "DIST") || equals_ci(tokens[start], "DISTANCE") ||
            equals_ci(tokens[start], "GAP") || equals_ci(tokens[start], "RANGE")) {
            condition->metric = METRIC_DISTANCE;
            return 1;
        }
        if (equals_ci(tokens[start], "CENTER_DISTANCE")) {
            condition->metric = METRIC_CENTER_DISTANCE;
            return 1;
        }
        if (equals_ci(tokens[start], "WALL") || equals_ci(tokens[start], "CAGE")) {
            condition->metric = METRIC_WALL;
            return 1;
        }
    }

    if (op_index == start + 2 &&
        (equals_ci(tokens[start], "SELF") || equals_ci(tokens[start], "ME") ||
         equals_ci(tokens[start], "OPP") || equals_ci(tokens[start], "OPPONENT") ||
         equals_ci(tokens[start], "ENEMY"))) {
        int self = equals_ci(tokens[start], "SELF") || equals_ci(tokens[start], "ME");
        BodyPart part = part_from_token(tokens[start + 1]);

        if (part != PART_INVALID) {
            condition->metric = self ? METRIC_SELF_PART : METRIC_OPP_PART;
            condition->part = part;
            return 1;
        }

        if (equals_ci(tokens[start + 1], "PROCESSOR") || equals_ci(tokens[start + 1], "CPU")) {
            condition->metric = self ? METRIC_SELF_PROCESSOR : METRIC_OPP_PROCESSOR;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "HEAT")) {
            condition->metric = self ? METRIC_SELF_HEAT : METRIC_OPP_HEAT;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "SHOCK")) {
            condition->metric = self ? METRIC_SELF_SHOCK : METRIC_OPP_SHOCK;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "STABILITY") || equals_ci(tokens[start + 1], "BALANCE")) {
            condition->metric = self ? METRIC_SELF_STABILITY : METRIC_OPP_STABILITY;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "DOWN") || equals_ci(tokens[start + 1], "FALLEN")) {
            condition->metric = self ? METRIC_SELF_DOWN : METRIC_OPP_DOWN;
            return 1;
        }
        if (self && (equals_ci(tokens[start + 1], "WALL") || equals_ci(tokens[start + 1], "CAGE"))) {
            condition->metric = METRIC_WALL;
            return 1;
        }
    }

    snprintf(error, error_size, "%s:%d unknown condition metric", path, line_no);
    return 0;
}

static int load_program(const char *path, Program *program, char *error,
                        size_t error_size)
{
    FILE *file;
    char line[MAX_LINE];
    int line_no = 0;

    memset(program, 0, sizeof(*program));
    copy_text(program->name, sizeof(program->name), base_name(path));

    file = fopen(path, "r");
    if (file == NULL) {
        snprintf(error, error_size, "cannot open %s", path);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *comment;
        char *text;
        char *tokens[MAX_TOKENS];
        int token_count;
        Rule rule;

        line_no++;
        comment = strchr(line, '#');
        if (comment != NULL) {
            *comment = '\0';
        }
        text = trim(line);
        if (*text == '\0') {
            continue;
        }

        if (starts_with_ci(text, "name:")) {
            char *name_text = trim(text + 5);
            if (*name_text != '\0') {
                copy_text(program->name, sizeof(program->name), name_text);
            }
            continue;
        }

        token_count = tokenize(text, tokens, MAX_TOKENS);
        if (token_count == 0) {
            continue;
        }

        memset(&rule, 0, sizeof(rule));
        rule.command.target = PART_INVALID;
        rule.line = line_no;

        if (equals_ci(tokens[0], "IF")) {
            int then_index = -1;
            int i;

            for (i = 1; i < token_count; i++) {
                if (equals_ci(tokens[i], "THEN")) {
                    then_index = i;
                    break;
                }
            }

            if (then_index < 0) {
                snprintf(error, error_size, "%s:%d IF rule missing THEN",
                         path, line_no);
                fclose(file);
                return 0;
            }

            if (!parse_condition_tokens(tokens, 1, then_index, &rule.condition,
                                        error, error_size, path, line_no) ||
                !parse_command_tokens(tokens, then_index + 1, token_count,
                                      &rule.command, error, error_size, path,
                                      line_no)) {
                fclose(file);
                return 0;
            }
        } else if (!parse_command_tokens(tokens, 0, token_count, &rule.command,
                                         error, error_size, path, line_no)) {
            fclose(file);
            return 0;
        }

        if (program->count >= MAX_RULES) {
            snprintf(error, error_size, "%s has more than %d rules",
                     path, MAX_RULES);
            fclose(file);
            return 0;
        }

        program->rules[program->count++] = rule;
    }

    fclose(file);

    if (program->count == 0) {
        snprintf(error, error_size, "%s contains no executable commands", path);
        return 0;
    }

    return 1;
}

static double center_distance(const Fight *fight)
{
    double dx = fight->robot[1].x - fight->robot[0].x;
    double dy = fight->robot[1].y - fight->robot[0].y;
    return sqrt(dx * dx + dy * dy);
}

static double surface_gap(const Fight *fight)
{
    return clamp_double(center_distance(fight) - 2.0 * ROBOT_RADIUS_M, 0.0, 99.0);
}

static double wall_gap(const RobotState *robot)
{
    double d = sqrt(robot->x * robot->x + robot->y * robot->y);
    return clamp_double(ARENA_RADIUS_M - ROBOT_RADIUS_M - d, 0.0, ARENA_RADIUS_M);
}

static void vector_to_opponent(const Fight *fight, int side, double *nx, double *ny)
{
    const RobotState *self = &fight->robot[side];
    const RobotState *opp = &fight->robot[side == 0 ? 1 : 0];
    double dx = opp->x - self->x;
    double dy = opp->y - self->y;
    double dist = sqrt(dx * dx + dy * dy);

    if (dist < 0.0001) {
        *nx = side == 0 ? 1.0 : -1.0;
        *ny = 0.0;
    } else {
        *nx = dx / dist;
        *ny = dy / dist;
    }
}

static int mode_available(const RobotState *robot, UseMode mode)
{
    switch (mode) {
    case USE_NONE:
        return 1;
    case USE_L_ARM:
        return !robot->detached[PART_L_ARM];
    case USE_R_ARM:
        return !robot->detached[PART_R_ARM];
    case USE_ANY_ARM:
        return !robot->detached[PART_L_ARM] || !robot->detached[PART_R_ARM];
    case USE_L_LEG:
        return !robot->detached[PART_L_LEG];
    case USE_R_LEG:
        return !robot->detached[PART_R_LEG];
    case USE_ANY_LEG:
        return !robot->detached[PART_L_LEG] || !robot->detached[PART_R_LEG];
    case USE_BOTH_ARMS:
        return !robot->detached[PART_L_ARM] && !robot->detached[PART_R_ARM];
    case USE_BOTH_LEGS:
        return !robot->detached[PART_L_LEG] && !robot->detached[PART_R_LEG];
    default:
        return 0;
    }
}

static BodyPart guard_arm(const RobotState *robot)
{
    if (!robot->detached[PART_L_ARM] &&
        (robot->integrity[PART_L_ARM] >= robot->integrity[PART_R_ARM] ||
         robot->detached[PART_R_ARM])) {
        return PART_L_ARM;
    }
    if (!robot->detached[PART_R_ARM]) {
        return PART_R_ARM;
    }
    return PART_INVALID;
}

static int max_stability(const RobotState *robot)
{
    int max_value = 100;

    if (robot->detached[PART_L_LEG]) {
        max_value -= 38;
    }
    if (robot->detached[PART_R_LEG]) {
        max_value -= 38;
    }
    if (robot->detached[PART_L_ARM]) {
        max_value -= 4;
    }
    if (robot->detached[PART_R_ARM]) {
        max_value -= 4;
    }
    if (robot->down) {
        max_value -= 12;
    }
    return clamp_int(max_value, 8, 100);
}

static double leg_mobility_scale(const RobotState *robot)
{
    int live_legs = 0;
    int leg_integrity = 0;
    double scale;

    if (!robot->detached[PART_L_LEG]) {
        live_legs++;
        leg_integrity += clamp_int(robot->integrity[PART_L_LEG], 0, part_initial[PART_L_LEG]);
    }
    if (!robot->detached[PART_R_LEG]) {
        live_legs++;
        leg_integrity += clamp_int(robot->integrity[PART_R_LEG], 0, part_initial[PART_R_LEG]);
    }

    if (live_legs == 0 || robot->down) {
        return 0.0;
    }

    scale = (double)leg_integrity / (double)(live_legs * part_initial[PART_L_LEG]);
    if (live_legs == 1) {
        scale *= 0.45;
    }
    scale *= 1.0 - clamp_double((double)robot->heat / 220.0, 0.0, 0.45);
    return clamp_double(scale, 0.10, 1.0);
}

static void init_robot(RobotState *robot, int side)
{
    int i;

    memset(robot, 0, sizeof(*robot));
    for (i = 0; i < PART_COUNT; i++) {
        robot->integrity[i] = part_initial[i];
        robot->detached[i] = 0;
    }
    robot->processor = 100;
    robot->shock = 0;
    robot->stability = 100;
    robot->heat = 0;
    robot->x = side == 0 ? -1.15 : 1.15;
    robot->y = side == 0 ? -0.20 : 0.20;
    robot->facing = side == 0 ? 0.0 : PI;
    robot->method[0] = '\0';
}

static void init_fight(Fight *fight, uint32_t seed)
{
    init_robot(&fight->robot[0], 0);
    init_robot(&fight->robot[1], 1);
    fight->clinch = 0;
    fight->rng = seed == 0 ? 1u : seed;
}

static void recover_robot(RobotState *robot)
{
    int cap;

    robot->guard = 0;
    robot->retreating = 0;
    robot->advancing = 0;
    robot->circling = 0;

    if (robot->heat > 0) {
        robot->heat -= 4;
        if (robot->heat < 0) {
            robot->heat = 0;
        }
    }

    if (robot->shock > 0) {
        robot->shock -= robot->down ? 1 : 3;
        if (robot->shock < 0) {
            robot->shock = 0;
        }
    }

    if (robot->down_ticks > 0) {
        robot->down_ticks--;
    }

    cap = max_stability(robot);
    if (robot->stability < cap && !robot->down) {
        robot->stability += 4;
        if (robot->stability > cap) {
            robot->stability = cap;
        }
    }
}

static void mark_defeat(RobotState *robot, const char *method)
{
    robot->defeated = 1;
    copy_text(robot->method, sizeof(robot->method), method);
}

static int evaluate_defeat(RobotState *robot)
{
    if (robot->defeated) {
        return 1;
    }

    if (robot->processor <= 0 || robot->integrity[PART_HEAD] <= 0) {
        mark_defeat(robot, "processor kill by head module failure");
        return 1;
    }
    if (robot->integrity[PART_TORSO] <= 0) {
        mark_defeat(robot, "technical knockout by torso power-bus collapse");
        return 1;
    }
    if (robot->shock >= 100) {
        mark_defeat(robot, "knockout by cranial watchdog reset");
        return 1;
    }
    if (robot->detached[PART_L_LEG] && robot->detached[PART_R_LEG]) {
        mark_defeat(robot, "mobility kill by bilateral leg removal");
        return 1;
    }

    return 0;
}

static void detach_if_needed(RobotState *robot, BodyPart part, char *out,
                             size_t out_size)
{
    if (is_limb(part) && !robot->detached[part] && robot->integrity[part] <= 0) {
        robot->detached[part] = 1;
        robot->shock += 14;
        robot->stability -= 24;
        if (robot->stability < 0) {
            robot->stability = 0;
        }
        append_text(out, out_size, " %s detached.", part_name(part));
    }
}

static BodyPart select_target(const RobotState *defender, Command command,
                              const MoveSpec *spec)
{
    BodyPart target = command.target;

    if (target == PART_INVALID) {
        target = spec->default_target;
    }

    if (target == PART_INVALID) {
        target = PART_TORSO;
    }

    if (is_limb(target) && defender->detached[target]) {
        return PART_TORSO;
    }

    return target;
}

static int structural_score(const RobotState *robot)
{
    int i;
    int score = 0;

    for (i = 0; i < PART_COUNT; i++) {
        if (robot->integrity[i] > 0) {
            score += robot->integrity[i];
        }
    }
    score += robot->processor * 2;
    score += robot->stability;
    score -= robot->shock;
    score -= robot->heat / 2;
    if (robot->down) {
        score -= 45;
    }
    return score;
}

static double metric_value(const Condition *condition, const Fight *fight,
                           int side)
{
    const RobotState *self = &fight->robot[side];
    const RobotState *opp = &fight->robot[side == 0 ? 1 : 0];

    switch (condition->metric) {
    case METRIC_SELF_PART:
        return (double)self->integrity[condition->part];
    case METRIC_OPP_PART:
        return (double)opp->integrity[condition->part];
    case METRIC_SELF_PROCESSOR:
        return (double)self->processor;
    case METRIC_OPP_PROCESSOR:
        return (double)opp->processor;
    case METRIC_SELF_HEAT:
        return (double)self->heat;
    case METRIC_OPP_HEAT:
        return (double)opp->heat;
    case METRIC_SELF_SHOCK:
        return (double)self->shock;
    case METRIC_OPP_SHOCK:
        return (double)opp->shock;
    case METRIC_SELF_STABILITY:
        return (double)self->stability;
    case METRIC_OPP_STABILITY:
        return (double)opp->stability;
    case METRIC_SELF_DOWN:
        return self->down ? 1.0 : 0.0;
    case METRIC_OPP_DOWN:
        return opp->down ? 1.0 : 0.0;
    case METRIC_DISTANCE:
        return surface_gap(fight);
    case METRIC_CENTER_DISTANCE:
        return center_distance(fight);
    case METRIC_WALL:
        return wall_gap(self);
    case METRIC_ALWAYS:
    default:
        return 1.0;
    }
}

static int compare_value(double left, CompareOp op, double right)
{
    switch (op) {
    case OP_LT:
        return left < right;
    case OP_LE:
        return left <= right;
    case OP_GT:
        return left > right;
    case OP_GE:
        return left >= right;
    case OP_EQ:
        return fabs(left - right) < 0.0001;
    case OP_NE:
        return fabs(left - right) >= 0.0001;
    default:
        return 0;
    }
}

static int condition_matches(const Condition *condition, const Fight *fight,
                             int side)
{
    double value;

    if (!condition->enabled) {
        return 1;
    }

    value = metric_value(condition, fight, side);
    return compare_value(value, condition->op, condition->value);
}

static Command select_command(const Program *program, const Fight *fight,
                              int side, size_t *cursor, char *out,
                              size_t out_size)
{
    size_t i;
    Command fallback;

    for (i = 0; i < program->count; i++) {
        const Rule *rule = &program->rules[i];
        if (rule->condition.enabled && condition_matches(&rule->condition, fight, side)) {
            append_text(out, out_size, "R%d conditional line %d selected. ",
                        side + 1, rule->line);
            return rule->command;
        }
    }

    memset(&fallback, 0, sizeof(fallback));
    fallback.id = CMD_GUARD;
    fallback.target = PART_INVALID;

    if (program->count == 0) {
        return fallback;
    }

    for (i = 0; i < program->count; i++) {
        size_t idx = (*cursor + i) % program->count;
        if (!program->rules[idx].condition.enabled) {
            *cursor = (idx + 1) % program->count;
            return program->rules[idx].command;
        }
    }

    return fallback;
}

static Intent build_intent(const Program *program, const Fight *fight,
                           int side, size_t *cursor, char *out,
                           size_t out_size)
{
    Intent intent;
    Command command;
    const MoveSpec *spec;
    const RobotState *robot = &fight->robot[side];

    memset(&intent, 0, sizeof(intent));
    command = select_command(program, fight, side, cursor, out, out_size);
    spec = &move_specs[command.id];
    intent.command = command;
    intent.spec = spec;
    intent.active = spec->is_attack;

    if (robot->down && command.id != CMD_STAND && command.id != CMD_GUARD) {
        intent.command.id = CMD_STAND;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_STAND];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "%s impossible while down; command converted to STAND",
                 spec->name);
        return intent;
    }

    if (!mode_available(robot, spec->use_mode)) {
        intent.command.id = robot->down ? CMD_STAND : CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[intent.command.id];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "%s actuator path unavailable; command converted to %s",
                 spec->name, intent.spec->name);
        return intent;
    }

    if (robot->heat >= 130 && command.id != CMD_STAND) {
        intent.command.id = CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_GUARD];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "thermal clamp engaged; command converted to GUARD");
    }

    return intent;
}

static void fall_down(RobotState *robot, const char *reason, char *out,
                      size_t out_size)
{
    if (!robot->down) {
        robot->down = 1;
        robot->down_ticks = 2;
        robot->stability = clamp_int(robot->stability - 18, 0, max_stability(robot));
        robot->shock = clamp_int(robot->shock + 6, 0, 140);
        append_text(out, out_size, " falls backward by %s.", reason);
    }
}

static void stand_robot(Fight *fight, int side, char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    int chance;
    int roll;

    if (!robot->down) {
        robot->guard = 1;
        robot->stability = clamp_int(robot->stability + 4, 0, max_stability(robot));
        append_text(out, out_size, "R%d STAND holds upright brace. ", side + 1);
        return;
    }

    if (robot->down_ticks > 0) {
        append_text(out, out_size, "R%d STAND waiting for frame recovery. ", side + 1);
        return;
    }

    if (!mode_available(robot, USE_ANY_LEG)) {
        append_text(out, out_size, "R%d STAND fails; lower frame unavailable. ", side + 1);
        return;
    }

    chance = 58 + robot->stability / 3 - robot->heat / 10;
    chance = clamp_int(chance, 10, 92);
    roll = rng_range(&fight->rng, 1, 100);
    if (roll <= chance) {
        robot->down = 0;
        robot->stability = clamp_int(robot->stability + 30, 0, max_stability(robot));
        append_text(out, out_size, "R%d STAND recovers upright (roll %d/%d). ",
                    side + 1, roll, chance);
    } else {
        robot->shock = clamp_int(robot->shock + 2, 0, 140);
        append_text(out, out_size, "R%d STAND fails recovery (roll %d/%d). ",
                    side + 1, roll, chance);
    }
}

static void apply_motion_command(Fight *fight, int side, const Intent *intent,
                                 char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    double nx;
    double ny;
    double tx;
    double ty;
    double scale;
    double impulse;

    if (intent->spec->is_attack) {
        return;
    }

    robot->heat = clamp_int(robot->heat + intent->spec->heat_cost, 0, 170);
    robot->stability = clamp_int(robot->stability - intent->spec->stability_cost,
                                 0, max_stability(robot));

    if (intent->command.id == CMD_STAND) {
        stand_robot(fight, side, out, out_size);
        return;
    }

    if (intent->command.id == CMD_GUARD) {
        robot->guard = 1;
        robot->stability = clamp_int(robot->stability + 2, 0, max_stability(robot));
        append_text(out, out_size, "R%d GUARD braces physical frame. ", side + 1);
        return;
    }

    if (robot->down) {
        append_text(out, out_size, "R%d %s has no movement while down. ",
                    side + 1, intent->spec->name);
        return;
    }

    vector_to_opponent(fight, side, &nx, &ny);
    tx = -ny;
    ty = nx;
    scale = leg_mobility_scale(robot);
    impulse = intent->spec->move_impulse_m * scale;

    switch (intent->command.id) {
    case CMD_ADVANCE:
        robot->advancing = 1;
        robot->vx += nx * impulse;
        robot->vy += ny * impulse;
        append_text(out, out_size, "R%d ADVANCE impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_RETREAT:
        robot->retreating = 1;
        robot->vx -= nx * impulse;
        robot->vy -= ny * impulse;
        append_text(out, out_size, "R%d RETREAT impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_STRAFE_L:
        robot->circling = 1;
        robot->vx += tx * impulse;
        robot->vy += ty * impulse;
        append_text(out, out_size, "R%d STRAFE_L impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_STRAFE_R:
        robot->circling = 1;
        robot->vx -= tx * impulse;
        robot->vy -= ty * impulse;
        append_text(out, out_size, "R%d STRAFE_R impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_CIRCLE_L:
        robot->circling = 1;
        robot->vx += tx * impulse + nx * impulse * 0.25;
        robot->vy += ty * impulse + ny * impulse * 0.25;
        append_text(out, out_size, "R%d CIRCLE_L arcs around opponent. ", side + 1);
        break;
    case CMD_CIRCLE_R:
        robot->circling = 1;
        robot->vx -= tx * impulse - nx * impulse * 0.25;
        robot->vy -= ty * impulse - ny * impulse * 0.25;
        append_text(out, out_size, "R%d CIRCLE_R arcs around opponent. ", side + 1);
        break;
    case CMD_RESET:
        fight->clinch = 0;
        robot->retreating = 1;
        robot->vx -= nx * impulse;
        robot->vy -= ny * impulse;
        append_text(out, out_size, "R%d RESET breaks contact and backs out. ", side + 1);
        break;
    default:
        break;
    }
}

static void resolve_arena_contact(RobotState *robot, int side, char *out,
                                  size_t out_size)
{
    double dist = sqrt(robot->x * robot->x + robot->y * robot->y);
    double max_dist = ARENA_RADIUS_M - ROBOT_RADIUS_M;

    if (dist > max_dist) {
        double nx = dist < 0.0001 ? (side == 0 ? -1.0 : 1.0) : robot->x / dist;
        double ny = dist < 0.0001 ? 0.0 : robot->y / dist;
        double outward_v = robot->vx * nx + robot->vy * ny;

        robot->x = nx * max_dist;
        robot->y = ny * max_dist;

        if (outward_v > 0.0) {
            robot->vx -= nx * outward_v * 1.35;
            robot->vy -= ny * outward_v * 1.35;
            if (outward_v > 0.22) {
                int impact = clamp_int((int)(outward_v * 30.0), 1, 12);
                robot->shock = clamp_int(robot->shock + impact, 0, 140);
                robot->stability = clamp_int(robot->stability - impact, 0,
                                             max_stability(robot));
                append_text(out, out_size, "R%d cage wall impact %.2fm/t. ",
                            side + 1, outward_v);
            }
        }
    }
}

static void resolve_robot_contact(Fight *fight, char *out, size_t out_size)
{
    RobotState *a = &fight->robot[0];
    RobotState *b = &fight->robot[1];
    double dx = b->x - a->x;
    double dy = b->y - a->y;
    double dist = sqrt(dx * dx + dy * dy);
    double min_sep = 2.0 * ROBOT_RADIUS_M;
    double nx;
    double ny;

    if (dist < 0.0001) {
        nx = 1.0;
        ny = 0.0;
        dist = 0.0001;
    } else {
        nx = dx / dist;
        ny = dy / dist;
    }

    if (dist < min_sep) {
        double penetration = min_sep - dist;
        double rel = (a->vx - b->vx) * nx + (a->vy - b->vy) * ny;

        a->x -= nx * penetration * 0.5;
        a->y -= ny * penetration * 0.5;
        b->x += nx * penetration * 0.5;
        b->y += ny * penetration * 0.5;

        if (rel > 0.0) {
            double impulse = rel * 0.58;
            a->vx -= nx * impulse;
            a->vy -= ny * impulse;
            b->vx += nx * impulse;
            b->vy += ny * impulse;

            if (!fight->clinch && rel > 0.20) {
                int impact = clamp_int((int)(rel * 24.0), 1, 10);
                a->shock = clamp_int(a->shock + impact / 2, 0, 140);
                b->shock = clamp_int(b->shock + impact / 2, 0, 140);
                a->stability = clamp_int(a->stability - impact / 2, 0,
                                         max_stability(a));
                b->stability = clamp_int(b->stability - impact / 2, 0,
                                         max_stability(b));
                append_text(out, out_size, "hard-body collision rel %.2fm/t. ", rel);
            }
        }
    }
}

static void physics_step(Fight *fight, char *out, size_t out_size)
{
    int i;
    int pass;

    for (i = 0; i < 2; i++) {
        RobotState *robot = &fight->robot[i];
        if (robot->down) {
            robot->vx *= 0.58;
            robot->vy *= 0.58;
        }
        robot->x += robot->vx;
        robot->y += robot->vy;
        resolve_arena_contact(robot, i, out, out_size);
    }

    for (pass = 0; pass < 3; pass++) {
        resolve_robot_contact(fight, out, out_size);
        resolve_arena_contact(&fight->robot[0], 0, out, out_size);
        resolve_arena_contact(&fight->robot[1], 1, out, out_size);
    }

    if (fight->clinch && surface_gap(fight) > CLINCH_BREAK_GAP_M) {
        fight->clinch = 0;
        append_text(out, out_size, "clinch breaks by physical separation. ");
    }

    for (i = 0; i < 2; i++) {
        double nx;
        double ny;
        vector_to_opponent(fight, i, &nx, &ny);
        fight->robot[i].facing = atan2(ny, nx);
        if (fight->clinch) {
            fight->robot[i].vx *= 0.45;
            fight->robot[i].vy *= 0.45;
        } else {
            fight->robot[i].vx *= FRICTION;
            fight->robot[i].vy *= FRICTION;
        }
        if (fabs(fight->robot[i].vx) < 0.002) {
            fight->robot[i].vx = 0.0;
        }
        if (fabs(fight->robot[i].vy) < 0.002) {
            fight->robot[i].vy = 0.0;
        }
    }
}

static void apply_damage(Fight *fight, int defender_side, BodyPart target,
                         int raw_damage, int guarded, char *out,
                         size_t out_size)
{
    RobotState *defender = &fight->robot[defender_side];
    int armor = part_armor[target];
    int net_damage = raw_damage - armor / 3;

    if (guarded) {
        BodyPart shield = guard_arm(defender);
        net_damage = raw_damage * 55 / 100 - armor / 3;
        if (shield != PART_INVALID && (target == PART_HEAD || target == PART_TORSO)) {
            int shield_damage = clamp_int(raw_damage / 5, 1, 12);
            defender->integrity[shield] -= shield_damage;
            append_text(out, out_size, " guard shunts %d into %s;",
                        shield_damage, part_name(shield));
            detach_if_needed(defender, shield, out, out_size);
        }
    }

    net_damage = clamp_int(net_damage, guarded ? 0 : 1, 80);
    defender->integrity[target] -= net_damage;

    if (target == PART_HEAD) {
        int processor_loss = net_damage * 3 / 4;
        if (raw_damage >= 30) {
            processor_loss += 4;
        }
        defender->processor -= processor_loss;
        defender->shock += net_damage * 2 + raw_damage / 5;
    } else if (target == PART_TORSO) {
        defender->shock += net_damage / 2 + raw_damage / 8;
        defender->stability -= net_damage / 4;
    } else if (target == PART_L_LEG || target == PART_R_LEG) {
        defender->shock += net_damage / 3;
        defender->stability -= net_damage / 2 + 5;
    } else {
        defender->shock += net_damage / 4;
        defender->stability -= net_damage / 6;
    }

    if (raw_damage >= 30) {
        defender->shock += 8;
        defender->stability -= 8;
    }

    defender->stability = clamp_int(defender->stability, 0, max_stability(defender));
    defender->processor = clamp_int(defender->processor, -100, 100);
    defender->shock = clamp_int(defender->shock, 0, 140);

    append_text(out, out_size, " net %d to %s", net_damage, part_name(target));
    detach_if_needed(defender, target, out, out_size);
}

static void apply_knockback(Fight *fight, int attacker_side, BodyPart target,
                            int raw_damage, const MoveSpec *spec, char *out,
                            size_t out_size)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    double nx;
    double ny;
    double dv;
    int fall_pressure;
    int threshold;

    vector_to_opponent(fight, attacker_side, &nx, &ny);
    dv = spec->strike_impulse_m * ((double)raw_damage / 20.0);

    if (target == PART_HEAD || target == PART_TORSO) {
        dv *= 1.12;
    }
    if (defender->down) {
        dv *= 0.40;
    }

    defender->vx += nx * dv;
    defender->vy += ny * dv;
    attacker->vx -= nx * dv * 0.20;
    attacker->vy -= ny * dv * 0.20;

    if (dv > 0.05) {
        append_text(out, out_size, " impulse %.2fm/t", dv);
    }

    if (defender->down) {
        return;
    }

    fall_pressure = raw_damage + (int)(spec->strike_impulse_m * 115.0) +
                    (100 - defender->stability) / 2;
    if (target == PART_HEAD || target == PART_TORSO) {
        fall_pressure += 8;
    }
    if (target == PART_L_LEG || target == PART_R_LEG) {
        fall_pressure += 14;
    }
    if (fight->clinch && spec->id == CMD_THROW) {
        fall_pressure += 38;
    }

    threshold = 92 + defender->stability / 4;
    if (defender->stability < 18 ||
        fall_pressure + rng_range(&fight->rng, 0, 38) > threshold) {
        fall_down(defender, spec->name, out, out_size);
        fight->clinch = 0;
    }
}

static void resolve_clinch(Fight *fight, int attacker_side, int defender_side,
                           const Intent *intent, char *out, size_t out_size)
{
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    const MoveSpec *spec = intent->spec;
    double gap = surface_gap(fight);
    int accuracy;
    int roll;

    if (gap > spec->max_gap_m) {
        append_text(out, out_size, "R%d CLINCH fails at %.2fm gap. ",
                    attacker_side + 1, gap);
        return;
    }

    accuracy = spec->accuracy + (attacker->stability - 75) / 3 -
               attacker->heat / 18;
    if (defender->retreating) {
        accuracy -= 12;
    }
    if (defender->down) {
        accuracy -= 20;
    }
    accuracy = clamp_int(accuracy, 8, 94);
    roll = rng_range(&fight->rng, 1, 100);

    if (roll <= accuracy) {
        int crush = rng_range(&fight->rng, 2, 6);
        fight->clinch = 1;
        attacker->vx *= 0.25;
        attacker->vy *= 0.25;
        defender->vx *= 0.25;
        defender->vy *= 0.25;
        defender->stability -= 8 + crush;
        defender->shock += crush;
        defender->stability = clamp_int(defender->stability, 0,
                                        max_stability(defender));
        append_text(out, out_size,
                    "R%d CLINCH attaches at %.2fm gap (roll %d/%d), crush %d. ",
                    attacker_side + 1, gap, roll, accuracy, crush);
    } else {
        append_text(out, out_size,
                    "R%d CLINCH misses latch timing (roll %d/%d). ",
                    attacker_side + 1, roll, accuracy);
    }
}

static void resolve_attack(Fight *fight, int attacker_side, const Intent *intent,
                           char *out, size_t out_size)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    const MoveSpec *spec = intent->spec;
    BodyPart target;
    double gap = surface_gap(fight);
    double center = center_distance(fight);
    double reach_penalty;
    int accuracy;
    int roll;
    int raw_damage;
    int guarded;

    if (attacker->defeated || defender->defeated) {
        return;
    }

    if (attacker->down) {
        append_text(out, out_size, "R%d %s cannot fire while down. ",
                    attacker_side + 1, spec->name);
        return;
    }

    attacker->heat = clamp_int(attacker->heat + spec->heat_cost, 0, 170);
    attacker->stability = clamp_int(attacker->stability - spec->stability_cost,
                                    0, max_stability(attacker));
    attacker->vx += cos(attacker->facing) * spec->move_impulse_m;
    attacker->vy += sin(attacker->facing) * spec->move_impulse_m;

    if (intent->command.id == CMD_CLINCH) {
        resolve_clinch(fight, attacker_side, defender_side, intent, out, out_size);
        evaluate_defeat(defender);
        return;
    }

    if (intent->command.id == CMD_THROW && !fight->clinch) {
        append_text(out, out_size, "R%d THROW cannot engage without clinch. ",
                    attacker_side + 1);
        return;
    }

    if (gap < spec->min_gap_m || gap > spec->max_gap_m) {
        append_text(out, out_size,
                    "R%d %s misses by geometry (gap %.2fm, reach %.2f-%.2fm). ",
                    attacker_side + 1, spec->name, gap, spec->min_gap_m,
                    spec->max_gap_m);
        return;
    }

    target = select_target(defender, intent->command, spec);
    reach_penalty = fabs(gap - (spec->min_gap_m + spec->max_gap_m) * 0.5) * 24.0;
    accuracy = spec->accuracy + (attacker->stability - 75) / 3 -
               attacker->heat / 20 - (int)reach_penalty;
    if (defender->retreating) {
        accuracy -= 16;
    }
    if (defender->advancing) {
        accuracy += 4;
    }
    if (defender->circling) {
        accuracy -= 8;
    }
    if (defender->guard && !defender->down) {
        accuracy -= (target == PART_HEAD || target == PART_TORSO) ? 16 : 8;
    }
    if (defender->down) {
        accuracy += intent->command.id == CMD_STOMP ? 24 : 10;
    }
    if (attacker->shock > 60) {
        accuracy -= 8;
    }
    if (wall_gap(defender) < 0.20 && (target == PART_HEAD || target == PART_TORSO)) {
        accuracy += 5;
    }
    accuracy = clamp_int(accuracy, 5, 95);

    roll = rng_range(&fight->rng, 1, 100);
    if (roll > accuracy) {
        append_text(out, out_size,
                    "R%d %s to %s misses (roll %d/%d, gap %.2fm, center %.2fm). ",
                    attacker_side + 1, spec->name, part_name(target), roll,
                    accuracy, gap, center);
        return;
    }

    raw_damage = spec->damage + rng_range(&fight->rng, -3, 6) +
                 (attacker->stability - 80) / 12 - attacker->heat / 35;
    if (defender->down && intent->command.id == CMD_STOMP) {
        raw_damage += 4;
    }
    raw_damage = clamp_int(raw_damage, 1, 80);
    guarded = defender->guard && !defender->down &&
              (target == PART_HEAD || target == PART_TORSO);

    append_text(out, out_size,
                "R%d %s hits %s (roll %d/%d, gap %.2fm), raw %d;",
                attacker_side + 1, spec->name, part_name(target), roll,
                accuracy, gap, raw_damage);
    apply_damage(fight, defender_side, target, raw_damage, guarded, out,
                 out_size);
    apply_knockback(fight, attacker_side, target, raw_damage, spec, out,
                    out_size);
    append_text(out, out_size, ". ");

    if (intent->command.id == CMD_THROW && fight->clinch) {
        fight->clinch = 0;
        fall_down(defender, "THROW release", out, out_size);
        append_text(out, out_size, " Throw releases clinch. ");
    }

    evaluate_defeat(defender);
}

#ifndef CFA_NO_CLI_MAIN
static void print_status(const char *label, const RobotState *robot)
{
    printf("%s pos(%+.2f,%+.2f) v(%+.2f,%+.2f) H%3d P%3d S%3d T%3d LA%3d RA%3d LL%3d RL%3d heat%3d shock%3d wall%.2f",
           label,
           robot->x,
           robot->y,
           robot->vx,
           robot->vy,
           robot->integrity[PART_HEAD],
           robot->processor,
           robot->stability,
           robot->integrity[PART_TORSO],
           robot->integrity[PART_L_ARM],
           robot->integrity[PART_R_ARM],
           robot->integrity[PART_L_LEG],
           robot->integrity[PART_R_LEG],
           robot->heat,
           robot->shock,
           wall_gap(robot));

    if (robot->down) {
        printf(" DOWN");
    }
    if (robot->detached[PART_L_ARM]) {
        printf(" -LA");
    }
    if (robot->detached[PART_R_ARM]) {
        printf(" -RA");
    }
    if (robot->detached[PART_L_LEG]) {
        printf(" -LL");
    }
    if (robot->detached[PART_R_LEG]) {
        printf(" -RL");
    }
    printf("\n");
}

static void print_final_status(const Program *left, const Program *right,
                               const Fight *fight)
{
    printf("\nFinal telemetry\n");
    print_status(left->name, &fight->robot[0]);
    print_status(right->name, &fight->robot[1]);
}

static BoutResult run_bout(const Program *left, const Program *right,
                           uint32_t seed, int verbose)
{
    Fight fight;
    BoutResult result;
    size_t cursor[2] = {0, 0};
    int turn;

    memset(&result, 0, sizeof(result));
    result.winner = -1;
    init_fight(&fight, seed);

    if (verbose) {
        printf("Cage Fighting Robot OS physical simulation\n");
        printf("Seed %u | R1 %s | R2 %s\n", seed, left->name, right->name);
        printf("Arena radius %.2fm | robot hard radius %.2fm | no-overlap contact solver enabled\n\n",
               ARENA_RADIUS_M, ROBOT_RADIUS_M);
    }

    for (turn = 1; turn <= MAX_TURNS; turn++) {
        Intent intent[2];
        char event[1536];
        int attack_order[2] = {0, 1};
        int score_a;
        int score_b;

        event[0] = '\0';

        recover_robot(&fight.robot[0]);
        recover_robot(&fight.robot[1]);

        intent[0] = build_intent(left, &fight, 0, &cursor[0], event, sizeof(event));
        intent[1] = build_intent(right, &fight, 1, &cursor[1], event, sizeof(event));

        if (intent[0].note[0] != '\0') {
            append_text(event, sizeof(event), "R1 %s. ", intent[0].note);
        }
        if (intent[1].note[0] != '\0') {
            append_text(event, sizeof(event), "R2 %s. ", intent[1].note);
        }

        apply_motion_command(&fight, 0, &intent[0], event, sizeof(event));
        apply_motion_command(&fight, 1, &intent[1], event, sizeof(event));
        physics_step(&fight, event, sizeof(event));

        if (intent[0].active && intent[1].active) {
            score_a = intent[0].spec->speed + fight.robot[0].stability / 20 -
                      fight.robot[0].heat / 30 + rng_range(&fight.rng, 0, 3);
            score_b = intent[1].spec->speed + fight.robot[1].stability / 20 -
                      fight.robot[1].heat / 30 + rng_range(&fight.rng, 0, 3);
            if (score_b > score_a) {
                attack_order[0] = 1;
                attack_order[1] = 0;
            }
        }

        if (intent[attack_order[0]].active) {
            resolve_attack(&fight, attack_order[0], &intent[attack_order[0]],
                           event, sizeof(event));
        }
        if (intent[attack_order[1]].active) {
            resolve_attack(&fight, attack_order[1], &intent[attack_order[1]],
                           event, sizeof(event));
        }
        physics_step(&fight, event, sizeof(event));

        if (verbose) {
            printf("T%03d gap%.2f center%.2f C%d | R1 %-10s R2 %-10s | %s\n",
                   turn, surface_gap(&fight), center_distance(&fight),
                   fight.clinch, intent[0].spec->name, intent[1].spec->name,
                   event);
            if (turn == 1 || turn % 8 == 0 ||
                fight.robot[0].defeated || fight.robot[1].defeated) {
                print_status("  R1", &fight.robot[0]);
                print_status("  R2", &fight.robot[1]);
            }
        }

        if (fight.robot[0].defeated || fight.robot[1].defeated) {
            result.turns = turn;
            if (fight.robot[0].defeated && fight.robot[1].defeated) {
                result.winner = -1;
                copy_text(result.method, sizeof(result.method),
                          "double stoppage");
            } else if (fight.robot[0].defeated) {
                result.winner = 1;
                copy_text(result.method, sizeof(result.method),
                          fight.robot[0].method);
            } else {
                result.winner = 0;
                copy_text(result.method, sizeof(result.method),
                          fight.robot[1].method);
            }
            break;
        }
    }

    if (result.turns == 0) {
        result.turns = MAX_TURNS;
        result.score[0] = structural_score(&fight.robot[0]);
        result.score[1] = structural_score(&fight.robot[1]);
        if (result.score[0] > result.score[1]) {
            result.winner = 0;
            copy_text(result.method, sizeof(result.method),
                      "decision by remaining structure and posture");
        } else if (result.score[1] > result.score[0]) {
            result.winner = 1;
            copy_text(result.method, sizeof(result.method),
                      "decision by remaining structure and posture");
        } else {
            result.winner = -1;
            copy_text(result.method, sizeof(result.method), "draw by equal score");
        }
    } else {
        result.score[0] = structural_score(&fight.robot[0]);
        result.score[1] = structural_score(&fight.robot[1]);
    }

    if (verbose) {
        const char *winner_name = "DRAW";
        if (result.winner == 0) {
            winner_name = left->name;
        } else if (result.winner == 1) {
            winner_name = right->name;
        }
        print_final_status(left, right, &fight);
        printf("\nResult: %s at T%03d by %s\n",
               winner_name, result.turns, result.method);
        printf("Scores: R1 %d | R2 %d\n", result.score[0], result.score[1]);
    }

    return result;
}
#endif

struct CFABout {
    Program left;
    Program right;
    Fight fight;
    uint32_t seed;
    int turn;
    int finished;
    int winner;
    int score[2];
    size_t cursor[2];
    char result_method[CFA_MAX_METHOD];
    Intent last_intent[2];
    char last_event[CFA_MAX_EVENT];
};

static int distance_band(const Fight *fight)
{
    double gap = surface_gap(fight);

    if (fight->clinch || gap < 0.10) {
        return 0;
    }
    if (gap < 0.80) {
        return 1;
    }
    return 2;
}

static void copy_robot_snapshot(CFARobotSnapshot *snapshot,
                                const RobotState *robot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->head = robot->integrity[PART_HEAD];
    snapshot->torso = robot->integrity[PART_TORSO];
    snapshot->leftArm = robot->integrity[PART_L_ARM];
    snapshot->rightArm = robot->integrity[PART_R_ARM];
    snapshot->leftLeg = robot->integrity[PART_L_LEG];
    snapshot->rightLeg = robot->integrity[PART_R_LEG];
    snapshot->processor = robot->processor;
    snapshot->shock = robot->shock;
    snapshot->stability = robot->stability;
    snapshot->heat = robot->heat;
    snapshot->guarding = robot->guard;
    snapshot->retreating = robot->retreating;
    snapshot->advancing = robot->advancing;
    snapshot->circling = robot->circling;
    snapshot->down = robot->down;
    snapshot->downTicks = robot->down_ticks;
    snapshot->defeated = robot->defeated;
    snapshot->leftArmDetached = robot->detached[PART_L_ARM];
    snapshot->rightArmDetached = robot->detached[PART_R_ARM];
    snapshot->leftLegDetached = robot->detached[PART_L_LEG];
    snapshot->rightLegDetached = robot->detached[PART_R_LEG];
    snapshot->x = robot->x;
    snapshot->y = robot->y;
    snapshot->vx = robot->vx;
    snapshot->vy = robot->vy;
    snapshot->facing = robot->facing;
    snapshot->wallGap = wall_gap(robot);
    copy_text(snapshot->method, sizeof(snapshot->method), robot->method);
}

static void finish_bout(CFABout *bout)
{
    bout->finished = 1;
    bout->score[0] = structural_score(&bout->fight.robot[0]);
    bout->score[1] = structural_score(&bout->fight.robot[1]);

    if (bout->fight.robot[0].defeated || bout->fight.robot[1].defeated) {
        if (bout->fight.robot[0].defeated && bout->fight.robot[1].defeated) {
            bout->winner = -1;
            copy_text(bout->result_method, sizeof(bout->result_method),
                      "double stoppage");
        } else if (bout->fight.robot[0].defeated) {
            bout->winner = 1;
            copy_text(bout->result_method, sizeof(bout->result_method),
                      bout->fight.robot[0].method);
        } else {
            bout->winner = 0;
            copy_text(bout->result_method, sizeof(bout->result_method),
                      bout->fight.robot[1].method);
        }
        return;
    }

    if (bout->score[0] > bout->score[1]) {
        bout->winner = 0;
        copy_text(bout->result_method, sizeof(bout->result_method),
                  "decision by remaining structure and posture");
    } else if (bout->score[1] > bout->score[0]) {
        bout->winner = 1;
        copy_text(bout->result_method, sizeof(bout->result_method),
                  "decision by remaining structure and posture");
    } else {
        bout->winner = -1;
        copy_text(bout->result_method, sizeof(bout->result_method),
                  "draw by equal score");
    }
}

static void fill_snapshot(const CFABout *bout, CFATurnSnapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->turn = bout->turn;
    snapshot->distance = distance_band(&bout->fight);
    snapshot->gap = surface_gap(&bout->fight);
    snapshot->centerDistance = center_distance(&bout->fight);
    snapshot->arenaRadius = ARENA_RADIUS_M;
    snapshot->robotRadius = ROBOT_RADIUS_M;
    snapshot->clinch = bout->fight.clinch;
    snapshot->finished = bout->finished;
    snapshot->winner = bout->winner;
    snapshot->leftScore = bout->score[0];
    snapshot->rightScore = bout->score[1];
    snapshot->leftTarget = PART_INVALID;
    snapshot->rightTarget = PART_INVALID;

    copy_text(snapshot->leftName, sizeof(snapshot->leftName), bout->left.name);
    copy_text(snapshot->rightName, sizeof(snapshot->rightName), bout->right.name);
    copy_text(snapshot->event, sizeof(snapshot->event), bout->last_event);
    copy_text(snapshot->resultMethod, sizeof(snapshot->resultMethod),
              bout->result_method);

    if (bout->last_intent[0].spec != NULL) {
        snapshot->leftCommandId = bout->last_intent[0].command.id;
        snapshot->leftTarget = bout->last_intent[0].command.target;
        copy_text(snapshot->leftCommand, sizeof(snapshot->leftCommand),
                  bout->last_intent[0].spec->name);
    } else {
        snapshot->leftCommandId = CMD_INVALID;
        snapshot->leftCommand[0] = '\0';
    }

    if (bout->last_intent[1].spec != NULL) {
        snapshot->rightCommandId = bout->last_intent[1].command.id;
        snapshot->rightTarget = bout->last_intent[1].command.target;
        copy_text(snapshot->rightCommand, sizeof(snapshot->rightCommand),
                  bout->last_intent[1].spec->name);
    } else {
        snapshot->rightCommandId = CMD_INVALID;
        snapshot->rightCommand[0] = '\0';
    }

    copy_robot_snapshot(&snapshot->leftRobot, &bout->fight.robot[0]);
    copy_robot_snapshot(&snapshot->rightRobot, &bout->fight.robot[1]);
}

CFABout *cfa_bout_create_from_files(const char *left_path,
                                    const char *right_path,
                                    uint32_t seed,
                                    char *error,
                                    size_t error_size)
{
    CFABout *bout = (CFABout *)calloc(1, sizeof(*bout));

    if (bout == NULL) {
        snprintf(error, error_size, "unable to allocate CFA bout");
        return NULL;
    }

    if (!load_program(left_path, &bout->left, error, error_size)) {
        free(bout);
        return NULL;
    }
    if (!load_program(right_path, &bout->right, error, error_size)) {
        free(bout);
        return NULL;
    }

    cfa_bout_restart(bout, seed);
    return bout;
}

void cfa_bout_destroy(CFABout *bout)
{
    free(bout);
}

void cfa_bout_restart(CFABout *bout, uint32_t seed)
{
    if (bout == NULL) {
        return;
    }

    bout->seed = seed == 0 ? 1u : seed;
    init_fight(&bout->fight, bout->seed);
    bout->turn = 0;
    bout->finished = 0;
    bout->winner = -2;
    bout->score[0] = structural_score(&bout->fight.robot[0]);
    bout->score[1] = structural_score(&bout->fight.robot[1]);
    bout->cursor[0] = 0;
    bout->cursor[1] = 0;
    bout->result_method[0] = '\0';
    memset(bout->last_intent, 0, sizeof(bout->last_intent));
    snprintf(bout->last_event, sizeof(bout->last_event),
             "Bout loaded: %s versus %s.", bout->left.name, bout->right.name);
}

int cfa_bout_step(CFABout *bout, CFATurnSnapshot *snapshot)
{
    Intent intent[2];
    char event[CFA_MAX_EVENT];
    int attack_order[2] = {0, 1};
    int score_a;
    int score_b;

    if (bout == NULL || snapshot == NULL) {
        return 0;
    }

    if (bout->finished) {
        fill_snapshot(bout, snapshot);
        return 0;
    }

    event[0] = '\0';
    bout->turn++;

    recover_robot(&bout->fight.robot[0]);
    recover_robot(&bout->fight.robot[1]);

    intent[0] = build_intent(&bout->left, &bout->fight, 0,
                             &bout->cursor[0], event, sizeof(event));
    intent[1] = build_intent(&bout->right, &bout->fight, 1,
                             &bout->cursor[1], event, sizeof(event));

    if (intent[0].note[0] != '\0') {
        append_text(event, sizeof(event), "R1 %s. ", intent[0].note);
    }
    if (intent[1].note[0] != '\0') {
        append_text(event, sizeof(event), "R2 %s. ", intent[1].note);
    }

    apply_motion_command(&bout->fight, 0, &intent[0], event, sizeof(event));
    apply_motion_command(&bout->fight, 1, &intent[1], event, sizeof(event));
    physics_step(&bout->fight, event, sizeof(event));

    if (intent[0].active && intent[1].active) {
        score_a = intent[0].spec->speed + bout->fight.robot[0].stability / 20 -
                  bout->fight.robot[0].heat / 30 +
                  rng_range(&bout->fight.rng, 0, 3);
        score_b = intent[1].spec->speed + bout->fight.robot[1].stability / 20 -
                  bout->fight.robot[1].heat / 30 +
                  rng_range(&bout->fight.rng, 0, 3);
        if (score_b > score_a) {
            attack_order[0] = 1;
            attack_order[1] = 0;
        }
    }

    if (intent[attack_order[0]].active) {
        resolve_attack(&bout->fight, attack_order[0],
                       &intent[attack_order[0]], event, sizeof(event));
    }
    if (intent[attack_order[1]].active) {
        resolve_attack(&bout->fight, attack_order[1],
                       &intent[attack_order[1]], event, sizeof(event));
    }
    physics_step(&bout->fight, event, sizeof(event));

    bout->last_intent[0] = intent[0];
    bout->last_intent[1] = intent[1];
    if (event[0] == '\0') {
        snprintf(event, sizeof(event), "Bodies coast under friction.");
    }
    copy_text(bout->last_event, sizeof(bout->last_event), event);
    bout->score[0] = structural_score(&bout->fight.robot[0]);
    bout->score[1] = structural_score(&bout->fight.robot[1]);

    if (bout->fight.robot[0].defeated || bout->fight.robot[1].defeated ||
        bout->turn >= MAX_TURNS) {
        finish_bout(bout);
    }

    fill_snapshot(bout, snapshot);
    return 1;
}

void cfa_bout_get_snapshot(const CFABout *bout, CFATurnSnapshot *snapshot)
{
    if (bout == NULL || snapshot == NULL) {
        return;
    }
    fill_snapshot(bout, snapshot);
}

int cfa_bout_is_finished(const CFABout *bout)
{
    return bout == NULL ? 1 : bout->finished;
}

const char *cfa_part_name(int part)
{
    return part_name((BodyPart)part);
}

int cfa_part_initial(int part)
{
    if (part < 0 || part >= PART_COUNT) {
        return 0;
    }
    return part_initial[part];
}

int cfa_part_armor(int part)
{
    if (part < 0 || part >= PART_COUNT) {
        return 0;
    }
    return part_armor[part];
}

const char *cfa_command_name(int command_id)
{
    if (command_id < 0 || command_id >= CMD_COUNT) {
        return "INVALID";
    }
    return move_specs[command_id].name;
}

#ifndef CFA_NO_CLI_MAIN
static void print_usage(const char *argv0)
{
    printf("Usage:\n");
    printf("  %s left.cfos right.cfos [seed]\n", argv0);
    printf("  %s --tournament seed command_set_a.cfos command_set_b.cfos [...]\n",
           argv0);
    printf("  %s --list-moves\n", argv0);
}

static void print_moves(void)
{
    int i;

    printf("Movement command table\n");
    printf("%-10s %-5s %-5s %-5s %-11s %-8s %-8s %s\n",
           "COMMAND", "DMG", "ACC", "SPD", "GAP_M", "MOVE", "IMPULSE",
           "DESCRIPTION");
    for (i = 0; i < CMD_COUNT; i++) {
        const MoveSpec *spec = &move_specs[i];
        char gap[24];
        if (spec->is_attack) {
            snprintf(gap, sizeof(gap), "%.2f-%.2f", spec->min_gap_m,
                     spec->max_gap_m);
        } else {
            snprintf(gap, sizeof(gap), "-");
        }
        printf("%-10s %-5d %-5d %-5d %-11s %-8.2f %-8.2f %s\n",
               spec->name, spec->damage, spec->accuracy, spec->speed, gap,
               spec->move_impulse_m, spec->strike_impulse_m,
               spec->description);
    }
}

static int run_tournament(int argc, char **argv)
{
    Program programs[MAX_PROGRAMS];
    int wins[MAX_PROGRAMS] = {0};
    int losses[MAX_PROGRAMS] = {0};
    int draws[MAX_PROGRAMS] = {0};
    int stoppages[MAX_PROGRAMS] = {0};
    int total_turns[MAX_PROGRAMS] = {0};
    int bouts[MAX_PROGRAMS] = {0};
    int count = argc - 3;
    uint32_t seed;
    int i;
    int j;
    int k;

    if (count < 2) {
        fprintf(stderr, "tournament requires at least two command sets\n");
        return 1;
    }
    if (count > MAX_PROGRAMS) {
        fprintf(stderr, "maximum tournament command sets: %d\n", MAX_PROGRAMS);
        return 1;
    }

    seed = (uint32_t)strtoul(argv[2], NULL, 10);
    if (seed == 0) {
        seed = 1;
    }

    for (i = 0; i < count; i++) {
        char error[256];
        if (!load_program(argv[i + 3], &programs[i], error, sizeof(error))) {
            fprintf(stderr, "%s\n", error);
            return 1;
        }
    }

    printf("Tournament seed %u, %d command sets, %d mirrored bouts per pairing\n",
           seed, count, TOURNAMENT_BOUTS * 2);

    for (i = 0; i < count; i++) {
        for (j = i + 1; j < count; j++) {
            for (k = 0; k < TOURNAMENT_BOUTS; k++) {
                BoutResult result_a;
                BoutResult result_b;
                uint32_t bout_seed = seed + (uint32_t)(i * 4099 + j * 131 + k * 17);

                result_a = run_bout(&programs[i], &programs[j], bout_seed, 0);
                result_b = run_bout(&programs[j], &programs[i], bout_seed + 9001u, 0);

                bouts[i]++;
                bouts[j]++;
                total_turns[i] += result_a.turns;
                total_turns[j] += result_a.turns;
                if (result_a.winner == 0) {
                    wins[i]++;
                    losses[j]++;
                    stoppages[i] += strstr(result_a.method, "decision") == NULL;
                } else if (result_a.winner == 1) {
                    wins[j]++;
                    losses[i]++;
                    stoppages[j] += strstr(result_a.method, "decision") == NULL;
                } else {
                    draws[i]++;
                    draws[j]++;
                }

                bouts[i]++;
                bouts[j]++;
                total_turns[i] += result_b.turns;
                total_turns[j] += result_b.turns;
                if (result_b.winner == 0) {
                    wins[j]++;
                    losses[i]++;
                    stoppages[j] += strstr(result_b.method, "decision") == NULL;
                } else if (result_b.winner == 1) {
                    wins[i]++;
                    losses[j]++;
                    stoppages[i] += strstr(result_b.method, "decision") == NULL;
                } else {
                    draws[i]++;
                    draws[j]++;
                }
            }
        }
    }

    printf("\n%-22s %5s %5s %5s %8s %8s\n",
           "program", "wins", "loss", "draw", "stop", "avg_t");
    for (i = 0; i < count; i++) {
        int avg_turns = bouts[i] == 0 ? 0 : total_turns[i] / bouts[i];
        printf("%-22s %5d %5d %5d %8d %8d\n",
               programs[i].name, wins[i], losses[i], draws[i], stoppages[i],
               avg_turns);
    }

    return 0;
}

int main(int argc, char **argv)
{
    Program left;
    Program right;
    char error[256];
    uint32_t seed = 42;

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (equals_ci(argv[1], "--list-moves")) {
        print_moves();
        return 0;
    }

    if (equals_ci(argv[1], "--tournament")) {
        return run_tournament(argc, argv);
    }

    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    if (argc >= 4) {
        seed = (uint32_t)strtoul(argv[3], NULL, 10);
        if (seed == 0) {
            seed = 1;
        }
    }

    if (!load_program(argv[1], &left, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    if (!load_program(argv[2], &right, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    (void)run_bout(&left, &right, seed, 1);
    return 0;
}
#endif
