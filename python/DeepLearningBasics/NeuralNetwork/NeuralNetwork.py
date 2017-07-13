# coding:utf-8
import numpy as np

# 双曲函数
def tanh(x):
	return np.tanh(x)

## 双曲函数导数
def tanh_deriv(x):
	return 1.0 - np.tanh(x)*np.tanh(x)

# 逻辑函数
def logistic(x):
	return 1 / (1 + np.exp(-x))

# 逻辑函数导数
def logistic_derivative(x):
	return logistic(x) * (1 - logistic(x))


class NeuralNetwork:
	def __init__(self, layers, activation='tanh'):
		"""
		:param layers: A list containing the number of units in each layer. 
		Should be at least two values.
		:param activation: The activation function to be used. Can be 
		"logistic" and "tanh".
		"""
		if activation == 'logistic':
			self.activation = logistic
			self.activation_deriv = logistic_derivative
		elif activation == 'tanh':
			self.activation = tanh
			self.activation_deriv = tanh_deriv

		self.weights = []
		# 排除第一层输入层0,
		for i in range(1, len(layers) - 1):
			self.weights.append((2 * np.random.random((layers[i-1] + 1, layers[i] + 1)) - 1) * 0.25)
			self.weights.append((2 * np.random.random((layers[i] + 1, layers[i+1])) - 1) * 0.25)

	def fit(self, X, y, learning_rate=0.2, epochs=10000):
		"""
		:param X: Dataset instance
		:param y: Class label
		:param learning_rate: Length of per step
		:param epochs: The number of iteration
		"""
		# 保证至少二维
		X = np.atleast_2d(X)
		# 构造一个行数跟 X 一致，列数多一列用来存储 bais 的矩阵
		temp = np.ones([X.shape[0], X.shape[1] + 1])
		# 列数取第一列到倒数一列
		temp[:, 0:-1] = X # adding the bias unit to the input layer
		X = temp
		y = np.array(y)

		for k in range(epochs):
			# 随机抽取一行
			i = np.random.randint(X.shape[0])
			a = [X[i]]

			for l in range(len(self.weights)): # Going forward network, for each layer
				a.append(self.activation(np.dot(a[l], self.weights[l]))) # Compute the node value

			error = y[i] - a[-1] # Compute the error at the top layer
			deltas = [error * self.activation_deriv(a[-1])] # For output layer

			# Starting backpropagation
			for l in range(len(a) - 2, 0, -1): # we need to begin at the second to last layer
				# compute the updated error(i.e. deltas) for each node going from top layer
				deltas.append(deltas[-1].dot(self.weights[l].T)*self.activation_deriv(a[l]))
				deltas.reverse()
				for i in range(len(self.weights)):
					layer = np.atleast_2d(a[i])
					delta = np.atleast_2d(deltas[i])
					# update the weights
					self.weights[i] += learning_rate * layer.T.dot(delta)

	def predict(self, x):
		x = np.array(x)
		temp = np.ones(x.shape[0] + 1)
		temp[0 : -1] = x
		a = temp
		for l in range(0, len(self.weights)):
			a = self.activation(np.dot(a, self.weights[l]))
		return a