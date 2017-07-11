import numpy as np

A = np.arange(3,15)
print(A)
print(A[3])

A = np.arange(3,15).reshape((3,4));
print(A)
print(A[2])
print(A[1][1])
print(A[1,1])
print("A[1,:]", A[1,:])
print("A[1,1:3]", A[1,1:3])
print("A[:,1]", A[:,1])

for row in A:
    print("row", row)

for col in A.T:
    print("col", col)

print("flatten", A.flatten())
for item in A.flat:
    print("item", item)
