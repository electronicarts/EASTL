## Contributing

Before you can contribute, EA must have a Contributor License Agreement (CLA) on file that has been signed by each contributor.
You can sign here: [Go to CLA](https://goo.gl/KPylZ3)

### Pull Request Policy

All code contributions to EASTL are submitted as [Github pull requests](https://help.github.com/articles/using-pull-requests/).  All pull requests will be reviewed by an EASTL maintainer according to the guidelines found in the next section.

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

EASTL uses CMake as its build system.

* Generate build scripts:
	* cmake -Byour_build_folder -H.
* Navigate to "your_build_folder"
* Build unit tests for "your_config":
	* cmake --build . --config your_config 
* Run the unit tests for "your_config":
	* ctest -C your_config

