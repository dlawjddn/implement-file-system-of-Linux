SRC_DIR = ./source/
INC_DIR = ./include/

TARGET = a.out
OBJS = $(SRC_DIR)main.o $(SRC_DIR)dir.o $(SRC_DIR)commands.o $(SRC_DIR)stack.o $(SRC_DIR)date.o $(SRC_DIR)thread.o $(SRC_DIR)users.o

$(TARGET):$(OBJS) $(INC_DIR)osproject.h
	gcc -o $(TARGET) $(OBJS) -lpthread

main.o: $(SRC_DIR)main.c
	gcc -c -o main.o $(SRC_DIR)main.c

dir.o: $(SRC_DIR)dir.c
	gcc -c -o dir.o $(SRC_DIR)dir.c

commands.o: $(SRC_DIR)commands.c 
	gcc -c -o commands.o $(SRC_DIR)commands.c

stack.o: $(SRC_DIR)stack.c
	gcc -c -o stack.o $(SRC_DIR)stack.c

date.o: $(SRC_DIR)date.c
	gcc -c -o date.o $(SRC_DIR)date.c

thread.o: $(SRC_DIR)thread.c
	gcc -c -o thread.o $(SRC_DIR)thread.c

users.o: $(SRC_DIR)user.c
	gcc -c -o user.o $(SRC_DIR)users.c

clean:
	rm -f $(SRC_DIR)*.o
	rm -f a.out
