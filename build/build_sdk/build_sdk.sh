#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved.
current_path=$(pwd)
set -e

./build_securec.sh
./build.sh
./packet.sh
./clean_sdk.sh
