How to build QT Samples application:
1. Edit custom.bat file. It is necessary to define following variables:
 - QTDIR path to where QT is installed
 - CASROOT path to where Open CasCade binaries are installed.
 - Add additional PATH to necessary products libraries like TBB and FREEIMAGE
   if they were used for Open CasCade building

2. Build the application:

On Windows:
 a. Generate project files with help of genproj.bat. For example:
 > genproj.bat vc141 win64 Debug

 b. Launch MS Visual Studio. For example:
 > msvc.bat vc141 win64 Debug

 Build the application with help of MS Visual Studio.

On Linux:
 Launch building of the application by make.sh script

Running the application:
On Windows:
 > run.bat vc141 win64 Debug

On Linux:
 > run.sh