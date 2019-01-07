AUXOBJ   = ./AuxFuncs/auxFuncs.o
OBJS     = $(AUXOBJ) ./HT/test.o ./HT/HT.o ./SHT/SHT.o
SOURCE	 = ./HT/test.c ./HT/HT.c ./SHT/SHT.c ./AuxFuncs/auxFuncs.c
HEADER   = ./HT/HT.h ./HT/HT_structs.h ./SHT/SHT.h ./SHT/SHT_structs.h ./AuxFuncs/auxFuncs.h
OUT  	 = index
CC	 	 = gcc -Wall
FLAGS    = -c
DEBUG	 = -g3

all: $(OUT)

# Creating Executables
index : $(OBJS)
	$(CC) $(DEBUG) -o index $(OBJS) ./BF/BF_64.a -no-pie

# ------------------------------------------------- #

# Creating Object Files
./HT/test.o : ./HT/test.c
	$(CC) $(DEBUG) -o ./HT/test.o           $(FLAGS) ./HT/test.c

./HT/HT.o : ./HT/HT.c
	$(CC) $(DEBUG) -o ./HT/HT.o             $(FLAGS) ./HT/HT.c

./SHT/SHT.o : ./SHT/SHT.c
	$(CC) $(DEBUG) -o ./SHT/SHT.o           $(FLAGS) ./SHT/SHT.c

./AuxFuncs/auxFuncs.o : ./AuxFuncs/auxFuncs.c
	$(CC) $(DEBUG) -o ./AuxFuncs/auxFuncs.o $(FLAGS) ./AuxFuncs/auxFuncs.c

# ------------------------------------------------- #

# Cleaning Files
clean:
	rm -f $(OUT) $(OBJS)

# ------------------------------------------------- #

# Counting Files
count:
	wc $(SOURCE) $(HEADER)
