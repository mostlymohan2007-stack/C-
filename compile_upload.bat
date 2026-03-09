@echo off
echo Compiling Space Battle Game with User Upload Support...
echo.

REM Set SFML path
set SFML_PATH=C:\SFML-2.6.1-windows-vc17-64-bit\SFML-2.6.1

REM Compile with Visual Studio
echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Compiling with user upload support...
cl /EHsc /std:c++17 /I include /I "%SFML_PATH%\include" src\main.cpp src\Game_upload.cpp src\Player_upload.cpp src\Enemy_upload.cpp src\Bullet_upload.cpp src\Collision.cpp /link /LIBPATH:"%SFML_PATH%\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib /out:SpaceBattleUpload.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo 🎮 Compilation successful!
    echo 🚀 Run SpaceBattleUpload.exe to play game!
    echo.
    echo ✨ NEW FEATURE: User Image Upload Support!
    echo 📁 Place your custom images in game folder:
    echo    - user_player.png (40x40)
    echo    - user_enemy.png (30x30) 
    echo    - user_bullet.png (5x10)
    echo.
    echo Copying SFML DLLs...
    copy "%SFML_PATH%\bin\*.dll" . >nul 2>&1
    echo DLLs copied successfully!
) else (
    echo.
    echo ❌ Compilation failed!
)

pause
