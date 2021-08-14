setlocal enabledelayedexpansion
set "pythonPath=%1"
set "pythonPath=%pythonPath:/=\%"
set "cppPath=%2"
set "cppPath=%cppPath:/=\%"

cd /D %pythonPath%
python %pythonPath%/ReGEN-Python/Main.py

cd /D %cppPath%/
START /WAIT ./Release/ReGen-Cpp.exe

FOR /L %%G IN (0,1,24) DO (
    fc %pythonPath%\Output\ParityTest\narrative%%G.txt %cppPath%\Output\ParityTest\narrative%%G.txt
    IF not !ERRORLEVEL! == 0 exit -1
)
exit 0
