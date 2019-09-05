
import math
import numpy as np
import matplotlib.pyplot as plt

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + np.exp(  -1 * ( (d - float(theta))/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch


def update_belief(a, o, b):
	
	b_prime = [0.0, 0.0, 0.0, 0.0]
	theta_set = [20, 40, 60, 80]
	#bp(sp) = O(o | sp  , a) * sum_s [T (sp  | s, a) * b(s)]
	# bp(sp) = o * sum_s [ T * b]


	for i in range(len(theta_set)):
		sp = theta_set[i]

		pcatch = P_c1_d(a, sp) 
		O = pcatch if o == 1 else 1 - pcatch
		
		summation = 0.0

		for j in range(len(theta_set)):
			s = theta_set[j]

			T = 1.0 if s == sp else 0.0

			bs = b[j]

			summation += T * bs


		b_prime[i] = O * summation
			
	
	
	# normalize
		
	b_prime_sum = sum(b_prime)
	inv_sum = 1/b_prime_sum
	
	for i in range(len(b_prime)):
		b_prime[i] *= inv_sum
	
	return b_prime


# starting with a uniform prior
belief = [0.25, 0.25, 0.25, 0.25]

# observe four successful catches with a = 100
a = 100
o = 1

for i in range(4):
	belief = update_belief(a, o, belief)

print("updated belief Q5: ", belief)


