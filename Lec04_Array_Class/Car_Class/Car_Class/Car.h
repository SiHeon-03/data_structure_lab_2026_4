#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>

class Car {
protected:
    int speed;
    char name[40];
public:
    int gear;

    Car() : speed(0), gear(1) {
        name[0] = '\0';
    }

    Car(int s, const char* n, int g) : speed(s), gear(g) {
        strcpy(name, n);
    }

    void changeGear(int g = 4) { gear = g; }
    void speedUp() { speed += 5; }
    void display() {
        printf("[%s] : 기어=%d단 속도=%dkmph\n", name, gear, speed);
    }

    void whereAmI() {
        printf("객체 주소 = %p\n", (void*)this);
    }
};

class SportsCar : public Car {
public:
    bool bTurbo;
    SportsCar() : bTurbo(false) {} 

    void setTurbo(bool bTur) { bTurbo = bTur; }
    void speedUp() {
        if (bTurbo) speed += 20;
        else Car::speedUp();
    }
};