cc_library(
    name = "eatest",
    srcs = glob([
        "source/**/*.cpp",
    ]),
    hdrs = glob([
        "include/EATest/**/*.h",
        "include/EATest/**/*.inl",
    ]),
    copts = [
        "-DEASTL_OPENSOURCE=1",
        "-DEASTL_THREAD_SUPPORT_AVAILABLE=0",
    ],
    strip_include_prefix = "include",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//:eastl",
        "//test/packages/EABase:eabase",
        "//test/packages/EAMain:eamain",
        "//test/packages/EAStdC:eastdc",
        "//test/packages/EAThread:eathread",
    ],
)
