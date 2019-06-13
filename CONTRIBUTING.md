## Contributing

Before you can contribute, EA must have a Contributor License Agreement (CLA) on file that has been signed by each contributor.
You can sign here: [Go to CLA](https://electronicarts.na1.echosign.com/public/esignWidget?wid=CBFCIBAA3AAABLblqZhByHRvZqmltGtliuExmuV-WNzlaJGPhbSRg2ufuPsM3P0QmILZjLpkGslg24-UJtek*)

### Pull Request Policy

All code contributions are submitted as [Github pull requests](https://help.github.com/articles/using-pull-requests/).  All pull requests will be reviewed by a maintainer according to the guidelines found in the next section.

Your pull request should:

* merge cleanly
* come with tests
	* tests should be minimal and stable
	* fail before your fix is applied
* pass the test suite
* code formatting is encoded in clang format
	* limit using clang format on new code
	* do not deviate from style already established in the files


### Running the Unit Tests

EAAssert uses CMake as its build system.

* Create and navigate to "your_build_folder":
	* mkdir your_build_folder && cd your_build_folder
* Generate build scripts:
	* cmake source_folder -DEABASE_BUILD_TESTS:BOOL=ON
* Build unit tests for "your_config":
	* cmake --build . --config your_config
* Run the unit tests for "your_config" from the test folder:
	* cd test && ctest -C your_config


Here is an example batch file.
```batch
set build_folder=out
mkdir %build_folder%
pushd %build_folder%
call cmake .. -DEABASE_BUILD_TESTS:BOOL=ON
call cmake --build . --config Release
call cmake --build . --config Debug
call cmake --build . --config RelWithDebInfo
call cmake --build . --config MinSizeRel
pushd test
call ctest -C Release
call ctest -C Debug
call ctest -C RelWithDebInfo
call ctest -C MinSizeRel
popd
popd
```

Here is an example bash file
```bash
build_folder=out
mkdir $build_folder
pushd $build_folder
cmake .. -DEABASE_BUILD_TESTS:BOOL=ON
cmake --build . --config Release
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --build . --config MinSizeRel
pushd test
ctest -C Release
ctest -C Debug
ctest -C RelWithDebInfo
ctest -C MinSizeRel
popd
popd
```

