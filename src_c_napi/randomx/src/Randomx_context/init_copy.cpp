void _Randomx_context_init_copy_impl(
  std::string*& err,
  Randomx_context* _this,
  Randomx_context* src_ctx
) {
  _this->flags      = src_ctx->flags;
  _this->cache_ptr  = src_ctx->cache_ptr;
  _this->dataset_ptr= src_ctx->dataset_ptr;
  _this->is_dataset_copy = true;
  
  _this->vm_ptr = randomx_create_vm((randomx_flags)_this->flags, _this->cache_ptr, _this->dataset_ptr);
  if (!_this->vm_ptr) {
    err = new std::string("!vm_ptr randomx_create_vm fail");
    return;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//   sync
////////////////////////////////////////////////////////////////////////////////////////////////////
napi_value Randomx_context_init_copy_sync(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(1)
  FN_ARG_THIS(Randomx_context)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_CLASS(Randomx_context, src_ctx)
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  std::string *err = nullptr;
  _Randomx_context_init_copy_impl(err, _this, src_ctx);
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
struct Worker_ctx_Randomx_context_init_copy {
  Randomx_context* _this;
  Randomx_context* src_ctx;
  napi_ref src_ctx_ref;
  
  std::string* err;
  napi_ref callback_reference;
  napi_async_work work;
};

void _worker_ctx_Randomx_context_init_copy_clear(napi_env env, struct Worker_ctx_Randomx_context_init_copy* worker_ctx) {
  if (worker_ctx->err) {
    delete worker_ctx->err;
    worker_ctx->err = nullptr;
  }
  napi_status status;
  if (worker_ctx->src_ctx_ref) {
    status = napi_delete_reference(env, worker_ctx->src_ctx_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for src_ctx");
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

void _execute_Randomx_context_init_copy(napi_env env, void* _data) {
  struct Worker_ctx_Randomx_context_init_copy* worker_ctx = (struct Worker_ctx_Randomx_context_init_copy*)_data;
  _Randomx_context_init_copy_impl(worker_ctx->err, worker_ctx->_this, worker_ctx->src_ctx);
}

void _complete_Randomx_context_init_copy(napi_env env, napi_status execute_status, void* _data) {
  napi_status status;
  struct Worker_ctx_Randomx_context_init_copy* worker_ctx = (struct Worker_ctx_Randomx_context_init_copy*)_data;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //    prepare for callback (common parts)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value callback;
  status = napi_get_reference_value(env, worker_ctx->callback_reference, &callback);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to get referenced callback (napi_get_reference_value)");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return;
  }
  status = napi_delete_reference(env, worker_ctx->callback_reference);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to delete reference callback_reference");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return;
  }
  
  napi_value global;
  status = napi_get_global(env, &global);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create return value global (napi_get_global)");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
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
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
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
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return;
  }
  _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
}

napi_value Randomx_context_init_copy(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(2)
  FN_ARG_THIS(Randomx_context)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_CLASS_VAL(Randomx_context, src_ctx)
  napi_value callback = argv[arg_idx];
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Worker_ctx_Randomx_context_init_copy* worker_ctx = new Worker_ctx_Randomx_context_init_copy;
  worker_ctx->err = nullptr;
  worker_ctx->src_ctx_ref = nullptr;
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
  worker_ctx->src_ctx = src_ctx;
  status = napi_create_reference(env, src_ctx_val, 1, &worker_ctx->src_ctx_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for src_ctx. napi_create_reference");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  status = napi_create_async_work(
    env,
    nullptr,
    async_resource_name,
    _execute_Randomx_context_init_copy,
    _complete_Randomx_context_init_copy,
    (void*)worker_ctx,
    &worker_ctx->work
  );
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "napi_create_async_work fail");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  status = napi_queue_async_work(env, worker_ctx->work);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "napi_queue_async_work fail");
    _worker_ctx_Randomx_context_init_copy_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  
  return ret_dummy;
}
