#ifndef ZUMBI_HPP
#define ZUMBI_HPP

#include "Entidade.hpp"
#include "Casa.hpp"
#include "Jogador.hpp"
#include "TexturasJogo.hpp"

class Zumbi : public Entidade
{
private:
    float velocidade;
    float dano;
    float intervaloAtaque;
    float tempoDesdeUltimoAtaque;
    float tempoCorpoNoChao;
    float tempoMaximoCorpoNoChao;

public:
    Zumbi(Vector2 posicaoInicial, bool noite, int diaAtual);

    void Atualizar(float delta, Jogador& jogador, Casa& casa, bool jogadorDentroDaCasa);
    void Desenhar() const override;
    void Desenhar(const TexturasJogo& texturas) const;

    bool PodeSerRemovido() const;
    void MarcarTempoDeCorpo(float delta);
};

#endif
