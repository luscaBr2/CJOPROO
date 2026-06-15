#ifndef JOGADOR_HPP
#define JOGADOR_HPP

#include "Entidade.hpp"
#include "Casa.hpp"
#include "TexturasJogo.hpp"
#include <string>
#include <vector>

struct ResultadoAtaque
{
    bool ocorreu;
    Vector2 centro;
    float raio;
    float dano;
    float desgasteArma;
    const char* tipo;
};

struct ArmaBranca
{
    std::string nome;
    float danoBase;
    float bonusAlcance;
    float durabilidade;
    float durabilidadeMaxima;
};

class Jogador : public Entidade
{
private:
    float velocidade;
    Vector2 direcaoOlhar;
    bool carregandoAtaque;
    float cargaAtaque;
    float cargaMaxima;
    float cooldownAtaque;
    float tempoCooldownAtaque;
    std::vector<ArmaBranca> armasBrancas;
    ResultadoAtaque ultimoAtaque;

    float ObterDanoBaseAtual() const;
    float ObterBonusAlcanceAtual() const;
    void DesgastarArmaEquipada(float desgaste);
    void DesenharIconeArmaEquipada(int x, int y, int tamanho) const;
    void DesenharIconeArmaEquipada(int x, int y, int tamanho, const TexturasJogo& texturas) const;

public:
    Jogador(Vector2 posicaoInicial);

    void Reiniciar(Vector2 posicaoInicial);
    void Atualizar(float delta, const Casa& casa);
    void Desenhar() const override;
    void Desenhar(const TexturasJogo& texturas) const;

    ResultadoAtaque ConsumirAtaque();
    void RegistrarAcertoEmZumbi(float desgaste);
    void DesenharBarraDeVida(int x, int y) const;
    void DesenharInventarioArmas(int x, int y) const;
    void DesenharInventarioArmas(int x, int y, const TexturasJogo& texturas) const;

    bool AdicionarArmaBranca(const std::string& nomeArma, float danoBase, float novoBonusAlcance, float durabilidadeMaxima);

    bool EstaCarregandoAtaque() const;
    float ObterPercentualCarga() const;
    float ObterPercentualCooldownAtaque() const;
    const std::string& ObterNomeArmaBranca() const;
    const std::vector<ArmaBranca>& ObterArmasBrancas() const;
};

#endif
