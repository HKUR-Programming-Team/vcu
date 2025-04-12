Pre-requisite: if you are using a Windows Machine, you have to download Visual Studio and its c++ compiler, make, cmake...<BR>
Then, do the following in your powershell:<BR>
```
cd <project_directory>/UnitTests
mkdir build
cd build
cmake ..
cmake --build .
```

If you want to add new user library to the test cases, make sure that you:<BR>
1. In the CMakeLists.txt, add the path of the src file (i.e. cpp file) under file `GLOB SOURCES`. Only add the sources that are to be unit-tested. Do NOT add the sources that will be mocked (e.g. the Logger library)
2. Add an if else statement to not include hardware-dependent libraries. For example: <BR>
```
#ifndef MOCK
	#include <stm32f1xx.h>
    (Other libraries like ADCManager, CANManager, Logger...)
#else
	#include <Inc/MockLibraries.hpp>
#endif
```
To run the built executable, run the unit-tests.exe in the Debug folder by:
```
.\Debug\unit-tests.exe
```