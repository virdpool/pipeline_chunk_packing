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
export THREAD_AFFINITY=1

UV_THREADPOOL_SIZE=$(nproc) ./src/bench_pipeline.coffee
