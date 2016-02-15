# EASTL Introduction

EASTL stands for Electronic Arts Standard Template Library. It is a C++ template library of containers, algorithms, and iterators useful for runtime and tool development across multiple platforms. It is a fairly extensive and robust implementation of such a library and has an emphasis on high performance above all other considerations.

## Intended Audience

This is a short document intended to provide a basic introduction to EASTL for those new to the concept of EASTL or STL. If you are familiar with the C++ STL or have worked with other templated container/algorithm libraries, you probably don't need to read this. If you have no familiarity with C++ templates at all, then you probably will need more than this document to get you up to speed. In this case you need to understand that templates, when used properly, are powerful vehicles for the ease of creation of optimized C++ code. A description of C++ templates is outside the scope of this documentation, but there is plenty of such documentation on the Internet. See the EASTL FAQ.html document for links to information related to learning templates and STL.

## EASTL Modules

EASTL consists primarily of containers, algorithms, and iterators. An example of a container is a linked list, while an example of an algorithm is a sort function; iterators are the entities of traversal for containers and algorithms. EASTL containers a fairly large number of containers and algorithms, each of which is a very clean, efficient, and unit-tested implementation. We can say with some confidence that you are not likely to find better implementations of these (commercial or otherwise), as these are the result of years of wisdom and diligent work. For a detailed list of EASTL modules, see EASTL Modules.html.

## EASTL Suitability

What uses are EASTL suitable for? Essentially any situation in tools and shipping applications where the functionality of EASTL is useful. Modern compilers are capable of producing good code with templates and many people are using them in both current generation and future generation applications on multiple platforms from embedded systems to servers and mainframes.

----------------------------------------------
End of document