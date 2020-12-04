CC=arm-linux-gnueabihf-gcc

obj-m:=mydev.o

ifndef EOS_REPO
  EOS_REPO=/hostroot/home/biomotion/eos-2020
endif

all:
	make -C $(EOS_REPO)/linux-xlnx M=$(PWD) modules
clean:
	make -C $(EOS_REPO)/linux-xlnx M=$(PWD) clean
