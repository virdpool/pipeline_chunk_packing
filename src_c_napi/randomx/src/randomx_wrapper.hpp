#pragma once

class Randomx_context;
class Init_worker_thread {
  public:
  THREAD_TYPE thread_id;
  class Randomx_context* ctx;
  unsigned long dataset_init_start_item;
  unsigned long dataset_init_item_count;
};

static void *init_dataset_thread(void *data);
