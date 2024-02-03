{
  "targets": [
    {
      "target_name": "shakki",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/index.cpp",
		"<!@(node -p \"require('fs').readdirSync('./src/shakki/shakki').map(f=>'src/shakki/shakki/'+f).join(' ')\")"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}