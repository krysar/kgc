import krpc
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
                        ser_out_str = str(int(vessel.flight().mean_altitude)) + ";" + str(int(vessel.orbit.apoapsis_altitude)) + ";" + str(int(vessel.orbit.periapsis_altitude))
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 2 = Inclination + eccentricity + mean anomaly
                    case 2:
                        inc = int(vessel.orbit.inclination * 180 / math.pi) # Convert radians to degrees
                        ser_out_str = str(inc) + ";" + str(int(vessel.orbit.eccentricity)) + ";" + str(int(vessel.orbit.mean_anomaly))
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 3 = Time to apoapsis [ss:mm:hhhh]
                    case 3:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_hhmmss(tta)
                        ser_out_str = str(tta[2]) + ";" + str(tta[1]) + ";" + str(tta[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)
                    
                    # Noun 4 = Time to apoapsis [mm:hh:dddd]
                    case 4:
                        tta = int(vessel.orbit.time_to_apoapsis)
                        tta = sec_to_ddhhmm(tta)
                        ser_out_str = str(tta[2]) + ";" + str(tta[1]) + ";" + str(tta[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 5 = Time to periapsis [ss:mm:hhhh]
                    case 5:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_hhmmss(ttp)
                        ser_out_str = str(ttp[2]) + ";" + str(ttp[1]) + ";" + str(ttp[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 6 = Time to periapsis [mm:hh:dddd]
                    case 6:
                        ttp = int(vessel.orbit.time_to_periapsis)
                        ttp = sec_to_ddhhmm(ttp)
                        ser_out_str = str(ttp[2]) + ";" + str(ttp[1]) + ";" + str(ttp[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 7 = Orbit period [ss:mm:hhhh]
                    case 7:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_hhmmss(orp)
                        ser_out_str = str(orp[2]) + ";" + str(orp[1]) + ";" + str(orp[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 8 = Orbit period [mm:hh:dddd]
                    case 8:
                        orp = int(vessel.orbit.period)
                        orp = sec_to_ddhhmm(orp)
                        ser_out_str = str(orp[2]) + ";" + str(orp[1]) + ";" + str(orp[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 9 = Time to sphere of influence change [ss:mm:hhhh]
                    case 9:
                        soic = int(vessel.orbit.time_to_soi_change)
                        soic = sec_to_hhmmss(soic)
                        ser_out_str = str(soic[2]) + ";" + str(soic[1]) + ";" + str(soic[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 10 = Time to sphere of influence change [mm:hh:dddd]
                    case 10:
                        soic = int(vessel.orbit.time_to_soi_change)
                        soic = sec_to_ddhhmm(soic)
                        ser_out_str = str(soic[2]) + ";" + str(soic[1]) + ";" + str(soic[0])
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 11 = Heading + pitch + roll
                    case 11:
                        ser_out_str = str(int(vessel.flight(vsl_frame).heading)) + ";" + str(int(vessel.flight(vsl_frame).pitch)) + ";" + str(int(vessel.flight(vsl_frame).roll))
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)

                    # Noun 12 = Velocity
                    case 12:
                        ser_out_str = str(int(vessel.velocity(srf_frame)[0])) + ";" + str(int(vessel.velocity(srf_frame)[1])) + ";" + str(int(vessel.velocity(srf_frame)[2]))
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)
                        

            # Testing output
            case 99:
                match noun:
                    case 2:
                        ser_out_str = str(tst) + "\n"
                        ser_out = ser_out_str.encode("utf-8")
                        ser_conn.write(ser_out)
                        tst += 1
