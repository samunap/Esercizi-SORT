CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Output files
LIB = libfasttrigo.a
EXEC = fasttrigo

# Object files 
OBJS = fast_trigo.o
MAIN_OBJ = main.o

all: $(EXEC)

# Build della libreria
$(LIB): $(OBJS)
	ar rcs $@ $^

$(EXEC): $(MAIN_OBJ) $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(MAIN_OBJ) $(LIB) $(EXEC)

.PHONY: all clean
