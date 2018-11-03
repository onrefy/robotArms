import serial
import json
import time
ser = serial.Serial('/dev/ttyUSB0')

from CommonDetector import Classifier
c = Classifier(VideoCaptureIndex=0)
c.Start()# start the detection system
i = 0
while(True):
    time.sleep(0.01)
    read_data = bytes.decode(ser.read())
    print(read_data)
#    i = int(read_data)
    if (read_data=='0'):
        i=0
    elif (read_data=='1'):
        i=1
    elif (read_data=='2'):
        i=2
    elif (read_data=='4'):
        send_data = {'mode':'4','value':'0'}
        js = json.dumps(send_data)
        ser.write(str.encode(js))
        continue
    else:
        continue
    # print(c.getValue(1))
    # value=-1
    value = c.getValue(i)
    print ('result:\t',value)
    if(value != -1):
        send_data = {'mode':str(i),'value':str(value)}
        js = json.dumps(send_data)
        ser.write(str.encode(js))

    if not c.IsAlive(): break
