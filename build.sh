build_folder=build

rm -rf $build_folder
mkdir $build_folder
pushd $build_folder

cmake .. -DEASTL_BUILD_TESTS:BOOL=OFF -DEASTL_BUILD_BENCHMARK:BOOL=ON
cmake --build . --config Release -- -j 32 

cmake .. -DEASTL_BUILD_TESTS:BOOL=OFF -DEASTL_BUILD_BENCHMARK:BOOL=OFF
cmake --build . --config Release -- -j 32 
 
cmake .. -DEASTL_BUILD_TESTS:BOOL=ON -DEASTL_BUILD_BENCHMARK:BOOL=OFF
cmake --build . --config Release -- -j 32 
 
cmake .. -DEASTL_BUILD_TESTS:BOOL=ON -DEASTL_BUILD_BENCHMARK:BOOL=ON
cmake --build . --config Release -- -j 32 
cmake --build . --config Debug -- -j 32
cmake --build . --config RelWithDebInfo -- -j 32
cmake --build . --config MinSizeRel -- -j 32
pushd test
ctest -C Release -V
ctest -C Debug -V
ctest -C RelWithDebInfo -V
ctest -C MinSizeRel -V
popd
popd
