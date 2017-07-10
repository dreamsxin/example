text = "This is my first test.\nThis is next line.";

print __name__

my_file = open('./my_file.txt', 'w')
my_file.write(text)
my_file.close()

append_text = "\nThis is appended line."

my_file = open('./my_file.txt', 'a')
my_file.write(append_text)
my_file.close()
