from sklearn import datasets
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt

loaded_data = datasets.load_boston()
data_X = loaded_data.data
data_Y = loaded_data.target

model = LinearRegression()
# 输出构建model参数
print(model.get_params())
model.fit(data_X, data_Y)
print(model.predict(data_X[:4, :]))

# y = 0.1x + 0.3
# 输出x系数0.1
print(model.coef_)
# 输出常数0.3
print(model.intercept_)
# 对比真实数据和预测数据，正确率打分 R^2 coefficient of determination
print(model.score(data_X, data_Y))


