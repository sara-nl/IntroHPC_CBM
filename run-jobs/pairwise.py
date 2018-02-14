#!/usr/bin/env python

import sys
import numpy as np


def pairwise_python(X):
    M = X.shape[0]
    N = X.shape[1]
    D = np.empty((M, M), dtype=np.float)
    for i in range(M):
        for j in range(M):
            d = 0.0
            for k in range(N):
                tmp = X[i, k] - X[j, k]
                d += tmp * tmp
            D[i, j] = np.sqrt(d)
    return D


if __name__ == "__main__":

   dim=int(sys.argv[1])
   X = np.random.random((dim, 3))

   res = pairwise_python(X)

   print res

