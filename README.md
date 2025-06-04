# Kerbal Guidance Computer

![KGC DSKY](doc/img/kgc_hw_prototype.jpg?raw=true "HW prototype of KGC's DSKY")

A HW flight computer for Kerbal Space Program inspired by NASA's DSKY - interface for [Apollo Guidance Computer](https://en.wikipedia.org/wiki/Apollo_Guidance_Computer).

It consists of Python script for PC which grab data from the [kRPC mod](https://krpc.github.io/krpc/index.html), KGC's DSKY based on Raspberry Pi Pico and program for it. DSKY communicate with PC via UART, so you also need some USB-UART converter. KiCad project for DSKY is located in kicad directory.

**Project is still in developement, it's pretty buggy and most of planned features are stil unimplemented!**

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

| Verb | Noun | Displays                                  | Actions                | Notes                      |
|------|------|-------------------------------------------|------------------------|----------------------------|
| 00   | 00   |                                           | No op                  |                            |
| 01   | 01   |                                           | Disconnect from server |                            |
| 00   | 02   |                                           | Reboot                 |                            |
| 01   | 00   |                                           | No op                  |                            |
| 01   | 01   | Altitude above sea level [m]              |                        |                            |
|      |      | Apoapsis [m]                              |                        |                            |
|      |      | Periapsis [m]                             |                        |                            |
| 01   | 02   | Orbital inclination [°]                   |                        | TODO: float                |
|      |      | Orbital eccentricity [°]                  |                        | TODO: float                |
|      |      | Mean anomaly [°]                          |                        | TODO: float                |
| 01   | 03   | Time to apoapsis [sec]                    |                        |                            |
|      |      | Time to apoapsis [min]                    |                        |                            |
|      |      | Time to apoapsis [hrs]                    |                        |                            |
| 01   | 04   | Time to apoapsis [min]                    |                        |                            |
|      |      | Time to apoapsis [hrs]                    |                        |                            |
|      |      | Time to apoapsis [days]                   |                        | Kerbin days                |
| 01   | 05   | Time to periapsis [sec]                   |                        |                            |
|      |      | Time to periapsis [min]                   |                        |                            |
|      |      | Time to periapsis [hrs]                   |                        |                            |
| 01   | 06   | Time to periapsis [min]                   |                        |                            |
|      |      | Time to periapsis [hrs]                   |                        |                            |
|      |      | Time to periapsis [days]                  |                        | Kerbin days                |
| 01   | 07   | Orbital period [sec]                      |                        |                            |
|      |      | Orbital period [min]                      |                        |                            |
|      |      | Orbital period [hrs]                      |                        |                            |
| 01   | 08   | Orbital period [min]                      |                        |                            |
|      |      | Orbital period [hrs]                      |                        |                            |
|      |      | Orbital period [days]                     |                        | Kerbin days                |
| 01   | 09   | Time to sphere of influence change [sec]  |                        | Crashes when no SOI change |
|      |      | Time to sphere of influence change [min]  |                        |                            |
|      |      | Time to sphere of influence change [hrs]  |                        |                            |
| 01   | 10   | Time to sphere of influence change [min]  |                        | Crashes when no SOI change |
|      |      | Time to sphere of influence change [hrs]  |                        |                            |
|      |      | Time to sphere of influence change [days] |                        | Kerbin days                |
| 01   | 11   | Heading [°]                               |                        | TODO: check ref. frame     |
|      |      | Pitch [°]                                 |                        | TODO: check ref. frame     |
|      |      | Roll [°]                                  |                        | TODO: check ref. frame     |
| 01   | 12   | Velocity - x [m/s]                        |                        | TODO: check ref. frame     |
|      |      | Velocity - y [m/s]                        |                        |                            |
|      |      | Velocity - z [m/s]                        |                        |                            |
| 01   | 13   | Orbital speed [m/s]                       |                        |                            |
|      |      | Surface speed [m/s]                       |                        |                            |
| 02   | 00   |                                           | No op                  |                            |
| 02   | 01   | Mission elapsed time [sec]                |                        |                            |
|      |      | Mission elapsed time [min]                |                        |                            |
|      |      | Mission elapsed time [hrs]                |                        | Strange bug                |
| 02   | 02   | Mission elapsed time [min]                |                        |                            |
|      |      | Mission elapsed time [min]                |                        | Strange bug                |
|      |      | Mission elapsed time [days]               |                        | Strange bug                |
| 02   | 03   | Mass [kg]                                 |                        |                            |
|      |      | Available thurst [N]                      |                        |                            |
|      |      | Specific impulse [sec]                    |                        |                            |