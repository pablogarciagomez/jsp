import sys
import re
import matplotlib.pyplot as plt

f = open(sys.argv[1], 'r')
trace = f.read()
quality = list(map(float, re.findall('Quality = (.*)', trace)))
plt.plot(quality, label='quality')
plt.title('Quality evolution')
plt.xlabel('Iterations')
plt.ylabel('Quality')
plt.legend(loc='best')
plt.tight_layout()
plt.savefig('quality.png')
