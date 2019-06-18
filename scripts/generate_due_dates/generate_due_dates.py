import glob
import os

factors = [1.3, 1.5, 1.6]

for filename in glob.iglob('./**/*.txt', recursive=True):
    lines = [line.rstrip('\n') for line in open(filename)]
    n, m = lines[0].split('\t')
    weights = [4]*int(int(n)*0.2) + [2]*int(int(n)*0.6) + [1]*int(int(n)*0.2)
    for factor in factors:
        f = open(filename[:-4]+'_' +
                 str(factor).replace('.', '')+filename[-4:], 'w+')
        f.write(n+'\t'+m+'\n')
        for l, w in zip(lines[1:], weights):
            due_date = int(sum(list(map(int, l.split('\t')))[1::2])*factor)
            f.write(str(due_date)+'\t'+str(w)+'\t'+l+'\n')
        f.close()
    os.remove(filename)
