exports_files(["LICENSE"])

cc_library(
    name = "eastl",
    srcs = glob([
        "source/**/*.cpp",
    ]),
    hdrs = glob([
        "include/EASTL/**/*.h",
    ]),
    copts = [
        "-D_CHAR16T",
        "-D_CRT_SECURE_NO_WARNINGS",
        "-D_SCL_SECURE_NO_WARNINGS",
        "-DEASTL_OPENSOURCE=1",
    ],
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//test/packages/EABase:eabase",
    ],
)

cc_library(
    name = "test",
    srcs = glob(
        [
            "test/source/**/*.cpp",
            "test/source/**/*.h",
        ],
        exclude = [
            "test/source/main.cpp",
        ],
    ),
    hdrs = glob(
        [
            "test/source/**/*.h",
        ],
    ),
    copts = [
        "-DEASTL_OPENSOURCE=1",
        "-DEASTL_THREAD_SUPPORT_AVAILABLE=0",
    ],
    strip_include_prefix = "test/source",
    textual_hdrs = glob([
        "test/source/**/*.inl",
    ]),
    deps = [
        "//:eastl",
        "//test/packages/EAAssert:eaassert",
        "//test/packages/EABase:eabase",
        "//test/packages/EAMain:eamain",
        "//test/packages/EAStdC:eastdc",
        "//test/packages/EATest:eatest",
        "//test/packages/EAThread:eathread",
    ],
)

cc_binary(
    name = "test-runner",
    srcs = glob([
        "test/source/main.cpp",
    ]),
    copts = [
        "-DEASTL_OPENSOURCE=1",
        "-DEASTL_THREAD_SUPPORT_AVAILABLE=0",
    ],
    deps = [
        "//:test",
    ],
)

cc_binary(
    name = "benchmark",
    srcs = glob([
        "benchmark/source/**/*.cpp",
        "benchmark/source/**/*.h",
    ]),
    copts = [
        "-DEASTL_OPENSOURCE=1",
        "-DEASTL_THREAD_SUPPORT_AVAILABLE=0",
    ],
    deps = [
        "//:eastl",
        "//:test",
        "//test/packages/EAStdC:eastdc",
        "//test/packages/EATest:eatest",
    ],
)
