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
        'include_dirs': [
          'C:/msys64/mingw64/include'
        ],
        'library_dirs': [
          'C:/msys64/mingw64/lib',
        ],
        "libraries": [
          "-llibunbound.dll.a"
        ]
      }, {
        "libraries": [
          "-lunbound"
        ]
      }]
    ]
  }]
}
