from sklearn.datasets import load_digits

digits = load_digits()
print(digits.data.shape)

import pylab as pl
pl.gray()
pl.matshow(digits.images[0])
pl.show()