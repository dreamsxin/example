import numpy as np
from astropy.units import Ybarn
import math

def computeCorrelation(X, Y):
	xBar = np.mean(X)
	yBar = np.mean(Y)
	SSR = 0
	varX = 0
	varY = 0
	for i in range(0, len(X)):
		diffXBar = X[i] - xBar
		diffYBar = Y[i] - yBar
		SSR += (diffXBar * diffYBar)
		varX += diffXBar ** 2
		varY += diffYBar ** 2

	SST = math.sqrt(varX * varY)
	return SSR / SST

# Polynomial Regression
def polyfit(x, y, degree):
	results = {}

	coeffs = np.polyfit(x, y, degree) # degree is the highest pow

	print("coeffs: ", coeffs)

	# Polynomial Coefficients
	results['polynomial'] = coeffs.tolist()

	# r-squared
	p = np.poly1d(coeffs)
	print("p: ", p)
	# fit values, and mean
	yhat = p(x) # or [p[z] for z in x]
	print("y_hat: ", yhat)
	ybar = np.sum(y) / len(y) # or sum(y) / len(y)
	ssreg = np.sum((yhat - ybar) ** 2) # or sum([ (yihat - ybar) ** 2 for yihat in yhat])
	sstot = np.sum((y - ybar) ** 2) # or sum([(yi - ybar) ** 2 for yi in y])
	results['determination'] = ssreg / sstot

	return results

testX = [1, 3, 8, 7, 9]
testY = [10, 12, 24, 21, 34]

print("r: ", computeCorrelation(testX, testY))
print("r^2: ", computeCorrelation(testX, testY) ** 2)

print(polyfit(testX, testY, 1)["determination"])