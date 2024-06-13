#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

PWD=$(pwd)
ROOT_DIR=${PWD}/../../

set -e

./build_mcs.sh
./build_libck.sh
./build_sample.sh