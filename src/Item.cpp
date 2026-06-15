#include "Item.hpp"
#include "Utilidades.hpp"

Item::Item(Vector2 posicaoInicial, TipoItem tipoInicial)
    : posicao(posicaoInicial), tipo(tipoInicial), coletado(false), raioColeta(18.0f)
{
}

void Item::Desenhar() const
{
    if (coletado)
        return;

    if (tipo == TipoItem::Madeira)
    {
        DrawRectangle(static_cast<int>(posicao.x - 12), static_cast<int>(posicao.y - 6), 24, 12, BROWN);
        DrawRectangleLines(static_cast<int>(posicao.x - 12), static_cast<int>(posicao.y - 6), 24, 12, DARKBROWN);
    }
    else if (tipo == TipoItem::TacoBeisebol)
    {
        DrawRectanglePro({posicao.x - 3.0f, posicao.y - 18.0f, 6.0f, 36.0f}, {3.0f, 18.0f}, 35.0f, ORANGE);
        DrawCircleV(posicao, 4.0f, DARKBROWN);
    }
    else if (tipo == TipoItem::CanoMetal)
    {
        DrawRectanglePro({posicao.x - 4.0f, posicao.y - 20.0f, 8.0f, 40.0f}, {4.0f, 20.0f}, -35.0f, LIGHTGRAY);
        DrawCircleV(posicao, 4.0f, DARKGRAY);
    }

    DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raioColeta, Color{0, 0, 0, 70});
}

bool Item::PodeSerColetado(Vector2 posicaoJogador, float raioJogador) const
{
    if (coletado)
        return false;
    return Distancia(posicao, posicaoJogador) <= raioColeta + raioJogador;
}

void Item::MarcarComoColetado()
{
    coletado = true;
}

bool Item::FoiColetado() const
{
    return coletado;
}

TipoItem Item::ObterTipo() const
{
    return tipo;
}

Vector2 Item::ObterPosicao() const
{
    return posicao;
}

std::string Item::ObterNome() const
{
    if (tipo == TipoItem::Madeira)
        return "Madeira";
    if (tipo == TipoItem::TacoBeisebol)
        return "Taco de beisebol";
    return "Barra de metal";
}

float Item::ObterDanoBase() const
{
    // Cada arma tem dano próprio.
    // Mãos ficam no Jogador com dano 8. Taco fica no meio. Barra de metal é a mais forte.
    if (tipo == TipoItem::TacoBeisebol)
        return 18.0f;
    if (tipo == TipoItem::CanoMetal)
        return 27.0f;
    return 0.0f;
}

float Item::ObterBonusAlcance() const
{
    if (tipo == TipoItem::TacoBeisebol)
        return 8.0f;
    if (tipo == TipoItem::CanoMetal)
        return 13.0f;
    return 0.0f;
}

float Item::ObterDurabilidadeMaxima() const
{
    // Durabilidade inicial de cada arma. Edite estes valores para balancear o jogo.
    if (tipo == TipoItem::TacoBeisebol)
        return 65.0f;
    if (tipo == TipoItem::CanoMetal)
        return 90.0f;
    return 0.0f;
}

void Item::Desenhar(const TexturasJogo &texturas) const
{
    if (coletado)
        return;

    const Texture2D *texturaItem = &texturas.madeira;

    if (tipo == TipoItem::TacoBeisebol)
    {
        texturaItem = &texturas.tacoBeisebol;
    }
    else if (tipo == TipoItem::CanoMetal)
    {
        texturaItem = &texturas.canoMetal;
    }

    if (TexturasJogo::TexturaValida(*texturaItem))
    {
        Rectangle origem = {0.0f, 0.0f, static_cast<float>(texturaItem->width), static_cast<float>(texturaItem->height)};
        Rectangle destino = {posicao.x - 17.0f, posicao.y - 17.0f, 34.0f, 34.0f};
        DrawTexturePro(*texturaItem, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        // Fallback: se o arquivo de sprite faltar, usa o desenho antigo.
        Desenhar();
        return;
    }

    DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raioColeta, Color{0, 0, 0, 70});
}
