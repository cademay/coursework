

import math
import numpy as np
import matplotlib.pyplot as plt

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch

def create_plot():

	# θ belongs to a discrete set Θ = {20, 40, 60, 80} #
	theta_set = [20, 40, 60, 80]

	# horizontal axis is d from 0 to 100 #
	D = np.arange(101)

	# different plot lines for each θ ∈ Θ #
	for theta in theta_set:

		# the vertical axis is P(c1|d)  
		catch_probabilities = [P_c1_d(d, theta) for d in D]
		
		# Plot y versus x with label
		legendLabel = "Θ = %d" % theta
		plt.plot(D, catch_probabilities, label=legendLabel)

	# set plot attributes
	plt.xlabel('Distance')
	plt.ylabel('P(catch)')
	plt.title('CS238 Midterm 3: Question 1 Plot')
	plt.legend(loc='upper right')

	# display plot
	plt.show()

create_plot()
