
import math
import numpy as np
import matplotlib.pyplot as plt
from q7 import get_alpha_vec

a_vec = get_alpha_vec()

print("a-vec in Q8:", a_vec)

# 8a
b_theta_is_80 = [0.0, 0.0, 0.0, 1.0]

q8a_result = np.dot(a_vec, b_theta_is_80)

print("theta=80 utility: ", q8a_result)


# 8b
b_uniform = [0.25, 0.25, 0.25, 0.25]

q8a_result = np.dot(a_vec, b_uniform)

print("uniform utility: ", q8a_result) 
