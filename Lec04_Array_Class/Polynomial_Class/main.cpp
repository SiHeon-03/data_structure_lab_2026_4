#define _CRT_SECURE_NO_WARNINGS
#include "Polynomial.h"
#include <cstdio>
#include <clocale>

Polynomial::Polynomial() {
    degree = 0;
    for (int i = 0; i < MAX_DEGREE; i++) {
        coef[i] = 0.0f;
    }
}

void Polynomial::read() {
    printf("다항식의 최고 차수를 입력하시오: ");
    if (scanf("%d", &degree) != 1) return;
    printf("각 항의 계수를 입력하시오 (총 %d개): ", degree + 1);
    for (int i = 0; i <= degree; i++) {
        scanf("%f", coef + i);
    }
}

void Polynomial::display(const char* str) {
    printf("\t%s", str);
    for (int i = 0; i < degree; i++) {
        printf("%5.1f x^%d + ", coef[i], degree - i);
    }
    printf("%4.1f\n", coef[degree]);
}

void Polynomial::add(Polynomial a, Polynomial b) {
    if (a.degree > b.degree) {
        *this = a;
        for (int i = 0; i <= b.degree; i++) {
            coef[i + (degree - b.degree)] += b.coef[i];
        }
    }
    else {
        *this = b;
        for (int i = 0; i <= a.degree; i++) {
            coef[i + (degree - a.degree)] += a.coef[i];
        }
    }
}

bool Polynomial::isZero() {
    return degree == 0;
}

void Polynomial::negate() {
    for (int i = 0; i <= degree; i++) {
        coef[i] = -coef[i];
    }
}

int main() {
    setlocale(LC_ALL, "KOREAN");

    Polynomial a, b, c;
    a.read();
    b.read();
    c.add(a, b);

    a.display("A = ");
    b.display("B = ");
    c.display("A + B = ");

    return 0;
}