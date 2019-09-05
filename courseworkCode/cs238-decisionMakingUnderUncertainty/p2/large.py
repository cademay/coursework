

import csv
import numpy as np
import pandas as pd


def get_Q(data):

	learning_rate = 0.01
	discount = 0.95
	
	num_states = np.array(data)[:,0].max()	
	num_actions = np.array(data)[:,1].max()

	Q = np.zeros((312021, num_actions + 1))

	# get initial state
	
	for j in range(150):

		for sample in data.itertuples(index=True, name="Pandas"):

			s = sample.s
			a = sample.a
			r = sample.r
			s_p = sample.sp
		
			# update Q #
			max_a_Q = max(Q[s_p,:])
			Q[s,a] += learning_rate * ( r + discount * max_a_Q - Q[s,a] ) 
		
 		
	return Q	
	

def get_policy(Q, data):
	
	policy = []

	print(len(Q))
	print(Q)
	for stateNum in range(1, 312021):
		

		curState = Q[stateNum, :]
		action = np.argmax(curState) 
	
		if action == 0:
			action = 4
		
		policy.append(action)
	
	return policy


		
def write_policy(policy, filename):
	
	with open(filename, 'w') as f:
		for action in policy:
			f.write("{}\n".format(action))

		

def main():

	# unpack data
	filename = 'large.csv'
	data = pd.read_csv(filename)

	Q = get_Q(data)

	policy = get_policy(Q, data)
	
	write_policy(policy, 'large.policy')


	

if __name__ == '__main__':
    main()
