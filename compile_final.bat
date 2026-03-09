@echo off
echo Compiling Space Battle Game...
echo.

REM Set SFML path to your installation
set SFML_PATH=C:\SFML-3.0.2

REM Compile with Visual Studio
echo Compiling with Visual Studio...
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" /EHsc /std:c++17 /I include /I "%SFML_PATH%/include" src/*.cpp /link /LIBPATH:"%SFML_PATH%/lib" sfml-graphics.lib sfml-window.lib sfml-system.lib /out:SpaceBattleGame.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo 🎮 Compilation successful!
    echo 🚀 Run SpaceBattleGame.exe to play the game!
) else (
    echo.
    echo ❌ Compilation failed!
)

pause
