{
  "targets": [
    {
      "target_name": "shakki",
      "sources": [
        "./src/index.cpp",
		"<!@(node -p \"require('fs').readdirSync('./src/shakki/shakki').filter(f=>f!=='main.cpp').map(f=>'src/shakki/shakki/'+f).join(' ')\")"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        ['OS!="win"', {
          # GCC/Clang: force-include the standard headers the sources rely on,
          # map the MSVC-only __forceinline keyword to inline, and keep C++
          # exceptions enabled (the engine uses try/catch).
          "cflags!": [ "-fno-exceptions" ],
          "cflags_cc!": [ "-fno-exceptions" ],
          "cflags": [ "-mcmodel=large" ],
          "cflags_cc": [
            "-mcmodel=large",
            "-D__forceinline=inline",
            "-include", "climits",
            "-include", "cstring",
            "-include", "cstdlib",
            "-include", "cctype",
            "-include", "cstdint"
          ]
        }],
        ['OS=="win"', {
          # MSVC: __forceinline is a native keyword, so no remap is needed.
          # /FI force-includes the same standard headers (node-gyp does not
          # apply the GCC cflags above under MSVC), /EHsc enables exceptions,
          # and /bigobj lets the large translation units exceed the default
          # section limit.
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
              "AdditionalOptions": [
                "/bigobj",
                "/FIclimits",
                "/FIcstring",
                "/FIcstdlib",
                "/FIcctype",
                "/FIcstdint"
              ]
            }
          }
        }]
      ]
    }
  ]
}
