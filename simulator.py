import sys, os , commands, collections, time, datetime
import numpy as np
import matplotlib.pyplot as plt 
from os import listdir
from os.path import isfile, join

BASE_DIRECTORY = "."
DATA_DIRECTORY = BASE_DIRECTORY+"/data"
RESULT_DIRECTORY = BASE_DIRECTORY+"/result"
SCRIPT_DIRECTORY = BASE_DIRECTORY+"/script"

if len(sys.argv) <= 1:
	print "You have to specify the number of components"
	exit(-1)

number_component = int(sys.argv[1])
time_remapping = int(sys.argv[2])

print "Number of component "+str(number_component)
print "Time remapping "+str(time_remapping)

#load all file in directory data where there are all information obtained with hotspot
#file name is indexconfig_statecomp1;indexconfig_statecomp2....txt
#statecompx = [0,1] 0 if failed, 1 otherwise
#map with first number as number of working state
map_files = {}
#list of all files in data directory
only_files = [f for f in listdir(DATA_DIRECTORY) if isfile(join(DATA_DIRECTORY,f))]

#create map with (index_config -> name file)
for file_data in only_files:
	map_files[file_data.split('_')[0]] = file_data.split('_')[1]

#sort map according to index_config
map_files = collections.OrderedDict(sorted(map_files.items()))


for key in map_files.keys():
	print map_files[key]

#extract current timestamp 
string_timestamp = str(datetime.datetime.now()).replace(" ","_")

#output file: source data for caliper simulation
out_file = open(RESULT_DIRECTORY+"/data_for_caliper_"+string_timestamp+".txt","aw")

list_states = []

#for every key(index_config)
for key in map_files.keys():

	dictionary = {}
	f = open(DATA_DIRECTORY+"/"+key+"_"+map_files[key])
	count = 0
	#load data from current output hotspot
	#create a dictionary with (name_component -> steady-state temperature)
	for line in f:
		count = count +1
		if count > number_component:
			break
		else:
			dictionary[line.split('\t')[0]] = line.split('\t')[1].replace("\n", "")

	f.close()


	count = 1
	#retrieve failed and working components parsing name file previously load
	name_file_splits = (map_files[key].replace(".txt","")).split(':')

	index = 0
	config_map = {}
	#write info for caliper on which component are failed
	for info in name_file_splits:
		if info == "0":
			config_map[str(index)] = "failed"
		index = index + 1
	
	if len(config_map.keys()) == 0:
		out_file.write("#")
	else:
		first_item = True
		to_be_written = ""
		for failed in config_map.keys():
			if first_item:
				to_be_written = to_be_written + str(failed)
				first_item = False
			else:
				to_be_written = to_be_written + "," + str(failed)
		print to_be_written
		if to_be_written in list_states:
		  first_elem = to_be_written.split(",")[0]
		  second_elem = to_be_written.split(",")[1]
		  to_be_written = second_elem+","+first_elem
		print to_be_written
		if len(to_be_written) > 1:
		  list_states.append(to_be_written)
		out_file.write(to_be_written)

	out_file.write(" ")
	
	#for every component compute alpha parameter using computeAlpha
	#and write in file for caliper simulation
	print "::::::::::::::::::::::::::::::::::::::::::"
	dictionary = collections.OrderedDict(sorted(dictionary.items()))
	for key in dictionary.keys():
		print key +  " " +dictionary[key]

		status, output = commands.getstatusoutput(SCRIPT_DIRECTORY+"/computeAlpha "+str(dictionary[key]))
		alpha = output.split(': ')[1]
		#output is the alpha of the component
		out_file.write(str(alpha))
		if count < number_component:
			out_file.write(' ')

	out_file.write('\n')

out_file.close()

command_montecarlo = None
if time_remapping == 0:
  command_montecarlo = SCRIPT_DIRECTORY+"/caliper  -f "+RESULT_DIRECTORY+"/data_for_caliper_"+string_timestamp+".txt -t 100000 -o "+RESULT_DIRECTORY+"/reliability_curve_"+string_timestamp+".txt"
else:
  command_montecarlo = SCRIPT_DIRECTORY+"/caliper  -f "+RESULT_DIRECTORY+"/data_for_caliper_"+string_timestamp+".txt -t 10000 -p "+str(time_remapping)+ " -o "+RESULT_DIRECTORY+"/reliability_curve_"+string_timestamp+".txt"

print command_montecarlo
#montecarlo simulation
status, output = commands.getstatusoutput(command_montecarlo)
print output

#print result
dataset = np.genfromtxt(fname=RESULT_DIRECTORY+'/reliability_curve_'+string_timestamp+'.txt',skip_header=0) 


x=dataset[:,0] 
y=dataset[:,1] 

fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_title("System Reliability")    
ax1.set_xlabel('time[hour]')
ax1.set_ylabel('R(t)')

ax1.plot(x,y, c='r', label='Reliability')

leg = ax1.legend()

plt.show()
