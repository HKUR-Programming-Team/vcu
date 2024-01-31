Pre-requisite: if you are using a Windows Machine, you have to download Visual Studio and its c++ compiler, make, cmake...<BR>
Create a folder called build<BR>
Then, do the following in your cmd:<BR>
cmake ..<BR>
cmake --build .<BR><BR>

If you want to add new user library to the test cases, make sure that you:<BR>
1. In the CMakeLists.txt, add the path of the src file (i.e. cpp file) under file(GLOB SOURCES
2. Add an if else statement to not include hardware-dependent libraries. For example <BR>
#ifndef MOCK<BR>
	#include <stm32f1xx.h><BR>
    (Other libraries like ADCManager, CANManager, Logger...)<BR>
#else<BR>
	#include <Inc/MockLibraries.hpp><BR>
#endif<BR><BR>
To run the built executable, run the unit-tests.exe in the Debug folder
