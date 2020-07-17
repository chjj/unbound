{
  "targets": [{
    "target_name": "node_unbound",
    "sources": [
      "./src/node_unbound.c"
    ],
    "conditions": [
      ["OS != 'mac' and OS != 'win'", {
        "cflags": [
          "-std=c99"
        ]
      }],
      ["OS == 'mac'", {
        "xcode_settings": {
          "GCC_C_LANGUAGE_STANDARD": "c99"
        }
      }],
      ["OS == 'win'", {
        "libraries": [
          "-lunbound.lib"
        ]
      }, {
        "libraries": [
          "-lunbound"
        ]
      }]
    ]
  }]
}
