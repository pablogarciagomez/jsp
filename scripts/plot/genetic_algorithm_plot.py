import sys
import re
import matplotlib.pyplot as plt

f = open(sys.argv[1], 'r')
trace = f.read()
average = list(map(float, re.findall('Average quality = (.*)', trace)))
maximum = list(map(float, re.findall('Maximum quality = (.*)', trace)))
plt.plot(average, label='average')
plt.plot(maximum, label='maximum')
plt.title('Fitness evolution')
plt.xlabel('Generations')
plt.ylabel('Fitness')
plt.legend(loc='best')
plt.tight_layout()
plt.savefig('fitness.png')
