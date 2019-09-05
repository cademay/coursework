

import math
import numpy as np
import matplotlib.pyplot as plt

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch

theta_set = [20, 40, 60, 80]

d = 80

for theta in theta_set:

	p = P_c1_d(d, theta)

	print("theta: ", theta)
	print("catch probability: ", p)
	print()

