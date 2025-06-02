import krpc
import serial

serial_port = input("Serial port? ")
ser_conn = serial.Serial(serial_port, 115200)
ser_conn.bytesize = serial.EIGHTBITS
ser_conn.stopbits = serial.STOPBITS_ONE
ser_conn.parity = serial.PARITY_EVEN
ser_conn.timeout = None

krpc_conn = krpc.connect(name='KGC 0.02')
if ser_conn.is_open == False:
    ser_conn.open()

vessel = krpc_conn.space_center.active_vessel # type: ignore

# flight = krpc_conn.add_stream(vessel.flight)

print(vessel.name)

tst = 1024
verb = 0
noun = 0

while True:
#    print("Altitude: ", flight().surface_altitude, flush=True)
#    print("Periapsis: ", vessel.orbit.periapsis, flush=True)
#    print("Waiting bytes: ", ser_conn.in_waiting)
    ser_in = ser_conn.readline()
    ser_str = ser_in.decode("utf-8", "strict")
    ser_in_str = ser_str.strip()
    ser_in_list = ser_str.split(';')
    print(ser_str)

    for i in ser_in_list:
        if i.startswith("V:"):
            verb = i.split(':')
            verb = verb[1].strip()
            verb = int(verb)
        elif i.startswith("N:"):
            noun = i.split(':')
            noun = noun[1].strip()
            noun = int(verb)
        print("Verb is ", verb, ", noun is ", verb)

    match verb:
        # Verb 1 = Display flight info
        case 1:
            match noun:
                # Noun 1 = Altitude + Apoapsis + Periapsis
                case 1:
                    ser_out_str = str(int(vessel.flight().surface_altitude)) + ";" + str(int(vessel.orbit.apoapsis_altitude)) + ";" + str(int(vessel.orbit.periapsis_altitude))
                    ser_out = ser_out_str.encode("utf-8")
                    ser_conn.write(ser_out)
        case 2:
            ser_out_str = str(tst) + "\n"
            ser_out = ser_out_str.encode("utf-8")
            ser_conn.write(ser_out)
            tst += 1
