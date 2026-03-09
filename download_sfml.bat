@echo off
echo Downloading SFML 2.6.1...
echo.

REM Download SFML 2.6.1 for Visual Studio 2022
echo Downloading SFML...
powershell -Command "Invoke-WebRequest -Uri 'https://www.sfml-dev.org/files/SFML-2.6.1-vc17-64-bit.zip' -OutFile 'SFML-2.6.1.zip'"

echo Extracting SFML...
powershell -Command "Expand-Archive -Path 'SFML-2.6.1.zip' -DestinationPath 'C:\' -Force"

echo Renaming folder...
ren "C:\SFML-2.6.1" "SFML"

echo Cleaning up...
del "SFML-2.6.1.zip"

echo.
echo SFML installation complete!
echo You can now run: compile_with_vs.bat
pause
