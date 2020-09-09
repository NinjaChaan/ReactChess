{
  "targets": [
    {
      "target_name": "greet",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/test.cpp",
        "./src/index.cpp",
		"./src/HillClimb.cpp",
		"./src/shakki/shakki/*.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}