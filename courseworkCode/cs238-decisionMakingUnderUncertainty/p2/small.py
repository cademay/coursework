
import csv
import numpy as np

def get_counts(data):

	num_states = data[:,0].max()	
	num_actions = data[:,1].max()

	Nsas = np.zeros((num_states, num_actions, num_states))
	Nsa = np.zeros((num_states, num_actions))

	for observation in data:
		
		s = observation[0] - 1
		a = observation[1] - 1
		s_p = observation[3] - 1

		Nsas[s][a][s_p] += 1
		Nsa[s][a] += 1
	
	return (Nsas, Nsa)	

def get_T(Nsas, Nsa, data):


	num_states = data[:,0].max()	
	num_actions = data[:,1].max()

	print (num_actions)
	T = np.zeros((num_states, num_states, num_actions))

	for s in range(num_states):
		for s_p in range(num_states):
			for a in range(num_actions):
				T[s_p][s][a] = float(Nsas[s][a][s_p]) / float(Nsa[s][a])

	return T
	
	
	

def estimate_transitions_and_rewards(data):

	num_states = data[:,0].max()	
	num_actions = data[:,1].max()

	Nsas, Nsa = get_counts(data)

	T = get_T(Nsas, Nsa, data) 
	print(T[0][1])



	R = np.zeros((num_states, num_actions))

	


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


def main():

	# unpack data
	filename = 'small.csv'
	data, columnNames = unpack_data(filename) 

	T, R = estimate_transitions_and_rewards(data)




if __name__ == '__main__':
    main()




