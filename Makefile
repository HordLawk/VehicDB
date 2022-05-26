CC = gcc

LINKERFLAG = -lm

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%.o)

all: ${BINS}
	@echo "Checking.."
	${CC} ${LINKERFLAG} ${BINS} -o main

%.o: %.c
	@echo "Creating object.."
	${CC} -c $<