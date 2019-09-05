import csv
import numpy as np
import random
import pandas as pd


nStates = 50001
nActions = 8

def sarsaLambda(data):

	learning_rate = 0.01
	discount = 0.95
	lam = .7
	
	n = len(data)

	num_states = np.array(data)[:,0].max()	
	num_actions = np.array(data)[:,1].max()

	Q = np.zeros((nStates, num_actions+1))
	N = np.zeros((nStates, num_actions+1))

	# get initial state
	for j in range(4):

		i = 0

		for sample in data.itertuples(index=True, name="Pandas"):

			if i % 2500 == 0:
				print(sample)	
			#	print(len(Q))
			#	print(len(Q[0]))
			if sample.Index < n - 1:
				
				s = sample.s
				a = sample.a
				r = sample.r
				s_p = sample.sp
	
				sample_prime = data.iloc[sample.Index+1]
				
				# if reward is big, then don't use the next datapoint to get a_p because it's random	
				a_p = sample_prime.a if r < 90000 else 1

				# N (st , at ) ← N (st , at ) + 1
				N[s,a] += 1

				# δ ← rt + γQ (st +1, at +1) − Q (st , at )
				oink = r + discount * discount * Q[s_p,a_p] - Q[s,a]
				
				for s in range(1, nStates):
					for a in range(1,nActions):
						
						# Q (s, a) ← Q (s, a) + αδN (s, a)
						Q[s,a] += learning_rate * oink * N[s,a]
		
						# N (s, a) ← γ λN (s, a)
						N[s,a] *= discount * lam
						
				if r > 1000: 
					
					N = np.zeros((nStates, nActions))

			i += 1 

	return Q

def get_policy(Q, data):
	
	policy = []

	print(len(Q))

	for stateNum in range(1, nStates):

		curState = Q[stateNum, :]
		action = np.argmax(curState[1:])
       	
		if stateNum % 2500 == 0:
			print(curState)
			print(action)
 
		if action == 0:
			action = 1

		policy.append(action)

	
	return policy



	

def write_policy(policy, filename):

	with open(filename, 'w') as f:
		for action in policy:
			f.write("{}\n".format(action))



def find_nearest(array, value):
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return array[idx]


				
		
				
	
def main():

	# unpack data
	filename = 'medium.csv'
	data = pd.read_csv(filename)

	Q = sarsaLambda(data)
	
	policy = get_policy(Q, data)

	#rewards = np.where(data[:, 2] > 100)	
	#print(rewards)
	#print(data[442])
	write_policy(policy, 'medium.policy')
		




if __name__ == '__main__':
    main()
