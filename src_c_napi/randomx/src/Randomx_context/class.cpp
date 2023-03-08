#pragma once

u32 Randomx_context_tag = 1;
class Randomx_context {
  public:
  u32   _class_tag = Randomx_context_tag;
  bool  _deleted = false;
  bool is_dataset_copy;
  u32 flags;
  u32 init_idx;
  struct randomx_dataset*  dataset_ptr;
  struct randomx_cache*    cache_ptr;
  randomx_vm*       vm_ptr;
  const u8*         tmpEntropy;
  
  napi_ref _wrapper;
  void free() {
    if (this->_deleted) return;
    this->_deleted = true;
  }
};
void Randomx_context_destructor(napi_env env, void* native_object, void* /*finalize_hint*/) {
  Randomx_context* _this = static_cast<Randomx_context*>(native_object);
  _this->free();
  delete _this;
}

napi_value Randomx_context_constructor(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value _js_this;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &_js_this, nullptr);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to create class Randomx_context");
    return nullptr;
  }
  
  Randomx_context* _this = new Randomx_context();
  
  
  status = napi_wrap(env, _js_this, _this, Randomx_context_destructor, nullptr /* finalize_hint */, &_this->_wrapper);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to napi_wrap for class Randomx_context");
    return nullptr;
  }
  
  return _js_this;
}
