#include "Casa.hpp"
#include "Utilidades.hpp"
#include <algorithm>

Casa::Casa()
{
    Reiniciar();
}

void Casa::Reiniciar()
{
    paredes.clear();
    janelas.clear();

    // Casa grande: o jogador consegue circular por dentro e defender as janelas.
    areaChao = {230.0f, 140.0f, 740.0f, 500.0f};
    areaInterna = {258.0f, 168.0f, 684.0f, 444.0f};

    // Escada/entrada: jogador atravessa por aqui; zumbi não atravessa.
    // Agora ela é vertical e atravessa levemente a parede inferior.
    // A largura continua maior que o diâmetro do jogador, então ele passa sem travar.
    areaEscada = {588.0f, 584.0f, 64.0f, 88.0f};

    // Janelas menores. O último valor é o nível de barricada: 0 = sem barricada.
    // Parede superior: 2 janelas.
    janelas.push_back({{390.0f, 140.0f, 90.0f, 28.0f}, 100.0f, 100.0f, 0}); // janela superior esquerda
    janelas.push_back({{720.0f, 140.0f, 90.0f, 28.0f}, 100.0f, 100.0f, 0}); // janela superior direita

    // Parede direita: 1 janela.
    janelas.push_back({{942.0f, 340.0f, 28.0f, 90.0f}, 100.0f, 100.0f, 0}); // janela direita

    // Parede esquerda: 1 janela.
    // A posição espelha a janela da parede direita, mantendo a casa mais justa de defender.
    janelas.push_back({{230.0f, 340.0f, 28.0f, 90.0f}, 100.0f, 100.0f, 0}); // janela esquerda

    // Parede inferior: 2 janelas. A escada fica entre elas.
    janelas.push_back({{340.0f, 612.0f, 90.0f, 28.0f}, 100.0f, 100.0f, 0}); // janela inferior esquerda
    janelas.push_back({{790.0f, 612.0f, 90.0f, 28.0f}, 100.0f, 100.0f, 0}); // janela inferior direita

    // Paredes cinzas com aberturas exatamente nos espaços das janelas e da escada.
    // IMPORTANTE: as paredes não desaparecem quando a janela quebra; só a área da janela vira passagem.
    paredes.push_back({230.0f, 140.0f, 160.0f, 28.0f}); // superior antes da janela 1
    paredes.push_back({480.0f, 140.0f, 240.0f, 28.0f}); // superior entre janelas
    paredes.push_back({810.0f, 140.0f, 160.0f, 28.0f}); // superior depois da janela 2

    paredes.push_back({230.0f, 612.0f, 110.0f, 28.0f}); // inferior antes da janela 1
    paredes.push_back({430.0f, 612.0f, 158.0f, 28.0f}); // inferior entre janela 1 e escada
    paredes.push_back({652.0f, 612.0f, 138.0f, 28.0f}); // inferior entre escada e janela 2
    paredes.push_back({880.0f, 612.0f, 90.0f, 28.0f});  // inferior depois da janela 2

    paredes.push_back({230.0f, 168.0f, 28.0f, 172.0f}); // esquerda antes da janela
    paredes.push_back({230.0f, 430.0f, 28.0f, 182.0f}); // esquerda depois da janela

    paredes.push_back({942.0f, 168.0f, 28.0f, 172.0f}); // direita antes da janela
    paredes.push_back({942.0f, 430.0f, 28.0f, 182.0f}); // direita depois da janela
}

void Casa::Desenhar() const
{
    // Chão branco da casa.
    DrawRectangleRec(areaChao, RAYWHITE);
    DrawRectangleLinesEx(areaChao, 2.0f, BLACK);

    // Escada desenhada na entrada inferior.
    // Fallback sem sprite: escada vertical de madeira.
    DrawRectangleRec(areaEscada, Color{142, 91, 48, 255});
    DrawRectangleLinesEx(areaEscada, 2.0f, DARKBROWN);
    DrawLineEx({areaEscada.x + 12.0f, areaEscada.y + 6.0f}, {areaEscada.x + 12.0f, areaEscada.y + areaEscada.height - 6.0f}, 4.0f, DARKBROWN);
    DrawLineEx({areaEscada.x + areaEscada.width - 12.0f, areaEscada.y + 6.0f}, {areaEscada.x + areaEscada.width - 12.0f, areaEscada.y + areaEscada.height - 6.0f}, 4.0f, DARKBROWN);
    for (float yDegrau = areaEscada.y + 14.0f; yDegrau < areaEscada.y + areaEscada.height - 8.0f; yDegrau += 16.0f)
    {
        DrawLineEx({areaEscada.x + 12.0f, yDegrau}, {areaEscada.x + areaEscada.width - 12.0f, yDegrau}, 4.0f, BROWN);
    }

    // Paredes cinza. Para colocar sprite de parede depois, troque este DrawRectangleRec por DrawTexturePro.
    for (const Rectangle &parede : paredes)
    {
        DrawRectangleRec(parede, GRAY);
        DrawRectangleLinesEx(parede, 1.0f, DARKGRAY);
    }

    // Janelas com barra de vida, dano e visual de barricada.
    for (const Janela &janela : janelas)
    {
        Color corJanela = SKYBLUE;
        Color corBorda = BLUE;

        if (janela.EstaQuebrada())
        {
            // Janela quebrada fica marrom escura para indicar buraco/passagem.
            corJanela = MAROON;
            corBorda = DARKBROWN;
        }
        else if (janela.EstaReforcada())
        {
            // Janela reforçada ainda é janela, mas recebe tábuas desenhadas por cima.
            corJanela = Color{150, 205, 235, 255};
            corBorda = BROWN;
        }

        DrawRectangleRec(janela.area, corJanela);
        DrawRectangleLinesEx(janela.area, 2.0f, corBorda);

        if (janela.EstaReforcada() && !janela.EstaQuebrada())
        {
            // Barricada simples com tábuas. Depois troque aqui por sprite de barricada.
            if (janela.area.width > janela.area.height)
            {
                DrawRectangle(static_cast<int>(janela.area.x + 6), static_cast<int>(janela.area.y + 5), static_cast<int>(janela.area.width - 12), 5, BROWN);
                DrawRectangle(static_cast<int>(janela.area.x + 6), static_cast<int>(janela.area.y + 17), static_cast<int>(janela.area.width - 12), 5, BROWN);
            }
            else
            {
                DrawRectangle(static_cast<int>(janela.area.x + 5), static_cast<int>(janela.area.y + 6), 5, static_cast<int>(janela.area.height - 12), BROWN);
                DrawRectangle(static_cast<int>(janela.area.x + 17), static_cast<int>(janela.area.y + 6), 5, static_cast<int>(janela.area.height - 12), BROWN);
            }
        }

        float larguraBarra = janela.area.width > janela.area.height ? janela.area.width : 52.0f;
        float alturaBarra = 6.0f;
        float xBarra = janela.Centro().x - larguraBarra / 2.0f;
        float yBarra = janela.area.y - 12.0f;

        if (janela.area.height > janela.area.width)
        {
            xBarra = janela.area.x - 16.0f;
            yBarra = janela.Centro().y - 30.0f;
        }

        float percentualVida = janela.vidaMaxima > 0.0f ? janela.vida / janela.vidaMaxima : 0.0f;
        DrawRectangle(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra), static_cast<int>(alturaBarra), RED);
        DrawRectangle(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra * percentualVida), static_cast<int>(alturaBarra), janela.EstaReforcada() ? GOLD : GREEN);
        DrawRectangleLines(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra), static_cast<int>(alturaBarra), BLACK);
    }
}

bool Casa::PontoEstaDentroDaCasa(Vector2 ponto) const
{
    return CheckCollisionPointRec(ponto, areaInterna);
}

bool Casa::PontoEstaNaAreaDaCasa(Vector2 ponto) const
{
    return CheckCollisionPointRec(ponto, areaChao);
}

bool Casa::CirculoColideComObstaculo(Vector2 centro, float raio, bool entidadeEhZumbi) const
{
    for (const Rectangle &parede : paredes)
    {
        if (CirculoColideComRetangulo(centro, raio, parede))
        {
            return true;
        }
    }

    for (const Janela &janela : janelas)
    {
        // Janela quebrada deixa de ser obstáculo. É aqui que zumbi e jogador podem passar.
        if (!janela.EstaQuebrada() && CirculoColideComRetangulo(centro, raio, janela.area))
        {
            return true;
        }
    }

    // Zumbi não sobe a escada/entrada. O jogador pode usar esse ponto para entrar e sair.
    if (entidadeEhZumbi && CirculoColideComRetangulo(centro, raio, areaEscada))
    {
        return true;
    }

    return false;
}

void Casa::MoverComColisao(Vector2 &posicao, float raio, Vector2 deslocamento, bool entidadeEhZumbi) const
{
    // Movimento separado por eixo evita que a entidade fique presa nas quinas.
    Vector2 tentativaX = {posicao.x + deslocamento.x, posicao.y};
    if (!CirculoColideComObstaculo(tentativaX, raio, entidadeEhZumbi))
    {
        posicao.x = tentativaX.x;
    }

    Vector2 tentativaY = {posicao.x, posicao.y + deslocamento.y};
    if (!CirculoColideComObstaculo(tentativaY, raio, entidadeEhZumbi))
    {
        posicao.y = tentativaY.y;
    }
}

int Casa::IndiceJanelaMaisProxima(Vector2 ponto) const
{
    int melhorIndice = -1;
    float melhorDistancia = 999999.0f;

    for (int i = 0; i < static_cast<int>(janelas.size()); i++)
    {
        float distancia = Distancia(ponto, janelas[i].Centro());
        if (distancia < melhorDistancia)
        {
            melhorDistancia = distancia;
            melhorIndice = i;
        }
    }

    return melhorIndice;
}

int Casa::IndiceJanelaQuebradaMaisProxima(Vector2 ponto) const
{
    int melhorIndice = -1;
    float melhorDistancia = 999999.0f;

    for (int i = 0; i < static_cast<int>(janelas.size()); i++)
    {
        if (!janelas[i].EstaQuebrada())
            continue;

        float distancia = Distancia(ponto, janelas[i].Centro());
        if (distancia < melhorDistancia)
        {
            melhorDistancia = distancia;
            melhorIndice = i;
        }
    }

    return melhorIndice;
}

int Casa::IndiceJanelaProximaDoPonto(Vector2 ponto, float distanciaMaxima) const
{
    int melhorIndice = -1;
    float melhorDistancia = distanciaMaxima;

    for (int i = 0; i < static_cast<int>(janelas.size()); i++)
    {
        float distancia = Distancia(ponto, janelas[i].Centro());
        if (distancia <= melhorDistancia)
        {
            melhorDistancia = distancia;
            melhorIndice = i;
        }
    }

    return melhorIndice;
}

int Casa::IndiceJanelaNaLinha(Vector2 inicio, Vector2 fim) const
{
    for (int i = 0; i < static_cast<int>(janelas.size()); i++)
    {
        if (LinhaColideComRetangulo(inicio, fim, janelas[i].area))
        {
            return i;
        }
    }

    return -1;
}

bool Casa::LinhaBloqueadaPorParede(Vector2 inicio, Vector2 fim) const
{
    for (const Rectangle &parede : paredes)
    {
        if (LinhaColideComRetangulo(inicio, fim, parede))
        {
            return true;
        }
    }

    return false;
}

bool Casa::PodeAtacarPorEstaJanela(int indiceJanela, Vector2 posicaoJogador, Vector2 posicaoZumbi, float raioZumbi) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
    {
        return false;
    }

    const Janela &janela = janelas[indiceJanela];

    // Janela quebrada é abertura. Se a linha de ataque passa por ela, não bloqueia o dano.
    if (janela.EstaQuebrada())
    {
        return true;
    }

    Rectangle areaMaiorDaJanela = {
        janela.area.x - 36.0f,
        janela.area.y - 36.0f,
        janela.area.width + 72.0f,
        janela.area.height + 72.0f};

    bool jogadorEstaNaJanela = CheckCollisionPointRec(posicaoJogador, areaMaiorDaJanela);
    bool zumbiEstaNaJanela = CirculoColideComRetangulo(posicaoZumbi, raioZumbi, areaMaiorDaJanela);

    return jogadorEstaNaJanela && zumbiEstaNaJanela;
}

void Casa::DanoNaJanela(int indiceJanela, float dano)
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return;

    // Barricada reduz dano recebido. Assim ela não é só vida extra; ela também dá resistência.
    float danoReal = janelas[indiceJanela].EstaReforcada() ? dano * 0.55f : dano;

    janelas[indiceJanela].vida -= danoReal;
    if (janelas[indiceJanela].vida < 0.0f)
    {
        janelas[indiceJanela].vida = 0.0f;
    }
}

bool Casa::JanelaQuebrada(int indiceJanela) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;
    return janelas[indiceJanela].EstaQuebrada();
}

bool Casa::JanelaReforcada(int indiceJanela) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;
    return janelas[indiceJanela].EstaReforcada();
}

bool Casa::JanelaPrecisaReparo(int indiceJanela) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;
    return janelas[indiceJanela].vida < janelas[indiceJanela].vidaMaxima;
}

bool Casa::CirculoEstaSobreJanela(int indiceJanela, Vector2 centro, float raio) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;

    // Usado para evitar que o jogador repare uma janela enquanto está em cima dela.
    // Se a janela voltar a ser colisão nesse momento, o jogador ficaria preso dentro do bloqueio.
    return CirculoColideComRetangulo(centro, raio, janelas[indiceJanela].area);
}

int Casa::IndiceJanelaQuebradaSobreCirculo(Vector2 centro, float raio) const
{
    for (int i = 0; i < static_cast<int>(janelas.size()); i++)
    {
        if (!janelas[i].EstaQuebrada())
            continue;

        if (CirculoColideComRetangulo(centro, raio, janelas[i].area))
        {
            return i;
        }
    }

    return -1;
}

bool Casa::RepararJanela(int indiceJanela)
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;
    if (!JanelaPrecisaReparo(indiceJanela))
        return false;

    // Reparo completo. Se estiver quebrada, volta a bloquear passagem.
    janelas[indiceJanela].vida = janelas[indiceJanela].vidaMaxima;
    return true;
}

bool Casa::ReforcarJanela(int indiceJanela)
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return false;
    if (janelas[indiceJanela].EstaReforcada())
        return false;

    // Barricada: aumenta vida máxima, restaura a janela e reduz dano recebido em DanoNaJanela().
    janelas[indiceJanela].nivelBarricada = 1;
    janelas[indiceJanela].vidaMaxima = 190.0f;
    janelas[indiceJanela].vida = janelas[indiceJanela].vidaMaxima;
    return true;
}

Vector2 Casa::CentroJanela(int indiceJanela) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return {0.0f, 0.0f};
    return janelas[indiceJanela].Centro();
}

Vector2 Casa::PontoInternoDepoisDaJanela(int indiceJanela, float margem) const
{
    if (indiceJanela < 0 || indiceJanela >= static_cast<int>(janelas.size()))
        return {0.0f, 0.0f};

    const Janela &janela = janelas[indiceJanela];
    Vector2 centro = janela.Centro();

    // Este ponto resolve o zumbi travado na janela quebrada.
    // Em vez de mirar no centro da janela, ele mira um ponto já dentro da casa.
    if (janela.area.y <= areaInterna.y)
    {
        return {centro.x, areaInterna.y + margem}; // janela superior
    }

    if (janela.area.y >= areaInterna.y + areaInterna.height - 1.0f)
    {
        return {centro.x, areaInterna.y + areaInterna.height - margem}; // janela inferior
    }

    if (janela.area.x >= areaInterna.x + areaInterna.width - 1.0f)
    {
        return {areaInterna.x + areaInterna.width - margem, centro.y}; // janela direita
    }

    return {areaInterna.x + margem, centro.y}; // caso exista janela esquerda no futuro
}

const std::vector<Janela> &Casa::ObterJanelas() const
{
    return janelas;
}

void Casa::Desenhar(const TexturasJogo &texturas) const
{
    auto desenharTexturaOuRetangulo = [](const Texture2D &textura, Rectangle destino, Color corFallback)
    {
        if (TexturasJogo::TexturaValida(textura))
        {
            Rectangle origem = {0.0f, 0.0f, static_cast<float>(textura.width), static_cast<float>(textura.height)};
            DrawTexturePro(textura, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
        }
        else
        {
            DrawRectangleRec(destino, corFallback);
        }
    };

    auto desenharTexturaRepetidaOuRetangulo = [](const Texture2D &textura, Rectangle area, Color corFallback)
    {
        if (!TexturasJogo::TexturaValida(textura))
        {
            DrawRectangleRec(area, corFallback);
            return;
        }

        // Paredes não devem esticar o sprite.
        // A textura é repetida em blocos e cortada apenas nas sobras das pontas.
        const float larguraTile = static_cast<float>(textura.width);
        const float alturaTile = static_cast<float>(textura.height);

        for (float y = 0.0f; y < area.height; y += alturaTile)
        {
            for (float x = 0.0f; x < area.width; x += larguraTile)
            {
                float larguraParte = std::min(larguraTile, area.width - x);
                float alturaParte = std::min(alturaTile, area.height - y);

                Rectangle origem = {0.0f, 0.0f, larguraParte, alturaParte};
                Rectangle destino = {area.x + x, area.y + y, larguraParte, alturaParte};

                DrawTexturePro(textura, origem, destino, {0.0f, 0.0f}, 0.0f, WHITE);
            }
        }
    };

    auto desenharSpriteOrientadoNaJanela = [](const Texture2D &textura, const Janela &janela, Color corFallback)
    {
        bool janelaHorizontal = janela.area.width >= janela.area.height;

        if (!TexturasJogo::TexturaValida(textura))
        {
            DrawRectangleRec(janela.area, corFallback);
            return;
        }

        Rectangle origem = {0.0f, 0.0f, static_cast<float>(textura.width), static_cast<float>(textura.height)};

        if (janelaHorizontal)
        {
            // Janelas de cima/baixo usam o sprite sem rotação.
            DrawTexturePro(textura, origem, janela.area, {0.0f, 0.0f}, 0.0f, WHITE);
        }
        else
        {
            // Janelas laterais usam o mesmo sprite rotacionado, não esticado.
            // Isso mantém o desenho no formato de janela mesmo quando a área física é vertical.
            Rectangle destinoRotacionado = {
                janela.Centro().x,
                janela.Centro().y,
                janela.area.height,
                janela.area.width};

            Vector2 origemRotacao = {destinoRotacionado.width / 2.0f, destinoRotacionado.height / 2.0f};
            DrawTexturePro(textura, origem, destinoRotacionado, origemRotacao, 90.0f, WHITE);
        }
    };

    // Chão da casa. A colisão continua sendo areaChao/areaInterna; o sprite muda apenas o visual.
    desenharTexturaOuRetangulo(texturas.chaoCasa, areaChao, RAYWHITE);
    DrawRectangleLinesEx(areaChao, 2.0f, BLACK);

    // Paredes. O sprite é repetido sem esticar para manter os tijolos proporcionais.
    for (const Rectangle &parede : paredes)
    {
        desenharTexturaRepetidaOuRetangulo(texturas.parede, parede, GRAY);
    }

    // Escada/entrada inferior. Ela é vertical e ultrapassa levemente a parede.
    // Desenhamos apenas o PNG transparente, sem borda extra, para não aparecer fundo preto.
    desenharTexturaOuRetangulo(texturas.escada, areaEscada, Color{142, 91, 48, 255});

    // Janelas inteiras/quebradas e barricadas.
    for (const Janela &janela : janelas)
    {
        const Texture2D &texturaJanela = janela.EstaQuebrada() ? texturas.janelaQuebrada : texturas.janelaInteira;
        Color corFallback = janela.EstaQuebrada() ? MAROON : SKYBLUE;
        Color corBorda = janela.EstaQuebrada() ? DARKBROWN : BLUE;

        desenharSpriteOrientadoNaJanela(texturaJanela, janela, corFallback);
        DrawRectangleLinesEx(janela.area, 2.0f, corBorda);

        if (janela.EstaReforcada() && !janela.EstaQuebrada())
        {
            desenharSpriteOrientadoNaJanela(texturas.barricada, janela, Color{150, 95, 45, 180});
            DrawRectangleLinesEx(janela.area, 2.0f, BROWN);
        }

        // A barra de vida foi mantida para preservar a leitura do estado da janela.
        float larguraBarra = janela.area.width > janela.area.height ? janela.area.width : 52.0f;
        float alturaBarra = 6.0f;
        float xBarra = janela.Centro().x - larguraBarra / 2.0f;
        float yBarra = janela.area.y - 12.0f;

        if (janela.area.height > janela.area.width)
        {
            xBarra = janela.area.x - 16.0f;
            yBarra = janela.Centro().y - 30.0f;
        }

        float percentualVida = janela.vidaMaxima > 0.0f ? janela.vida / janela.vidaMaxima : 0.0f;
        DrawRectangle(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra), static_cast<int>(alturaBarra), RED);
        DrawRectangle(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra * percentualVida), static_cast<int>(alturaBarra), janela.EstaReforcada() ? GOLD : GREEN);
        DrawRectangleLines(static_cast<int>(xBarra), static_cast<int>(yBarra), static_cast<int>(larguraBarra), static_cast<int>(alturaBarra), BLACK);
    }
}
