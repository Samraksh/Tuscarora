import sys
from pylab import *
from collections import Counter
import matplotlib.pyplot as plt
import numpy as np
import math
import re
import time


def PlotMemUsage(x, y, legend, style):
     
  plt.xlabel('Elapsed Time (S)')
  plt.ylabel('Memory Used (KB)')
  plt.title('Memory Used by Simulation')
  grid('on')
  # figlegend(legend)
  plt.plot(x,y,style, label=legend)
  plt.legend(loc='upper left')
  #plt.show();
  #plt.close()

#Main
if len(sys.argv) < 2 :
  print "Provide atleast one file to plot\n"
  exit(-1)

#Open file and read contents as columns
lineCount=0;
fileName = sys.argv[1]
pid=[];elapsedTime=[]; mem=[];
with open(fileName,'r') as f:
  content = f.readlines()
  content = content[1:]
  for line in content:
    line = line.strip()
    cols = line.split()
    pid.append(float(cols[0]))
    time = cols[1].split(':')
    if(len(time) == 3):
      secs = int(time[0])*3600 + int(time[1])*60 + int(time[2]);
    else:
      secs = int(time[0])*60 + int(time[1]);
      
    elapsedTime.append(secs)
    mem.append(float(cols[2]))
    lineCount = lineCount +1;
    
print cols[0], cols[1], cols[2]
print "Numer of lines read : ", lineCount;

PlotMemUsage(elapsedTime, mem, 'Timer-Test', 'ro-');
#PlotMemUsage(x,2* np.array(y), 'test-2', 'g*-');

plt.show()