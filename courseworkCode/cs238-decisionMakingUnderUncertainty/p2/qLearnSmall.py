

import csv
import numpy as np
import pandas as pd

def unpack_data(infile):

	arrayOfArrays = []
	i = 0
		
	with open(infile) as f:

		reader = csv.reader(f)

		for row in reader:
				
			if i == 0:
				rowArray = []
				for value in row:
					rowArray.append(value)
					columnNames = rowArray
			else:
				rowArray = []
				for value in row:
					rowArray.append(int(value))
				arrayOfArrays.append(rowArray)

			i += 1

	arrayOfArrays = np.array(arrayOfArrays)

	return (arrayOfArrays, columnNames)
		


def get_Q(data):

	learning_rate = 0.01
	discount = 0.95
	
	num_states = np.array(data)[:,0].max()	
	num_actions = np.array(data)[:,1].max()

	Q = np.zeros((50001, num_actions + 1))

	# get initial state
	
	for j in range(45):

		for sample in data.itertuples(index=True, name="Pandas"):

			s = sample.s
			a = sample.a
			r = sample.r
			s_p = sample.sp
		
			# update Q #
			max_a_Q = max(Q[s_p,:])
			Q[s,a] += learning_rate * ( r + discount * max_a_Q - Q[s,a] ) 
		
 
			# nearest neighbors after the fact, after you have a big and likely sparse Q
			#  /  sarsa lambda to propogate large reward

			# in medium, the terminal states are the ones with reward
			# that means that that's the one time you shouldn't just take the next value in the data, s_p
			# in that case,  you should actually just skip the whole max() thing, but do do the subtraction part
			# just skip the max part when reward is large

			# nearest neighbors, local approx, find something, in terms of the current state
			# interpolation
			#### pos : 0-499, v: 0-99: 
			# eq = 1+ pos + 500v

			# all states , subtract 1, get p = state % 500, then plug in to get velocity
			# so velocity always comes in multiples of 500
			# whereever v =0 ,it's the same velocity, you just dont know what velocity it is


			
		
	return Q	
	
	

def get_policy(Q, data):
	
	policy = []

	print(len(Q))
	print(Q)
	for stateNum in range(1, 50001):
		

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
	filename = 'medium.csv'
	data = pd.read_csv(filename)

	Q = get_Q(data)

	policy = get_policy(Q, data)
	
	write_policy(policy, 'medium.policy')


	

if __name__ == '__main__':
    main()
