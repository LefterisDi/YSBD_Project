AUXOBJ      = ./AuxFuncs/auxFuncs.o

OBJS        = $(AUXOBJ) ./HT/test.o
PRIM_OBJS   = ./HT/HT.o
SEC_OBJS    = ./SHT/SHT.o

SOURCE	    = ./AuxFuncs/auxFuncs.c ./HT/test.c
PRIM_SOURCE = ./HT/HT.c
SEC_SOURCE  = ./SHT/SHT.c

HEADER      = ./AuxFuncs/auxFuncs.h
PRIM_HEADER = ./HT/HT.h ./HT/HT_structs.h
SEC_HEADER  = ./SHT/SHT.h ./SHT/SHT_structs.h

OUT  	 	= index
CC	 	 	= gcc -Wall
FLAGS    	= -c
DEBUG	 	= -g3

# ------------------------------------------------- #

all: $(OUT)

# Creating Executables
index : $(OBJS) $(PRIM_OBJS) $(SEC_OBJS)
	$(CC) $(DEBUG) -o index $(OBJS) $(PRIM_OBJS) $(SEC_OBJS) ./BF/BF_64.a -no-pie

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
	rm -f $(OUT) $(OBJS) $(PRIM_OBJS) $(SEC_OBJS)

# ------------------------------------------------- #

# Counting Files
count:
	wc $(SOURCE) $(PRIM_SOURCE) $(SEC_SOURCE) $(HEADER) $(PRIM_HEADER) $(PRIM_SOURCE)
