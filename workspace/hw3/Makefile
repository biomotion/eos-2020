CXX=g++
CFLAGS=-Wall -lpthread


all:
	${CXX} ${CFLAGS} -std=c++11 -c hw3.cpp
	${CXX} ${CFLAGS} -std=c++11 -c socket_utils.c
	${CXX} ${CFLAGS} -std=c++11 -c parking_lot.cpp
	${CXX} -std=c++11 hw3.o socket_utils.o parking_lot.o ${CFLAGS} ${INC_FLAGS} -o hw3

clean:
	${RM} hw3.o socket_utils.o parking_lot.o hw3