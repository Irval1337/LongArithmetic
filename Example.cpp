#include <iostream>
#include <conio.h>
#include "large.hpp"

int main() {
    setlocale(LC_ALL, "RU");
    string num1, num2;
    cout << "Первое число: ";
    getline(cin, num1);
    cout << "Первое число: ";
    getline(cin, num2);
    cout << "\nСумма: " << (large(num1) + large(num2)).ToString() << endl;
    cout << "Разность: " << (large(num1) - large(num2)).ToString() << endl;
    cout << "Произведение: " << (large(num1) * large(num2)).ToString() << endl;
    cout << "Частное: " << (large(num1) / large(num2)).ToString() << endl;
    cout << "Остаток от деления: " << (large(num1) % large(num2)).ToString() << endl;
    cout << "A в степени B: " << (large(num1).pow(large(num2))).ToString() << endl;
    _getch();
}