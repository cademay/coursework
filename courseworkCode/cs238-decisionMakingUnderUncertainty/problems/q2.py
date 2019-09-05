
import math
import numpy as np
import matplotlib.pyplot as plt

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch

def get_U(distance):

	outputU = 0.0
	# Initially, Johnny has a uniform distribution over θ.
	# I believe that means assigning equal probabilities to every discrete value in the set you mentioned
	# It is equal probabilities to every discrete value in the set.
	theta_set = [20, 40, 60, 80]
	b = [.25, .25, .25, .25]

	reward = distance
	
	for i in range(len(theta_set)):
		
		theta = theta_set[i]
		b_s = b[i]

		prob= P_c1_d(distance, theta)

		outputU += reward * prob * b_s

	return outputU
		

		

	


# Johnny chooses a distance from a discrete set A = {10, 20, . . . , 100} #
A = np.arange(10, 110, 10)

utilities = [get_U(d) for d in A]

plt.plot(A, utilities)

plt.xlabel('actions (distance)')
plt.ylabel('utility')
plt.title('CS238 Midterm 3: Question 2 Plot')


plt.show()
