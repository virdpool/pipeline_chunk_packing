#!/bin/bash

sync; echo 1 > /proc/sys/vm/drop_caches

sysctl -w vm.nr_hugepages=1000
cat /proc/meminfo | grep HugePages

export JIT=1
export FULL_MEM=1
export HARD_AES=1
export ARGON2_SSSE3=1
export SECURE=1
export LARGE_PAGES=1
# you can try thread overuse
# export CTX_COUNT=30

# you will probably get equal results
UV_THREADPOOL_SIZE=$(nproc) ./src/bench_req_res.coffee
# UV_THREADPOOL_SIZE=$(nproc) ./src/bench_req_res_split.coffee
# UV_THREADPOOL_SIZE=$(nproc) ./src/bench_req_res2.coffee

