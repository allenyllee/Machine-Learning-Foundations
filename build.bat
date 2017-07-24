@echo off
set TL_PATH=C:\MinGW\bin
set PATH=%TL_PATH%;%PATH%
::cd %CD%
gcc %1 -o %1.exe
::gcc %1 -o %1.exe
echo "finished"