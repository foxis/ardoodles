# Target file name (without extension).
TARGET = test_me test_tr


# List C source files here. (C dependencies are automatically generated.)
SRC = test_me.c me.c
SRC1 = test_tr.c me.c transmission.c hamming.c

# Define programs and commands.
SHELL = sh
CC = gcc
REMOVE = rm -f
COPY = cp




# Define all object files.
OBJ = $(SRC:.c=.o) 
OBJ1 = $(SRC1:.c=.o) 

# Define all listing files.
LST = $(SRC:.c=.lst) 
LST1 = $(SRC1:.c=.lst) 


# Default target.
all: $(TARGET)

# Link: create ELF output file from object files.
test_me: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)
	
test_tr: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $(OBJ1)

# Compile: create object files from C source files.
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 



# Target: clean project.
clean:
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) .dep/*



