ReGEN
=====

A Graph Rewriting Tool for Plot Generation, uses Graph Grammars


### UNIT TESTING
To run unit testing, open a terminal in the build folder, execute `cmake .. "-DCMAKE_TOOLCHAIN_FILE=[PATH_TO]\vcpkg\scripts\buildsystems\vcpkg.cmake"` with the appropriate path, open your solution in Visual Studio and build the `Regen-cpp` target in release mode and then run `ctest -R parityTest`. 
If you wish to change the testing conditions, please note that there's currently no relation between the number of stories to generate (set in TestLayout XML) and the number of stories compared (hard coded in `parityTest.bat`). Don't forget to match these numbers if needed.