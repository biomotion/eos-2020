CC=arm-unknown-linux-gnu-g++
CFLAGS=-Wall

ARM_LIB_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/lib/
ARM_INC_OPT=/opt/arm-unknown-linux-gnu-lib/arm-unknown-linux-gnu/include/
MICROTIME_INC_OPT=${MICROTIME_PATH}/linux/include/


all: 
	${CC} ${CFLAGS} -o reader reader.cpp -L ${ARM_LIB_OPT} -I ${ARM_INC_OPT} -I ${MICROTIME_INC_OPT}
	${CC} ${CFLAGS} -o writer writer.cpp -L ${ARM_LIB_OPT} -I ${ARM_INC_OPT} -I ${MICROTIME_INC_OPT}

clean: 
	${RM} writer reader
