import pickle

a1 = 'apple'  
b1 = {1: 'One', 2: 'Two', 3: 'Three'}  
c1 = ['fee', 'fie', 'foe', 'fum']
print a1
print b1
print c1

f1 = file('temp.pickle', 'wb')  
pickle.dump(a1, f1, True)  
pickle.dump(b1, f1, True)  
pickle.dump(c1, f1, True)  
f1.close()  
f2 = file('temp.pickle', 'rb')  
a2 = pickle.load(f2)  
b2 = pickle.load(f2)  
c2 = pickle.load(f2)  
f2.close()
print a2
print b2
print c2
