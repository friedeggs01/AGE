import numpy as np
# Example vector
v = np.array([3, 4])

# Computing the Euclidean norm (L2 norm)
euclidean_norm = np.linalg.norm(v)
print("Euclidean norm:", euclidean_norm)

# Computing the Manhattan norm (L1 norm)
manhattan_norm = np.linalg.norm(v, ord=1.457)
print("Manhattan norm:", manhattan_norm)