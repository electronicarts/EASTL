cc_library(
    name = "eastdc",
    srcs = glob([
        "source/**/*.cpp",
    ]),
    hdrs = glob([
        "include/EAStdC/**/*.h",
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
        "//test/packages/EAAssert:eaassert",
        "//test/packages/EABase:eabase",
    ],
)
