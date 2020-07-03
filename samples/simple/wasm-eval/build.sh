# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

WAMR_DIR=${PWD}/../wamr/
WASM_FILE_NAME=$1
C_FILE_NAME=$2

/opt/wasi-sdk-10.0/bin/clang     \
        --target=wasm32 -O3 \
        -z stack-size=4096 -Wl,--initial-memory=65536 \
        --sysroot=${WAMR_DIR}/wamr-sdk/app/libc-builtin-sysroot    \
        -Wl,--allow-undefined-file=${WAMR_DIR}/wamr-sdk/app/libc-builtin-sysroot/share/defined-symbols.txt \
        -Wl,--export=main,\
        -Wl,--allow-undefined,\
        -Wl,--no-threads,--strip-all,--no-entry \
        -nostdlib -o $WASM_FILE_NAME $C_FILE_NAME
#./jeffdump -o test_wasm.h -n wasm_test_file test.wasm
