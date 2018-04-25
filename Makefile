
CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -Wextra -pedantic 
DEPS = httpd.h CalcFramer.hpp CalcParser.hpp HandleTCPClient.h DieWithError.h
SRCS = httpd.cpp DieWithError.c HandleTCPClient.c CalcFramer.cpp CalcParser.cpp
MAIN_SRCS = main.c $(SRCS)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

default: httpd

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

httpd:    $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o httpd $(MAIN_OBJS) -lpthread

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f httpd *.o
