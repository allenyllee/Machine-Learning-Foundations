#!/usr/bin/python

import numpy as np

# open a file
#fo = open("hw1_15_train.dat", "r+")
#str = fo.read()

from io import StringIO

#c = StringIO(u"0 1\n2 3")
#arrar1 = np.loadtxt(c)

#print arrar1

#d = StringIO(u"M 21 72\nF 35 58")
#array2 = np.loadtxt(d, dtype={'names': ('gender', 'age', 'weight'),
#                              'formats': ('S1', 'i4', 'f4')})

#print array2

#c = StringIO("1,0,2\n3,0,4")

#x, y = np.loadtxt(c, delimiter=',', usecols=(0, 2), unpack=True)

#fo = open("hw1_15_train.dat", "r+")

#x[0:3], y = np.loadtxt(fo, unpack=True)

#print x
#print y


#with open('hw1_15_train.dat') as f:
#    lines = f.readlines()

lines = [line.rstrip('\n') for line in open('hw1_15_train.dat')]

for x in range(0,9): print lines[x] 

print '\n\n'

print '\n'.join(str(lines[x]) for x in range(0,9))

print '\n\n'

print '\n'.join(str(line) for line in lines[0:10])

print '\n' + lines[0] + '\n'

#d = StringIO(lines[0].rstrip(' '))
#x[0:3], y = np.loadtxt(d, unpack=False)

print "\n".join(lines[0].split()[i] for i in range(0,5))

x = [lines[i].split()[0:4] for i in range(0,10)]
y = [lines[i].split()[4] for i in range(0,10)]
print x[2]
print y[2]
#x, y = lines[0][0:]
#x[0:3], y = lines[0]1
#print lines[0]'\n'
#print x
#print y

