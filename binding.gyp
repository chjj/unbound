{
  "targets": [{
    "target_name": "unbound",
    "sources": [
      "./src/node_unbound.c"
    ],
    "cflags": [
      "-Wall",
      "-Wextra",
      "-O3"
    ],
    "cflags_c": [
      "-std=c99"
    ],
    "libraries": [
      "-lunbound"
    ]
  }]
}
