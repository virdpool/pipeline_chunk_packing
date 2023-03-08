static void *init_dataset_thread(void *data) {
  Init_worker_thread *worker_ptr = (Init_worker_thread*) data;
  Randomx_context* ctx = worker_ptr->ctx;
  randomx_init_dataset(
    ctx->dataset_ptr,
    ctx->cache_ptr,
    worker_ptr->dataset_init_start_item,
    worker_ptr->dataset_init_item_count
  );
  return NULL;
}
