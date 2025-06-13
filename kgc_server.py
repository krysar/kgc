import krpc
import krpc.attributes
import krpc.types
import krpc.utils
import serial
import math

# Function definitions

# Convert seconds to hours, minutes and seconds
def sec_to_hhmmss(time_in_sec: int) -> tuple[int, int, int]:
    hr = time_in_sec // 3600
    min = (time_in_sec // 60) % 60
    sec = time_in_sec % 60
    time = (hr, min, sec)

    return time

# Convert seconds to Kerbin days, hours and minutes
def sec_to_ddhhmm(time_in_sec: int) -> tuple[int, int, int]:
    day = time_in_sec // 36000
    hr = (time_in_sec // 3600) % 10
    min = (time_in_sec // 60) % 60
    time = (day, hr, min)

    return time

def dsky_send(num1: int, num2: int, num3: int):
    # FLD define:
    match str(krpc_conn.krpc.current_game_scene): # type: ignore
        case "GameScene.space_center":
            fld = 0
            elc = 0
            mop = 0
            lfo = 0
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.flight":
            vessel = krpc_conn.space_center.active_vessel # type: ignore

            # Remaining electric charge
            if vessel.resources.has_resource("ElectricCharge"):
                el_charge = vessel.resources.amount("ElectricCharge") / vessel.resources.max("ElectricCharge")
            else:
                el_charge = -1

            # Remaining monopropellant
            if vessel.resources.has_resource("MonoPropellant"):
                monoprop = vessel.resources.amount("MonoPropellant") / vessel.resources.max("MonoPropellant")
            else:
                monoprop = -1

            # Remaining fuel
            if vessel.resources.has_resource("LiquidFuel"):
                liq_fuel = vessel.resources.amount("LiquidFuel") / vessel.resources.max("LiquidFuel")
            else:
                liq_fuel = -1

            fld = 1

            if el_charge > 0 and el_charge <= 0.1:
                elc = 1
            elif el_charge == 0:
                elc = 2
            else:
                elc = 0

            if monoprop > 0 and monoprop <= 0.1:
                mop = 1
            elif monoprop == 0:
                mop = 2
            else:
                mop = 0

            if liq_fuel > 0 and liq_fuel <= 0.1:
                lfo = 1
            elif liq_fuel == 0:
                lfo = 2
            else:
                lfo = 0

            if vessel.control.sas:
                sas = 1
            else:
                sas = 0

            if vessel.control.rcs:
                rcs = 1
            else:
                rcs = 0

            if vessel.control.gear: 
                gea = 1
            else:
                gea = 0

            if vessel.control.brakes: 
                brk = 1
            else:
                brk = 0

        case "GameScene.tracking_station":
            fld = 2
            elc = 0
            mop = 0
            lfo = 0
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.editor_vab":
            fld = 3
            elc = 0
            mop = 0
            lfo = 0
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.editor_sph":
            fld = 4
            elc = 0
            mop = 0
            lfo = 0
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case _:
            fld = -1
            elc = 0
            mop = 0
            lfo = 0
            sas = 0
            rcs = 0
            gea = 0
            brk = 0

    if krpc_conn.krpc.paused: # type: ignore
        pau = 1
    else:
        pau = 0

    # TODO: support OTR
    otr = 0

    ser_out = str(num1) + ";" + str(num2) + ";" + str(num3) + ";"
    ser_out += str(fld) + ";" + str(elc) + ";" + str(mop) + ";"
    ser_out += str(lfo) + ";" + str(otr) + ";" + str(sas) + ";"
    ser_out += str(rcs) + ";" + str(gea) + ";" + str(brk) + ";" + str(pau)

    ser_out = ser_out.encode("utf-8", "strict")
    ser_conn.write(ser_out)

def dsky_send_empty_string():
    ser_out = "0;0;0;0;0;0;0;0;0;0;0;0"
    ser_out = ser_out.encode("utf-8", "strict")
    ser_conn.write(ser_out)

# It's not effective, I just copied and edited function from max7219.c (:
def dec2bin(dec: int) -> list[bool]:
    bin = 0; rem = 0; i = 1

    while dec != 0:
        rem = dec % 2
        dec = dec >> 1
        bin += rem * i
        i *= 10

    l_bin = [0, 0, 0, 0]

    for i in range(4, 0, -1):
        l_bin[i - 1] = bin % 10
        bin = bin // 10

    l_bool = []

    for i in l_bin:
        l_bool.append(bool(i))

    return l_bool

# Main program:

print("KGC 0.4.0")

serial_port = input("Serial port? ")
ser_conn = serial.Serial(serial_port, 115200)
ser_conn.bytesize = serial.EIGHTBITS
ser_conn.stopbits = serial.STOPBITS_ONE
ser_conn.parity = serial.PARITY_EVEN
ser_conn.timeout = None

krpc_conn = krpc.connect(name='KGC 0.4.0')
if ser_conn.is_open == False:
    ser_conn.open()

tst = 1024
verb = 0
noun = 0
dsky_data_in = [0.0, 0.0, 0.0]

# Waiting for handshake
print("Connecting to kgc...", end=" ")
handshake_in = ser_conn.readline()
handshake_in = handshake_in.decode("utf-8", "strict")
if handshake_in == "WAITING\n":
    handshake_out = "ACCEPTED\n"
    handshake_out = handshake_out.encode("utf-8", "strict")
    ser_conn.write(handshake_out)
    print("[ok]")

while handshake_in == "WAITING\n":
    ser_in = ser_conn.readline()
    ser_str = ser_in.decode("utf-8", "strict")
    ser_in_str = ser_str.strip()
    ser_in_list = ser_str.split(';')
    # print(ser_str)

    for i in ser_in_list:
        if i.startswith("V:"):
            verb = i.split(':')
            verb = verb[1].strip()
            verb = int(verb)
        elif i.startswith("N:"):
            noun = i.split(':')
            noun = noun[1].strip()
            noun = int(noun)
        elif i.startswith("D1:"):
            buf = i.split(':')
            buf = buf[1].strip()
            dsky_data_in[0] = float(buf)
        elif i.startswith("D2:"):
            buf = i.split(':')
            buf = buf[1].strip()
            dsky_data_in[1] = float(buf)
        elif i.startswith("D3:"):
            buf = i.split(':')
            buf = buf[1].strip()
            dsky_data_in[2] = float(buf)
        # print("Verb is ", verb, ", noun is ", noun)

    if str(krpc_conn.krpc.current_game_scene) == "GameScene.flight": # type: ignore
        vessel = krpc_conn.space_center.active_vessel # type: ignore
        vsl_frame = vessel.surface_reference_frame
        srf_frame = vessel.orbit.body.reference_frame

        match verb:
            # Verb 1 = Display flight info
            case 1:
                match noun:
                    # Noun 1 = Altitude above sea level + Apoapsis + Periapsis
                    case 1:
                        dsky_send(int(vessel.flight().mean_altitude), int(vessel.orbit.apoapsis_altitude), int(vessel.orbit.periapsis_altitude))

                    # Noun 2 = Inclination + eccentricity + mean anomaly
                    case 2:
                        inc = int((vessel.orbit.inclination * 180 / math.pi) * 1000) # Convert radians to degrees
                        ano = int((vessel.orbit.mean_anomaly * 180 / math.pi) * 1000)
                        ecc = int(vessel.orbit.eccentricity * 1000)
                        dsky_send(inc, ecc, ano)

                    # Noun 3 = Time to apoapsis [ss:mm:hhhh]
                    case 3:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_hhmmss(tta)
                        dsky_send(tta[0], tta[1], tta[2])
                    
                    # Noun 4 = Time to apoapsis [mm:hh:dddd]
                    case 4:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_ddhhmm(tta)
                        dsky_send(tta[0], tta[1], tta[2])

                    # Noun 5 = Time to periapsis [ss:mm:hhhh]
                    case 5:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_hhmmss(ttp)
                        dsky_send(ttp[0], ttp[1], ttp[2])

                    # Noun 6 = Time to periapsis [mm:hh:dddd]
                    case 6:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_ddhhmm(ttp)
                        dsky_send(ttp[0], ttp[1], ttp[2])

                    # Noun 7 = Orbit period [ss:mm:hhhh]
                    case 7:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_hhmmss(orp)
                        dsky_send(orp[0], orp[1], orp[2])

                    # Noun 8 = Orbit period [mm:hh:dddd]
                    case 8:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_ddhhmm(orp)
                        dsky_send(orp[0], orp[1], orp[2])

                    # Noun 9 = Time to sphere of influence change [ss:mm:hhhh]
                    case 9:
                        if math.isnan(vessel.orbit.time_to_soi_change):
                            dsky_send_empty_string()
                        else:
                            soic = int(vessel.orbit.time_to_soi_change)
                            soic = sec_to_hhmmss(soic)
                            dsky_send(soic[0], soic[1], soic[2])

                    # Noun 10 = Time to sphere of influence change [mm:hh:dddd]
                    case 10:
                        if math.isnan(vessel.orbit.time_to_soi_change):
                            dsky_send_empty_string()
                        else:
                            soic = int(vessel.orbit.time_to_soi_change)
                            soic = sec_to_ddhhmm(soic)
                            dsky_send(soic[0], soic[1], soic[2])

                    # Noun 11 = Heading + pitch + roll
                    case 11:
                        dsky_send(int(vessel.flight(vsl_frame).heading), int(vessel.flight(vsl_frame).pitch), int(vessel.flight(vsl_frame).roll))

                    # Noun 12 = Velocity
                    case 12:
                        dsky_send(int(vessel.velocity(srf_frame)[0]), int(vessel.velocity(srf_frame)[1]), int(vessel.velocity(srf_frame)[2]))

                    # Noun 13 = Orbital speed + surface speed
                    case 13:
                        dsky_send(int(vessel.orbit.speed), int(vessel.flight(srf_frame).speed), 0)

                    case _:
                        dsky_send(0,0,0)

            # Verb 2 = Display vessel info
            case 2:
                  match noun:
                    # Noun 1 = Mission Elapsed Time [ss:mm:hhhh]    
                    case 1:
                          met = int(vessel.met)
                          met = sec_to_hhmmss(met)
                          dsky_send(met[0], met[1], met[2])

                    # Noun 2 = Mission Elapsed Time [mm:hh:dddd]
                    case 2:
                          met = int(vessel.met)
                          met = sec_to_ddhhmm(met)
                          dsky_send(met[0], met[1], met[2])

                    # Noun 3 = Mass + available thrust + specific impulse
                    case 3:
                          dsky_send(int(vessel.mass), int(vessel.available_thrust), int(vessel.specific_impulse))

                    # Noun 4 = ELC capacity + MOP capacity + LFO capacity
                    case 4:
                        if vessel.resources.has_resource("ElectricCharge"):
                            el_charge = int(vessel.resources.max("ElectricCharge"))
                        else:
                            el_charge = 0
                        
                        if vessel.resources.has_resource("MonoPropellant"):
                            monoprop = int(vessel.resources.max("MonoPropellant"))
                        else:
                            monoprop = 0
                        
                        if vessel.resources.has_resource("LiquidFuel"):
                            liq_fuel = int(vessel.resources.max("LiquidFuel"))
                        else:
                            liq_fuel = 0

                        dsky_send(el_charge, monoprop, liq_fuel)

                    # Noun 5 = ELC amount + MOP amount + LFO amount
                    case 5:
                        if vessel.resources.has_resource("ElectricCharge"):
                            el_charge = int(vessel.resources.amount("ElectricCharge"))
                        else:
                            el_charge = 0
                        
                        if vessel.resources.has_resource("MonoPropellant"):
                            monoprop = int(vessel.resources.amount("MonoPropellant"))
                        else:
                            monoprop = 0
                        
                        if vessel.resources.has_resource("LiquidFuel"):
                            liq_fuel = int(vessel.resources.amount("LiquidFuel"))
                        else:
                            liq_fuel = 0

                        dsky_send(el_charge, monoprop, liq_fuel)

                    # Noun 6 = ELC amount [%] + MOP amount [%] + LFO amount [%]
                    case 6:
                        if vessel.resources.has_resource("ElectricCharge"):
                            el_charge = int(vessel.resources.amount("ElectricCharge") / vessel.resources.max("ElectricCharge") * 100 * 1000)
                        else:
                            el_charge = 0
                        
                        if vessel.resources.has_resource("MonoPropellant"):
                            monoprop = int(vessel.resources.amount("MonoPropellant") / vessel.resources.max("MonoPropellant") * 100 * 1000)
                        else:
                            monoprop = 0
                        
                        if vessel.resources.has_resource("LiquidFuel"):
                            liq_fuel = int(vessel.resources.amount("LiquidFuel") / vessel.resources.max("LiquidFuel") * 100 * 1000)
                        else:
                            liq_fuel = 0

                        dsky_send(el_charge, monoprop, liq_fuel)

                    # Noun 7 = ELC capacity + ELC amount + ELC amount [%]
                    case 7:
                        if vessel.resources.has_resource("ElectricCharge"):
                            el_max = int(vessel.resources.max("ElectricCharge"))
                            el_amo = int(vessel.resources.amount("ElectricCharge"))
                            el_prc = int(vessel.resources.amount("ElectricCharge") / vessel.resources.max("ElectricCharge") * 100 * 1000)
                        else:
                            el_max = 0
                            el_amo = 0
                            el_prc = 0

                        dsky_send(el_max, el_amo, el_prc)

                    # Noun 8 = MOP capacity + MOP amount + MOP amount [%]
                    case 8:
                        if vessel.resources.has_resource("MonoPropellant"):
                            mop_max = int(vessel.resources.max("MonoPropellant"))
                            mop_amo = int(vessel.resources.amount("MonoPropellant"))
                            mop_prc = int(vessel.resources.amount("MonoPropellant") / vessel.resources.max("MonoPropellant") * 100 * 1000)
                        else:
                            mop_max = 0
                            mop_amo = 0
                            mop_prc = 0

                        dsky_send(mop_max, mop_amo, mop_prc)

                    # Noun 9 = LFO capacity + LFO amount + LFO amount [%]
                    case 9:
                        if vessel.resources.has_resource("LiquidFuel"):
                            lfo_max = int(vessel.resources.max("LiquidFuel"))
                            lfo_amo = int(vessel.resources.amount("LiquidFuel"))
                            lfo_prc = int(vessel.resources.amount("LiquidFuel") / vessel.resources.max("LiquidFuel") * 100 * 1000)
                        else:
                            lfo_max = 0
                            lfo_amo = 0
                            lfo_prc = 0

                        dsky_send(lfo_max, lfo_amo, lfo_prc)

                    case _:
                          dsky_send(0,0,0)

            # Verb 3 = Take off
            case 3:
                match noun:
                    # Noun 1 = Surface speed + vertical speed + surface altitude
                    case 1:
                        dsky_send(int(vessel.flight(srf_frame).speed), int(vessel.flight(srf_frame).vertical_speed), int(vessel.flight().surface_altitude))

                    # Noun 2 = Mean altitude + apoapsis + inclination
                    case 2:
                        inc = int((vessel.orbit.inclination * 180 / math.pi) * 1000) # Convert radians to degrees
                        dsky_send(int(vessel.flight().mean_altitude), int(vessel.orbit.apoapsis_altitude), inc)

                    # Noun 3 = Apoapsis + periapsis + inclination
                    case 3:
                        inc = int((vessel.orbit.inclination * 180 / math.pi) * 1000) # Convert radians to degrees
                        dsky_send(int(vessel.orbit.apoapsis_altitude), int(vessel.orbit.periapsis_altitude), inc)

                    case _:
                        dsky_send(0,0,0)

            # Verb 4 = Maneuver
            case 4:
                if len(vessel.control.nodes) == 0:
                    dsky_send(0,0,0)
                else:
                    next_node = vessel.control.nodes[0]
                    match noun:
                        # Verb 1 = Time to node [h:m:s]
                        case 1:
                            ttn = int(next_node.time_to)
                            ttn = sec_to_hhmmss(ttn)
                            dsky_send(ttn[0], ttn[1], ttn[2])

                        # Verb 2 = Prograde, normal, radial
                        case 2:
                            dsky_send(int(next_node.prograde), int(next_node.normal), int(next_node.radial))

                        # Verb 3 = Delta v, remaining Delta v, time to node [s]
                        case 3:
                            dsky_send(int(next_node.delta_v * 1000), int(next_node.remaining_delta_v * 1000), int(next_node.time_to))

                        # Verb 4 = Result apoapsis, periapsis, inclination
                        case 4:
                            inc = int((next_node.orbit.inclination * 180 / math.pi) * 1000) # Convert radians to degrees
                            dsky_send(int(next_node.orbit.apoapsis_altitude), int(next_node.orbit.periapsis_altitude), inc)

                        case _:
                            dsky_send(0,0,0)

            # Verb 5 = Landing
            case 5:
                match noun:
                    # Noun 1 = Surface altitude + surface speed + LFO amount [%]
                    case 1:
                        if vessel.resources.has_resource("LiquidFuel"):
                            lfo_prc = int(vessel.resources.amount("LiquidFuel") / vessel.resources.max("LiquidFuel") * 100 * 1000)
                            dsky_send(int(vessel.flight().surface_altitude), int(vessel.flight(srf_frame).speed * 1000), lfo_prc)
                        else:
                            dsky_send(int(vessel.flight().surface_altitude), int(vessel.flight(srf_frame).speed * 1000), 0)

                    # Noun 2 = Surface altitude + vertical speed + horizontal speed
                    case 2:
                        dsky_send(int(vessel.flight().surface_altitude), int(-vessel.flight(srf_frame).vertical_speed * 1000), int(vessel.flight(srf_frame).horizontal_speed * 1000))

                    case _:
                        dsky_send(0,0,0)

            # Basic vessel control
            case 12:
                match noun:
                    # SAS Mode = stability assist
                    case 1:
                        vessel.control.sas = True
                        vessel.control.sas_mode = krpc_conn.space_center.SASMode.stability_assist # type: ignore

                    # SAS Mode = maneuver
                    case 2:
                        if len(vessel.control.nodes) != 0:
                            vessel.control.sas = True
                            vessel.control.sas_mode = krpc_conn.space_center.SASMode.maneuver # type: ignore

                    # SAS Mode = prograde
                    case 3:
                        if vessel.situation != krpc_conn.space_center.VesselSituation.pre_launch and vessel.situation != krpc_conn.space_center.VesselSituation.landed and vessel.situation != krpc_conn.space_center.VesselSituation.splashed: # type: ignore
                            vessel.control.sas = True
                            vessel.control.sas_mode = krpc_conn.space_center.SASMode.prograde # type: ignore

                    # SAS Mode = retrograde
                    case 4:
                        if vessel.situation != krpc_conn.space_center.VesselSituation.pre_launch and vessel.situation != krpc_conn.space_center.VesselSituation.landed and vessel.situation != krpc_conn.space_center.VesselSituation.splashed: # type: ignore
                            vessel.control.sas = True
                            vessel.control.sas_mode = krpc_conn.space_center.SASMode.retrograde # type: ignore

                    # SAS Mode = normal
                    case 5:
                        vessel.control.sas = True
                        vessel.control.sas_mode = krpc_conn.space_center.SASMode.normal # type: ignore

                    # SAS Mode = anti normal
                    case 6:
                        vessel.control.sas = True
                        vessel.control.sas_mode = krpc_conn.space_center.SASMode.anti_normal # type: ignore

                    # SAS Mode = radial
                    case 7:
                        vessel.control.sas = True
                        vessel.control.sas_mode = krpc_conn.space_center.SASMode.radial # type: ignore

                    # SAS Mode = anti radial
                    case 8:
                        vessel.control.sas = True
                        vessel.control.sas_mode = krpc_conn.space_center.SASMode.anti_radial # type: ignore

                    # SAS Mode = target
                    case 9:
                        if krpc_conn.space_center.target_body != None and krpc_conn.space_center.target_docking_port != None and krpc_conn.space_center.target_vessel != None: # type: ignore
                            vessel.control.sas = True
                            vessel.control.sas_mode = krpc_conn.space_center.SASMode.target # type: ignore

                    # SAS Mode = anti target
                    case 10:
                        if krpc_conn.space_center.target_body != None and krpc_conn.space_center.target_docking_port != None and krpc_conn.space_center.target_vessel != None: # type: ignore
                            vessel.control.sas = True
                            vessel.control.sas_mode = krpc_conn.space_center.SASMode.anti_target # type: ignore

                    # SAS off
                    case 11:
                        vessel.control.sas = False

                    # RCS on
                    case 12:
                        vessel.control.rcs = True

                    # RCS off
                    case 13:
                        vessel.control.rcs = False

                    # Extend gear
                    case 14:
                        vessel.control.gear = True

                    # Retract gear
                    case 15:
                        vessel.control.gear = False

                    # Lights on
                    case 16:
                        vessel.control.lights = True

                    # Lights off
                    case 17:
                        vessel.control.lights = False

                    # Brakes on
                    case 18:
                        vessel.control.brakes = True

                    # Brakes off
                    case 19:
                        vessel.control.brakes = False

                    # Extend solar panels
                    case 20:
                        vessel.control.solar_panels = True

                    # Retract solar panels
                    case 21:
                        vessel.control.solar_panels = False

                    # Extend antennas
                    case 22:
                        vessel.control.antennas = True

                    # Retract antennas
                    case 23:
                        vessel.control.antennas = False

                    # Activate next stage
                    case 24:
                        vessel.control.activate_next_stage()

                    # Abort
                    case 25:
                        vessel.control.abort = True

            # Ascent guidance
            case 13:
                mechjeb = krpc_conn.mech_jeb # type: ignore
                ascent_guidance = mechjeb.ascent_autopilot
                otr_opts_dec = 0
                match noun:
                    # Insert orbit altitude, inclination, ascent profile {1, 2, 3}
                    case 1:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.desired_orbit_altitude = dsky_data_in[0] * 1000
                        if dsky_data_in[1] > -1000:
                            ascent_guidance.desired_inclination = dsky_data_in[1]
                        if dsky_data_in[2] > -2:
                            ascent_guidance.ascent_path_index = int(dsky_data_in[2] - 1)

                    # Display orbit altitude, inclination, ascent profile {1, 2, 3}
                    case 2:
                        dsky_send(int(ascent_guidance.desired_orbit_altitude / 1000), int(ascent_guidance.desired_inclination), ascent_guidance.ascent_path_index + 1)

                    # Insert acceleration limit, throttle limit
                    case 3:
                        if dsky_data_in[0] == -1:
                            ascent_guidance.thrust_controller.limit_acceleration = False
                        elif dsky_data_in[0] >= 0:
                            ascent_guidance.thrust_controller.limit_acceleration = True
                            ascent_guidance.thrust_controller.max_acceleration = dsky_data_in[0]

                        if dsky_data_in[1] == -1:
                            ascent_guidance.thrust_controller.limit_throttle = False
                        elif dsky_data_in[1] >= 0:
                            ascent_guidance.thrust_controller.limit_throttle = True
                            ascent_guidance.thrust_controller.max_throttle = dsky_data_in[1] / 100

                    # Display acceleration limit, throttle limit
                    case 4:
                        if ascent_guidance.thrust_controller.limit_acceleration == False:
                            max_acc = -1
                        else:
                            max_acc = int(ascent_guidance.thrust_controller.max_acceleration)

                        if ascent_guidance.thrust_controller.limit_throttle == False:
                            max_thr = -1
                        else:
                            max_thr = int(ascent_guidance.thrust_controller.max_throttle * 100)

                        dsky_send(max_acc, max_thr, 0)

                    # Insert force roll climb, turn
                    case 5:
                        if dsky_data_in[0] == -1:
                            ascent_guidance.force_roll = False
                        elif dsky_data_in[0] >= 0:
                            ascent_guidance.force_roll = True
                            ascent_guidance.turn_roll = dsky_data_in[0]
                            ascent_guidance.vertical_roll = dsky_data_in[1]

                    # Display force roll climb, turn
                    case 6:
                        if ascent_guidance.force_roll == False:
                            t_roll = -1
                            v_roll = -1
                        else:
                            t_roll = int(ascent_guidance.turn_roll)
                            v_roll = int(ascent_guidance.vertical_roll)

                        dsky_send(t_roll, v_roll, 0)

                    # Insert autostage pre-delay, post-delay, clamp autostage thrust
                    case 7:
                        if dsky_data_in[0] == -1:
                            ascent_guidance.staging_controller.enabled = False
                        elif dsky_data_in[0] >= 0:
                            ascent_guidance.staging_controller.enabled = True
                            ascent_guidance.staging_controller.autostage_pre_delay = dsky_data_in[0]
                            ascent_guidance.staging_controller.autostage_post_delay = dsky_data_in[1]
                            ascent_guidance.staging_controller.clamp_auto_stage_thrust_pct = dsky_data_in[2]

                    # Display autostage pre-delay, post-delay, clamp autostage thrust
                    case 8:
                        if ascent_guidance.staging_controller.enabled == False:
                            pre_delay = -1
                            post_delay = -1
                            clamp_stg = -1
                        else:
                            pre_delay = int(ascent_guidance.staging_controller.autostage_pre_delay)
                            post_delay = int(ascent_guidance.staging_controller.autostage_post_delay)
                            clamp_stg = ascent_guidance.staging_controller.clamp_auto_stage_thrust_pct

                        dsky_send(pre_delay, post_delay, clamp_stg)

                    # Insert min. throttle, stop stage, other options
                    case 9:
                        if dsky_data_in[0] == -1:
                            ascent_guidance.thrust_controller.limiter_min_throttle = False
                        elif dsky_data_in[0] >= 0:
                            ascent_guidance.thrust_controller.limiter_min_throttle = True
                            ascent_guidance.thrust_controller.min_throttle = dsky_data_in[0] / 100

                        if ascent_guidance.staging_controller.enabled == True and dsky_data_in[1] > -2:
                            ascent_guidance.staging_controller.autostage_limit = int(dsky_data_in[1])

                        if dsky_data_in[2] >= 0 and dsky_data_in[2] <= 15:
                            otr_opts_dec = int(dsky_data_in[2])
                            otr_opts = dec2bin(otr_opts_dec)
                            ascent_guidance.thrust_controller.limit_to_prevent_overheats = otr_opts[0]
                            ascent_guidance.autodeploy_solar_panels = otr_opts[1]
                            ascent_guidance.auto_deploy_antennas = otr_opts[2]
                            ascent_guidance.skip_circularization = otr_opts[3]

                    # Display min. throttle, stop stage, other options
                    case 10:
                        if ascent_guidance.thrust_controller.limiter_min_throttle == False:
                            min_thr = -1
                        else:
                            min_thr = int(ascent_guidance.thrust_controller.min_throttle * 100)

                        if ascent_guidance.staging_controller.enabled == False:
                            as_limit = -1
                        else:
                            as_limit = ascent_guidance.staging_controller.autostage_limit

                        dsky_send(min_thr, as_limit, otr_opts_dec)

                    # Classic ascent profile
                    # Insert turn start altitude, turn start velocity, turn end altitude
                    case 11:
                        if dsky_data_in[0] == -1:
                            ascent_guidance.ascent_path_classic.auto_path = True
                        elif dsky_data_in[0] >= 0:
                            ascent_guidance.ascent_path_classic.auto_path = False
                            ascent_guidance.ascent_path_classic.turn_start_altitude = dsky_data_in[0] * 1000
                            ascent_guidance.ascent_path_classic.turn_start_velocity = dsky_data_in[1]
                            ascent_guidance.ascent_path_classic.turn_end_altitude = dsky_data_in[2] * 1000

                    # Display turn start altitude, turn start velocity, turn end altitude
                    case 12:
                        if ascent_guidance.ascent_path_classic.auto_path == True:
                            turn_start = int(ascent_guidance.ascent_path_classic.auto_turn_start_altitude / 1000)
                            velo_start = int(ascent_guidance.ascent_path_classic.auto_turn_start_velocity)
                            turn_end = int(ascent_guidance.ascent_path_classic.auto_turn_end_altitude)
                        else:
                            turn_start = int(ascent_guidance.ascent_path_classic.turn_start_altitude / 1000)
                            velo_start = int(ascent_guidance.ascent_path_classic.turn_start_velocity)
                            turn_end = int(ascent_guidance.ascent_path_classic.turn_end_altitude / 1000)

                        dsky_send(turn_start, velo_start, turn_end)

                    # Insert final flight path angle, turn shape
                    case 13:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_classic.turn_end_angle = dsky_data_in[0]
                        if dsky_data_in[1] > -2:
                            ascent_guidance.ascent_path_classic.turn_shape_exponent = dsky_data_in[1] / 100

                    # Display final flight path angle, turn shape
                    case 14:
                        dsky_send(int(ascent_guidance.ascent_path_classic.turn_end_angle), int(ascent_guidance.ascent_path_classic.turn_shape_exponent * 100), 0)

                    # Stock style gravity turn ascent profile
                    # Insert turn start altitude, turn start velocity, turn start pitch
                    case 15:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_gt.turn_start_altitude = dsky_data_in[0] * 1000
                        if dsky_data_in[1] > -2:
                            ascent_guidance.ascent_path_gt.turn_start_velocity = dsky_data_in[1]
                        if dsky_data_in[2] > -2:
                            ascent_guidance.ascent_path_gt.turn_start_pitch = dsky_data_in[2]

                    # Display turn start altitude, turn start velocity, turn start pitch
                    case 16:
                        dsky_send(int(ascent_guidance.ascent_path_gt.turn_start_altitude / 1000), int(ascent_guidance.ascent_path_gt.turn_start_velocity), int(ascent_guidance.ascent_path_gt.turn_start_pitch))

                    # Insert intermediate altitude, hold AP time
                    case 17:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_gt.intermediate_altitude = dsky_data_in[0] * 1000
                        if dsky_data_in[1] > -2:
                            ascent_guidance.ascent_path_gt.hold_ap_time = dsky_data_in[1]

                    # Display intermediate altitude, hold AP time
                    case 18:
                        dsky_send(int(ascent_guidance.ascent_path_gt.intermediate_altitude / 1000), int(ascent_guidance.ascent_path_gt.hold_ap_time), 0)

                    # PVG ascent profile
                    # Insert target apoapsis, attach altitude, booster pitch start
                    case 19:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_pvg.desired_apoapsis = dsky_data_in[0] * 1000
                        if dsky_data_in[1] > -2:
                            ascent_guidance.ascent_path_pvg.desired_attach_alt = dsky_data_in[1] * 1000
                        if dsky_data_in[2] > -2:
                            ascent_guidance.ascent_path_pvg.pitch_start_velocity = dsky_data_in[2]

                    # Display target apoapsis, attach altitude, booster pitch start
                    case 20:
                        dsky_send(int(ascent_guidance.ascent_path_pvg.desired_attach_alt / 1000), int(ascent_guidance.ascent_path_pvg.desired_attach_alt / 1000), int(ascent_guidance.ascent_path_pvg.pitch_start_velocity))

                    # Insert booster pitch rate, PVG after stage
                    case 21:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_pvg.ascent_path_pvg.pitch_rate = dsky_data_in[0]

                        if(dsky_data_in[1] == -1):
                            ascent_guidance.ascent_path_pvg.staging_trigger_flag = False
                        elif(dsky_data_in[1] >= 0):
                            ascent_guidance.ascent_path_pvg.staging_trigger_flag = True
                            ascent_guidance.ascent_path_pvg.staging_trigger = int(dsky_data_in[1])

                        ascent_guidance.ascent_path_pvg.dynamic_pressure_trigger = dsky_data_in[2]

                    # Display booster pitch rate, PVG after stage, 
                    case 22:
                        if ascent_guidance.ascent_path_pvg.staging_trigger_flag == False:
                            stag_trig = -1
                        else:
                            stag_trig = ascent_guidance.ascent_path_pvg.staging_trigger

                        dsky_send(int(ascent_guidance.ascent_path_pvg.ascent_path_pvg.pitch_rate), stag_trig, 0)

                    # Insert Q trigger, fixed coast length
                    case 23:
                        if dsky_data_in[0] > -2:
                            ascent_guidance.ascent_path_pvg.dynamic_pressure_trigger = dsky_data_in[0]

                        if dsky_data_in[1] == -1:
                            ascent_guidance.ascent_path_pvg.fixed_coast = False
                        elif dsky_data_in[1] >= 0:
                            ascent_guidance.ascent_path_pvg.fixed_coast = True
                            ascent_guidance.ascent_path_pvg.fixed_coast_length = dsky_data_in[1]

                    # Display Q trigger, fixed coast length
                    case 24:
                        if ascent_guidance.ascent_path_pvg.fixed_coast == False:
                            fix_coast = -1
                        else:
                            fix_coast = int(ascent_guidance.ascent_path_pvg.fixed_coast_length)

                        dsky_send(int(ascent_guidance.ascent_path_pvg.dynamic_pressure_trigger), fix_coast , 0)

                    # Enage autopilot
                    case 25:
                        if(dsky_data_in[0] == 1):
                            ascent_guidance.enabled = True
                            if vessel.situation == krpc_conn.space_center.VesselSituation.pre_launch: # type: ignore
                                vessel.control.activate_next_stage()

                    # Disenage autopilot
                    case 26:
                        if(dsky_data_in[0] == 1):
                            ascent_guidance.enabled = False

                    case _:
                        dsky_send(0,0,0)

            # Testing output
            case 99:
                match noun:
                    case 2:
                        dsky_send(tst, 0, 0)
                        tst += 1
                    case _:
                        dsky_send(0,0,0)

            case _:
                dsky_send(0,0,0)
    else:
        dsky_send_empty_string()
