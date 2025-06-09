import krpc
import krpc.attributes
import krpc.types
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

# Main program:

print("KGC 0.3.1")

serial_port = input("Serial port? ")
ser_conn = serial.Serial(serial_port, 115200)
ser_conn.bytesize = serial.EIGHTBITS
ser_conn.stopbits = serial.STOPBITS_ONE
ser_conn.parity = serial.PARITY_EVEN
ser_conn.timeout = None

krpc_conn = krpc.connect(name='KGC 0.3.1')
if ser_conn.is_open == False:
    ser_conn.open()

tst = 1024
verb = 0
noun = 0

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

                    # Noun 3 = Mass + available thurst + specific impulse
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
                        dsky_send(int(vessel.flight().mean_altitude), int(vessel.orbit.apoapsis), inc)

                    # Noun 3 = Apoapsis + periapsis + inclination
                    case 3:
                        inc = int((vessel.orbit.inclination * 180 / math.pi) * 1000) # Convert radians to degrees
                        dsky_send(int(vessel.orbit.apoapsis_altitude), int(vessel.orbit.periapsis_altitude), inc)

                    case _:
                        dsky_send(0,0,0)

            # Verb 4 = Maneuver
            case 4:
                if len(vessel.control.nodes) == 0:
                    dsky_send_empty_string()
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
