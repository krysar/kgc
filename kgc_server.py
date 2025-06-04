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
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.flight":
            fld = 1

            if krpc_conn.space_center.active_vessel.control.sas: # type: ignore
                sas = 1
            else:
                sas = 0

            if krpc_conn.space_center.active_vessel.control.rcs: # type: ignore
                rcs = 1
            else:
                rcs = 0

            if krpc_conn.space_center.active_vessel.control.gear: # type: ignore
                gea = 1
            else:
                gea = 0

            if krpc_conn.space_center.active_vessel.control.brakes: # type: ignore
                brk = 1
            else:
                brk = 0

        case "GameScene.tracking_station":
            fld = 2
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.editor_vab":
            fld = 3
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case "GameScene.editor_sph":
            fld = 4
            sas = 0
            rcs = 0
            gea = 0
            brk = 0
        case _:
            fld = -1
            sas = 0
            rcs = 0
            gea = 0
            brk = 0

    if krpc_conn.krpc.paused: # type: ignore
        pau = 1
    else:
        pau = 0

    # TODO: support for ELC, MOP, LFO and OTR
    elc = 0
    mop = 0
    lfo = 0
    otr = 0

    ser_out = str(num1) + ";" + str(num2) + ";" + str(num3) + ";"
    ser_out += str(fld) + ";" + str(elc) + ";" + str(mop) + ";"
    ser_out += str(lfo) + ";" + str(otr) + ";" + str(sas) + ";"
    ser_out += str(rcs) + ";" + str(gea) + ";" + str(brk) + ";" + str(pau)

    ser_out = ser_out.encode("utf-8", "strict")
    ser_conn.write(ser_out)

# Main program:

print("KGC 0.2")

serial_port = input("Serial port? ")
ser_conn = serial.Serial(serial_port, 115200)
ser_conn.bytesize = serial.EIGHTBITS
ser_conn.stopbits = serial.STOPBITS_ONE
ser_conn.parity = serial.PARITY_EVEN
ser_conn.timeout = None

krpc_conn = krpc.connect(name='KGC 0.02')
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
                        inc = int(vessel.orbit.inclination * 180 / math.pi) # Convert radians to degrees
                        ano = int(vessel.orbit.mean_anomaly * 180 / math.pi)
                        ecc = int(vessel.orbit.eccentricity * 10000)
                        dsky_send(inc, ecc, ano)

                    # Noun 3 = Time to apoapsis [ss:mm:hhhh]
                    case 3:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_hhmmss(tta)
                        dsky_send(tta[2], tta[1], tta[0])
                    
                    # Noun 4 = Time to apoapsis [mm:hh:dddd]
                    case 4:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_ddhhmm(tta)
                        dsky_send(tta[2], tta[1], tta[0])

                    # Noun 5 = Time to periapsis [ss:mm:hhhh]
                    case 5:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_hhmmss(ttp)
                        dsky_send(ttp[2], ttp[1], ttp[0])

                    # Noun 6 = Time to periapsis [mm:hh:dddd]
                    case 6:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_ddhhmm(ttp)
                        dsky_send(ttp[2], ttp[1], ttp[0])

                    # Noun 7 = Orbit period [ss:mm:hhhh]
                    case 7:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_hhmmss(orp)
                        dsky_send(orp[2], orp[1], orp[0])

                    # Noun 8 = Orbit period [mm:hh:dddd]
                    case 8:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_ddhhmm(orp)
                        dsky_send(orp[2], orp[1], orp[0])

                    # Noun 9 = Time to sphere of influence change [ss:mm:hhhh]
                    case 9:
                        soic = int(vessel.orbit.time_to_soi_change)
                        soic = sec_to_hhmmss(soic)
                        dsky_send(soic[2], soic[1], soic[0])

                    # Noun 10 = Time to sphere of influence change [mm:hh:dddd]
                    case 10:
                        soic = int(vessel.orbit.time_to_soi_change)
                        soic = sec_to_ddhhmm(soic)
                        dsky_send(soic[2], soic[1], soic[0])

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
                        dsky_send(0, 0, 0)

            # Verb 2 = Display vessel info
            case 2:
                  match noun:
                    # Noun 1 = Mission Elapsed Time [ss:mm:hhhh]    
                    case 1:
                          met = int(vessel.met)
                          met = sec_to_hhmmss(met)
                          dsky_send(met[2], met[1], met[0])

                    # Noun 2 = Mission Elapsed Time [mm:hh:dddd]
                    case 2:
                          met = int(vessel.met)
                          met = sec_to_ddhhmm(met)
                          dsky_send(met[2], met[1], met[0])

                    # Noun 3 = Mass + available thurst + specific impulse
                    case 3:
                          dsky_send(int(vessel.mass), int(vessel.available_thrust), int(vessel.specific_impulse))


            # Testing output
            case 99:
                match noun:
                    case 2:
                        dsky_send(tst, 0, 0)
                        tst += 1

            case _:
                dsky_send(0, 0, 0)
