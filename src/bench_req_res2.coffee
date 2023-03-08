#!/usr/bin/env iced
fs = require "fs"
os = require "os"
crypto = require "crypto"
require "fy"
# require "lock_mixin"
randomx = require "./randomx"
randomx_flag = require "./randomx_flag"
config = require "./config"

cb = (err)->
  if err
    throw err
  puts "done"
  process.exit()

# ###################################################################################################
#    config
# ###################################################################################################
flags = 0
flags |= randomx_flag.FLAG_JIT           if config.jit
flags |= randomx_flag.FLAG_FULL_MEM      if config.full_mem
flags |= randomx_flag.FLAG_HARD_AES      if config.hard_aes
flags |= randomx_flag.FLAG_ARGON2_SSSE3  if config.argon2_ssse3
flags |= randomx_flag.FLAG_SECURE        if config.secure
flags |= randomx_flag.FLAG_LARGE_PAGES   if config.large_pages

randomx_key_buf = Buffer.from "some randomx ds key"
pack_key_buf    = Buffer.from "some pack key"

thread_count = os.cpus().length

# ###################################################################################################
#    init
# ###################################################################################################
p "randomx init...", flags
root_ctx = new randomx.Randomx_context

start_ts = Date.now()
await root_ctx.init randomx_key_buf, thread_count, flags, defer(err); return cb err if err
elp_ts = Date.now() - start_ts

ctx_list = []
ctx_list.push root_ctx
for i in [1 ... config.ctx_count] by 1
  ctx = new randomx.Randomx_context
  ctx.init_copy_sync root_ctx
  ctx_list.push ctx

p "chunk init..."

chunk_i_list = []
for i in [0 ... config.work_item_count]
  buf = Buffer.alloc 256*1024
  chunk_i_list.push buf
  crypto.randomFillSync buf

chunk_o_list = []
for i in [0 ... config.work_item_count]
  buf = Buffer.alloc 256*1024
  chunk_o_list.push buf

# ###################################################################################################
#    bench
# ###################################################################################################
p "bench..."
bench_count = 0
start_ts = Date.now()
working = true
work_item_idx = 0

for ctx, idx in ctx_list
  # continue if idx != 0
  do (ctx)->
    while working
      work_item_i = chunk_i_list[work_item_idx]
      work_item_o = chunk_o_list[work_item_idx]
      work_item_idx++
      work_item_idx = 0 if work_item_idx >= config.work_item_count
      
      work_item_i2 = chunk_i_list[work_item_idx]
      work_item_o2 = chunk_o_list[work_item_idx]
      work_item_idx++
      work_item_idx = 0 if work_item_idx >= config.work_item_count
      
      await ctx.chunk_pack2 pack_key_buf, work_item_i, work_item_o, pack_key_buf, work_item_i2, work_item_o2, config.packing_iter, defer()
      
      bench_count+=2
    return

ema_p = config.bench_ema_p
ema_q = 1-ema_p
cps_ema = 0
for i in [0 ... config.bench_sample_count]
  loop
    elp_ts = Date.now() - start_ts
    break if elp_ts > config.bench_sample_time
    await setTimeout defer(), 100
  
  cps = bench_count / (elp_ts / 1000)
  # quick start
  if cps_ema == 0
    cps_ema = cps
  cps_ema = cps_ema*ema_p + cps*ema_q
  p "cps = #{cps.toFixed 2} ema = #{cps_ema.toFixed 2}"
  
  bench_count = 0
  start_ts = Date.now()

working = false

cb()
