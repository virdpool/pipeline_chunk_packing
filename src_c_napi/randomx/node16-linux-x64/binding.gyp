{
  "targets": [
    {
      "target_name": "module",
      "sources": [
        "../src/module.cpp"
      ],
      "cflags": [],
      "cflags_cc": [
        "-fexceptions"
      ],
      "link_settings": {
        "libraries": [
          "'<!(pwd)'/../../../src_c_lib/randomx/build_node16-linux-x64/librandomx.a",
          "-lpthread"
        ]
      },
      "include_dirs": [
        "../../../src_c_lib/randomx/repo/src",
        "../../../src_c_lib/arweave/repo/apps/arweave/c_src"
      ]
    }
  ]
}