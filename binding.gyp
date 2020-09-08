{
  "targets": [
    {
      "target_name": "electron_share_memory",
      "sources": [ ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "conditions": [
        ['OS=="win"', {
          "sources": [ "lib/electron-share-memory.cc", "lib/addon.cc" ],
        }]
      ]
    }
  ]
}
