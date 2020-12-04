CC=arm-unknown-linux-gnu-g++
CC_X86=g++
CFLAGS=-Wall

ARM_LIB_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/lib/
ARM_INC_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/include/
MICROTIME_INC_OPT=${MICROTIME_PATH}/linux/include/

EXE_FILE=run_parkinglot

all: 
	${CC} ${CFLAGS} -o ${EXE_FILE} -L ${ARM_LIB_OPT} -I ${ARM_INC_OPT} -I ${MICROTIME_INC_OPT} parking_lot.cpp main.cpp

clean: 
	${RM} ${EXE_FILE} x86_${EXE_FILE}

x86:
	${CC_X86} ${CFLAGS} -o x86_${EXE_FILE} parking_lot.cpp main.cpp

