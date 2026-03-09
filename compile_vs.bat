@echo off
echo Compiling Space Battle Game with Visual Studio...
echo.

REM Set SFML path (modify if SFML is installed elsewhere)
set SFML_PATH=C:\SFML

REM Compile with Visual Studio cl.exe (if using VS Developer Command Prompt)
echo Compiling with Visual Studio...
cl /EHsc /std:c++17 /I include /I "%SFML_PATH%/include" src/*.cpp /link /LIBPATH:"%SFML_PATH%/lib" sfml-graphics.lib sfml-window.lib sfml-system.lib /out:SpaceBattleGame.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    echo Run SpaceBattleGame.exe to play the game.
) else (
    echo.
    echo Compilation failed!
    echo Make sure you are running from Visual Studio Developer Command Prompt.
    echo Also verify SFML is installed and paths are correct.
)

pause
