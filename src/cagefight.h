#ifndef CAGEFIGHT_H
#define CAGEFIGHT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CFA_MAX_NAME 80
#define CFA_MAX_EVENT 1536
#define CFA_MAX_METHOD 96
#define CFA_MAX_COMMAND_NAME 24
#define CFA_MAX_TURNS 240
#define CFA_MAX_CAPSULES 14

typedef struct CFABout CFABout;

typedef struct {
    int scuff;
    int dent;
    int exposed;
} CFAPartDamage;

typedef enum {
    CFA_PART_HEAD = 0,
    CFA_PART_TORSO,
    CFA_PART_L_ARM,
    CFA_PART_R_ARM,
    CFA_PART_L_LEG,
    CFA_PART_R_LEG,
    CFA_PART_COUNT
} CFAPart;

typedef enum {
    CFA_FOOT_PLANTED = 0,
    CFA_FOOT_LIFTING,
    CFA_FOOT_SWINGING,
    CFA_FOOT_LANDING,
    CFA_FOOT_PIVOTING,
    CFA_FOOT_SLIPPING
} CFAFootState;

typedef enum {
    CFA_BALANCE_SUPPORTED = 0,
    CFA_BALANCE_EDGE,
    CFA_BALANCE_OUTSIDE
} CFABalanceState;

typedef struct {
    int part;
    double ax;
    double ay;
    double az;
    double bx;
    double by;
    double bz;
    double radius;
    double massKg;
} CFACapsuleSnapshot;

typedef struct {
    /* Structural integrity points, mirrored by CFA_PART_* order. */
    int head;
    int torso;
    int leftArm;
    int rightArm;
    int leftLeg;
    int rightLeg;

    /* Core simulation condition. */
    int processor;
    int shock;
    int stability;
    int heat;

    /* Current command/posture flags used by animation and tactics. */
    int guarding;
    int retreating;
    int advancing;
    int circling;

    /* Defeat and recovery state. */
    int down;
    int downTicks;
    int defeated;
    int leftArmDetached;
    int rightArmDetached;
    int leftLegDetached;
    int rightLegDetached;

    /* Arena-space body root state in meters, meters/turn, and radians. */
    double x;
    double y;
    double vx;
    double vy;
    double facing;
    double angularVelocity;
    double wallGap;

    /* Approximate center of mass and stance support in arena-space meters. */
    double centerMassX;
    double centerMassY;
    double centerMassZ;
    double supportCenterX;
    double supportCenterY;
    double balanceOffset;
    int balanceSupported;
    int balanceState;
    double supportRadius;

    /* Foot placement and support hints. Feet are world-space meters. */
    double leftFootX;
    double leftFootY;
    double rightFootX;
    double rightFootY;
    double leftFootTargetX;
    double leftFootTargetY;
    double rightFootTargetX;
    double rightFootTargetY;
    int leftFootState;
    int rightFootState;
    double leftFootPhase;
    double rightFootPhase;
    int swingFoot;
    int pivotFoot;
    double pivotAngle;

    /* Boundary contact response. */
    int wallBraced;
    double wallNormalX;
    double wallNormalY;

    /* Latest contact/block telemetry for rendering, commentary, and debug. */
    int lastImpactPart;
    int lastImpactDamage;
    int blocking;
    int blockSuccess;
    int blockArm;
    int blockPart;
    int blockAmount;
    double blockReaction;
    int parryActive;
    int guardSide;
    double guardHandHeight;
    double guardElbowAngle;
    double guardCoverage;
    int windupCommand;
    double windupProgress;
    int strikeReleased;
    int recoveryTicks;
    int strikePhase;
    double followThrough;
    double recoil;
    int staggerState;
    double staggerProgress;
    double staggerDirectionX;
    double staggerDirectionY;
    double fallProgress;
    double fallDirectionX;
    double fallDirectionY;
    double fallAngularVelocity;
    int fallContactPart;
    int groundContactMask;
    int groundImpactPart;
    double groundImpact;
    double groundSlide;
    double groundSettle;
    double wallImpulse;
    double wallFlex;
    int getUpState;
    double getUpProgress;
    int getUpBlocked;
    double getUpForceX;
    double getUpForceY;
    double getUpForceZ;
    int getUpSupportMask;
    double getUpPressure;
    double groundedDefense;
    double groundedRoll;
    double clinchLeverage;
    double clinchPressure;
    double throwTorque;
    int recentDamagePart;
    int recentDamageRaw;
    int recentDamageNet;
    double recentDamageX;
    double recentDamageY;
    double recentDamageZ;
    int headDetached;
    double headDetachX;
    double headDetachY;
    double headDetachZ;
    double headDetachVX;
    double headDetachVY;
    double headDetachVZ;
    double headDetachSpin;

    /* Accumulated renderer-facing surface damage. */
    CFAPartDamage headDamage;
    CFAPartDamage torsoDamage;
    CFAPartDamage leftArmDamage;
    CFAPartDamage rightArmDamage;
    CFAPartDamage leftLegDamage;
    CFAPartDamage rightLegDamage;
    char method[CFA_MAX_METHOD];
} CFARobotSnapshot;

typedef struct {
    int turn;
    int distance;
    double gap;
    double centerDistance;
    double arenaRadius;
    double robotRadius;
    int clinch;
    int finished;
    int winner;
    int leftScore;
    int rightScore;
    int leftCommandId;
    int rightCommandId;
    int leftTarget;
    int rightTarget;
    char leftName[CFA_MAX_NAME];
    char rightName[CFA_MAX_NAME];
    char leftCommand[CFA_MAX_COMMAND_NAME];
    char rightCommand[CFA_MAX_COMMAND_NAME];
    char event[CFA_MAX_EVENT];
    char resultMethod[CFA_MAX_METHOD];
    CFARobotSnapshot leftRobot;
    CFARobotSnapshot rightRobot;
} CFATurnSnapshot;

CFABout *cfa_bout_create_from_files(const char *left_path,
                                    const char *right_path,
                                    uint32_t seed,
                                    char *error,
                                    size_t error_size);
void cfa_bout_destroy(CFABout *bout);
void cfa_bout_restart(CFABout *bout, uint32_t seed);
int cfa_bout_step(CFABout *bout, CFATurnSnapshot *snapshot);
void cfa_bout_get_snapshot(const CFABout *bout, CFATurnSnapshot *snapshot);
int cfa_bout_is_finished(const CFABout *bout);
int cfa_bout_get_robot_capsules(const CFABout *bout,
                                int side,
                                CFACapsuleSnapshot *capsules,
                                size_t capacity);

const char *cfa_part_name(int part);
int cfa_part_initial(int part);
int cfa_part_armor(int part);
double cfa_part_mass_kg(int part);
const char *cfa_command_name(int command_id);

#ifdef __cplusplus
}
#endif

#endif
