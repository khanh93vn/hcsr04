import os.path
import serial
import numpy as np
import matplotlib.pyplot as plt

from struct import unpack

# -----------------------------------------------------------------------------
# Constants

UP_UINT16 = '1H'
DEFAULT_BUFFER_PATH = ".\results\result"

# -----------------------------------------------------------------------------
# SonarCom class

class Sonar:
    """
    Maintain connection, send instruction and read from arduino.
    """
    
    def __init__(self, bufferSize, *args, port='COM4', baudrate=115200, **kwargs):
        """
        Instantiate object with connected port and baudrate.
        """
        self.bufferReadSize = bufferSize * 2
        self.unpackFormat = str(bufferSize) + "H"
        self.stream = serial.Serial(port, baudrate)
        
        self.buffer = object()
    
    def close(self):
        self.stream.flushOutput()
        self.stream.flushInput()
        self.stream.close()
    
    def send(self, message):
        self.stream.write(message.encode())
    
    def receive(self,
                size=1,
                format='1B'):
        return np.array(unpack(format,
                               self.stream.read(size)))
        
    def measureOnce(self):
        self.send('m')
        return self.receive(2, '1H')
    
    def measureManyTimes(self):
        self.send('M')
        return self.receive(self.bufferReadSize, self.unpackFormat)
    
    def readBuffer(self):
        self.send('r')
        
        # Read buffer size:
        size = unpack(UP_UINT16, self.stream.read(2))[0]
        print(size)
        
        # Read buffer:
        format = str(size) + 'H'
        recv_str = self.stream.read(size*2)
        
        # Return:
        self.buffer = np.array(unpack(format, recv_str))
        return self.buffer
        
    def saveBuffer(self, path=DEFAULT_BUFFER_PATH):
        
        for i in range(1000):
            s = DEFAULT_BUFFER_PATH + "{0:03d}".format(i)
            if not os.path.exist(s):
                break
        self.buffer.tofile(s)

# -----------------------------------------------------------------------------
# Functions

def pulselen2cm(l):
    return l * 0.034 / 2

if __name__ == "__main__":
    sonar = Sonar(256, timeout=10)
    print("sonar is ready!")
    
    # data.tofile("data.txt")
    # con.close()