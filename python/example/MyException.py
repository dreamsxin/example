class MyException(Exception):
    def __init__(self,message):
        Exception.__init__(self,message)
        self.message=message  

class MyException2(Exception):pass
