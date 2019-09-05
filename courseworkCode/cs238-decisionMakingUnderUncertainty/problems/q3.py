
import math
import numpy as np
import matplotlib.pyplot as plt

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch


# Johnny chooses a distance from a discrete set A = {10, 20, . . . , 100} #
A = np.arange(10, 110, 10)

# Johnny is certain that θ = 20
theta = 20

U = [d * P_c1_d(d, theta) for d in A]

plt.plot(A, U)


plt.xlabel('actions (distance)')
plt.ylabel('utility')
plt.title('CS238 Midterm 3: Question 3 Plot')


plt.show()
