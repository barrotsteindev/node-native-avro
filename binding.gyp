{
  "targets": [
    {
      "target_name": "native-avro",
      "sources": [ "<!@(ls -1 src/*.cpp)", "<!@(ls -1 src/*.c)"],
      "libraries": [ "-lavro" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "<(module_root_dir)/",
        "/usr/lib",
        "/usr/local/lib",
        "/include/"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ]
    }
  ]
}
