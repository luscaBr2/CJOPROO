#include "Entidade.hpp"
#include "Utilidades.hpp"

Entidade::Entidade(Vector2 posicaoInicial, float raioInicial, float vidaInicial)
    : posicao(posicaoInicial), raio(raioInicial), vida(vidaInicial), vidaMaxima(vidaInicial), vivo(true)
{
}

Vector2 Entidade::ObterPosicao() const
{
    return posicao;
}

float Entidade::ObterRaio() const
{
    return raio;
}

float Entidade::ObterVida() const
{
    return vida;
}

float Entidade::ObterVidaMaxima() const
{
    return vidaMaxima;
}

bool Entidade::EstaVivo() const
{
    return vivo;
}

void Entidade::DefinirPosicao(Vector2 novaPosicao)
{
    posicao = novaPosicao;
}

void Entidade::ReceberDano(float dano)
{
    if (!vivo) return;

    vida -= dano;

    if (vida <= 0.0f)
    {
        vida = 0.0f;
        vivo = false;
    }
}

void Entidade::Curar(float quantidade)
{
    if (!vivo) return;

    vida = LimitarFloat(vida + quantidade, 0.0f, vidaMaxima);
}
