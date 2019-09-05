import sys

import networkx as nx
import csv
import numpy as np

import random
from operator import itemgetter  
import collections
import scipy
var_maxes = {}
maxK_global = 0
import random
q = {}

import copy
def get_max_j(data):

	output = 1
	for i in range(len(data[0])):
		varMax = data[:,i].max()
		output *= varMax

	return output

def get_max_k(data):
	global maxK_global 	
	maxK = data.flatten().max()
	maxK_global = maxK
	return maxK

def get_m_ij0(m, i, j, ri):

	out = 0.0
	for k in range(0, ri):
		out += m[i][j][k] if i in m and j in m[i] and k in m[i][j] else 0.0 
	return out	

def get_mkey(i, j, k ):
	mkey = str(i) + "_" + str(j) + "_" + str(k)
	return mkey

def init_m(data, nodeList, graph):

	m = {i:{} for i in range(len(nodeList))}
	print (m)
	maxNumber = get_max_j(data)
	maxK = get_max_k(data)
	"""
	for node in m:
		
		m[node] = {j:{} for j in range(maxNumber + 1)}

		for jindex in m[node]:
			m[node][jindex] = {k:0 for k in range(maxK + 1)}
	"""
	# first graph
	j = 1

	for sample in data:

		for i in range(len(data[0])):
		
			k = sample[i]

			if j in m[i]:

				if k in m[i][j]:
					m[i][j][k] += 1

				else:
					m[i][j] = {k: 1}

			else:
				m[i].update({j:{k:0 for k in range(var_maxes[i] + 1)}})
				m[i][j][k] += 1


	print("int",m)
	print("")
	return m

def get_j_from_parents(nodeName, nodeList, m, graph, sample, i):
	parents = graph.predecessors(nodeName)
	inds = []
	vals = []

	for parent in parents:

		parentIndex = nodeList.index(parent)

		parentValue = sample[parentIndex]
		
		inds.append(parentIndex)
		vals.append(parentValue)	

	vals = tuple(vals)
	maxes = tuple([var_maxes[x] + 1 for x in inds])

	if vals:
		j = np.ravel_multi_index(vals, maxes)	
		
	else:
		j = 1

	k = sample[nodeList.index(nodeName)]

	if j in m[i]:

		if k in m[i][j]:
			m[i][j][k] += 1

		else:
			m[i][j] = {k: 1}

	else:
		m[i].update({j:{k:0 for k in range(var_maxes[i] + 1)}})
		m[i][j][k] += 1

	return m

def update_m(graph, data, nodeList, m, curChildProspect):
		
	i = nodeList.index(curChildProspect)	
	m[i] = {}
	for sample in data:

		parents = graph.predecessors(curChildProspect)
		inds = []
		vals = []

		# get j
		for parent in parents:

			parentIndex = nodeList.index(parent)

			parentValue = sample[parentIndex]
			
			inds.append(parentIndex)
			vals.append(parentValue)	

		vals = tuple(vals)
		maxes = tuple([var_maxes[x] + 1 for x in inds])

		if vals:
			j = np.ravel_multi_index(vals, maxes)	
		else:
			j = 1

		k = sample[i]

		# update 
		if j in m[i]:

			if k in m[i][j]:
				m[i][j][k] += 1

			else:
				m[i][j] = {k: 1}

		else:
			m[i].update({j:{k:0 for k in range(var_maxes[i] + 1)}})
			m[i][j][k] += 1

	
	return m


def scoreGraph(data, nodeList, m, curChildProspect):
	
	output = 0.0

	for i in range(len(m)):	

		nodeName = nodeList[i]

		for j in m[i]:
		
			ri = var_maxes[i] 
			a_ij0 = ri
			
			m_ij0 = 0

			for k in m[i][j]:
				m_ij0 += m[i][j][k]

			gam1 = scipy.special.loggamma(a_ij0)
			gam2 = scipy.special.loggamma(a_ij0 + m_ij0)

			part1 = gam1 - gam2

			output += part1
		
			part2 = 0.0
			for k in range(len(m[i][j])):	
				
				inp3 = 1 + m[i][j][k]

				gam3 = scipy.special.loggamma(inp3)	
				part2+=gam3

			output += part2 

	return output


def createEmptyGraph(nodeList):

	graph = nx.DiGraph()
	
	for node in nodeList:
		graph.add_node(node)

	return graph

def K2(data, nodeList, outfile):
	
	G = createEmptyGraph(nodeList)
	#Gprime = createEmptyGraph(nodeList)

	m = init_m(data, nodeList, G)

	#G = nx.DiGraph()
	#Gprime = nx.DiGraph()
	gscore = scoreGraph(data, nodeList, m, None)
	
	# for each node
	for i in range(len(nodeList)):

		curChildProspect = nodeList[i]
	
		# greedily add parents to this node
		while True:

			gprime = copy.deepcopy(G)
			mprime = copy.deepcopy(m)
			####### get best prospective parent of curChild ######

			# for each node prospect
			for j in range(len(nodeList)):
				curParentProspect = nodeList[j]
				#print(curParentProspect)

				if curParentProspect != curChildProspect:
						
					# create new graph
					gtest = copy.deepcopy(G)
					mtest = copy.deepcopy(m)

					gtest.add_edge(curParentProspect, curChildProspect)
					
					parentals = gtest.predecessors(curChildProspect)
					parents = []
					for p in parentals:
						parents.append(p)
					if len(parents) > 4:
						continue
					mtest = update_m(gtest, data, nodeList, mtest , curChildProspect)

					try:	
						nx.find_cycle(gtest)
						continue	
					except:
						pass
					
					
					
					Testscore = scoreGraph(data, nodeList, mtest, curChildProspect)
					primeScore = scoreGraph(data, nodeList, mprime, curChildProspect)
						
					#print("test", Testscore)
					#print("prime", primeScore)
					#print()	

					if Testscore > primeScore:
						gprime = gtest
						mprime = mtest


			print("-------------------------------------------------")

			gprimeScore = scoreGraph(data, nodeList, mprime, curChildProspect)
			
			print(gscore)	
			print(gprimeScore)
			print()	
			if gprimeScore <= gscore:
				break
			else:
				print("else")
				G = gprime
				m = mprime
				gscore = scoreGraph(data, nodeList, m, None)

			# if GprimeScore <== Gscore
	write_gph2(G, outfile)

	import matplotlib.pyplot as plt
	nx.draw(G)
	plt.show()
	print(G.edges())
	print("")

def parseInput(infile):


	print (infile)
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

	for i in range(len(arrayOfArrays[0])):
		varMax = arrayOfArrays[:,i].max()
		var_maxes[i] = varMax

	print (var_maxes)
	print(arrayOfArrays[:, 0])
	return (arrayOfArrays, columnNames)


def write_gph2(dag, filename):
	with open(filename, 'w') as f:
		for edge in dag.edges():
			f.write("{}, {}\n".format(edge[0], edge[1]))

def write_gph(dag, idx2names, filename):
	with open(filename, 'w') as f:
		for edge in dag.edges():
			f.write("{}, {}\n".format(idx2names[edge[0]], idx2names[edge[1]]))


def compute(infile, outfile):
	data, columnNames = parseInput(infile)
	graphMap = K2(data, columnNames, outfile)

def main():
    if len(sys.argv) != 3:
        raise Exception("usage: python project1.py <infile>.csv <outfile>.gph")

    inputfilename = sys.argv[1]
    outputfilename = sys.argv[2]
    compute(inputfilename, outputfilename)


if __name__ == '__main__':
    main()






















