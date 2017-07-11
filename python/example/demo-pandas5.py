import pandas as pd
import numpy as np

# csv excel hdf sql json msgpack html gbq stata sas clipboard pickle
data = pd.read_csv('students.csv')
print(data)

data.to_pickle('students.pickle')

# concatenating
df1 = pd.DataFrame(np.zeros((3,4)),columns=['a','b','c','d'])
print(df1)

df2 = pd.DataFrame(np.ones((3,4))*1,columns=['a','b','c','d'])
print(df2)

df3 = pd.DataFrame(np.ones((3,4))*2,columns=['a','b','c','d'])
print(df3)

# 上下合并
res = pd.concat([df1,df2,df3], axis=0)
print(res)
# 对index重新排序
res = pd.concat([df1,df2,df3], axis=0, ignore_index=True)
print(res)

# join, ['inner', 'outer']
df1 = pd.DataFrame(np.zeros((3,4)),columns=['a','b','c','d'], index=[1,2,3])
print(df1)

df2 = pd.DataFrame(np.ones((3,4))*1,columns=['b','c','d','e'], index=[2,3,4])
print(df2)

# outer 并集
res = pd.concat([df1,df2], ignore_index=True)
print(res)
# inner 交集
res = pd.concat([df1,df2], join='inner', ignore_index=True)
print(res)

# join_axes
df1 = pd.DataFrame(np.zeros((3,4)),columns=['a','b','c','d'], index=[1,2,3])
df2 = pd.DataFrame(np.ones((3,4))*1,columns=['b','c','d','e'], index=[2,3,4])
res = pd.concat([df1,df2],axis=1)
print(res)
#按照df1的index合并，忽略df2
res = pd.concat([df1,df2],axis=1,join_axes=[df1.index])
print(res)

# append
df1 = pd.DataFrame(np.zeros((3,4)),columns=['a','b','c','d'])
print(df1)

df2 = pd.DataFrame(np.ones((3,4))*1,columns=['a','b','c','d'])
print(df2)

df3 = pd.DataFrame(np.ones((3,4))*2,columns=['a','b','c','d'], index=[2,3,4])
print(df3)
res = df1.append(df2, ignore_index=True)
print('append', res)

res = df1.append([df2,df3])
print('append2', res)

s1 = pd.Series([1,2,3,4],index=['a','b','c','d'])
res = df1.append(s1, ignore_index=True)
print(res)
