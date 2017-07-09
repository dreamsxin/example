from sklearn.feature_extraction import DictVectorizer
import csv
from sklearn import preprocessing
from sklearn import tree
from sklearn.externals.six import StringIO

allElectronicsData = open(r'./AllElectronics.csv', 'rb')
reader = csv.reader(allElectronicsData)
headers = reader.next()

print(headers)