import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import datetime as dt

# Testig diagram
tname = 'real_data/testing.txt'

testing_dates = []
testing_week = []
testing_total = []
with open(tname, 'r') as fin:
	next(fin)
	for line in fin:
		temp = line.strip().split()
		testing_dates.append(dt.datetime.strptime(temp[0], '%m/%d/%Y'))
		testing_week.append(float(temp[6]))
		testing_total.append(int(temp[8]))

plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=5))
plt.plot(testing_dates,testing_week)
plt.gcf().autofmt_xdate()
plt.show()

#np.random.seed(1)
#
#N = 100
#y = np.random.rand(N)
#
#now = dt.datetime.now()
#then = now + dt.timedelta(days=100)
#days = mdates.drange(now,then,dt.timedelta(days=1))
#
#plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
#plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=5))
#plt.plot(days,y)
#plt.gcf().autofmt_xdate()
#plt.show()
