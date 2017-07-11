import pandas as pd
import numpy as np


dates = pd.date_range('20170101', periods=6)
df = pd.DataFrame(np.arange(24).reshape((6,4)),
                  index=dates,
                  columns=['A', 'B', 'C', 'D'])
print(df)
# 取某列
print(df['A'])
print(df.A)
# 切片 0-3行
print(df[0:3])
# 取列
print(df['20170101':'20170102'])

# select by label:loc
print(df.loc['20170101'])
print(df.loc[:, ['A', 'B']])
print(df.loc['20170101', ['A', 'B']])

# select by position: iloc
print('iloc')
print(df.iloc[3])

print(df.iloc[3,1])
print(df.iloc[3:5,1:3])
print(df.iloc[[1,3,5],1:3])

