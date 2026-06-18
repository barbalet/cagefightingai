# Open Humanoid Sports Robotics (OHSR)

Historical Legacy Specification

Status: Historical legacy document
Version: 0.1
Scope: Open humanoid robot sport events, demonstrations, and simulations

## 1. Purpose

Open Humanoid Sports Robotics (OHSR) describes a safety-first framework for
humanoid robot competition where the primary contest is balance, timing,
control, tactics, mobility, and resilience rather than destruction. OHSR exists
to make humanoid robot sport inspectable, repeatable, and suitable for public
demonstration without normalizing weaponized systems.

This document is retained as a historical legacy specification for CFA and
related cage-fighting AI experiments. The CFA robots fall within this
specification as humanoid sports robots when configured for bounded arena play,
non-destructive scoring, emergency stop control, declared power systems, and no
weaponized payloads.

OHSR is not a legal standard, safety certification, or substitute for local
law, venue policy, insurance requirements, or engineering review.

## 2. Design Posture

OHSR uses the following posture:

- Sport, not weaponization.
- Non-destructive scoring, not damage-maximizing combat.
- Safety systems are mandatory, not optional match features.
- Inspection is part of the event, not an afterthought.
- Human operators, spectators, officials, and staff remain outside the contest.
- CFA-style humanoid robots are permitted only as sports robots with bounded
  behavior and no weaponized payloads.

Robots may push, evade, brace, feint, grapple within permitted contact rules,
strike within force limits, recover from falls, and compete for position. They
must not be designed, tuned, armed, or scored to injure people, destroy
opponents, breach the arena, ignite materials, or deliver payloads.

## 3. Definitions

Robot: A mobile electromechanical or simulated participant with a humanoid
body plan, including a torso, head or sensor mast, two primary arms, and two
primary legs.

Humanoid sports robot: A robot whose form and behavior are intended for
athletic humanoid competition, not for payload delivery or destructive force.

Match: A bounded period of scored activity between two or more eligible robots.

Arena: The inspected enclosure, mat, ring, cage, or simulated boundary where
the match occurs.

Emergency stop: A tested, authoritative mechanism that places the robot into a
safe state without relying on normal match software.

Weaponized payload: Any mechanism, material, signal, software behavior, or
detachable object intended to injure, burn, cut, puncture, blind, poison,
entangle dangerously, disable electronics, breach containment, or cause
destructive damage.

Non-destructive scoring: A scoring model that awards points for control,
position, balance, technique, permitted contact, defense, and recovery rather
than for breaking the opponent.

## 4. Robot Eligibility

An OHSR robot must:

- Present a recognizable humanoid sports form.
- Use legs, feet, or leg-like lower limbs as its primary locomotion system.
- Keep all active contact surfaces rounded, compliant, guarded, or otherwise
  inspected for safe sports contact.
- Declare all stored energy systems, actuators, radios, autonomy modes, and
  payload-equivalent devices before inspection.
- Include mandatory emergency stop behavior.
- Be configured to remain inside the arena.
- Avoid exposed sharp edges, unguarded pinch points, brittle shattering
  surfaces, unmanaged hot surfaces, or uncontrolled high-speed external parts.

An OHSR robot must not:

- Carry weaponized payloads.
- Use projectiles, blades, spikes, hooks, cutting tools, flame, chemical
  dispersal, smoke generation, explosives, pyrotechnics, high-power lasers,
  electromagnetic attack devices, radio jammers, acoustic weapons, or
  deliberate vision-damaging lights.
- Launch detachable parts except for inspected, passive, non-hazardous
  sacrificial covers explicitly approved by officials.
- Deliberately attack the arena, officials, operators, spectators, chargers,
  fire equipment, or other infrastructure.
- Continue autonomous pursuit after a stop command, arena breach, fall stop, or
  official halt.

## 5. Weight And Height Classes

Events may define stricter classes. If no event-specific classes exist, OHSR
uses the following maximum measured competition mass and standing height:

| Class | Mass | Standing Height | Typical Use |
| --- | ---: | ---: | --- |
| OHSR-Micro | up to 5 kg | up to 0.60 m | tabletop, lab, and education |
| OHSR-Light | over 5 kg to 25 kg | over 0.60 m to 1.20 m | small arena research |
| OHSR-Middle | over 25 kg to 60 kg | over 1.20 m to 1.60 m | controlled public demos |
| OHSR-Heavy | over 60 kg to 120 kg | over 1.60 m to 1.90 m | advanced inspected events |
| OHSR-Exhibition | over 120 kg or over 1.90 m | event-defined | special safety case only |

Mass includes batteries, covers, fluid reservoirs, onboard compute, radios,
guards, and all match-ready parts. Height is measured in the normal upright
ready stance. A robot that exceeds either mass or height for a class moves to
the higher applicable class.

Simulated CFA robots may be recorded as OHSR-Legacy/Synthetic participants.
Physical CFA-derived robots must enter the measured class that matches their
actual mass and height.

## 6. Autonomy And Teleoperation Classes

Robots must declare an autonomy class before inspection:

| Class | Name | Description |
| --- | --- | --- |
| A0 | Passive Demonstration | Powered display, no independent match motion. |
| A1 | Direct Teleoperation | Human commands primary motion continuously. |
| A2 | Assisted Teleoperation | Human directs goals; onboard systems stabilize, balance, and avoid hazards. |
| A3 | Supervised Autonomy | Robot chooses tactics inside approved bounds; human supervisor can halt at any time. |
| A4 | Bounded Full Autonomy | Robot operates within a formally inspected arena model and safety envelope. |
| A5 | Exhibition Autonomy | Higher-risk autonomy requiring event-specific review, logs, and safety case. |

All autonomy classes require emergency stop compliance. Higher autonomy does
not reduce operator responsibility. A robot must fail safe on loss of command
link, localization failure, unsafe thermal state, power anomaly, arena breach
estimate, or emergency stop activation.

## 7. Match Format

An OHSR match should be brief, observable, and resettable. A recommended
baseline format is:

- Three rounds of up to two minutes each.
- One minute minimum technical inspection window between rounds.
- Immediate halt for emergency stop, fire concern, unsafe behavior, arena
  breach, battery alarm, uncontrolled fall, or official intervention.
- Restart only after officials confirm the robots, arena, operators, and safety
  systems are ready.

Events may use shorter rounds for heavier robots or early prototypes.

## 8. Non-Destructive Scoring

OHSR scoring must reward sports performance instead of destruction. A baseline
100-point round may allocate:

- 25 points for balance, stance quality, and fall avoidance.
- 20 points for controlled contact, clean touches, legal strikes, and legal
  grappling entries.
- 20 points for ring position, footwork, pivots, escapes, and boundary control.
- 15 points for defense, guard recovery, impact absorption, and safe disengage.
- 10 points for mobility quality, gait consistency, and recovery after slips.
- 10 points for energy discipline, thermal discipline, and safe tactical
  pacing.

Permitted contact must be controlled and bounded by event force limits. A robot
may score by causing a legal off-balance event, forcing a step-out, gaining a
brief positional advantage, or landing an inspected contact action. A robot
must not receive extra score for breaking an opponent, damaging batteries,
damaging sensors, exposing wiring, creating fire risk, or continuing to strike
after a stop condition.

Officials should penalize:

- Excessive force outside the declared class.
- Repeated attempts to damage joints, batteries, sensors, covers, radios, or
  arena hardware.
- Unsafe falls caused by illegal lifting, twisting, tripping, or entanglement.
- Contact after a halt, fall stop, or emergency stop.
- Weaponized behavior, even if no prohibited hardware is found.

Damage telemetry may be used in CFA simulation as a legacy gameplay proxy, but
physical OHSR scoring must not incentivize destructive damage. Physical damage
should trigger inspection, repair, penalty, or disqualification rather than
serve as a primary scoring objective.

## 9. Emergency Stop Requirements

Every physical OHSR robot must include emergency stop behavior with at least
the following properties:

- A visible, reachable hardware emergency stop control on the robot or a
  tethered safety pendant when direct access is safe.
- A remote emergency stop control available to the operator.
- An official emergency stop path available to event safety staff.
- A fail-safe state that removes or limits actuator power, stops locomotion,
  prevents autonomous restart, and keeps stored energy contained.
- A tested link-loss behavior that enters the same safe state or a stricter
  one.
- A visible or audible indication that emergency stop is active.
- A manual reset process that cannot occur automatically from match software.
- A pre-match test showing that the stop path works from operator and official
  controls.

Emergency stop must not depend solely on the normal AI planner, match script,
or high-level application loop. Robots with high stored energy, high actuator
torque, or high mass should include independent power isolation contactors,
motor controller inhibit lines, watchdogs, and braking or limp strategies
appropriate to the design.

## 10. Battery, Power, And Fire Rules

Battery and power systems must be declared, inspected, and operated under a
fire-aware protocol.

Required battery disclosures:

- Chemistry, nominal voltage, maximum voltage, capacity, and maximum discharge.
- Pack construction, enclosure material, connector type, fuse locations, and
  battery management system details.
- Charger model, charge rate, balance method, and storage procedure.
- Thermal limits and alarm thresholds.
- Emergency disconnect method.

Required battery rules:

- Packs must be physically secured and protected from direct sports contact.
- Packs must use appropriate fusing, current limits, strain relief, and
  insulation.
- Lithium packs must use a battery management system or equivalent monitored
  balancing and protection process approved by inspection.
- Damaged, swollen, punctured, overheated, leaking, or suspicious packs are not
  permitted in the arena.
- Charging must occur only in the designated charging area under supervision.
- Charging equipment must match the declared chemistry and pack configuration.
- Spare packs must be stored in approved fire-resistant containers or areas.
- Robots must halt on battery thermal alarm, over-current alarm, low-voltage
  alarm, smoke, unusual odor, visible swelling, or power instability.

Required fire posture:

- Events must provide fire extinguishers and containment tools appropriate to
  the declared battery chemistries and venue requirements.
- Officials must define a battery quarantine location before matches begin.
- Robots with smoke, flame, battery venting, or uncontrolled heat are stopped
  immediately and handled only under the event fire protocol.
- No robot may intentionally generate flame, smoke, hot debris, sparks, or
  pyrotechnic effects.

## 11. Inspection Protocol

Inspection must occur before a robot enters the arena. Officials may repeat
inspection after repairs, firmware changes, battery replacement, hard impacts,
falls, or abnormal behavior.

### 11.1 Documentation Review

Teams must provide:

- Robot name, team, class, mass, height, and autonomy class.
- Mechanical overview and actuator description.
- Battery and power disclosures.
- Radio frequencies, control links, and link-loss behavior.
- Emergency stop design and reset process.
- Payload declaration confirming no weaponized payloads.
- Software version or script set used for the match.
- Known hazards, repair history, and recent changes.

### 11.2 Physical Inspection

Officials should verify:

- Weight and height class.
- Guards, covers, rounded contact surfaces, and absence of sharp edges.
- Battery mounting, fusing, insulation, and connector security.
- No prohibited payloads or hidden detachable hazardous parts.
- No exposed high-temperature surfaces or fragile shattering materials.
- Feet, hands, knees, elbows, and head surfaces are appropriate for contact.
- Arena compatibility, including stance width, fall envelope, and boundary
  clearance.

### 11.3 Functional Inspection

Officials should test:

- Operator control and declared autonomy mode.
- Low-power gait, stance, braking, and safe fall behavior.
- Emergency stop from operator control.
- Emergency stop from official control.
- Link-loss behavior.
- Battery alarms and visible status indicators where practical.
- Restart process after stop.

### 11.4 Post-Match Inspection

After a match, officials should inspect for:

- Battery heat, swelling, odor, leakage, or physical damage.
- Loose covers, exposed wiring, damaged guards, and cracked structures.
- Actuator overheating or runaway behavior.
- Software or command set changes before any rematch.
- Damage that would convert normal sports contact into sharp, hot, or
  entangling contact.

## 12. Arena And Cage Requirements

The arena must be designed for the declared robot class. A physical cage or
barrier should:

- Keep robots, detached safe covers, and expected debris inside the match area.
- Keep humans outside the fall and reach envelope.
- Withstand expected pushes, falls, and low-speed impacts for the class.
- Include clear official access for emergency stop, fire response, and robot
  removal.
- Provide a non-slip surface compatible with the robots' feet.
- Avoid floor gaps that trap feet or create uncontrolled tripping.
- Include power-on, ready, halt, and reset procedures visible to officials.

For CFA simulations, the cage boundary must be authoritative in the physics
model. A simulated robot should not pass through or fall outside the cage during
normal match operation.

## 13. Allowed And Prohibited Contact

Allowed contact may include:

- Controlled pushes with arms, shoulders, torso, or inspected hands.
- Legal taps or strikes within event force limits.
- Brief clinches, frames, and positional contact.
- Footwork-based positioning, pivots, checks, and evasions.
- Balance contests and boundary pressure.

Prohibited contact includes:

- Cutting, stabbing, puncturing, tearing, or crushing designed to damage.
- Battery, wiring, sensor, radio, or joint targeting.
- Neck, head, or sensor-mast twisting beyond inspected limits.
- Entanglement intended to immobilize actuators or create unsafe falls.
- Continuing contact after a halt, fall stop, arena breach, or emergency stop.
- Any contact pattern that functions as a weaponized payload.

## 14. Software And Command Set Rules

Software used in OHSR must be reviewable at the event level appropriate to the
class. Officials may require source review, binary hash recording, command set
registration, simulation logs, or match telemetry.

Software must:

- Respect arena boundaries.
- Honor emergency stop and link-loss states.
- Keep autonomy within the declared autonomy class.
- Avoid deliberate weaponized behavior.
- Avoid unsafe recovery attempts when pinned, fallen, overheated, or partially
  outside the arena.
- Record meaningful logs when practical.

CFA command sets may be edited to improve sports tactics, gait, guard behavior,
balance, recovery, and commentary. They must not be edited to add prohibited
payload behavior, arena attack behavior, or destructive scoring objectives for
physical OHSR operation.

## 15. CFA Legacy Mapping

CFA robots fit OHSR as historical legacy humanoid sports robots when the
following conditions are true:

- They are rendered or constructed as humanoid competitors with head, torso,
  arms, legs, and feet.
- They fight in a bounded cage or arena.
- They use sports contact and positional tactics rather than weapon payloads.
- They can be halted by the simulation or physical event controller.
- Their command sets are inspectable and bounded.
- Their scoring can be configured to reward control, balance, footwork,
  recovery, and legal contact.

The CFA damage model may remain as a simulation-specific historical mechanic
for visual dents, scuffs, degradation, and commentary. In physical OHSR, damage
is a safety condition and inspection trigger, not the primary objective.

## 16. Compliance Levels

OHSR recognizes three informal compliance levels:

- OHSR-Simulated: Software-only or rendered robots with no physical risk.
- OHSR-Lab: Physical prototypes operated in a controlled lab without public
  spectators.
- OHSR-Event: Physical robots operated in an inspected public or private event
  arena with officials, emergency stop protocol, battery protocol, and match
  rules.

A CFA software demo may claim OHSR-Simulated legacy alignment. A physical robot
derived from CFA behavior must satisfy OHSR-Lab or OHSR-Event requirements
before operation.

## 17. Quick Compliance Checklist

Before a match, confirm:

- Robot is in a declared height and weight class.
- Autonomy or teleoperation class is declared.
- Emergency stop works from operator and official controls.
- Link-loss behavior is safe.
- Battery chemistry, charging, fusing, and fire posture are approved.
- No weaponized payloads are present.
- Contact surfaces are inspected and safe.
- Arena boundary is appropriate for the robot class.
- Scoring rewards control, balance, technique, and safe contact.
- Command set or software version is recorded.
- Officials have authority to halt, inspect, penalize, or disqualify.

## 18. Historical Legacy Note

This document is kept as a historical legacy specification for the CFA project
and related humanoid sports robotics experiments. It captures an early posture:
humanoid robot fighting can be framed as controlled sport, simulation,
inspection, and non-destructive scoring rather than escalation toward weapons.

Future CFA documents may supersede this specification, but should preserve the
core OHSR commitments to emergency stop, battery and fire safety, inspection,
declared autonomy, declared weight and height class, non-destructive scoring,
and no weaponized payloads.
