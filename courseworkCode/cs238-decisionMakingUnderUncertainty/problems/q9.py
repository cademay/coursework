
import math
import numpy as np
import matplotlib.pyplot as plt
from q7 import get_alpha_vec


def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
	p_catch = 1.0 - (1 / denom)
	return p_catch

def get_U(distance, b):

	outputU = 0.0
	# Initially, Johnny has a uniform distribution over θ.
	# I believe that means assigning equal probabilities to every discrete value in the set you mentioned
	# It is equal probabilities to every discrete value in the set.
	theta_set = [20, 40, 60, 80]

	reward = distance
	
	for i in range(len(theta_set)):
		
		theta = theta_set[i]
		b_s = b[i]

		prob = P_c1_d(distance, theta)

		outputU += reward * prob * b_s

	return outputU


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



def get_Rba(b, a):
	
	Rba = 0.0

	theta_set = [20, 40, 60, 80]

	for i in range(len(b)):
	
		s = theta_set[i]
		
		Rsa = a * P_c1_d(a, s)	
		bs = b[i]
		
		Rba += Rsa * bs

	return Rba

def get_Poba(o, b, a):

	output_Poba = 0.0

	theta_set = [20, 40, 60, 80]
	
	nStates = len(b)

	for i in range(nStates):
		s = theta_set[i]
	
		bs = b[i]

		sp_summation = 0.0

		for j in range(nStates):
			sp = theta_set[j]

			pcatch = P_c1_d(a, sp)
			Oosa = pcatch if o == 1 else 1 - pcatch

			T = 1.0 if sp == s else 0.0

			sp_summation += Oosa * T
		
		output_Poba += bs * sp_summation
			
	return output_Poba
			

def sum_over_o(b, a, a_vec):

	output = 0.0	
	for o in range(2):

		Poba = get_Poba(o, b, a)

		updated_belief = update_belief(a,o,b)
		U = np.dot(a_vec, updated_belief)
		
		output += Poba * U

	return output
	
def get_value(a, b, a_vec):

	y = 1.0

	Rba =  get_Rba(b, a)

	o_summation = sum_over_o(b, a, a_vec)

	return Rba + y * o_summation

	
	


def get_pi(b):

	# prepare to take argmax over actions
	actions = np.arange(10, 110, 10)
	values =  []


	a_vec = get_alpha_vec()

	for i in range(len(actions)):
		
		a = actions[i]
		value = get_value(a, b, a_vec)

		tup = (a, value)
		values.append(tup)

	print(values)
	sorted_values = sorted(values, key=lambda x: x[1])

	arg_max = sorted_values[len(sorted_values) - 1]

	return arg_max

 
b = [0.5, 0.5, 0.0, 0.0]
pi_b = get_pi(b)

print("argmax q9: ", pi_b)
