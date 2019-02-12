import numpy as np
a = np.random.random([5000,5000])
a = a + a.T
b = np.linalg.pinv(a)
print(np.amax(np.dot(a,b)))
