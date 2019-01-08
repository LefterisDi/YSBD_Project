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
OUT2  	 	= test
CC	 	 	= gcc -Wall
FLAGS    	= -c
DEBUG	 	= -g3

# ------------------------------------------------- #

all: $(OUT) $(OUT2)

# Creating Executables
index : $(OBJS) $(PRIM_OBJS) $(SEC_OBJS)
	$(CC) $(DEBUG) -o $(OUT) $(OBJS) $(PRIM_OBJS) $(SEC_OBJS) ./BF/BF_64.a -no-pie

test : $(AUXOBJ) ./HT/ht_main_test.o $(PRIM_OBJS) $(SEC_OBJS)
	$(CC) $(DEBUG) -o $(OUT2) $(AUXOBJ) ./HT/ht_main_test.o $(PRIM_OBJS) $(SEC_OBJS) ./BF/BF_64.a -no-pie

# ------------------------------------------------- #

# Creating Object Files
./HT/ht_main_test.o : ./HT/ht_main_test.c
	$(CC) $(DEBUG) -o ./HT/ht_main_test.o   $(FLAGS) ./HT/ht_main_test.c

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
