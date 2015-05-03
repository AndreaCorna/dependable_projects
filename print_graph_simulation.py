import numpy 
import matplotlib.pyplot as plt 

dataset = numpy.genfromtxt(fname='./result.txt',skip_header=0) 

print dataset 

x=dataset[:,0] 
y=dataset[:,1] 

fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_title("Plot title...")    
ax1.set_xlabel('your x label..')
ax1.set_ylabel('your y label...')

ax1.plot(x,y, c='r', label='the data')

leg = ax1.legend()

plt.show()