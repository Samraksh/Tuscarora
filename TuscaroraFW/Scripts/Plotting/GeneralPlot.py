import numpy as np
import matplotlib.pyplot as plt

x=[1,2,3,4,5,6,7,8,9,10]
y=1/np.sqrt(x)
plt.plot(x,y)
plt.ylabel('Sqrt')
plt.show()