fs = require "fs"
path = __dirname+"/randomx.node"
module.exports = if fs.existsSync path
  require path
else
  require "../src_c_napi/randomx"
