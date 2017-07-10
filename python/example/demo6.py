class Calculator:
    name = None
    price = 10
    def __init__(self, name='Good calculator', price=10, hight=50,wight=100, weight=20):
        self.name = name
        self.price = price
        self.hight = hight
        self.wight = wight
        self.weight = weight
        self.add(0,1)

    def add(self, x, y):
        print self.name
        print 'add:', x + y

    def minus(self, x, y):
        print 'minus:', x - y

    def times(self, x, y):
        print('times:', x * y)

    def divide(self, x, y):
        print('divide:', x/y)

cal = Calculator()
cal.add(1, 2)
cal.minus(1,2)
cal.times(1,2)
cal.divide(1,2)
print 'name:', cal.name
print 'price:', cal.price
print 'hight:', cal.hight

cal = Calculator('New calculator')
print 'name:', cal.name
