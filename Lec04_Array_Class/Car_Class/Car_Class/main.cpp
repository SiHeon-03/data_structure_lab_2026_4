#include "Car.h"
#include <clocale>

int main() {
    setlocale(LC_ALL, "KOREAN");
    SportsCar subaru;

    printf("--- 스포츠카 가속 테스트 ---\n");

    subaru.setTurbo(false);
    subaru.speedUp();
    subaru.display();

    printf("터보 가동!\n");
    subaru.setTurbo(true);
    subaru.speedUp();
    subaru.display();

    subaru.whereAmI();

    return 0; 
}