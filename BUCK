cxx_library(
  name = 'eastl',
  header_namespace = 'EASTL',
  exported_headers = subdir_glob([
    ('include/EASTL', '**/*.h'),
  ]),
  srcs = glob([
    'source/**/*.cpp',
  ]),
  compiler_flags = [
    '-DEASTL_OPENSOURCE=1',
    '-DEASTL_THREAD_SUPPORT_AVAILABLE=0',
  ],
  licenses = [
    'LICENSE',
  ],
  visibility = [
    'PUBLIC',
  ],
  deps = [
    '//test/packages/EABase:eabase',
  ],
)

cxx_library(
  name = 'test',
  header_namespace = '',
  exported_headers = subdir_glob([
    ('test/source', '**/*.h'),
  ]),
  srcs = glob([
    'test/source/**/*.cpp',
  ], excludes = [
    'test/source/main.cpp',
  ]),
  compiler_flags = [
    '-DEASTL_OPENSOURCE=1',
    '-DEASTL_THREAD_SUPPORT_AVAILABLE=0',
  ],
  deps = [
    '//:eastl',
    '//test/packages/EAAssert:eaassert',
    '//test/packages/EABase:eabase',
    '//test/packages/EAMain:eamain',
    '//test/packages/EAStdC:eastdc',
    '//test/packages/EATest:eatest',
    '//test/packages/EAThread:eathread',
  ],
)

cxx_binary(
  name = 'test-runner',
  srcs = glob([
    'test/source/main.cpp',
  ]),
  compiler_flags = [
    '-DEASTL_OPENSOURCE=1',
    '-DEASTL_THREAD_SUPPORT_AVAILABLE=0',
  ],
  deps = [
    '//:test',
  ],
)

cxx_binary(
  name = 'benchmark',
  header_namespace = '',
  headers = subdir_glob([
    ('benchmark/source', '**/*.h'),
  ]),
  srcs = glob([
    'benchmark/source/**/*.cpp',
  ]),
  compiler_flags = [
    '-DEASTL_OPENSOURCE=1',
    '-DEASTL_THREAD_SUPPORT_AVAILABLE=0',
  ],
  deps = [
    '//:eastl',
    '//:test',
    '//test/packages/EAStdC:eastdc',
    '//test/packages/EATest:eatest',
  ],
)
