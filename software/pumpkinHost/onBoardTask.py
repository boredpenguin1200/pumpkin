# Echo server program
import socket, serial

serName = "/dev/ttyACM0"
ser = serial.Serial(serName, 115200, timeout=1)

while(1):
    HOST = ''  # Symbolic name meaning all available interfaces
    PORT = 1336  # Arbitrary non-privileged port
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    print('Connected by', addr)
    conn.send("pumpkin".encode('latin-1'))
    while 1:
        data = conn.recv(1024)
        if data:
            ser.write(data)
            conn.send(ser.readline())
    conn.close()
