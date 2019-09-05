
import math
import numpy as np
import matplotlib.pyplot as plt


def P_c1_d(d, theta):

	# calculate P(catch|d) = 1 − [ 1 / (1 + exp(− d− θ /15 )) ]
	denom = 1 + math.exp(  -1 * ( (d - theta)/15 )  )
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

def get_Rba(b, a):
	
	Rba = 0.0

	theta_set = [20, 40, 60, 80]

	for i in range(len(b)):
	
		s = theta_set[i]
		
		Rsa = a * P_c1_d(a, s)	
		bs = b[i]
		
		Rba += Rsa * bs

	return Rba


def selectAction(b, d):

	if d == 0:
		return (None, 0)

	y = 1.0
	a_star = None
	u_star = float("-inf")

	A = np.arange(10, 110, 10)

	for a in A:
		
		u = get_Rba(b, a)

		for o in range(2):
			
			b_prime = update_belief(a,o,b)
			
			a_prime, u_prime = selectAction(b_prime, d - 1)

			u = u + y * get_Poba(o, b, a) * u_prime

		if u > u_star:
			a_star = a
			u_star = u


	return (a_star, u_star)
			
			
				

# assuming a uniform prior over 
b = [0.25, 0.25, 0.25, 0.25]
d = 4

action = selectAction(b, d)

print(action)
