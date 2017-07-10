condition = 1
while condition < 5:
    print condition
    condition += 1


example_list = ["abc",2,3,4,5]
print(example_list)
for i in example_list:
    print(i)

for i in range(1, 5):
    print("range", i)

for i in range(1, 10, 2):
    print("step range", i)

x,y,z = 1,2,0
if x>y:
    print('x is greater than y')
elif y>z:
    print('y is greater than z')
else:
    print('x is less than y')

x,y,z = 1,2,3
if x<y<z:
    print('x is less than y and y is less than z')

if x<y and y<z:
    print('x is less than y and y is less than z')


