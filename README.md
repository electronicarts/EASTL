# EA Standard Template Library

[![Build Status](https://travis-ci.org/electronicarts/EASTL.svg?branch=master)](https://travis-ci.org/electronicarts/EASTL)  

EASTL stands for Electronic Arts Standard Template Library. It is a C++ template library of containers, algorithms, and iterators useful for runtime and tool development across multiple platforms. It is a fairly extensive and robust implementation of such a library and has an emphasis on high performance above all other considerations.


## Usage

If you are familiar with the C++ STL or have worked with other templated container/algorithm libraries, you probably don't need to read this. If you have no familiarity with C++ templates at all, then you probably will need more than this document to get you up to speed. In this case, you need to understand that templates, when used properly, are powerful vehicles for the ease of creation of optimized C++ code. A description of C++ templates is outside the scope of this documentation, but there is plenty of such documentation on the Internet.

EASTL is suitable for any tools and shipping applications where the functionality of EASTL is useful. Modern compilers are capable of producing good code with templates and many people are using them in both current generation and future generation applications on multiple platforms from embedded systems to servers and mainframes.

## Package Managers

You can download and install EASTL using the [Conan](https://github.com/conan-io/conan) package manager:

    conan install eastl/3.15.00@

The EASTL package in conan is kept up to date by Conan team members and community contributors. If the version is out-of-date, please [create an issue or pull request](https://github.com/conan-io/conan-center-index) on the Conan Center Index repository.


You can download and install EASTL using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    vcpkg install eastl

The EASTL port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.


## Documentation

Please see [EASTL Introduction](doc/Introduction.md).


## Compiling sources

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details on compiling and testing the source.

## Credits And Maintainers

EASTL was created by Paul Pedriana and he maintained the project for roughly 10 years.  

EASTL was subsequently maintained by Roberto Parolin for more than 8 years.
He was the driver and proponent for getting EASTL opensourced.
Rob was a mentor to all members of the team and taught us everything we ever wanted to know about C++ spookyness.

Max Winkler is the current EASTL owner and primary maintainer within EA and is responsible for the open source repository.

Significant EASTL contributions were made by (in alphabetical order):

* Avery Lee
* Colin Andrews
* JP Flouret
* Liam Mitchell
* Matt Newport
* Max Winkler
* Paul Pedriana
* Roberto Parolin
* Simon Everett

## Contributors

## Projects And Products Using EASTL

* Frostbite - Electronic Arts - [https://www.ea.com/frostbite]

## License

Modified BSD License (3-Clause BSD license) see the file LICENSE in the project root.
