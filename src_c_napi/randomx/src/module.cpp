#include <node_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.hpp"
#include "macro.hpp"
#include "randomx.h"
#include "randomx_long_with_entropy.cpp"
#include "feistel_msgsize_key_cipher.cpp"
#include "sha-256.h"
#include "thread_util.hpp"



#include "randomx_wrapper.hpp"
#include "message_ring.hpp"
#include "pipeline_util.hpp"

class Randomx_context;
class Randomx_pipeline_task;
class Randomx_pipeline_thread;
class Randomx_pipeline_node;

#include "Randomx_context/class.cpp"
void* randomx_pipeline_worker_thread(void* ptr);
std::vector<bool (*)(Randomx_pipeline_node*, Randomx_pipeline_thread*)> randomx_pipeline_global_registry_fn_list;
std::vector<Randomx_pipeline_task*> randomx_pipeline_global_free_pipeline_task_list;
i32 randomx_pipeline_global_task_instance_uid = 0;
i32 randomx_pipeline_global_task_uid = 0;
#include "Randomx_pipeline_task/class.cpp"
#include "Randomx_pipeline_thread/class.cpp"
#include "Randomx_pipeline_node/class.cpp"
#include "Randomx_context/init.cpp"
#include "Randomx_context/init_copy.cpp"
#include "Randomx_context/free.cpp"
#include "Randomx_context/chunk_pack.cpp"
#include "Randomx_context/chunk_pack2.cpp"
#include "Randomx_context/chunk_pack_s1.cpp"
#include "Randomx_context/chunk_pack_s2.cpp"
#include "Randomx_pipeline_task/set.cpp"
#include "Randomx_pipeline_thread/cpu_core_id_set.cpp"
#include "Randomx_pipeline_thread/node_attach.cpp"
#include "Randomx_pipeline_thread/start.cpp"
#include "Randomx_pipeline_thread/stop_schedule.cpp"
#include "Randomx_pipeline_thread/stop_hard_term.cpp"
#include "Randomx_pipeline_thread/stop_hard_kill.cpp"
#include "Randomx_pipeline_thread/started_get.cpp"
#include "Randomx_pipeline_node/pad_reset.cpp"
#include "Randomx_pipeline_node/pad_i_add.cpp"
#include "Randomx_pipeline_node/pad_o_add.cpp"
#include "Randomx_pipeline_node/fn_set.cpp"
#include "Randomx_pipeline_node/fn_get.cpp"
#include "Randomx_pipeline_node/link.cpp"
#include "Randomx_pipeline_node/ep_i.cpp"
#include "Randomx_pipeline_node/ep_o.cpp"
#include "Randomx_pipeline_node/task_push.cpp"
#include "Randomx_pipeline_node/task_pull.cpp"
#include "Randomx_pipeline_node/task_pull_count.cpp"



#include "randomx_wrapper.cpp"
#include "randomx_pipeline_fan_1n_mod_in.cpp"
#include "randomx_pipeline_fan_n1_mod_out.cpp"
#include "randomx_pipeline_worker_thread.cpp"
#include "randomx_pipeline_pack.cpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  randomx_pipeline_global_registry_fn_list.push_back(randomx_pipeline_fan_1n_mod_in);
  randomx_pipeline_global_registry_fn_list.push_back(randomx_pipeline_fan_n1_mod_out);
  randomx_pipeline_global_registry_fn_list.push_back(randomx_pipeline_pack);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  CLASS_DEF(Randomx_context)
  CLASS_METHOD(Randomx_context, init)
  CLASS_METHOD(Randomx_context, init_copy)
  CLASS_METHOD(Randomx_context, free)
  CLASS_METHOD(Randomx_context, chunk_pack)
  CLASS_METHOD(Randomx_context, chunk_pack2)
  CLASS_METHOD(Randomx_context, chunk_pack_s1)
  CLASS_METHOD(Randomx_context, chunk_pack_s2)
  CLASS_EXPORT(Randomx_context)
  CLASS_DEF(Randomx_pipeline_task)
  CLASS_METHOD(Randomx_pipeline_task, set)
  CLASS_EXPORT(Randomx_pipeline_task)
  CLASS_DEF(Randomx_pipeline_thread)
  CLASS_METHOD(Randomx_pipeline_thread, cpu_core_id_set)
  CLASS_METHOD(Randomx_pipeline_thread, node_attach)
  CLASS_METHOD(Randomx_pipeline_thread, start)
  CLASS_METHOD(Randomx_pipeline_thread, stop_schedule)
  CLASS_METHOD(Randomx_pipeline_thread, stop_hard_term)
  CLASS_METHOD(Randomx_pipeline_thread, stop_hard_kill)
  CLASS_METHOD(Randomx_pipeline_thread, started_get)
  CLASS_EXPORT(Randomx_pipeline_thread)
  CLASS_DEF(Randomx_pipeline_node)
  CLASS_METHOD(Randomx_pipeline_node, pad_reset)
  CLASS_METHOD(Randomx_pipeline_node, pad_i_add)
  CLASS_METHOD(Randomx_pipeline_node, pad_o_add)
  CLASS_METHOD(Randomx_pipeline_node, fn_set)
  CLASS_METHOD(Randomx_pipeline_node, fn_get)
  CLASS_METHOD(Randomx_pipeline_node, link)
  CLASS_METHOD(Randomx_pipeline_node, ep_i)
  CLASS_METHOD(Randomx_pipeline_node, ep_o)
  CLASS_METHOD(Randomx_pipeline_node, task_push)
  CLASS_METHOD(Randomx_pipeline_node, task_pull)
  CLASS_METHOD(Randomx_pipeline_node, task_pull_count)
  CLASS_EXPORT(Randomx_pipeline_node)
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
