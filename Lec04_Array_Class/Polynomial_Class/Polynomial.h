#pragma once
#include <cstdio>

#define MAX_DEGREE 80

class Polynomial {
    int degree;
    float coef[MAX_DEGREE];

public:
    Polynomial();
    void read();
    void display(const char* str = " Poly = ");
    void add(Polynomial a, Polynomial b);
    bool isZero();
    void negate();
};
