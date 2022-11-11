#include <iostream>
#include <conio.h>
#include "large.hpp"

int main() {
    setlocale(LC_ALL, "RU");
    large num1, num2;
    cout << "Первое число: ";
    getline(cin, num1);
    cout << "Первое число: ";
    getline(cin, num2);
    cout << "\nСумма: " << num1 + num2 << endl;
    cout << "Разность: " << num1 - num2 << endl;
    cout << "Произведение: " << num1 * num2 << endl;
    cout << "Частное: " << num1 / num2 << endl;
    cout << "Остаток от деления: " << num1 % num2 << endl;
    cout << "A в степени B: " << num1.pow(num2) << endl;
    _getch();
}
