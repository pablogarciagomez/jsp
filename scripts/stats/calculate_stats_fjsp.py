import glob
import os
import sys
import re
import statistics

execution_times = list()
values = list()

for filename in sys.argv[2:]:
    f = open(filename)
    trace = f.read()
    f.close()
    execution_times.append(
        float(re.search('Execution Time = (.*)', trace).group(1)))
    values.append(
        float(re.search('Expected Makespan = (.*)', trace).group(1)))

f = open(sys.argv[1], 'w+')

f.write('Best = '+str(min(values))+'\n')
f.write('Average = '+str(statistics.mean(values))+'\n')
f.write('Standard deviation = '+str(statistics.stdev(values))+'\n')
f.write('Average execution time = ' +
        str(statistics.mean(execution_times)/1000000)+'\n')

f.close()
