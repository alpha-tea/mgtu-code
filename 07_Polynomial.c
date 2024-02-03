#include "07_Polynomial.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/* Лабораторная №4. Реализовать объект многочлена с одной переменной и функции работы с ним.
 * Список и описание функций приведены ниже. Все функции сделать максимально надежными и подробным выводом в консоль.
 * Полином классический, коэффициенты целые числа со знаком, степени безнаковые целые.
 * Обработать флаг переполнения или неверного значения, если это возможно.
 * Использовать динамическую память. В случае отказа, выйти с exit(-1).
 *
 * */

// Ограничение на глобальный размер данных включая текст, необязятельно, но для учебного примера.
#define MAX_DATA_SIZE 0x100

// Вспомогательное перечисление для всех частей полинома и операции сравнения.
enum polynomial_extra { poly_nop, poly_con, poly_exp, poly_all, poly_equal, poly_less, poly_more };

//5*x + 3*x^0  // [5,3] [1,0]

struct polynomial {                                         // Структура полинома с одной переменной.
    unsigned int size;                                      // Количество элементов в полиноме.
    int* constants;                                         // Динамический массив констант, целые числа со знаком.
    unsigned int* exponents;                                // Динамический массив степеней, целые числа без знака.
};

void create_polynomial(struct polynomial* obj)
{   // Создание полинома, простая инициализация, без добавления элементов.

}

unsigned int size_polynomial(struct polynomial* obj)
{   // Возвращение размера полинома в элементах.

}

void copy_polynomial(struct polynomial* dst, struct polynomial* src)
{   // Копирование полинома с полным копированием всех компонентов и динамической памяти в пустом назначении.

}

void destroy_polynomial(struct polynomial* obj)
{   // Уничтожение объекта полинома и освобождение динамической памяти.

}

void move_polynomial(struct polynomial** dst, struct polynomial** src)
{   // Перемещение полинома, без копирования элементов. Объект назначения если не пустой, то очищается.

}

void resize_polynomial(struct polynomial* obj, unsigned int index, unsigned int size)
{   // Изменение размера полинома, начиная от индекса и плюс размер.

}

void set_constants(struct polynomial* obj, int* src, unsigned int index, unsigned int size)
{   // Установка констант в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    // Для изменеия размера вызвать функцию.

}

void set_exponents(struct polynomial* obj, unsigned int* src, unsigned int index, unsigned int size)
{   // Установка экспонент в полином. Стартовый индекс и количество элементов в еём из исходного массива.

}

int* get_constants(struct polynomial* obj, unsigned int index, unsigned int size)
{   // Получение адреса первой константы полинома. Количество только требуется для проверки.

}

unsigned int* get_exponents(struct polynomial* obj, unsigned int index, unsigned int size)
{   // Получение адреса первой экспоненты полинома. Количество требуется только для проверки.

}

void print_polynomial(struct polynomial* obj, unsigned int endl)
{   // Вывод полинома в виде: "3*X^2 - 5*X^7". Дополнительный параметр количество перевода строк после вывода.

}

int to_string_polynomail(struct polynomial* obj, char* dst)
{   // Преобразование полинома в строку, формат по умолчанию.

}

int from_string_polynomial(struct polynomial* obj, char* src)
{   // Преобразование строки в полином. Без библиотечных функций, лишние или неверные символы пропускать.

}

void inc_polynomial(struct polynomial* obj, unsigned int index, enum polynomial_extra parts)
{   // Увеличение константы или экспоненты полинома по индексу. Тип увеличения по отдельности или вместе.

}

void dec_polynomial(struct polynomial* obj, unsigned int index, enum polynomial_extra parts)
{   // Уменьшение константы или экспоненты полинома по индексу. Тип уменьшения по отдельности или вместе.

}

void add_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

void sub_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

void mul_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

void div_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

void mod_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

int compare_polynomial(struct polynomial* left, struct polynomial right, enum polynomial_extra type)
{   // Сравнение полиномов. Размеры должны совпадать. На вход тип сравнения: равно, меньше или больше.

}

int calculate_polynomial(struct polynomial* obj)
{   // Вычисление полинома, обычное целое число. Пока что полином целиком.

}

// Дополнительные фукнции для полинома, подумать еще.
// Проверка является-ли полином - мономом.
// Является-ли элемент свободным.
// Вычисление степени и полной степени полинома.
// Оптимизировать полином оставив только носители.
// Приведение полинома к нормированному виду.
// Является ли полином однородным.
// Нарисовать график по заданным параметрам, попробовать типовые.

int polynomial()
{
    printf("Laboratory 4. Polynomials with signed integer constants and unsigned exponents.\n");
    // Вывод базовой информации об объетке, по аналогии с рациональным числом.

    // Создание, копирование, перемещение и уничтожение объектов полином.

    // Чтение, запись, вывод и вычисление полинома.

    // Функции конвертации из одного формата в другой.

    // Все арифметические функции.

    // Функции сравнения.

    // Дополнительные функции полинома.
}
