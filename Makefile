CC = gcc
CFLAGS = -I Mint_lib/include -O2
LDFLAGS = -lm
LIB_SRCS = Mint_lib/src/Mint_Engine.c Mint_lib/src/Mint_Default_Assets.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

all: mint_test

mint_test: $(LIB_OBJS) Mint_test.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o mint_test
