@echo off
echo Compiling with SFML 2.6.1...
echo.

REM Set SFML path to your installation
set SFML_PATH=C:\SFML-2.6.1-windows-vc17-64-bit\SFML-2.6.1

REM Compile with Visual Studio
echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Compiling...
cl /EHsc /std:c++17 /I include /I "%SFML_PATH%\include" src\main.cpp src\Game.cpp src\Player.cpp src\Enemy.cpp src\Bullet.cpp src\Collision.cpp /link /LIBPATH:"%SFML_PATH%\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib /out:SpaceBattleGame.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo 🎮 Compilation successful!
    echo 🚀 Run SpaceBattleGame.exe to play the game!
    echo.
    echo Copying SFML DLLs...
    copy "%SFML_PATH%\bin\*.dll" . >nul 2>&1
    echo DLLs copied successfully!
) else (
    echo.
    echo ❌ Compilation failed!
)

pause
