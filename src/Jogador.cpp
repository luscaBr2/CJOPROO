#include "Jogador.hpp"
#include "Utilidades.hpp"
#include <algorithm>

float vidaInicialJogador = 200.0f;

Jogador::Jogador(Vector2 posicaoInicial)
    : Entidade(posicaoInicial, 17.0f, vidaInicialJogador),
      velocidade(190.0f),
      direcaoOlhar({0.0f, -1.0f}),
      carregandoAtaque(false),
      cargaAtaque(0.0f),
      cargaMaxima(1.5f),
      cooldownAtaque(1.0f),
      tempoCooldownAtaque(0.0f),
      armasBrancas(),
      ultimoAtaque({false, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, ""})
{
}

void Jogador::Reiniciar(Vector2 posicaoInicial)
{
    posicao = posicaoInicial;
    vida = vidaMaxima;
    vivo = true;
    direcaoOlhar = {0.0f, -1.0f};
    carregandoAtaque = false;
    cargaAtaque = 0.0f;
    tempoCooldownAtaque = 0.0f;
    armasBrancas.clear();
    ultimoAtaque = {false, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, ""};
}

void Jogador::Atualizar(float delta, const Casa &casa)
{
    ultimoAtaque.ocorreu = false;

    if (!vivo)
        return;

    if (tempoCooldownAtaque > 0.0f)
    {
        tempoCooldownAtaque -= delta;
        if (tempoCooldownAtaque < 0.0f)
        {
            tempoCooldownAtaque = 0.0f;
        }
    }

    Vector2 entrada = {0.0f, 0.0f};

    // Movimento WASD ou setas.
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        entrada.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        entrada.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        entrada.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        entrada.x += 1.0f;

    if (Comprimento(entrada) > 0.0f)
    {
        entrada = NormalizarSeguro(entrada);
        direcaoOlhar = entrada;
    }

    Vector2 deslocamento = Multiplicar(entrada, velocidade * delta);
    casa.MoverComColisao(posicao, raio, deslocamento, false);

    // Mantém o jogador dentro da tela
    posicao.x = LimitarFloat(posicao.x, raio, static_cast<float>(GetScreenWidth()) - raio);
    posicao.y = LimitarFloat(posicao.y, raio, static_cast<float>(GetScreenHeight()) - raio);

    // Ataque carregado: segura ESPAÇO, solta para bater.
    // O cooldown impede spam de ataques leves clicando várias vezes.
    if (IsKeyPressed(KEY_SPACE) && tempoCooldownAtaque <= 0.0f)
    {
        carregandoAtaque = true;
        cargaAtaque = 0.0f;
    }

    if (carregandoAtaque && IsKeyDown(KEY_SPACE))
    {
        cargaAtaque += delta;
        if (cargaAtaque > cargaMaxima)
        {
            cargaAtaque = cargaMaxima;
        }
    }

    if (carregandoAtaque && IsKeyReleased(KEY_SPACE))
    {
        float percentual = ObterPercentualCarga();
        float multiplicadorForca = 1.0f;
        float desgasteArma = 2.0f;
        const char *tipo = "leve";

        if (percentual >= 0.99f)
        {
            multiplicadorForca = 2.45f;
            desgasteArma = 7.0f;
            tipo = "forte";
        }
        else if (percentual >= 0.50f)
        {
            multiplicadorForca = 1.65f;
            desgasteArma = 4.0f;
            tipo = "medio";
        }

        // Cada arma tem dano próprio. As mãos são o menor dano e não quebram.
        float dano = ObterDanoBaseAtual() * multiplicadorForca;

        float bonusAlcance = ObterBonusAlcanceAtual();
        float distanciaCentroAtaque = 42.0f + bonusAlcance;
        float raioAtaque = 46.0f + bonusAlcance * 0.35f;
        Vector2 centroAtaque = Somar(posicao, Multiplicar(direcaoOlhar, distanciaCentroAtaque));
        ultimoAtaque = {true, centroAtaque, raioAtaque, dano, desgasteArma, tipo};

        // A durabilidade NÃO é consumida aqui.
        // Ela só cai em RegistrarAcertoEmZumbi(), chamado pelo Mundo quando o golpe acerta um zumbi.
        carregandoAtaque = false;
        cargaAtaque = 0.0f;
        tempoCooldownAtaque = cooldownAtaque;
    }

    // Se o jogador apertou ESPAÇO durante o cooldown e soltou, garantimos que não fique carga fantasma.
    if (!IsKeyDown(KEY_SPACE) && !carregandoAtaque)
    {
        cargaAtaque = 0.0f;
    }
}

void Jogador::Desenhar() const
{
    Color corJogador = vivo ? BLUE : DARKBLUE;

    // Jogador como círculo azul
    DrawCircleV(posicao, raio, corJogador);
    DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raio, BLACK);

    // Pequena linha indicando para onde o jogador está olhando/atacando.
    float bonusAlcance = ObterBonusAlcanceAtual();
    Vector2 pontaDirecao = Somar(posicao, Multiplicar(direcaoOlhar, raio + 12.0f + bonusAlcance * 0.20f));
    DrawLineEx(posicao, pontaDirecao, 3.0f, YELLOW);

    // Barra de força só aparece enquanto o ataque está carregando.
    if (carregandoAtaque)
    {
        float percentual = ObterPercentualCarga();
        int largura = 52;
        int altura = 7;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y + raio + 10);

        DrawRectangle(x, y, largura, altura, DARKGRAY);
        DrawRectangle(x, y, static_cast<int>(largura * percentual), altura, ORANGE);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
    else if (tempoCooldownAtaque > 0.0f)
    {
        // Pequena barra cinza mostrando que o ataque ainda está em recarga.
        int largura = 52;
        int altura = 5;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y + raio + 10);
        float pronto = 1.0f - ObterPercentualCooldownAtaque();

        DrawRectangle(x, y, largura, altura, DARKGRAY);
        DrawRectangle(x, y, static_cast<int>(largura * pronto), altura, LIGHTGRAY);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
}

ResultadoAtaque Jogador::ConsumirAtaque()
{
    ResultadoAtaque ataque = ultimoAtaque;
    ultimoAtaque.ocorreu = false;
    return ataque;
}

void Jogador::RegistrarAcertoEmZumbi(float desgaste)
{
    // Esta função existe para evitar perda de durabilidade ao atacar o ar.
    // O Mundo só chama esta função quando pelo menos um zumbi foi atingido.
    DesgastarArmaEquipada(desgaste);
}

void Jogador::DesenharBarraDeVida(int x, int y) const
{
    int largura = 220;
    int altura = 20;
    float percentual = vidaMaxima > 0.0f ? vida / vidaMaxima : 0.0f;

    DrawText("Vida do jogador", x, y - 22, 18, BLACK);
    DrawRectangle(x, y, largura, altura, RED);
    DrawRectangle(x, y, static_cast<int>(largura * percentual), altura, GREEN);
    DrawRectangleLines(x, y, largura, altura, BLACK);
}

void Jogador::DesenharInventarioArmas(int x, int y) const
{
    // HUD de armas no canto inferior esquerdo.
    // Ficou mais estreita e mais transparente para não atrapalhar a movimentação.
    // A área à direita é o espaço preparado para o sprite/ícone da arma equipada.
    int larguraHud = 326;
    int alturaHud = 112;

    DrawRectangle(x, y, larguraHud, alturaHud, Color{255, 255, 255, 110});
    DrawRectangleLines(x, y, larguraHud, alturaHud, Color{70, 70, 70, 160});

    // O texto pedido fica focado apenas nas armas reservas.
    DrawText("Armas reservas:", x + 10, y + 10, 17, BLACK);
    DrawText(TextFormat("1) %s", armasBrancas.size() > 1 ? armasBrancas[1].nome.c_str() : "Vazio"), x + 10, y + 32, 17, BLACK);
    DrawText(TextFormat("2) %s", armasBrancas.size() > 2 ? armasBrancas[2].nome.c_str() : "Vazio"), x + 10, y + 54, 17, BLACK);
    DrawText(TextFormat("3) %s", armasBrancas.size() > 3 ? armasBrancas[3].nome.c_str() : "Vazio"), x + 10, y + 76, 17, BLACK);

    // Barra de durabilidade da arma equipada.
    // Mãos não possuem durabilidade; armas coletadas mostram a barra sem poluir a HUD com texto extra.
    if (!armasBrancas.empty())
    {
        const ArmaBranca &equipada = armasBrancas[0];
        float percentual = equipada.durabilidadeMaxima > 0.0f ? equipada.durabilidade / equipada.durabilidadeMaxima : 0.0f;

        DrawRectangle(x + 10, y + 98, 190, 8, RED);
        DrawRectangle(x + 10, y + 98, static_cast<int>(190 * percentual), 8, GREEN);
        DrawRectangleLines(x + 10, y + 98, 190, 8, BLACK);
    }
    else
    {
        DrawRectangle(x + 10, y + 98, 190, 8, Color{90, 90, 90, 130});
        DrawRectangleLines(x + 10, y + 98, 190, 8, BLACK);
    }

    DesenharIconeArmaEquipada(x + 228, y + 26, 70);
}

void Jogador::DesenharIconeArmaEquipada(int x, int y, int tamanho) const
{
    // Espaço de imagem da arma equipada.
    // Quando tiver sprite, substitua o desenho interno por DrawTexturePro usando este retângulo.
    DrawRectangle(x, y, tamanho, tamanho, Color{235, 235, 235, 150});
    DrawRectangleLines(x, y, tamanho, tamanho, DARKGRAY);

    std::string nome = ObterNomeArmaBranca();

    if (nome == "Mãos")
    {
        DrawCircle(x + tamanho / 2 - 9, y + tamanho / 2, 10.0f, Color{235, 190, 150, 255});
        DrawCircle(x + tamanho / 2 + 9, y + tamanho / 2, 10.0f, Color{235, 190, 150, 255});
        DrawCircleLines(x + tamanho / 2 - 9, y + tamanho / 2, 10.0f, BROWN);
        DrawCircleLines(x + tamanho / 2 + 9, y + tamanho / 2, 10.0f, BROWN);
    }
    else if (nome == "Taco de beisebol")
    {
        DrawRectanglePro({static_cast<float>(x + tamanho / 2 - 4), static_cast<float>(y + 12), 8.0f, static_cast<float>(tamanho - 22)}, {4.0f, 0.0f}, 35.0f, ORANGE);
        DrawCircle(x + tamanho / 2, y + tamanho / 2, 5.0f, DARKBROWN);
    }
    else
    {
        DrawRectanglePro({static_cast<float>(x + tamanho / 2 - 4), static_cast<float>(y + 10), 8.0f, static_cast<float>(tamanho - 20)}, {4.0f, 0.0f}, -35.0f, LIGHTGRAY);
        DrawCircle(x + tamanho / 2, y + tamanho / 2, 5.0f, DARKGRAY);
    }
}

bool Jogador::AdicionarArmaBranca(const std::string &nomeArma, float danoBase, float novoBonusAlcance, float durabilidadeMaxima)
{
    // Novo HUD possui três linhas de reserva: 1 equipada + 3 reservas = 4 armas no total.
    // As mãos não contam como arma do inventário.
    if (armasBrancas.size() >= 4)
    {
        return false;
    }

    ArmaBranca novaArma;
    novaArma.nome = nomeArma;
    novaArma.danoBase = danoBase;
    novaArma.bonusAlcance = novoBonusAlcance;
    novaArma.durabilidade = durabilidadeMaxima;
    novaArma.durabilidadeMaxima = durabilidadeMaxima;

    armasBrancas.push_back(novaArma);
    return true;
}

float Jogador::ObterDanoBaseAtual() const
{
    // Mãos causam o menor dano e não têm durabilidade.
    if (armasBrancas.empty())
        return 8.0f;
    return armasBrancas[0].danoBase;
}

float Jogador::ObterBonusAlcanceAtual() const
{
    if (armasBrancas.empty())
        return 0.0f;
    return armasBrancas[0].bonusAlcance;
}

void Jogador::DesgastarArmaEquipada(float desgaste)
{
    // Mãos não quebram, então só desgastamos armas realmente coletadas.
    if (armasBrancas.empty())
        return;

    armasBrancas[0].durabilidade -= desgaste;
    if (armasBrancas[0].durabilidade > 0.0f)
        return;

    // A arma quebrou. Ao apagar o primeiro item, a reserva seguinte sobe automaticamente.
    armasBrancas.erase(armasBrancas.begin());
}

bool Jogador::EstaCarregandoAtaque() const
{
    return carregandoAtaque;
}

float Jogador::ObterPercentualCarga() const
{
    return LimitarFloat(cargaAtaque / cargaMaxima, 0.0f, 1.0f);
}

float Jogador::ObterPercentualCooldownAtaque() const
{
    if (cooldownAtaque <= 0.0f)
        return 0.0f;
    return LimitarFloat(tempoCooldownAtaque / cooldownAtaque, 0.0f, 1.0f);
}

const std::string &Jogador::ObterNomeArmaBranca() const
{
    static const std::string nomeMaos = "Mãos";
    if (armasBrancas.empty())
        return nomeMaos;
    return armasBrancas[0].nome;
}

const std::vector<ArmaBranca> &Jogador::ObterArmasBrancas() const
{
    return armasBrancas;
}

void Jogador::Desenhar(const TexturasJogo &texturas) const
{
    if (TexturasJogo::TexturaValida(texturas.jogador))
    {
        Rectangle origem = {0.0f, 0.0f, static_cast<float>(texturas.jogador.width), static_cast<float>(texturas.jogador.height)};
        // A escala visual é maior que o raio de colisão para o sprite ficar legível.
        // A colisão continua usando apenas o raio da Entidade.
        float tamanhoSprite = raio * 4.2f;
        Rectangle destino = {posicao.x - tamanhoSprite / 2.0f, posicao.y - tamanhoSprite / 2.0f, tamanhoSprite, tamanhoSprite};
        DrawTexturePro(texturas.jogador, origem, destino, {0.0f, 0.0f}, 0.0f, vivo ? WHITE : Color{120, 120, 160, 255});
    }
    else
    {
        Color corJogador = vivo ? BLUE : DARKBLUE;
        DrawCircleV(posicao, raio, corJogador);
        DrawCircleLines(static_cast<int>(posicao.x), static_cast<int>(posicao.y), raio, BLACK);
    }

    // Mantém a linha de direção do ataque. Ela é informação de jogabilidade, não só desenho do personagem.
    float bonusAlcance = ObterBonusAlcanceAtual();
    Vector2 pontaDirecao = Somar(posicao, Multiplicar(direcaoOlhar, raio + 12.0f + bonusAlcance * 0.20f));
    DrawLineEx(posicao, pontaDirecao, 3.0f, YELLOW);

    // Barra de força só aparece enquanto o ataque está carregando.
    if (carregandoAtaque)
    {
        float percentual = ObterPercentualCarga();
        int largura = 52;
        int altura = 7;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y + raio + 10);

        DrawRectangle(x, y, largura, altura, DARKGRAY);
        DrawRectangle(x, y, static_cast<int>(largura * percentual), altura, ORANGE);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
    else if (tempoCooldownAtaque > 0.0f)
    {
        int largura = 52;
        int altura = 5;
        int x = static_cast<int>(posicao.x - largura / 2);
        int y = static_cast<int>(posicao.y + raio + 10);
        float pronto = 1.0f - ObterPercentualCooldownAtaque();

        DrawRectangle(x, y, largura, altura, DARKGRAY);
        DrawRectangle(x, y, static_cast<int>(largura * pronto), altura, LIGHTGRAY);
        DrawRectangleLines(x, y, largura, altura, BLACK);
    }
}

void Jogador::DesenharInventarioArmas(int x, int y, const TexturasJogo &texturas) const
{
    int larguraHud = 326;
    int alturaHud = 112;

    DrawRectangle(x, y, larguraHud, alturaHud, Color{255, 255, 255, 110});
    DrawRectangleLines(x, y, larguraHud, alturaHud, Color{70, 70, 70, 160});

    DrawText("Armas reservas:", x + 10, y + 10, 17, BLACK);
    DrawText(TextFormat("1) %s", armasBrancas.size() > 1 ? armasBrancas[1].nome.c_str() : "Vazio"), x + 10, y + 32, 17, BLACK);
    DrawText(TextFormat("2) %s", armasBrancas.size() > 2 ? armasBrancas[2].nome.c_str() : "Vazio"), x + 10, y + 54, 17, BLACK);
    DrawText(TextFormat("3) %s", armasBrancas.size() > 3 ? armasBrancas[3].nome.c_str() : "Vazio"), x + 10, y + 76, 17, BLACK);

    if (!armasBrancas.empty())
    {
        const ArmaBranca &equipada = armasBrancas[0];
        float percentual = equipada.durabilidadeMaxima > 0.0f ? equipada.durabilidade / equipada.durabilidadeMaxima : 0.0f;

        DrawRectangle(x + 10, y + 98, 190, 8, RED);
        DrawRectangle(x + 10, y + 98, static_cast<int>(190 * percentual), 8, GREEN);
        DrawRectangleLines(x + 10, y + 98, 190, 8, BLACK);
    }
    else
    {
        DrawRectangle(x + 10, y + 98, 190, 8, Color{90, 90, 90, 130});
        DrawRectangleLines(x + 10, y + 98, 190, 8, BLACK);
    }

    DesenharIconeArmaEquipada(x + 228, y + 26, 70, texturas);
}

void Jogador::DesenharIconeArmaEquipada(int x, int y, int tamanho, const TexturasJogo &texturas) const
{
    DrawRectangle(x, y, tamanho, tamanho, Color{235, 235, 235, 150});
    DrawRectangleLines(x, y, tamanho, tamanho, DARKGRAY);

    std::string nome = ObterNomeArmaBranca();
    const Texture2D *texturaArma = &texturas.maos;

    if (nome == "Taco de beisebol")
    {
        texturaArma = &texturas.tacoBeisebol;
    }
    else if (nome == "Barra de metal")
    {
        texturaArma = &texturas.canoMetal;
    }

    if (TexturasJogo::TexturaValida(*texturaArma))
    {
        Rectangle origem = {0.0f, 0.0f, static_cast<float>(texturaArma->width), static_cast<float>(texturaArma->height)};
        Rectangle destino = {static_cast<float>(x + 8), static_cast<float>(y + 8), static_cast<float>(tamanho - 16), static_cast<float>(tamanho - 16)};
        DrawTexturePro(*texturaArma, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        // Fallback para o desenho antigo se o sprite da arma estiver faltando.
        DesenharIconeArmaEquipada(x, y, tamanho);
    }
}
