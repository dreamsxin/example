import numpy as np

def fitSLR(x, y):
	n = len(x)
	dinominator = 0
	numerator = 0
	x_mean = np.mean(x)
	y_mean = np.mean(y)
	for i in range(0, n):
		numerator += (x[i] - x_mean) * (y[i] - y_mean)
		dinominator += (x[i] - x_mean) ** 2

	print("numerator: ", numerator)
	print("dinominator: ", dinominator)
	b1 = numerator / float(dinominator)
	b0 = np.mean(y) - b1 * np.mean(x)

	print("b0: ", b0)
	print("b1: ", b1)

	return b0, b1

def predict(x, b0, b1):
	return b0 + x * b1

x = [1, 3, 2, 1, 3]
y = [14, 24, 18, 17, 27]

if __name__ == '__main__':
	b0, b1 = fitSLR(x, y)
	print(predict(6, b0, b1))