## Using EASTL in your own projects

This page describes the steps needed to use EASTL in your own projects

## Setting up your project

### Using CMake

Add to your CMakeLists.txt:

```cmake
set(EASTL_ROOT_DIR C:/EASTL)
include_directories (${EASTL_ROOT_DIR}/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAAssert/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EABase/include/Common)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAMain/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EATest/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAThread/include)
set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)
add_custom_target(NatVis SOURCES ${EASTL_ROOT_DIR}/doc/EASTL.natvis)
```

And then add the library into the linker 

```
target_link_libraries(... ${EASTL_LIBRARY})
```

### Using Visual Studio

Using Visual Studio projecs directly you will need do the following steps:
- Add the include paths
- Add the library path
- Add the library dependency
- Add natvis (optional)

> Note that in the examples below ${EASTL_ROOT_DIR} is the folder in which you stored EASTL. You could create an environment variable for this.

#### Add the include paths

Add the following paths to your C/C++ -> General -> Additional include directories:
```
${EASTL_ROOT_DIR}/include
${EASTL_ROOT_DIR}/test/packages/EAAssert/include
${EASTL_ROOT_DIR}/test/packages/EABase/include/Common
${EASTL_ROOT_DIR}/test/packages/EAMain/include)
${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
${EASTL_ROOT_DIR}/test/packages/EATest/include)
${EASTL_ROOT_DIR}/test/packages/EAThread/include)
```

#### Add the library path

Add the following library path to your Linker -> General -> Additional Library Directories:
```
${EASTL_ROOT_DIR}/build/$(Configuration)
```

#### Add the library dependency

Either add the following library to your Linker -> Input -> Additional Dependencies
```
EASTL.lib
```
Or in code use the following:
```
#pragma comment(lib, "EASTL.lib")
```

#### Add natvis (optional)

> Adding the natvis file to your project allows the debugger to use custom visualizers for the eastl data types. This greatly enhances the debugging experience.

Add the natvis file anywhere in your solution:

```
Right-click your project: Add -> Existing item and then add the following file:
${EASTL_ROOT_DIR}/doc/EASTL.natvis
```

## Setting up your code

### Overloading operator new[]

EASTL requires you to have an overload for the operator new[], here is an example that just forwards to global new[]:

```c
void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}
```
