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

typedef struct {
    int head;
    int torso;
    int leftArm;
    int rightArm;
    int leftLeg;
    int rightLeg;
    int processor;
    int shock;
    int stability;
    int heat;
    int guarding;
    int retreating;
    int advancing;
    int circling;
    int down;
    int downTicks;
    int defeated;
    int leftArmDetached;
    int rightArmDetached;
    int leftLegDetached;
    int rightLegDetached;
    double x;
    double y;
    double vx;
    double vy;
    double facing;
    double wallGap;
    double leftFootX;
    double leftFootY;
    double rightFootX;
    double rightFootY;
    int swingFoot;
    int pivotFoot;
    int wallBraced;
    double wallNormalX;
    double wallNormalY;
    int lastImpactPart;
    int lastImpactDamage;
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

const char *cfa_part_name(int part);
int cfa_part_initial(int part);
int cfa_part_armor(int part);
const char *cfa_command_name(int command_id);

#ifdef __cplusplus
}
#endif

#endif
