#include "raylib.h"
#include "Mundo.hpp"

int main()
{
    // Configuração básica da janela do jogo.
    const int larguraTela = 1200;
    const int alturaTela = 800;

    InitWindow(larguraTela, alturaTela, "Lar Doce Lar");
    SetTargetFPS(60);

    // O bloco garante que Mundo descarregue as texturas antes de CloseWindow().
    {
        Mundo mundo;

        // roda enquanto a janela não for fechada
        while (!WindowShouldClose())
        {
            float delta = GetFrameTime();

            mundo.Atualizar(delta);

            BeginDrawing();
            mundo.Desenhar();
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
