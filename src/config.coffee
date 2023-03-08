module = @
require "fy"
require("events").EventEmitter.defaultMaxListeners = Infinity
global.arch = "node16-linux-x64"
argv = require("minimist")(process.argv.slice(2))
config = Object.assign(require("dotenv-flow").config({silent:!!global.is_fork}).parsed || {}, process.env)
for k,v of argv
  config[k.toUpperCase().split("-").join("_")] = v

bool = (name, default_value = "0", config_name = name.toUpperCase())->
  module[name] = !!+(config[config_name] ? default_value)

i32 = int  = (name, default_value = "0", config_name = name.toUpperCase())->
  module[name] = +(config[config_name] ? default_value)

f32 = f64  = (name, default_value = "0", config_name = name.toUpperCase())->
  module[name] = +(config[config_name] ? default_value)

str  = (name, default_value = "", config_name = name.toUpperCase())->
  module[name] = config[config_name] ? default_value

str_list  = (name, default_value = "", config_name = name.toUpperCase())->
  module[name] = (config[config_name] ? default_value).split(",").filter (t)->t != ""

# ###################################################################################################
bool "jit"
bool "full_mem"
bool "hard_aes"
bool "argon2_ssse3"
bool "secure"
bool "large_pages"
int  "packing_iter", 360
int  "work_item_count", 10000
int  "bench_sample_time", 1000
int  "bench_sample_count", 120
f32  "bench_ema_p", 0.95
int  "ctx_count", require('os').cpus().length
bool "thread_affinity"
