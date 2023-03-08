#pragma once

bool randomx_pipeline_fan_n1_mod_out(Randomx_pipeline_node* node, Randomx_pipeline_thread* _thread) {
  node->heartbeat_counter++;
  
  if (node->pad_i_list.size() == 0) {
    return false;
  }
  
  if (node->pad_o_list.size() == 0) {
    return false;
  }
  
  Message_ring<Randomx_pipeline_task>* o_ring = node->pad_o_list[0];
  if (!o_ring) {
    return false;
  }
  if (!o_ring->push_and_get_free_nonbackpressure_push_size()) {
    return false;
  }
  
  
  Message_ring<Randomx_pipeline_task>** i_ring_list       = node->pad_i_list.data();
  size_t                        i_ring_list_count = node->pad_i_list.size();
  
  bool res = false;
  for(size_t j=0;j<i_ring_list_count;j++) {
    Message_ring<Randomx_pipeline_task>* i_ring = i_ring_list[j];
    if (!i_ring) continue;
    while (i_ring->can_pull()) {
      if (!o_ring->get_free_nonbackpressure_push_size()) {
        return res;
      }
      Randomx_pipeline_task* task = i_ring->pull();
      o_ring->push(task);
      res = true;
    }
  }
  
  return res;
}
