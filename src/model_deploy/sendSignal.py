import numpy as np
import serial
import time
waitTime = 0.1
# generate the song
Song = np.array([
# Little star
  261, 261, 392, 392, 440, 440, 392, 349, 349, 330, 330, 294, 
  294, 261, 392, 392, 349, 349, 330, 330, 294, 392, 392, 349, 
  349, 330, 330, 294, 261, 261, 392, 392, 440, 440, 392, 349, 
  349, 330, 330, 294,           
# Little bee
  392, 330, 330, 349, 294, 294, 262, 294, 300, 349, 392, 392, 
  392, 392, 330, 330, 349, 294, 294, 262, 330, 392, 392, 330, 
  392, 330, 330, 349, 294, 294, 262, 294, 300, 349, 392, 392, 
  392, 392, 330, 330,     
# Jingle bell  
  330, 330, 330, 330, 330, 330, 330, 392, 262, 294, 330, 349, 
  349, 349, 349, 330, 330, 330, 330, 294, 294, 330, 294, 392
])

noteLength = np.array([
# Little star
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
  2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,                                            
# Little bee
  1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 
  1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2,     
# Jingle bell
  1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
  1, 1, 2, 2
])


formatter = lambda x: "%d" % x

# send the waveform table to K66F
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")
print("It may take about %d seconds ..." % (int(np.size(Song) * waitTime)))

for data1 in Song:
  s.write(bytes(formatter(data1), 'UTF-8'))
  time.sleep(waitTime)
for data2 in noteLength:
  s.write(bytes(formatter(data2), 'UTF-8'))
  time.sleep(waitTime)

s.close()
print("Signal sended")