# Kerbal Guidance Computer

![KGC DSKY](doc/img/kgc_hw_prototype.jpg?raw=true "HW prototype of KGC's DSKY")

A HW flight computer for Kerbal Space Program inspired by NASA's DSKY - interface for [Apollo Guidance Computer](https://en.wikipedia.org/wiki/Apollo_Guidance_Computer).

It consists of Python script for PC which grab data from the [kRPC mod](https://krpc.github.io/krpc/index.html), KGC's DSKY based on Raspberry Pi Pico and program for it. DSKY communicate with PC via UART, so you also need some USB-UART converter. KiCad project for DSKY is located in kicad directory.

**Project is still in developement, it's pretty buggy and many of planned features are stil unimplemented!**

## Features
### Implemented
- Display several flight informations depending on selected Verb and Noun.
### Planned
- Basic ship control
- MechJeb control

## Installation
TODO

## Usage
TODO

## Verbs and Nouns list

| Verb | Noun | Displays                                  | Actions                               | Notes                               |
|------|------|-------------------------------------------|---------------------------------------|-------------------------------------|
| 00   | 00   |                                           | No op                                 |                                     |
| 01   | 01   |                                           | Disconnect from server                |                                     |
| 00   | 02   |                                           | Reboot                                |                                     |
| 01   | 00   |                                           | No op                                 |                                     |
| 01   | 01   | Altitude above sea level [m]              |                                       |                                     |
|      |      | Apoapsis [m]                              |                                       |                                     |
|      |      | Periapsis [m]                             |                                       |                                     |
| 01   | 02   | Orbital inclination [°]                   |                                       |                                     |
|      |      | Orbital eccentricity [°]                  |                                       |                                     |
|      |      | Mean anomaly [°]                          |                                       |                                     |
| 01   | 03   | Time to apoapsis [hrs]                    |                                       |                                     |
|      |      | Time to apoapsis [min]                    |                                       |                                     |
|      |      | Time to apoapsis [sec]                    |                                       |                                     |
| 01   | 04   | Time to apoapsis [days]                   |                                       |                                     |
|      |      | Time to apoapsis [hrs]                    |                                       |                                     |
|      |      | Time to apoapsis [min]                    |                                       | Kerbin days                         |
| 01   | 05   | Time to periapsis [hrs]                   |                                       |                                     |
|      |      | Time to periapsis [min]                   |                                       |                                     |
|      |      | Time to periapsis [sec]                   |                                       |                                     |
| 01   | 06   | Time to periapsis [days]                  |                                       |                                     |
|      |      | Time to periapsis [hrs]                   |                                       |                                     |
|      |      | Time to periapsis [min]                   |                                       | Kerbin days                         |
| 01   | 07   | Orbital period [hrs]                      |                                       |                                     |
|      |      | Orbital period [min]                      |                                       |                                     |
|      |      | Orbital period [sec]                      |                                       |                                     |
| 01   | 08   | Orbital period [days]                     |                                       |                                     |
|      |      | Orbital period [hrs]                      |                                       |                                     |
|      |      | Orbital period [min]                      |                                       | Kerbin days                         |
| 01   | 09   | Time to sphere of influence change [hrs]  |                                       |                                     |
|      |      | Time to sphere of influence change [min]  |                                       |                                     |
|      |      | Time to sphere of influence change [sec]  |                                       |                                     |
| 01   | 10   | Time to sphere of influence change [days] |                                       |                                     |
|      |      | Time to sphere of influence change [hrs]  |                                       |                                     |
|      |      | Time to sphere of influence change [min]  |                                       | Kerbin days                         |
| 01   | 11   | Heading [°]                               |                                       | TODO: check ref. frame              |
|      |      | Pitch [°]                                 |                                       | TODO: check ref. frame              |
|      |      | Roll [°]                                  |                                       | TODO: check ref. frame              |
| 01   | 12   | Velocity - x [m/s]                        |                                       | TODO: check ref. frame              |
|      |      | Velocity - y [m/s]                        |                                       |                                     |
|      |      | Velocity - z [m/s]                        |                                       |                                     |
| 01   | 13   | Orbital speed [m/s]                       |                                       |                                     |
|      |      | Surface speed [m/s]                       |                                       |                                     |
| 02   | 00   |                                           | No op                                 |                                     |
| 02   | 01   | Mission elapsed time [hrs]                |                                       |                                     |
|      |      | Mission elapsed time [min]                |                                       |                                     |
|      |      | Mission elapsed time [sec]                |                                       | Strange bug                         |
| 02   | 02   | Mission elapsed time [days]               |                                       |                                     |
|      |      | Mission elapsed time [min]                |                                       | Strange bug                         |
|      |      | Mission elapsed time [hrs]                |                                       | Strange bug                         |
| 02   | 03   | Mass [kg]                                 |                                       |                                     |
|      |      | Available thurst [N]                      |                                       |                                     |
|      |      | Specific impulse [sec]                    |                                       |                                     |
| 02   | 04   | Electric charge capacity [-]              |                                       |                                     |
|      |      | Monopropellant capacity [-]               |                                       |                                     |
|      |      | Liquid fuel capacity [-]                  |                                       |                                     |
| 02   | 05   | Electric charge amount [-]                |                                       | TODO: float                         |
|      |      | Monopropellant amount [-]                 |                                       | TODO: float                         |
|      |      | Liquid fuel amount [-]                    |                                       | TODO: float                         |
| 02   | 06   | Electric charge amount [%]                |                                       |                                     |
|      |      | Monopropellant amount [%]                 |                                       |                                     |
|      |      | Liquid fuel amount [%]                    |                                       |                                     |
| 02   | 07   | Electric charge capacity [-]              |                                       |                                     |
|      |      | Electric charge amount [-]                |                                       | TODO: float                         |
|      |      | Electric charge amount [%]                |                                       |                                     |
| 02   | 08   | Monopropellant capacity [-]               |                                       |                                     |
|      |      | Monopropellant amount [-]                 |                                       | TODO: float                         |
|      |      | Monopropellant amount [%]                 |                                       |                                     |
| 02   | 09   | Liquid fuel capacity [-]                  |                                       |                                     |
|      |      | Liquid fuel amount [-]                    |                                       | TODO: float                         |
|      |      | Liquid fuel amount [%]                    |                                       |                                     |
| 03   | 01   | Surface speed [m/s]                       |                                       |                                     |
|      |      | Vertical speed [m/s]                      |                                       |                                     |
|      |      | Altitude above surface [m]                |                                       |                                     |
| 03   | 02   | Altitude above sea level                  |                                       |                                     |
|      |      | Apoapsis [m]                              |                                       |                                     |
|      |      | Orbital inclination [°]                   |                                       |                                     |
| 03   | 03   | Apoapsis [m]                              |                                       |                                     |
|      |      | Periapsis [m]                             |                                       |                                     |
|      |      | Orbital inclination [°]                   |                                       |                                     |
| 04   | 01   | Time to node [hrs]                        |                                       |                                     |
|      |      | Time to node [min]                        |                                       |                                     |
|      |      | Time to node [sec]                        |                                       |                                     |
| 04   | 02   | Δv magnitude - prograde direction [m/s]   |                                       |                                     |
|      |      | Δv magnitude - normal direction [m/s]     |                                       |                                     |
|      |      | Δv magnitude - radial direction [m/s]     |                                       |                                     |
| 04   | 03   | Δv [m/s]                                  |                                       |                                     |
|      |      | Remaining Δv [m/s]                        |                                       |                                     |
|      |      | Time to node [sec]                        |                                       |                                     |
| 04   | 04   | Result apoapsis [m]                       |                                       |                                     |
|      |      | Result periapsis [m]                      |                                       |                                     |
|      |      | Result inclination [°]                    |                                       |                                     |
| 05   | 01   | Altitude above surface [m]                |                                       |                                     |
|      |      | Surface speed [m/s]                       |                                       |                                     |
|      |      | Liquid fuel amount [%]                    |                                       |                                     |
| 05   | 02   | Altitude above surface [m]                |                                       |                                     |
|      |      | Vertical speed [m/s]                      |                                       |                                     |
|      |      | Horizontal speed [m/s]                    |                                       |                                     |
| 12   | 01   |                                           | Activate SAS in stability assist mode | Then return to previous Verb + Noun |
| 12   | 02   |                                           | Activate SAS in maneuver mode         | Then return to previous Verb + Noun |
| 12   | 03   |                                           | Activate SAS in prograde mode         | Then return to previous Verb + Noun |
| 12   | 04   |                                           | Activate SAS in retrograde mode       | Then return to previous Verb + Noun |
| 12   | 05   |                                           | Activate SAS in normal mode           | Then return to previous Verb + Noun |
| 12   | 06   |                                           | Activate SAS in anti-normal mode      | Then return to previous Verb + Noun |
| 12   | 07   |                                           | Activate SAS in radial mode           | Then return to previous Verb + Noun |
| 12   | 08   |                                           | Activate SAS in anti-radial mode      | Then return to previous Verb + Noun |
| 12   | 09   |                                           | Activate SAS in target mode           | Then return to previous Verb + Noun |
| 12   | 10   |                                           | Activate SAS in anti-targer mode      | Then return to previous Verb + Noun |
| 12   | 11   |                                           | Deactivate SAS                        | Then return to previous Verb + Noun |
| 12   | 12   |                                           | Activate RCS                          | Then return to previous Verb + Noun |
| 12   | 13   |                                           | Deactivate RCS                        | Then return to previous Verb + Noun |
| 12   | 14   |                                           | Extend gear                           | Then return to previous Verb + Noun |
| 12   | 15   |                                           | Retract gear                          | Then return to previous Verb + Noun |
| 12   | 16   |                                           | Activate ligts                        | Then return to previous Verb + Noun |
| 12   | 17   |                                           | Deactivate lights                     | Then return to previous Verb + Noun |
| 12   | 18   |                                           | Activate brakes                       | Then return to previous Verb + Noun |
| 12   | 19   |                                           | Deactivate brakes                     | Then return to previous Verb + Noun |
| 12   | 20   |                                           | Extend solar panels                   | Then return to previous Verb + Noun |
| 12   | 21   |                                           | Retract solar panels                  | Then return to previous Verb + Noun |
| 12   | 22   |                                           | Extend antennas                       | Then return to previous Verb + Noun |
| 12   | 23   |                                           | Retract antennas                      | Then return to previous Verb + Noun |
| 12   | 24   |                                           | Activate next stage                   | Then return to previous Verb + Noun |
| 12   | 25   |                                           | Activate abort group                  | Then return to previous Verb + Noun |
