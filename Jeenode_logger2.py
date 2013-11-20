import serial
import datetime

ser = serial.Serial('COM15', 57600, timeout = 2)

linenum = 0

while 1:
    now = datetime.datetime.now()
    analog = ser.readline()
    yr = '%02d' % now.year
    mon = '%02d' % now.month
    dy = '%02d' % now.day
    hr = '%02d' % now.hour
    min = '%02d' % now.minute
    sec = '%02d' % now.second

    dte = str(mon) + '/' + str(dy) + '/' + str(yr)
    tme = str(hr) + ':' + str(min) + ':' + str(sec)

    dtme = dte + " " + tme
    heading = "Date, Time, Sensor Reading"

    if linenum < 10:
        lineprt = analog
    else:
        lineprt = dte + "," + tme + "," + analog

    if analog != "":
        log_file = open("pythonoutput.csv","w")
        print lineprt
        log_file.write(heading + '\n')
        log_file.write(lineprt)
        log_file.flush()

    linenum = linenum + 1
