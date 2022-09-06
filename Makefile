CC = g++
CFLAGS = -Wall -pthread -std=c++17
BIGFLOAT = BigFloat/BigFloat
HEADERS =
PROGRAM = threadpool
OBJECTS = BigFloat.o
EXAMPLE = example

default: ${EXAMPLE}

${OBJECTS} : BigFloat/BigFloat.cc
	${CC} -c ${CFLAGS} $^

${EXAMPLE}.o: ${EXAMPLE}.cpp
	g++ ${CFLAGS} -c ${EXAMPLE}.cpp -o ${EXAMPLE}.o

${EXAMPLE}: ${EXAMPLE}.o ${OBJECTS}
	g++ ${CFLAGS} ${EXAMPLE}.o ${OBJECTS} -o ${EXAMPLE}

run: ${EXAMPLE}
	./${EXAMPLE}

clean:
	-rm -f ${EXAMPLE}.o
	-rm -f ${EXAMPLE}
	-rm -f BigFloat.o
