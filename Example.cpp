#include <iostream>
#include "Large.h"
#include <conio.h>

int main() {
    setlocale(LC_ALL, "RU");
    Large num1, num2;
    std::cout << "First num:";
    std::cin >> num1;
    std::cout << "Second num:";
    std::cin >> num2;
    std::cout << std::endl << "first + second = " << num1 + num2 << std::endl;
    std::cout << "first - second = " << num1 - num2 << std::endl;
    std::cout << "first * second = " << num1 * num2 << std::endl;
    std::cout << "first / second = " << num1 / num2 << std::endl;
    std::cout << "first % second = " << num1 % num2 << std::endl;
    std::cout << "first ^ second = " << pow(num1, num2) << std::endl;
    _getch();
}
