#ifndef TEXTURAS_JOGO_HPP
#define TEXTURAS_JOGO_HPP

#include "raylib.h"

// Centraliza o carregamento e descarregamento dos sprites do jogo.
// só tem as imagens aqui para carregar 1x só e não a cada frame
struct TexturasJogo
{
    Texture2D jogador{};
    Texture2D zumbi{};
    Texture2D zumbiMorto{};

    Texture2D grama{};
    Texture2D chaoCasa{};
    Texture2D parede{};
    Texture2D escada{};
    Texture2D janelaInteira{};
    Texture2D janelaQuebrada{};
    Texture2D barricada{};

    Texture2D madeira{};
    Texture2D tacoBeisebol{};
    Texture2D canoMetal{};
    Texture2D maos{};

    bool carregadas = false;

    static bool TexturaValida(const Texture2D &textura)
    {
        return textura.id != 0 && textura.width > 0 && textura.height > 0;
    }

    void Carregar()
    {
        if (carregadas)
            return;

        jogador = LoadTexture("assets/jogador.png");
        zumbi = LoadTexture("assets/zumbi.png");
        zumbiMorto = LoadTexture("assets/zumbi_morto.png");

        grama = LoadTexture("assets/grama.png");
        chaoCasa = LoadTexture("assets/chao_casa.png");
        parede = LoadTexture("assets/parede.png");
        escada = LoadTexture("assets/escada.png");
        janelaInteira = LoadTexture("assets/janela_inteira.png");
        janelaQuebrada = LoadTexture("assets/janela_quebrada.png");
        barricada = LoadTexture("assets/barricada.png");

        madeira = LoadTexture("assets/madeira.png");
        tacoBeisebol = LoadTexture("assets/taco_beisebol.png");
        canoMetal = LoadTexture("assets/cano_metal.png");
        maos = LoadTexture("assets/maos.png");

        carregadas = true;
    }

    void Descarregar()
    {
        if (!carregadas)
            return;

        if (TexturaValida(jogador))
            UnloadTexture(jogador);
        if (TexturaValida(zumbi))
            UnloadTexture(zumbi);
        if (TexturaValida(zumbiMorto))
            UnloadTexture(zumbiMorto);

        if (TexturaValida(grama))
            UnloadTexture(grama);
        if (TexturaValida(chaoCasa))
            UnloadTexture(chaoCasa);
        if (TexturaValida(parede))
            UnloadTexture(parede);
        if (TexturaValida(escada))
            UnloadTexture(escada);
        if (TexturaValida(janelaInteira))
            UnloadTexture(janelaInteira);
        if (TexturaValida(janelaQuebrada))
            UnloadTexture(janelaQuebrada);
        if (TexturaValida(barricada))
            UnloadTexture(barricada);

        if (TexturaValida(madeira))
            UnloadTexture(madeira);
        if (TexturaValida(tacoBeisebol))
            UnloadTexture(tacoBeisebol);
        if (TexturaValida(canoMetal))
            UnloadTexture(canoMetal);
        if (TexturaValida(maos))
            UnloadTexture(maos);

        carregadas = false;
    }
};

#endif
