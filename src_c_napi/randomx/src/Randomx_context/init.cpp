void _Randomx_context_init_impl(
  std::string*& err,
  Randomx_context* _this,
  u8*          data_src,
  size_t       data_src_len,
  i32          thread_count,
  u32          flags
) {
  _this->flags = flags;
  _this->init_idx++;
  _this->cache_ptr = randomx_alloc_cache((randomx_flags)flags);
  if (!_this->cache_ptr) {
    err = new std::string("randomx_alloc_cache fail");
    return;
  }
  randomx_init_cache(_this->cache_ptr, data_src, data_src_len);
  
  _this->dataset_ptr = randomx_alloc_dataset((randomx_flags)flags);
  if (!_this->dataset_ptr) {
    err = new std::string("!dataset_ptr randomx_alloc_dataset fail");
    return;
  }
  
  
  std::vector<Init_worker_thread> sub_ctx_list;
  sub_ctx_list.resize(thread_count);
  
  unsigned long start_item = 0;
  unsigned long items_per_thread;
  unsigned long items_remainder;
  
  items_per_thread= randomx_dataset_item_count() / thread_count;
  items_remainder = randomx_dataset_item_count() % thread_count;
  
  for(i32 i=0;i<thread_count;i++) {
    Init_worker_thread& sub_ctx = sub_ctx_list[i];
    sub_ctx.ctx = _this;
    sub_ctx.dataset_init_start_item = start_item;
    if (i + 1 == thread_count) {
      sub_ctx.dataset_init_item_count = items_per_thread + items_remainder;
    } else {
      sub_ctx.dataset_init_item_count = items_per_thread;
    }
    
    start_item += sub_ctx.dataset_init_item_count;
    
    THREAD_CREATE(sub_ctx.thread_id, err, init_dataset_thread, &sub_ctx)
  }
  
  for(i32 i=0;i<thread_count;i++) {
    THREAD_JOIN(sub_ctx_list[i].thread_id)
  }
  
  _this->vm_ptr = randomx_create_vm((randomx_flags)flags, _this->cache_ptr, _this->dataset_ptr);
  if (!_this->vm_ptr) {
    err = new std::string("!vm_ptr randomx_create_vm fail");
    return;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//   sync
////////////////////////////////////////////////////////////////////////////////////////////////////
napi_value Randomx_context_init_sync(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(3)
  FN_ARG_THIS(Randomx_context)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_BUF(data_src)
  FN_ARG_I32(thread_count)
  FN_ARG_U32(flags)
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  std::string *err = nullptr;
  _Randomx_context_init_impl(err, _this, data_src, data_src_len, thread_count, flags);
  if (err) {
    napi_throw_error(env, nullptr, err->c_str());
    delete err;
    return ret_dummy;
  }
  
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  return ret_dummy;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//   async
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Worker_ctx_Randomx_context_init {
  Randomx_context* _this;
  u8* data_src;
  size_t data_src_len;
  napi_ref data_src_ref;
  i32 thread_count;
  u32 flags;
  
  std::string* err;
  napi_ref callback_reference;
  napi_async_work work;
};

void _worker_ctx_Randomx_context_init_clear(napi_env env, struct Worker_ctx_Randomx_context_init* worker_ctx) {
  if (worker_ctx->err) {
    delete worker_ctx->err;
    worker_ctx->err = nullptr;
  }
  napi_status status;
  if (worker_ctx->data_src_ref) {
    status = napi_delete_reference(env, worker_ctx->data_src_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for data_src");
      return;
    }
  }
  
  status = napi_delete_async_work(env, worker_ctx->work);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "napi_delete_async_work fail");
  }
  delete worker_ctx;
}

void _execute_Randomx_context_init(napi_env env, void* _data) {
  struct Worker_ctx_Randomx_context_init* worker_ctx = (struct Worker_ctx_Randomx_context_init*)_data;
  _Randomx_context_init_impl(worker_ctx->err, worker_ctx->_this, worker_ctx->data_src, worker_ctx->data_src_len, worker_ctx->thread_count, worker_ctx->flags);
}

void _complete_Randomx_context_init(napi_env env, napi_status execute_status, void* _data) {
  napi_status status;
  struct Worker_ctx_Randomx_context_init* worker_ctx = (struct Worker_ctx_Randomx_context_init*)_data;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //    prepare for callback (common parts)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value callback;
  status = napi_get_reference_value(env, worker_ctx->callback_reference, &callback);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to get referenced callback (napi_get_reference_value)");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return;
  }
  status = napi_delete_reference(env, worker_ctx->callback_reference);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to delete reference callback_reference");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return;
  }
  
  napi_value global;
  status = napi_get_global(env, &global);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create return value global (napi_get_global)");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  if (execute_status != napi_ok) {
    // avoid code duplication
    if (!worker_ctx->err) {
      worker_ctx->err = new std::string("execute_status != napi_ok");
    }
  }
  
  if (worker_ctx->err) {
    napi_helper_error_cb(env, worker_ctx->err->c_str(), callback);
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //    callback OK
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value result;
  napi_value call_argv[2];
  
  status = napi_call_function(env, global, callback, 0, call_argv, &result);
  if (status != napi_ok) {
    fprintf(stderr, "status = %d\n", status);
    napi_throw_error(env, nullptr, "napi_call_function FAIL");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return;
  }
  _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
}

napi_value Randomx_context_init(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(4)
  FN_ARG_THIS(Randomx_context)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_BUF_VAL(data_src)
  FN_ARG_I32(thread_count)
  FN_ARG_U32(flags)
  napi_value callback = argv[arg_idx];
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Worker_ctx_Randomx_context_init* worker_ctx = new Worker_ctx_Randomx_context_init;
  worker_ctx->err = nullptr;
  worker_ctx->data_src_ref = nullptr;
  status = napi_create_reference(env, callback, 1, &worker_ctx->callback_reference);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_helper_error_cb(env, "Unable to create reference for callback. napi_create_reference", callback);
    delete worker_ctx;
    return ret_dummy;
  }
  
  // NOTE no free utf8 string
  napi_value async_resource_name;
  status = napi_create_string_utf8(env, "dummy", 5, &async_resource_name);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create value async_resource_name set to 'dummy'");
    delete worker_ctx;
    return ret_dummy;
  }
  
  worker_ctx->_this = _this;
  worker_ctx->data_src = data_src;
  worker_ctx->data_src_len = data_src_len;
  status = napi_create_reference(env, data_src_val, 1, &worker_ctx->data_src_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for data_src. napi_create_reference");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return ret_dummy;
  }
  worker_ctx->thread_count = thread_count;
  worker_ctx->flags = flags;
  
  status = napi_create_async_work(
    env,
    nullptr,
    async_resource_name,
    _execute_Randomx_context_init,
    _complete_Randomx_context_init,
    (void*)worker_ctx,
    &worker_ctx->work
  );
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "napi_create_async_work fail");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  status = napi_queue_async_work(env, worker_ctx->work);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "napi_queue_async_work fail");
    _worker_ctx_Randomx_context_init_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  
  return ret_dummy;
}
