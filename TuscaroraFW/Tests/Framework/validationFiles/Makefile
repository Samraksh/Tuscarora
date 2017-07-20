
INCLUDES=../../../ 
CC=g++

all: TestNormal Add Replace Cancel

TestNormal:
	$(CC) -I$(INCLUDES) Validation.cpp -o TestNormal


Add:
	$(CC) -I$(INCLUDES) Add.cpp -o Add


Replace:
	$(CC) -I$(INCLUDES) Replace.cpp -o Replace


Cancel:
	$(CC) -I$(INCLUDES) Cancel.cpp -o Cancel

clean:
	rm -f *.o TestNormal Add Replace Cancel
