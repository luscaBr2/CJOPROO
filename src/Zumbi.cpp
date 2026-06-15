#include "Zumbi.hpp"
#include "Utilidades.hpp"
#include <algorithm>

namespace
{
    // O balanceamento dos zumbis fica concentrado aqui para ser fácil de ajustar.
    // A vida e o dano começam baixos e crescem com o dia atual usando multiplicador.
    float DiaSeguro(int diaAtual)
    {
        return static_cast<float>(std::max(1, diaAtual));
    }

    float MultiplicadorPorDia(int diaAtual)
    {
        // Dia 1: 1.00x
        // Dia 2: 1.18x
        // Dia 3: 1.36x
        // A cada novo dia, vida e dano aumentam 18%.
        return 1.0f + (DiaSeguro(diaAtual) - 1.0f) * 0.18f;
    }

    float FatorPeriodo(bool noite)
    {
        // De dia, os zumbis têm apenas 25% da vida e do dano.
        // Isso equivale a ficarem 75% mais fracos durante o dia.
        return noite ? 1.0f : 0.25f;
    }

    float VidaDoZumbi(bool noite, int diaAtual)
    {
        const float vidaBase = 44.0f;
        return vidaBase * MultiplicadorPorDia(diaAtual) * FatorPeriodo(noite);
    }

    float DanoDoZumbi(bool noite, int diaAtual)
    {
        const float danoBase = 7.0f;
        return danoBase * MultiplicadorPorDia(diaAtual) * FatorPeriodo(noite);
    }

    float VelocidadeDoZumbi(bool noite, int diaAtual)
    {
        // A velocidade também cresce levemente, mas bem menos que vida/dano,
        // para o aumento de dificuldade não parecer injusto logo no começo.
        float dia = DiaSeguro(diaAtual);
        return noite ? 78.0f + (dia - 1.0f) * 3.0f
                     : 34.0f + (dia - 1.0f) * 1.2f;
    }
}

Zumbi::Zumbi(Vector2 posicaoInicial, bool noite, int diaAtual)
    : Entidade(posicaoInicial, 16.0f, VidaDoZumbi(noite, diaAtual)),
      velocidade(VelocidadeDoZumbi(noite, diaAtual)),
      dano(DanoDoZumbi(noite, diaAtual)),
      intervaloAtaque(noite ? 1.05f : 1.55f),
      tempoDesdeUltimoAtaque(0.0f),
      tempoCorpoNoChao(0.0f),
      tempoMaximoCorpoNoChao(4.0f)
{
}

void Zumbi::Atualizar(float delta, Jogador& jogador, Casa& casa, bool jogadorDentroDaCasa)
{
    if (!vivo)
    {
        MarcarTempoDeCorpo(delta);
        return;
    }

    tempoDesdeUltimoAtaque += delta;

    Vector2 alvo = jogador.ObterPosicao();
    int janelaAlvo = -1;
    bool zumbiDentroDaCasa = casa.PontoEstaDentroDaCasa(posicao);

    // Se o jogador está dentro da casa e o zumbi ainda está fora:
    // o zumbi SEMPRE avalia a janela mais próxima dele.
    // - Se a janela mais próxima estiver inteira, ele tenta quebrar essa janela.
    // - Se a janela mais próxima já estiver quebrada, ele atravessa por ela.
    // Isso evita o problema de todos os zumbis correrem para a primeira janela quebrada do mapa.
    if (jogadorDentroDaCasa && !zumbiDentroDaCasa)
    {
        janelaAlvo = casa.IndiceJanelaMaisProxima(posicao);

        if (janelaAlvo >= 0 && casa.JanelaQuebrada(janelaAlvo))
        {
            // Janela quebrada mais próxima: atravessa para um ponto já dentro da casa.
            alvo = casa.PontoInternoDepoisDaJanela(janelaAlvo, raio + 10.0f);
        }
        else if (janelaAlvo >= 0)
        {
            // Janela inteira mais próxima: vai até ela para atacar e quebrar.
            alvo = casa.CentroJanela(janelaAlvo);
        }
    }

    Vector2 direcao = NormalizarSeguro(Subtrair(alvo, posicao));
    Vector2 deslocamento = Multiplicar(direcao, velocidade * delta);
    casa.MoverComColisao(posicao, raio, deslocamento, true);

    if (jogadorDentroDaCasa && !zumbiDentroDaCasa && janelaAlvo >= 0)
    {
        float distanciaAteJanela = Distancia(posicao, casa.CentroJanela(janelaAlvo));

        if (distanciaAteJanela < 52.0f && tempoDesdeUltimoAtaque >= intervaloAtaque)
        {
            // Se a janela ainda está inteira, o zumbi bate nela.
            // Quando quebrar, a colisão da janela é removida em Casa::CirculoColideComObstaculo().
            if (!casa.JanelaQuebrada(janelaAlvo))
            {
                casa.DanoNaJanela(janelaAlvo, dano);
                tempoDesdeUltimoAtaque = 0.0f;
            }
        }
    }
    else
    {
        // Fora da casa, ou depois de invadir, zumbis perseguem e atacam diretamente o jogador.
        float distanciaJogador = Distancia(posicao, jogador.ObterPosicao());
        if (distanciaJogador <= raio + jogador.ObterRaio() + 6.0f && tempoDesdeUltimoAtaque >= intervaloAtaque)
        {
            jogador.ReceberDano(dano);
            tempoDesdeUltimoAtaque = 0.0f;
        }
    }
}

void Zumbi::Desenhar() const
{
    if (vivo)
    {
        // Zumbi vivo: círculo verde. Troque aqui por sprite depois.
        DrawCircleV(posicao, raio, GREEN);
        DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raio, DARKGREEN);

        // Barra de vida acima da cabeça.
        int largura = 38;
        int altura = 6;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y - raio - 14);
        float percentual = vidaMaxima > 0.0f ? vida / vidaMaxima : 0.0f;

        DrawRectangle(x, y, largura, altura, RED);
        DrawRectangle(x, y, static_cast<int>(largura * percentual), altura, LIME);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
    else
    {
        // Corpo temporário: fica alguns segundos e depois some.
        DrawCircleV(posicao, raio, DARKGREEN);
        DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raio, BLACK);
    }
}

bool Zumbi::PodeSerRemovido() const
{
    return !vivo && tempoCorpoNoChao >= tempoMaximoCorpoNoChao;
}

void Zumbi::MarcarTempoDeCorpo(float delta)
{
    tempoCorpoNoChao += delta;
}

void Zumbi::Desenhar(const TexturasJogo& texturas) const
{
    const Texture2D& texturaZumbi = vivo ? texturas.zumbi : texturas.zumbiMorto;

    if (TexturasJogo::TexturaValida(texturaZumbi))
    {
        Rectangle origem = {0.0f, 0.0f, static_cast<float>(texturaZumbi.width), static_cast<float>(texturaZumbi.height)};
        // Sprite maior para o zumbi ficar visível sem alterar a colisão.
        float tamanhoSprite = raio * 4.1f;
        Rectangle destino = {posicao.x - tamanhoSprite / 2.0f, posicao.y - tamanhoSprite / 2.0f, tamanhoSprite, tamanhoSprite};
        DrawTexturePro(texturaZumbi, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        Desenhar();
        return;
    }

    if (vivo)
    {
        // Barra de vida acima da cabeça, igual ao desenho antigo.
        int largura = 38;
        int altura = 6;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y - raio - 14);
        float percentual = vidaMaxima > 0.0f ? vida / vidaMaxima : 0.0f;

        DrawRectangle(x, y, largura, altura, RED);
        DrawRectangle(x, y, static_cast<int>(largura * percentual), altura, LIME);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
}
