@echo off
title Compiling Space Battle Enhanced Edition
echo ============================================
echo   SPACE BATTLE - ENHANCED EDITION
echo   Compiling ...
echo ============================================
echo.

REM Setup Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1

set SFML_PATH=C:\SFML-2.6.1-windows-vc17-64-bit\SFML-2.6.1

echo Compiling source files...
cl /EHsc /O2 /W3 /std:c++17 ^
   /I"%SFML_PATH%\include" /I"include" ^
   main.cpp ^
   src/Game.cpp ^
   src/Player.cpp ^
   src/Enemy.cpp ^
   src/Bullet.cpp ^
   src/Collision.cpp ^
   src/PowerUp.cpp ^
   src/SoundManager.cpp ^
   /link /LIBPATH:"%SFML_PATH%\lib" ^
   sfml-graphics.lib ^
   sfml-window.lib ^
   sfml-system.lib ^
   sfml-audio.lib ^
   openal32.lib ^
   /SUBSYSTEM:CONSOLE ^
   /out:SpaceBattle.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ============================================
    echo  Compilation SUCCESSFUL!
    echo ============================================
    echo.

    echo Copying SFML DLLs...
    copy "%SFML_PATH%\bin\sfml-graphics-2.dll" . >nul 2>&1
    copy "%SFML_PATH%\bin\sfml-window-2.dll"   . >nul 2>&1
    copy "%SFML_PATH%\bin\sfml-system-2.dll"   . >nul 2>&1
    copy "%SFML_PATH%\bin\sfml-audio-2.dll"    . >nul 2>&1
    copy "%SFML_PATH%\bin\openal32.dll"        . >nul 2>&1

    echo.
    echo  Game Controls:
    echo    WASD / Arrow Keys .... Move
    echo    SPACE / CTRL / Z ..... Shoot
    echo    ESC .................. Exit
    echo.
    echo  Enemy Types:
    echo    Level 1: Alien Invaders (Green alien PNG sprites)
    echo    Level 2: Robot Drones   (Purple robot PNG sprites)
    echo    Level 3: Demon Overlords (Demon battleship PNG sprites)
    echo.
    echo  Power-Ups:
    echo    Gold Star ... Rapid Fire (triple shot, 5s)
    echo    Green Cross . Health Boost (+20%% HP)
    echo.
    echo  Sound Effects:
    echo    Laser shoot, enemy fire, explosions,
    echo    power-up pickup, shield hit, background music!
    echo.
    echo ============================================
    echo  READY TO PLAY! Running SpaceBattle.exe
    echo ============================================
    SpaceBattle.exe
) else (
    echo.
    echo ============================================
    echo  Compilation FAILED!
    echo  Check error messages above.
    echo ============================================
)

pause
