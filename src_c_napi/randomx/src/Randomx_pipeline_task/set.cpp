void _Randomx_pipeline_task_set_impl(
  std::string*& err,
  Randomx_pipeline_task* _this,
  Randomx_context* feed_root_ctx,
  u8*          pack_key,
  size_t       pack_key_len,
  u8*          chunk_i,
  size_t       chunk_i_len,
  u8*          chunk_o,
  size_t       chunk_o_len,
  i32          packing_iter
) {
  _this->feed_root_ctx  = feed_root_ctx;
  _this->pack_key     = pack_key;
  _this->pack_key_len = pack_key_len;
  _this->chunk_i      = chunk_i;
  _this->chunk_i_len  = chunk_i_len;
  _this->chunk_o      = chunk_o;
  _this->packing_iter = packing_iter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//   sync
////////////////////////////////////////////////////////////////////////////////////////////////////
napi_value Randomx_pipeline_task_set_sync(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(5)
  FN_ARG_THIS(Randomx_pipeline_task)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_CLASS(Randomx_context, feed_root_ctx)
  FN_ARG_BUF(pack_key)
  FN_ARG_BUF(chunk_i)
  FN_ARG_BUF(chunk_o)
  FN_ARG_I32(packing_iter)
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  std::string *err = nullptr;
  _Randomx_pipeline_task_set_impl(err, _this, feed_root_ctx, pack_key, pack_key_len, chunk_i, chunk_i_len, chunk_o, chunk_o_len, packing_iter);
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
struct Worker_ctx_Randomx_pipeline_task_set {
  Randomx_pipeline_task* _this;
  Randomx_context* feed_root_ctx;
  napi_ref feed_root_ctx_ref;
  u8* pack_key;
  size_t pack_key_len;
  napi_ref pack_key_ref;
  u8* chunk_i;
  size_t chunk_i_len;
  napi_ref chunk_i_ref;
  u8* chunk_o;
  size_t chunk_o_len;
  napi_ref chunk_o_ref;
  i32 packing_iter;
  
  std::string* err;
  napi_ref callback_reference;
  napi_async_work work;
};

void _worker_ctx_Randomx_pipeline_task_set_clear(napi_env env, struct Worker_ctx_Randomx_pipeline_task_set* worker_ctx) {
  if (worker_ctx->err) {
    delete worker_ctx->err;
    worker_ctx->err = nullptr;
  }
  napi_status status;
  if (worker_ctx->feed_root_ctx_ref) {
    status = napi_delete_reference(env, worker_ctx->feed_root_ctx_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for feed_root_ctx");
      return;
    }
  }
  if (worker_ctx->pack_key_ref) {
    status = napi_delete_reference(env, worker_ctx->pack_key_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for pack_key");
      return;
    }
  }
  if (worker_ctx->chunk_i_ref) {
    status = napi_delete_reference(env, worker_ctx->chunk_i_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for chunk_i");
      return;
    }
  }
  if (worker_ctx->chunk_o_ref) {
    status = napi_delete_reference(env, worker_ctx->chunk_o_ref);
    if (status != napi_ok) {
      printf("status = %d\n", status);
      napi_throw_error(env, nullptr, "napi_delete_reference fail for chunk_o");
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

void _execute_Randomx_pipeline_task_set(napi_env env, void* _data) {
  struct Worker_ctx_Randomx_pipeline_task_set* worker_ctx = (struct Worker_ctx_Randomx_pipeline_task_set*)_data;
  _Randomx_pipeline_task_set_impl(worker_ctx->err, worker_ctx->_this, worker_ctx->feed_root_ctx, worker_ctx->pack_key, worker_ctx->pack_key_len, worker_ctx->chunk_i, worker_ctx->chunk_i_len, worker_ctx->chunk_o, worker_ctx->chunk_o_len, worker_ctx->packing_iter);
}

void _complete_Randomx_pipeline_task_set(napi_env env, napi_status execute_status, void* _data) {
  napi_status status;
  struct Worker_ctx_Randomx_pipeline_task_set* worker_ctx = (struct Worker_ctx_Randomx_pipeline_task_set*)_data;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //    prepare for callback (common parts)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value callback;
  status = napi_get_reference_value(env, worker_ctx->callback_reference, &callback);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to get referenced callback (napi_get_reference_value)");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return;
  }
  status = napi_delete_reference(env, worker_ctx->callback_reference);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to delete reference callback_reference");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return;
  }
  
  napi_value global;
  status = napi_get_global(env, &global);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create return value global (napi_get_global)");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
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
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
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
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return;
  }
  _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
}

napi_value Randomx_pipeline_task_set(napi_env env, napi_callback_info info) {
  FN_ARG_HEAD(6)
  FN_ARG_THIS(Randomx_pipeline_task)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  FN_ARG_CLASS_VAL(Randomx_context, feed_root_ctx)
  FN_ARG_BUF_VAL(pack_key)
  FN_ARG_BUF_VAL(chunk_i)
  FN_ARG_BUF_VAL(chunk_o)
  FN_ARG_I32(packing_iter)
  napi_value callback = argv[arg_idx];
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Worker_ctx_Randomx_pipeline_task_set* worker_ctx = new Worker_ctx_Randomx_pipeline_task_set;
  worker_ctx->err = nullptr;
  worker_ctx->feed_root_ctx_ref = nullptr;
  worker_ctx->pack_key_ref = nullptr;
  worker_ctx->chunk_i_ref = nullptr;
  worker_ctx->chunk_o_ref = nullptr;
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
  worker_ctx->feed_root_ctx = feed_root_ctx;
  status = napi_create_reference(env, feed_root_ctx_val, 1, &worker_ctx->feed_root_ctx_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for feed_root_ctx. napi_create_reference");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  worker_ctx->pack_key = pack_key;
  worker_ctx->pack_key_len = pack_key_len;
  status = napi_create_reference(env, pack_key_val, 1, &worker_ctx->pack_key_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for pack_key. napi_create_reference");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  worker_ctx->chunk_i = chunk_i;
  worker_ctx->chunk_i_len = chunk_i_len;
  status = napi_create_reference(env, chunk_i_val, 1, &worker_ctx->chunk_i_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for chunk_i. napi_create_reference");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  worker_ctx->chunk_o = chunk_o;
  worker_ctx->chunk_o_len = chunk_o_len;
  status = napi_create_reference(env, chunk_o_val, 1, &worker_ctx->chunk_o_ref);
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "Unable to create reference for chunk_o. napi_create_reference");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  worker_ctx->packing_iter = packing_iter;
  
  status = napi_create_async_work(
    env,
    nullptr,
    async_resource_name,
    _execute_Randomx_pipeline_task_set,
    _complete_Randomx_pipeline_task_set,
    (void*)worker_ctx,
    &worker_ctx->work
  );
  if (status != napi_ok) {
    printf("status = %d\n", status);
    napi_throw_error(env, nullptr, "napi_create_async_work fail");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  status = napi_queue_async_work(env, worker_ctx->work);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "napi_queue_async_work fail");
    _worker_ctx_Randomx_pipeline_task_set_clear(env, worker_ctx);
    return ret_dummy;
  }
  
  
  return ret_dummy;
}
