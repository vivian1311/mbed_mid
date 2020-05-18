import numpy as np
import serial
import time
waitTime = 0.1
# generate the waveform table
song1=np.array([ 
  #40 note
  # Little star
  261, 261, 392, 392, 440, 440, 392, 349, 349, 330, 330, 294, 
  294, 261, 392, 392, 349, 349, 330, 330, 294, 392, 392, 349, 
  349, 330, 330, 294, 261, 261, 392, 392, 440, 440, 392, 349, 
  349, 330, 330, 294
  ])

noteLength1 = np.array([
# Little star
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
  2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1
  ])

song2=np.array([  
  #42 note
  # Little bee
  392, 330, 330, 349, 294, 294, 262, 294, 300, 349, 392, 392, 
  392, 392, 330, 330, 349, 294, 294, 262, 330, 392, 392, 330, 
  392, 330, 330, 349, 294, 294, 262, 294, 300, 349, 392, 392, 
  392, 392, 330, 330
  ])


noteLength2 = np.array([
# Little bee
  1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 
  1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2
  ])

song3=np.array([  
  # 24 note
  # Jingle bell  
  330, 330, 330, 330, 330, 330, 330, 392, 262, 294, 330, 349, 
  349, 349, 349, 330, 330, 330, 330, 294, 294, 330, 294, 392
  ])

noteLength3 = np.array([
  # Jingle bell
  1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
  1, 1, 2, 2
  ])


# output formatter
formatter = lambda x: "%d" % x
# send the waveform table to K66F
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")

#print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
for data1_1 in song1:
  s.write(bytes(formatter(data1_1), 'UTF-8'))
  time.sleep(waitTime)
for data1_2 in noteLength1:
  s.write(bytes(formatter(data1_2), 'UTF-8'))
  time.sleep(waitTime)

for data2_1 in song2:
  s.write(bytes(formatter(data2_1), 'UTF-8'))
  time.sleep(waitTime)
for data2_2 in noteLength2:
  s.write(bytes(formatter(data2_2), 'UTF-8'))
  time.sleep(waitTime)

for date3_1 in song3:
  s.write(bytes(formatter(date3_1), 'UTF-8'))
  time.sleep(waitTime)
for data3_2 in noteLength3:
  s.write(bytes(formatter(data3_2), 'UTF-8'))
  time.sleep(waitTime)


s.close()

print("Signal sended")