#pragma once

bool randomx_pipeline_fan_1n_mod_in(Randomx_pipeline_node* node, Randomx_pipeline_thread* _thread) {
  node->heartbeat_counter++;
  
  if (node->pad_i_list.size() == 0) {
    return false;
  }
  
  if (node->pad_o_list.size() <= 1) {
    return false;
  }
  
  
  Message_ring<Randomx_pipeline_task>** o_ring_list       = node->pad_o_list.data();
  size_t                        o_ring_list_count = node->pad_o_list.size();
  
  Message_ring<Randomx_pipeline_task>* i_ring = node->pad_i_list[0];
  if (!i_ring) {
    return false;
  }
  
  if (!i_ring->can_pull()) {
    return false;
  }
  
  bool ret = false;
  // TODO config
  size_t task_saturation_threshold = 100;
  
  // TODO config
  for(int i=0;i<10;i++) {
    if (!i_ring->can_pull()) break;
    Message_ring<Randomx_pipeline_task>* send_ring = nullptr;
    
    // NOTE o_ring_list_count[0] is o_ring_err
    for(size_t j=1;j<o_ring_list_count;j++) {
      Message_ring<Randomx_pipeline_task>* o_ring = o_ring_list[j];
      if (!o_ring) continue;
      o_ring->push_and_get_free_nonbackpressure_push_size();
    }
    
    // policy: first count < hash_thread_task_saturation_threshold
    // reason keep most threads in sleep state, wakeup introduces extra latency
    //   much more than calc hash_thread_task_saturation_threshold hashes
    //   hash_thread_task_saturation_threshold should be tuned
    for(size_t j=1;j<o_ring_list_count;j++) {
      Message_ring<Randomx_pipeline_task>* o_ring = o_ring_list[j];
      if (!o_ring) continue;
      if (o_ring->get_available_pull_size() >= task_saturation_threshold) continue;
      send_ring = o_ring;
      break;
    }
    
    if (!send_ring) {
      // policy: most free ring
      // assume all have same size, so most free count == less loaded ring
      size_t best_free_count = 0;
      for(size_t j=1;j<o_ring_list_count;j++) {
        Message_ring<Randomx_pipeline_task>* o_ring = o_ring_list[j];
        if (!o_ring) continue;
        size_t curr_free_count = o_ring->get_free_nonbackpressure_push_size();
        if (best_free_count < curr_free_count) {
          send_ring = o_ring;
          best_free_count = curr_free_count;
        }
      }
    }
    
    if (send_ring) {
      ret = true;
      Randomx_pipeline_task* task = i_ring->pull();
      send_ring->push(task);
    }
  }
  
  return ret;
}
