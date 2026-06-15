#ifndef ITEM_HPP
#define ITEM_HPP

#include "raylib.h"
#include "TexturasJogo.hpp"
#include <string>

// Tipos simples de itens
enum class TipoItem
{
    Madeira,
    TacoBeisebol,
    CanoMetal
};

class Item
{
private:
    Vector2 posicao;
    TipoItem tipo;
    bool coletado;
    float raioColeta;

public:
    Item(Vector2 posicaoInicial, TipoItem tipoInicial);

    void Desenhar() const;
    void Desenhar(const TexturasJogo &texturas) const;
    bool PodeSerColetado(Vector2 posicaoJogador, float raioJogador) const;
    void MarcarComoColetado();

    bool FoiColetado() const;
    TipoItem ObterTipo() const;
    Vector2 ObterPosicao() const;
    std::string ObterNome() const;
    float ObterDanoBase() const;
    float ObterBonusAlcance() const;
    float ObterDurabilidadeMaxima() const;
};

#endif
