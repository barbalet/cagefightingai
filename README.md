# Cage Fighting AI Robot

This repository defines a deterministic C simulation for identical humanoid cage-fighting robots. Each robot has the same chassis, mass properties, armor, actuators, sensors, and failure thresholds. The only competitive difference is the command program loaded into the head-resident control processor.

The simulator is intended to evaluate command sets against one another. A command set is a cyclic list of movement and strike commands. During a bout the runtime resolves range, timing, hit probability, structural damage, processor damage, shock accumulation, detachment events, and stoppage conditions.

## Build

```sh
make
```

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
Headhunter                10     8     0        8       67
Limb Breaker               2    16     0        0       84
Clinch Driver              9     9     0        1      116
Counter Guard             15     3     0       15       40
```

Print the movement command table exposed by the robot operating system:

```sh
make moves
```

## Reference Hardware Specification

The simulated platform is the `CFM-1` humanoid cage-fighting test article.

| Subsystem | Specification |
| --- | --- |
| Architecture | Bipedal humanoid, bilateral upper and lower limbs, single armored head module |
| Command processing | All tactical command processing, command sequencing, perception fusion, and bout state estimation run in the head module |
| Distributed electronics | Limbs contain only low-level motor controllers, joint encoders, thermal sensors, and current-limit boards |
| Processor failure behavior | Loss of the head processor terminates command execution immediately |
| Torso role | Battery pack, power bus, inertial reference, and structural spine |
| Limb role | Actuated striking, guarding, clinch attachment, posture recovery, and mobility |
| Armor model | Head, torso, arms, and legs use separate armor constants in the simulator |
| Attachment model | Arms and legs can detach when structural integrity crosses zero |
| Combat boundary | Two-robot enclosed cage; no weapons, projectiles, tools, or environmental hazards |

Initial structural values:

| Body part | Initial integrity | Armor | Simulation function |
| --- | ---: | ---: | --- |
| Head | 100 | 12 | Processor, sensor fusion, tactical control |
| Torso | 160 | 18 | Power bus, trunk frame, balance reference |
| Left arm | 90 | 9 | Guarding, jabs, hooks, clinch control |
| Right arm | 90 | 9 | Crosses, hooks, clinch control |
| Left leg | 110 | 11 | Mobility, low kicks, knees, stomps |
| Right leg | 110 | 11 | Mobility, high kicks, knees, stomps |

The hardware is intentionally symmetric. A command set cannot change actuator power, armor, integrity, sensor range, or processor location. Tactical performance comes from command order, target selection, range control, heat management, and damage prioritization.

## Robot Operating System Command Surface

Command programs use `.cfos` files. The file is line oriented:

```text
name: Program Name
COMMAND
COMMAND TARGET
```

Blank lines and text after `#` are ignored. Commands loop until the bout stops or the maximum turn count is reached.

Example:

```text
name: Processor Pressure

ADVANCE
L_JAB HEAD
R_CROSS HEAD
GUARD
LOW_KICK R_LEG
R_CROSS HEAD
```

Supported targets:

```text
HEAD
TORSO
L_ARM
R_ARM
L_LEG
R_LEG
```

Supported movement and attack commands:

| Command | Function |
| --- | --- |
| `GUARD` | Raise arm structure around head and torso; improves immediate damage reduction |
| `ADVANCE` | Close one range band |
| `RETREAT` | Open one range band and increase evasion for the current tick |
| `RESET` | Break contact, leave clinch, and return to outside range |
| `L_JAB` | Fast left-arm linear strike |
| `R_CROSS` | Rear straight punch with higher transfer |
| `L_HOOK` | Short left arc strike |
| `R_HOOK` | Short right arc strike |
| `UPPERCUT` | Close vertical head strike using an available arm |
| `LOW_KICK` | Low-line leg attack using an available leg |
| `HIGH_KICK` | High-energy right-leg head attack |
| `KNEE` | Close piston strike to torso or head |
| `ELBOW` | Compact close strike using an available arm |
| `CLINCH` | Attach both arms and force distance zero |
| `THROW` | Clinch-range rotational takedown |
| `STOMP` | Downward close-range strike against leg or torso structure |

The simulator also accepts aliases such as `JAB`, `CROSS`, `HOOK`, and `KICK`.

## Simulation Model

Each turn resolves in this order:

1. Clear transient guard, retreat, and advance flags.
2. Recover a small amount of heat, shock, and stability.
3. Read the next command from each program.
4. Convert impossible commands to `GUARD` if required actuators have detached or thermal clamp is active.
5. Apply position commands.
6. Resolve active attacks in timing order.
7. Apply armor, guard transfer, structural damage, processor damage, shock, stability loss, and detachment events.
8. Check stoppage conditions.

Range is discrete:

| Range | Meaning |
| ---: | --- |
| 0 | Clinch or frame contact |
| 1 | Pocket striking range |
| 2 | Outside range |

Stoppage methods:

| Method | Condition |
| --- | --- |
| Processor kill | Head integrity or head processor reaches zero |
| Technical knockout | Torso power bus reaches zero |
| Knockout | Cranial shock reaches watchdog reset threshold |
| Mobility kill | Both legs detach |
| Decision | Maximum turn count reached; higher remaining structural score wins |

## Included Command Sets

| File | Doctrine |
| --- | --- |
| `command_sets/headhunter.cfos` | Close range and repeatedly attack the head processor |
| `command_sets/limb_breaker.cfos` | Attack legs and arms before finishing upstairs |
| `command_sets/clinch_driver.cfos` | Compress distance, clinch, knee, elbow, and throw |
| `command_sets/counter_guard.cfos` | Guard and retreat to force misses, then counter with efficient strikes |

## Graphic Fight Examples

The following diagrams show the type of events represented by the simulator logs. They are structural telemetry examples, not separate rules.

### Example 1: Processor Attack

```text
Range 1: pocket

R1 Headhunter                         R2 Counter Guard

     [HEAD: 100]                           [HEAD: 100]
        |                                      |
   LA --+-- RA       R_CROSS HEAD  ===>   LA -+-- RA
        |                                      |
      LL RL                                  LL RL

Effect:
  raw strike energy is reduced by head armor
  net damage reduces HEAD integrity
  part of net head damage reduces PROCESSOR health
  head impact increases cranial shock
```

### Example 2: Guard Transfer

```text
R1 strike path: R_CROSS HEAD

Before guard:
  incoming load -> HEAD

With GUARD active:
  incoming load -> ARM SHIELD -> HEAD

Telemetry effect:
  guard shunts a portion of damage into L_ARM or R_ARM
  remaining damage is reduced before reaching HEAD or TORSO
```

### Example 3: Limb Removal

```text
Range 1: pocket

R1 Limb Breaker                         R2 Headhunter

 LOW_KICK L_LEG  ===>

                                             [HEAD]
                                               |
                                          LA --+-- RA
                                               |
                                          LL: integrity crosses zero
                                          RL

Event:
  L_LEG detached.

Operational result:
  maximum stability is reduced
  commands requiring both legs may convert to GUARD
  if both legs detach, the bout ends by mobility kill
```

### Example 4: Clinch Throw

```text
Range 0: clinch

R1 Clinch Driver                      R2 Limb Breaker

  both arms attached to opponent frame
  torso rotation command loaded

  THROW TORSO
       |
       v
  torso damage + stability loss + range reset to 1
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

Use fixed seeds for regression testing. Change the seed when exploring whether a command set is robust to timing variation.

## Implementation Notes

The simulator is contained in `src/cagefight.c`. It uses only the C standard library and a local deterministic linear congruential generator. No external runtime is required.

The movement table is data driven in `move_specs`. To add a new command, add an enum value, define the corresponding `MoveSpec`, and include any required actuator availability logic if it uses a new limb mode.
