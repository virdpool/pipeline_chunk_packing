#pragma once

void* randomx_pipeline_worker_thread(void* ptr) {
  Randomx_pipeline_thread* thread = (Randomx_pipeline_thread*)ptr;
  
  if (thread->cpu_core_id) {
    std::string err;
    if (!thread_affinity_single_core_set(err, thread->thread, thread->cpu_core_id-1)) {
      fprintf(stderr, "pipeline thread error %s\n", err.c_str());
      return ptr;
    }
  }
  
  
  Randomx_pipeline_node** node_list = thread->node_list.data();
  size_t node_list_count    = thread->node_list.size();
  
  bool need_sleep = false;
  while (!thread->need_shutdown) {
    bool need_wait = true;
    for(size_t i=0;i<node_list_count;i++) {
      Randomx_pipeline_node* node = node_list[i];
      bool res = node->fn(node, thread);
      need_wait &= !res;
      if (thread->need_shutdown) break;
    }
    if (need_wait) {
      atomic_thread_fence(std::memory_order_release);
      if (need_sleep) {
        std::this_thread::sleep_for(std::chrono::microseconds(thread->idle_delay_mcs));
      } else {
        // last hope before go to sleep. Maybe counters were outdated. Just 1 spinlock iteration
        need_sleep = true;
      }
      atomic_thread_fence(std::memory_order_acquire);
    } else {
      need_sleep = false;
    }
  }
  thread->started = false;
  return ptr;
}
