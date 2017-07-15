from numpy import genfromtxt
import numpy as np
from sklearn import datasets, linear_model

import os

data_path = os.path.join(os.path.dirname(__file__), "DeliveryDummy.csv")
deliveryData = genfromtxt(data_path, delimiter=',')

print("data: \n", deliveryData)

X = deliveryData[:, :-1]
Y = deliveryData[:, -1]

print("X: \n", X)
print("Y: \n", Y)

regr = linear_model.LinearRegression()

regr.fit(X, Y)

print("coefficients: \n")
print(regr.coef_)
print("intercept: \n")
print(regr.intercept_)

xPred = [102, 6, 1, 0, 0]
yPred = regr.predict([xPred])
print("predicted Y: ", yPred)