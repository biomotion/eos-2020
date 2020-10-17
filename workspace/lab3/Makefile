CC=arm-unknown-linux-gnu-gcc
CC_X86=gcc
CFLAGS=-Wall

ARM_LIB_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/lib/
ARM_INC_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/include/
MICROTIME_INC_OPT=${MICROTIME_PATH}/linux/include/

EXE_FILE=a.out

all: 
	${CC} ${CFLAGS} -o ${EXE_FILE} ${TARGET} -L ${ARM_LIB_OPT} -I ${ARM_INC_OPT} -I ${MICROTIME_INC_OPT}

clean: 
	${RM} ${EXE_FILE}
