#pragma once

bool randomx_pipeline_pack(Randomx_pipeline_node* node, Randomx_pipeline_thread* thread) {
  node->heartbeat_counter++;
  
  if (node->pad_i_list.size() == 0) {
    return false;
  }
  
  if (node->pad_o_list.size() <= 1) {
    return false;
  }
  
  Message_ring<Randomx_pipeline_task>* i_ring = node->pad_i_list[0];
  if (!i_ring) {
    return false;
  }
  if (!i_ring->can_pull()) {
    return false;
  }
  
  
  Message_ring<Randomx_pipeline_task>* err_o_ring = node->pad_o_list[0];
  if (!err_o_ring) {
    return false;
  }
  if (!err_o_ring->push_and_get_free_nonbackpressure_push_size()) {
    return false;
  }
  
  Message_ring<Randomx_pipeline_task>* o_ring = node->pad_o_list[1];
  if (!o_ring) {
    return false;
  }
  if (!o_ring->push_and_get_free_nonbackpressure_push_size()) {
    return false;
  }
  
  bool res = false;
  while (i_ring->can_pull()) {
    if (!o_ring->get_free_nonbackpressure_push_size()) {
      return res;
    }
    Randomx_pipeline_task* task = i_ring->pull();
    // check ctx and copy if needed
    if (thread->curr_feed_ctx != task->feed_root_ctx) {
      thread->curr_feed_ctx = task->feed_root_ctx;
      if (thread->work_ctx) {
        randomx_destroy_vm(thread->work_ctx->vm_ptr);
        // always is_dataset_copy
      }
      if (!thread->work_ctx) {
        thread->work_ctx = new Randomx_context;
      }
      thread->work_ctx->flags      = task->feed_root_ctx->flags;
      thread->work_ctx->cache_ptr  = task->feed_root_ctx->cache_ptr;
      thread->work_ctx->dataset_ptr= task->feed_root_ctx->dataset_ptr;
      
      thread->work_ctx->vm_ptr = randomx_create_vm((randomx_flags)thread->work_ctx->flags, thread->work_ctx->cache_ptr, thread->work_ctx->dataset_ptr);
      if (!thread->work_ctx->vm_ptr) {
        printf("!vm_ptr randomx_create_vm fail\n");
        return res;
      }
    }
    
    randomx_encrypt_chunk(thread->work_ctx->vm_ptr, task->pack_key, task->pack_key_len, task->chunk_i, task->chunk_i_len, task->chunk_o, task->packing_iter);
    
    o_ring->push(task);
    res = true;
  }
  
  return res;
}
