my_file = open('./my_file.txt', 'r')
content = my_file.read()
my_file.close()

print(content)

my_file = open('./my_file.txt', 'r')
firstline = my_file.readline()
my_file.close()

print(firstline)

my_file = open('./my_file.txt', 'r')
lines = my_file.readlines()
my_file.close()

print(lines)
