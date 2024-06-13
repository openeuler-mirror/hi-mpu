#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.

current_path=$(pwd)
chmod 777 $current_path/ -R
set -e
./build.sh
./packet.sh
