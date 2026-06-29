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
#define MAX_TURNS CFA_MAX_TURNS
#define TOURNAMENT_BOUTS 3

#define PI 3.14159265358979323846
#define ARENA_RADIUS_M 3.00
#define ROBOT_HEIGHT_M 2.45
#define ROBOT_STL_SCALE_M_PER_UNIT 0.017080482
#define ROBOT_FULL_ARM_SPAN_M 2.054
#define ROBOT_FULL_DEPTH_M 0.434
#define ROBOT_TORSO_HEIGHT_M 0.971
#define ROBOT_TORSO_WIDTH_M 0.469
#define ROBOT_TORSO_DEPTH_M 0.406
#define ROBOT_HEAD_HEIGHT_M 0.333
#define ROBOT_HEAD_WIDTH_M 0.264
#define ROBOT_HEAD_DEPTH_M 0.351
#define ROBOT_ARM_REACH_M 0.91
#define ROBOT_UPPER_ARM_LENGTH_M 0.38
#define ROBOT_FOREARM_LENGTH_M 0.43
#define ROBOT_HAND_SEGMENT_LENGTH_M 0.10
#define ROBOT_LEG_REACH_M 1.44
#define ROBOT_UPPER_LEG_LENGTH_M 0.68
#define ROBOT_LOWER_LEG_LENGTH_M 0.76
#define ROBOT_FOOT_LENGTH_M 0.315
#define ROBOT_FOOT_SEGMENT_LENGTH_M 0.10
#define ROBOT_FOOT_WIDTH_M 0.130
#define ROBOT_FOOT_HEIGHT_M 0.216
#define ROBOT_HEAD_MASS_KG 7.0
#define ROBOT_TORSO_MASS_KG 51.0
#define ROBOT_UPPER_ARM_MASS_KG 3.7
#define ROBOT_FOREARM_MASS_KG 3.1
#define ROBOT_HAND_MASS_KG 1.7
#define ROBOT_ARM_MASS_KG \
    (ROBOT_UPPER_ARM_MASS_KG + ROBOT_FOREARM_MASS_KG + ROBOT_HAND_MASS_KG)
#define ROBOT_UPPER_LEG_MASS_KG 10.0
#define ROBOT_LOWER_LEG_MASS_KG 8.5
#define ROBOT_FOOT_MASS_KG 3.0
#define ROBOT_LEG_MASS_KG \
    (ROBOT_UPPER_LEG_MASS_KG + ROBOT_LOWER_LEG_MASS_KG + ROBOT_FOOT_MASS_KG)
#define ROBOT_PELVIS_HEIGHT_M 1.12
#define ROBOT_HIP_HEIGHT_M 1.29
#define ROBOT_WAIST_HEIGHT_M 1.46
#define ROBOT_CHEST_HEIGHT_M 1.72
#define ROBOT_SHOULDER_HEIGHT_M 1.92
#define ROBOT_NECK_HEIGHT_M 2.10
#define ROBOT_HEAD_CENTER_HEIGHT_M 2.29
#define ROBOT_KNEE_HEIGHT_M 0.72
#define ROBOT_HIP_HALF_WIDTH_M 0.144
#define ROBOT_SHOULDER_HALF_WIDTH_M 0.227
#define ROBOT_REST_HAND_HEIGHT_M 1.48
#define ROBOT_GUARD_HAND_HEIGHT_M 1.94
#define ROBOT_STRIKING_HAND_HEIGHT_M 1.90
#define ROBOT_UPPER_ARM_RADIUS_M 0.056
#define ROBOT_ELBOW_RADIUS_M 0.047
#define ROBOT_WRIST_RADIUS_M 0.036
#define ROBOT_FIST_RADIUS_M 0.044
#define ROBOT_UPPER_LEG_RADIUS_M 0.062
#define ROBOT_KNEE_RADIUS_M 0.052
#define ROBOT_ANKLE_RADIUS_M 0.038
#define ROBOT_TOE_RADIUS_M 0.024
#define ROBOT_FOOT_HALF_STANCE_M 0.125
#define ROBOT_FOOT_FORE_OFFSET_M 0.006
#define ROBOT_FOOT_DOWN_INSET_M 0.16
#define ROBOT_STEP_SUPPORT_LIMIT_M 0.54
#define ROBOT_STEP_TRIGGER_M 0.44
#define ROBOT_FOOT_SETTLE_RATE 0.18
#define ROBOT_RADIUS_M 0.36
#define ROBOT_DOWN_RADIUS_BONUS_M 0.11
#define ROBOT_CONTACT_MARGIN_M 0.04
#define ROBOT_CONTACT_SLOP_M 0.006
#define ROBOT_CONTACT_RESTITUTION 0.78
#define ROBOT_CONTACT_SOLVER_PASSES 6
#define ROBOT_MAX_CAPSULES 14
#define ROBOT_STRIKE_CONTACT_MARGIN_M 0.06
#define ROBOT_HARD_CONTACT_BIAS_M 0.018
#define ROBOT_GLANCING_COS 0.46
#define ROBOT_GUARD_HEAD_COVERAGE_M 0.11
#define ROBOT_GUARD_TORSO_COVERAGE_M 0.08
#define ROBOT_MIN_BLOCK_REACTION 0.18
#define ROBOT_MAX_BLOCK_REACTION 1.00
#define ROBOT_STRIKE_PHASE_IDLE 0
#define ROBOT_STRIKE_PHASE_WINDUP 1
#define ROBOT_STRIKE_PHASE_RELEASE 2
#define ROBOT_STRIKE_PHASE_FOLLOW 3
#define ROBOT_STRIKE_PHASE_RECOIL 4
#define ROBOT_STAGGER_NONE 0
#define ROBOT_STAGGER_FLINCH 1
#define ROBOT_STAGGER_STEP_BACK 2
#define ROBOT_STAGGER_STUMBLE 3
#define ROBOT_STAGGER_KNEEL 4
#define ROBOT_STAGGER_COLLAPSE 5
#define CFA_GROUND_HEAD (1 << 0)
#define CFA_GROUND_TORSO (1 << 1)
#define CFA_GROUND_L_HAND (1 << 2)
#define CFA_GROUND_R_HAND (1 << 3)
#define CFA_GROUND_L_KNEE (1 << 4)
#define CFA_GROUND_R_KNEE (1 << 5)
#define CFA_GROUND_L_SHIN (1 << 6)
#define CFA_GROUND_R_SHIN (1 << 7)
#define CFA_GROUND_L_FOOT (1 << 8)
#define CFA_GROUND_R_FOOT (1 << 9)
#define CFA_GETUP_NONE 0
#define CFA_GETUP_STUNNED 1
#define CFA_GETUP_ROLL_SIDE 2
#define CFA_GETUP_BRACE_HAND 3
#define CFA_GETUP_KNEE_UNDER 4
#define CFA_GETUP_PUSH_UP 5
#define CFA_GETUP_CROUCH 6
#define CFA_GETUP_STAND 7
#define CFA_GETUP_GUARD_RESET 8
#define CFA_GETUP_HAND_SUPPORT (CFA_GROUND_L_HAND | CFA_GROUND_R_HAND)
#define CFA_GETUP_KNEE_SUPPORT (CFA_GROUND_L_KNEE | CFA_GROUND_R_KNEE)
#define CFA_GETUP_FOOT_SUPPORT (CFA_GROUND_L_FOOT | CFA_GROUND_R_FOOT)
#define CFA_GETUP_LEG_SUPPORT (CFA_GETUP_KNEE_SUPPORT | CFA_GETUP_FOOT_SUPPORT)
#define ROBOT_HEAD_DETACH_TRAUMA_THRESHOLD 70
#define PHYSICS_SUBSTEPS 3
#define ROBOT_MAX_SPEED_M_PER_TURN 0.82
#define ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN 0.42
#define ROBOT_TURN_RESPONSE 0.18
#define ROBOT_ANGULAR_DAMPING 0.74
#define ROBOT_BALANCE_EDGE_MARGIN_M 0.055
#define ROBOT_BALANCE_FALL_MARGIN_M 0.18
#define ROBOT_ARM_STRIKE_GAP_M 0.60
#define ROBOT_HOOK_STRIKE_GAP_M 0.44
#define ROBOT_UPPERCUT_STRIKE_GAP_M 0.34
#define ROBOT_KICK_STRIKE_GAP_M 0.92
#define ROBOT_TORSO_TWIST_REACH_M 0.08
#define ROBOT_HIP_TWIST_REACH_M 0.12
#define ROBOT_TORSO_TWIST_FORCE_SCALE 1.08
#define ROBOT_HIP_TWIST_FORCE_SCALE 1.10
#define ROBOT_MASS_KG 118.0
#define FRICTION 0.82
#define CLINCH_BREAK_GAP_M 0.34
#define CFA_PHYSICS_STEPS_PER_TURN 2.0
#define FORCED_MOVE_APART_RELEASE_GAP_M 0.38
#define FORCED_MOVE_APART_IMPULSE_M 0.34
#define FOOT_LEFT 0
#define FOOT_RIGHT 1
#define FOOT_NONE -1

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
    METRIC_SELF_GUARD,
    METRIC_OPP_GUARD,
    METRIC_SELF_GETUP,
    METRIC_OPP_GETUP,
    METRIC_SELF_GETUP_PRESSURE,
    METRIC_OPP_GETUP_PRESSURE,
    METRIC_SELF_BALANCE_OFFSET,
    METRIC_OPP_BALANCE_OFFSET,
    METRIC_SELF_WALL_PRESSURE,
    METRIC_OPP_WALL_PRESSURE,
    METRIC_SELF_CLINCH_PRESSURE,
    METRIC_OPP_CLINCH_PRESSURE,
    METRIC_SELF_RECENT_DAMAGE,
    METRIC_OPP_RECENT_DAMAGE,
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
    int scuff[PART_COUNT];
    int dent[PART_COUNT];
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
    double angular_velocity;
    double prev_x;
    double prev_y;
    double prev_facing;
    double prev_foot_x[2];
    double prev_foot_y[2];
    double center_mass_x;
    double center_mass_y;
    double center_mass_z;
    double support_center_x;
    double support_center_y;
    double balance_offset;
    int balance_supported;
    int balance_state;
    double support_radius;
    double foot_x[2];
    double foot_y[2];
    double foot_target_x[2];
    double foot_target_y[2];
    int foot_state[2];
    double foot_phase[2];
    int swing_foot;
    int pivot_foot;
    double pivot_angle;
    int step_sequence;
    int wall_braced;
    double wall_nx;
    double wall_ny;
    int last_impact_part;
    int last_impact_damage;
    int block_active;
    int block_success;
    int block_arm;
    int block_part;
    int block_amount;
    double block_reaction;
    double block_target_x;
    double block_target_y;
    double block_target_z;
    int parry_active;
    int guard_side;
    double guard_hand_height;
    double guard_elbow_angle;
    double guard_coverage_m;
    Command windup_command;
    int windup_ticks;
    int windup_total;
    double windup_progress;
    int attack_released;
    int recovery_ticks;
    int strike_phase;
    double follow_through;
    double recoil;
    int stagger_state;
    int stagger_ticks;
    int stagger_total;
    double stagger_progress;
    double stagger_dir_x;
    double stagger_dir_y;
    double fall_progress;
    double fall_dir_x;
    double fall_dir_y;
    double fall_angular_velocity;
    int fall_ticks;
    int fall_total;
    BodyPart fall_contact_part;
    int ground_contact_mask;
    BodyPart ground_impact_part;
    double ground_impact;
    double ground_slide;
    double ground_settle;
    double wall_impulse;
    double wall_flex;
    int getup_state;
    int getup_ticks;
    int getup_total;
    double getup_progress;
    int getup_blocked;
    double getup_force_x;
    double getup_force_y;
    double getup_force_z;
    int getup_support_mask;
    double getup_pressure;
    double grounded_defense;
    double grounded_roll;
    double clinch_leverage;
    double clinch_pressure;
    double throw_torque;
    BodyPart recent_damage_part;
    int recent_damage_raw;
    int recent_damage_net;
    double recent_damage_x;
    double recent_damage_y;
    double recent_damage_z;
    int recent_damage_ticks;
    int head_detached;
    double head_detach_x;
    double head_detach_y;
    double head_detach_z;
    double head_detach_vx;
    double head_detach_vy;
    double head_detach_vz;
    double head_detach_spin;
    int head_detach_ticks;
    char method[96];
} RobotState;

typedef struct {
    double energy;
    double cheer;
    double clap;
    double gasp;
    double chant;
    int chant_side;
} CrowdState;

typedef struct {
    RobotState robot[2];
    CrowdState crowd;
    int clinch;
    int stuck_ticks;
    uint32_t rng;
} Fight;

typedef struct {
    Command command;
    const MoveSpec *spec;
    int active;
    char note[160];
} Intent;

typedef struct {
    int attempted;
    int success;
    BodyPart shield;
    BodyPart target;
    int coverage;
    int accuracy_penalty;
    int damage_reduction_pct;
    int roll;
    int chance;
    double reaction;
    double target_x;
    double target_y;
    double target_z;
    int late;
    int parry;
} BlockResult;

typedef struct {
    int winner;
    int turns;
    char method[96];
    int score[2];
} BoutResult;

static void clear_windup_state(RobotState *robot);

typedef struct {
    double x;
    double y;
    double z;
} Vec3;

typedef struct {
    BodyPart part;
    Vec3 a;
    Vec3 b;
    double radius;
    double mass_kg;
} PhysicsCapsule;

typedef struct {
    PhysicsCapsule items[ROBOT_MAX_CAPSULES];
    int count;
} RobotCapsules;

typedef struct {
    int hit;
    BodyPart attacker_part;
    BodyPart defender_part;
    Vec3 normal;
    double clearance_m;
    double penetration_m;
    double attacker_mass_kg;
    double defender_mass_kg;
    Vec3 attacker_point;
    Vec3 defender_point;
    double angle_cos;
    double relative_speed_m;
    int glancing;
    int guarded;
} StrikeContact;

static const int part_initial[PART_COUNT] = {
    100, 160, 90, 90, 110, 110
};

static const int part_armor[PART_COUNT] = {
    12, 18, 9, 9, 11, 11
};

static const double part_mass_kg[PART_COUNT] = {
    ROBOT_HEAD_MASS_KG, ROBOT_TORSO_MASS_KG,
    ROBOT_ARM_MASS_KG, ROBOT_ARM_MASS_KG,
    ROBOT_LEG_MASS_KG, ROBOT_LEG_MASS_KG
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
    { CMD_L_JAB, "L_JAB", 1, 14, 82, 10, 0.15, ROBOT_ARM_STRIKE_GAP_M + ROBOT_TORSO_TWIST_REACH_M, 0.03, 0.06, PART_HEAD, USE_L_ARM, 3, 3,
      "Fast left linear strike used for range finding and processor shock." },
    { CMD_R_CROSS, "R_CROSS", 1, 28, 72, 8, 0.12, ROBOT_ARM_STRIKE_GAP_M + ROBOT_TORSO_TWIST_REACH_M, 0.04, 0.14, PART_HEAD, USE_R_ARM, 5, 5,
      "Rear straight punch with stronger momentum transfer." },
    { CMD_L_HOOK, "L_HOOK", 1, 32, 66, 7, 0.00, ROBOT_HOOK_STRIKE_GAP_M + ROBOT_TORSO_TWIST_REACH_M * 0.65, 0.03, 0.17, PART_HEAD, USE_L_ARM, 7, 6,
      "Short arc strike for pocket and collision range." },
    { CMD_R_HOOK, "R_HOOK", 1, 36, 64, 7, 0.00, ROBOT_HOOK_STRIKE_GAP_M + ROBOT_TORSO_TWIST_REACH_M * 0.65, 0.03, 0.19, PART_HEAD, USE_R_ARM, 7, 7,
      "Heavy right arc strike with elevated knockdown pressure." },
    { CMD_UPPERCUT, "UPPERCUT", 1, 38, 61, 6, 0.00, ROBOT_UPPERCUT_STRIKE_GAP_M + ROBOT_TORSO_TWIST_REACH_M * 0.50, 0.02, 0.20, PART_HEAD, USE_ANY_ARM, 9, 8,
      "Vertical close-range strike against the head module." },
    { CMD_LOW_KICK, "LOW_KICK", 1, 30, 70, 7, 0.18, ROBOT_KICK_STRIKE_GAP_M + ROBOT_HIP_TWIST_REACH_M, 0.02, 0.17, PART_L_LEG, USE_ANY_LEG, 8, 7,
      "Low-line strike against knee, hip, and ankle load paths." },
    { CMD_HIGH_KICK, "HIGH_KICK", 1, 46, 48, 5, 0.22, ROBOT_KICK_STRIKE_GAP_M + ROBOT_HIP_TWIST_REACH_M, 0.02, 0.25, PART_HEAD, USE_R_LEG, 16, 12,
      "High-energy head strike with high fall risk and thermal cost." },
    { CMD_KNEE, "KNEE", 1, 40, 63, 6, 0.00, 0.30, 0.02, 0.22, PART_TORSO, USE_ANY_LEG, 10, 8,
      "Short piston strike to torso or head from contact range." },
    { CMD_ELBOW, "ELBOW", 1, 38, 67, 7, 0.00, 0.28, 0.01, 0.20, PART_HEAD, USE_ANY_ARM, 7, 6,
      "Compact close strike using the forearm hinge and elbow hardpoint." },
    { CMD_CLINCH, "CLINCH", 1, 4, 74, 8, 0.00, 0.32, 0.03, 0.02, PART_TORSO, USE_BOTH_ARMS, 6, 5,
      "Attach both arms to the opponent frame and constrain separation." },
    { CMD_THROW, "THROW", 1, 48, 52, 4, 0.00, 0.30, 0.00, 0.44, PART_TORSO, USE_BOTH_ARMS, 18, 14,
      "Clinch-only rotational takedown with high knockback and fall pressure." },
    { CMD_STOMP, "STOMP", 1, 42, 55, 4, 0.00, 0.28, 0.00, 0.23, PART_R_LEG, USE_ANY_LEG, 15, 11,
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

static double normalize_angle(double angle)
{
    while (angle > PI) {
        angle -= PI * 2.0;
    }
    while (angle < -PI) {
        angle += PI * 2.0;
    }
    return angle;
}

static double clamp_abs(double value, double max_abs)
{
    return clamp_double(value, -max_abs, max_abs);
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

static void init_crowd(CrowdState *crowd)
{
    crowd->energy = 0.16;
    crowd->cheer = 0.0;
    crowd->clap = 0.0;
    crowd->gasp = 0.0;
    crowd->chant = 0.0;
    crowd->chant_side = -1;
}

static void decay_crowd(CrowdState *crowd)
{
    const double ambient = 0.16;

    crowd->energy = ambient + (crowd->energy - ambient) * 0.965;
    crowd->energy = clamp_double(crowd->energy, ambient, 1.0);
    crowd->cheer *= 0.72;
    crowd->clap *= 0.58;
    crowd->gasp *= 0.50;
    crowd->chant *= 0.82;
    if (crowd->cheer < 0.015) {
        crowd->cheer = 0.0;
    }
    if (crowd->clap < 0.015) {
        crowd->clap = 0.0;
    }
    if (crowd->gasp < 0.015) {
        crowd->gasp = 0.0;
    }
    if (crowd->chant < 0.05) {
        crowd->chant = 0.0;
        crowd->chant_side = -1;
    }
}

static void crowd_add(CrowdState *crowd, double energy, double cheer,
                      double clap, double gasp, double chant, int chant_side)
{
    crowd->energy = clamp_double(crowd->energy + energy, 0.0, 1.0);
    crowd->cheer = clamp_double(crowd->cheer + cheer, 0.0, 1.0);
    crowd->clap = clamp_double(crowd->clap + clap, 0.0, 1.0);
    crowd->gasp = clamp_double(crowd->gasp + gasp, 0.0, 1.0);
    if (chant_side >= 0 && chant > 0.0) {
        crowd->chant = clamp_double(crowd->chant + chant, 0.0, 1.0);
        crowd->chant_side = chant_side;
    }
}

static void crowd_react_hit(Fight *fight, int attacker_side, BodyPart target,
                            int raw_damage)
{
    double force = clamp_double((double)raw_damage / 112.0, 0.0, 1.0);
    double head_bonus = target == PART_HEAD ? 0.05 : 0.0;
    double body_bonus = target == PART_TORSO ? 0.025 : 0.0;
    double chant = raw_damage >= 28 ? 0.10 + force * 0.10 : 0.0;

    crowd_add(&fight->crowd,
              0.035 + force * 0.10 + head_bonus,
              0.035 + force * 0.12 + head_bonus,
              0.030 + force * 0.10 + body_bonus,
              raw_damage >= 42 ? 0.08 : 0.0,
              chant,
              attacker_side);
}

static void crowd_react_knockdown(Fight *fight, int acclaim_side,
                                  BodyPart contact_part, int surprise)
{
    double head_bonus = contact_part == PART_HEAD ? 0.08 : 0.0;
    double gasp = surprise ? 0.34 : head_bonus * 0.55;

    crowd_add(&fight->crowd,
              0.20 + head_bonus,
              0.24 + head_bonus,
              0.18,
              gasp,
              acclaim_side >= 0 ? 0.28 : 0.0,
              acclaim_side);
}

static void crowd_react_head_detach(Fight *fight, int attacker_side)
{
    crowd_add(&fight->crowd, 0.55, 0.42, 0.32, 1.0, 0.44,
              attacker_side);
}

static Vec3 vec3(double x, double y, double z)
{
    Vec3 value;

    value.x = x;
    value.y = y;
    value.z = z;
    return value;
}

static Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static Vec3 vec3_scale(Vec3 value, double scale)
{
    return vec3(value.x * scale, value.y * scale, value.z * scale);
}

static double vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static double vec3_length_sq(Vec3 value)
{
    return vec3_dot(value, value);
}

static double vec3_length(Vec3 value)
{
    return sqrt(vec3_length_sq(value));
}

static Vec3 vec3_normalize_or(Vec3 value, Vec3 fallback)
{
    double length = vec3_length(value);

    if (length > 0.000001) {
        return vec3_scale(value, 1.0 / length);
    }
    length = vec3_length(fallback);
    if (length > 0.000001) {
        return vec3_scale(fallback, 1.0 / length);
    }
    return vec3(1.0, 0.0, 0.0);
}

static Vec3 local_to_world_point(const RobotState *robot, Vec3 local)
{
    double fx = cos(robot->facing);
    double fy = sin(robot->facing);
    double rx = -sin(robot->facing);
    double ry = cos(robot->facing);

    return vec3(robot->x + fx * local.x + rx * local.z,
                robot->y + fy * local.x + ry * local.z,
                local.y);
}

static RobotState previous_pose_robot(const RobotState *robot)
{
    RobotState previous = *robot;

    previous.x = robot->prev_x;
    previous.y = robot->prev_y;
    previous.facing = robot->prev_facing;
    previous.foot_x[FOOT_LEFT] = robot->prev_foot_x[FOOT_LEFT];
    previous.foot_y[FOOT_LEFT] = robot->prev_foot_y[FOOT_LEFT];
    previous.foot_x[FOOT_RIGHT] = robot->prev_foot_x[FOOT_RIGHT];
    previous.foot_y[FOOT_RIGHT] = robot->prev_foot_y[FOOT_RIGHT];
    return previous;
}

static Vec3 robot_forward_vec(const RobotState *robot)
{
    return vec3(cos(robot->facing), sin(robot->facing), 0.0);
}

static Vec3 robot_side_vec(const RobotState *robot, double side)
{
    return vec3(-sin(robot->facing) * side, cos(robot->facing) * side, 0.0);
}

static void closest_points_on_segments(Vec3 p1, Vec3 q1,
                                       Vec3 p2, Vec3 q2,
                                       Vec3 *c1, Vec3 *c2)
{
    Vec3 d1 = vec3_sub(q1, p1);
    Vec3 d2 = vec3_sub(q2, p2);
    Vec3 r = vec3_sub(p1, p2);
    double a = vec3_dot(d1, d1);
    double e = vec3_dot(d2, d2);
    double f = vec3_dot(d2, r);
    double s;
    double t;
    double c;
    double b;
    double denom;

    if (a <= 0.00000001 && e <= 0.00000001) {
        *c1 = p1;
        *c2 = p2;
        return;
    }

    if (a <= 0.00000001) {
        s = 0.0;
        t = clamp_double(f / e, 0.0, 1.0);
    } else {
        c = vec3_dot(d1, r);
        if (e <= 0.00000001) {
            t = 0.0;
            s = clamp_double(-c / a, 0.0, 1.0);
        } else {
            b = vec3_dot(d1, d2);
            denom = a * e - b * b;
            if (denom != 0.0) {
                s = clamp_double((b * f - c * e) / denom, 0.0, 1.0);
            } else {
                s = 0.0;
            }
            t = (b * s + f) / e;
            if (t < 0.0) {
                t = 0.0;
                s = clamp_double(-c / a, 0.0, 1.0);
            } else if (t > 1.0) {
                t = 1.0;
                s = clamp_double((b - c) / a, 0.0, 1.0);
            }
        }
    }

    *c1 = vec3_add(p1, vec3_scale(d1, s));
    *c2 = vec3_add(p2, vec3_scale(d2, t));
}

static double capsule_clearance_details(const PhysicsCapsule *a,
                                        const PhysicsCapsule *b,
                                        Vec3 *normal,
                                        Vec3 *point_a,
                                        Vec3 *point_b)
{
    Vec3 pa;
    Vec3 pb;
    Vec3 delta;
    double distance;

    closest_points_on_segments(a->a, a->b, b->a, b->b, &pa, &pb);
    if (point_a != NULL) {
        *point_a = pa;
    }
    if (point_b != NULL) {
        *point_b = pb;
    }
    delta = vec3_sub(pb, pa);
    distance = vec3_length(delta);
    if (distance > 0.000001) {
        *normal = vec3_scale(delta, 1.0 / distance);
    } else {
        Vec3 ca = vec3_scale(vec3_add(a->a, a->b), 0.5);
        Vec3 cb = vec3_scale(vec3_add(b->a, b->b), 0.5);
        *normal = vec3_normalize_or(vec3_sub(cb, ca), vec3(1.0, 0.0, 0.0));
    }
    return distance - a->radius - b->radius;
}

static double capsule_clearance(const PhysicsCapsule *a,
                                const PhysicsCapsule *b,
                                Vec3 *normal)
{
    return capsule_clearance_details(a, b, normal, NULL, NULL);
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
        if (equals_ci(tokens[start + 1], "BALANCE_OFFSET") ||
            equals_ci(tokens[start + 1], "COM_OFFSET") ||
            equals_ci(tokens[start + 1], "LEAN")) {
            condition->metric = self ? METRIC_SELF_BALANCE_OFFSET :
                                       METRIC_OPP_BALANCE_OFFSET;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "DOWN") || equals_ci(tokens[start + 1], "FALLEN")) {
            condition->metric = self ? METRIC_SELF_DOWN : METRIC_OPP_DOWN;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "GUARD") ||
            equals_ci(tokens[start + 1], "GUARDING")) {
            condition->metric = self ? METRIC_SELF_GUARD : METRIC_OPP_GUARD;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "GETUP") ||
            equals_ci(tokens[start + 1], "GET_UP")) {
            condition->metric = self ? METRIC_SELF_GETUP : METRIC_OPP_GETUP;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "GETUP_PRESSURE") ||
            equals_ci(tokens[start + 1], "GET_UP_PRESSURE") ||
            equals_ci(tokens[start + 1], "GROUND_PRESSURE")) {
            condition->metric = self ? METRIC_SELF_GETUP_PRESSURE :
                                       METRIC_OPP_GETUP_PRESSURE;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "WALL_PRESSURE") ||
            equals_ci(tokens[start + 1], "CAGE_PRESSURE")) {
            condition->metric = self ? METRIC_SELF_WALL_PRESSURE :
                                       METRIC_OPP_WALL_PRESSURE;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "CLINCH_PRESSURE") ||
            equals_ci(tokens[start + 1], "GRAPPLE_PRESSURE")) {
            condition->metric = self ? METRIC_SELF_CLINCH_PRESSURE :
                                       METRIC_OPP_CLINCH_PRESSURE;
            return 1;
        }
        if (equals_ci(tokens[start + 1], "RECENT_DAMAGE") ||
            equals_ci(tokens[start + 1], "DAMAGE_PULSE")) {
            condition->metric = self ? METRIC_SELF_RECENT_DAMAGE :
                                       METRIC_OPP_RECENT_DAMAGE;
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

static double robot_contact_radius(const RobotState *robot)
{
    return ROBOT_RADIUS_M + (robot->down ? ROBOT_DOWN_RADIUS_BONUS_M : 0.0);
}

static double robot_min_separation(const RobotState *a, const RobotState *b)
{
    return robot_contact_radius(a) + robot_contact_radius(b);
}

static double surface_gap(const Fight *fight)
{
    return clamp_double(center_distance(fight) -
                            robot_min_separation(&fight->robot[0],
                                                 &fight->robot[1]),
                        0.0, 99.0);
}

static double wall_gap(const RobotState *robot)
{
    double d = sqrt(robot->x * robot->x + robot->y * robot->y);
    return clamp_double(ARENA_RADIUS_M - robot_contact_radius(robot) - d,
                        0.0, ARENA_RADIUS_M);
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

static int deflectable_target(BodyPart target)
{
    return target == PART_HEAD || target == PART_TORSO;
}

static BodyPart preferred_block_arm(const RobotState *robot,
                                    CommandId attack_id,
                                    BodyPart target)
{
    BodyPart preferred = PART_INVALID;
    BodyPart fallback = guard_arm(robot);

    (void)target;
    switch (attack_id) {
    case CMD_L_JAB:
    case CMD_L_HOOK:
    case CMD_ELBOW:
        preferred = PART_R_ARM;
        break;
    case CMD_R_CROSS:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
    case CMD_HIGH_KICK:
        preferred = PART_L_ARM;
        break;
    default:
        preferred = fallback;
        break;
    }

    if (preferred != PART_INVALID &&
        !robot->detached[preferred] &&
        robot->integrity[preferred] > 0) {
        return preferred;
    }
    return fallback;
}

static void clear_block_state(RobotState *robot)
{
    robot->block_active = 0;
    robot->block_success = 0;
    robot->block_arm = PART_INVALID;
    robot->block_part = PART_INVALID;
    robot->block_amount = 0;
    robot->block_reaction = 0.0;
    robot->block_target_x = 0.0;
    robot->block_target_y = 0.0;
    robot->block_target_z = 0.0;
    robot->parry_active = 0;
}

static void clear_guard_pose_state(RobotState *robot)
{
    robot->guard_side = PART_INVALID;
    robot->guard_hand_height = 0.0;
    robot->guard_elbow_angle = 0.0;
    robot->guard_coverage_m = 0.0;
}

static int guard_part_available(const RobotState *robot, BodyPart part)
{
    return (part == PART_L_ARM || part == PART_R_ARM) &&
           !robot->detached[part] &&
           robot->integrity[part] > 0;
}

static void predict_block_target(const RobotState *defender,
                                 BodyPart shield,
                                 BodyPart target,
                                 CommandId attack_id,
                                 double gap,
                                 double *target_x,
                                 double *target_y,
                                 double *target_z)
{
    double side = shield == PART_R_ARM ? 1.0 : -1.0;
    double forward = target == PART_TORSO ? 0.30 : 0.27;
    double height = target == PART_TORSO ?
                    ROBOT_CHEST_HEIGHT_M + 0.02 :
                    ROBOT_HEAD_CENTER_HEIGHT_M - 0.20;
    double lateral = target == PART_TORSO ? 0.13 : 0.10;

    switch (attack_id) {
    case CMD_L_JAB:
        lateral += 0.05;
        forward += 0.02;
        break;
    case CMD_R_CROSS:
        lateral -= 0.05;
        forward += 0.04;
        break;
    case CMD_L_HOOK:
    case CMD_R_HOOK:
        lateral += side * 0.04;
        forward -= 0.04;
        break;
    case CMD_UPPERCUT:
        height -= 0.07;
        forward -= 0.05;
        break;
    case CMD_HIGH_KICK:
        lateral += side * 0.07;
        height += 0.03;
        break;
    case CMD_KNEE:
    case CMD_ELBOW:
        forward -= 0.06;
        break;
    default:
        break;
    }

    if (!defender->guard) {
        forward += 0.04;
    }
    if (gap < 0.12) {
        forward -= 0.03;
    }

    *target_x = clamp_double(forward, 0.18, 0.36);
    *target_y = clamp_double(height, ROBOT_CHEST_HEIGHT_M - 0.16,
                             ROBOT_HEAD_CENTER_HEIGHT_M + 0.04);
    *target_z = side * clamp_double(fabs(lateral), 0.06, 0.22);
}

static double block_reaction_score(const Fight *fight,
                                   int defender_side,
                                   BodyPart shield,
                                   BodyPart target,
                                   const MoveSpec *spec,
                                   CommandId attack_id,
                                   double gap)
{
    int attacker_side = defender_side == 0 ? 1 : 0;
    const RobotState *defender = &fight->robot[defender_side];
    const RobotState *attacker = &fight->robot[attacker_side];
    BodyPart preferred = preferred_block_arm(defender, attack_id, target);
    double reaction = defender->guard ? 0.58 : 0.30;

    reaction += clamp_double(attacker->windup_progress, 0.0, 1.0) * 0.30;
    reaction += clamp_double((gap - 0.06) * 0.38, -0.08, 0.15);
    reaction += (double)defender->stability / 360.0;
    reaction -= (double)defender->heat / 360.0;
    reaction -= (double)defender->shock / 300.0;
    reaction -= (double)spec->speed / 80.0;

    if (shield != preferred) {
        reaction -= 0.14;
    }
    if (target == PART_TORSO && !defender->guard) {
        reaction -= 0.08;
    }
    if (fight->clinch) {
        reaction -= 0.11;
    }
    if (attack_id == CMD_L_JAB || attack_id == CMD_R_CROSS) {
        reaction -= 0.05;
    } else if (attack_id == CMD_HIGH_KICK || attack_id == CMD_STOMP) {
        reaction += 0.08;
    }

    return clamp_double(reaction, ROBOT_MIN_BLOCK_REACTION,
                        ROBOT_MAX_BLOCK_REACTION);
}

static void update_guard_pose_state(RobotState *robot)
{
    BodyPart shield = PART_INVALID;
    BodyPart protected_part = PART_HEAD;
    double coverage = 0.0;

    clear_guard_pose_state(robot);
    if (robot->down || robot->defeated) {
        return;
    }

    if (robot->block_active && guard_part_available(robot, robot->block_arm)) {
        shield = robot->block_arm;
        protected_part = robot->block_part;
        coverage = (protected_part == PART_TORSO ?
                    ROBOT_GUARD_TORSO_COVERAGE_M :
                    ROBOT_GUARD_HEAD_COVERAGE_M) +
                   clamp_double((double)robot->block_amount / 1000.0, 0.0, 0.08) *
                       clamp_double(0.55 + robot->block_reaction * 0.70,
                                    0.55, 1.20);
    } else if (robot->guard) {
        shield = guard_arm(robot);
        coverage = ROBOT_GUARD_HEAD_COVERAGE_M;
    }

    if (!guard_part_available(robot, shield)) {
        return;
    }

    robot->guard_side = shield;
    robot->guard_hand_height = robot->block_target_y > 0.0
        ? robot->block_target_y
        : (protected_part == PART_TORSO
               ? ROBOT_CHEST_HEIGHT_M + 0.02
               : ROBOT_HEAD_CENTER_HEIGHT_M - 0.17);
    robot->guard_elbow_angle = protected_part == PART_TORSO
        ? 1.05
        : 1.32;
    if (robot->guard) {
        coverage += protected_part == PART_TORSO ? 0.025 : 0.035;
    }
    robot->guard_coverage_m = clamp_double(coverage, 0.0, 0.24);
}

static BlockResult attempt_block(Fight *fight,
                                 int defender_side,
                                 BodyPart target,
                                 const MoveSpec *spec,
                                 CommandId attack_id,
                                 double gap)
{
    RobotState *defender = &fight->robot[defender_side];
    BlockResult block;
    BodyPart shield;
    int chance;
    double target_x;
    double target_y;
    double target_z;
    double reaction;

    memset(&block, 0, sizeof(block));
    block.shield = PART_INVALID;
    block.target = target;

    if (!deflectable_target(target) || defender->down || defender->defeated) {
        return block;
    }

    shield = preferred_block_arm(defender, attack_id, target);
    if (shield == PART_INVALID) {
        return block;
    }

    block.attempted = 1;
    block.shield = shield;
    predict_block_target(defender, shield, target, attack_id, gap,
                         &target_x, &target_y, &target_z);
    reaction = block_reaction_score(fight, defender_side, shield, target,
                                    spec, attack_id, gap);
    block.target_x = target_x;
    block.target_y = target_y;
    block.target_z = target_z;
    block.reaction = reaction;
    block.late = reaction < 0.42;

    chance = target == PART_HEAD ? 34 : 20;
    chance += defender->guard ? (target == PART_HEAD ? 34 : 26) : 0;
    chance += defender->stability / 5;
    chance += clamp_int(defender->integrity[shield], 0, part_initial[shield]) / 9;
    chance += (int)(reaction * 28.0) - 12;
    chance -= defender->heat / 9;
    chance -= defender->shock / 9;
    chance -= spec->speed / 3;

    if (!defender->guard && target == PART_TORSO) {
        chance -= 4;
    }
    if (gap < 0.10) {
        chance -= 8;
    } else if (gap > 0.55) {
        chance += 5;
    }
    if (fight->clinch) {
        chance -= 8;
    }
    if (block.late) {
        chance -= 12;
    }

    switch (attack_id) {
    case CMD_L_JAB:
        chance -= 4;
        break;
    case CMD_L_HOOK:
    case CMD_R_HOOK:
    case CMD_ELBOW:
        chance -= 8;
        break;
    case CMD_UPPERCUT:
        chance -= 10;
        break;
    case CMD_HIGH_KICK:
        chance += 6;
        break;
    case CMD_THROW:
        chance -= 18;
        break;
    default:
        break;
    }

    block.chance = clamp_int(chance, 6, 92);
    block.roll = rng_range(&fight->rng, 1, 100);
    block.success = block.roll <= block.chance;

    if (block.success) {
        int spread = rng_range(&fight->rng, -6, 8);
        block.coverage = target == PART_HEAD
            ? 42 + block.chance / 3 + spread
            : 28 + block.chance / 4 + spread;
        if (defender->guard) {
            block.coverage += target == PART_HEAD ? 12 : 8;
        }
        block.coverage = (int)((double)block.coverage *
                               clamp_double(0.68 + reaction * 0.48,
                                            0.68, 1.18));
        block.coverage = clamp_int(block.coverage, 25,
                                   target == PART_HEAD ? 82 : 66);
        if (block.late) {
            block.coverage = clamp_int(block.coverage - 12, 12, block.coverage);
        }
        block.accuracy_penalty = target == PART_HEAD
            ? 8 + block.coverage / 5
            : 4 + block.coverage / 8;
        block.damage_reduction_pct = block.coverage;
        block.parry = reaction > 0.74 &&
                      block.roll <= clamp_int(block.chance * 3 / 5, 1, 100) &&
                      target == PART_HEAD;
        if (block.parry) {
            block.coverage = clamp_int(block.coverage + 8, 25, 88);
            block.damage_reduction_pct = clamp_int(block.damage_reduction_pct + 10,
                                                   0, 88);
            block.accuracy_penalty += 4;
        }
    } else {
        block.coverage = defender->guard
            ? (target == PART_HEAD ? 20 : 12)
            : (target == PART_HEAD ? 8 : 5);
        block.coverage = (int)((double)block.coverage *
                               clamp_double(0.52 + reaction * 0.55,
                                            0.52, 1.00));
        block.accuracy_penalty = defender->guard
            ? (target == PART_HEAD ? 6 : 3)
            : 2;
        block.damage_reduction_pct = defender->guard
            ? block.coverage / 2
            : 0;
    }

    defender->block_active = block.attempted;
    defender->block_success = block.success;
    defender->block_arm = shield;
    defender->block_part = target;
    defender->block_amount = block.coverage;
    defender->block_reaction = reaction;
    defender->block_target_x = target_x;
    defender->block_target_y = target_y;
    defender->block_target_z = target_z;
    defender->parry_active = block.parry;
    update_guard_pose_state(defender);
    return block;
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

static void facing_basis(const RobotState *robot, double *fx, double *fy,
                         double *rx, double *ry)
{
    *fx = cos(robot->facing);
    *fy = sin(robot->facing);
    *rx = -sin(robot->facing);
    *ry = cos(robot->facing);
}

static void clamp_contact_to_arena(double *x, double *y, double inset)
{
    double max_dist = ARENA_RADIUS_M - inset;
    double dist = sqrt(*x * *x + *y * *y);

    if (dist > max_dist && dist > 0.0001) {
        *x = *x / dist * max_dist;
        *y = *y / dist * max_dist;
    }
}

static void add_capsule(RobotCapsules *capsules, BodyPart part,
                        Vec3 a, Vec3 b, double radius, double mass_kg)
{
    PhysicsCapsule *capsule;

    if (capsules->count >= ROBOT_MAX_CAPSULES) {
        return;
    }

    capsule = &capsules->items[capsules->count++];
    capsule->part = part;
    capsule->a = a;
    capsule->b = b;
    capsule->radius = radius;
    capsule->mass_kg = mass_kg > 0.0 ? mass_kg : part_mass_kg[part];
}

static int is_core_part(BodyPart part)
{
    return part == PART_TORSO || part == PART_HEAD;
}

static double capsule_pair_hardness(const PhysicsCapsule *a,
                                    const PhysicsCapsule *b)
{
    double hardness = 1.0;

    if (is_core_part(a->part)) {
        hardness += 0.18;
    }
    if (is_core_part(b->part)) {
        hardness += 0.18;
    }
    if ((a->part == PART_L_LEG || a->part == PART_R_LEG) &&
        (b->part == PART_L_LEG || b->part == PART_R_LEG)) {
        hardness += 0.08;
    }
    if ((a->part == PART_L_ARM || a->part == PART_R_ARM) ||
        (b->part == PART_L_ARM || b->part == PART_R_ARM)) {
        hardness -= 0.08;
    }
    return clamp_double(hardness, 0.82, 1.42);
}

static void apply_contact_yaw(RobotState *robot, double nx, double ny,
                              double impulse, double inverse_mass,
                              double sign)
{
    double lateral = -sin(robot->facing) * nx + cos(robot->facing) * ny;
    double yaw = lateral * impulse * inverse_mass * 0.030 * sign;

    robot->angular_velocity =
        clamp_abs(robot->angular_velocity + yaw,
                  ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
}

static Vec3 vec3_blend(Vec3 from, Vec3 to, double amount)
{
    double t = clamp_double(amount, 0.0, 1.0);
    return vec3_add(from, vec3_scale(vec3_sub(to, from), t));
}

static Vec3 fixed_two_bone_joint(Vec3 anchor, Vec3 target,
                                 double upper_length, double lower_length,
                                 Vec3 preferred_bend, Vec3 *end_out)
{
    double max_reach = upper_length + lower_length - 0.001;
    double min_reach = fabs(upper_length - lower_length) + 0.001;
    Vec3 delta = vec3_sub(target, anchor);
    double requested_distance = vec3_length(delta);
    Vec3 direction = vec3_normalize_or(delta, preferred_bend);
    double distance = clamp_double(requested_distance, min_reach, max_reach);
    Vec3 end = vec3_add(anchor, vec3_scale(direction, distance));
    double along = (upper_length * upper_length + distance * distance -
                    lower_length * lower_length) / (2.0 * distance);
    double lift = sqrt(fmax(0.0, upper_length * upper_length - along * along));
    Vec3 bend = vec3_sub(preferred_bend,
                         vec3_scale(direction, vec3_dot(preferred_bend,
                                                        direction)));

    if (vec3_length(bend) < 0.0001) {
        Vec3 fallback = fabs(direction.z) < 0.92 ? vec3(0.0, 0.0, 1.0) :
                                                   vec3(1.0, 0.0, 0.0);
        bend = vec3_sub(fallback,
                        vec3_scale(direction, vec3_dot(fallback, direction)));
    }
    bend = vec3_normalize_or(bend, vec3(0.0, 0.0, 1.0));

    if (end_out != NULL) {
        *end_out = end;
    }
    return vec3_add(vec3_add(anchor, vec3_scale(direction, along)),
                    vec3_scale(bend, lift));
}

static Vec3 robot_hand_local_for_contact(const RobotState *robot, double side,
                                         BodyPart part)
{
    double guard_height = robot->guard_hand_height > 0.0 ?
                          robot->guard_hand_height :
                          ROBOT_GUARD_HAND_HEIGHT_M + 0.05;
    Vec3 hand = vec3(0.18, ROBOT_GUARD_HAND_HEIGHT_M - 0.04, side * 0.21);

    if (robot->guard) {
        hand = vec3(0.22, guard_height, side * 0.15);
    }

    if (robot->block_active && robot->block_arm == part) {
        double target_y = robot->block_part == PART_TORSO ?
                          ROBOT_CHEST_HEIGHT_M + 0.02 :
                          ROBOT_HEAD_CENTER_HEIGHT_M - 0.22;
        double lateral = robot->block_part == PART_TORSO ? 0.13 : 0.10;
        double forward_reach = robot->block_part == PART_TORSO ? 0.30 : 0.27;
        double reaction = clamp_double(robot->block_reaction, 0.0, 1.0);
        double blend = (0.24 + (double)robot->block_amount / 120.0) *
                       (0.42 + reaction * 0.78);
        Vec3 block_target = vec3(forward_reach, target_y, side * lateral);

        if (robot->block_target_y > 0.0) {
            block_target = vec3(robot->block_target_x,
                                robot->block_target_y,
                                robot->block_target_z);
        }
        if (robot->parry_active) {
            block_target.z += side * 0.055;
            block_target.x += 0.025;
        }
        hand = vec3_blend(hand, block_target, blend);
    }

    return hand;
}

static void add_arm_capsules(const RobotState *robot, RobotCapsules *capsules,
                             double side)
{
    BodyPart part = side < 0.0 ? PART_L_ARM : PART_R_ARM;
    Vec3 shoulder;
    Vec3 hand;
    Vec3 hand_direction;
    Vec3 wrist_target;
    Vec3 elbow;
    Vec3 wrist;
    Vec3 hand_tip;

    if (robot->detached[part]) {
        return;
    }

    shoulder = vec3(0.02, ROBOT_SHOULDER_HEIGHT_M,
                    side * ROBOT_SHOULDER_HALF_WIDTH_M);
    hand = robot_hand_local_for_contact(robot, side, part);
    hand_direction = vec3_normalize_or(vec3_sub(hand, shoulder),
                                       vec3(1.0, -0.1, side * 0.25));
    wrist_target = vec3_sub(hand,
                            vec3_scale(hand_direction,
                                       ROBOT_HAND_SEGMENT_LENGTH_M));
    elbow = fixed_two_bone_joint(shoulder, wrist_target,
                                 ROBOT_UPPER_ARM_LENGTH_M,
                                 ROBOT_FOREARM_LENGTH_M,
                                 vec3(-0.10, -0.08, side * 0.42),
                                 &wrist);
    hand_tip = vec3_add(wrist,
                        vec3_scale(hand_direction,
                                   ROBOT_HAND_SEGMENT_LENGTH_M));

    add_capsule(capsules, part,
                local_to_world_point(robot, shoulder),
                local_to_world_point(robot, elbow),
                ROBOT_UPPER_ARM_RADIUS_M, ROBOT_UPPER_ARM_MASS_KG);
    add_capsule(capsules, part,
                local_to_world_point(robot, elbow),
                local_to_world_point(robot, wrist),
                (ROBOT_ELBOW_RADIUS_M + ROBOT_WRIST_RADIUS_M) * 0.5,
                ROBOT_FOREARM_MASS_KG);
    add_capsule(capsules, part,
                local_to_world_point(robot, wrist),
                local_to_world_point(robot, hand_tip),
                ROBOT_FIST_RADIUS_M, ROBOT_HAND_MASS_KG);
}

static void add_leg_capsules(const RobotState *robot, RobotCapsules *capsules,
                             int foot, double side)
{
    BodyPart part = foot == FOOT_LEFT ? PART_L_LEG : PART_R_LEG;
    Vec3 hip_local;
    Vec3 hip_world;
    Vec3 foot_world;
    Vec3 forward_world;
    Vec3 side_world;
    Vec3 knee;
    Vec3 ankle;
    Vec3 toe;

    if (robot->detached[part]) {
        return;
    }

    hip_local = vec3(-0.01, ROBOT_HIP_HEIGHT_M, side * ROBOT_HIP_HALF_WIDTH_M);
    hip_world = local_to_world_point(robot, hip_local);
    foot_world = vec3(robot->foot_x[foot], robot->foot_y[foot],
                      ROBOT_FOOT_HEIGHT_M * 0.25);
    forward_world = robot_forward_vec(robot);
    side_world = robot_side_vec(robot, side);
    knee = fixed_two_bone_joint(hip_world, foot_world,
                                ROBOT_UPPER_LEG_LENGTH_M,
                                ROBOT_LOWER_LEG_LENGTH_M,
                                vec3_add(vec3_add(vec3_scale(forward_world, 0.35),
                                                  vec3_scale(side_world, 0.08)),
                                         vec3(0.0, 0.0, 0.06)),
                                &ankle);
    toe = vec3_add(ankle, vec3_scale(forward_world,
                                     ROBOT_FOOT_SEGMENT_LENGTH_M));

    add_capsule(capsules, part, hip_world, knee,
                ROBOT_UPPER_LEG_RADIUS_M, ROBOT_UPPER_LEG_MASS_KG);
    add_capsule(capsules, part, knee, ankle,
                (ROBOT_KNEE_RADIUS_M + ROBOT_ANKLE_RADIUS_M) * 0.5,
                ROBOT_LOWER_LEG_MASS_KG);
    add_capsule(capsules, part, ankle, toe,
                ROBOT_FOOT_WIDTH_M * 0.5, ROBOT_FOOT_MASS_KG);
}

static void build_down_capsules(const RobotState *robot,
                                RobotCapsules *capsules)
{
    add_capsule(capsules, PART_TORSO,
                local_to_world_point(robot, vec3(-0.26, 0.34, 0.0)),
                local_to_world_point(robot, vec3(0.52, 0.34, 0.0)),
                0.23, part_mass_kg[PART_TORSO]);
    if (!robot->head_detached) {
        add_capsule(capsules, PART_HEAD,
                    local_to_world_point(robot, vec3(0.54, 0.36, 0.0)),
                    local_to_world_point(robot, vec3(0.74, 0.36, 0.0)),
                    fmax(ROBOT_HEAD_WIDTH_M, ROBOT_HEAD_DEPTH_M) * 0.46,
                    part_mass_kg[PART_HEAD]);
    }
    if (!robot->detached[PART_L_ARM]) {
        add_capsule(capsules, PART_L_ARM,
                    local_to_world_point(robot, vec3(0.02, 0.28, -0.24)),
                    local_to_world_point(robot, vec3(0.48, 0.24, -0.34)),
                    ROBOT_ELBOW_RADIUS_M, part_mass_kg[PART_L_ARM]);
    }
    if (!robot->detached[PART_R_ARM]) {
        add_capsule(capsules, PART_R_ARM,
                    local_to_world_point(robot, vec3(0.02, 0.28, 0.24)),
                    local_to_world_point(robot, vec3(0.48, 0.24, 0.34)),
                    ROBOT_ELBOW_RADIUS_M, part_mass_kg[PART_R_ARM]);
    }
    if (!robot->detached[PART_L_LEG]) {
        add_capsule(capsules, PART_L_LEG,
                    local_to_world_point(robot, vec3(-0.24, 0.22, -0.13)),
                    local_to_world_point(robot, vec3(-0.82, 0.19, -0.20)),
                    ROBOT_UPPER_LEG_RADIUS_M, part_mass_kg[PART_L_LEG]);
    }
    if (!robot->detached[PART_R_LEG]) {
        add_capsule(capsules, PART_R_LEG,
                    local_to_world_point(robot, vec3(-0.24, 0.22, 0.13)),
                    local_to_world_point(robot, vec3(-0.82, 0.19, 0.20)),
                    ROBOT_UPPER_LEG_RADIUS_M, part_mass_kg[PART_R_LEG]);
    }
}

static void build_robot_capsules(const RobotState *robot,
                                 RobotCapsules *capsules)
{
    memset(capsules, 0, sizeof(*capsules));

    if (robot->down) {
        build_down_capsules(robot, capsules);
        return;
    }

    add_capsule(capsules, PART_TORSO,
                local_to_world_point(robot, vec3(0.00, ROBOT_PELVIS_HEIGHT_M, 0.0)),
                local_to_world_point(robot, vec3(0.02, ROBOT_NECK_HEIGHT_M, 0.0)),
                fmax(ROBOT_TORSO_WIDTH_M, ROBOT_TORSO_DEPTH_M) * 0.45,
                part_mass_kg[PART_TORSO]);
    if (!robot->head_detached) {
        add_capsule(capsules, PART_HEAD,
                    local_to_world_point(robot,
                                         vec3(0.03,
                                              ROBOT_HEAD_CENTER_HEIGHT_M -
                                                  ROBOT_HEAD_HEIGHT_M * 0.23,
                                              0.0)),
                    local_to_world_point(robot,
                                         vec3(0.03,
                                              ROBOT_HEAD_CENTER_HEIGHT_M +
                                                  ROBOT_HEAD_HEIGHT_M * 0.23,
                                              0.0)),
                    fmax(ROBOT_HEAD_WIDTH_M, ROBOT_HEAD_DEPTH_M) * 0.46,
                    part_mass_kg[PART_HEAD]);
    }
    add_arm_capsules(robot, capsules, -1.0);
    add_arm_capsules(robot, capsules, 1.0);
    add_leg_capsules(robot, capsules, FOOT_LEFT, -1.0);
    add_leg_capsules(robot, capsules, FOOT_RIGHT, 1.0);
}

static double point_segment_distance_2d(double px, double py,
                                        double ax, double ay,
                                        double bx, double by,
                                        double *closest_x,
                                        double *closest_y)
{
    double sx = bx - ax;
    double sy = by - ay;
    double denom = sx * sx + sy * sy;
    double t = 0.0;
    double cx;
    double cy;
    double dx;
    double dy;

    if (denom > 0.000001) {
        t = clamp_double(((px - ax) * sx + (py - ay) * sy) / denom,
                         0.0, 1.0);
    }

    cx = ax + sx * t;
    cy = ay + sy * t;
    if (closest_x != NULL) {
        *closest_x = cx;
    }
    if (closest_y != NULL) {
        *closest_y = cy;
    }

    dx = px - cx;
    dy = py - cy;
    return sqrt(dx * dx + dy * dy);
}

static int foot_can_support(const RobotState *robot, int foot)
{
    if (foot < 0 || foot > 1 || robot->detached[PART_L_LEG + foot]) {
        return 0;
    }

    switch (robot->foot_state[foot]) {
    case CFA_FOOT_PLANTED:
    case CFA_FOOT_PIVOTING:
        return 1;
    case CFA_FOOT_LANDING:
        return robot->foot_phase[foot] > 0.74;
    default:
        return 0;
    }
}

static void update_robot_balance(RobotState *robot)
{
    RobotCapsules capsules;
    double total_mass = 0.0;
    double mx = 0.0;
    double my = 0.0;
    double mz = 0.0;
    int left_support = foot_can_support(robot, FOOT_LEFT);
    int right_support = foot_can_support(robot, FOOT_RIGHT);
    int i;

    build_robot_capsules(robot, &capsules);
    for (i = 0; i < capsules.count; i++) {
        const PhysicsCapsule *capsule = &capsules.items[i];
        double mass = fmax(0.1, capsule->mass_kg);
        double cx = (capsule->a.x + capsule->b.x) * 0.5;
        double cy = (capsule->a.y + capsule->b.y) * 0.5;
        double cz = (capsule->a.z + capsule->b.z) * 0.5;

        mx += cx * mass;
        my += cy * mass;
        mz += cz * mass;
        total_mass += mass;
    }

    if (total_mass <= 0.0) {
        robot->center_mass_x = robot->x;
        robot->center_mass_y = robot->y;
        robot->center_mass_z = robot->down ? 0.34 : ROBOT_PELVIS_HEIGHT_M;
    } else {
        robot->center_mass_x = mx / total_mass;
        robot->center_mass_y = my / total_mass;
        robot->center_mass_z = mz / total_mass;
    }

    if (robot->down) {
        robot->support_center_x = robot->center_mass_x;
        robot->support_center_y = robot->center_mass_y;
        robot->balance_offset = 0.0;
        robot->balance_supported = 1;
        robot->balance_state = CFA_BALANCE_SUPPORTED;
        robot->support_radius = ROBOT_FOOT_LENGTH_M;
        return;
    }

    if (!left_support && !right_support) {
        int fallback = robot->pivot_foot >= 0 && robot->pivot_foot <= 1 ?
                       robot->pivot_foot : FOOT_LEFT;
        double dx;
        double dy;

        if (robot->detached[PART_L_LEG + fallback]) {
            fallback = fallback == FOOT_LEFT ? FOOT_RIGHT : FOOT_LEFT;
        }
        robot->support_center_x = robot->foot_x[fallback];
        robot->support_center_y = robot->foot_y[fallback];
        dx = robot->center_mass_x - robot->support_center_x;
        dy = robot->center_mass_y - robot->support_center_y;
        robot->support_radius = ROBOT_FOOT_WIDTH_M * 0.55;
        robot->balance_offset = sqrt(dx * dx + dy * dy) -
                                robot->support_radius;
    } else if (left_support && right_support) {
        double closest_x;
        double closest_y;
        double distance = point_segment_distance_2d(
            robot->center_mass_x, robot->center_mass_y,
            robot->foot_x[FOOT_LEFT], robot->foot_y[FOOT_LEFT],
            robot->foot_x[FOOT_RIGHT], robot->foot_y[FOOT_RIGHT],
            &closest_x, &closest_y);

        robot->support_center_x = closest_x;
        robot->support_center_y = closest_y;
        robot->support_radius = ROBOT_FOOT_WIDTH_M * 0.70 +
                                ROBOT_FOOT_LENGTH_M * 0.38;
        robot->balance_offset = distance - robot->support_radius;
    } else {
        int foot = left_support ? FOOT_LEFT : FOOT_RIGHT;
        double dx = robot->center_mass_x - robot->foot_x[foot];
        double dy = robot->center_mass_y - robot->foot_y[foot];

        robot->support_center_x = robot->foot_x[foot];
        robot->support_center_y = robot->foot_y[foot];
        robot->support_radius = ROBOT_FOOT_LENGTH_M * 0.45;
        robot->balance_offset = sqrt(dx * dx + dy * dy) -
                                robot->support_radius;
    }

    if (robot->balance_offset <= -ROBOT_BALANCE_EDGE_MARGIN_M) {
        robot->balance_state = CFA_BALANCE_SUPPORTED;
        robot->balance_supported = 1;
    } else if (robot->balance_offset <= ROBOT_BALANCE_EDGE_MARGIN_M) {
        robot->balance_state = CFA_BALANCE_EDGE;
        robot->balance_supported = 1;
    } else {
        robot->balance_state = CFA_BALANCE_OUTSIDE;
        robot->balance_supported = 0;
    }
}

static void foot_home(const RobotState *robot, int foot,
                      double *x, double *y)
{
    double fx;
    double fy;
    double rx;
    double ry;
    double side = foot == FOOT_LEFT ? -1.0 : 1.0;
    double fore = foot == FOOT_LEFT ? ROBOT_FOOT_FORE_OFFSET_M :
                                      -ROBOT_FOOT_FORE_OFFSET_M;

    facing_basis(robot, &fx, &fy, &rx, &ry);
    *x = robot->x + fx * fore + rx * side * ROBOT_FOOT_HALF_STANCE_M;
    *y = robot->y + fy * fore + ry * side * ROBOT_FOOT_HALF_STANCE_M;
    clamp_contact_to_arena(x, y, ROBOT_FOOT_WIDTH_M);
}

static void place_feet_from_body(RobotState *robot)
{
    foot_home(robot, FOOT_LEFT, &robot->foot_x[FOOT_LEFT],
              &robot->foot_y[FOOT_LEFT]);
    foot_home(robot, FOOT_RIGHT, &robot->foot_x[FOOT_RIGHT],
              &robot->foot_y[FOOT_RIGHT]);
    robot->foot_target_x[FOOT_LEFT] = robot->foot_x[FOOT_LEFT];
    robot->foot_target_y[FOOT_LEFT] = robot->foot_y[FOOT_LEFT];
    robot->foot_target_x[FOOT_RIGHT] = robot->foot_x[FOOT_RIGHT];
    robot->foot_target_y[FOOT_RIGHT] = robot->foot_y[FOOT_RIGHT];
    robot->foot_state[FOOT_LEFT] = CFA_FOOT_PLANTED;
    robot->foot_state[FOOT_RIGHT] = CFA_FOOT_PLANTED;
    robot->foot_phase[FOOT_LEFT] = 1.0;
    robot->foot_phase[FOOT_RIGHT] = 1.0;
    robot->swing_foot = FOOT_NONE;
    robot->pivot_foot = FOOT_NONE;
    robot->pivot_angle = 0.0;
    update_robot_balance(robot);
}

static void store_previous_pose(RobotState *robot)
{
    robot->prev_x = robot->x;
    robot->prev_y = robot->y;
    robot->prev_facing = robot->facing;
    robot->prev_foot_x[FOOT_LEFT] = robot->foot_x[FOOT_LEFT];
    robot->prev_foot_y[FOOT_LEFT] = robot->foot_y[FOOT_LEFT];
    robot->prev_foot_x[FOOT_RIGHT] = robot->foot_x[FOOT_RIGHT];
    robot->prev_foot_y[FOOT_RIGHT] = robot->foot_y[FOOT_RIGHT];
}

static int next_step_foot(RobotState *robot)
{
    robot->step_sequence++;
    return (robot->step_sequence & 1) ? FOOT_LEFT : FOOT_RIGHT;
}

static int active_swing_foot(const RobotState *robot)
{
    int foot;

    for (foot = 0; foot < 2; foot++) {
        if (robot->foot_state[foot] == CFA_FOOT_LIFTING ||
            robot->foot_state[foot] == CFA_FOOT_SWINGING ||
            robot->foot_state[foot] == CFA_FOOT_LANDING) {
            return foot;
        }
    }
    return FOOT_NONE;
}

static double command_pivot_angle(CommandId command)
{
    switch (command) {
    case CMD_CIRCLE_L:
        return -0.09;
    case CMD_CIRCLE_R:
        return 0.09;
    case CMD_L_HOOK:
    case CMD_ELBOW:
        return -0.12;
    case CMD_R_CROSS:
        return 0.12;
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
        return 0.15;
    case CMD_LOW_KICK:
    case CMD_HIGH_KICK:
    case CMD_KNEE:
    case CMD_STOMP:
        return 0.20;
    default:
        return 0.0;
    }
}

static void pivot_body_about_foot(RobotState *robot, int foot, double angle)
{
    double dx;
    double dy;
    double ca;
    double sa;

    if (foot < 0 || foot > 1 || robot->detached[PART_L_LEG + foot]) {
        return;
    }

    dx = robot->x - robot->foot_x[foot];
    dy = robot->y - robot->foot_y[foot];
    ca = cos(angle);
    sa = sin(angle);
    robot->x = robot->foot_x[foot] + dx * ca - dy * sa;
    robot->y = robot->foot_y[foot] + dx * sa + dy * ca;
    robot->facing = normalize_angle(robot->facing + angle);
    robot->angular_velocity = clamp_abs(robot->angular_velocity + angle * 0.45,
                                        ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    clamp_contact_to_arena(&robot->x, &robot->y, robot_contact_radius(robot));
}

static void plan_foot_target(RobotState *robot, int foot,
                             double dir_x, double dir_y,
                             double stride)
{
    double home_x;
    double home_y;
    double length = sqrt(dir_x * dir_x + dir_y * dir_y);

    if (length > 0.0001) {
        dir_x /= length;
        dir_y /= length;
    } else {
        dir_x = cos(robot->facing);
        dir_y = sin(robot->facing);
    }

    foot_home(robot, foot, &home_x, &home_y);
    robot->foot_target_x[foot] = home_x + dir_x * stride;
    robot->foot_target_y[foot] = home_y + dir_y * stride;
    clamp_contact_to_arena(&robot->foot_target_x[foot],
                           &robot->foot_target_y[foot],
                           ROBOT_FOOT_WIDTH_M);

    if (robot->foot_state[foot] != CFA_FOOT_LIFTING &&
        robot->foot_state[foot] != CFA_FOOT_SWINGING &&
        robot->foot_state[foot] != CFA_FOOT_LANDING) {
        robot->foot_phase[foot] = 0.0;
    }
    robot->foot_state[foot] = CFA_FOOT_LIFTING;
}

static void set_step_footwork(RobotState *robot, CommandId command,
                              double dir_x, double dir_y, double impulse)
{
    int active = active_swing_foot(robot);
    double stride = clamp_double(0.11 + impulse * 0.85, 0.10, 0.34);

    switch (command) {
    case CMD_STRAFE_L:
        robot->swing_foot = active == FOOT_NONE ? FOOT_LEFT : active;
        robot->pivot_foot = FOOT_RIGHT;
        break;
    case CMD_STRAFE_R:
        robot->swing_foot = active == FOOT_NONE ? FOOT_RIGHT : active;
        robot->pivot_foot = FOOT_LEFT;
        break;
    case CMD_CIRCLE_L:
        robot->swing_foot = active == FOOT_NONE ? FOOT_RIGHT : active;
        robot->pivot_foot = FOOT_LEFT;
        break;
    case CMD_CIRCLE_R:
        robot->swing_foot = active == FOOT_NONE ? FOOT_LEFT : active;
        robot->pivot_foot = FOOT_RIGHT;
        break;
    case CMD_ADVANCE:
    case CMD_RETREAT:
    case CMD_RESET:
        robot->swing_foot = active == FOOT_NONE ? next_step_foot(robot) : active;
        robot->pivot_foot = robot->swing_foot == FOOT_LEFT ? FOOT_RIGHT : FOOT_LEFT;
        break;
    default:
        break;
    }

    robot->pivot_angle = command_pivot_angle(command);
    if (robot->swing_foot >= 0 && robot->swing_foot <= 1 &&
        !robot->detached[PART_L_LEG + robot->swing_foot]) {
        plan_foot_target(robot, robot->swing_foot, dir_x, dir_y, stride);
    }
    if (robot->pivot_foot >= 0 && robot->pivot_foot <= 1 &&
        !robot->detached[PART_L_LEG + robot->pivot_foot]) {
        robot->foot_state[robot->pivot_foot] =
            fabs(robot->pivot_angle) > 0.001 ? CFA_FOOT_PIVOTING :
                                               CFA_FOOT_PLANTED;
        robot->foot_phase[robot->pivot_foot] = 1.0;
        pivot_body_about_foot(robot, robot->pivot_foot,
                              robot->pivot_angle * 0.20);
    }
}

static void set_attack_footwork(RobotState *robot, CommandId command)
{
    robot->pivot_angle = command_pivot_angle(command);

    switch (command) {
    case CMD_LOW_KICK:
    case CMD_HIGH_KICK:
    case CMD_KNEE:
    case CMD_STOMP:
        robot->swing_foot = FOOT_RIGHT;
        robot->pivot_foot = FOOT_LEFT;
        break;
    case CMD_R_CROSS:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
        robot->swing_foot = FOOT_NONE;
        robot->pivot_foot = FOOT_RIGHT;
        break;
    case CMD_L_HOOK:
    case CMD_ELBOW:
        robot->swing_foot = FOOT_NONE;
        robot->pivot_foot = FOOT_LEFT;
        break;
    case CMD_CLINCH:
    case CMD_THROW:
        robot->swing_foot = FOOT_NONE;
        robot->pivot_foot = FOOT_NONE;
        break;
    default:
        break;
    }

    if (robot->pivot_foot >= 0 && robot->pivot_foot <= 1 &&
        !robot->detached[PART_L_LEG + robot->pivot_foot]) {
        robot->foot_state[robot->pivot_foot] =
            fabs(robot->pivot_angle) > 0.001 ? CFA_FOOT_PIVOTING :
                                               CFA_FOOT_PLANTED;
        robot->foot_phase[robot->pivot_foot] = 1.0;
        pivot_body_about_foot(robot, robot->pivot_foot,
                              robot->pivot_angle * 0.32);
    }
    if (robot->swing_foot >= 0 && robot->swing_foot <= 1 &&
        !robot->detached[PART_L_LEG + robot->swing_foot]) {
        if (command == CMD_LOW_KICK || command == CMD_HIGH_KICK ||
            command == CMD_KNEE || command == CMD_STOMP) {
            double fx = cos(robot->facing);
            double fy = sin(robot->facing);
            plan_foot_target(robot, robot->swing_foot, fx, fy, 0.26);
        }
    }
}

static void update_robot_footwork(RobotState *robot)
{
    int foot;
    int support = robot->pivot_foot;
    double home_x[2];
    double home_y[2];

    foot_home(robot, FOOT_LEFT, &home_x[FOOT_LEFT], &home_y[FOOT_LEFT]);
    foot_home(robot, FOOT_RIGHT, &home_x[FOOT_RIGHT], &home_y[FOOT_RIGHT]);

    if (robot->down || !mode_available(robot, USE_ANY_LEG)) {
        for (foot = 0; foot < 2; foot++) {
            robot->foot_x[foot] += (home_x[foot] - robot->foot_x[foot]) * 0.65;
            robot->foot_y[foot] += (home_y[foot] - robot->foot_y[foot]) * 0.65;
            robot->foot_target_x[foot] = home_x[foot];
            robot->foot_target_y[foot] = home_y[foot];
            robot->foot_state[foot] = robot->down ? CFA_FOOT_SLIPPING :
                                                    CFA_FOOT_PLANTED;
            robot->foot_phase[foot] = 1.0;
            clamp_contact_to_arena(&robot->foot_x[foot], &robot->foot_y[foot],
                                   ROBOT_FOOT_DOWN_INSET_M);
        }
        robot->swing_foot = FOOT_NONE;
        robot->pivot_foot = FOOT_NONE;
        robot->pivot_angle = 0.0;
        update_robot_balance(robot);
        return;
    }

    if (support < 0 || support > 1 || robot->detached[PART_L_LEG + support]) {
        support = robot->swing_foot == FOOT_LEFT ? FOOT_RIGHT : FOOT_LEFT;
        if (support < 0 || support > 1 || robot->detached[PART_L_LEG + support]) {
            support = FOOT_NONE;
        }
    }

    for (foot = 0; foot < 2; foot++) {
        double dx = home_x[foot] - robot->foot_x[foot];
        double dy = home_y[foot] - robot->foot_y[foot];
        double distance = sqrt(dx * dx + dy * dy);
        int moving = robot->foot_state[foot] == CFA_FOOT_LIFTING ||
                     robot->foot_state[foot] == CFA_FOOT_SWINGING ||
                     robot->foot_state[foot] == CFA_FOOT_LANDING;

        if (robot->detached[PART_L_LEG + foot]) {
            robot->foot_x[foot] = home_x[foot];
            robot->foot_y[foot] = home_y[foot];
            robot->foot_target_x[foot] = home_x[foot];
            robot->foot_target_y[foot] = home_y[foot];
            robot->foot_state[foot] = CFA_FOOT_SLIPPING;
            robot->foot_phase[foot] = 1.0;
            continue;
        }

        if (foot == robot->swing_foot || moving) {
            double tx = robot->foot_target_x[foot];
            double ty = robot->foot_target_y[foot];
            double progress;
            double settle;

            robot->foot_phase[foot] = clamp_double(robot->foot_phase[foot] + 0.42,
                                                   0.0, 1.0);
            progress = robot->foot_phase[foot];
            settle = 0.42 + progress * 0.36;
            robot->foot_x[foot] += (tx - robot->foot_x[foot]) * settle;
            robot->foot_y[foot] += (ty - robot->foot_y[foot]) * settle;

            if (progress < 0.34) {
                robot->foot_state[foot] = CFA_FOOT_LIFTING;
            } else if (progress < 0.78) {
                robot->foot_state[foot] = CFA_FOOT_SWINGING;
            } else if (progress < 1.0) {
                robot->foot_state[foot] = CFA_FOOT_LANDING;
            } else {
                robot->foot_x[foot] = tx;
                robot->foot_y[foot] = ty;
                robot->foot_state[foot] = CFA_FOOT_PLANTED;
                if (robot->swing_foot == foot) {
                    robot->swing_foot = FOOT_NONE;
                }
            }
        } else if (foot == support && distance < ROBOT_STEP_SUPPORT_LIMIT_M) {
            robot->foot_target_x[foot] = robot->foot_x[foot];
            robot->foot_target_y[foot] = robot->foot_y[foot];
            robot->foot_state[foot] = foot == robot->pivot_foot &&
                                      fabs(robot->pivot_angle) > 0.001 ?
                                      CFA_FOOT_PIVOTING : CFA_FOOT_PLANTED;
            robot->foot_phase[foot] = 1.0;
        } else if (distance > ROBOT_STEP_TRIGGER_M) {
            robot->swing_foot = foot;
            robot->foot_target_x[foot] = home_x[foot];
            robot->foot_target_y[foot] = home_y[foot];
            robot->foot_state[foot] = CFA_FOOT_LIFTING;
            robot->foot_phase[foot] = 0.0;
        } else {
            robot->foot_x[foot] += dx * ROBOT_FOOT_SETTLE_RATE;
            robot->foot_y[foot] += dy * ROBOT_FOOT_SETTLE_RATE;
            robot->foot_target_x[foot] = home_x[foot];
            robot->foot_target_y[foot] = home_y[foot];
            robot->foot_state[foot] = distance > 0.05 ? CFA_FOOT_LANDING :
                                                       CFA_FOOT_PLANTED;
            robot->foot_phase[foot] = 1.0;
        }

        clamp_contact_to_arena(&robot->foot_x[foot], &robot->foot_y[foot],
                               ROBOT_FOOT_WIDTH_M);
    }
    update_robot_balance(robot);
}

static void init_robot(RobotState *robot, int side)
{
    int i;

    memset(robot, 0, sizeof(*robot));
    for (i = 0; i < PART_COUNT; i++) {
        robot->integrity[i] = part_initial[i];
        robot->detached[i] = 0;
        robot->scuff[i] = 0;
        robot->dent[i] = 0;
    }
    robot->processor = 100;
    robot->shock = 0;
    robot->stability = 100;
    robot->heat = 0;
    robot->x = side == 0 ? -1.15 : 1.15;
    robot->y = side == 0 ? -0.20 : 0.20;
    robot->facing = side == 0 ? 0.0 : PI;
    robot->angular_velocity = 0.0;
    robot->wall_braced = 0;
    robot->wall_nx = side == 0 ? -1.0 : 1.0;
    robot->wall_ny = 0.0;
    robot->step_sequence = side;
    place_feet_from_body(robot);
    store_previous_pose(robot);
    robot->last_impact_part = PART_INVALID;
    robot->last_impact_damage = 0;
    clear_block_state(robot);
    clear_guard_pose_state(robot);
    clear_windup_state(robot);
    robot->attack_released = 0;
    robot->recovery_ticks = 0;
    robot->strike_phase = ROBOT_STRIKE_PHASE_IDLE;
    robot->follow_through = 0.0;
    robot->recoil = 0.0;
    robot->stagger_state = ROBOT_STAGGER_NONE;
    robot->stagger_ticks = 0;
    robot->stagger_total = 0;
    robot->stagger_progress = 0.0;
    robot->stagger_dir_x = 0.0;
    robot->stagger_dir_y = 0.0;
    robot->fall_progress = 0.0;
    robot->fall_dir_x = 0.0;
    robot->fall_dir_y = 0.0;
    robot->fall_angular_velocity = 0.0;
    robot->fall_ticks = 0;
    robot->fall_total = 0;
    robot->fall_contact_part = PART_INVALID;
    robot->ground_contact_mask = 0;
    robot->ground_impact_part = PART_INVALID;
    robot->ground_impact = 0.0;
    robot->ground_slide = 0.0;
    robot->ground_settle = 0.0;
    robot->wall_impulse = 0.0;
    robot->wall_flex = 0.0;
    robot->getup_state = CFA_GETUP_NONE;
    robot->getup_ticks = 0;
    robot->getup_total = 0;
    robot->getup_progress = 0.0;
    robot->getup_blocked = 0;
    robot->getup_force_x = 0.0;
    robot->getup_force_y = 0.0;
    robot->getup_force_z = 0.0;
    robot->getup_support_mask = 0;
    robot->getup_pressure = 0.0;
    robot->grounded_defense = 0.0;
    robot->grounded_roll = 0.0;
    robot->clinch_leverage = 0.0;
    robot->clinch_pressure = 0.0;
    robot->throw_torque = 0.0;
    robot->recent_damage_part = PART_INVALID;
    robot->recent_damage_raw = 0;
    robot->recent_damage_net = 0;
    robot->recent_damage_x = 0.0;
    robot->recent_damage_y = 0.0;
    robot->recent_damage_z = 0.0;
    robot->recent_damage_ticks = 0;
    robot->head_detached = 0;
    robot->head_detach_x = 0.0;
    robot->head_detach_y = 0.0;
    robot->head_detach_z = 0.0;
    robot->head_detach_vx = 0.0;
    robot->head_detach_vy = 0.0;
    robot->head_detach_vz = 0.0;
    robot->head_detach_spin = 0.0;
    robot->head_detach_ticks = 0;
    robot->method[0] = '\0';
}

static void init_fight(Fight *fight, uint32_t seed)
{
    init_robot(&fight->robot[0], 0);
    init_robot(&fight->robot[1], 1);
    init_crowd(&fight->crowd);
    fight->clinch = 0;
    fight->stuck_ticks = 0;
    fight->rng = seed == 0 ? 1u : seed;
}

static void recover_robot(RobotState *robot)
{
    int cap;

    robot->guard = 0;
    robot->retreating = 0;
    robot->advancing = 0;
    robot->circling = 0;
    robot->swing_foot = FOOT_NONE;
    robot->pivot_foot = FOOT_NONE;
    robot->pivot_angle = 0.0;
    robot->wall_braced = 0;
    robot->last_impact_part = PART_INVALID;
    robot->last_impact_damage = 0;
    if (robot->getup_force_x != 0.0 || robot->getup_force_y != 0.0 ||
        robot->getup_force_z != 0.0) {
        robot->getup_force_x *= 0.54;
        robot->getup_force_y *= 0.54;
        robot->getup_force_z *= 0.58;
        if (fabs(robot->getup_force_x) < 0.006) {
            robot->getup_force_x = 0.0;
        }
        if (fabs(robot->getup_force_y) < 0.006) {
            robot->getup_force_y = 0.0;
        }
        if (fabs(robot->getup_force_z) < 0.006) {
            robot->getup_force_z = 0.0;
        }
    }
    if (robot->getup_pressure > 0.0) {
        robot->getup_pressure *= 0.66;
        if (robot->getup_pressure < 0.02) {
            robot->getup_pressure = 0.0;
        }
    }
    robot->getup_support_mask = 0;
    if (robot->grounded_defense > 0.0) {
        robot->grounded_defense *= 0.70;
        if (robot->grounded_defense < 0.02) {
            robot->grounded_defense = 0.0;
        }
    }
    if (robot->grounded_roll != 0.0) {
        robot->grounded_roll *= 0.62;
        if (fabs(robot->grounded_roll) < 0.02) {
            robot->grounded_roll = 0.0;
        }
    }
    if (robot->clinch_leverage > 0.0) {
        robot->clinch_leverage *= 0.68;
        if (robot->clinch_leverage < 0.02) {
            robot->clinch_leverage = 0.0;
        }
    }
    if (robot->clinch_pressure > 0.0) {
        robot->clinch_pressure *= 0.66;
        if (robot->clinch_pressure < 0.02) {
            robot->clinch_pressure = 0.0;
        }
    }
    if (robot->throw_torque != 0.0) {
        robot->throw_torque *= 0.58;
        if (fabs(robot->throw_torque) < 0.02) {
            robot->throw_torque = 0.0;
        }
    }
    if (robot->recent_damage_ticks > 0) {
        robot->recent_damage_ticks--;
        if (robot->recent_damage_ticks == 0) {
            robot->recent_damage_part = PART_INVALID;
            robot->recent_damage_raw = 0;
            robot->recent_damage_net = 0;
        }
    }
    if (robot->head_detached) {
        robot->head_detach_ticks++;
        robot->head_detach_x += robot->head_detach_vx;
        robot->head_detach_y += robot->head_detach_vy;
        robot->head_detach_z = fmax(0.13, robot->head_detach_z + robot->head_detach_vz);
        robot->head_detach_vx *= 0.82;
        robot->head_detach_vy *= 0.82;
        robot->head_detach_vz = robot->head_detach_vz * 0.58 - 0.018;
        if (robot->head_detach_z <= 0.13 && robot->head_detach_vz < 0.0) {
            robot->head_detach_vz *= -0.22;
        }
        robot->head_detach_spin *= 0.88;
    }
    clear_block_state(robot);
    clear_guard_pose_state(robot);
    robot->attack_released = 0;
    if (robot->follow_through > 0.0) {
        robot->follow_through *= 0.50;
        if (robot->follow_through < 0.035) {
            robot->follow_through = 0.0;
        }
    }
    if (robot->recoil > 0.0) {
        robot->recoil *= 0.58;
        if (robot->recoil < 0.035) {
            robot->recoil = 0.0;
        }
    }
    if (robot->stagger_ticks > 0) {
        robot->stagger_ticks--;
        if (robot->stagger_total > 0) {
            robot->stagger_progress =
                1.0 - (double)robot->stagger_ticks /
                          (double)robot->stagger_total;
        }
        if (robot->stagger_ticks <= 0) {
            robot->stagger_state = ROBOT_STAGGER_NONE;
            robot->stagger_total = 0;
            robot->stagger_progress = 0.0;
        }
    }
    if (robot->ground_impact > 0.0) {
        robot->ground_impact *= 0.58;
        if (robot->ground_impact < 0.02) {
            robot->ground_impact = 0.0;
            robot->ground_impact_part = PART_INVALID;
        }
    }
    if (robot->ground_slide > 0.0) {
        robot->ground_slide *= 0.72;
        if (robot->ground_slide < 0.01) {
            robot->ground_slide = 0.0;
        }
    }
    if (robot->ground_settle > 0.0) {
        robot->ground_settle *= 0.70;
        if (robot->ground_settle < 0.02) {
            robot->ground_settle = 0.0;
            if (!robot->down) {
                robot->ground_contact_mask = 0;
            }
        }
    }
    if (robot->wall_impulse > 0.0) {
        robot->wall_impulse *= 0.62;
        if (robot->wall_impulse < 0.01) {
            robot->wall_impulse = 0.0;
        }
    }
    if (robot->wall_flex > 0.0) {
        robot->wall_flex *= 0.66;
        if (robot->wall_flex < 0.01) {
            robot->wall_flex = 0.0;
        }
    }
    if (!robot->down) {
        robot->fall_progress = 0.0;
        robot->fall_ticks = 0;
        robot->fall_total = 0;
        robot->fall_dir_x = 0.0;
        robot->fall_dir_y = 0.0;
        robot->fall_angular_velocity = 0.0;
        robot->fall_contact_part = PART_INVALID;
        robot->getup_state = CFA_GETUP_NONE;
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
        robot->getup_blocked = 0;
        robot->getup_support_mask = 0;
    }
    if (robot->strike_phase != ROBOT_STRIKE_PHASE_WINDUP &&
        robot->follow_through <= 0.0 && robot->recoil <= 0.0 &&
        robot->recovery_ticks <= 0) {
        robot->strike_phase = ROBOT_STRIKE_PHASE_IDLE;
    } else if (robot->recoil > robot->follow_through) {
        robot->strike_phase = ROBOT_STRIKE_PHASE_RECOIL;
    }
    if (robot->windup_ticks == 0) {
        robot->windup_progress = 0.0;
    }
    if (robot->down || robot->defeated) {
        clear_windup_state(robot);
        robot->recovery_ticks = 0;
    }

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

    if (robot->head_detached) {
        mark_defeat(robot, "dramatic knockout by head detachment");
        return 1;
    }
    if (robot->processor <= 0 || robot->integrity[PART_HEAD] <= 0 ||
        (robot->processor <= 20 && robot->shock >= 38)) {
        mark_defeat(robot, "processor kill by head module failure");
        return 1;
    }
    if (robot->integrity[PART_TORSO] <= 0 ||
        robot->integrity[PART_TORSO] <= 45 ||
        (robot->integrity[PART_TORSO] <= 70 && robot->shock >= 26)) {
        mark_defeat(robot, "technical knockout by torso power-bus collapse");
        return 1;
    }
    if (robot->shock >= 64) {
        mark_defeat(robot, "knockout by cranial watchdog reset");
        return 1;
    }
    if (robot->down && robot->shock >= 50 && robot->processor <= 80) {
        mark_defeat(robot, "knockout by failed get-up control");
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
        robot->scuff[part] = 100;
        robot->dent[part] = 100;
        robot->shock += 14;
        robot->stability -= 24;
        if (robot->stability < 0) {
            robot->stability = 0;
        }
        append_text(out, out_size, " %s detached.", part_name(part));
    }
}

static Vec3 damage_anchor_for_part(const RobotState *robot, BodyPart part)
{
    switch (part) {
    case PART_HEAD:
        return local_to_world_point(robot,
                                    vec3(0.06,
                                         robot->down ? 0.38 :
                                             ROBOT_HEAD_CENTER_HEIGHT_M,
                                         0.0));
    case PART_TORSO:
        return local_to_world_point(robot,
                                    vec3(0.04,
                                         robot->down ? 0.34 :
                                             ROBOT_CHEST_HEIGHT_M,
                                         0.0));
    case PART_L_ARM:
        return local_to_world_point(robot,
                                    vec3(0.18,
                                         robot->down ? 0.26 :
                                             ROBOT_REST_HAND_HEIGHT_M,
                                         -0.28));
    case PART_R_ARM:
        return local_to_world_point(robot,
                                    vec3(0.18,
                                         robot->down ? 0.26 :
                                             ROBOT_REST_HAND_HEIGHT_M,
                                         0.28));
    case PART_L_LEG:
        return local_to_world_point(robot,
                                    vec3(-0.18,
                                         robot->down ? 0.20 :
                                             ROBOT_KNEE_HEIGHT_M,
                                         -0.17));
    case PART_R_LEG:
        return local_to_world_point(robot,
                                    vec3(-0.18,
                                         robot->down ? 0.20 :
                                             ROBOT_KNEE_HEIGHT_M,
                                         0.17));
    default:
        return vec3(robot->x, robot->y, robot->down ? 0.32 : ROBOT_CHEST_HEIGHT_M);
    }
}

static void set_recent_damage_site(RobotState *robot, BodyPart part,
                                   Vec3 point)
{
    if (part < 0 || part >= PART_COUNT) {
        return;
    }
    robot->recent_damage_part = part;
    robot->recent_damage_x = point.x;
    robot->recent_damage_y = point.y;
    robot->recent_damage_z = point.z;
    robot->recent_damage_ticks = 4;
}

static void record_surface_damage(RobotState *robot, BodyPart part,
                                  int raw_damage, int net_damage)
{
    int scuff_gain;
    int dent_gain;

    if (part < 0 || part >= PART_COUNT) {
        return;
    }

    scuff_gain = clamp_int(raw_damage / 2 + net_damage, 1, 30);
    dent_gain = 0;
    if (raw_damage >= 18 || net_damage >= 12) {
        dent_gain = clamp_int((raw_damage - 10) / 4 + net_damage / 8, 1, 22);
    }
    if (raw_damage >= 30) {
        dent_gain += 6;
    }

    robot->scuff[part] = clamp_int(robot->scuff[part] + scuff_gain, 0, 100);
    robot->dent[part] = clamp_int(robot->dent[part] + dent_gain, 0, 100);
    robot->last_impact_part = part;
    robot->last_impact_damage = clamp_int(raw_damage + net_damage, 0, 160);
    robot->recent_damage_part = part;
    robot->recent_damage_raw = raw_damage;
    robot->recent_damage_net = net_damage;
    set_recent_damage_site(robot, part, damage_anchor_for_part(robot, part));
}

static void detach_head_for_knockout(RobotState *robot, double nx, double ny,
                                     double impulse, double yaw_impulse,
                                     int raw_damage,
                                     const StrikeContact *contact,
                                     char *out, size_t out_size)
{
    int trauma;
    Vec3 anchor;

    if (robot->head_detached) {
        return;
    }
    if (contact != NULL && contact->guarded && raw_damage < 48) {
        return;
    }
    if (contact != NULL && contact->glancing && raw_damage < 52) {
        return;
    }

    trauma = raw_damage +
             (int)(fabs(yaw_impulse) * 840.0) +
             (int)(impulse * 90.0) +
             (100 - clamp_int(robot->integrity[PART_HEAD], 0, 100)) / 2 +
             robot->dent[PART_HEAD] / 3;
    if (trauma < ROBOT_HEAD_DETACH_TRAUMA_THRESHOLD ||
        (robot->integrity[PART_HEAD] > 18 && raw_damage < 45)) {
        return;
    }

    anchor = damage_anchor_for_part(robot, PART_HEAD);
    robot->head_detached = 1;
    robot->head_detach_x = anchor.x;
    robot->head_detach_y = anchor.y;
    robot->head_detach_z = fmax(anchor.z, ROBOT_HEAD_CENTER_HEIGHT_M * 0.55);
    robot->head_detach_vx = robot->vx + nx * clamp_double(impulse * 0.90, 0.18, 0.52);
    robot->head_detach_vy = robot->vy + ny * clamp_double(impulse * 0.90, 0.18, 0.52);
    robot->head_detach_vz = 0.16 + clamp_double(raw_damage / 180.0, 0.0, 0.20);
    robot->head_detach_spin = clamp_abs(yaw_impulse * 7.0 +
                                            (nx >= 0.0 ? 0.8 : -0.8),
                                        4.0);
    robot->head_detach_ticks = 0;
    robot->integrity[PART_HEAD] = 0;
    robot->processor = 0;
    robot->shock = 140;
    robot->scuff[PART_HEAD] = 100;
    robot->dent[PART_HEAD] = 100;
    set_recent_damage_site(robot, PART_HEAD, anchor);
    mark_defeat(robot, "dramatic knockout by head detachment");
    append_text(out, out_size,
                " HEAD DETACH KO trauma %d, head tumbles %.2fm/t;",
                trauma,
                sqrt(robot->head_detach_vx * robot->head_detach_vx +
                     robot->head_detach_vy * robot->head_detach_vy));
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

static void clear_windup_state(RobotState *robot)
{
    robot->windup_command.id = CMD_INVALID;
    robot->windup_command.target = PART_INVALID;
    robot->windup_command.line = 0;
    robot->windup_ticks = 0;
    robot->windup_total = 0;
    robot->windup_progress = 0.0;
}

static int attack_windup_ticks(CommandId id)
{
    switch (id) {
    case CMD_L_JAB:
        return 1;
    case CMD_R_CROSS:
    case CMD_L_HOOK:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
    case CMD_ELBOW:
        return 1;
    case CMD_LOW_KICK:
    case CMD_KNEE:
        return 1;
    case CMD_HIGH_KICK:
    case CMD_THROW:
    case CMD_STOMP:
        return 2;
    case CMD_CLINCH:
        return 1;
    default:
        return 0;
    }
}

static int attack_recovery_ticks(CommandId id)
{
    switch (id) {
    case CMD_L_JAB:
        return 0;
    case CMD_R_CROSS:
    case CMD_L_HOOK:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
    case CMD_ELBOW:
    case CMD_LOW_KICK:
    case CMD_KNEE:
        return 1;
    case CMD_HIGH_KICK:
    case CMD_THROW:
    case CMD_STOMP:
        return 2;
    case CMD_CLINCH:
        return 1;
    default:
        return 0;
    }
}

static double current_windup_progress(const RobotState *robot)
{
    if (robot->windup_total <= 0) {
        return robot->attack_released ? 1.0 : 0.0;
    }
    return clamp_double((double)(robot->windup_total - robot->windup_ticks) /
                            (double)robot->windup_total,
                        0.0, 1.0);
}

static void set_strike_motion(RobotState *robot, int phase,
                              double follow_through, double recoil)
{
    robot->strike_phase = phase;
    robot->follow_through = clamp_double(follow_through, 0.0, 1.0);
    robot->recoil = clamp_double(recoil, 0.0, 1.0);
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
    if (robot->head_detached) {
        score -= 180;
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
    case METRIC_SELF_GUARD:
        return self->guard ? 1.0 : 0.0;
    case METRIC_OPP_GUARD:
        return opp->guard ? 1.0 : 0.0;
    case METRIC_SELF_GETUP:
        return (double)self->getup_state;
    case METRIC_OPP_GETUP:
        return (double)opp->getup_state;
    case METRIC_SELF_GETUP_PRESSURE:
        return self->getup_pressure;
    case METRIC_OPP_GETUP_PRESSURE:
        return opp->getup_pressure;
    case METRIC_SELF_BALANCE_OFFSET:
        return fabs(self->balance_offset);
    case METRIC_OPP_BALANCE_OFFSET:
        return fabs(opp->balance_offset);
    case METRIC_SELF_WALL_PRESSURE:
        return self->wall_impulse + self->wall_flex;
    case METRIC_OPP_WALL_PRESSURE:
        return opp->wall_impulse + opp->wall_flex;
    case METRIC_SELF_CLINCH_PRESSURE:
        return self->clinch_pressure;
    case METRIC_OPP_CLINCH_PRESSURE:
        return opp->clinch_pressure;
    case METRIC_SELF_RECENT_DAMAGE:
        return (double)(self->recent_damage_raw + self->recent_damage_net);
    case METRIC_OPP_RECENT_DAMAGE:
        return (double)(opp->recent_damage_raw + opp->recent_damage_net);
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

static Intent build_intent(const Program *program, Fight *fight,
                           int side, size_t *cursor, char *out,
                           size_t out_size)
{
    Intent intent;
    Command command;
    const MoveSpec *spec;
    RobotState *robot = &fight->robot[side];

    memset(&intent, 0, sizeof(intent));

    if (robot->windup_ticks > 0 &&
        robot->windup_command.id > CMD_INVALID &&
        robot->windup_command.id < CMD_COUNT) {
        command = robot->windup_command;
        spec = &move_specs[command.id];
        intent.command = command;
        intent.spec = spec;
        intent.active = 0;

        if (robot->down || !mode_available(robot, spec->use_mode)) {
            clear_windup_state(robot);
        } else {
            robot->windup_ticks--;
            robot->windup_progress = current_windup_progress(robot);
            if (robot->windup_ticks > 0) {
                set_strike_motion(robot, ROBOT_STRIKE_PHASE_WINDUP,
                                  robot->windup_progress * 0.25, 0.0);
                snprintf(intent.note, sizeof(intent.note),
                         "%s loading strike %.0f%%",
                         spec->name, robot->windup_progress * 100.0);
                return intent;
            }

            intent.active = spec->is_attack;
            robot->attack_released = intent.active;
            robot->windup_progress = 1.0;
            set_strike_motion(robot, ROBOT_STRIKE_PHASE_RELEASE, 0.34, 0.0);
            snprintf(intent.note, sizeof(intent.note),
                     "%s releases loaded strike", spec->name);
            return intent;
        }
    }

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

    if (spec->is_attack && robot->recovery_ticks > 0) {
        robot->recovery_ticks--;
        intent.command.id = CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_GUARD];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "%s held by strike recovery", spec->name);
        return intent;
    }

    if (robot->heat >= 130 && command.id != CMD_STAND) {
        intent.command.id = CMD_GUARD;
        intent.command.target = PART_INVALID;
        intent.spec = &move_specs[CMD_GUARD];
        intent.active = 0;
        snprintf(intent.note, sizeof(intent.note),
                 "thermal clamp engaged; command converted to GUARD");
        return intent;
    }

    if (spec->is_attack) {
        int windup_ticks = attack_windup_ticks(command.id);

        if (windup_ticks > 0) {
            robot->windup_command = command;
            robot->windup_ticks = windup_ticks;
            robot->windup_total = windup_ticks;
            robot->windup_progress =
                clamp_double(1.0 / (double)(windup_ticks + 1), 0.25, 0.50);
            set_strike_motion(robot, ROBOT_STRIKE_PHASE_WINDUP,
                              robot->windup_progress * 0.20, 0.0);
            intent.active = 0;
            snprintf(intent.note, sizeof(intent.note),
                     "%s begins wind-up %.0f%%",
                     spec->name, robot->windup_progress * 100.0);
            return intent;
        }
    }

    return intent;
}

static const char *stagger_name(int state)
{
    switch (state) {
    case ROBOT_STAGGER_FLINCH:
        return "flinch";
    case ROBOT_STAGGER_STEP_BACK:
        return "step-back";
    case ROBOT_STAGGER_STUMBLE:
        return "stumble";
    case ROBOT_STAGGER_KNEEL:
        return "kneel";
    case ROBOT_STAGGER_COLLAPSE:
        return "collapse";
    default:
        return "none";
    }
}

static Vec3 planar_direction_or(double x, double y, double fallback_x,
                                double fallback_y)
{
    double len = sqrt(x * x + y * y);

    if (len > 0.0001) {
        return vec3(x / len, y / len, 0.0);
    }
    len = sqrt(fallback_x * fallback_x + fallback_y * fallback_y);
    if (len > 0.0001) {
        return vec3(fallback_x / len, fallback_y / len, 0.0);
    }
    return vec3(1.0, 0.0, 0.0);
}

static void set_stagger(RobotState *robot, int state, double dir_x,
                        double dir_y, char *out, size_t out_size)
{
    Vec3 dir;
    int total;

    if (state <= ROBOT_STAGGER_NONE || robot->defeated) {
        return;
    }
    if (robot->down && state < ROBOT_STAGGER_COLLAPSE) {
        return;
    }

    dir = planar_direction_or(dir_x, dir_y, robot->vx, robot->vy);
    total = 1 + state / 2;
    if (state >= ROBOT_STAGGER_STUMBLE) {
        total++;
    }

    if (state < robot->stagger_state &&
        robot->stagger_ticks > 0) {
        return;
    }

    robot->stagger_state = state;
    robot->stagger_ticks = clamp_int(total, 1, 5);
    robot->stagger_total = robot->stagger_ticks;
    robot->stagger_progress = 0.12;
    robot->stagger_dir_x = dir.x;
    robot->stagger_dir_y = dir.y;

    if (state >= ROBOT_STAGGER_STEP_BACK) {
        double shove = 0.018 + state * 0.008;
        robot->vx += dir.x * shove;
        robot->vy += dir.y * shove;
    }
    if (state >= ROBOT_STAGGER_STUMBLE) {
        robot->angular_velocity =
            clamp_abs(robot->angular_velocity +
                          (dir.y * cos(robot->facing) -
                           dir.x * sin(robot->facing)) * 0.050,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    }

    append_text(out, out_size, " stagger %s;", stagger_name(state));
}

static void fall_down_direction(RobotState *robot, const char *reason,
                                double dir_x, double dir_y,
                                BodyPart contact_part,
                                double angular_velocity,
                                char *out, size_t out_size)
{
    if (!robot->down) {
        Vec3 dir = planar_direction_or(dir_x, dir_y,
                                       -cos(robot->facing),
                                       -sin(robot->facing));
        robot->down = 1;
        robot->down_ticks = 4;
        robot->fall_progress = 0.08;
        robot->fall_ticks = 0;
        robot->fall_total = contact_part == PART_HEAD ? 3 : 4;
        robot->fall_dir_x = dir.x;
        robot->fall_dir_y = dir.y;
        robot->fall_angular_velocity =
            clamp_abs(angular_velocity,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        if (fabs(robot->fall_angular_velocity) < 0.045) {
            double lateral = -sin(robot->facing) * dir.x +
                             cos(robot->facing) * dir.y;
            robot->fall_angular_velocity =
                clamp_abs(lateral * 0.16,
                          ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        }
        robot->fall_contact_part = contact_part;
        robot->ground_contact_mask = 0;
        robot->ground_impact = 0.0;
        robot->ground_slide = 0.0;
        robot->ground_settle = 0.0;
        robot->ground_impact_part = PART_INVALID;
        robot->getup_state = CFA_GETUP_STUNNED;
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
        robot->getup_blocked = 1;
        robot->stability = clamp_int(robot->stability - 24, 0, max_stability(robot));
        robot->shock = clamp_int(robot->shock + 8, 0, 140);
        set_stagger(robot, ROBOT_STAGGER_COLLAPSE, dir.x, dir.y,
                    out, out_size);
        append_text(out, out_size,
                    " begins staged fall by %s through %s.",
                    reason, part_name(contact_part));
    }
}

static int ground_mask_for_fall(const RobotState *robot)
{
    int mask = CFA_GROUND_TORSO;

    switch (robot->fall_contact_part) {
    case PART_HEAD:
        mask |= CFA_GROUND_HEAD | CFA_GROUND_L_HAND | CFA_GROUND_R_HAND;
        break;
    case PART_L_ARM:
        mask |= CFA_GROUND_L_HAND | CFA_GROUND_L_KNEE | CFA_GROUND_L_SHIN;
        break;
    case PART_R_ARM:
        mask |= CFA_GROUND_R_HAND | CFA_GROUND_R_KNEE | CFA_GROUND_R_SHIN;
        break;
    case PART_L_LEG:
        mask |= CFA_GROUND_L_KNEE | CFA_GROUND_L_SHIN | CFA_GROUND_L_FOOT;
        break;
    case PART_R_LEG:
        mask |= CFA_GROUND_R_KNEE | CFA_GROUND_R_SHIN | CFA_GROUND_R_FOOT;
        break;
    case PART_TORSO:
    default:
        mask |= CFA_GROUND_L_HAND | CFA_GROUND_R_HAND |
                CFA_GROUND_L_KNEE | CFA_GROUND_R_KNEE;
        break;
    }

    if (robot->wall_braced) {
        mask |= CFA_GROUND_TORSO;
    }
    if (robot->detached[PART_L_ARM]) {
        mask &= ~CFA_GROUND_L_HAND;
    }
    if (robot->detached[PART_R_ARM]) {
        mask &= ~CFA_GROUND_R_HAND;
    }
    if (robot->detached[PART_L_LEG]) {
        mask &= ~(CFA_GROUND_L_KNEE | CFA_GROUND_L_SHIN | CFA_GROUND_L_FOOT);
    }
    if (robot->detached[PART_R_LEG]) {
        mask &= ~(CFA_GROUND_R_KNEE | CFA_GROUND_R_SHIN | CFA_GROUND_R_FOOT);
    }
    return mask;
}

static BodyPart ground_primary_part_from_mask(int mask)
{
    if (mask & CFA_GROUND_HEAD) {
        return PART_HEAD;
    }
    if (mask & CFA_GROUND_TORSO) {
        return PART_TORSO;
    }
    if (mask & (CFA_GROUND_L_HAND | CFA_GROUND_R_HAND)) {
        return (mask & CFA_GROUND_L_HAND) ? PART_L_ARM : PART_R_ARM;
    }
    if (mask & (CFA_GROUND_L_KNEE | CFA_GROUND_L_SHIN | CFA_GROUND_L_FOOT)) {
        return PART_L_LEG;
    }
    if (mask & (CFA_GROUND_R_KNEE | CFA_GROUND_R_SHIN | CFA_GROUND_R_FOOT)) {
        return PART_R_LEG;
    }
    return PART_TORSO;
}

static void apply_ground_landing(RobotState *robot, int side,
                                 char *out, size_t out_size)
{
    double speed = sqrt(robot->vx * robot->vx + robot->vy * robot->vy);
    double angular = fabs(robot->fall_angular_velocity) +
                     fabs(robot->angular_velocity) * 0.35;
    int mask = ground_mask_for_fall(robot);
    BodyPart part = robot->fall_contact_part == PART_INVALID ?
                    ground_primary_part_from_mask(mask) :
                    robot->fall_contact_part;
    int raw = clamp_int((int)(speed * 36.0 + angular * 45.0) +
                            robot->shock / 14 + 4,
                        3, 34);
    int net = clamp_int(raw / 3, 1, 14);

    robot->ground_contact_mask = mask;
    robot->ground_impact_part = part;
    robot->ground_impact = clamp_double((double)raw / 34.0, 0.10, 1.0);
    robot->ground_slide = clamp_double(speed, 0.0, 0.75);
    robot->ground_settle = 1.0;
    robot->vx *= 0.34;
    robot->vy *= 0.34;
    robot->angular_velocity *= 0.48;
    robot->shock = clamp_int(robot->shock + raw / 3, 0, 140);
    robot->stability = clamp_int(robot->stability - raw / 4, 0,
                                 max_stability(robot));
    record_surface_damage(robot, part, raw, net);
    append_text(out, out_size,
                " R%d grounds on %s impact %.0f%% slide %.2fm/t;",
                side + 1, part_name(part), robot->ground_impact * 100.0,
                robot->ground_slide);
}

static void update_ground_contact(RobotState *robot, int side,
                                  char *out, size_t out_size)
{
    if (!robot->down) {
        return;
    }

    if (robot->fall_total > 0 && robot->fall_ticks < robot->fall_total) {
        robot->fall_ticks++;
        robot->fall_progress =
            clamp_double((double)robot->fall_ticks /
                             (double)robot->fall_total,
                         0.08, 1.0);
        robot->angular_velocity =
            clamp_abs(robot->angular_velocity +
                          robot->fall_angular_velocity * 0.13,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        if (robot->fall_ticks >= robot->fall_total) {
            apply_ground_landing(robot, side, out, out_size);
        }
        return;
    }

    if (robot->ground_contact_mask == 0) {
        robot->ground_contact_mask = ground_mask_for_fall(robot);
        robot->ground_impact_part =
            ground_primary_part_from_mask(robot->ground_contact_mask);
        robot->ground_settle = fmax(robot->ground_settle, 0.45);
    }
    if (robot->ground_slide > 0.02) {
        robot->vx *= 0.72;
        robot->vy *= 0.72;
        robot->ground_slide =
            clamp_double(sqrt(robot->vx * robot->vx + robot->vy * robot->vy),
                         0.0, 0.75);
    } else {
        robot->vx *= 0.55;
        robot->vy *= 0.55;
    }
    robot->angular_velocity *= 0.62;
}

static void record_wall_contact(RobotState *robot, int side,
                                double nx, double ny, double impulse,
                                char *out, size_t out_size)
{
    double clamped = clamp_double(impulse, 0.0, 1.20);

    if (clamped <= 0.0) {
        return;
    }
    robot->wall_nx = nx;
    robot->wall_ny = ny;
    robot->wall_braced = robot->down || clamped > 0.12 ? 1 : robot->wall_braced;
    if (clamped > robot->wall_impulse) {
        robot->wall_impulse = clamped;
    }
    if (clamped * 0.42 > robot->wall_flex) {
        robot->wall_flex = clamp_double(clamped * 0.42, 0.03, 0.42);
    }
    if (clamped > 0.28) {
        append_text(out, out_size, "R%d cage flex %.0f%%. ",
                    side + 1, robot->wall_flex * 100.0);
    }
}

static const char *getup_state_name(int state)
{
    switch (state) {
    case CFA_GETUP_STUNNED:
        return "stunned";
    case CFA_GETUP_ROLL_SIDE:
        return "roll-to-side";
    case CFA_GETUP_BRACE_HAND:
        return "brace-hand";
    case CFA_GETUP_KNEE_UNDER:
        return "knee-under-body";
    case CFA_GETUP_PUSH_UP:
        return "push-up";
    case CFA_GETUP_CROUCH:
        return "crouch";
    case CFA_GETUP_STAND:
        return "stand";
    case CFA_GETUP_GUARD_RESET:
        return "guard-reset";
    default:
        return "idle";
    }
}

static int getup_stage_duration(const Fight *fight, int side, int state)
{
    const RobotState *robot = &fight->robot[side];
    const RobotState *opponent = &fight->robot[side == 0 ? 1 : 0];
    double dx = opponent->x - robot->x;
    double dy = opponent->y - robot->y;
    double center = sqrt(dx * dx + dy * dy);
    int duration;
    int arm_penalty = 0;
    int leg_penalty = 0;

    switch (state) {
    case CFA_GETUP_STUNNED:
        duration = 1;
        break;
    case CFA_GETUP_ROLL_SIDE:
    case CFA_GETUP_BRACE_HAND:
    case CFA_GETUP_KNEE_UNDER:
        duration = 2;
        break;
    case CFA_GETUP_PUSH_UP:
    case CFA_GETUP_CROUCH:
        duration = 2;
        break;
    case CFA_GETUP_STAND:
    case CFA_GETUP_GUARD_RESET:
    default:
        duration = 1;
        break;
    }

    if (robot->shock > 58) {
        duration++;
    }
    if (robot->heat > 120) {
        duration++;
    }
    if (center < 0.95 && !opponent->down) {
        duration++;
    }
    if (robot->detached[PART_L_ARM] || robot->integrity[PART_L_ARM] < 32) {
        arm_penalty++;
    }
    if (robot->detached[PART_R_ARM] || robot->integrity[PART_R_ARM] < 32) {
        arm_penalty++;
    }
    if (robot->detached[PART_L_LEG] || robot->integrity[PART_L_LEG] < 38) {
        leg_penalty++;
    }
    if (robot->detached[PART_R_LEG] || robot->integrity[PART_R_LEG] < 38) {
        leg_penalty++;
    }
    if (state == CFA_GETUP_BRACE_HAND || state == CFA_GETUP_PUSH_UP) {
        duration += arm_penalty;
    }
    if (state == CFA_GETUP_KNEE_UNDER || state == CFA_GETUP_CROUCH ||
        state == CFA_GETUP_STAND) {
        duration += leg_penalty;
    }
    return clamp_int(duration, 1, 6);
}

static int getup_next_state(int state)
{
    switch (state) {
    case CFA_GETUP_NONE:
        return CFA_GETUP_STUNNED;
    case CFA_GETUP_STUNNED:
        return CFA_GETUP_ROLL_SIDE;
    case CFA_GETUP_ROLL_SIDE:
        return CFA_GETUP_BRACE_HAND;
    case CFA_GETUP_BRACE_HAND:
        return CFA_GETUP_KNEE_UNDER;
    case CFA_GETUP_KNEE_UNDER:
        return CFA_GETUP_PUSH_UP;
    case CFA_GETUP_PUSH_UP:
        return CFA_GETUP_CROUCH;
    case CFA_GETUP_CROUCH:
        return CFA_GETUP_STAND;
    case CFA_GETUP_STAND:
        return CFA_GETUP_GUARD_RESET;
    default:
        return CFA_GETUP_GUARD_RESET;
    }
}

static double part_support_quality(const RobotState *robot, BodyPart part)
{
    if (part < 0 || part >= PART_COUNT || robot->detached[part]) {
        return 0.0;
    }
    return clamp_double((double)robot->integrity[part] /
                            (double)part_initial[part],
                        0.0, 1.0);
}

static int getup_support_mask_for_state(const RobotState *robot, int state)
{
    int mask = 0;

    switch (state) {
    case CFA_GETUP_ROLL_SIDE:
        mask = CFA_GROUND_TORSO;
        if (part_support_quality(robot, PART_L_ARM) > 0.0) {
            mask |= CFA_GROUND_L_HAND;
        }
        if (part_support_quality(robot, PART_R_ARM) > 0.0) {
            mask |= CFA_GROUND_R_HAND;
        }
        break;
    case CFA_GETUP_BRACE_HAND:
    case CFA_GETUP_PUSH_UP:
        if (part_support_quality(robot, PART_L_ARM) > 0.0) {
            mask |= CFA_GROUND_L_HAND;
        }
        if (part_support_quality(robot, PART_R_ARM) > 0.0) {
            mask |= CFA_GROUND_R_HAND;
        }
        if (mask == 0) {
            mask |= CFA_GETUP_LEG_SUPPORT;
        }
        break;
    case CFA_GETUP_KNEE_UNDER:
        if (part_support_quality(robot, PART_L_LEG) > 0.0) {
            mask |= CFA_GROUND_L_KNEE | CFA_GROUND_L_FOOT;
        }
        if (part_support_quality(robot, PART_R_LEG) > 0.0) {
            mask |= CFA_GROUND_R_KNEE | CFA_GROUND_R_FOOT;
        }
        if (mask == 0) {
            mask = CFA_GETUP_HAND_SUPPORT;
        }
        break;
    case CFA_GETUP_CROUCH:
    case CFA_GETUP_STAND:
    case CFA_GETUP_GUARD_RESET:
        if (part_support_quality(robot, PART_L_LEG) > 0.0) {
            mask |= CFA_GROUND_L_FOOT;
        }
        if (part_support_quality(robot, PART_R_LEG) > 0.0) {
            mask |= CFA_GROUND_R_FOOT;
        }
        break;
    default:
        mask = robot->ground_contact_mask;
        break;
    }

    return mask;
}

static double getup_support_quality(const RobotState *robot, int state,
                                    int mask)
{
    double quality = 0.0;
    double weight = 0.0;

    if (mask & CFA_GROUND_L_HAND) {
        quality += part_support_quality(robot, PART_L_ARM) * 0.50;
        weight += 0.50;
    }
    if (mask & CFA_GROUND_R_HAND) {
        quality += part_support_quality(robot, PART_R_ARM) * 0.50;
        weight += 0.50;
    }
    if (mask & (CFA_GROUND_L_KNEE | CFA_GROUND_L_SHIN | CFA_GROUND_L_FOOT)) {
        quality += part_support_quality(robot, PART_L_LEG) * 0.72;
        weight += 0.72;
    }
    if (mask & (CFA_GROUND_R_KNEE | CFA_GROUND_R_SHIN | CFA_GROUND_R_FOOT)) {
        quality += part_support_quality(robot, PART_R_LEG) * 0.72;
        weight += 0.72;
    }
    if (mask & CFA_GROUND_TORSO) {
        quality += part_support_quality(robot, PART_TORSO) * 0.28;
        weight += 0.28;
    }

    if (weight <= 0.0) {
        return 0.0;
    }
    quality /= weight;
    if ((state == CFA_GETUP_BRACE_HAND || state == CFA_GETUP_PUSH_UP) &&
        (mask & CFA_GETUP_HAND_SUPPORT) == 0) {
        quality *= 0.48;
    }
    if ((state == CFA_GETUP_CROUCH || state == CFA_GETUP_STAND) &&
        (mask & CFA_GETUP_FOOT_SUPPORT) == 0) {
        quality *= 0.35;
    }
    return clamp_double(quality, 0.0, 1.0);
}

static double opponent_ground_pressure(const Fight *fight, int side)
{
    const RobotState *robot = &fight->robot[side];
    const RobotState *opponent = &fight->robot[side == 0 ? 1 : 0];
    double dx = opponent->x - robot->x;
    double dy = opponent->y - robot->y;
    double center = sqrt(dx * dx + dy * dy);

    if (opponent->down) {
        return 0.0;
    }
    return clamp_double((1.05 - center) / 0.78, 0.0, 1.0);
}

static double apply_getup_ground_force(Fight *fight, int side, int state,
                                       char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    double away_x;
    double away_y;
    double pressure = opponent_ground_pressure(fight, side);
    int mask = getup_support_mask_for_state(robot, state);
    double quality = getup_support_quality(robot, state, mask);
    double stage_lift;
    double lateral_roll;

    vector_to_opponent(fight, side, &away_x, &away_y);
    away_x = -away_x;
    away_y = -away_y;

    stage_lift = 0.04;
    if (state == CFA_GETUP_BRACE_HAND) {
        stage_lift = 0.13;
    } else if (state == CFA_GETUP_KNEE_UNDER) {
        stage_lift = 0.18;
    } else if (state == CFA_GETUP_PUSH_UP) {
        stage_lift = 0.27;
    } else if (state == CFA_GETUP_CROUCH) {
        stage_lift = 0.22;
    } else if (state == CFA_GETUP_STAND || state == CFA_GETUP_GUARD_RESET) {
        stage_lift = 0.16;
    }

    lateral_roll = (side == 0 ? -1.0 : 1.0) *
                   clamp_double((1.0 - quality) * 0.10 + pressure * 0.05,
                                0.0, 0.16);
    robot->getup_support_mask = mask;
    robot->ground_contact_mask |= mask;
    robot->getup_pressure = pressure;
    robot->getup_force_x = away_x * stage_lift * (0.42 + quality * 0.46);
    robot->getup_force_y = away_y * stage_lift * (0.42 + quality * 0.46);
    robot->getup_force_z = stage_lift * quality * (1.0 - pressure * 0.32);
    robot->grounded_roll = lateral_roll;
    robot->grounded_defense = clamp_double(0.24 + quality * 0.46 +
                                               pressure * 0.10,
                                           0.0, 1.0);

    robot->vx += robot->getup_force_x * 0.22;
    robot->vy += robot->getup_force_y * 0.22;
    robot->angular_velocity =
        clamp_abs(robot->angular_velocity + lateral_roll * 0.30,
                  ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    if (quality < 0.24 || pressure > 0.82) {
        robot->getup_blocked = 1;
        append_text(out, out_size,
                    "R%d GETUP blocked support %.0f%% pressure %.0f%%. ",
                    side + 1, quality * 100.0, pressure * 100.0);
    }
    return quality;
}

static void stand_robot(Fight *fight, int side, char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    int state;
    int duration;
    int chance;
    int roll;
    double support_quality;

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

    if (robot->fall_total > 0 && robot->fall_progress < 1.0) {
        append_text(out, out_size, "R%d STAND waiting for fall landing. ",
                    side + 1);
        return;
    }

    if (!mode_available(robot, USE_ANY_LEG)) {
        append_text(out, out_size, "R%d STAND fails; lower frame unavailable. ", side + 1);
        robot->getup_blocked = 1;
        return;
    }

    if (robot->getup_state == CFA_GETUP_NONE) {
        robot->getup_state = CFA_GETUP_STUNNED;
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
    }

    state = robot->getup_state;
    robot->getup_blocked = 0;
    duration = getup_stage_duration(fight, side, state);
    support_quality = apply_getup_ground_force(fight, side, state, out,
                                               out_size);
    if (support_quality < 0.34 &&
        (state == CFA_GETUP_BRACE_HAND || state == CFA_GETUP_PUSH_UP ||
         state == CFA_GETUP_STAND)) {
        duration = clamp_int(duration + 1, 1, 7);
    }
    if (robot->getup_total != duration) {
        robot->getup_total = duration;
        robot->getup_ticks = 0;
    }
    robot->getup_ticks++;
    robot->getup_progress =
        clamp_double((double)robot->getup_ticks / (double)duration,
                     0.0, 1.0);

    if (robot->getup_ticks < duration) {
        append_text(out, out_size, "R%d GETUP %s %.0f%%. ",
                    side + 1, getup_state_name(state),
                    robot->getup_progress * 100.0);
        if (state == CFA_GETUP_BRACE_HAND || state == CFA_GETUP_PUSH_UP) {
            robot->ground_contact_mask |= robot->getup_support_mask != 0 ?
                                          robot->getup_support_mask :
                                          CFA_GROUND_L_HAND | CFA_GROUND_R_HAND;
        }
        if (state == CFA_GETUP_KNEE_UNDER || state == CFA_GETUP_CROUCH) {
            robot->ground_contact_mask |= robot->getup_support_mask != 0 ?
                                          robot->getup_support_mask :
                                          CFA_GROUND_L_KNEE | CFA_GROUND_R_KNEE |
                                          CFA_GROUND_L_FOOT | CFA_GROUND_R_FOOT;
        }
        return;
    }

    if (state != CFA_GETUP_GUARD_RESET) {
        robot->getup_state = getup_next_state(state);
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
        append_text(out, out_size, "R%d GETUP shifts to %s. ",
                    side + 1, getup_state_name(robot->getup_state));
        return;
    }

    chance = 54 + robot->stability / 3 - robot->heat / 12 -
             robot->shock / 18;
    chance += (int)(support_quality * 18.0) - 8;
    chance -= (int)(robot->getup_pressure * 16.0);
    if (robot->ground_slide > 0.08) {
        chance -= 8;
    }
    chance = clamp_int(chance, 12, 94);
    roll = rng_range(&fight->rng, 1, 100);
    if (roll <= chance) {
        robot->down = 0;
        robot->fall_progress = 0.0;
        robot->fall_ticks = 0;
        robot->fall_total = 0;
        robot->fall_dir_x = 0.0;
        robot->fall_dir_y = 0.0;
        robot->fall_angular_velocity = 0.0;
        robot->fall_contact_part = PART_INVALID;
        robot->stagger_state = ROBOT_STAGGER_NONE;
        robot->stagger_ticks = 0;
        robot->stagger_total = 0;
        robot->stagger_progress = 0.0;
        robot->ground_contact_mask = 0;
        robot->ground_impact = 0.0;
        robot->ground_slide = 0.0;
        robot->ground_settle = 0.0;
        robot->ground_impact_part = PART_INVALID;
        robot->getup_state = CFA_GETUP_NONE;
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
        robot->getup_blocked = 0;
        robot->stability = clamp_int(robot->stability + 30, 0, max_stability(robot));
        place_feet_from_body(robot);
        append_text(out, out_size, "R%d STAND recovers upright (roll %d/%d). ",
                    side + 1, roll, chance);
    } else {
        robot->shock = clamp_int(robot->shock + 2, 0, 140);
        robot->getup_state = CFA_GETUP_BRACE_HAND;
        robot->getup_ticks = 0;
        robot->getup_total = 0;
        robot->getup_progress = 0.0;
        append_text(out, out_size, "R%d STAND fails final push (roll %d/%d). ",
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
    double dir_x = 0.0;
    double dir_y = 0.0;

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
        if (robot->down) {
            double nx;
            double ny;
            double side_roll = side == 0 ? -1.0 : 1.0;

            vector_to_opponent(fight, side, &nx, &ny);
            robot->grounded_defense = clamp_double(
                0.40 + part_support_quality(robot, PART_TORSO) * 0.18 +
                    fmax(part_support_quality(robot, PART_L_ARM),
                         part_support_quality(robot, PART_R_ARM)) * 0.26,
                0.15, 0.86);
            robot->grounded_roll = side_roll *
                                   clamp_double(0.04 +
                                                    robot->grounded_defense * 0.10,
                                                0.04, 0.16);
            robot->ground_contact_mask |=
                (part_support_quality(robot, PART_L_ARM) > 0.0 ?
                    CFA_GROUND_L_HAND : 0) |
                (part_support_quality(robot, PART_R_ARM) > 0.0 ?
                    CFA_GROUND_R_HAND : 0) |
                CFA_GROUND_TORSO;
            robot->vx -= nx * 0.025;
            robot->vy -= ny * 0.025;
            robot->angular_velocity =
                clamp_abs(robot->angular_velocity + robot->grounded_roll * 0.26,
                          ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
            append_text(out, out_size,
                        "R%d GROUNDED GUARD covers %.0f%% and rolls off-line. ",
                        side + 1, robot->grounded_defense * 100.0);
            return;
        }
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
        dir_x = nx;
        dir_y = ny;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx += nx * impulse;
        robot->vy += ny * impulse;
        append_text(out, out_size, "R%d ADVANCE impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_RETREAT:
        robot->retreating = 1;
        dir_x = -nx;
        dir_y = -ny;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx -= nx * impulse;
        robot->vy -= ny * impulse;
        append_text(out, out_size, "R%d RETREAT impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_STRAFE_L:
        robot->circling = 1;
        dir_x = tx;
        dir_y = ty;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx += tx * impulse;
        robot->vy += ty * impulse;
        append_text(out, out_size, "R%d STRAFE_L impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_STRAFE_R:
        robot->circling = 1;
        dir_x = -tx;
        dir_y = -ty;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx -= tx * impulse;
        robot->vy -= ty * impulse;
        append_text(out, out_size, "R%d STRAFE_R impulse %.2fm. ", side + 1, impulse);
        break;
    case CMD_CIRCLE_L:
        robot->circling = 1;
        dir_x = tx + nx * 0.25;
        dir_y = ty + ny * 0.25;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx += tx * impulse + nx * impulse * 0.25;
        robot->vy += ty * impulse + ny * impulse * 0.25;
        append_text(out, out_size, "R%d CIRCLE_L arcs around opponent. ", side + 1);
        break;
    case CMD_CIRCLE_R:
        robot->circling = 1;
        dir_x = -tx + nx * 0.25;
        dir_y = -ty + ny * 0.25;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx -= tx * impulse - nx * impulse * 0.25;
        robot->vy -= ty * impulse - ny * impulse * 0.25;
        append_text(out, out_size, "R%d CIRCLE_R arcs around opponent. ", side + 1);
        break;
    case CMD_RESET:
        fight->clinch = 0;
        fight->stuck_ticks = 0;
        robot->retreating = 1;
        dir_x = -nx;
        dir_y = -ny;
        set_step_footwork(robot, intent->command.id, dir_x, dir_y, impulse);
        robot->vx -= nx * impulse;
        robot->vy -= ny * impulse;
        append_text(out, out_size, "R%d RESET breaks contact and backs out. ", side + 1);
        break;
    default:
        break;
    }
}

static void clamp_robot_speed(RobotState *robot)
{
    double speed = sqrt(robot->vx * robot->vx + robot->vy * robot->vy);
    if (speed > ROBOT_MAX_SPEED_M_PER_TURN && speed > 0.0001) {
        double scale = ROBOT_MAX_SPEED_M_PER_TURN / speed;
        robot->vx *= scale;
        robot->vy *= scale;
    }
}

static void resolve_capsule_arena_contact(RobotState *robot, int side,
                                          char *out, size_t out_size)
{
    RobotCapsules capsules;
    double deepest = 0.0;
    double best_nx = 0.0;
    double best_ny = 0.0;
    int i;

    build_robot_capsules(robot, &capsules);

    for (i = 0; i < capsules.count; i++) {
        const PhysicsCapsule *capsule = &capsules.items[i];
        double sx = capsule->b.x - capsule->a.x;
        double sy = capsule->b.y - capsule->a.y;
        double denom = sx * sx + sy * sy;
        double t = 0.0;
        double px;
        double py;
        double dist;
        double penetration;

        if (denom > 0.000001) {
            t = clamp_double(-(capsule->a.x * sx + capsule->a.y * sy) / denom,
                             0.0, 1.0);
        }
        px = capsule->a.x + sx * t;
        py = capsule->a.y + sy * t;
        dist = sqrt(px * px + py * py);
        penetration = dist + capsule->radius - ARENA_RADIUS_M;
        if (penetration > deepest && dist > 0.0001) {
            deepest = penetration;
            best_nx = px / dist;
            best_ny = py / dist;
        }
    }

    if (deepest > 0.0) {
        double outward_v = robot->vx * best_nx + robot->vy * best_ny;

        robot->x -= best_nx * (deepest + ROBOT_CONTACT_SLOP_M);
        robot->y -= best_ny * (deepest + ROBOT_CONTACT_SLOP_M);
        robot->wall_nx = best_nx;
        robot->wall_ny = best_ny;
        robot->wall_braced = robot->down ? 1 : robot->wall_braced;
        record_wall_contact(robot, side, best_nx, best_ny,
                            fmax(deepest, outward_v), out, out_size);

        if (outward_v > 0.0) {
            robot->vx -= best_nx * outward_v * 1.18;
            robot->vy -= best_ny * outward_v * 1.18;
            if (outward_v > 0.24) {
                int impact = clamp_int((int)(outward_v * 28.0), 1, 14);
                robot->shock = clamp_int(robot->shock + impact, 0, 140);
                robot->stability = clamp_int(robot->stability - impact, 0,
                                             max_stability(robot));
                append_text(out, out_size,
                            "R%d limb/body capsule caught by cage %.2fm/t. ",
                            side + 1, outward_v);
            }
        }
    }
}

static void resolve_arena_contact(Fight *fight, int side, char *out,
                                  size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    double dist = sqrt(robot->x * robot->x + robot->y * robot->y);
    double max_dist = ARENA_RADIUS_M - robot_contact_radius(robot);

    if (dist > max_dist) {
        double nx = dist < 0.0001 ? (side == 0 ? -1.0 : 1.0) : robot->x / dist;
        double ny = dist < 0.0001 ? 0.0 : robot->y / dist;
        double outward_v = robot->vx * nx + robot->vy * ny;
        double restitution = robot->down ? 1.0 : 1.35;

        robot->x = nx * max_dist;
        robot->y = ny * max_dist;
        robot->wall_nx = nx;
        robot->wall_ny = ny;
        robot->wall_braced = robot->down ? 1 : robot->wall_braced;
        record_wall_contact(robot, side, nx, ny,
                            fmax(dist - max_dist, outward_v),
                            out, out_size);

        if (outward_v > 0.0) {
            robot->vx -= nx * outward_v * restitution;
            robot->vy -= ny * outward_v * restitution;
            if (outward_v > 0.22) {
                int impact = clamp_int((int)(outward_v * 34.0), 1, 16);
                robot->shock = clamp_int(robot->shock + impact, 0, 140);
                robot->stability = clamp_int(robot->stability - impact, 0,
                                             max_stability(robot));
                append_text(out, out_size, "R%d cage wall impact %.2fm/t. ",
                            side + 1, outward_v);
                if (!robot->down && outward_v > 0.34 &&
                    (robot->stability < 42 || outward_v > 0.52)) {
                    int was_down = robot->down;
                    robot->vx -= nx * outward_v * 0.25;
                    robot->vy -= ny * outward_v * 0.25;
                    fall_down_direction(robot, "cage wall impact",
                                        -nx, -ny, PART_TORSO,
                                        robot->angular_velocity,
                                        out, out_size);
                    if (!was_down && robot->down) {
                        crowd_react_knockdown(fight, -1, PART_TORSO, 1);
                    }
                    robot->wall_braced = 1;
                    robot->wall_nx = nx;
                    robot->wall_ny = ny;
                }
            }
        }
    }

    dist = sqrt(robot->x * robot->x + robot->y * robot->y);
    if (robot->down && wall_gap(robot) < 0.28 && dist > 0.0001) {
        robot->wall_braced = 1;
        robot->wall_nx = robot->x / dist;
        robot->wall_ny = robot->y / dist;
    }

    resolve_capsule_arena_contact(robot, side, out, out_size);
}

static void resolve_robot_contact(Fight *fight, char *out, size_t out_size,
                                  int *reported)
{
    RobotCapsules a_capsules;
    RobotCapsules b_capsules;
    RobotState *a = &fight->robot[0];
    RobotState *b = &fight->robot[1];
    int i;
    int j;
    double center_dx = b->x - a->x;
    double center_dy = b->y - a->y;

    if (center_dx * center_dx + center_dy * center_dy > 4.25) {
        return;
    }

    build_robot_capsules(a, &a_capsules);
    build_robot_capsules(b, &b_capsules);

    for (i = 0; i < a_capsules.count; i++) {
        for (j = 0; j < b_capsules.count; j++) {
            const PhysicsCapsule *ca = &a_capsules.items[i];
            const PhysicsCapsule *cb = &b_capsules.items[j];
            Vec3 normal3;
            double clearance = capsule_clearance(ca, cb, &normal3);
            double penetration = -clearance;
            double nx = normal3.x;
            double ny = normal3.y;
            double horizontal = sqrt(nx * nx + ny * ny);
            double hardness = capsule_pair_hardness(ca, cb);
            double rel;

            if (clearance > ROBOT_CONTACT_MARGIN_M) {
                continue;
            }

            if (horizontal < 0.0001) {
                double dist = sqrt(center_dx * center_dx + center_dy * center_dy);
                if (dist < 0.0001) {
                    nx = 1.0;
                    ny = 0.0;
                } else {
                    nx = center_dx / dist;
                    ny = center_dy / dist;
                }
            } else {
                nx /= horizontal;
                ny /= horizontal;
            }

            if (penetration > 0.0) {
                double inv_a = 1.0 / fmax(2.0, ca->mass_kg);
                double inv_b = 1.0 / fmax(2.0, cb->mass_kg);
                double inv_sum = inv_a + inv_b;
                double hard_bias = is_core_part(ca->part) || is_core_part(cb->part)
                    ? ROBOT_HARD_CONTACT_BIAS_M
                    : ROBOT_CONTACT_SLOP_M;
                double correction = penetration * hardness + hard_bias;
                double share_a = inv_a / inv_sum;
                double share_b = inv_b / inv_sum;

                a->x -= nx * correction * share_a;
                a->y -= ny * correction * share_a;
                b->x += nx * correction * share_b;
                b->y += ny * correction * share_b;
            }

            rel = (a->vx - b->vx) * nx + (a->vy - b->vy) * ny;
            if (rel > 0.0 || penetration > 0.0) {
                double inv_a = 1.0 / fmax(2.0, ca->mass_kg);
                double inv_b = 1.0 / fmax(2.0, cb->mass_kg);
                double inv_sum = inv_a + inv_b;
                double impulse = (rel * (1.0 + ROBOT_CONTACT_RESTITUTION) * hardness +
                                  clamp_double(penetration, 0.0, 0.18) * 0.24 * hardness) /
                                 inv_sum;

                if (impulse > 0.0) {
                    a->vx -= nx * impulse * inv_a;
                    a->vy -= ny * impulse * inv_a;
                    b->vx += nx * impulse * inv_b;
                    b->vy += ny * impulse * inv_b;
                    apply_contact_yaw(a, nx, ny, impulse, inv_a, -1.0);
                    apply_contact_yaw(b, nx, ny, impulse, inv_b, 1.0);
                    clamp_robot_speed(a);
                    clamp_robot_speed(b);
                }

                if (!fight->clinch && rel > 0.20 && reported != NULL &&
                    !*reported) {
                    int impact = clamp_int((int)(rel * 34.0), 1, 18);
                    if (is_core_part(ca->part) || is_core_part(cb->part)) {
                        impact = clamp_int(impact + 2, 1, 22);
                    }
                    a->shock = clamp_int(a->shock + impact / 2, 0, 140);
                    b->shock = clamp_int(b->shock + impact / 2, 0, 140);
                    a->stability = clamp_int(a->stability - impact / 2, 0,
                                             max_stability(a));
                    b->stability = clamp_int(b->stability - impact / 2, 0,
                                             max_stability(b));
                    append_text(out, out_size,
                                "capsule collision %s/%s rel %.2fm/t. ",
                                part_name(ca->part), part_name(cb->part), rel);
                    *reported = 1;
                    if (!a->down && rel > 0.34) {
                        set_stagger(a,
                                    rel > 0.58 ? ROBOT_STAGGER_STUMBLE :
                                    ROBOT_STAGGER_STEP_BACK,
                                    -nx, -ny, out, out_size);
                    }
                    if (!b->down && rel > 0.34) {
                        set_stagger(b,
                                    rel > 0.58 ? ROBOT_STAGGER_STUMBLE :
                                    ROBOT_STAGGER_STEP_BACK,
                                    nx, ny, out, out_size);
                    }
                    if (rel > 0.46) {
                        double shove = clamp_double(rel * 0.20, 0.04, 0.18);
                        if (!a->down && (a->stability < 35 || rel > 0.76)) {
                            int was_down = a->down;
                            a->vx -= nx * shove;
                            a->vy -= ny * shove;
                            fall_down_direction(a, "capsule collision",
                                                -nx, -ny, ca->part,
                                                a->angular_velocity,
                                                out, out_size);
                            if (!was_down && a->down) {
                                crowd_react_knockdown(fight, 1, ca->part,
                                                      rel > 0.72);
                            }
                        }
                        if (!b->down && (b->stability < 35 || rel > 0.76)) {
                            int was_down = b->down;
                            b->vx += nx * shove;
                            b->vy += ny * shove;
                            fall_down_direction(b, "capsule collision",
                                                nx, ny, cb->part,
                                                b->angular_velocity,
                                                out, out_size);
                            if (!was_down && b->down) {
                                crowd_react_knockdown(fight, 0, cb->part,
                                                      rel > 0.72);
                            }
                        }
                    }
                }
            }
        }
    }
}

static int forced_move_apart_threshold_ticks(void)
{
    int ticks = (int)(FORCED_MOVE_APART * CFA_PHYSICS_STEPS_PER_TURN /
                      CFA_SECONDS_PER_TURN + 0.999);
    return ticks < 1 ? 1 : ticks;
}

static int fighters_are_stuck_together(const Fight *fight)
{
    const RobotState *a = &fight->robot[0];
    const RobotState *b = &fight->robot[1];
    double nx;
    double ny;
    double separating_speed;

    if (a->defeated || b->defeated) {
        return 0;
    }
    if (fight->clinch) {
        return 1;
    }
    if (surface_gap(fight) > ROBOT_CONTACT_MARGIN_M * 1.5) {
        return 0;
    }

    vector_to_opponent(fight, 0, &nx, &ny);
    separating_speed = (b->vx - a->vx) * nx + (b->vy - a->vy) * ny;
    return separating_speed <= 0.06;
}

static void clear_forced_clinch_pressure(Fight *fight)
{
    int i;

    fight->clinch = 0;
    for (i = 0; i < 2; i++) {
        fight->robot[i].clinch_leverage = 0.0;
        fight->robot[i].clinch_pressure = 0.0;
        fight->robot[i].throw_torque = 0.0;
    }
}

static void force_move_apart(Fight *fight, char *out, size_t out_size)
{
    RobotState *a = &fight->robot[0];
    RobotState *b = &fight->robot[1];
    double nx;
    double ny;
    double dist;
    double desired_dist;
    double correction;
    double impulse_a;
    double impulse_b;

    vector_to_opponent(fight, 0, &nx, &ny);
    dist = center_distance(fight);
    desired_dist = robot_min_separation(a, b) + FORCED_MOVE_APART_RELEASE_GAP_M;
    correction = desired_dist - dist;

    clear_forced_clinch_pressure(fight);
    fight->stuck_ticks = 0;

    if (correction > 0.0) {
        a->x -= nx * correction * 0.50;
        a->y -= ny * correction * 0.50;
        b->x += nx * correction * 0.50;
        b->y += ny * correction * 0.50;
    }

    impulse_a = FORCED_MOVE_APART_IMPULSE_M * (a->down ? 0.55 : 1.0);
    impulse_b = FORCED_MOVE_APART_IMPULSE_M * (b->down ? 0.55 : 1.0);
    a->vx -= nx * impulse_a;
    a->vy -= ny * impulse_a;
    b->vx += nx * impulse_b;
    b->vy += ny * impulse_b;
    a->retreating = 1;
    b->retreating = 1;

    if (!a->down && mode_available(a, USE_ANY_LEG)) {
        set_step_footwork(a, CMD_RESET, -nx, -ny, FORCED_MOVE_APART_IMPULSE_M);
    }
    if (!b->down && mode_available(b, USE_ANY_LEG)) {
        set_step_footwork(b, CMD_RESET, nx, ny, FORCED_MOVE_APART_IMPULSE_M);
    }

    clamp_robot_speed(a);
    clamp_robot_speed(b);
    resolve_arena_contact(fight, 0, out, out_size);
    resolve_arena_contact(fight, 1, out, out_size);
    update_robot_balance(a);
    update_robot_balance(b);

    append_text(out, out_size,
                "FORCED_MOVE_APART after %.1fs separates stuck contact. ",
                FORCED_MOVE_APART);
}

static void update_forced_move_apart(Fight *fight, char *out, size_t out_size)
{
    if (!fighters_are_stuck_together(fight)) {
        fight->stuck_ticks = 0;
        return;
    }

    fight->stuck_ticks++;
    if (fight->stuck_ticks >= forced_move_apart_threshold_ticks()) {
        force_move_apart(fight, out, out_size);
    }
}

static void apply_balance_pressure(Fight *fight, int side,
                                   char *out, size_t out_size)
{
    RobotState *robot = &fight->robot[side];
    if (robot->down || robot->balance_state == CFA_BALANCE_SUPPORTED) {
        return;
    }

    if (robot->balance_state == CFA_BALANCE_EDGE) {
        robot->stability = clamp_int(robot->stability - 1, 0,
                                     max_stability(robot));
        robot->angular_velocity =
            clamp_abs(robot->angular_velocity +
                          (robot->pivot_foot == FOOT_LEFT ? 0.010 : -0.010),
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        return;
    }

    robot->stability = clamp_int(
        robot->stability - clamp_int((int)(robot->balance_offset * 20.0), 1, 5),
        0, max_stability(robot));
    robot->shock = clamp_int(robot->shock + 1, 0, 140);

    if (robot->balance_offset > ROBOT_BALANCE_FALL_MARGIN_M &&
        (robot->stability < 38 || robot->shock > 70 ||
         fabs(robot->angular_velocity) > 0.28)) {
        double fall_x = robot->center_mass_x - robot->support_center_x;
        double fall_y = robot->center_mass_y - robot->support_center_y;
        int was_down = robot->down;
        append_text(out, out_size,
                    "R%d center of mass leaves support polygon %.2fm. ",
                    side + 1, robot->balance_offset);
        fall_down_direction(robot, "lost balance", fall_x, fall_y,
                            PART_TORSO, robot->angular_velocity,
                            out, out_size);
        if (!was_down && robot->down) {
            crowd_react_knockdown(fight, -1, PART_TORSO, 1);
        }
    } else if (robot->balance_offset > ROBOT_BALANCE_FALL_MARGIN_M * 0.90 &&
               (robot->stability < 70 || robot->shock > 16 ||
                fabs(robot->angular_velocity) > 0.18)) {
        set_stagger(robot,
                    robot->balance_offset > ROBOT_BALANCE_FALL_MARGIN_M * 1.40 ?
                        ROBOT_STAGGER_STUMBLE : ROBOT_STAGGER_STEP_BACK,
                    robot->center_mass_x - robot->support_center_x,
                    robot->center_mass_y - robot->support_center_y,
                    out, out_size);
    }
}

static void project_strike_contact(Fight *fight, int attacker_side,
                                   const StrikeContact *contact,
                                   char *out, size_t out_size)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    double dx = defender->x - attacker->x;
    double dy = defender->y - attacker->y;
    double dist = sqrt(dx * dx + dy * dy);
    double min_sep = robot_min_separation(attacker, defender) + ROBOT_CONTACT_SLOP_M;
    double nx;
    double ny;
    double penetration;
    int suppress_contact_log = 1;

    resolve_robot_contact(fight, out, out_size, &suppress_contact_log);

    if (dist < 0.0001) {
        vector_to_opponent(fight, attacker_side, &nx, &ny);
        dist = 0.0001;
    } else {
        nx = dx / dist;
        ny = dy / dist;
    }

    penetration = min_sep - dist;
    if (penetration <= 0.0) {
        return;
    }

    attacker->x -= nx * penetration * 0.35;
    attacker->y -= ny * penetration * 0.35;
    defender->x += nx * penetration * 0.65;
    defender->y += ny * penetration * 0.65;
    resolve_arena_contact(fight, attacker_side, out, out_size);
    resolve_arena_contact(fight, defender_side, out, out_size);
    append_text(out, out_size, " contact shells separate %.2fm;", penetration);

    if (contact != NULL && contact->hit) {
        append_text(out, out_size, " swept %s/%s surface;",
                    part_name(contact->attacker_part),
                    part_name(contact->defender_part));
    }
}

static double command_strike_mass_kg(CommandId id)
{
    switch (id) {
    case CMD_L_JAB:
        return 7.5;
    case CMD_R_CROSS:
    case CMD_L_HOOK:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
        return 10.5;
    case CMD_ELBOW:
        return 12.0;
    case CMD_LOW_KICK:
        return 20.0;
    case CMD_HIGH_KICK:
        return 24.0;
    case CMD_KNEE:
    case CMD_STOMP:
        return 22.0;
    case CMD_THROW:
        return 54.0;
    default:
        return 12.0;
    }
}

static BodyPart command_strike_part(const RobotState *attacker, CommandId id)
{
    switch (id) {
    case CMD_L_JAB:
    case CMD_L_HOOK:
        return PART_L_ARM;
    case CMD_R_CROSS:
    case CMD_R_HOOK:
        return PART_R_ARM;
    case CMD_UPPERCUT:
    case CMD_ELBOW:
        return !attacker->detached[PART_R_ARM] ? PART_R_ARM : PART_L_ARM;
    case CMD_LOW_KICK:
    case CMD_HIGH_KICK:
    case CMD_KNEE:
    case CMD_STOMP:
        return PART_R_LEG;
    default:
        return PART_TORSO;
    }
}

static Vec3 fixed_punch_tip_local(const RobotState *robot, CommandId id,
                                  BodyPart part, Vec3 requested_tip)
{
    double side = part == PART_L_ARM ? -1.0 : 1.0;
    Vec3 shoulder = vec3(0.02, ROBOT_SHOULDER_HEIGHT_M,
                         side * ROBOT_SHOULDER_HALF_WIDTH_M);
    Vec3 requested = requested_tip;
    Vec3 direction;
    Vec3 wrist_target;
    Vec3 wrist;
    double reach_boost = 0.0;

    switch (id) {
    case CMD_R_CROSS:
        reach_boost = ROBOT_TORSO_TWIST_REACH_M * 0.90;
        break;
    case CMD_L_JAB:
        reach_boost = ROBOT_TORSO_TWIST_REACH_M * 0.42;
        break;
    case CMD_L_HOOK:
    case CMD_R_HOOK:
        reach_boost = ROBOT_TORSO_TWIST_REACH_M * 0.36;
        break;
    case CMD_UPPERCUT:
    case CMD_ELBOW:
        reach_boost = ROBOT_TORSO_TWIST_REACH_M * 0.22;
        break;
    default:
        break;
    }

    if (robot->pivot_foot != FOOT_NONE) {
        reach_boost += fabs(robot->pivot_angle) * 0.10;
    }
    requested.x += clamp_double(reach_boost, 0.0, 0.11);

    direction = vec3_normalize_or(vec3_sub(requested, shoulder),
                                  vec3(1.0, -0.05, side * 0.16));
    wrist_target = vec3_sub(requested,
                            vec3_scale(direction, ROBOT_HAND_SEGMENT_LENGTH_M));
    fixed_two_bone_joint(shoulder, wrist_target,
                         ROBOT_UPPER_ARM_LENGTH_M,
                         ROBOT_FOREARM_LENGTH_M,
                         vec3(-0.10, -0.08, side * 0.42),
                         &wrist);
    return vec3_add(wrist, vec3_scale(direction,
                                      ROBOT_HAND_SEGMENT_LENGTH_M));
}

static Vec3 fixed_kick_tip_world_from_request(const RobotState *robot,
                                              CommandId id,
                                              Vec3 requested_tip)
{
    Vec3 hip = local_to_world_point(robot,
                                    vec3(-0.01, ROBOT_HIP_HEIGHT_M,
                                         ROBOT_HIP_HALF_WIDTH_M));
    Vec3 forward = robot_forward_vec(robot);
    Vec3 side = robot_side_vec(robot, 1.0);
    Vec3 direction;
    Vec3 ankle_target;
    Vec3 knee;
    Vec3 ankle;
    double reach_boost = 0.0;

    switch (id) {
    case CMD_LOW_KICK:
        reach_boost = ROBOT_HIP_TWIST_REACH_M * 0.62;
        break;
    case CMD_HIGH_KICK:
        reach_boost = ROBOT_HIP_TWIST_REACH_M * 0.86;
        break;
    case CMD_STOMP:
        reach_boost = ROBOT_HIP_TWIST_REACH_M * 0.34;
        break;
    default:
        break;
    }
    if (robot->pivot_foot != FOOT_NONE) {
        reach_boost += fabs(robot->pivot_angle) * 0.13;
    }
    requested_tip = vec3_add(requested_tip,
                             vec3_scale(forward,
                                        clamp_double(reach_boost, 0.0, 0.16)));

    direction = vec3_normalize_or(vec3_sub(requested_tip, hip),
                                  vec3_add(forward,
                                           vec3_scale(side, 0.12)));
    ankle_target = vec3_sub(requested_tip,
                            vec3_scale(direction,
                                       ROBOT_FOOT_SEGMENT_LENGTH_M));
    knee = fixed_two_bone_joint(
        hip, ankle_target,
        ROBOT_UPPER_LEG_LENGTH_M,
        ROBOT_LOWER_LEG_LENGTH_M,
        vec3_add(vec3_add(vec3_scale(forward, 0.35),
                          vec3_scale(side, 0.08)),
                 vec3(0.0, 0.0, 0.06)),
        &ankle);
    (void)knee;
    return vec3_add(ankle,
                    vec3_scale(direction, ROBOT_FOOT_SEGMENT_LENGTH_M));
}

static Vec3 fixed_kick_tip_world_local(const RobotState *robot, CommandId id,
                                       Vec3 requested_local)
{
    return fixed_kick_tip_world_from_request(
        robot, id, local_to_world_point(robot, requested_local));
}

static Vec3 fixed_knee_strike_world(const RobotState *robot,
                                    Vec3 requested_knee_local)
{
    Vec3 hip = local_to_world_point(robot,
                                    vec3(-0.01, ROBOT_HIP_HEIGHT_M,
                                         ROBOT_HIP_HALF_WIDTH_M));
    Vec3 requested = local_to_world_point(robot, requested_knee_local);
    Vec3 forward = robot_forward_vec(robot);
    Vec3 side = robot_side_vec(robot, 1.0);
    Vec3 direction = vec3_normalize_or(vec3_sub(requested, hip),
                                       vec3_add(forward,
                                                vec3_scale(side, 0.16)));

    return vec3_add(hip, vec3_scale(direction, ROBOT_UPPER_LEG_LENGTH_M));
}

static int build_strike_sweep_capsule(const RobotState *attacker,
                                      CommandId id,
                                      PhysicsCapsule *capsule)
{
    BodyPart part = command_strike_part(attacker, id);
    RobotState previous = previous_pose_robot(attacker);
    Vec3 start;
    Vec3 end;
    double side = 1.0;
    double radius = ROBOT_FIST_RADIUS_M;

    if (part == PART_L_ARM) {
        side = -1.0;
    }

    if ((part == PART_L_ARM || part == PART_R_ARM) &&
        attacker->detached[part]) {
        return 0;
    }
    if (part == PART_R_LEG && attacker->detached[PART_R_LEG]) {
        return 0;
    }

    switch (id) {
    case CMD_L_JAB:
        start = robot_hand_local_for_contact(attacker, -1.0, PART_L_ARM);
        end = vec3(0.80, ROBOT_STRIKING_HAND_HEIGHT_M + 0.04, -0.13);
        radius = ROBOT_FIST_RADIUS_M;
        break;
    case CMD_R_CROSS:
        start = robot_hand_local_for_contact(attacker, 1.0, PART_R_ARM);
        end = vec3(0.84, ROBOT_STRIKING_HAND_HEIGHT_M + 0.02, 0.13);
        radius = ROBOT_FIST_RADIUS_M;
        break;
    case CMD_L_HOOK:
        start = robot_hand_local_for_contact(attacker, -1.0, PART_L_ARM);
        end = vec3(0.70, ROBOT_STRIKING_HAND_HEIGHT_M + 0.02, -0.07);
        radius = ROBOT_FIST_RADIUS_M * 1.08;
        break;
    case CMD_R_HOOK:
        start = robot_hand_local_for_contact(attacker, 1.0, PART_R_ARM);
        end = vec3(0.74, ROBOT_STRIKING_HAND_HEIGHT_M + 0.02, 0.07);
        radius = ROBOT_FIST_RADIUS_M * 1.08;
        break;
    case CMD_UPPERCUT:
        start = robot_hand_local_for_contact(attacker, side, part);
        end = vec3(0.64, ROBOT_HEAD_CENTER_HEIGHT_M - 0.18, side * 0.10);
        radius = ROBOT_FIST_RADIUS_M * 1.06;
        break;
    case CMD_ELBOW:
        start = robot_hand_local_for_contact(attacker, side, part);
        end = vec3(0.50, ROBOT_STRIKING_HAND_HEIGHT_M, side * 0.08);
        radius = ROBOT_ELBOW_RADIUS_M * 1.16;
        break;
    case CMD_LOW_KICK:
        start = fixed_kick_tip_world_from_request(
            &previous, id,
            vec3(previous.foot_x[FOOT_RIGHT],
                 previous.foot_y[FOOT_RIGHT],
                 ROBOT_FOOT_HEIGHT_M * 0.25));
        end = fixed_kick_tip_world_local(
            attacker, id, vec3(1.04, 0.24, ROBOT_HIP_HALF_WIDTH_M));
        radius = ROBOT_FOOT_WIDTH_M * 0.52;
        capsule->part = PART_R_LEG;
        capsule->a = start;
        capsule->b = end;
        capsule->radius = radius;
        capsule->mass_kg = command_strike_mass_kg(id);
        return 1;
    case CMD_HIGH_KICK:
        start = fixed_kick_tip_world_from_request(
            &previous, id,
            vec3(previous.foot_x[FOOT_RIGHT],
                 previous.foot_y[FOOT_RIGHT],
                 ROBOT_FOOT_HEIGHT_M * 0.25));
        end = fixed_kick_tip_world_local(
            attacker, id, vec3(1.14,
                               ROBOT_KNEE_HEIGHT_M + 0.70,
                               ROBOT_HIP_HALF_WIDTH_M * 0.78));
        radius = ROBOT_FOOT_WIDTH_M * 0.52;
        capsule->part = PART_R_LEG;
        capsule->a = start;
        capsule->b = end;
        capsule->radius = radius;
        capsule->mass_kg = command_strike_mass_kg(id);
        return 1;
    case CMD_KNEE:
        start = fixed_knee_strike_world(&previous,
                                        vec3(0.10, ROBOT_HIP_HEIGHT_M,
                                             ROBOT_HIP_HALF_WIDTH_M));
        end = fixed_knee_strike_world(
            attacker,
            vec3(0.56, ROBOT_CHEST_HEIGHT_M - 0.12,
                 ROBOT_HIP_HALF_WIDTH_M * 0.58));
        radius = ROBOT_KNEE_RADIUS_M * 1.35;
        capsule->part = PART_R_LEG;
        capsule->a = start;
        capsule->b = end;
        capsule->radius = radius;
        capsule->mass_kg = command_strike_mass_kg(id);
        return 1;
    case CMD_STOMP:
        start = fixed_kick_tip_world_from_request(
            &previous, id,
            vec3(previous.foot_x[FOOT_RIGHT],
                 previous.foot_y[FOOT_RIGHT],
                 ROBOT_FOOT_HEIGHT_M * 0.25));
        end = fixed_kick_tip_world_local(
            attacker, id, vec3(0.84, 0.38, ROBOT_HIP_HALF_WIDTH_M));
        radius = ROBOT_FOOT_WIDTH_M * 0.56;
        capsule->part = PART_R_LEG;
        capsule->a = start;
        capsule->b = end;
        capsule->radius = radius;
        capsule->mass_kg = command_strike_mass_kg(id);
        return 1;
    default:
        return 0;
    }

    if (part == PART_L_ARM || part == PART_R_ARM) {
        start = fixed_punch_tip_local(&previous, id, part, start);
        end = fixed_punch_tip_local(attacker, id, part, end);
    }

    capsule->part = part;
    capsule->a = local_to_world_point(&previous, start);
    capsule->b = local_to_world_point(attacker, end);
    capsule->radius = radius;
    capsule->mass_kg = command_strike_mass_kg(id);
    return 1;
}

static int capsule_matches_target(const PhysicsCapsule *capsule,
                                  BodyPart target)
{
    return capsule->part == target;
}

static StrikeContact find_strike_contact(const Fight *fight, int attacker_side,
                                         CommandId id, BodyPart target,
                                         const BlockResult *block)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    const RobotState *attacker = &fight->robot[attacker_side];
    const RobotState *defender = &fight->robot[defender_side];
    PhysicsCapsule strike;
    RobotCapsules defender_capsules;
    StrikeContact result;
    BodyPart preferred_part = target;
    BodyPart guard_part = PART_INVALID;
    BodyPart wanted_parts[3];
    int wanted_count = 0;
    int pass;

    memset(&result, 0, sizeof(result));
    result.attacker_part = command_strike_part(attacker, id);
    result.defender_part = target;
    result.normal = vec3_normalize_or(vec3(defender->x - attacker->x,
                                           defender->y - attacker->y,
                                           0.0),
                                      vec3(1.0, 0.0, 0.0));
    result.clearance_m = 99.0;
    result.attacker_mass_kg = command_strike_mass_kg(id);
    result.defender_mass_kg = part_mass_kg[target];
    result.angle_cos = 1.0;

    if (!build_strike_sweep_capsule(attacker, id, &strike)) {
        result.hit = 1;
        result.clearance_m = 0.0;
        return result;
    }

    if (block != NULL && block->attempted && block->success &&
        block->shield != PART_INVALID) {
        preferred_part = block->shield;
    }
    if ((target == PART_HEAD || target == PART_TORSO) &&
        !defender->down && !defender->defeated) {
        if (block != NULL && block->attempted &&
            guard_part_available(defender, block->shield)) {
            guard_part = block->shield;
        } else if (defender->guard) {
            guard_part = guard_arm(defender);
        }
    }

    if (guard_part_available(defender, guard_part)) {
        wanted_parts[wanted_count++] = guard_part;
    }
    if (preferred_part != PART_INVALID && preferred_part != guard_part) {
        wanted_parts[wanted_count++] = preferred_part;
    }
    if (target != PART_INVALID && target != guard_part && target != preferred_part) {
        wanted_parts[wanted_count++] = target;
    }
    if (wanted_count == 0 && target != PART_INVALID) {
        wanted_parts[wanted_count++] = target;
    }

    build_robot_capsules(defender, &defender_capsules);

    for (pass = 0; pass < wanted_count; pass++) {
        BodyPart wanted = wanted_parts[pass];
        int i;

        for (i = 0; i < defender_capsules.count; i++) {
            const PhysicsCapsule *capsule = &defender_capsules.items[i];
            Vec3 normal;
            Vec3 strike_point;
            Vec3 defender_point;
            Vec3 strike_delta;
            Vec3 strike_dir;
            double clearance;
            double root_speed;
            double angle_cos;

            if (!capsule_matches_target(capsule, wanted)) {
                continue;
            }

            clearance = capsule_clearance_details(&strike, capsule, &normal,
                                                  &strike_point,
                                                  &defender_point);
            strike_delta = vec3_sub(strike.b, strike.a);
            strike_dir = vec3_normalize_or(strike_delta,
                                           robot_forward_vec(attacker));
            angle_cos = clamp_double(vec3_dot(strike_dir, normal), 0.0, 1.0);
            root_speed = (attacker->vx - defender->vx) * normal.x +
                         (attacker->vy - defender->vy) * normal.y;
            if (clearance < result.clearance_m) {
                result.clearance_m = clearance;
                result.penetration_m = clearance < 0.0 ? -clearance : 0.0;
                result.normal = normal;
                result.attacker_part = strike.part;
                result.defender_part = capsule->part;
                result.attacker_mass_kg = strike.mass_kg;
                result.defender_mass_kg = capsule->mass_kg;
                result.attacker_point = strike_point;
                result.defender_point = defender_point;
                result.angle_cos = angle_cos;
                result.relative_speed_m = fmax(0.0,
                                               vec3_length(strike_delta) +
                                               root_speed);
                result.guarded = capsule->part == guard_part &&
                                 guard_part != PART_INVALID;
            }
        }

        if (result.clearance_m < 99.0) {
            double margin = ROBOT_STRIKE_CONTACT_MARGIN_M;
            if (result.guarded) {
                double guard_extra = defender->guard_coverage_m > 0.0
                    ? defender->guard_coverage_m * 0.35
                    : ROBOT_GUARD_HEAD_COVERAGE_M * 0.25;
                margin += clamp_double(guard_extra, 0.0, 0.055);
            }
            if (pass == wanted_count - 1 || result.clearance_m <= margin) {
                break;
            }
        }
    }

    if (result.guarded) {
        double guard_extra = defender->guard_coverage_m > 0.0
            ? defender->guard_coverage_m * 0.35
            : ROBOT_GUARD_HEAD_COVERAGE_M * 0.25;
        result.hit = result.clearance_m <=
            ROBOT_STRIKE_CONTACT_MARGIN_M +
            clamp_double(guard_extra, 0.0, 0.055);
        if (result.hit) {
            result.glancing = 1;
        }
    } else {
        result.hit = result.clearance_m <= ROBOT_STRIKE_CONTACT_MARGIN_M;
        if (result.hit && result.angle_cos < ROBOT_GLANCING_COS) {
            result.glancing = 1;
        }
    }

    if (result.clearance_m < 99.0 && result.relative_speed_m <= 0.0) {
        result.relative_speed_m = fmax(0.0, -result.clearance_m);
    }

    if (result.clearance_m < 99.0) {
        return result;
    }

    if (result.clearance_m >= 99.0) {
        result.clearance_m = 0.0;
        result.hit = 1;
        return result;
    }

    result.hit = result.clearance_m <= ROBOT_STRIKE_CONTACT_MARGIN_M;
    return result;
}

static void physics_step(Fight *fight, char *out, size_t out_size)
{
    int i;
    int pass;
    int substep;
    int reported_contact = 0;

    for (i = 0; i < 2; i++) {
        RobotState *robot = &fight->robot[i];
        store_previous_pose(robot);
        if (robot->down) {
            robot->vx *= 0.68;
            robot->vy *= 0.68;
            robot->angular_velocity *= 0.58;
        }
        clamp_robot_speed(robot);
        robot->angular_velocity = clamp_abs(robot->angular_velocity,
                                            ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    }

    for (substep = 0; substep < PHYSICS_SUBSTEPS; substep++) {
        for (i = 0; i < 2; i++) {
            RobotState *robot = &fight->robot[i];
            robot->x += robot->vx / (double)PHYSICS_SUBSTEPS;
            robot->y += robot->vy / (double)PHYSICS_SUBSTEPS;
            resolve_arena_contact(fight, i, out, out_size);
        }

        for (pass = 0; pass < ROBOT_CONTACT_SOLVER_PASSES; pass++) {
            resolve_robot_contact(fight, out, out_size, &reported_contact);
            resolve_arena_contact(fight, 0, out, out_size);
            resolve_arena_contact(fight, 1, out, out_size);
        }
    }

    if (fight->clinch && surface_gap(fight) > CLINCH_BREAK_GAP_M) {
        fight->clinch = 0;
        append_text(out, out_size, "clinch breaks by physical separation. ");
    }
    if (fight->clinch) {
        RobotState *a = &fight->robot[0];
        RobotState *b = &fight->robot[1];
        double nx;
        double ny;
        double pressure;

        if (a->down || b->down || !mode_available(a, USE_ANY_ARM) ||
            !mode_available(b, USE_ANY_ARM)) {
            fight->clinch = 0;
            append_text(out, out_size,
                        "clinch breaks as support frame collapses. ");
        } else {
            vector_to_opponent(fight, 0, &nx, &ny);
            pressure = clamp_double((a->clinch_leverage +
                                     b->clinch_leverage) * 0.5,
                                    0.10, 0.80);
            a->vx += nx * pressure * 0.018;
            a->vy += ny * pressure * 0.018;
            b->vx -= nx * pressure * 0.018;
            b->vy -= ny * pressure * 0.018;
            a->clinch_pressure = fmax(a->clinch_pressure,
                                      b->clinch_leverage * 0.62);
            b->clinch_pressure = fmax(b->clinch_pressure,
                                      a->clinch_leverage * 0.62);
        }
    }

    update_forced_move_apart(fight, out, out_size);

    for (i = 0; i < 2; i++) {
        double nx;
        double ny;
        double desired;
        double delta;
        vector_to_opponent(fight, i, &nx, &ny);
        desired = atan2(ny, nx);
        delta = normalize_angle(desired - fight->robot[i].facing);
        fight->robot[i].angular_velocity += delta * ROBOT_TURN_RESPONSE;
        fight->robot[i].angular_velocity =
            clamp_abs(fight->robot[i].angular_velocity,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        fight->robot[i].facing = normalize_angle(fight->robot[i].facing +
                                                 fight->robot[i].angular_velocity);
        if (fight->clinch) {
            fight->robot[i].vx *= 0.45;
            fight->robot[i].vy *= 0.45;
            fight->robot[i].angular_velocity *= 0.50;
        } else {
            fight->robot[i].vx *= FRICTION;
            fight->robot[i].vy *= FRICTION;
            fight->robot[i].angular_velocity *= ROBOT_ANGULAR_DAMPING;
        }
        if (fabs(fight->robot[i].vx) < 0.002) {
            fight->robot[i].vx = 0.0;
        }
        if (fabs(fight->robot[i].vy) < 0.002) {
            fight->robot[i].vy = 0.0;
        }
        if (fabs(fight->robot[i].angular_velocity) < 0.001) {
            fight->robot[i].angular_velocity = 0.0;
        }
        update_robot_footwork(&fight->robot[i]);
        update_guard_pose_state(&fight->robot[i]);
        apply_balance_pressure(fight, i, out, out_size);
        update_ground_contact(&fight->robot[i], i, out, out_size);
    }
}

static void apply_damage(Fight *fight, int defender_side, BodyPart target,
                         int raw_damage, const BlockResult *block, char *out,
                         size_t out_size)
{
    RobotState *defender = &fight->robot[defender_side];
    int armor = part_armor[target];
    int net_damage = raw_damage - armor / 3;
    int deflected = 0;

    if (block != NULL &&
        block->attempted &&
        block->damage_reduction_pct > 0 &&
        block->shield != PART_INVALID) {
        int reduction = clamp_int(block->damage_reduction_pct, 0, 85);
        BodyPart shield = block->shield;
        int breakthrough = 0;
        deflected = 1;
        if (raw_damage > block->coverage / 2 + defender->integrity[shield] / 8) {
            breakthrough = raw_damage -
                           (block->coverage / 2 + defender->integrity[shield] / 8);
            reduction = clamp_int(reduction - breakthrough * 2, 8, reduction);
        }
        net_damage = raw_damage * (100 - reduction) / 100 - armor / 3;
        if (target == PART_HEAD || target == PART_TORSO) {
            int shield_damage = clamp_int(raw_damage *
                                              (block->parry ? 24 :
                                               block->success ? 18 : 10) /
                                              100 +
                                              breakthrough / 3,
                                          1, block->parry ? 18 :
                                             block->success ? 14 : 8);
            defender->integrity[shield] -= shield_damage;
            record_surface_damage(defender, shield, raw_damage / 2, shield_damage);
            append_text(out, out_size, " %s %s deflects %d%%, shunting %d;",
                        part_name(shield),
                        block->parry ? "parry" :
                        block->success ? "guard" : "glancing guard",
                        reduction, shield_damage);
            if (breakthrough > 0) {
                append_text(out, out_size, " breakthrough %d;",
                            breakthrough);
            }
            detach_if_needed(defender, shield, out, out_size);
        }
    }

    net_damage = clamp_int(net_damage, deflected ? 0 : 1, 112);
    defender->integrity[target] -= net_damage;
    record_surface_damage(defender, target, raw_damage, net_damage);

    if (target == PART_HEAD) {
        int processor_loss = net_damage * 5 / 4;
        if (raw_damage >= 24) {
            processor_loss += 8;
        }
        if (raw_damage >= 36) {
            processor_loss += 8;
        }
        defender->processor -= processor_loss;
        defender->shock += net_damage * 2 + raw_damage / 3;
    } else if (target == PART_TORSO) {
        defender->shock += net_damage + raw_damage / 6;
        defender->stability -= net_damage / 3;
    } else if (target == PART_L_LEG || target == PART_R_LEG) {
        defender->shock += net_damage / 3;
        defender->stability -= net_damage / 2 + 5;
    } else {
        defender->shock += net_damage / 4;
        defender->stability -= net_damage / 6;
    }

    if (raw_damage >= 30) {
        defender->shock += 10;
        defender->stability -= 10;
    }

    defender->stability = clamp_int(defender->stability, 0, max_stability(defender));
    defender->processor = clamp_int(defender->processor, -100, 100);
    defender->shock = clamp_int(defender->shock, 0, 140);

    append_text(out, out_size, " net %d to %s", net_damage, part_name(target));
    detach_if_needed(defender, target, out, out_size);
}

static double twist_force_scale(CommandId id)
{
    switch (id) {
    case CMD_L_JAB:
    case CMD_R_CROSS:
    case CMD_L_HOOK:
    case CMD_R_HOOK:
    case CMD_UPPERCUT:
    case CMD_ELBOW:
        return ROBOT_TORSO_TWIST_FORCE_SCALE;
    case CMD_LOW_KICK:
    case CMD_HIGH_KICK:
    case CMD_KNEE:
    case CMD_STOMP:
        return ROBOT_HIP_TWIST_FORCE_SCALE;
    default:
        return 1.0;
    }
}

static void apply_knockback(Fight *fight, int attacker_side, BodyPart target,
                            int raw_damage, const MoveSpec *spec,
                            const StrikeContact *contact,
                            char *out, size_t out_size)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    double nx;
    double ny;
    double dv;
    double instability;
    double strike_mass = command_strike_mass_kg(spec->id);
    double target_mass = part_mass_kg[target];
    double mass_ratio;
    double recoil_scale;
    double angle_factor = 1.0;
    int fall_pressure;
    int threshold;
    int stagger_score;
    int stagger_state = ROBOT_STAGGER_NONE;

    vector_to_opponent(fight, attacker_side, &nx, &ny);
    if (contact != NULL && contact->hit) {
        double hx = contact->normal.x;
        double hy = contact->normal.y;
        double horizontal = sqrt(hx * hx + hy * hy);
        if (horizontal > 0.0001) {
            nx = hx / horizontal;
            ny = hy / horizontal;
        }
        strike_mass = contact->attacker_mass_kg;
        target_mass = contact->defender_mass_kg;
        angle_factor = 0.46 + clamp_double(contact->angle_cos, 0.0, 1.0) * 0.54;
        if (contact->glancing) {
            double tx = -ny;
            double ty = nx;
            double lateral = -sin(attacker->facing) * nx +
                             cos(attacker->facing) * ny;
            double tangent_mix = clamp_double(fabs(lateral) * 0.35 + 0.18,
                                              0.18, 0.46);
            double mixed_x = nx * (1.0 - tangent_mix) +
                             tx * (lateral >= 0.0 ? tangent_mix : -tangent_mix);
            double mixed_y = ny * (1.0 - tangent_mix) +
                             ty * (lateral >= 0.0 ? tangent_mix : -tangent_mix);
            double mixed_len = sqrt(mixed_x * mixed_x + mixed_y * mixed_y);

            if (mixed_len > 0.0001) {
                nx = mixed_x / mixed_len;
                ny = mixed_y / mixed_len;
            }
            angle_factor *= contact->guarded ? 0.62 : 0.72;
        }
        if (contact->guarded) {
            angle_factor *= 0.78;
        }
    }

    instability = clamp_double((100.0 - (double)defender->stability) / 100.0,
                               0.0, 0.85);
    dv = spec->strike_impulse_m * ((double)raw_damage / 16.0) *
         (1.0 + instability * 0.22);
    dv *= twist_force_scale(spec->id);
    dv *= angle_factor;
    if (contact != NULL && contact->hit) {
        dv *= 0.92 + clamp_double(contact->relative_speed_m, 0.0, 0.70) * 0.16;
    }
    mass_ratio = clamp_double(strike_mass / fmax(2.0, target_mass), 0.28, 1.55);
    dv *= 0.78 + mass_ratio * 0.36;

    if (target == PART_HEAD || target == PART_TORSO) {
        dv *= 1.26;
    }
    if (target == PART_L_LEG || target == PART_R_LEG) {
        dv *= 1.10;
    }
    if (spec->id == CMD_HIGH_KICK || spec->id == CMD_THROW) {
        dv *= 1.18;
    }
    if (defender->down) {
        dv *= 0.42;
    }
    if (defender->parry_active && contact != NULL && contact->guarded) {
        double tx = -ny;
        double ty = nx;
        double side = attacker_side == 0 ? 1.0 : -1.0;

        dv *= 0.66;
        attacker->vx -= nx * spec->strike_impulse_m * 0.15;
        attacker->vy -= ny * spec->strike_impulse_m * 0.15;
        attacker->vx += tx * side * spec->strike_impulse_m * 0.10;
        attacker->vy += ty * side * spec->strike_impulse_m * 0.10;
        attacker->stability = clamp_int(attacker->stability - 3, 0,
                                        max_stability(attacker));
        attacker->angular_velocity =
            clamp_abs(attacker->angular_velocity + side * 0.035,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        append_text(out, out_size, " parry recoil redirects line;");
    }

    project_strike_contact(fight, attacker_side, contact, out, out_size);
    defender->vx += nx * dv;
    defender->vy += ny * dv;
    {
        double lateral = -sin(defender->facing) * nx + cos(defender->facing) * ny;
        double torque_scale = target == PART_HEAD ? 0.30 :
                              target == PART_TORSO ? 0.20 : 0.14;
        double yaw_impulse = clamp_abs(lateral * dv * torque_scale,
                                       ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN * 0.45);

        if (target == PART_L_ARM || target == PART_L_LEG) {
            yaw_impulse -= dv * 0.035;
        } else if (target == PART_R_ARM || target == PART_R_LEG) {
            yaw_impulse += dv * 0.035;
        }

        defender->angular_velocity =
            clamp_abs(defender->angular_velocity + yaw_impulse,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        attacker->angular_velocity =
            clamp_abs(attacker->angular_velocity - yaw_impulse * 0.35,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        if (target == PART_HEAD) {
            int was_head_detached = defender->head_detached;
            detach_head_for_knockout(defender, nx, ny, dv, yaw_impulse,
                                     raw_damage, contact, out, out_size);
            if (!was_head_detached && defender->head_detached) {
                crowd_react_head_detach(fight, attacker_side);
            }
        }
    }
    recoil_scale = clamp_double(target_mass / fmax(2.0, strike_mass + target_mass) * 0.55,
                                0.18, 0.52);
    attacker->vx -= nx * dv * recoil_scale;
    attacker->vy -= ny * dv * recoil_scale;
    {
        double follow = clamp_double(0.22 + dv * 1.35 +
                                         (double)raw_damage / 90.0,
                                     0.18, 1.0);
        double recoil = clamp_double(recoil_scale * 1.28 +
                                         (contact != NULL && contact->guarded ?
                                          0.18 : 0.0) +
                                         (contact != NULL && contact->glancing ?
                                          0.14 : 0.0),
                                     0.06, 0.88);
        int phase = recoil > follow * 0.92 ? ROBOT_STRIKE_PHASE_RECOIL :
                                             ROBOT_STRIKE_PHASE_FOLLOW;

        set_strike_motion(attacker, phase, follow, recoil);
    }
    clamp_robot_speed(defender);
    clamp_robot_speed(attacker);

    if (dv > 0.05) {
        append_text(out, out_size, " impulse %.2fm/t mass %.0f>%.0fkg",
                    dv, strike_mass, target_mass);
    }

    if (defender->down) {
        return;
    }

    fall_pressure = raw_damage + (int)(spec->strike_impulse_m * 145.0) +
                    (100 - defender->stability) * 2 / 3 +
                    (int)(dv * 70.0);
    if (target == PART_HEAD || target == PART_TORSO) {
        fall_pressure += 14;
    }
    if (target == PART_L_LEG || target == PART_R_LEG) {
        fall_pressure += 18;
    }
    if (spec->id == CMD_HIGH_KICK || spec->id == CMD_KNEE) {
        fall_pressure += 16;
    }
    if (fight->clinch && spec->id == CMD_THROW) {
        fall_pressure += 48;
    }
    if (contact != NULL && contact->hit) {
        fall_pressure += (int)(clamp_double(contact->penetration_m, 0.0, 0.12) *
                               120.0);
        if (contact->defender_part == PART_HEAD ||
            contact->defender_part == PART_TORSO) {
            fall_pressure += 6;
        }
    }

    stagger_score = raw_damage + (int)(dv * 120.0) +
                    (100 - defender->stability) / 2;
    if (target == PART_HEAD || target == PART_TORSO) {
        stagger_score += 10;
    }
    if (target == PART_L_LEG || target == PART_R_LEG) {
        stagger_score += 14;
    }
    if (contact != NULL && contact->guarded) {
        stagger_score -= 10;
    }
    if (stagger_score >= 96) {
        stagger_state = ROBOT_STAGGER_KNEEL;
    } else if (stagger_score >= 74) {
        stagger_state = ROBOT_STAGGER_STUMBLE;
    } else if (stagger_score >= 48) {
        stagger_state = ROBOT_STAGGER_STEP_BACK;
    } else if (stagger_score >= 28) {
        stagger_state = ROBOT_STAGGER_FLINCH;
    }

    threshold = 70 + defender->stability / 5;
    if (raw_damage >= 24 && (target == PART_HEAD || target == PART_TORSO)) {
        threshold -= 8;
    }
    if (defender->stability < 26 ||
        fall_pressure + rng_range(&fight->rng, 0, 32) > threshold) {
        double fall_impulse = clamp_double(dv * 0.55 + instability * 0.10,
                                           0.06, 0.26);
        int was_down = defender->down;
        int surprise = raw_damage < 18 && defender->stability > 32;
        defender->vx += nx * fall_impulse;
        defender->vy += ny * fall_impulse;
        fall_down_direction(defender, spec->name, nx, ny, target,
                            defender->angular_velocity, out, out_size);
        if (!was_down && defender->down) {
            crowd_react_knockdown(fight, attacker_side, target, surprise);
        }
        fight->clinch = 0;
    } else if (stagger_state != ROBOT_STAGGER_NONE) {
        set_stagger(defender, stagger_state, nx, ny, out, out_size);
    }
}

static double planted_support_score(const RobotState *robot)
{
    double score = 0.0;

    if (foot_can_support(robot, FOOT_LEFT)) {
        score += 0.50;
    }
    if (foot_can_support(robot, FOOT_RIGHT)) {
        score += 0.50;
    }
    if (robot->balance_state == CFA_BALANCE_EDGE) {
        score *= 0.72;
    } else if (robot->balance_state == CFA_BALANCE_OUTSIDE) {
        score *= 0.42;
    }
    return clamp_double(score, 0.0, 1.0);
}

static double clinch_leverage_score(const Fight *fight, int attacker_side)
{
    const RobotState *attacker = &fight->robot[attacker_side];
    const RobotState *defender = &fight->robot[attacker_side == 0 ? 1 : 0];
    double arm_quality =
        (part_support_quality(attacker, PART_L_ARM) +
         part_support_quality(attacker, PART_R_ARM)) * 0.5;
    double foot_score = planted_support_score(attacker);
    double defender_bad_balance =
        clamp_double((100.0 - (double)defender->stability) / 100.0,
                     0.0, 1.0);
    double wall_bonus = wall_gap(defender) < 0.24 ? 0.12 : 0.0;
    double heat_penalty = (double)attacker->heat / 360.0;

    return clamp_double(arm_quality * 0.42 + foot_score * 0.30 +
                            defender_bad_balance * 0.22 + wall_bonus -
                            heat_penalty,
                        0.0, 1.0);
}

static int prepare_throw_mechanics(Fight *fight, int attacker_side,
                                   char *out, size_t out_size)
{
    int defender_side = attacker_side == 0 ? 1 : 0;
    RobotState *attacker = &fight->robot[attacker_side];
    RobotState *defender = &fight->robot[defender_side];
    double nx;
    double ny;
    double leverage = clinch_leverage_score(fight, attacker_side);
    double defender_leverage = clinch_leverage_score(fight, defender_side);
    int chance;
    int roll;
    double torque;

    vector_to_opponent(fight, attacker_side, &nx, &ny);
    chance = clamp_int(36 + (int)(leverage * 62.0) -
                           (int)(defender_leverage * 12.0) -
                           attacker->shock / 10,
                       12, 92);
    roll = rng_range(&fight->rng, 1, 100);
    attacker->clinch_leverage = leverage;
    defender->clinch_pressure = clamp_double(leverage * 0.86, 0.0, 1.0);
    torque = (attacker_side == 0 ? 1.0 : -1.0) *
             clamp_double((leverage - defender_leverage * 0.35) * 0.42,
                          0.04, 0.38);
    attacker->throw_torque = torque * 0.55;
    defender->throw_torque = torque;

    if (roll > chance) {
        fight->clinch = 0;
        attacker->stability = clamp_int(attacker->stability -
                                            clamp_int(8 + (int)((1.0 - leverage) * 18.0),
                                                      8, 24),
                                        0, max_stability(attacker));
        attacker->vx -= nx * 0.09;
        attacker->vy -= ny * 0.09;
        attacker->angular_velocity =
            clamp_abs(attacker->angular_velocity - torque * 0.65,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        set_stagger(attacker, ROBOT_STAGGER_STUMBLE, -nx, -ny,
                    out, out_size);
        append_text(out, out_size,
                    "R%d THROW fails leverage %.0f%% (roll %d/%d). ",
                    attacker_side + 1, leverage * 100.0, roll, chance);
        return 0;
    }

    defender->center_mass_x += nx * clamp_double(0.06 + leverage * 0.14,
                                                 0.06, 0.20);
    defender->center_mass_y += ny * clamp_double(0.06 + leverage * 0.14,
                                                 0.06, 0.20);
    defender->vx += nx * clamp_double(0.08 + leverage * 0.20, 0.08, 0.28);
    defender->vy += ny * clamp_double(0.08 + leverage * 0.20, 0.08, 0.28);
    defender->angular_velocity =
        clamp_abs(defender->angular_velocity + torque,
                  ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    defender->stability =
        clamp_int(defender->stability - clamp_int(10 + (int)(leverage * 28.0),
                                                  10, 38),
                  0, max_stability(defender));
    append_text(out, out_size,
                "R%d THROW loads pivot torque %.0f%% (roll %d/%d). ",
                attacker_side + 1, leverage * 100.0, roll, chance);
    return 1;
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
        double leverage = clinch_leverage_score(fight, attacker_side);
        double defender_leverage = clinch_leverage_score(fight, defender_side);
        double nx;
        double ny;
        double push;

        vector_to_opponent(fight, attacker_side, &nx, &ny);
        push = clamp_double(0.035 + leverage * 0.085, 0.03, 0.13);
        fight->clinch = 1;
        attacker->vx *= 0.25;
        attacker->vy *= 0.25;
        defender->vx *= 0.25;
        defender->vy *= 0.25;
        attacker->vx += nx * push * 0.36;
        attacker->vy += ny * push * 0.36;
        defender->vx += nx * push;
        defender->vy += ny * push;
        attacker->clinch_leverage = leverage;
        defender->clinch_leverage = defender_leverage;
        attacker->clinch_pressure = clamp_double(defender_leverage * 0.56, 0.0, 1.0);
        defender->clinch_pressure = clamp_double(leverage * 0.72, 0.0, 1.0);
        defender->angular_velocity =
            clamp_abs(defender->angular_velocity +
                          (attacker_side == 0 ? 1.0 : -1.0) * leverage * 0.055,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        defender->stability -= 8 + crush;
        defender->shock += crush;
        defender->stability = clamp_int(defender->stability, 0,
                                        max_stability(defender));
        append_text(out, out_size,
                    "R%d CLINCH attaches at %.2fm gap (roll %d/%d), leverage %.0f%% crush %d. ",
                    attacker_side + 1, gap, roll, accuracy,
                    leverage * 100.0, crush);
    } else {
        append_text(out, out_size,
                    "R%d CLINCH misses latch timing (roll %d/%d). ",
                    attacker_side + 1, roll, accuracy);
    }
}

static void mark_attack_miss_recovery(RobotState *attacker,
                                      const MoveSpec *spec,
                                      double severity,
                                      char *out, size_t out_size)
{
    double clamped = clamp_double(severity, 0.0, 1.0);
    int extra = 0;

    set_strike_motion(attacker, ROBOT_STRIKE_PHASE_RECOIL,
                      0.18 + clamped * 0.28,
                      0.34 + clamped * 0.48);
    attacker->stability = clamp_int(attacker->stability -
                                        clamp_int((int)(clamped * 5.0), 1, 6),
                                    0, max_stability(attacker));
    attacker->heat = clamp_int(attacker->heat + (int)(clamped * 3.0), 0, 170);
    attacker->angular_velocity =
        clamp_abs(attacker->angular_velocity +
                      (spec->id == CMD_L_HOOK || spec->id == CMD_LOW_KICK ||
                       spec->id == CMD_HIGH_KICK ? -0.030 : 0.030) *
                          (0.55 + clamped),
                  ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);

    if (spec->id == CMD_LOW_KICK || spec->id == CMD_HIGH_KICK ||
        spec->id == CMD_STOMP || spec->id == CMD_R_HOOK ||
        spec->id == CMD_L_HOOK || spec->id == CMD_THROW) {
        extra = 1;
    }
    if (clamped > 0.72) {
        extra++;
    }
    attacker->recovery_ticks = clamp_int(attacker->recovery_ticks + extra,
                                         0, 4);
    append_text(out, out_size, " overextension recoil %.0f%%;",
                clamped * 100.0);
}

static int apply_grounded_defense(Fight *fight, int defender_side,
                                  CommandId attack_id, BodyPart *target,
                                  char *out, size_t out_size)
{
    RobotState *defender = &fight->robot[defender_side];
    double arm_quality = fmax(part_support_quality(defender, PART_L_ARM),
                              part_support_quality(defender, PART_R_ARM));
    double leg_quality = fmax(part_support_quality(defender, PART_L_LEG),
                              part_support_quality(defender, PART_R_LEG));
    int chance;
    int roll;
    int reduction = 0;
    double nx;
    double ny;
    double lateral;

    if (!defender->down ||
        !(attack_id == CMD_STOMP || attack_id == CMD_LOW_KICK ||
          attack_id == CMD_HIGH_KICK || attack_id == CMD_KNEE)) {
        return 0;
    }

    chance = 16 + (int)(arm_quality * 26.0) + (int)(leg_quality * 16.0);
    if (defender->guard) {
        chance += 18;
    }
    if (defender->getup_state == CFA_GETUP_ROLL_SIDE ||
        defender->getup_state == CFA_GETUP_BRACE_HAND) {
        chance += 10;
    }
    chance -= defender->shock / 10;
    chance -= clamp_int((int)(defender->ground_slide * 18.0), 0, 12);
    chance = clamp_int(chance, 6, 78);
    roll = rng_range(&fight->rng, 1, 100);

    vector_to_opponent(fight, defender_side, &nx, &ny);
    lateral = defender_side == 0 ? -1.0 : 1.0;
    defender->grounded_defense =
        clamp_double((double)chance / 100.0, 0.0, 1.0);
    defender->grounded_roll =
        lateral * clamp_double(0.04 + defender->grounded_defense * 0.14,
                               0.04, 0.18);
    defender->ground_contact_mask |= CFA_GROUND_TORSO;

    if (roll <= chance) {
        BodyPart shield = guard_arm(defender);
        reduction = clamp_int(28 + chance / 3, 28, 58);
        defender->vx -= nx * (0.04 + leg_quality * 0.05);
        defender->vy -= ny * (0.04 + leg_quality * 0.05);
        defender->angular_velocity =
            clamp_abs(defender->angular_velocity + defender->grounded_roll * 0.48,
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
        if (shield != PART_INVALID &&
            (*target == PART_HEAD || *target == PART_TORSO)) {
            *target = shield;
            defender->ground_contact_mask |=
                shield == PART_L_ARM ? CFA_GROUND_L_HAND : CFA_GROUND_R_HAND;
        } else {
            defender->ground_contact_mask |=
                leg_quality > 0.0 ?
                    (CFA_GROUND_L_FOOT | CFA_GROUND_R_FOOT) :
                    (CFA_GROUND_L_HAND | CFA_GROUND_R_HAND);
        }
        append_text(out, out_size,
                    "R%d grounded cover rolls the stomp line (roll %d/%d, reduce %d%%). ",
                    defender_side + 1, roll, chance, reduction);
    } else {
        reduction = defender->guard ? 12 : 0;
        append_text(out, out_size,
                    "R%d grounded cover is late (roll %d/%d). ",
                    defender_side + 1, roll, chance);
    }
    return reduction;
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
    int grounded_reduction = 0;
    BlockResult block;
    StrikeContact strike_contact;

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
    set_attack_footwork(attacker, intent->command.id);
    attacker->attack_released = 1;
    if (attack_recovery_ticks(intent->command.id) > attacker->recovery_ticks) {
        attacker->recovery_ticks = attack_recovery_ticks(intent->command.id);
    }
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

    if (intent->command.id == CMD_THROW &&
        !prepare_throw_mechanics(fight, attacker_side, out, out_size)) {
        return;
    }

    if (gap < spec->min_gap_m || gap > spec->max_gap_m) {
        double range_mid = (spec->min_gap_m + spec->max_gap_m) * 0.5;
        double miss_severity = clamp_double(fabs(gap - range_mid) /
                                                fmax(0.20, spec->max_gap_m),
                                            0.20, 0.92);
        append_text(out, out_size,
                    "R%d %s misses by geometry (gap %.2fm, reach %.2f-%.2fm). ",
                    attacker_side + 1, spec->name, gap, spec->min_gap_m,
                    spec->max_gap_m);
        mark_attack_miss_recovery(attacker, spec, miss_severity,
                                  out, out_size);
        return;
    }

    target = select_target(defender, intent->command, spec);
    if (defender->down && intent->command.id == CMD_STOMP &&
        intent->command.target == PART_INVALID) {
        target = rng_range(&fight->rng, 0, 3) == 0 ? PART_HEAD : PART_TORSO;
    }
    grounded_reduction = apply_grounded_defense(fight, defender_side,
                                                intent->command.id, &target,
                                                out, out_size);
    block = attempt_block(fight, defender_side, target, spec,
                          intent->command.id, gap);
    strike_contact = find_strike_contact(fight, attacker_side,
                                         intent->command.id, target, &block);
    if (!strike_contact.hit && strike_contact.clearance_m > 0.18) {
        append_text(out, out_size,
                    "R%d %s misses by swept capsule geometry against %s (clear %.2fm). ",
                    attacker_side + 1, spec->name, part_name(target),
                    strike_contact.clearance_m);
        mark_attack_miss_recovery(
            attacker, spec,
            clamp_double(strike_contact.clearance_m * 1.6, 0.18, 0.95),
            out, out_size);
        return;
    }
    if (strike_contact.hit &&
        strike_contact.defender_part != target &&
        !(block.attempted && block.success)) {
        append_text(out, out_size,
                    "R%d %s surface contact redirects %s to %s. ",
                    attacker_side + 1, spec->name, part_name(target),
                    part_name(strike_contact.defender_part));
        target = strike_contact.defender_part;
    }
    reach_penalty = fabs(gap - (spec->min_gap_m + spec->max_gap_m) * 0.5) * 24.0;
    accuracy = spec->accuracy + (attacker->stability - 75) / 3 -
               attacker->heat / 20 - (int)reach_penalty;
    if (strike_contact.hit) {
        accuracy += 5;
    } else {
        accuracy -= clamp_int((int)(strike_contact.clearance_m * 120.0), 0, 28);
    }
    if (defender->retreating) {
        accuracy -= 16;
    }
    if (defender->advancing) {
        accuracy += 4;
    }
    if (defender->circling) {
        accuracy -= 8;
    }
    if (block.attempted) {
        accuracy -= block.accuracy_penalty;
    } else if (defender->guard && !defender->down) {
        accuracy -= 4;
    }
    if (defender->down) {
        accuracy += intent->command.id == CMD_STOMP ? 24 : 10;
        accuracy -= grounded_reduction / 3;
        if (intent->command.id == CMD_STOMP &&
            attacker->balance_state == CFA_BALANCE_OUTSIDE) {
            accuracy -= 12;
        }
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
        if (block.attempted && block.success) {
            append_text(out, out_size,
                        "R%d %s tracks the line with %s (block roll %d/%d). ",
                        defender_side + 1, part_name(block.shield),
                        part_name(block.target), block.roll, block.chance);
        }
        if ((strike_contact.clearance_m <= ROBOT_STRIKE_CONTACT_MARGIN_M * 2.0) ||
            (block.attempted && block.success)) {
            double miss_yaw = attacker_side == 0 ? 0.018 : -0.018;
            attacker->stability = clamp_int(attacker->stability - 2, 0,
                                            max_stability(attacker));
            attacker->heat = clamp_int(attacker->heat + 1, 0, 170);
            attacker->angular_velocity =
                clamp_abs(attacker->angular_velocity + miss_yaw,
                          ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
            append_text(out, out_size, "near-line recovery penalty. ");
            mark_attack_miss_recovery(
                attacker, spec,
                block.attempted && block.success ? 0.55 : 0.35,
                out, out_size);
        } else {
            mark_attack_miss_recovery(attacker, spec, 0.22,
                                      out, out_size);
        }
        return;
    }

    raw_damage = spec->damage + rng_range(&fight->rng, -3, 6) +
                 (attacker->stability - 80) / 12 - attacker->heat / 35;
    if (defender->down && intent->command.id == CMD_STOMP) {
        raw_damage += 4;
    }
    if (grounded_reduction > 0) {
        raw_damage = clamp_int(raw_damage * (100 - grounded_reduction) / 100,
                               1, 112);
    }
    raw_damage = clamp_int(raw_damage, 1, 112);
    if (strike_contact.hit && strike_contact.glancing) {
        double retained = strike_contact.guarded
            ? 0.48
            : 0.56 + clamp_double(strike_contact.angle_cos, 0.0, 1.0) * 0.30;
        raw_damage = clamp_int((int)((double)raw_damage * retained), 1, 112);
        attacker->stability = clamp_int(attacker->stability -
                                            (strike_contact.guarded ? 1 : 3),
                                        0, max_stability(attacker));
        attacker->heat = clamp_int(attacker->heat + 2, 0, 170);
        attacker->angular_velocity =
            clamp_abs(attacker->angular_velocity +
                          (attacker_side == 0 ? 0.022 : -0.022),
                      ROBOT_MAX_ANGULAR_SPEED_RAD_PER_TURN);
    }

    append_text(out, out_size,
                "R%d %s hits %s (roll %d/%d, gap %.2fm), raw %d;",
                attacker_side + 1, spec->name, part_name(target), roll,
                accuracy, gap, raw_damage);
    crowd_react_hit(fight, attacker_side, target, raw_damage);
    if (strike_contact.hit && strike_contact.guarded) {
        append_text(out, out_size, " physical guard intercept %.0f%%;",
                    strike_contact.angle_cos * 100.0);
    } else if (strike_contact.hit && strike_contact.glancing) {
        append_text(out, out_size, " glancing angle %.0f%%;",
                    strike_contact.angle_cos * 100.0);
    }
    apply_damage(fight, defender_side, target, raw_damage, &block, out,
                 out_size);
    if (strike_contact.hit && strike_contact.clearance_m < 98.0) {
        set_recent_damage_site(defender, target, strike_contact.defender_point);
    }
    apply_knockback(fight, attacker_side, target, raw_damage, spec,
                    &strike_contact, out, out_size);
    append_text(out, out_size, ". ");

    if (intent->command.id == CMD_THROW && fight->clinch) {
        double throw_x;
        double throw_y;
        int was_down = defender->down;

        fight->clinch = 0;
        vector_to_opponent(fight, attacker_side, &throw_x, &throw_y);
        fall_down_direction(defender, "THROW release", throw_x, throw_y,
                            PART_TORSO, defender->angular_velocity,
                            out, out_size);
        if (!was_down && defender->down) {
            crowd_react_knockdown(fight, attacker_side, PART_TORSO, 0);
        }
        append_text(out, out_size, " Throw releases clinch. ");
    }

    evaluate_defeat(defender);
}

static void append_robot_commentary(const Fight *fight, int side,
                                    char *out, size_t out_size)
{
    const RobotState *robot = &fight->robot[side];
    const char *label = side == 0 ? "R1" : "R2";

    if (robot->head_detached) {
        append_text(out, out_size,
                    "%s critical head damage: detached-head knockout. ",
                    label);
        return;
    }
    if (robot->block_success) {
        append_text(out, out_size, "%s blocked %d through %s guard. ",
                    label, robot->block_amount, part_name(robot->block_arm));
    } else if (robot->block_active) {
        append_text(out, out_size, "%s guard tracks the incoming line. ",
                    label);
    }
    if (robot->parry_active) {
        append_text(out, out_size, "%s parries and redirects momentum. ",
                    label);
    }
    if (robot->stagger_state > 0 && robot->stagger_progress < 0.95) {
        append_text(out, out_size, "%s staggered %.0f%% off balance. ",
                    label, robot->stagger_progress * 100.0);
    }
    if (robot->wall_impulse > 0.10) {
        append_text(out, out_size,
                    "%s driven against the cage, impulse %.2f. ",
                    label, robot->wall_impulse);
    }
    if (robot->ground_impact > 0.16 && robot->ground_settle > 0.95) {
        append_text(out, out_size,
                    "%s hard knockdown on %s, slide %.2fm/t. ",
                    label, part_name(robot->ground_impact_part),
                    robot->ground_slide);
    }
    if (robot->getup_state == CFA_GETUP_GUARD_RESET &&
        robot->getup_progress > 0.45 && !robot->down) {
        append_text(out, out_size, "%s back to guard after recovery. ",
                    label);
    } else if (robot->getup_state > CFA_GETUP_NONE && robot->down) {
        append_text(out, out_size,
                    "%s get-up uses support mask %d under %.0f%% pressure. ",
                    label, robot->getup_support_mask,
                    robot->getup_pressure * 100.0);
    }
    if (robot->recent_damage_part != PART_INVALID &&
        robot->recent_damage_ticks > 0 &&
        robot->recent_damage_raw + robot->recent_damage_net > 18) {
        append_text(out, out_size,
                    "%s visible %s damage pulse raw %d net %d. ",
                    label, part_name(robot->recent_damage_part),
                    robot->recent_damage_raw, robot->recent_damage_net);
    }
}

static void append_commentary_cues(const Fight *fight, char *out,
                                   size_t out_size)
{
    append_robot_commentary(fight, 0, out, out_size);
    append_robot_commentary(fight, 1, out, out_size);
    if (fight->crowd.gasp > 0.40) {
        append_text(out, out_size, " Crowd gasps. ");
    } else if (fight->crowd.cheer > 0.55) {
        append_text(out, out_size, " Crowd roars. ");
    } else if (fight->crowd.clap > 0.45) {
        append_text(out, out_size, " Crowd claps. ");
    }
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
    if (robot->head_detached) {
        printf(" -HEAD");
    }
    printf("\n");
}

static void print_crowd_status(const CrowdState *crowd)
{
    const char *chant = "-";

    if (crowd->chant_side == 0) {
        chant = "R1";
    } else if (crowd->chant_side == 1) {
        chant = "R2";
    }

    printf("  Crowd energy %.2f cheer %.2f clap %.2f gasp %.2f chant %.2f %s\n",
           crowd->energy,
           crowd->cheer,
           crowd->clap,
           crowd->gasp,
           crowd->chant,
           chant);
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
        decay_crowd(&fight.crowd);

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
        append_commentary_cues(&fight, event, sizeof(event));

        if (verbose) {
            printf("T%03d gap%.2f center%.2f C%d | R1 %-10s R2 %-10s | %s\n",
                   turn, surface_gap(&fight), center_distance(&fight),
                   fight.clinch, intent[0].spec->name, intent[1].spec->name,
                   event);
            if (turn == 1 || turn % 8 == 0 ||
                fight.robot[0].defeated || fight.robot[1].defeated) {
                print_status("  R1", &fight.robot[0]);
                print_status("  R2", &fight.robot[1]);
                print_crowd_status(&fight.crowd);
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
        result.winner = -1;
        copy_text(result.method, sizeof(result.method),
                  "time limit expired without knockout");
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
        print_crowd_status(&fight.crowd);
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
    int turn_limit;
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

static void copy_part_damage(CFAPartDamage *snapshot,
                             const RobotState *robot,
                             BodyPart part)
{
    int lost;

    memset(snapshot, 0, sizeof(*snapshot));
    if (part < 0 || part >= PART_COUNT) {
        return;
    }

    lost = part_initial[part] - robot->integrity[part];
    snapshot->scuff = clamp_int(robot->scuff[part], 0, 100);
    snapshot->dent = clamp_int(robot->dent[part], 0, 100);
    snapshot->exposed = clamp_int(lost * 100 / part_initial[part], 0, 100);
    if (robot->detached[part] || (part == PART_HEAD && robot->head_detached)) {
        snapshot->exposed = 100;
    }
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
    snapshot->angularVelocity = robot->angular_velocity;
    snapshot->wallGap = wall_gap(robot);
    snapshot->centerMassX = robot->center_mass_x;
    snapshot->centerMassY = robot->center_mass_y;
    snapshot->centerMassZ = robot->center_mass_z;
    snapshot->supportCenterX = robot->support_center_x;
    snapshot->supportCenterY = robot->support_center_y;
    snapshot->balanceOffset = robot->balance_offset;
    snapshot->balanceSupported = robot->balance_supported;
    snapshot->balanceState = robot->balance_state;
    snapshot->supportRadius = robot->support_radius;
    snapshot->leftFootX = robot->foot_x[FOOT_LEFT];
    snapshot->leftFootY = robot->foot_y[FOOT_LEFT];
    snapshot->rightFootX = robot->foot_x[FOOT_RIGHT];
    snapshot->rightFootY = robot->foot_y[FOOT_RIGHT];
    snapshot->leftFootTargetX = robot->foot_target_x[FOOT_LEFT];
    snapshot->leftFootTargetY = robot->foot_target_y[FOOT_LEFT];
    snapshot->rightFootTargetX = robot->foot_target_x[FOOT_RIGHT];
    snapshot->rightFootTargetY = robot->foot_target_y[FOOT_RIGHT];
    snapshot->leftFootState = robot->foot_state[FOOT_LEFT];
    snapshot->rightFootState = robot->foot_state[FOOT_RIGHT];
    snapshot->leftFootPhase = robot->foot_phase[FOOT_LEFT];
    snapshot->rightFootPhase = robot->foot_phase[FOOT_RIGHT];
    snapshot->swingFoot = robot->swing_foot;
    snapshot->pivotFoot = robot->pivot_foot;
    snapshot->pivotAngle = robot->pivot_angle;
    snapshot->wallBraced = robot->wall_braced;
    snapshot->wallNormalX = robot->wall_nx;
    snapshot->wallNormalY = robot->wall_ny;
    snapshot->lastImpactPart = robot->last_impact_part;
    snapshot->lastImpactDamage = robot->last_impact_damage;
    snapshot->blocking = robot->block_active;
    snapshot->blockSuccess = robot->block_success;
    snapshot->blockArm = robot->block_arm;
    snapshot->blockPart = robot->block_part;
    snapshot->blockAmount = robot->block_amount;
    snapshot->blockReaction = robot->block_reaction;
    snapshot->parryActive = robot->parry_active;
    snapshot->guardSide = robot->guard_side;
    snapshot->guardHandHeight = robot->guard_hand_height;
    snapshot->guardElbowAngle = robot->guard_elbow_angle;
    snapshot->guardCoverage = robot->guard_coverage_m;
    snapshot->windupCommand = robot->windup_command.id;
    snapshot->windupProgress = robot->windup_progress;
    snapshot->strikeReleased = robot->attack_released;
    snapshot->recoveryTicks = robot->recovery_ticks;
    snapshot->strikePhase = robot->strike_phase;
    snapshot->followThrough = robot->follow_through;
    snapshot->recoil = robot->recoil;
    snapshot->staggerState = robot->stagger_state;
    snapshot->staggerProgress = robot->stagger_progress;
    snapshot->staggerDirectionX = robot->stagger_dir_x;
    snapshot->staggerDirectionY = robot->stagger_dir_y;
    snapshot->fallProgress = robot->fall_progress;
    snapshot->fallDirectionX = robot->fall_dir_x;
    snapshot->fallDirectionY = robot->fall_dir_y;
    snapshot->fallAngularVelocity = robot->fall_angular_velocity;
    snapshot->fallContactPart = robot->fall_contact_part;
    snapshot->groundContactMask = robot->ground_contact_mask;
    snapshot->groundImpactPart = robot->ground_impact_part;
    snapshot->groundImpact = robot->ground_impact;
    snapshot->groundSlide = robot->ground_slide;
    snapshot->groundSettle = robot->ground_settle;
    snapshot->wallImpulse = robot->wall_impulse;
    snapshot->wallFlex = robot->wall_flex;
    snapshot->getUpState = robot->getup_state;
    snapshot->getUpProgress = robot->getup_progress;
    snapshot->getUpBlocked = robot->getup_blocked;
    snapshot->getUpForceX = robot->getup_force_x;
    snapshot->getUpForceY = robot->getup_force_y;
    snapshot->getUpForceZ = robot->getup_force_z;
    snapshot->getUpSupportMask = robot->getup_support_mask;
    snapshot->getUpPressure = robot->getup_pressure;
    snapshot->groundedDefense = robot->grounded_defense;
    snapshot->groundedRoll = robot->grounded_roll;
    snapshot->clinchLeverage = robot->clinch_leverage;
    snapshot->clinchPressure = robot->clinch_pressure;
    snapshot->throwTorque = robot->throw_torque;
    snapshot->recentDamagePart = robot->recent_damage_part;
    snapshot->recentDamageRaw = robot->recent_damage_raw;
    snapshot->recentDamageNet = robot->recent_damage_net;
    snapshot->recentDamageX = robot->recent_damage_x;
    snapshot->recentDamageY = robot->recent_damage_y;
    snapshot->recentDamageZ = robot->recent_damage_z;
    snapshot->headDetached = robot->head_detached;
    snapshot->headDetachX = robot->head_detach_x;
    snapshot->headDetachY = robot->head_detach_y;
    snapshot->headDetachZ = robot->head_detach_z;
    snapshot->headDetachVX = robot->head_detach_vx;
    snapshot->headDetachVY = robot->head_detach_vy;
    snapshot->headDetachVZ = robot->head_detach_vz;
    snapshot->headDetachSpin = robot->head_detach_spin;
    copy_part_damage(&snapshot->headDamage, robot, PART_HEAD);
    copy_part_damage(&snapshot->torsoDamage, robot, PART_TORSO);
    copy_part_damage(&snapshot->leftArmDamage, robot, PART_L_ARM);
    copy_part_damage(&snapshot->rightArmDamage, robot, PART_R_ARM);
    copy_part_damage(&snapshot->leftLegDamage, robot, PART_L_LEG);
    copy_part_damage(&snapshot->rightLegDamage, robot, PART_R_LEG);
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

    bout->winner = -1;
    copy_text(bout->result_method, sizeof(bout->result_method),
              "time limit expired without knockout");
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
    snapshot->crowd.energy = bout->fight.crowd.energy;
    snapshot->crowd.cheer = bout->fight.crowd.cheer;
    snapshot->crowd.clap = bout->fight.crowd.clap;
    snapshot->crowd.gasp = bout->fight.crowd.gasp;
    snapshot->crowd.chant = bout->fight.crowd.chant;
    snapshot->crowd.chantSide = bout->fight.crowd.chant_side;

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
    if (bout->turn_limit <= 0) {
        bout->turn_limit = MAX_TURNS;
    }
    bout->score[0] = structural_score(&bout->fight.robot[0]);
    bout->score[1] = structural_score(&bout->fight.robot[1]);
    bout->cursor[0] = 0;
    bout->cursor[1] = 0;
    bout->result_method[0] = '\0';
    memset(bout->last_intent, 0, sizeof(bout->last_intent));
    snprintf(bout->last_event, sizeof(bout->last_event),
             "Bout loaded: %s versus %s.", bout->left.name, bout->right.name);
}

void cfa_bout_set_turn_limit(CFABout *bout, int max_turns)
{
    if (bout == NULL) {
        return;
    }
    bout->turn_limit = max_turns > 0 ? max_turns : MAX_TURNS;
}

void cfa_bout_finish_time_limit(CFABout *bout)
{
    if (bout == NULL || bout->finished) {
        return;
    }
    snprintf(bout->last_event, sizeof(bout->last_event),
             "One-hour time limit expired without knockout.");
    finish_bout(bout);
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
    decay_crowd(&bout->fight.crowd);

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
    append_commentary_cues(&bout->fight, event, sizeof(event));

    bout->last_intent[0] = intent[0];
    bout->last_intent[1] = intent[1];
    if (event[0] == '\0') {
        snprintf(event, sizeof(event), "Bodies coast under friction.");
    }
    copy_text(bout->last_event, sizeof(bout->last_event), event);
    bout->score[0] = structural_score(&bout->fight.robot[0]);
    bout->score[1] = structural_score(&bout->fight.robot[1]);

    if (bout->fight.robot[0].defeated || bout->fight.robot[1].defeated ||
        bout->turn >= bout->turn_limit) {
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

int cfa_bout_get_robot_capsules(const CFABout *bout,
                                int side,
                                CFACapsuleSnapshot *capsules,
                                size_t capacity)
{
    RobotCapsules source;
    size_t count;
    size_t i;

    if (bout == NULL || capsules == NULL || capacity == 0 ||
        side < 0 || side > 1) {
        return 0;
    }

    build_robot_capsules(&bout->fight.robot[side], &source);
    count = (size_t)source.count;
    if (count > capacity) {
        count = capacity;
    }

    for (i = 0; i < count; i++) {
        capsules[i].part = source.items[i].part;
        capsules[i].ax = source.items[i].a.x;
        capsules[i].ay = source.items[i].a.y;
        capsules[i].az = source.items[i].a.z;
        capsules[i].bx = source.items[i].b.x;
        capsules[i].by = source.items[i].b.y;
        capsules[i].bz = source.items[i].b.z;
        capsules[i].radius = source.items[i].radius;
        capsules[i].massKg = source.items[i].mass_kg;
    }
    return (int)count;
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

double cfa_part_mass_kg(int part)
{
    if (part < 0 || part >= PART_COUNT) {
        return 0.0;
    }
    return part_mass_kg[part];
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
                    stoppages[i]++;
                } else if (result_a.winner == 1) {
                    wins[j]++;
                    losses[i]++;
                    stoppages[j]++;
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
                    stoppages[j]++;
                } else if (result_b.winner == 1) {
                    wins[i]++;
                    losses[j]++;
                    stoppages[i]++;
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
