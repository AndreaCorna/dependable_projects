import numpy as np
import matplotlib.pyplot as plt

dataset = np.genfromtxt(fname='/home/andrea/aging.txt',skip_header=0) 

x=dataset[:,0] 
y=dataset[:,1] 

fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_title("System Reliability")    
ax1.set_xlabel('time[hour]')
ax1.set_ylabel('R(t)')

ax1.plot(x,y, c='r', label='R(t) avg aging rate approach')

dataset = np.genfromtxt(fname='/home/andrea/mean.txt',skip_header=0) 


x=dataset[:,0] 
y=dataset[:,1] 


ax1.plot(x,y, c='b', label='R(t) mean power approach')

leg = ax1.legend()

plt.show()
