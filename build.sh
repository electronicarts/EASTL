build_folder=build
mkdir $build_folder
pushd $build_folder
cmake .. -DEASTL_BUILD_TESTS:BOOL=ON -DEASTL_BUILD_BENCHMARK:BOOL=OFF
cmake --build . --config Release
# cmake --build . --config Debug
# cmake --build . --config RelWithDebInfo
# cmake --build . --config MinSizeRel
pushd test
ctest -C Release -V
# ctest -C Debug
# ctest -C RelWithDebInfo
# ctest -C MinSizeRel
popd
popd
