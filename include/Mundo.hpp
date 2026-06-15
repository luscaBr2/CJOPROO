#ifndef MUNDO_HPP
#define MUNDO_HPP

#include "Casa.hpp"
#include "Jogador.hpp"
#include "Zumbi.hpp"
#include "Item.hpp"
#include "TexturasJogo.hpp"
#include <vector>
#include <string>

class Mundo
{
private:
    Casa casa;
    Jogador jogador;
    std::vector<Zumbi> zumbis;
    std::vector<Item> itens;
    TexturasJogo texturas;

    int diaAtual;
    int madeiraColetada;
    bool noite;
    float tempoPeriodo;
    float tempoParaSpawn;
    float tempoVisualAtaque;
    float tempoMensagemItem;
    int indiceJanelaQuebradaTocadaPeloJogador;
    std::string mensagemItem;
    ResultadoAtaque ataqueVisual;

    const float duracaoDia;
    const float duracaoNoite;
    const int custoMadeiraReparoJanela;
    const int custoMadeiraBarricadaJanela;

    void AtualizarCicloDiaNoite(float delta);
    void AtualizarSpawn(float delta);
    void AtualizarItens(float delta);
    void AtualizarDanoDaJanelaQuebrada();
    void AtualizarInteracaoComJanelas();
    void CriarZumbiAleatorio();
    void GerarItensDoDia();
    bool PosicaoValidaParaItem(Vector2 posicaoItem) const;
    int ContarZumbisVivos() const;
    void ProcessarAtaqueDoJogador(const ResultadoAtaque& ataque);
    void RemoverZumbisAntigos();
    void RemoverItensColetados();
    void DefinirMensagem(const std::string& texto);
    std::string FormatarTempo(float segundos) const;
    void DesenharHud() const;
    void ReiniciarJogo();

public:
    Mundo();
    ~Mundo();

    void Atualizar(float delta);
    void Desenhar() const;
};

#endif
