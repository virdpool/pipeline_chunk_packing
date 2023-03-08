#pragma once

u32 Randomx_pipeline_thread_tag = 4;
class Randomx_pipeline_thread {
  public:
  u32   _class_tag = Randomx_pipeline_thread_tag;
  bool  _deleted = false;
  Randomx_context* curr_feed_ctx;
  Randomx_context* work_ctx;
  std::vector<Randomx_pipeline_node*> node_list;
  volatile bool started = false;
  volatile bool need_shutdown = false;
  u64 idle_delay_mcs = 100;
  THREAD_TYPE thread;
  u32 cpu_core_id = 0;
  
  napi_ref _wrapper;
  void free() {
    if (this->_deleted) return;
    this->_deleted = true;
  }
};
void Randomx_pipeline_thread_destructor(napi_env env, void* native_object, void* /*finalize_hint*/) {
  Randomx_pipeline_thread* _this = static_cast<Randomx_pipeline_thread*>(native_object);
  _this->free();
  delete _this;
}

napi_value Randomx_pipeline_thread_constructor(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value _js_this;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &_js_this, nullptr);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to create class Randomx_pipeline_thread");
    return nullptr;
  }
  
  Randomx_pipeline_thread* _this = new Randomx_pipeline_thread();
  
  
  status = napi_wrap(env, _js_this, _this, Randomx_pipeline_thread_destructor, nullptr /* finalize_hint */, &_this->_wrapper);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to napi_wrap for class Randomx_pipeline_thread");
    return nullptr;
  }
  
  return _js_this;
}
