import csv
import numpy as np
import random
import pandas as pd



def get_Q(data):

	learning_rate = 0.01
	discount = 0.95
	
	num_states = np.array(data)[:,0].max()	
	num_actions = np.array(data)[:,1].max()

	Q = np.zeros((50001, num_actions+1))

	# get initial state
	i = 0
	for j in range(25):

		for sample in data.itertuples(index=True, name="Pandas"):
			
			if i % 10000 == 0:
				print("learning q: i, j, sample", i, j ,sample)

			s = sample.s
			a = sample.a
			r = sample.r
			s_p = sample.sp
				
			if r > 100000000:
				print ("BIG REWARD:", sample)
				print(i)
				print()
			# update Q #
			
			s_primes_row = Q[s_p,:]
			max_a_Q = max(s_primes_row)

			if r > 1000:
				print(s_primes_row)
				print(max_a_Q)
				print()

			
			Q[s,a] += learning_rate * ( r + discount * max_a_Q - Q[s,a] ) 

			i += 1 

	return Q

def get_policy(Q, data):
	
	policy = []

	print(len(Q))

	for stateNum in range(1, 50001):

		curState = Q[stateNum, :]
		action = np.argmax(curState)
       	
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


def propogate(Q, data):

	print("extracting positions with values assigned")

	vals = []
	pos_to_val = {}

	for s in range(1, 50001):
		for a in range(1,8):
			if Q[s,a] != 0.0:

				curPos = (s - 1) % 500
				vals.append(curPos)
				pos_to_val[curPos] = Q[s,a]

	
			
	print("done extracting positions with values assigned")
	print()
				
	
	
	
	for s in range(1, 50001):

		if s % 5000 == 0:
			print("nearest neighbor: ", s)

		for a in range(1,8):

			if Q[s,a] == 0.0:
				# we need to give it a value; let's find its nearest neighbor with a value

				curPos = (s - 1) % 500
				
				nearestValuedPos = find_nearest(vals, curPos)
				
				nearestQVal = pos_to_val[nearestValuedPos] 

				Q[s,a] = nearestQVal


				
		
				
		

def main():

	# unpack data
	filename = 'medium.csv'
	data = pd.read_csv(filename)

	Q = get_Q(data)
	
	propogate(Q, data)

	policy = get_policy(Q, data)

	#rewards = np.where(data[:, 2] > 100)	
	#print(rewards)
	#print(data[442])
	write_policy(policy, 'medium.policy')
		




if __name__ == '__main__':
    main()
