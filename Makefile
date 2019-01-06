AUXOBJ   = auxFuncs.o
OBJS     = $(AUXOBJ) ./HT/test.o ./HT/HT.o ./SHT/SHT.o
SOURCE	 = ./HT/test.c ./HT/HT.c ./SHT/SHT.c auxFuncs.c
HEADER   = ./HT/HT.h ./HT/HT_structs.h ./SHT/SHT.h ./SHT/SHT_structs.h auxFuncs.h
OUT  	 = index
CC	 	 = gcc -Wall
FLAGS    = -g3 -c

all: $(OUT)

# Creating Executables
index : $(OBJS)
	$(CC) $(OBJS) ./BF/BF_64.a -o index -no-pie

# ------------------------------------------------- #

# Creating Object Files
./HT/test.o : ./HT/test.c
	$(CC) $(FLAGS) ./HT/test.c -o ./HT/test.o

./HT/HT.o : ./HT/HT.c
	$(CC) $(FLAGS) ./HT/HT.c   -o ./HT/HT.o

./SHT/SHT.o : ./SHT/SHT.c
	$(CC) $(FLAGS) ./SHT/SHT.c -o ./SHT/SHT.o

auxFuncs.o : auxFuncs.c
	$(CC) $(FLAGS) auxFuncs.c

# ------------------------------------------------- #

# Cleaning Files
clean:
	rm -f $(OUT) $(OBJS)

# ------------------------------------------------- #

# Counting Files
count:
	wc $(SOURCE) $(HEADER)
