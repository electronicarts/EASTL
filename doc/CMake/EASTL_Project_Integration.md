## Using EASTL in your own projects

This page describes the steps needed to use EASTL in your own projects

## Setting up your project

### Using CMake

Add to your CMakeLists.txt:

```cmake
set(EASTL_ROOT_DIR C:/EASTL)

add_subdirectory(${EASTL_ROOT_DIR})

target_include_directories(... PUBLIC "${EASTL_ROOT_DIR}/include")
```

And then add the library into the linker 

```
target_link_libraries(... ${EASTL})
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
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags,
                     const char* file, int line) {
    return new uint8_t[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName,
                     int flags, unsigned debugFlags, const char* file, int line) {
    return new uint8_t[size];
}
```
