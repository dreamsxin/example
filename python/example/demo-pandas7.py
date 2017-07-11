import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# plot data

# bar, hist, box, kde, area, scatter, hexbin, pie

# Series
data = pd.Series(np.random.randn(100), index=np.arange(100))
data = data.cumsum()


data.plot() #plt.plot(x=,y=)
plt.show()

# DataFrame
data = pd.DataFrame(np.random.randn(1000,4),
                    index=np.arange(1000),
                    columns=list('ABCD'))

data = data.cumsum()

print(data.head())
data.plot()
plt.show()


ax = data.plot.scatter(x='A',y='B',color='DarkBlue', label='ClassOne')
data.plot.scatter(x='A',y='C',color='DarkGreen', label='ClassTwo',ax=ax)
plt.show()


