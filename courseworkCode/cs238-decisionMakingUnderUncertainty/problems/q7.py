
import math
import numpy as np
import matplotlib.pyplot as plt


theta_set = [20, 40, 60, 80]

def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch

def Up(s, action, level):

	if level >= 4:
		return 0.0

	utilityOutput = 0.0

	theta = s
	distance = action
	Rsa = action * P_c1_d(distance, theta)

	sp_summation = 0.0
	for i in range(len(theta_set)):
		
		sp = theta_set[i]

		T = 1.0 if sp == s else 0.0

		o_summation = 0.0

		for o in range(2):
	
			P = P_c1_d(action, sp)

			if o == 1:
				# successful catch
				U_sub = Up(sp, action + 10, level + 1)
				o_summation += P * U_sub

			else:
				# no catch
				U_sub = Up(sp, action - 10, level + 1)
				o_summation += (1 - P) * U_sub

			
		sp_summation += T * o_summation
	
	return Rsa + sp_summation
	
	
def get_alpha_vec():

	possible_states_b = [20, 40, 60, 80]
	alpha_vec = [0.0] * 4
	root_action = 50

	for i in range(len(alpha_vec)):
		
		s = possible_states_b[i]
		
		alpha_vec[i] = Up(s, root_action, 0)

	return alpha_vec


alpha_vec = get_alpha_vec()  
print("alpha_vec: ", alpha_vec)
