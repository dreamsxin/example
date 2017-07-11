# encodeing:utf-8
import pandas as pd
import numpy as np


dates = pd.date_range('20170101', periods=6)
df = pd.DataFrame(np.arange(24).reshape((6,4)),
                  index=dates,
                  columns=['A', 'B', 'C', 'D'])

df.iloc[0,1] = np.nan
df.iloc[1,2] = np.nan
print(df)
#是否丢失数据
print(df.isnull())
print('any null', np.any(df.isnull()) == True)

#将 nan 数据填入0
print(df.fillna(value=0))

#删除带nan数据的行
print(df.dropna(axis=0, how='any')) # how={'any', 'all'}

