TARGET  := ../acmag
SRCS    := \
	lxr.c \
	main.c
	

CC = gcc
CFLAGS = -std=gnu99 -O0 -Wall -ggdb 
LDFLAGS = 
LIBS    = 

OBJS    := ${SRCS:.c=.o} 
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 


.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} -o $@ $^ ${LIBS} ${LDFLAGS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

clean:
	rm *.o ${TARGET} *.dep

distclean: clean

