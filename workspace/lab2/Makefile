CC=arm-unknown-linux-gnu-gcc
CC_X86=gcc
CFLAGS=-Wall

ifndef TARGET
  TARGET=lab2
endif

ARM_LIB_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/lib/
ARM_INC_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/include/
MICROTIME_INC_OPT=${MICROTIME_PATH}/linux/include/

EXE_FILE=$(TARGET)

all: 
	${CC} ${CFLAGS} -o ${EXE_FILE} ${TARGET}.c -L ${ARM_LIB_OPT} -I ${ARM_INC_OPT} -I ${MICROTIME_INC_OPT}

x86:
	${CC_X86} ${CFLAGS} -o ${EXE_FILE}.x86 ${TARGET}.c


clean: 
	${RM} ${EXE_FILE}
