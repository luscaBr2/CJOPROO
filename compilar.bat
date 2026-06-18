@echo off
REM Compila o jogo usando MinGW/G++ e a pasta local do raylib.
REM Rode este arquivo na raiz do projeto.

g++ -std=c++17 -Wall -Wextra ^
-Iinclude ^
-Iraylib-5.5_win64_mingw-w64/include ^
src/main.cpp src/Entidade.cpp src/Casa.cpp src/Jogador.cpp src/Zumbi.cpp src/Item.cpp src/Mundo.cpp ^
-o jogo.exe ^
-Lraylib-5.5_win64_mingw-w64/lib ^
-lraylib -lopengl32 -lgdi32 -lwinmm

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Falha ao compilar. Verifique se o g++ esta instalado e se a pasta raylib-5.5_win64_mingw-w64 esta na raiz do projeto.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Compilado com sucesso: jogo.exe
pause
