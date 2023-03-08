{Pipeline} = require "./pipeline"

pipeline = new Pipeline
pipeline.mod    = require "../randomx"
pipeline.prefix_set "Randomx_pipeline_"
fn_hash_idx = 1
pipeline.fn_hash.fan_1n_mod_in  = fn_hash_idx++
pipeline.fn_hash.fan_n1_mod_out = fn_hash_idx++
pipeline.fn_hash.pack           = fn_hash_idx++;

module.exports = pipeline

###
# quick starter, boilerplate
pipeline = require "./pipeline/pipeline_randomx_pipeline_default"
pipeline.default_layout()
worker_thread = pipeline.thread_alloc()
my_node = pipeline.node_create "napi_pipeline_easy_fn"
worker_thread.node_attach_sync my_node

pipeline.chain pipeline.node_i_list[0], my_node, pipeline.node_o_list[0]
pipeline.start()

task = pipeline.task_get()
# TODO task set
task.hello = "world"
pipeline.task_push task

loop
  ret = pipeline.task_pull()
  break if ret
  await setTimeout defer(), 10

pipeline.task_pull_ack ret
###
