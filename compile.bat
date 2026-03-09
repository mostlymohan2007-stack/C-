@echo off
echo Compiling Space Battle Game...
echo.

REM Set SFML path (modify if SFML is installed elsewhere)
set SFML_PATH=C:\SFML

REM Compile with MinGW/g++ (if using MinGW)
echo Compiling with MinGW...
g++ -std=c++17 src/*.cpp -I include -I "%SFML_PATH%/include" -L "%SFML_PATH%/lib" -lsfml-graphics -lsfml-window -lsfml-system -o SpaceBattleGame.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    echo Run SpaceBattleGame.exe to play the game.
) else (
    echo.
    echo Compilation failed!
    echo Make sure SFML is installed and paths are correct.
)

pause
