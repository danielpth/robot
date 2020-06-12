# List Source
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

LDFLAGS := -lpthread

%.o: %.c
	$(CC) -O0 -ggdb3 -Wformat=2 -Wextra -Wall -Werror -o $@ -c $<

all: robotd

robotd: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean distclean

clean: 
	rm -f $(OBJ) robotd

distclean: clean csclean
	rm -f *.log
