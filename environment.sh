#!/bin/bash

REPO="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH=$PATH:$REPO/scripts

export EOS_IMAGE_NAME=eos-2020:latest
export EOS_CONTAINER_NAME=eos

export MICROTIME_HOST_PATH=$HOME/microtime
export MICROTIME_PATH=/hostroot$MICROTIME_HOST_PATH
