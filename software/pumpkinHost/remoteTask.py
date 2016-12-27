# Echo client program
import socket, time

s = ''

def getSocket():
    global s
    HOST = 'localhost'  # The remote host
    PORT = 1336  # The same port as used by the server
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    # data = s.recv(1024)
    # s.timeout = .1
    return s

def setMotor1Percent(percent, dirFor=True):
    if dirFor:
        dirFor = '+'
    else:
        dirFor = '-'
    string = "M2 {dirFor}{percent:03d}\n".format(percent=percent, dirFor=dirFor)
    s.send(string.encode("latin-1"))
    print(s.recv(1024))


def setMotor2Percent(percent, dirFor=True):
    if dirFor:
        dirFor = '+'
    else:
        dirFor = '-'
    s.send("M2 {dirFor}{percent:03d}\n".format(percent=percent, dirFor=dirFor).encode("latin-1"))
    print(s.recv(1024))


def flushBuf():
    s.send('>'.encode('latin-1'))
    print(s.recv(1024))

def forward():
    flushBuf()
    setMotor1Percent(50)
    setMotor2Percent(50)
    time.sleep(.1)
    setMotor1Percent(0)
    setMotor2Percent(0)

def backward():
    setMotor1Percent(50, dirFor=False)
    setMotor2Percent(50, dirFor=False)
    time.sleep(.1)
    setMotor1Percent(0)
    setMotor2Percent(0)

def left():
    setMotor1Percent(50, dirFor=False)
    setMotor2Percent(50, dirFor=True)
    time.sleep(.1)
    setMotor1Percent(0)
    setMotor2Percent(0)

def right():
    setMotor1Percent(50, dirFor=True)
    setMotor2Percent(50, dirFor=False)
    time.sleep(.1)
    setMotor1Percent(0)
    setMotor2Percent(0)


