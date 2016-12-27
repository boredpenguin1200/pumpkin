# Echo server program
import socket

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 50007              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)
conn, addr = s.accept()
print 'Connected by', addr
while 1:
    data = conn.recv(1024)
    if not data: break
    conn.sendall(data)
conn.close()











import serial
# import getchr
import time
import readchar
import sys


class _GetchUnix:
    def __init__(self):
        import tty
        import sys

    def __call__(self):
        import sys
        import tty
        import termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch


serName = "/dev/ttyACM0"

ser = serial.Serial(serName, 115200, timeout=1)
# ser.write(">".encode('latin-1'))
# ser.readline()

# ser.write("M1 1 100\n".encode("latin-1"))
# print(ser.readline())

m1Percent = 0
m2Percent = 0

def setMotor1Percent(percent):
    ser.write("M1 1 {percent:03d}\n".format(percent=(percent)).encode("latin-1"))
    print(ser.readline())
def setMotor2Percent(percent):
    ser.write("M2 1 {percent:03d}\n".format(percent=(percent)).encode("latin-1"))
    print(ser.readline())
def flushBuf():
    ser.write('>'.encode('latin-1'))
    ser.readline()
while(1):
    flushBuf()
    c = input('move(wasd):')
    # getchr = _GetchUnix
    # c = getchr()
    if c == 'w':
        setMotor1Percent(100)
        setMotor2Percent(100)
        time.sleep(.1)
        setMotor1Percent(0)
        setMotor2Percent(0)
    elif c == 'a':
        setMotor1Percent(20)
        setMotor2Percent(0)
        time.sleep(.1)
        setMotor1Percent(0)
        setMotor2Percent(0)
    elif c == 'd':
        setMotor1Percent(0)
        setMotor2Percent(20)
        time.sleep(.1)
        setMotor1Percent(0)
        setMotor2Percent(0)