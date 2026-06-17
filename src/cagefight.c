#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cagefight.h"

#define MAX_COMMANDS 256
#define MAX_NAME 80
#define MAX_LINE 256
#define MAX_PROGRAMS 32
#define MAX_TURNS 160
#define TOURNAMENT_BOUTS 3

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
    CMD_RESET,
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

typedef struct {
    CommandId id;
    BodyPart target;
    int line;
} Command;

typedef struct {
    char name[MAX_NAME];
    Command commands[MAX_COMMANDS];
    size_t count;
} Program;

typedef struct {
    CommandId id;
    const char *name;
    int is_attack;
    int damage;
    int accuracy;
    int speed;
    int min_range;
    int max_range;
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
    int defeated;
    char method[96];
} RobotState;

typedef struct {
    RobotState robot[2];
    int distance;
    int clinch;
    uint32_t rng;
} Fight;

typedef struct {
    Command command;
    const MoveSpec *spec;
    int active;
    char note[128];
} Intent;

typedef struct {
    int winner;
    int turns;
    char method[96];
    int score[2];
} BoutResult;

struct CFABout {
    Program left;
    Program right;
    Fight fight;
    uint32_t seed;
    int turn;
    int finished;
    int winner;
    int score[2];
    char result_method[96];
    Intent last_intent[2];
    char last_event[1024];
};

static const int part_initial[PART_COUNT] = {
    100, 160, 90, 90, 110, 110
};

static const int part_armor[PART_COUNT] = {
    12, 18, 9, 9, 11, 11
};

static const MoveSpec move_specs[CMD_COUNT] = {
    { CMD_GUARD, "GUARD", 0, 0, 0, 7, 0, 2, PART_INVALID, USE_NONE, -2, -4,
      "Raise forearms and brace head/torso. Reduces incoming accuracy and damage." },
    { CMD_ADVANCE, "ADVANCE", 0, 0, 0, 8, 0, 2, PART_INVALID, USE_BOTH_LEGS, 1, 1,
      "Close one range band toward the opponent." },
    { CMD_RETREAT, "RETREAT", 0, 0, 0, 8, 0, 2, PART_INVALID, USE_BOTH_LEGS, 1, 1,
      "Open one range band and increase evasion for the current tick." },
    { CMD_RESET, "RESET", 0, 0, 0, 6, 0, 2, PART_INVALID, USE_ANY_LEG, 4, 2,
      "Break contact, exit clinch state, and return to outside range." },
    { CMD_L_JAB, "L_JAB", 1, 10, 82, 10, 1, 2, PART_HEAD, USE_L_ARM, 3, 3,
      "Fast left linear strike used for range finding and head processor shock." },
    { CMD_R_CROSS, "R_CROSS", 1, 18, 72, 8, 1, 2, PART_HEAD, USE_R_ARM, 5, 5,
      "Rear straight punch with higher head and torso transfer." },
    { CMD_L_HOOK, "L_HOOK", 1, 21, 66, 7, 0, 1, PART_HEAD, USE_L_ARM, 7, 6,
      "Short arc strike, effective in pocket range and clinch breaks." },
    { CMD_R_HOOK, "R_HOOK", 1, 23, 64, 7, 0, 1, PART_HEAD, USE_R_ARM, 7, 7,
      "Heavy right arc strike with elevated knockout probability." },
    { CMD_UPPERCUT, "UPPERCUT", 1, 24, 61, 6, 0, 1, PART_HEAD, USE_ANY_ARM, 9, 8,
      "Vertical close-range strike against the head assembly." },
    { CMD_LOW_KICK, "LOW_KICK", 1, 20, 70, 7, 1, 2, PART_L_LEG, USE_ANY_LEG, 8, 7,
      "Low-line strike against knee, hip, and ankle load paths." },
    { CMD_HIGH_KICK, "HIGH_KICK", 1, 31, 48, 5, 1, 2, PART_HEAD, USE_R_LEG, 16, 12,
      "High-energy head strike. Powerful but thermally and balance expensive." },
    { CMD_KNEE, "KNEE", 1, 26, 63, 6, 0, 1, PART_TORSO, USE_ANY_LEG, 10, 8,
      "Short piston strike to the torso or head from close range." },
    { CMD_ELBOW, "ELBOW", 1, 25, 67, 7, 0, 1, PART_HEAD, USE_ANY_ARM, 7, 6,
      "Compact armored impact using the forearm hinge and elbow hardpoint." },
    { CMD_CLINCH, "CLINCH", 1, 4, 74, 8, 0, 1, PART_TORSO, USE_BOTH_ARMS, 6, 5,
      "Attach upper limbs to the opponent frame and force distance zero." },
    { CMD_THROW, "THROW", 1, 34, 52, 4, 0, 0, PART_TORSO, USE_BOTH_ARMS, 18, 14,
      "Clinch-only rotational takedown. Can damage torso and legs." },
    { CMD_STOMP, "STOMP", 1, 29, 55, 4, 0, 1, PART_R_LEG, USE_ANY_LEG, 15, 11,
      "Downward strike against exposed leg or torso structure." }
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
    if (equals_ci(token, "L_CROSS")) {
        return CMD_L_JAB;
    }
    if (equals_ci(token, "HOOK")) {
        return CMD_L_HOOK;
    }
    if (equals_ci(token, "R_UPPERCUT")) {
        return CMD_UPPERCUT;
    }
    if (equals_ci(token, "L_UPPERCUT")) {
        return CMD_UPPERCUT;
    }
    if (equals_ci(token, "KICK")) {
        return CMD_LOW_KICK;
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

static void init_robot(RobotState *robot)
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
    robot->defeated = 0;
    robot->method[0] = '\0';
}

static void init_fight(Fight *fight, uint32_t seed)
{
    init_robot(&fight->robot[0]);
    init_robot(&fight->robot[1]);
    fight->distance = 2;
    fight->clinch = 0;
    fight->rng = seed == 0 ? 1u : seed;
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
    return clamp_int(max_value, 8, 100);
}

static void recover_robot(RobotState *robot)
{
    int cap;

    robot->guard = 0;
    robot->retreating = 0;
    robot->advancing = 0;

    if (robot->heat > 0) {
        robot->heat -= 4;
        if (robot->heat < 0) {
            robot->heat = 0;
        }
    }

    if (robot->shock > 0) {
        robot->shock -= 3;
        if (robot->shock < 0) {
            robot->shock = 0;
        }
    }

    cap = max_stability(robot);
    if (robot->stability < cap) {
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

static void detach_if_needed(RobotState *robot, BodyPart part, char *out, size_t out_size)
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
    return score;
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
        char *first;
        char *second;
        CommandId command_id;
        BodyPart target = PART_INVALID;

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

        first = strtok(text, " \t\r\n");
        second = strtok(NULL, " \t\r\n");
        if (first == NULL) {
            continue;
        }

        command_id = command_from_token(first);
        if (command_id == CMD_INVALID) {
            snprintf(error, error_size, "%s:%d unknown command '%s'",
                     path, line_no, first);
            fclose(file);
            return 0;
        }

        if (second != NULL) {
            target = part_from_token(second);
            if (target == PART_INVALID) {
                snprintf(error, error_size, "%s:%d unknown target '%s'",
                         path, line_no, second);
                fclose(file);
                return 0;
            }
        }

        if (program->count >= MAX_COMMANDS) {
            snprintf(error, error_size, "%s has more than %d commands",
                     path, MAX_COMMANDS);
            fclose(file);
            return 0;
        }

        program->commands[program->count].id = command_id;
        program->commands[program->count].target = target;
        program->commands[program->count].line = line_no;
        program->count++;
    }

    fclose(file);

    if (program->count == 0) {
        snprintf(error, error_size, "%s contains no executable commands", path);
        return 0;
    }

    return 1;
}

static Intent build_intent(const Program *program, const RobotState *robot,
                           int turn)
{
    Intent intent;
    Command command;
    const MoveSpec *spec;

    memset(&intent, 0, sizeof(intent));
    command = program->commands[(size_t)(turn - 1) % program->count];
    spec = &move_specs[command.id];
    intent.command = command;
    intent.spec = spec;
    intent.active = spec->is_attack;

    if (!mode_available(robot, spec->use_mode)) {
        intent.command.id = CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_GUARD];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "%s actuator path unavailable; command converted to GUARD",
                 spec->name);
        return intent;
    }

    if (robot->heat >= 120) {
        intent.command.id = CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_GUARD];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "thermal clamp engaged; command converted to GUARD");
    }

    return intent;
}

static void apply_position_command(Fight *fight, int side, const Intent *intent,
                                   char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];

    switch (intent->command.id) {
    case CMD_GUARD:
        robot->guard = 1;
        robot->stability = clamp_int(robot->stability + 2, 0, max_stability(robot));
        append_text(out, out_size, "R%d GUARD locks head/torso shell. ",
                    side + 1);
        break;
    case CMD_ADVANCE:
        robot->advancing = 1;
        if (!fight->clinch && fight->distance > 0) {
            fight->distance--;
        }
        append_text(out, out_size, "R%d ADVANCE sets range %d. ",
                    side + 1, fight->distance);
        break;
    case CMD_RETREAT:
        robot->retreating = 1;
        if (!fight->clinch && fight->distance < 2) {
            fight->distance++;
        }
        append_text(out, out_size, "R%d RETREAT sets range %d. ",
                    side + 1, fight->distance);
        break;
    case CMD_RESET:
        fight->clinch = 0;
        fight->distance = 2;
        append_text(out, out_size, "R%d RESET breaks contact to range 2. ",
                    side + 1);
        break;
    default:
        break;
    }

    robot->heat = clamp_int(robot->heat + intent->spec->heat_cost, 0, 160);
    robot->stability = clamp_int(robot->stability - intent->spec->stability_cost,
                                 0, max_stability(robot));
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

static void resolve_clinch(Fight *fight, int attacker_side, int defender_side,
                           const Intent *intent, char *out, size_t out_size)
{
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    const MoveSpec *spec = intent->spec;
    int accuracy;
    int roll;

    if (fight->distance > spec->max_range) {
        append_text(out, out_size, "R%d CLINCH fails outside range. ",
                    attacker_side + 1);
        return;
    }

    accuracy = spec->accuracy + (attacker->stability - 75) / 3 -
               attacker->heat / 18;
    if (defender->retreating) {
        accuracy -= 12;
    }
    accuracy = clamp_int(accuracy, 8, 94);
    roll = rng_range(&fight->rng, 1, 100);

    if (roll <= accuracy) {
        int crush = rng_range(&fight->rng, 2, 6);
        fight->clinch = 1;
        fight->distance = 0;
        defender->stability -= 8 + crush;
        defender->shock += crush;
        defender->stability = clamp_int(defender->stability, 0,
                                        max_stability(defender));
        append_text(out, out_size,
                    "R%d CLINCH attaches upper frame (roll %d/%d), crush %d. ",
                    attacker_side + 1, roll, accuracy, crush);
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
    int accuracy;
    int roll;
    int raw_damage;
    int guarded;

    if (attacker->defeated || defender->defeated) {
        return;
    }

    attacker->heat = clamp_int(attacker->heat + spec->heat_cost, 0, 160);
    attacker->stability = clamp_int(attacker->stability - spec->stability_cost,
                                    0, max_stability(attacker));

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

    if (fight->distance < spec->min_range || fight->distance > spec->max_range) {
        append_text(out, out_size, "R%d %s misses at range %d. ",
                    attacker_side + 1, spec->name, fight->distance);
        return;
    }

    if (fight->clinch && spec->min_range > 0) {
        append_text(out, out_size, "R%d %s fouled by clinch geometry. ",
                    attacker_side + 1, spec->name);
        return;
    }

    target = select_target(defender, intent->command, spec);
    accuracy = spec->accuracy + (attacker->stability - 75) / 3 -
               attacker->heat / 20;
    if (defender->retreating) {
        accuracy -= 18;
    }
    if (defender->advancing) {
        accuracy += 4;
    }
    if (defender->guard) {
        accuracy -= (target == PART_HEAD || target == PART_TORSO) ? 16 : 8;
    }
    if (attacker->shock > 60) {
        accuracy -= 8;
    }
    accuracy = clamp_int(accuracy, 5, 95);

    roll = rng_range(&fight->rng, 1, 100);
    if (roll > accuracy) {
        append_text(out, out_size, "R%d %s to %s misses (roll %d/%d). ",
                    attacker_side + 1, spec->name, part_name(target), roll,
                    accuracy);
        return;
    }

    raw_damage = spec->damage + rng_range(&fight->rng, -3, 6) +
                 (attacker->stability - 80) / 12 - attacker->heat / 35;
    raw_damage = clamp_int(raw_damage, 1, 80);
    guarded = defender->guard && (target == PART_HEAD || target == PART_TORSO);

    append_text(out, out_size, "R%d %s hits %s (roll %d/%d), raw %d;",
                attacker_side + 1, spec->name, part_name(target), roll,
                accuracy, raw_damage);
    apply_damage(fight, defender_side, target, raw_damage, guarded, out,
                 out_size);
    append_text(out, out_size, ". ");

    if (intent->command.id == CMD_THROW && fight->clinch) {
        fight->clinch = 0;
        fight->distance = 1;
        append_text(out, out_size, "Throw release resets range 1. ");
    }

    evaluate_defeat(defender);
}

static void print_status(const char *label, const RobotState *robot)
{
    printf("%s H%3d P%3d S%3d T%3d LA%3d RA%3d LL%3d RL%3d heat%3d shock%3d",
           label,
           robot->integrity[PART_HEAD],
           robot->processor,
           robot->stability,
           robot->integrity[PART_TORSO],
           robot->integrity[PART_L_ARM],
           robot->integrity[PART_R_ARM],
           robot->integrity[PART_L_LEG],
           robot->integrity[PART_R_LEG],
           robot->heat,
           robot->shock);

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
    int turn;

    memset(&result, 0, sizeof(result));
    result.winner = -1;
    init_fight(&fight, seed);

    if (verbose) {
        printf("Cage Fighting Robot OS simulation\n");
        printf("Seed %u | R1 %s | R2 %s\n", seed, left->name, right->name);
        printf("Range bands: 0 clinch, 1 pocket, 2 outside\n\n");
    }

    for (turn = 1; turn <= MAX_TURNS; turn++) {
        Intent intent[2];
        char event[1024];
        int attack_order[2] = {0, 1};
        int score_a;
        int score_b;

        event[0] = '\0';

        recover_robot(&fight.robot[0]);
        recover_robot(&fight.robot[1]);

        intent[0] = build_intent(left, &fight.robot[0], turn);
        intent[1] = build_intent(right, &fight.robot[1], turn);

        if (intent[0].note[0] != '\0') {
            append_text(event, sizeof(event), "R1 %s. ", intent[0].note);
        }
        if (intent[1].note[0] != '\0') {
            append_text(event, sizeof(event), "R2 %s. ", intent[1].note);
        }

        apply_position_command(&fight, 0, &intent[0], event, sizeof(event));
        apply_position_command(&fight, 1, &intent[1], event, sizeof(event));

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

        if (verbose) {
            printf("T%03d R%d C%d | R1 %-10s R2 %-10s | %s\n",
                   turn, fight.distance, fight.clinch,
                   intent[0].spec->name, intent[1].spec->name, event);
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
                      "decision by remaining structure");
        } else if (result.score[1] > result.score[0]) {
            result.winner = 1;
            copy_text(result.method, sizeof(result.method),
                      "decision by remaining structure");
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
    snapshot->defeated = robot->defeated;
    snapshot->leftArmDetached = robot->detached[PART_L_ARM];
    snapshot->rightArmDetached = robot->detached[PART_R_ARM];
    snapshot->leftLegDetached = robot->detached[PART_L_LEG];
    snapshot->rightLegDetached = robot->detached[PART_R_LEG];
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
                  "decision by remaining structure");
    } else if (bout->score[1] > bout->score[0]) {
        bout->winner = 1;
        copy_text(bout->result_method, sizeof(bout->result_method),
                  "decision by remaining structure");
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
    snapshot->distance = bout->fight.distance;
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
    bout->result_method[0] = '\0';
    memset(bout->last_intent, 0, sizeof(bout->last_intent));
    snprintf(bout->last_event, sizeof(bout->last_event),
             "Bout loaded: %s versus %s.", bout->left.name, bout->right.name);
}

int cfa_bout_step(CFABout *bout, CFATurnSnapshot *snapshot)
{
    Intent intent[2];
    char event[1024];
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

    intent[0] = build_intent(&bout->left, &bout->fight.robot[0], bout->turn);
    intent[1] = build_intent(&bout->right, &bout->fight.robot[1], bout->turn);

    if (intent[0].note[0] != '\0') {
        append_text(event, sizeof(event), "R1 %s. ", intent[0].note);
    }
    if (intent[1].note[0] != '\0') {
        append_text(event, sizeof(event), "R2 %s. ", intent[1].note);
    }

    apply_position_command(&bout->fight, 0, &intent[0], event, sizeof(event));
    apply_position_command(&bout->fight, 1, &intent[1], event, sizeof(event));

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

    bout->last_intent[0] = intent[0];
    bout->last_intent[1] = intent[1];
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
    printf("%-10s %-6s %-5s %-5s %-5s %-9s %s\n",
           "COMMAND", "DMG", "ACC", "SPD", "RNG", "DEFAULT", "DESCRIPTION");
    for (i = 0; i < CMD_COUNT; i++) {
        const MoveSpec *spec = &move_specs[i];
        char range[16];
        if (spec->is_attack) {
            snprintf(range, sizeof(range), "%d-%d", spec->min_range,
                     spec->max_range);
        } else {
            snprintf(range, sizeof(range), "-");
        }
        printf("%-10s %-6d %-5d %-5d %-5s %-9s %s\n",
               spec->name, spec->damage, spec->accuracy, spec->speed, range,
               part_name(spec->default_target), spec->description);
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

#ifndef CFA_NO_CLI_MAIN
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
