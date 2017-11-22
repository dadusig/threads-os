CC = gcc
CFLAGS = -pthread -g
TARGETS = multiproc_1 multiproc_2 multithread_1 multithread_2

all: $(TARGETS)

$(TARGETS): % : %.c util.c util.h
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGETS)
