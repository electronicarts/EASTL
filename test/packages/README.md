## Packages ReadMe 

The packages in this folder are dependencies required by the EASTL unit tests.  

The following packages are stubs of internal EA technology that allow our developers to write portable code on our supported platforms.  These packages are required dependencies so please do not invest time into improving them.  We will of course accept critical fixes for compiler errors, compiler warnings or any other issues that prevent the use of EASTL or prevent the running of its unit tests. 

	Stub Packages:
	* EAAssert
	* EAMain
	* EAStdC
	* EAThread

The following packages are full releases of our internal EA technology.  Please see the in package documentation for more details.  If you have ideas on how to improve these technologies please contact us before investing any time or resources. 

	Full Releases:
	* EABase
	* EATest


## Why did you create package stubs?

We were not prepared to open source all of EASTL's dependency code that provide platform abstraction at this time.  We have allowed for the possibility of open sourcing these technologies in the future but we can't guarantee that it will happen.


