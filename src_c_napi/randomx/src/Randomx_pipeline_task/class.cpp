#pragma once

u32 Randomx_pipeline_task_tag = 3;
class Randomx_pipeline_task {
  public:
  u32   _class_tag = Randomx_pipeline_task_tag;
  bool  _deleted = false;
  Randomx_context* feed_root_ctx;
  u8*     pack_key;
  size_t  pack_key_len;
  u8*     chunk_i;
  size_t  chunk_i_len;
  u8*     chunk_o;
  size_t  chunk_o_len;
  i32 packing_iter;
  i32 task_instance_uid;
  i32 task_uid;
  i32 curr_fn_index;
  
  napi_ref _wrapper;
  void free() {
    if (this->_deleted) return;
    this->_deleted = true;
  }
};
void Randomx_pipeline_task_destructor(napi_env env, void* native_object, void* /*finalize_hint*/) {
  Randomx_pipeline_task* _this = static_cast<Randomx_pipeline_task*>(native_object);
  _this->free();
  delete _this;
}

napi_value Randomx_pipeline_task_constructor(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value _js_this;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &_js_this, nullptr);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to create class Randomx_pipeline_task");
    return nullptr;
  }
  
  Randomx_pipeline_task* _this = new Randomx_pipeline_task();
  _this->task_instance_uid = ++randomx_pipeline_global_task_instance_uid;
  
  status = napi_wrap(env, _js_this, _this, Randomx_pipeline_task_destructor, nullptr /* finalize_hint */, &_this->_wrapper);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to napi_wrap for class Randomx_pipeline_task");
    return nullptr;
  }
  
  return _js_this;
}
