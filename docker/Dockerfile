FROM ubuntu:18.04
LABEL maintainer="bensonpuppy1997@gmail.com"

ARG USER="user"
ARG PASSWD="pass"

RUN apt-get update && \
    apt-get install -y \
        build-essential libc6-dev \
        manpages-posix-dev \
        manpages-dev \
        lib32ncurses5-dev libncurses5-dev \
        libxrender-dev libxtst-dev \
        flex bison swig python-dev \
        ibyaml-dev pkg-config python3-dev \
        make patch \
        net-tools \
        wget \
        sudo \
        tar \
        vim \
        git \
        build-essential \
        parted \
        dosfstools \
	minicom

COPY arm-linux-toolchain-bin.4.0.2.tar.gz /data/

WORKDIR /data
RUN tar zxvf arm-linux-toolchain-bin.4.0.2.tar.gz -C / 

ENV UID=1000
RUN useradd -mU -s /bin/bash \
        --password $PASSWD \
        --uid $UID \
        --group sudo \
        $USER && \
    echo "$USER:$PASSWD" | chpasswd
   
USER $USER
ENV HOME=/home/$USER
#WORKDIR /home/$USER

RUN echo "source /$HOME/.arm_bashrc" >> $HOME/.bashrc
