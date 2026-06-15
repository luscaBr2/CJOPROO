#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include "raylib.h"
#include <cmath>
#include <string>

// Este arquivo concentra funções pequenas usadas por várias classes.

inline float Comprimento(Vector2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline Vector2 NormalizarSeguro(Vector2 v)
{
    float tamanho = Comprimento(v);
    if (tamanho <= 0.0001f)
    {
        return {0.0f, 0.0f};
    }
    return {v.x / tamanho, v.y / tamanho};
}

inline Vector2 Somar(Vector2 a, Vector2 b)
{
    return {a.x + b.x, a.y + b.y};
}

inline Vector2 Subtrair(Vector2 a, Vector2 b)
{
    return {a.x - b.x, a.y - b.y};
}

inline Vector2 Multiplicar(Vector2 v, float escala)
{
    return {v.x * escala, v.y * escala};
}

inline float Distancia(Vector2 a, Vector2 b)
{
    return Comprimento(Subtrair(a, b));
}

inline float LimitarFloat(float valor, float minimo, float maximo)
{
    if (valor < minimo)
        return minimo;
    if (valor > maximo)
        return maximo;
    return valor;
}

inline bool CirculoColideComRetangulo(Vector2 centro, float raio, Rectangle retangulo)
{
    // Encontra o ponto do retângulo mais próximo do centro do círculo.
    float pontoX = LimitarFloat(centro.x, retangulo.x, retangulo.x + retangulo.width);
    float pontoY = LimitarFloat(centro.y, retangulo.y, retangulo.y + retangulo.height);

    float dx = centro.x - pontoX;
    float dy = centro.y - pontoY;

    return (dx * dx + dy * dy) <= (raio * raio);
}

inline bool SegmentosSeCruzam(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4)
{
    auto orientacao = [](Vector2 a, Vector2 b, Vector2 c)
    {
        return (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
    };

    float o1 = orientacao(p1, p2, p3);
    float o2 = orientacao(p1, p2, p4);
    float o3 = orientacao(p3, p4, p1);
    float o4 = orientacao(p3, p4, p2);

    return (o1 * o2 < 0.0f) && (o3 * o4 < 0.0f);
}

inline bool LinhaColideComRetangulo(Vector2 inicio, Vector2 fim, Rectangle retangulo)
{
    // Primeiro testa se uma das pontas está dentro do retângulo.
    if (CheckCollisionPointRec(inicio, retangulo) || CheckCollisionPointRec(fim, retangulo))
    {
        return true;
    }

    Vector2 a = {retangulo.x, retangulo.y};
    Vector2 b = {retangulo.x + retangulo.width, retangulo.y};
    Vector2 c = {retangulo.x + retangulo.width, retangulo.y + retangulo.height};
    Vector2 d = {retangulo.x, retangulo.y + retangulo.height};

    return SegmentosSeCruzam(inicio, fim, a, b) ||
           SegmentosSeCruzam(inicio, fim, b, c) ||
           SegmentosSeCruzam(inicio, fim, c, d) ||
           SegmentosSeCruzam(inicio, fim, d, a);
}

inline std::string FormatarTempo(float segundos)
{
    int total = static_cast<int>(segundos);
    int minutos = total / 60;
    int restoSegundos = total % 60;

    std::string textoSegundos = (restoSegundos < 10 ? "0" : "") + std::to_string(restoSegundos);
    return std::to_string(minutos) + ":" + textoSegundos;
}

#endif
