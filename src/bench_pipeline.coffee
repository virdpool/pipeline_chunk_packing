#!/usr/bin/env iced
fs = require "fs"
os = require "os"
crypto = require "crypto"
require "fy"
# require "lock_mixin"
randomx = require "./randomx"
randomx_flag = require "./randomx_flag"
randomx_pipeline_default = require "./pipeline/pipeline_randomx_pipeline_default"
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
#    pipeline init
# ###################################################################################################
randomx_pipeline_default.default_layout()
# task_wip_count_threshold = config.ctx_count * 50 # TODO config
task_wip_count_threshold = config.ctx_count * 60 # TODO config
# task_wip_count_threshold = config.ctx_count * 70 # TODO config
# task_wip_count_threshold = config.ctx_count * 80 # TODO config
# task_wip_count_threshold = config.ctx_count * 90 # TODO config
# task_wip_count_threshold = config.work_item_count // 2

do ()->
  node_i = randomx_pipeline_default.node_i_list[0]
  node_o = randomx_pipeline_default.node_o_list[0]
  
  for i in [0 ... config.ctx_count]
    thread = randomx_pipeline_default.thread_alloc()
    if config.thread_affinity
      thread.cpu_core_id_set_sync i
    node = randomx_pipeline_default.node_create "pack"
    thread.node_attach_sync node
    randomx_pipeline_default.chain node_i, node, node_o
  
  randomx_pipeline_default.start()

# ###################################################################################################
#    bench
# ###################################################################################################
p "bench..."
bench_count = 0
start_ts = Date.now()
working = true
work_item_idx = 0

do ()->
  while working
    need_wait = true
    
    # push
    while randomx_pipeline_default.task_wip_count < task_wip_count_threshold
      need_wait = false
      task = randomx_pipeline_default.task_get()
      
      work_item_i = chunk_i_list[work_item_idx]
      work_item_o = chunk_o_list[work_item_idx]
      work_item_idx++
      work_item_idx = 0 if work_item_idx >= config.work_item_count
      
      task.set_sync(
        root_ctx,
        pack_key_buf,
        work_item_i,
        work_item_o,
        config.packing_iter
      )
      randomx_pipeline_default.task_push task
    
    # pull
    while task = randomx_pipeline_default.task_pull()
      need_wait = false
      randomx_pipeline_default.task_pull_ack task
      bench_count++
    
    if need_wait
      await setTimeout defer(), 10
  
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
