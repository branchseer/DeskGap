{
  'target_defaults': {
    'target_conditions': [
      ['OS == "win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': [
                '/std:c++17',
                '/EHa', '/MD'
              ]
            }
          },
      }],
      ['OS == "mac"', {
        'xcode_settings': {
          'MACOSX_DEPLOYMENT_TARGET': '10.10',
          'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
          'CLANG_CXX_LIBRARY': 'libc++',
          'GCC_C_LANGUAGE_STANDARD': 'c11'
        }
      }],
      ['OS == "linux"', {
        'cflags': [ '-fexceptions' ],
        'cflags_cc': [ '-fexceptions', '-std=c++17' ],
        'cflags_cc!': [ '-fno-rtti' ]
      }]
    ],
  },
  'targets': [
    {
      'target_name': 'deskgap_native',
      'include_dirs': ['<!@(node -p "require(\'node-addon-api\').include")'],
      'dependencies': ['<!(node -p "require(\'node-addon-api\').gyp")'],
      'defines': [ 'NAPI_CPP_EXCEPTIONS', 'NAPI_EXPERIMENTAL' ],
      'sources': [
        'src/index.cc',
        'src/dispatch/node_dispatch.cc',
        'src/dispatch/ui_dispatch.cc',
        'src/app/app_wrap.cc',
        'src/menu/menu_wrap.cc',
        'src/window/browser_window_wrap.cc',
        'src/webview/webview_wrap.cc',
        'src/shell/shell_wrap.cc',
        'src/system_preferences/system_preferences_wrap.cc',
        'src/dialog/dialog_wrap.cc',
      ],
      'dependencies': [ 'deskgap_native_platform' ],

      'conditions': [
        ['OS =="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': [

              ]
            }
          },
        }],
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_LDFLAGS': [
              '-ObjC'
            ],
            'GCC_ENABLE_CPP_EXCEPTIONS': 'true',
          }
        }]
      ],
    },
    {
      'target_name': 'deskgap_native_platform',
      'type': 'static_library',
      
      'conditions': [
        ['OS == "win"', {
          "sources": [
            "src/win/ui_dispatch_platform.cpp",
            "src/win/util/control_geometry.cpp",
            "src/win/app.cpp",
            "src/win/menu.cpp",
            "src/win/BrowserWindow.cpp",
            "src/win/webview.cpp",
            "src/win/shell.cpp",
            "src/win/dialog.cpp",
          ],
          "msbuild_settings": {
            "ClCompile": {
              "CompileAsManaged": "true",
              "ExceptionHandling": "Async",
              "AdditionalOptions": [
                '/AI"<(module_root_dir)\\..\\deps\"',
                '/FU mscorlib.dll',
                '/FU System.dll',
                '/FU System.Drawing.dll',
                '/FU System.Windows.Forms.dll',
                '/FU "<!(echo %ProgramFiles(x86)%)\\Reference Assemblies\\Microsoft\\Framework\\.NETFramework\\v4.7\\System.Net.Http.dll"',
                '/FU Microsoft.Toolkit.Forms.UI.Controls.WebView.dll',
                '/std:c++17'
              ]
            }
          },
          "dependencies": [ 'deskgap_native_unmanaged_win' ]
        }],

        ['OS == "mac"', {
          "sources": [
            "src/mac/ui_dispatch_platform.mm",
            "src/mac/util/NSScreen+Geometry.m",
            "src/mac/app.mm",
            "src/mac/menu.mm",
            "src/mac/BrowserWindow.mm",
            "src/mac/webview.mm",
            "src/mac/shell.mm",
            "src/mac/system_preferences.mm",
            "src/mac/dialog.mm",
            "src/mac/cocoa/DeskGapWindow.m",
            "src/mac/cocoa/DeskGapLocalURLSchemeHandler.mm"
          ],
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-fobjc-arc',
            ],
          },
          'link_settings' : {
            'libraries' : [
              '-framework Cocoa',
              '-framework WebKit'
            ]
          }
        }],

        ['OS == "linux"', {
          'sources': [
            'src/gtk/app.cpp',
            'src/gtk/ui_dispatch_platform.cpp',
            'src/gtk/system_preferences.cpp',
            "src/gtk/menu.cpp",
            "src/gtk/BrowserWindow.cpp",
            "src/gtk/webview.cpp",
            "src/gtk/shell.cpp",
            "src/gtk/dialog.cpp",
          ],
          'cflags': [
            '<!@(pkg-config --cflags gtkmm-3.0)',
            '<!@(pkg-config --cflags webkit2gtk-4.0)'
          ],
          'link_settings': {
            'libraries': [
              '<!@(pkg-config --libs gtkmm-3.0)',
              '<!@(pkg-config --libs webkit2gtk-4.0)'
            ]
          }
        }]
      ] # conditions
    },
    {
      'target_name': 'deskgap_native_unmanaged_win',
      'type': 'static_library',
      'conditions': [
        ['OS == "win"', {
          'sources': [
            "src/win/system_preferences.cpp",
          ]
        }]
      ]
    }
  ]
}
