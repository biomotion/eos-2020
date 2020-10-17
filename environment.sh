#!/bin/bash

REPO="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH=$PATH:$REPO/scripts

export EOS_IMAGE_NAME=eos-2020:latest
export EOS_CONTAINER_NAME=eos
