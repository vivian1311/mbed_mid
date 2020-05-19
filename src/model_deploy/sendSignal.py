import numpy as np
import serial
import time
waitTime = 0.1
# generate the waveform table
song1=np.array([ 
  #40 note
  # Little star
  0.261, 0.261, 0.392, 0.392, 0.440, 0.440, 0.392, 0.349, 0.349, 0.330, 
  0.330, 0.294, 0.294, 0.261, 0.392, 0.392, 0.349, 0.349, 0.330, 0.330, 
  0.294, 0.392, 0.392, 0.349, 0.349, 0.330, 0.330, 0.294, 0.261, 0.261, 
  0.392, 0.392, 0.440, 0.440, 0.392, 0.349, 0.349, 0.330, 0.330, 0.294
  ])

noteLength1 = np.array([
# Little star
  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001,
  0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 
  0.002, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 
  0.001, 0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001, 0.001, 0.001
  ])

song2=np.array([  
  #42 note
  # Little bee
  0.392, 0.330, 0.330, 0.349, 0.294, 0.294, 0.262, 0.294, 0.330, 0.349, 
  0.392, 0.392, 0.392, 0.392, 0.330, 0.330, 0.349, 0.294, 0.294, 0.262, 
  0.330, 0.392, 0.392, 0.330, 0.392, 0.330, 0.330, 0.349, 0.294, 0.294, 
  0.262, 0.294, 0.300, 0.349, 0.392, 0.392, 0.392, 0.392, 0.330, 0.330
  ])


noteLength2 = np.array([
# Little bee
  0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001, 0.001, 
  0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 0.001, 
  0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 
  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.001, 0.001, 0.002
  ])

song3=np.array([  
  # 24 note
  # Jingle bell  
  0.330, 0.330, 0.330, 0.330, 0.330, 0.330, 0.330, 0.392, 0.262, 0.294, 
  0.330, 0.349, 0.349, 0.349, 0.349, 0.330, 0.330, 0.330, 0.330, 0.294, 
  0.294, 0.330, 0.294, 0.392, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 
  0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000
  ])

noteLength3 = np.array([
  # Jingle bell
  0.001, 0.001, 0.002, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001, 0.001, 
  0.002, 0.001, 0.001, 0.002, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 
  0.001, 0.001, 0.002, 0.002, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 
  0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000
  ])


# output formatter
formatter = lambda x: "%.3f" % x
# send the waveform table to K66F
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")

print("It may take about %d seconds ..." % (int(np.size(song1)*3 + np.size(noteLength1)*3) * waitTime))
for data1_1 in song1:
  s.write(bytes(formatter(data1_1), 'UTF-8'))
  print("%f\n", data1_1)
  time.sleep(waitTime)

for data2_1 in song2:
  s.write(bytes(formatter(data2_1), 'UTF-8'))
  print("%f\n", data2_1)
  time.sleep(waitTime)

for date3_1 in song3:
  s.write(bytes(formatter(date3_1), 'UTF-8'))
  print("%f\n", date3_1)
  time.sleep(waitTime)

for data1_2 in noteLength1:
  s.write(bytes(formatter(data1_2), 'UTF-8'))
  print("%f\n", data1_2)
  time.sleep(waitTime)

for data2_2 in noteLength2:
  s.write(bytes(formatter(data2_2), 'UTF-8'))
  print("%f\n", data2_2)
  time.sleep(waitTime)

for data3_2 in noteLength3:
  s.write(bytes(formatter(data3_2), 'UTF-8'))
  print("%f\n", data3_2)
  time.sleep(waitTime)

s.close()

print("Signal sended")