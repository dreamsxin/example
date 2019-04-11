import os
import pandas as pd
import requests

PATH = os.path.abspath(os.path.dirname(__file__)) + r'/'

if not os.path.isfile(PATH + 'iris.data'):
	r= requests.get('https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data')

	with open(PATH + 'iris.data', 'w') as f:
		f.write(r.text)

os.chdir(PATH)

df = pd.read_csv(PATH +  'iris.data',  names=['sepal length', 'sepal width', 'petal length', 'petal width', 'class'])

print('显示count')
print(df.count())

print('显示一行')
print(df.head(1))

print('显示一列')
print(df['class'])

print('显示列唯一')
print(df['class'].unique())

print('显示前四行与前两列')
print(df.ix[:3, :2])

print('显示clss匹配Iris-virginica')
print(df[df['class']=='Iris-virginica'])

print('显示count')
print(df[df['class']=='Iris-virginica'].count())