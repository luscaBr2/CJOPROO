#include "Mundo.hpp"
#include "Utilidades.hpp"
#include <algorithm>

namespace
{
    // Edite este valor para testar o jogo começando em dias avançados.
    // Exemplo: 1 = início normal, 5 = começa no dia 5, 10 = começa no dia 10.
    constexpr int DIA_INICIAL_TESTE = 1;
}

Mundo::Mundo()
    : casa(),
      jogador({610.0f, 500.0f}),
      diaAtual(DIA_INICIAL_TESTE),
      madeiraColetada(0),
      noite(false),
      tempoPeriodo(0.0f),
      tempoParaSpawn(1.0f),
      tempoVisualAtaque(0.0f),
      tempoMensagemItem(0.0f),
      indiceJanelaQuebradaTocadaPeloJogador(-1),
      mensagemItem(""),
      ataqueVisual({false, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, ""}),
      duracaoDia(45.0f),
      duracaoNoite(90.0f),
      custoMadeiraReparoJanela(2),
      custoMadeiraBarricadaJanela(4)
{
    GerarItensDoDia();
    texturas.Carregar();
}

Mundo::~Mundo()
{
    texturas.Descarregar();
}

void Mundo::Atualizar(float delta)
{
    if (!jogador.EstaVivo())
    {
        if (IsKeyPressed(KEY_R))
        {
            ReiniciarJogo();
        }
        return;
    }

    AtualizarCicloDiaNoite(delta);
    AtualizarSpawn(delta);
    AtualizarItens(delta);
    AtualizarInteracaoComJanelas();

    jogador.Atualizar(delta, casa);
    AtualizarDanoDaJanelaQuebrada();

    bool jogadorDentroDaCasa = casa.PontoEstaDentroDaCasa(jogador.ObterPosicao());
    for (Zumbi &zumbi : zumbis)
    {
        zumbi.Atualizar(delta, jogador, casa, jogadorDentroDaCasa);
    }

    ResultadoAtaque ataque = jogador.ConsumirAtaque();
    if (ataque.ocorreu)
    {
        ProcessarAtaqueDoJogador(ataque);
        ataqueVisual = ataque;
        tempoVisualAtaque = 0.12f;
    }

    if (tempoVisualAtaque > 0.0f)
    {
        tempoVisualAtaque -= delta;
    }

    if (tempoMensagemItem > 0.0f)
    {
        tempoMensagemItem -= delta;
    }

    RemoverZumbisAntigos();
    RemoverItensColetados();
}

void Mundo::AtualizarCicloDiaNoite(float delta)
{
    tempoPeriodo += delta;

    if (!noite && tempoPeriodo >= duracaoDia)
    {
        noite = true;
        tempoPeriodo = 0.0f;
        tempoParaSpawn = 0.5f;

        // itens.clear(); // Quando anoitece, os itens do dia desaparecem
    }
    else if (noite && tempoPeriodo >= duracaoNoite)
    {
        noite = false;
        tempoPeriodo = 0.0f;
        tempoParaSpawn = 0.5f;
        diaAtual++;

        // Não reiniciamos mais a casa automaticamente.
        // Agora reparos e barricadas feitos com madeira continuam tendo valor estratégico.
        GerarItensDoDia();
    }
}

void Mundo::AtualizarSpawn(float delta)
{
    tempoParaSpawn -= delta;
    if (tempoParaSpawn > 0.0f)
        return;

    // Dia: poucos zumbis. Noite: invasão mais intensa.
    if (!noite)
    {
        int limiteZumbisVivosDia = 2 + diaAtual / 2;
        if (ContarZumbisVivos() >= limiteZumbisVivosDia)
        {
            tempoParaSpawn = 4.0f;
            return;
        }
    }

    CriarZumbiAleatorio();

    float intervaloBase = noite ? 2.2f : 13.0f;
    float reducaoPorDia = static_cast<float>(diaAtual - 1) * (noite ? 0.12f : 0.30f);
    tempoParaSpawn = std::max(noite ? 0.75f : 7.0f, intervaloBase - reducaoPorDia);
}

void Mundo::AtualizarItens(float delta)
{
    (void)delta;

    if (noite)
        return;

    // Coleta com E.
    // Madeira soma recurso. Armas entram no inventário
    for (Item &item : itens)
    {
        if (!item.PodeSerColetado(jogador.ObterPosicao(), jogador.ObterRaio()))
            continue;

        if (IsKeyPressed(KEY_E))
        {
            if (item.ObterTipo() == TipoItem::Madeira)
            {
                madeiraColetada++;
                DefinirMensagem("+1 madeira coletada");
                item.MarcarComoColetado();
            }
            else
            {
                bool pegouArma = jogador.AdicionarArmaBranca(
                    item.ObterNome(),
                    item.ObterDanoBase(),
                    item.ObterBonusAlcance(),
                    item.ObterDurabilidadeMaxima());

                if (pegouArma)
                {
                    DefinirMensagem("arma guardada: " + item.ObterNome());
                    item.MarcarComoColetado();
                }
                else
                {
                    DefinirMensagem("inventario de armas cheio");
                }
            }
        }
    }
}

void Mundo::AtualizarDanoDaJanelaQuebrada()
{
    // Passar por uma janela quebrada machuca o jogador.
    // O dano acontece ao entrar na área da janela quebrada, não a cada frame,
    // para evitar que a vida derreta se o jogador parar em cima da abertura.
    int janelaTocada = casa.IndiceJanelaQuebradaSobreCirculo(jogador.ObterPosicao(), jogador.ObterRaio());

    if (janelaTocada < 0)
    {
        indiceJanelaQuebradaTocadaPeloJogador = -1;
        return;
    }

    if (janelaTocada != indiceJanelaQuebradaTocadaPeloJogador)
    {
        jogador.ReceberDano(6.0f);
        indiceJanelaQuebradaTocadaPeloJogador = janelaTocada;
        DefinirMensagem("cortes da janela quebrada: -6 vida");
    }
}

void Mundo::AtualizarInteracaoComJanelas()
{
    // O jogador precisa estar perto da janela para reparar ou reforçar.
    int janelaProxima = casa.IndiceJanelaProximaDoPonto(jogador.ObterPosicao(), 82.0f);
    if (janelaProxima < 0)
        return;

    if (IsKeyPressed(KEY_F))
    {
        if (!casa.JanelaPrecisaReparo(janelaProxima))
        {
            DefinirMensagem("essa janela ja esta inteira");
            return;
        }

        // Se o jogador ficar parado no local da janela e reparar ele fica preso, então é obrigatório se afastar da janela pra reparar
        if (casa.JanelaQuebrada(janelaProxima) &&
            casa.CirculoEstaSobreJanela(janelaProxima, jogador.ObterPosicao(), jogador.ObterRaio() + 2.0f))
        {
            DefinirMensagem("afaste-se da janela antes de reparar");
            return;
        }

        if (madeiraColetada < custoMadeiraReparoJanela)
        {
            DefinirMensagem(TextFormat("faltam madeiras: reparo custa %d", custoMadeiraReparoJanela));
            return;
        }

        if (casa.RepararJanela(janelaProxima))
        {
            madeiraColetada -= custoMadeiraReparoJanela;
            indiceJanelaQuebradaTocadaPeloJogador = -1;
            DefinirMensagem(TextFormat("janela reparada: -%d madeiras", custoMadeiraReparoJanela));
        }
    }

    if (IsKeyPressed(KEY_B))
    {
        if (casa.JanelaReforcada(janelaProxima))
        {
            DefinirMensagem("essa janela ja tem barricada");
            return;
        }

        // A barricada também restaura a janela e cria bloqueio físico, então a mesma proteção vale aqui.
        if (casa.JanelaQuebrada(janelaProxima) &&
            casa.CirculoEstaSobreJanela(janelaProxima, jogador.ObterPosicao(), jogador.ObterRaio() + 2.0f))
        {
            DefinirMensagem("afaste-se da janela antes de barricadar");
            return;
        }

        if (madeiraColetada < custoMadeiraBarricadaJanela)
        {
            DefinirMensagem(TextFormat("faltam madeiras: barricada custa %d", custoMadeiraBarricadaJanela));
            return;
        }

        if (casa.ReforcarJanela(janelaProxima))
        {
            madeiraColetada -= custoMadeiraBarricadaJanela;
            indiceJanelaQuebradaTocadaPeloJogador = -1;
            DefinirMensagem(TextFormat("barricada criada: -%d madeiras", custoMadeiraBarricadaJanela));
        }
    }
}

void Mundo::CriarZumbiAleatorio()
{
    int larguraTela = GetScreenWidth();
    int alturaTela = GetScreenHeight();
    int lado = GetRandomValue(0, 3);

    Vector2 posicaoSpawn = {0.0f, 0.0f};
    if (lado == 0)
        posicaoSpawn = {static_cast<float>(GetRandomValue(20, larguraTela - 20)), 25.0f};
    if (lado == 1)
        posicaoSpawn = {static_cast<float>(GetRandomValue(20, larguraTela - 20)), static_cast<float>(alturaTela - 25)};
    if (lado == 2)
        posicaoSpawn = {25.0f, static_cast<float>(GetRandomValue(20, alturaTela - 20))};
    if (lado == 3)
        posicaoSpawn = {static_cast<float>(larguraTela - 25), static_cast<float>(GetRandomValue(20, alturaTela - 20))};

    zumbis.emplace_back(posicaoSpawn, noite, diaAtual);
}

void Mundo::GerarItensDoDia()
{
    // itens.clear(); // Limpa os itens no começo do dia

    int quantidadeItens = GetRandomValue(12, 25) + std::min(diaAtual, 6);

    for (int i = 0; i < quantidadeItens; i++)
    {
        bool criado = false;

        for (int tentativa = 0; tentativa < 80 && !criado; tentativa++)
        {
            Vector2 posicaoItem = {
                static_cast<float>(GetRandomValue(50, GetScreenWidth() - 50)),
                static_cast<float>(GetRandomValue(70, GetScreenHeight() - 50))};

            if (!PosicaoValidaParaItem(posicaoItem))
                continue;

            int sorteio = GetRandomValue(1, 100);
            TipoItem tipo = TipoItem::Madeira;

            if (sorteio > 72 && sorteio <= 90)
            {
                tipo = TipoItem::TacoBeisebol;
            }
            else if (sorteio > 90)
            {
                tipo = TipoItem::CanoMetal;
            }

            itens.emplace_back(posicaoItem, tipo);
            criado = true;
        }
    }
}

bool Mundo::PosicaoValidaParaItem(Vector2 posicaoItem) const
{
    // Itens devem aparecer fora da casa, não em cima do chão branco nem colados demais na casa.
    if (casa.PontoEstaNaAreaDaCasa(posicaoItem))
    {
        return false;
    }

    Rectangle zonaSeguraAoRedorDaCasa = {190.0f, 100.0f, 820.0f, 580.0f};
    if (CheckCollisionPointRec(posicaoItem, zonaSeguraAoRedorDaCasa))
    {
        return false;
    }

    for (const Item &itemExistente : itens)
    {
        if (Distancia(itemExistente.ObterPosicao(), posicaoItem) < 45.0f)
        {
            return false;
        }
    }

    return true;
}

int Mundo::ContarZumbisVivos() const
{
    int total = 0;
    for (const Zumbi &zumbi : zumbis)
    {
        if (zumbi.EstaVivo())
            total++;
    }
    return total;
}

void Mundo::ProcessarAtaqueDoJogador(const ResultadoAtaque &ataque)
{
    bool acertouPeloMenosUmZumbi = false;

    for (Zumbi &zumbi : zumbis)
    {
        if (!zumbi.EstaVivo())
            continue;

        float distancia = Distancia(ataque.centro, zumbi.ObterPosicao());
        bool alcanceAcertou = distancia <= ataque.raio + zumbi.ObterRaio();
        if (!alcanceAcertou)
            continue;

        Vector2 posicaoJogador = jogador.ObterPosicao();
        Vector2 posicaoZumbi = zumbi.ObterPosicao();

        // Regra importante:
        // Parede bloqueia dano. Janela fechada só permite dano se jogador e zumbi estiverem nela.
        // Janela quebrada é abertura e permite dano através dela.
        if (casa.LinhaBloqueadaPorParede(posicaoJogador, posicaoZumbi))
        {
            continue;
        }

        int janelaNaLinha = casa.IndiceJanelaNaLinha(posicaoJogador, posicaoZumbi);
        if (janelaNaLinha >= 0)
        {
            if (!casa.PodeAtacarPorEstaJanela(janelaNaLinha, posicaoJogador, posicaoZumbi, zumbi.ObterRaio()))
            {
                continue;
            }
        }

        zumbi.ReceberDano(ataque.dano);
        acertouPeloMenosUmZumbi = true;
    }

    // A durabilidade da arma só cai quando o golpe realmente acerta zumbi.
    // Atacar o ar ou bater numa parede não consome durabilidade.
    if (acertouPeloMenosUmZumbi)
    {
        jogador.RegistrarAcertoEmZumbi(ataque.desgasteArma);
    }
}

void Mundo::RemoverZumbisAntigos()
{
    zumbis.erase(
        std::remove_if(zumbis.begin(), zumbis.end(), [](const Zumbi &zumbi)
                       { return zumbi.PodeSerRemovido(); }),
        zumbis.end());
}

void Mundo::RemoverItensColetados()
{
    itens.erase(
        std::remove_if(itens.begin(), itens.end(), [](const Item &item)
                       { return item.FoiColetado(); }),
        itens.end());
}

void Mundo::DefinirMensagem(const std::string &texto)
{
    mensagemItem = texto;
    tempoMensagemItem = 2.0f;
}

std::string Mundo::FormatarTempo(float segundos) const
{
    // Converte segundos restantes para mm:ss, usado no HUD do ciclo dia/noite.
    if (segundos < 0.0f)
        segundos = 0.0f;

    int totalSegundos = static_cast<int>(segundos + 0.999f);
    int minutos = totalSegundos / 60;
    int segundosRestantes = totalSegundos % 60;

    return TextFormat("%02d:%02d", minutos, segundosRestantes);
}

void Mundo::Desenhar() const
{
    Color corFundo = noite ? Color{35, 38, 58, 255} : Color{120, 170, 110, 255};
    ClearBackground(corFundo);

    // Fundo externo com sprite de grama
    if (TexturasJogo::TexturaValida(texturas.grama))
    {
        Rectangle origem = {0.0f, 0.0f, static_cast<float>(texturas.grama.width), static_cast<float>(texturas.grama.height)};
        for (int y = 0; y < GetScreenHeight(); y += 64)
        {
            for (int x = 0; x < GetScreenWidth(); x += 64)
            {
                Rectangle destino = {static_cast<float>(x), static_cast<float>(y), 64.0f, 64.0f};
                DrawTexturePro(texturas.grama, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
            }
        }

        if (noite)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 35, 105});
        }
    }

    for (const Item &item : itens)
    {
        item.Desenhar(texturas);
    }

    casa.Desenhar(texturas);

    for (const Zumbi &zumbi : zumbis)
    {
        zumbi.Desenhar(texturas);
    }

    jogador.Desenhar(texturas);

    if (tempoVisualAtaque > 0.0f && ataqueVisual.ocorreu)
    {
        DrawCircleLines(static_cast<int>(ataqueVisual.centro.x), static_cast<int>(ataqueVisual.centro.y), ataqueVisual.raio, YELLOW);
    }

    DesenharHud();

    if (!jogador.EstaVivo())
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});
        DrawText("VOCE CAIU", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 30, 34, RAYWHITE);
        DrawText("Pressione R para reiniciar", GetScreenWidth() / 2 - 135, GetScreenHeight() / 2 + 12, 22, RAYWHITE);
    }
}

void Mundo::DesenharHud() const
{
    // HUD principal limpa: vida do jogador, tempo do ciclo, dia e madeiras.
    jogador.DesenharBarraDeVida(20, 38);

    float tempoRestantePeriodo = (noite ? duracaoNoite : duracaoDia) - tempoPeriodo;
    const char *textoProximaFase = noite ? "Dia em" : "Noite em";
    DrawText(TextFormat("%s: %s", textoProximaFase, FormatarTempo(tempoRestantePeriodo).c_str()), 260, 38, 20, BLACK);

    DrawText(TextFormat("Dia: %d", diaAtual), 20, 84, 22, BLACK);

    // Ícone de madeira à esquerda do texto.
    int xMadeira = 20;
    int yMadeira = 116;
    if (TexturasJogo::TexturaValida(texturas.madeira))
    {
        Rectangle origemMadeira = {0.0f, 0.0f, static_cast<float>(texturas.madeira.width), static_cast<float>(texturas.madeira.height)};
        Rectangle destinoMadeira = {static_cast<float>(xMadeira), static_cast<float>(yMadeira - 2), 30.0f, 30.0f};
        DrawTexturePro(texturas.madeira, origemMadeira, destinoMadeira, {0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        DrawRectangle(xMadeira, yMadeira + 5, 28, 12, BROWN);
        DrawRectangleLines(xMadeira, yMadeira + 5, 28, 12, DARKBROWN);
        DrawLineEx({static_cast<float>(xMadeira + 4), static_cast<float>(yMadeira + 11)}, {static_cast<float>(xMadeira + 24), static_cast<float>(yMadeira + 11)}, 1.0f, DARKBROWN);
    }
    DrawText(TextFormat("Madeiras: %d", madeiraColetada), xMadeira + 38, yMadeira, 22, BLACK);

    // Legenda simples de controles no canto superior direito.
    const int larguraLegenda = 310;
    const int alturaLegenda = 132;
    const int xLegenda = GetScreenWidth() - larguraLegenda - 20;
    const int yLegenda = 20;

    DrawRectangle(xLegenda, yLegenda, larguraLegenda, alturaLegenda, Color{255, 255, 255, 135});
    DrawRectangleLines(xLegenda, yLegenda, larguraLegenda, alturaLegenda, Color{40, 40, 40, 150});
    DrawText("Controles", xLegenda + 12, yLegenda + 10, 18, BLACK);
    DrawText("W, A, S, D: mover o jogador", xLegenda + 12, yLegenda + 34, 15, BLACK);
    DrawText("ESPACO: Atacar", xLegenda + 12, yLegenda + 54, 15, BLACK);
    DrawText("E: Coletar", xLegenda + 12, yLegenda + 74, 15, BLACK);
    DrawText("F: Reparar janela", xLegenda + 12, yLegenda + 94, 15, BLACK);
    DrawText("B: Criar barricada na janela", xLegenda + 12, yLegenda + 114, 15, BLACK);

    // Inventário/durabilidade de armas no canto inferior esquerdo.
    jogador.DesenharInventarioArmas(20, GetScreenHeight() - 132, texturas);

    // Mensagens de coleta/reparo no canto inferior direito.
    if (tempoMensagemItem > 0.0f)
    {
        int largura = 440;
        int altura = 36;
        int x = GetScreenWidth() - largura - 20;
        int y = GetScreenHeight() - altura - 20;

        DrawRectangle(x, y, largura, altura, Color{255, 255, 255, 210});
        DrawRectangleLines(x, y, largura, altura, DARKGRAY);
        DrawText(mensagemItem.c_str(), x + 12, y + 9, 18, BLACK);
    }
}

void Mundo::ReiniciarJogo()
{
    casa.Reiniciar();
    jogador.Reiniciar({610.0f, 500.0f});
    zumbis.clear();
    itens.clear();
    diaAtual = DIA_INICIAL_TESTE;
    madeiraColetada = 0;
    noite = false;
    tempoPeriodo = 0.0f;
    tempoParaSpawn = 1.0f;
    tempoVisualAtaque = 0.0f;
    tempoMensagemItem = 0.0f;
    indiceJanelaQuebradaTocadaPeloJogador = -1;
    mensagemItem = "";
    ataqueVisual = {false, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, ""};
    GerarItensDoDia();
}
