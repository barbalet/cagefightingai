# Cage Fighting AI Robot

This repository defines a deterministic C simulation for identical humanoid cage-fighting robots. Each robot has the same hard-metal body, mass model, armor, actuator availability, processor placement, component health, and failure thresholds. The only competitive difference is the command program loaded into the head-resident control processor.

The current simulator is a physical arena model. Robots have positions, velocities, rigid circular footprints, wall contact, body contact, momentum transfer, knockback, downed states, standing recovery, and component damage. The bodies are not allowed to overlap.

## Build

```sh
make
```

## Xcode

Open `CageFightingAI.xcodeproj` in Xcode and select the `CageFightingAI` scheme. The scheme builds the simulator as a macOS command-line executable and runs from `$(PROJECT_DIR)` so command-set paths resolve correctly.

Default run arguments:

```text
command_sets/headhunter.cfos command_sets/limb_breaker.cfos 42
```

## Run

Run a single bout:

```sh
build/cagefight command_sets/headhunter.cfos command_sets/limb_breaker.cfos 42
```

Run the sample tournament:

```sh
make tournament
```

Expected sample output for seed `42`:

```text
program                 wins  loss  draw     stop    avg_t
Headhunter                17     1     0       17       30
Limb Breaker               2    16     0        1       72
Clinch Driver              9     9     0        9       60
Counter Guard              8    10     0        8       31
```

Print the movement command table:

```sh
make moves
```

## Reference Hardware Specification

The simulated platform is the `CFM-1` humanoid cage-fighting test article.

| Subsystem | Specification |
| --- | --- |
| Architecture | Bipedal humanoid, bilateral upper and lower limbs, single armored head module |
| Physical model | Hard circular body footprint in a circular cage |
| Arena radius | `3.00 m` |
| Robot hard radius | `0.34 m` |
| Robot mass reference | `118 kg` |
| Body contact | Rigid separation solver; robots cannot overlap |
| Cage contact | Position clamp with reflected velocity and wall-impact shock |
| Command processing | All tactical command processing, sequencing, and state evaluation run in the head module |
| Distributed electronics | Limbs contain only low-level motor control, encoders, thermal sensors, and current-limit boards |
| Processor failure behavior | Loss of head processor terminates command execution immediately |
| Torso role | Battery pack, power bus, inertial reference, and structural spine |
| Limb role | Striking, guarding, clinch attachment, movement, posture recovery, and balance |

Initial structural values:

| Body part | Initial integrity | Armor | Simulation function |
| --- | ---: | ---: | --- |
| Head | 100 | 12 | Processor, sensors, tactical control |
| Torso | 160 | 18 | Power bus, trunk frame, balance reference |
| Left arm | 90 | 9 | Guarding, jabs, hooks, clinch control |
| Right arm | 90 | 9 | Crosses, hooks, clinch control |
| Left leg | 110 | 11 | Mobility, low kicks, knees, stomps |
| Right leg | 110 | 11 | Mobility, high kicks, knees, stomps |

The hardware is intentionally symmetric. A command set cannot change actuator power, mass, armor, footprint radius, sensor range, or processor location. Tactical performance comes from movement, spacing, target selection, heat management, and conditional behavior.

## Physical Simulation Model

Each turn resolves in this order:

1. Clear transient movement and guard flags.
2. Recover a small amount of heat, shock, and stability.
3. Evaluate `.cfos` priority conditions.
4. Select the next unconditional command if no condition fires.
5. Apply movement impulses.
6. Step physical state and separate hard-body contacts.
7. Resolve attacks in timing order.
8. Apply damage, guard transfer, knockback, fall checks, and detachment events.
9. Step physical state again after impact.
10. Check stoppage conditions.

Distances are continuous meters:

| Telemetry | Meaning |
| --- | --- |
| `center` | Center-to-center distance between robot bodies |
| `gap` | Surface gap between hard body footprints |
| `wall` | Distance from robot footprint to cage wall |
| `pos(x,y)` | Robot center in the cage plane |
| `v(x,y)` | Current body velocity |
| `DOWN` | Robot is on the floor and must execute `STAND` before normal movement |

No-overlap behavior is enforced by the contact solver. When center distance falls below `2 * ROBOT_RADIUS_M`, the bodies are separated along the collision normal and their velocities are adjusted. High relative contact velocity adds shock and stability loss.

Persistent clinch or contact-lock is also timed. If the robots remain stuck together for `FORCED_MOVE_APART` seconds, currently `2.0`, the simulation clears clinch pressure and forces both bodies to step and impulse apart.

## Robot Operating System Command Surface

Command programs use `.cfos` files. The format supports unconditional cyclic commands and priority conditional rules.

Unconditional command:

```text
COMMAND
COMMAND TARGET
```

Conditional command:

```text
IF METRIC OP VALUE THEN COMMAND
IF METRIC OP VALUE THEN COMMAND TARGET
```

Conditionals are checked from the top of the file every turn. The first true condition runs immediately. If no condition is true, the simulator runs the next unconditional command in cyclic order.

Example:

```text
name: Physical Pressure

IF SELF DOWN == 1 THEN STAND
IF SELF HEAT > 120 THEN GUARD
IF DISTANCE > 0.85 THEN ADVANCE
IF DISTANCE < 0.05 THEN RETREAT
IF OPP HEAD < 35 THEN R_CROSS HEAD
IF OPP DOWN == 1 THEN STOMP TORSO

ADVANCE
L_JAB HEAD
R_CROSS HEAD
CIRCLE_L
LOW_KICK R_LEG
GUARD
```

Supported comparison operators:

```text
< <= > >= == != =
```

Supported conditional metrics:

| Metric | Value |
| --- | --- |
| `SELF HEAD` / `OPP HEAD` | Component integrity |
| `SELF TORSO` / `OPP TORSO` | Component integrity |
| `SELF L_ARM` / `OPP L_ARM` | Component integrity |
| `SELF R_ARM` / `OPP R_ARM` | Component integrity |
| `SELF L_LEG` / `OPP L_LEG` | Component integrity |
| `SELF R_LEG` / `OPP R_LEG` | Component integrity |
| `SELF PROCESSOR` / `OPP PROCESSOR` | Head processor health |
| `SELF HEAT` / `OPP HEAT` | Thermal load |
| `SELF SHOCK` / `OPP SHOCK` | Shock accumulator |
| `SELF STABILITY` / `OPP STABILITY` | Balance and posture reserve |
| `SELF DOWN` / `OPP DOWN` | `1` when down, otherwise `0` |
| `DISTANCE`, `GAP`, or `RANGE` | Surface gap in meters |
| `CENTER_DISTANCE` | Center-to-center distance in meters |
| `WALL` or `CAGE` | Own footprint distance to cage wall in meters |

Supported targets:

```text
HEAD
TORSO
L_ARM
R_ARM
L_LEG
R_LEG
```

Supported movement commands:

| Command | Function |
| --- | --- |
| `GUARD` | Brace frame and raise arms around head and torso |
| `ADVANCE` | Apply body impulse toward opponent |
| `RETREAT` | Apply body impulse away from opponent |
| `STRAFE_L` | Move laterally left while facing opponent |
| `STRAFE_R` | Move laterally right while facing opponent |
| `CIRCLE_L` | Circle left with slight inward pressure |
| `CIRCLE_R` | Circle right with slight inward pressure |
| `RESET` | Break clinch and back out |
| `STAND` | Recover from downed state if lower frame can carry load |

Hands stay in a raised guard posture by default. Incoming head and torso
strikes can trigger dynamic block attempts when an arm actuator is available.
`GUARD` increases block tracking, reduces strike accuracy, and can shunt part
of the impact into the shielding arm. Body shots can be deflected in some
close and mid-range exchanges, though head protection remains the strongest
guard behavior.

Supported attack commands:

| Command | Function |
| --- | --- |
| `L_JAB` | Fast left-arm linear strike |
| `R_CROSS` | Rear straight punch with stronger momentum transfer |
| `L_HOOK` | Short left arc strike |
| `R_HOOK` | Short right arc strike |
| `UPPERCUT` | Close vertical head strike |
| `LOW_KICK` | Low-line leg attack |
| `HIGH_KICK` | High-energy head kick |
| `KNEE` | Contact-range piston strike |
| `ELBOW` | Compact close strike using arm hardpoint |
| `CLINCH` | Attach both arms and constrain separation |
| `THROW` | Clinch-only rotational takedown |
| `STOMP` | Downward strike against close or downed opponent |

## Stoppage Methods

| Method | Condition |
| --- | --- |
| Processor kill | Head integrity or head processor reaches zero |
| Technical knockout | Torso power bus reaches zero |
| Knockout | Cranial shock reaches watchdog reset threshold |
| Mobility kill | Both legs detach |
| Decision | Maximum turn count reached; higher remaining structural and posture score wins |

## Included Command Sets

| File | Doctrine |
| --- | --- |
| `command_sets/headhunter.cfos` | Maintain striking distance, attack head processor, stomp if opponent falls |
| `command_sets/joint_reaper.cfos` | Guarded leg destruction that opens a head finish after Headhunter loses balance |
| `command_sets/shock_clinch.cfos` | Short-range clinch pressure, throws, stomps, and torso/head shock accumulation |
| `command_sets/cross_guard.cfos` | Guarded head-counter plan with hooks, uppercuts, resets, and heat control |
| `command_sets/limb_breaker.cfos` | Damage legs and arms, adapt to damaged lower frame, finish upstairs |
| `command_sets/clinch_driver.cfos` | Enter body contact, clinch, knee, elbow, and throw |
| `command_sets/counter_guard.cfos` | Guard, retreat, strafe, and counter when opponent overcommits |

## Graphic Fight Examples

### Example 1: Hard Body Separation

```text
Before contact solve:

        R1 radius 0.34m          R2 radius 0.34m
             (  overlap  )
             [###][###]

After contact solve:

             [###]  [###]
             gap = 0.00m

Result:
  bodies are separated along the collision normal
  velocities are adjusted
  high relative velocity adds shock and stability loss
```

### Example 2: Strike With Knockback

```text
R1 R_CROSS HEAD

R1 center ----------------------> R2 center
             impact vector

Effects:
  head armor reduces raw damage
  net damage reduces HEAD integrity
  part of net head damage reduces PROCESSOR health
  strike impulse adds velocity to R2 away from R1
  high pressure may set R2 DOWN
```

### Example 3: Wall Interaction

```text
        circular cage wall
       /                  \
      |   R2 knocked back  |
      |        ---> [###]  |
       \__________________/

If the robot footprint crosses the arena boundary:
  position is clamped inside the cage
  outward velocity is reflected
  wall impact may add shock and reduce stability
```

### Example 4: Conditional Component Response

```text
IF SELF L_LEG < 35 THEN RETREAT
IF OPP DOWN == 1 THEN STOMP TORSO
IF DISTANCE > 0.85 THEN ADVANCE

The command processor evaluates these rules every turn before the cyclic plan.
This allows component health and physical position to cause behavior changes.
```

## Extending Command Sets

Create a new `.cfos` file under `command_sets/` and run it against the existing programs:

```sh
build/cagefight command_sets/my_program.cfos command_sets/headhunter.cfos 1001
```

For tournament comparison:

```sh
build/cagefight --tournament 1001 command_sets/*.cfos
```

Use fixed seeds for regression testing. Change the seed when exploring whether a command set is robust to timing and impact variation.

## Implementation Notes

The simulator is contained in `src/cagefight.c`. It uses only the C standard library, `math.h`, and a deterministic local linear congruential generator. No external runtime is required.

The movement table is data driven in `move_specs`. To add a new command, add an enum value, define its `MoveSpec`, and add any special physical handling if it is not a normal movement or strike command.
