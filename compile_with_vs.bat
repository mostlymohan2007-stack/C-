@echo off
echo Compiling with Visual Studio...
echo.

REM Set SFML path (modify if SFML is installed elsewhere)
set SFML_PATH=C:\SFML

REM Compile with Visual Studio using full path
echo Compiling with Visual Studio...
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" /EHsc /std:c++17 /I include /I "%SFML_PATH%/include" src/*.cpp /link /LIBPATH:"%SFML_PATH%/lib" sfml-graphics.lib sfml-window.lib sfml-system.lib /out:SpaceBattleGame.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    echo Run SpaceBattleGame.exe to play the game.
) else (
    echo.
    echo Compilation failed!
    echo Make sure SFML is installed at C:\SFML
)

pause
