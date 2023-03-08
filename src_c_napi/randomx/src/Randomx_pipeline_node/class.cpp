#pragma once

u32 Randomx_pipeline_node_tag = 5;
class Randomx_pipeline_node {
  public:
  u32   _class_tag = Randomx_pipeline_node_tag;
  bool  _deleted = false;
  std::vector<Message_ring<Randomx_pipeline_task>*> pad_i_list;
  std::vector<Message_ring<Randomx_pipeline_task>*> pad_o_list;
  u32 fn_index;
  bool thread_set;
  u32 heartbeat_counter;
  bool (*fn)(Randomx_pipeline_node*, Randomx_pipeline_thread*);
  
  napi_ref _wrapper;
  void free() {
    if (this->_deleted) return;
    this->_deleted = true;
  }
};
void Randomx_pipeline_node_destructor(napi_env env, void* native_object, void* /*finalize_hint*/) {
  Randomx_pipeline_node* _this = static_cast<Randomx_pipeline_node*>(native_object);
  _this->free();
  delete _this;
}

napi_value Randomx_pipeline_node_constructor(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value _js_this;
  status = napi_get_cb_info(env, info, nullptr, nullptr, &_js_this, nullptr);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to create class Randomx_pipeline_node");
    return nullptr;
  }
  
  Randomx_pipeline_node* _this = new Randomx_pipeline_node();
  
  
  status = napi_wrap(env, _js_this, _this, Randomx_pipeline_node_destructor, nullptr /* finalize_hint */, &_this->_wrapper);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Unable to napi_wrap for class Randomx_pipeline_node");
    return nullptr;
  }
  
  return _js_this;
}
