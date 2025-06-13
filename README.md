# Kerbal Guidance Computer

![KGC DSKY](doc/img/kgc_hw_prototype.jpg?raw=true "HW prototype of KGC's DSKY")

A HW flight computer for Kerbal Space Program inspired by NASA's DSKY - interface for [Apollo Guidance Computer](https://en.wikipedia.org/wiki/Apollo_Guidance_Computer).

It consists of Python script for PC which grab data from the [kRPC mod](https://krpc.github.io/krpc/index.html), KGC's DSKY based on Raspberry Pi Pico and program for it. DSKY communicate with PC via UART, so you also need some USB-UART converter. KiCad project for DSKY is located in kicad directory.

**Project is still in developement, it's pretty buggy and some of planned features are stil unimplemented.**

## Features
### Implemented
- Display several flight informations depending on selected Verb and Noun.
- Ship control:
    - SAS control
    - RCS control
    - Gear control
    - Lights control
    - Brakes control
    - Solar panels control (extend/retract)
    - Antennas control (extend/retract)
    - Next stage activation
    - Abort group activation
- MechJeb control:
    - Ascent guidance
### Planned
- Ship control:
    - Thrust control
- MechJeb control:
    - Landing guidance
    - Rendezvous autopilot
    - Docking autopilot
    - Maneuver planner
- Maneuver planning

## Installation
TODO

## Usage
TODO

## Verbs and Nouns list

| Verb | Noun | Input/Output                                 | Actions                               | Notes                                                                |
|------|------|----------------------------------------------|---------------------------------------|----------------------------------------------------------------------|
| 00   | 00   |                                              | No op                                 |                                                                      |
| 00   | 01   |                                              | Disconnect from server                |                                                                      |
| 00   | 02   |                                              | Reboot                                |                                                                      |
| 01   | 00   |                                              | No op                                 |                                                                      |
| 01   | 01   | O: Altitude above sea level [m]              |                                       |                                                                      |
|      |      | O: Apoapsis [m]                              |                                       |                                                                      |
|      |      | O: Periapsis [m]                             |                                       |                                                                      |
| 01   | 02   | O: Orbital inclination [°]                   |                                       |                                                                      |
|      |      | O: Orbital eccentricity [°]                  |                                       |                                                                      |
|      |      | O: Mean anomaly [°]                          |                                       |                                                                      |
| 01   | 03   | O: Time to apoapsis [hrs]                    |                                       |                                                                      |
|      |      | O: Time to apoapsis [min]                    |                                       |                                                                      |
|      |      | O: Time to apoapsis [sec]                    |                                       |                                                                      |
| 01   | 04   | O: Time to apoapsis [days]                   |                                       |                                                                      |
|      |      | O: Time to apoapsis [hrs]                    |                                       |                                                                      |
|      |      | O: Time to apoapsis [min]                    |                                       | Kerbin days                                                          |
| 01   | 05   | O: Time to periapsis [hrs]                   |                                       |                                                                      |
|      |      | O: Time to periapsis [min]                   |                                       |                                                                      |
|      |      | O: Time to periapsis [sec]                   |                                       |                                                                      |
| 01   | 06   | O: Time to periapsis [days]                  |                                       |                                                                      |
|      |      | O: Time to periapsis [hrs]                   |                                       |                                                                      |
|      |      | O: Time to periapsis [min]                   |                                       | Kerbin days                                                          |
| 01   | 07   | O: Orbital period [hrs]                      |                                       |                                                                      |
|      |      | O: Orbital period [min]                      |                                       |                                                                      |
|      |      | O: Orbital period [sec]                      |                                       |                                                                      |
| 01   | 08   | O: Orbital period [days]                     |                                       |                                                                      |
|      |      | O: Orbital period [hrs]                      |                                       |                                                                      |
|      |      | O: Orbital period [min]                      |                                       | Kerbin days                                                          |
| 01   | 09   | O: Time to sphere of influence change [hrs]  |                                       |                                                                      |
|      |      | O: Time to sphere of influence change [min]  |                                       |                                                                      |
|      |      | O: Time to sphere of influence change [sec]  |                                       |                                                                      |
| 01   | 10   | O: Time to sphere of influence change [days] |                                       |                                                                      |
|      |      | O: Time to sphere of influence change [hrs]  |                                       |                                                                      |
|      |      | O: Time to sphere of influence change [min]  |                                       | Kerbin days                                                          |
| 01   | 11   | O: Heading [°]                               |                                       | TODO: check ref. frame                                               |
|      |      | O: Pitch [°]                                 |                                       | TODO: check ref. frame                                               |
|      |      | O: Roll [°]                                  |                                       | TODO: check ref. frame                                               |
| 01   | 12   | O: Velocity - x [m/s]                        |                                       | TODO: check ref. frame                                               |
|      |      | O: Velocity - y [m/s]                        |                                       |                                                                      |
|      |      | O: Velocity - z [m/s]                        |                                       |                                                                      |
| 01   | 13   | O: Orbital speed [m/s]                       |                                       |                                                                      |
|      |      | O: Surface speed [m/s]                       |                                       |                                                                      |
| 02   | 00   |                                              | No op                                 |                                                                      |
| 02   | 01   | O: Mission elapsed time [hrs]                |                                       |                                                                      |
|      |      | O: Mission elapsed time [min]                |                                       |                                                                      |
|      |      | O: Mission elapsed time [sec]                |                                       | Strange bug                                                          |
| 02   | 02   | O: Mission elapsed time [days]               |                                       |                                                                      |
|      |      | O: Mission elapsed time [min]                |                                       | Strange bug                                                          |
|      |      | O: Mission elapsed time [hrs]                |                                       | Strange bug                                                          |
| 02   | 03   | O: Mass [kg]                                 |                                       |                                                                      |
|      |      | O: Available thurst [N]                      |                                       |                                                                      |
|      |      | O: Specific impulse [sec]                    |                                       |                                                                      |
| 02   | 04   | O: Electric charge capacity [-]              |                                       |                                                                      |
|      |      | O: Monopropellant capacity [-]               |                                       |                                                                      |
|      |      | O: Liquid fuel capacity [-]                  |                                       |                                                                      |
| 02   | 05   | O: Electric charge amount [-]                |                                       | TODO: float                                                          |
|      |      | O: Monopropellant amount [-]                 |                                       | TODO: float                                                          |
|      |      | O: Liquid fuel amount [-]                    |                                       | TODO: float                                                          |
| 02   | 06   | O: Electric charge amount [%]                |                                       |                                                                      |
|      |      | O: Monopropellant amount [%]                 |                                       |                                                                      |
|      |      | O: Liquid fuel amount [%]                    |                                       |                                                                      |
| 02   | 07   | O: Electric charge capacity [-]              |                                       |                                                                      |
|      |      | O: Electric charge amount [-]                |                                       | TODO: float                                                          |
|      |      | O: Electric charge amount [%]                |                                       |                                                                      |
| 02   | 08   | O: Monopropellant capacity [-]               |                                       |                                                                      |
|      |      | O: Monopropellant amount [-]                 |                                       | TODO: float                                                          |
|      |      | O: Monopropellant amount [%]                 |                                       |                                                                      |
| 02   | 09   | O: Liquid fuel capacity [-]                  |                                       |                                                                      |
|      |      | O: Liquid fuel amount [-]                    |                                       | TODO: float                                                          |
|      |      | O: Liquid fuel amount [%]                    |                                       |                                                                      |
| 03   | 01   | O: Surface speed [m/s]                       |                                       |                                                                      |
|      |      | O: Vertical speed [m/s]                      |                                       |                                                                      |
|      |      | O: Altitude above surface [m]                |                                       |                                                                      |
| 03   | 02   | O: Altitude above sea level                  |                                       |                                                                      |
|      |      | O: Apoapsis [m]                              |                                       |                                                                      |
|      |      | O: Orbital inclination [°]                   |                                       |                                                                      |
| 03   | 03   | O: Apoapsis [m]                              |                                       |                                                                      |
|      |      | O: Periapsis [m]                             |                                       |                                                                      |
|      |      | O: Orbital inclination [°]                   |                                       |                                                                      |
| 04   | 01   | O: Time to node [hrs]                        |                                       |                                                                      |
|      |      | O: Time to node [min]                        |                                       |                                                                      |
|      |      | O: Time to node [sec]                        |                                       |                                                                      |
| 04   | 02   | O: Δv magnitude - prograde direction [m/s]   |                                       |                                                                      |
|      |      | O: Δv magnitude - normal direction [m/s]     |                                       |                                                                      |
|      |      | O: Δv magnitude - radial direction [m/s]     |                                       |                                                                      |
| 04   | 03   | O: Δv [m/s]                                  |                                       |                                                                      |
|      |      | O: Remaining Δv [m/s]                        |                                       |                                                                      |
|      |      | O: Time to node [sec]                        |                                       |                                                                      |
| 04   | 04   | O: Result apoapsis [m]                       |                                       |                                                                      |
|      |      | O: Result periapsis [m]                      |                                       |                                                                      |
|      |      | O: Result inclination [°]                    |                                       |                                                                      |
| 05   | 01   | O: Altitude above surface [m]                |                                       |                                                                      |
|      |      | O: Surface speed [m/s]                       |                                       |                                                                      |
|      |      | O: Liquid fuel amount [%]                    |                                       |                                                                      |
| 05   | 02   | O: Altitude above surface [m]                |                                       |                                                                      |
|      |      | O: Vertical speed [m/s]                      |                                       |                                                                      |
|      |      | O: Horizontal speed [m/s]                    |                                       |                                                                      |
| 12   | 01   |                                              | Activate SAS in stability assist mode | Then return to previous Verb + Noun                                  |
| 12   | 02   |                                              | Activate SAS in maneuver mode         | Then return to previous Verb + Noun                                  |
| 12   | 03   |                                              | Activate SAS in prograde mode         | Then return to previous Verb + Noun                                  |
| 12   | 04   |                                              | Activate SAS in retrograde mode       | Then return to previous Verb + Noun                                  |
| 12   | 05   |                                              | Activate SAS in normal mode           | Then return to previous Verb + Noun                                  |
| 12   | 06   |                                              | Activate SAS in anti-normal mode      | Then return to previous Verb + Noun                                  |
| 12   | 07   |                                              | Activate SAS in radial mode           | Then return to previous Verb + Noun                                  |
| 12   | 08   |                                              | Activate SAS in anti-radial mode      | Then return to previous Verb + Noun                                  |
| 12   | 09   |                                              | Activate SAS in target mode           | Then return to previous Verb + Noun                                  |
| 12   | 10   |                                              | Activate SAS in anti-targer mode      | Then return to previous Verb + Noun                                  |
| 12   | 11   |                                              | Deactivate SAS                        | Then return to previous Verb + Noun                                  |
| 12   | 12   |                                              | Activate RCS                          | Then return to previous Verb + Noun                                  |
| 12   | 13   |                                              | Deactivate RCS                        | Then return to previous Verb + Noun                                  |
| 12   | 14   |                                              | Extend gear                           | Then return to previous Verb + Noun                                  |
| 12   | 15   |                                              | Retract gear                          | Then return to previous Verb + Noun                                  |
| 12   | 16   |                                              | Activate ligts                        | Then return to previous Verb + Noun                                  |
| 12   | 17   |                                              | Deactivate lights                     | Then return to previous Verb + Noun                                  |
| 12   | 18   |                                              | Activate brakes                       | Then return to previous Verb + Noun                                  |
| 12   | 19   |                                              | Deactivate brakes                     | Then return to previous Verb + Noun                                  |
| 12   | 20   |                                              | Extend solar panels                   | Then return to previous Verb + Noun                                  |
| 12   | 21   |                                              | Retract solar panels                  | Then return to previous Verb + Noun                                  |
| 12   | 22   |                                              | Extend antennas                       | Then return to previous Verb + Noun                                  |
| 12   | 23   |                                              | Retract antennas                      | Then return to previous Verb + Noun                                  |
| 12   | 24   |                                              | Activate next stage                   | Then return to previous Verb + Noun                                  |
| 12   | 25   |                                              | Activate abort group                  | Then return to previous Verb + Noun                                  |
| 13   | 00   |                                              | Initialize ascent autpoilot           | MechJeb is used                                                      |
| 13   | 01   | I: Orbit altitude [km]                       |                                       |                                                                      |
|      |      | I: Orbit inclination [°]                     |                                       |                                                                      |
|      |      | I: Ascent profile {1; 2; 3}                  |                                       | 1 = Classic ascent profile                                           |
|      |      |                                              |                                       | 2 = Stock-style GravityTurn ascent profile                           |
|      |      |                                              |                                       | 3 = Primer Vector Guidance ascent profile                            |
| 13   | 02   | O: Orbit altitude [km]                       |                                       |                                                                      |
|      |      | O: Orbit inclination [°]                     |                                       |                                                                      |
|      |      | O: Ascent profile {1; 2; 3}                  |                                       |                                                                      |
| 13   | 03   | I: Acceleration limit [m/s]                  |                                       | -1 for disable; -2 for ignore                                        |
|      |      | I: Throttle limit [%]                        |                                       | -1 for disable; -2 for ignore                                        |
| 13   | 04   | O: Acceleration limit [m/s]                  |                                       |                                                                      |
|      |      | O: Throttle limit [%]                        |                                       |                                                                      |
| 13   | 05   | I: Force roll - climb [°]                    |                                       | -1 for disable; -2 for ignore (whole force roll)                     |
|      |      | I: Force roll - turn [°]                     |                                       |                                                                      |
| 13   | 06   | O: Force roll - climb [°]                    |                                       |                                                                      |
|      |      | O: Force roll - turn [°]                     |                                       |                                                                      |
| 13   | 07   | I: Autostage pre-delay [s]                   |                                       | -1 for disable; -2 for ignore (whole autostage)                      |
|      |      | I: Autostage post-delay [s]                  |                                       |                                                                      |
|      |      | I: Clamp autostage thrust [%]                |                                       |                                                                      |
| 13   | 08   | O: Autostage pre-delay [s]                   |                                       |                                                                      |
|      |      | O: Autostage post-delay [s]                  |                                       |                                                                      |
|      |      | O: Clamp autostage thrust [%]                |                                       |                                                                      |
| 13   | 09   | I: Minimal throttle [%]                      |                                       | -1 for disable; -2 for ignore                                        |
|      |      | I: Autostage stop stage                      |                                       | Details in truth table                                               |
|      |      | I: Other options                             |                                       |                                                                      |
| 13   | 10   | O: Minimal throttle [%]                      |                                       |                                                                      |
|      |      | O: Autostage stop stage                      |                                       |                                                                      |
|      |      | O: Other options                             |                                       |                                                                      |
| 13   | 11   | I: Turn start altitude [km]                  |                                       | Classic ascent profile                                               |
|      |      | I: Turn start velocity [m/s]                 |                                       | Classic ascent profile                                               |
|      |      | I: Turn end altitude [km]                    |                                       | Classic ascent profile                                               |
| 13   | 12   | O: Turn start altitude [km]                  |                                       | Classic ascent profile                                               |
|      |      | O: Turn start velocity [m/s]                 |                                       | Classic ascent profile                                               |
|      |      | O: Turn end altitude [km]                    |                                       | Classic ascent profile                                               |
| 13   | 13   | I: Final flight path angle [°]               |                                       | Classic ascent profile; -1 for disable; -2 for ignore                |
|      |      | I: Turn shape [%]                            |                                       | Classic ascent profile; -1 for disable; -2 for ignore                |
| 13   | 14   | O: Final flight path angle [°]               |                                       | Classic ascent profile                                               |
|      |      | O: Turn shape [%]                            |                                       | Classic ascent profile                                               |
| 13   | 15   | I: Turn start altitude [km]                  |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | I: Turn start velocity [m/s]                 |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | I: Turn start pitch [°]                      |                                       | Stock-style GravityTurn ascent profile                               |
| 13   | 16   | O: Turn start altitude [km]                  |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | O: Turn start velocity [m/s]                 |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | O: Turn start pitch [°]                      |                                       | Stock-style GravityTurn ascent profile                               |
| 13   | 17   | I: Intermediate altitude [km]                |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | I: Hold AP time [s]                          |                                       | Stock-style GravityTurn ascent profile                               |
| 13   | 18   | O: Intermediate altitude [km]                |                                       | Stock-style GravityTurn ascent profile                               |
|      |      | O: Hold AP time [s]                          |                                       | Stock-style GravityTurn ascent profile                               |
| 13   | 19   | I: Apoapsis altitude [km]                    |                                       | Primer Vector Guidance ascent profile                                |
|      |      | I: Attach altitude [km]                      |                                       | Primer Vector Guidance ascent profile                                |
|      |      | I: Booster pitch start velocity [m/s]        |                                       | Primer Vector Guidance ascent profile                                |
| 13   | 20   | O: Apoapsis altitude [km]                    |                                       | Primer Vector Guidance ascent profile                                |
|      |      | O: Attach altitude [km]                      |                                       | Primer Vector Guidance ascent profile                                |
|      |      | O: Booster pitch start velocity [m/s]        |                                       | Primer Vector Guidance ascent profile                                |
| 13   | 21   | I: Booster pitch rate [°/s]                  |                                       | Primer Vector Guidance ascent profile                                |
|      |      | I: PVG after stage                           |                                       | Primer Vector Guidance ascent profile; -1 for disable; -2 for ignore |
| 13   | 22   | O: Booster pitch rate [°/s]                  |                                       | Primer Vector Guidance ascent profile                                |
|      |      | O: PVG after stage                           |                                       | Primer Vector Guidance ascent profile                                |
| 13   | 23   | I: Q trigger [kPa]                           |                                       | Primer Vector Guidance ascent profile                                |
|      |      | I: Fixed coast length [s]                    |                                       | Primer Vector Guidance ascent profile; -1 for disable; -2 for ignore |
| 13   | 24   | O: Q trigger [kPa]                           |                                       | Primer Vector Guidance ascent profile                                |
|      |      | O: Fixed coast length [s]                    |                                       | Primer Vector Guidance ascent profile                                |
| 13   | 25   |                                              | Enage ascent autopilot                |                                                                      |
| 13   | 26   |                                              | Disenage ascent autopilot             |                                                                      |

## Truth table for other options value in Ascent autopilot

| Prevent overheat | Solar panels | Antennas | Skip circularization | Dec |
|------------------|--------------|----------|----------------------|-----|
| 0                | 0            | 0        | 0                    | 0   |
| 0                | 0            | 0        | 1                    | 1   |
| 0                | 0            | 1        | 0                    | 2   |
| 0                | 0            | 1        | 1                    | 3   |
| 0                | 1            | 0        | 0                    | 4   |
| 0                | 1            | 0        | 1                    | 5   |
| 0                | 1            | 1        | 0                    | 6   |
| 0                | 1            | 1        | 1                    | 7   |
| 1                | 0            | 0        | 0                    | 8   |
| 1                | 0            | 0        | 1                    | 9   |
| 1                | 0            | 1        | 0                    | 10  |
| 1                | 0            | 1        | 1                    | 11  |
| 1                | 1            | 0        | 0                    | 12  |
| 1                | 1            | 0        | 1                    | 13  |
| 1                | 1            | 1        | 0                    | 14  |
| 1                | 1            | 1        | 1                    | 15  |

## Error codes
### Ascent guidance:
- 1301: Noun doesn't exist
- 1302: Autopilot wasn't initialized
- 1303: Ascent profile selected in V13;N01 doesn't exist