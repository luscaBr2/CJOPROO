#ifndef CASA_HPP
#define CASA_HPP

#include "raylib.h"
#include "TexturasJogo.hpp"
#include <vector>

struct Janela
{
    Rectangle area;
    float vida;
    float vidaMaxima;
    int nivelBarricada;

    Vector2 Centro() const
    {
        return {area.x + area.width / 2.0f, area.y + area.height / 2.0f};
    }

    bool EstaQuebrada() const
    {
        return vida <= 0.0f;
    }

    bool EstaReforcada() const
    {
        return nivelBarricada > 0;
    }
};

// A casa controla chão, paredes, janelas, barricadas e escada.
// Ponto importante: paredes continuam sendo bloqueios físicos.
// Janelas fechadas bloqueiam passagem; janelas quebradas viram passagem real.
// A escada é passagem do jogador, mas bloqueia zumbis.
class Casa
{
private:
    Rectangle areaChao;
    Rectangle areaInterna;
    Rectangle areaEscada;
    std::vector<Rectangle> paredes;
    std::vector<Janela> janelas;

public:
    Casa();

    void Reiniciar();
    void Desenhar() const;
    void Desenhar(const TexturasJogo& texturas) const;

    bool PontoEstaDentroDaCasa(Vector2 ponto) const;
    bool PontoEstaNaAreaDaCasa(Vector2 ponto) const;
    bool CirculoColideComObstaculo(Vector2 centro, float raio, bool entidadeEhZumbi) const;
    void MoverComColisao(Vector2& posicao, float raio, Vector2 deslocamento, bool entidadeEhZumbi) const;

    int IndiceJanelaMaisProxima(Vector2 ponto) const;
    int IndiceJanelaQuebradaMaisProxima(Vector2 ponto) const;
    int IndiceJanelaProximaDoPonto(Vector2 ponto, float distanciaMaxima) const;
    int IndiceJanelaNaLinha(Vector2 inicio, Vector2 fim) const;

    bool LinhaBloqueadaPorParede(Vector2 inicio, Vector2 fim) const;
    bool PodeAtacarPorEstaJanela(int indiceJanela, Vector2 posicaoJogador, Vector2 posicaoZumbi, float raioZumbi) const;

    void DanoNaJanela(int indiceJanela, float dano);
    bool JanelaQuebrada(int indiceJanela) const;
    bool JanelaReforcada(int indiceJanela) const;
    bool JanelaPrecisaReparo(int indiceJanela) const;
    bool CirculoEstaSobreJanela(int indiceJanela, Vector2 centro, float raio) const;
    int IndiceJanelaQuebradaSobreCirculo(Vector2 centro, float raio) const;
    bool RepararJanela(int indiceJanela);
    bool ReforcarJanela(int indiceJanela);
    Vector2 CentroJanela(int indiceJanela) const;
    Vector2 PontoInternoDepoisDaJanela(int indiceJanela, float margem) const;

    const std::vector<Janela>& ObterJanelas() const;
};

#endif
