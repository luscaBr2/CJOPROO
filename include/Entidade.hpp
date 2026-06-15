#ifndef ENTIDADE_HPP
#define ENTIDADE_HPP

#include "raylib.h"

// Classe base para tudo que tem posição, raio e vida.
// Jogador e Zumbi herdam desta classe.
class Entidade
{
protected:
    Vector2 posicao;
    float raio;
    float vida;
    float vidaMaxima;
    bool vivo;

public:
    Entidade(Vector2 posicaoInicial, float raioInicial, float vidaInicial);
    virtual ~Entidade() = default;

    virtual void Desenhar() const = 0;

    Vector2 ObterPosicao() const;
    float ObterRaio() const;
    float ObterVida() const;
    float ObterVidaMaxima() const;
    bool EstaVivo() const;

    void DefinirPosicao(Vector2 novaPosicao);
    void ReceberDano(float dano);
    void Curar(float quantidade);
};

#endif
