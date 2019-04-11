import os    
import pandas as pd    
import requests    

PATH = os.path.abspath(os.path.dirname(__file__)) + r'/' 

r= requests.get('https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data')    

with open(PATH + 'iris.data', 'w') as f:    
	f.write(r.text)    

os.chdir(PATH)    

df = pd.read_csv(PATH +  'iris.data',  names=['sepal length', 'sepal width', 'petal length', 'petal width', 'class']) 

df.head()