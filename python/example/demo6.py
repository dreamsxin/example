class Calculator:
    name = 'Good calculator'
    price = 10
    def add(self, x, y):
        print self.name
        print x + y
    def minus(self, x, y):
        print x - y
    def times(self, x, y):
        print(x * y)
    def divide(self, x, y):
        print(x/y)

cal = Calculator()
cal.add(1, 2)
cal.minus(1,2)
cal.times(1,2)
cal.divide(1,2)
print 'name:', cal.name
print 'price:', cal.price
