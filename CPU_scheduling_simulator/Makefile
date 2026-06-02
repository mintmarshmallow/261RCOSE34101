CC = gcc
CFLAGS = -Wall -Wextra -g

OBJS = main.o process.o scheduler.o evaluation.o
TARGET = simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c process.h scheduler.h evaluation.h
	$(CC) $(CFLAGS) -c main.c

process.o: process.c process.h
	$(CC) $(CFLAGS) -c process.c

scheduler.o: scheduler.c scheduler.h process.h evaluation.h
	$(CC) $(CFLAGS) -c scheduler.c

evaluation.o: evaluation.c evaluation.h process.h
	$(CC) $(CFLAGS) -c evaluation.c

clean:
	rm -f $(OBJS) $(TARGET)
