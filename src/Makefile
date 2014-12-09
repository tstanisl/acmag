TARGET  := ../acmag
SRCS    := \
	ast.c \
	common.c \
	compiler.c \
	cstr.c \
	lxr.c \
	main.c \
	object.c \
	machine.c \
	machine_test.c \
	value.c \
	varmap.c \
	vec.c
	

CC = gcc
CFLAGS = -std=gnu99 -O0 -Wall -pedantic -g -ggdb
LDFLAGS = -rdynamic
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
	${CC} ${CFLAGS} -MM $< > $@

clean:
	rm *.o ${TARGET} *.dep

distclean: clean

