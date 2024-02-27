﻿#include "07_Polynomial.h"
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Лабораторная №4. Реализовать объект многочлена с одной переменной и функции работы с ним.
 * Список и описание функций приведены ниже. Все функции сделать максимально надежными и подробным выводом в консоль.
 * Полином классический, коэффициенты целые числа со знаком, степени безнаковые целые.
 * Обработать флаг переполнения или неверного значения, если это возможно.
 * Использовать динамическую память. В случае отказа, выйти с exit(-1).
 * */

// Ограничение на глобальный размер данных включая текст, необязятельно, но для учебного примера.
#define DATA_MAX 0x100
#define MEM_MAX 0x8000

// Вспомогательное перечисление для всех частей полинома и операции сравнения.
enum polynomial_extra { poly_nop, poly_con, poly_exp, poly_all, poly_equal, poly_less, poly_more,
                      poly_full, poly_max, poly_min, poly_not};
// Код возвращаемой ошибки
enum errors_type {err_ok = 0, err_incorrect = 1, err_memory = 2, err_convert = 3, err_range = 4, err_not = 5};

struct polynomial {                                         // Структура полинома с одной переменной.
    unsigned char size;                                      // Количество элементов в полиноме.
    char* constants;                                         // Динамический массив констант, целые числа со знаком.
    unsigned char* exponents;                                // Динамический массив степеней, целые числа без знака.
};

// Статичные константы для всей программы.
static const char* errors[] = {"No errors.", "Incorrect parameters.", "Not enough memory.",
                               "Conversion failed.", "Indexes out of range."};
static const unsigned char element_size = sizeof(char) + sizeof(unsigned char);
static const unsigned char rnd_consts_max = 10, rnd_exps_max = 4;

// Статичные переменные для всей программы.
static unsigned short memory = MEM_MAX;
static char element_fmt[] = "[%C^%E]";  // Формат элемента при преобразовании по умолчанию. | [%C^%E]
                                          //разделитель пробел или табуляция

char is_correct_polynomial(struct polynomial* obj)
{   // проверка на пустоту и на NULL, если размер полинома больше нуля то проверить константы или экспоненты
    if (obj != NULL) {
        if ((obj->size == 0 && (obj->constants != NULL || obj->exponents != NULL)) ||
                (obj->size > 0 && (obj->constants == NULL || obj->exponents == NULL))) {
            return err_incorrect;
        } else
            return err_ok;
    } else
        return err_incorrect;
}

unsigned char size_polynomial(struct polynomial* obj)
{   // Возвращение размера полинома в элементах.
    if (is_correct_polynomial(obj) != err_ok) {
        printf("size error: polynomial is incorrect;\n");
        return 0;
    }
    return obj->size;
}

char* get_constants(struct polynomial* obj, unsigned char index)
{   // Получение адреса первой константы полинома. Количество только требуется для проверки.
    if (is_correct_polynomial(obj) != err_ok || size_polynomial(obj) == 0 || index >= size_polynomial(obj)) {
        printf("error get constants: polynomial is incorrect or empty, or wrong index;\n");
        return NULL;
    }
    return &(obj->constants[index]);
}

unsigned char* get_exponents(struct polynomial* obj, unsigned char index)
{   // Получение адреса первой экспоненты полинома. Количество требуется только для проверки.
    if (is_correct_polynomial(obj) != err_ok || size_polynomial(obj) == 0 || index >= size_polynomial(obj)) {
        printf("error get exponents: polynomial is incorrect or empty, or wrong index;\n");
        return NULL;
    }
    return &(obj->exponents[index]);
}

char create_polynomial(struct polynomial* obj, unsigned char size, char* consts,
                       unsigned char* exps, char is_rnd)
{   // Создание полинома, простая инициализация, без добавления элементов.
    if (obj == NULL || size == 0 || (!is_rnd && (consts == NULL || exps == NULL))
            || is_correct_polynomial(obj) != err_ok || size_polynomial(obj) != 0) {
        printf("create error: polynomial addr, constants or exponents are NULL or size is 0, "
               "or polynomial not empty;\n");
        return err_incorrect;
    }
    if (element_size * size > memory) {
        printf("error: unable to create a new object - not enough memory;\n");
        printf("Need memory: %hu;\nMemory free: %hu\n", (short)(element_size * size), memory);
        return err_memory;
    }
    printf("Create polynomial size %hhu, random flag is %hhd, constants(%p) and exponents(%p) sources;\n",
           size, is_rnd, consts, exps);
    obj->size = size;
    obj->constants = (char*)calloc(size, sizeof(char));
    obj->exponents = (unsigned char*)calloc(size, sizeof(unsigned char));
    if (obj->constants == NULL || obj->exponents == NULL) {
        printf("error memory allocating for polynomial constants(%p) or exponents(%p);\n",
               obj->constants, obj->exponents);
        exit(-1);
    }
    // При копировании компонентов можно использовать memcpy.
    for (unsigned char i = 0; i < size; ++i) {
        if (is_rnd) {
            obj->constants[i] = rand() % rnd_consts_max;
            obj->exponents[i] = rand() % rnd_exps_max;
        } else {
            obj->constants[i] = consts[i];
            obj->exponents[i] = exps[i];
        }
    }
    printf("Polynomial created, address %p, constants(%p) or exponents(%p);\n",
           obj, obj->constants, obj->exponents);
    memory -= element_size * size;
    printf("Memory free: %hu\n", memory);
    return err_ok;
}

char copy_polynomial(struct polynomial* dst, struct polynomial* src)
{   // Копирование полинома с полным копированием всех компонентов и динамической памяти в пустом назначении.
    if (is_correct_polynomial(dst) != err_ok || is_correct_polynomial(src) != err_ok
            || src == dst || size_polynomial(src) == 0 || size_polynomial(dst) != 0) {
        printf("copy polynomial error: dst or src is NULL, or src and dst are the same, or size incorrect;\n");
        return err_incorrect;
    }
    char r = create_polynomial(dst, size_polynomial(src), get_constants(src, 0),
                               get_exponents(src, 0), 0);
    if (r == err_ok)
        printf("copy polynomial - ok! from %p to %p, elements %u;\n", src, dst, size_polynomial(src));
    else
        printf("copy polynimail error, can't create copy;\n");
    printf("free memory %hu bytes;\n", memory);
    return r;
}

char destroy_polynomial(struct polynomial* obj)
{   // Уничтожение объекта полинома и освобождение динамической памяти.
    if (is_correct_polynomial(obj) != err_ok || obj->size == 0) {
        printf("destroy error: polynomial is NULL or empty;\n");
        return err_incorrect;
    }
    memory += element_size * obj->size;
    printf("Destroying polynomial - %p;\nMemory free: %hu\n", obj, memory);
    obj->size = 0;
    free(obj->exponents);
    free(obj->constants);
    obj->exponents = NULL;
    obj->constants = NULL;
    return err_ok;
}

char move_polynomial(struct polynomial** dst, struct polynomial** src)
{   // Перемещение полинома, без копирования элементов. Объект назначения если не пустой, то очищается.
    if (dst == NULL || src == NULL || is_correct_polynomial(*dst) != err_ok || is_correct_polynomial(*src) != err_ok) {
        printf("move error: dst or src is NULL;\n");
        return err_incorrect;
    }
    unsigned char src_size = size_polynomial(*src);
    printf("move polynomial from %p to %p size %u;\n", (*src), (*dst), src_size);
    printf("Move from addreses constants from %p to %p, exponents from %p to %p;\n",
           get_constants(*src, 0), get_constants(*dst, 0),
           get_exponents(*src, 0), get_exponents(*dst, 0));
    (*dst)->size = (*src)->size;
    (*dst)->constants = (*src)->constants;
    (*dst)->exponents = (*src)->exponents;
    (*src)->size = 0;
    (*src)->constants = NULL;
    (*src)->exponents = NULL;
    printf("Memory free: %hu\n", memory);
    //(*dst) = (*src);
    return err_ok;
}

char resize_polynomial(struct polynomial* obj, unsigned char new_size)
{   // Изменение размера полинома, если больше, то нули.
    if (is_correct_polynomial(obj) != err_ok || new_size == 0) {
        printf("resize polynomial error: obj is NULL or index incorrect;\n");
        return err_incorrect;
    }
    printf("Resize polynomial %p, constants %p, exponents %p, size %u, new size = %u;\n",
           obj, obj->constants, obj->exponents, size_polynomial(obj), new_size);
    unsigned char size = size_polynomial(obj);
    if (new_size == size) {
        printf("resize polynomial is equal to new size %hhu;\n", new_size);
        return err_ok;
    }
    obj->size = new_size;
    obj->constants = (char*)realloc(obj->constants, new_size * sizeof(char));
    obj->exponents = (unsigned char*)realloc(obj->exponents, new_size * sizeof(unsigned char));
    if (obj->constants == NULL || obj->exponents == NULL) {
        printf("error memory reallocating for polynomial constants(%p) or exponents(%p);\n",
               obj->constants, obj->exponents);
        exit(-1);
    }
    // memset(obj->constants + size, 0, (new_size - size) * sizeof(char));
    // memset(obj->exponents + size, 0, (new_size - size) * sizeof(unsigned char));
    for (unsigned char i = obj->size; i < new_size; ++i) {
            obj->constants[i] = 0;
            obj->exponents[i] = 0;
    }
    memory -= element_size * (new_size - size);
    printf("Memory free: %hu\n", memory);
    return err_ok;
}

// 1 2 3 4 5 6

char set_constants(struct polynomial* obj, char* src, unsigned char src_size, unsigned char index)
{   // Установка констант в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    if (is_correct_polynomial(obj) != err_ok || src == NULL || index >= size_polynomial(obj)
            || src_size == 0) {
        printf("set constants polynomial error: obj is NULL or src or index incorrect;\n");
        return err_incorrect;
    }       // проверку размера полинома отдельно, если ошибка выхода за индекс.
    if (src_size + index >= size_polynomial(obj)) {
        printf("error: outside the polynomial;\n");
        return err_range;
    }
    printf("Set constants(%p) in object (%p), size %u, from index %u;\n", src, obj, src_size, index);
    for (unsigned char i = index; i < src_size + index; ++i)
        obj->constants[i] = src[i];
    return err_ok;
}

char set_exponents(struct polynomial* obj, unsigned char* src, unsigned char src_size, unsigned char index)
{   // Установка экспонент в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    if (is_correct_polynomial(obj) != err_ok || src == NULL || index >= size_polynomial(obj)
            || src_size == 0) {
        printf("set exponents polynomial error: obj is NULL or src or index incorrect;\n");
        return err_incorrect;
    }
    printf("Set exponents(%p) in object (%p), size %u, index %d;\n", src, obj, src_size, index);
    if (src_size + index >= size_polynomial(obj)) {
        printf("error: outside the polynomial;\n");
        return err_range;
    }
    for (unsigned char i = index; i < src_size + index; ++i)
        obj->exponents[i] = src[i];
    return err_ok;
}

char print_polynomial(struct polynomial* obj, unsigned char endl)
{   // Вывод полинома в виде: "3*X^2 - 5*X^7". Дополнительный параметр количество перевода строк после вывода.
    if (is_correct_polynomial(obj) != err_ok) {
        printf("error print: polynomial is incorrect;\n");
        return err_incorrect;
    }
    unsigned char elements = size_polynomial(obj);
    printf("polynomial at %p, size %hhu: ", obj, elements);
    if (elements > 0) {
        char* consts_ptr = get_constants(obj, 0);
        unsigned char* exps_ptr = get_exponents(obj, 0);
        for (unsigned char i = 0; i < elements; ++i)
            printf("%+hhd*(X)^%hhu ", consts_ptr[i], exps_ptr[i]);
    } else
        printf("is empty");
    while (endl--)
        printf("\n");
    return err_ok;
}

int to_string_polynomial(struct polynomial* obj, char* dst)
{   // Преобразование полинома в строку, формат по умолчанию в статичной переменной.
    // Корректность длины строки на стороне вызова. Максимальная длина по памяти. (Использовать spritf, sprintf_s)
    if (is_correct_polynomial(obj) != err_ok || dst == NULL) {
        printf("to string polynomial error: polynomial is incorrect or dst string is NULL;\n");
        return err_incorrect;
    }
    unsigned char size = size_polynomial(obj);
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char i = 0, j = 0, k = 0, len = 0;
    printf("Polynomial(%p) to string(%p) conversion;\n", obj, dst);
    for (i = 0; i < size; ++i) {
        for (j = 0; element_fmt[j] != '\0'; ++j) {
            k = 1;
            if (element_fmt[j] == '%' && element_fmt[j + 1] != '\0') {
                switch (element_fmt[j + 1]) {
                case 'C':
                case 'c':
                    k = sprintf_s(&dst[len], DATA_MAX, "%+hhd", consts[i]);
                    ++j;
                    break;
                case 'E':
                case 'e':
                    k = sprintf_s(&dst[len], DATA_MAX, "%hhu", exps[i]);
                    ++j;
                    break;
                case '%':
                    dst[len] = '%';
                    ++j;
                    break;
                default:
                    printf("char in format error;\n");
                    return err_incorrect;
                }
            } else {
                dst[len] = element_fmt[j];
            }
            if (k < 0) {
                printf("error to string while convert parameters;\n");
                return err_convert;
            }
            len += k;
        }
        if (i < size - 1)
            dst[len++] = ' ';
    }
    dst[len] = '\0';
    return err_ok;
}

int from_string_polynomial(struct polynomial* obj, char* src)
{   // Преобразование строки в полином. Без библиотечных функций, лишние или неверные символы пропускать.
    if (obj == NULL || src == NULL || size_polynomial(obj) > 0) {
        printf("from string polynomial error: polynomial is incorrect or dst string is NULL;\n");
        return err_incorrect;
    }
    unsigned char size = 0;
    printf("\nConverting from string '%s' to polynomial format '%s';\n", src, element_fmt);
    char consts[DATA_MAX];
    unsigned char exps[DATA_MAX];
    for (short i = 0, j = 0, k = 0, is_elem; src[i] != '\0' && i < DATA_MAX;) {
        for (j = 0, is_elem = 1; is_elem && element_fmt[j] != '\0' && src[i] != '\0' && i < DATA_MAX; ++j, ++i) {
            if (element_fmt[j] == '%') {
                if (toupper(element_fmt[j + 1]) == 'C' || toupper(element_fmt[j + 1]) == 'E') {
                    char num_txt[DATA_MAX];
                    k = 0;
                    num_txt[k] = '\0';
                    if (src[i] == '+' || src[i] == '-')
                        num_txt[k++] = src[i];
                    while (i + k < DATA_MAX && src[i + k] >= '0' && src[i + k] <= '9') {
                        num_txt[k] = src[i + k];
                        k++;
                    }
                    if (((num_txt[0] == '-' || num_txt[0] == '+') && k > 1) ||
                            (num_txt[0] != '-' && num_txt[0] != '+' && k > 0)) {
                        num_txt[k] = '\0';
                        short num = (short)atoi(num_txt);
                        printf("'%s' at %hd position, short is %hd, ", num_txt, i, num);
                        ++j;
                        if (toupper(element_fmt[j]) == 'C') {
                            consts[size] = (char)num;
                            printf("const char %hhd;\n", consts[size]);
                        }
                        if (toupper(element_fmt[j]) == 'E') {
                            exps[size] = (unsigned char)num;
                            printf("exps unsigned char %hhu;\n", exps[size]);
                        }
                    } else
                        printf("'%s' at %hd number incorrect;\n", num_txt, i);
                    i += k - 1;
                } else if (element_fmt[j + 1] == '%') {
                    ++j;
                } else {
                    printf("from string error: wrong in element format;\n");
                    return err_convert;
                }
            } else if (element_fmt[j] != src[i]) {
                printf("not equal chars are %c and %c in format ;\n", src[i], element_fmt[j]);
                is_elem = 0;
            }
        }
        if (is_elem)
            printf("elements is founded index %hd, polynomail size is %hd;\n", i, ++size);
    }
    create_polynomial(obj, size, consts, exps, 0);
    return err_ok;
}

void inc_polynomial(struct polynomial* obj, unsigned char index, enum polynomial_extra parts)
{   // Увеличение константы или экспоненты полинома по индексу. Тип увеличения по отдельности или вместе.
    if (is_correct_polynomial(obj) != err_ok || index >= size_polynomial(obj)
            || parts < poly_con || parts > poly_all) {
        printf("inc polynomial error: polynomial is incorrect or dst string is NULL;\n");
        return;
    }
    if (parts == poly_con || parts == poly_all) {
        if ((char)(obj->constants[index] + (char)(1)) == CHAR_MIN)
            printf("warning inc polynomial: constants overflow!;\n");
        obj->constants[index]++;
    }
    if (parts == poly_con || parts == poly_all) {
        if ((unsigned char)(obj->exponents[index] + (unsigned char)(1)) == 0)
            printf("warning inc polynomial: exponents overflow!;\n");
        obj->exponents[index]++;
    }
}

void dec_polynomial(struct polynomial* obj, unsigned char index, enum polynomial_extra parts)
{   // Уменьшение константы или экспоненты полинома по индексу. Тип уменьшения по отдельности или вместе.
    if (is_correct_polynomial(obj) != err_ok || index >= size_polynomial(obj)
            || parts < poly_con || parts > poly_all) {
        printf("dec polynomial error: polynomial is incorrect or dst string is NULL;\n");
        return;
    }
    if (parts == poly_con || parts == poly_all) { // проверка на переполнение
        if ((char)(obj->constants[index] - 1) == CHAR_MAX)
            printf("warning dec polynomial: constants overflow!;\n");
        obj->constants[index]--;
    }
    if (parts == poly_con || parts == poly_all) {
        if ((unsigned char)(obj->exponents[index] - 1) == UCHAR_MAX)
            printf("warning dec polynomial: exponents overflow!;\n");
        obj->exponents[index]--;
    }
}

unsigned char degree(struct polynomial obj, enum polynomial_extra type)
{   // Вычисление степен полинома

}

char add_polynomial(struct polynomial* left, struct polynomial* right)
{   // Сложение полиномовв, правосторонний прибавляется к левоторонниму изменяя его значение.
    // Допустимо сложение одного и того же объекта

}

char sub_polynomial(struct polynomial* left, struct polynomial* right)
{   //

}

char mul_polynomial(struct polynomial* left, struct polynomial* right)
{   // Усножение полиномовв, правосторонний прибавляется к левоторонниму изменяя его значение.
    // Допустимо умножение одного и того же объекта

}

char div_polynomial(struct polynomial* left, struct polynomial* right)
{   //  деление столбиком, возможно с отстатком

}

char compare_polynomial(struct polynomial* left, struct polynomial right, enum polynomial_extra type)
{   // Сравнение полиномов. Размеры должны совпадать. На вход тип сравнения: равно, меньше или больше.

}

char compact_polynomial(struct polynomial* src)
{   // Пока работаем с одним и тем же объектом. Удаляются все элементы, константы которых равны нулю.
    if (is_correct_polynomial(src) != err_ok || size_polynomial(src) == 0) {
        printf("compact polynomial error: polynomial is incorrect;\n");
        return 0;
    }
    unsigned char size = size_polynomial(src), i = 0, compact = 0, k = 0;
    char* consts = get_constants(src, 0);
    unsigned char* exps = get_exponents(src, 0);
    for (i = 0; i < size - compact;) {
        if (consts[i] == 0) {
            for (k = i; k < size - 1; ++k) {
                consts[k] = consts[k + 1];
                exps[k] = exps[k + 1];
            }
            ++compact;
        } else
            ++i;
    }
    if (i == size) {
        printf("nothing to compact, polynomial is ok!\n");
        return size - compact;
    }
    resize_polynomial(src, size - compact);
    printf("compact polynomial %p: new size = %u;\n", src, size);
    return size - compact;
}

double calculate_polynomial(struct polynomial* obj, double value)
{   // Вычисление полинома, обычное целое число. Пока что полином целиком.

}

char to_monic(struct polynomial left)
{   // Преобразование полинома к привелённому виду с потерями данных.

}

char resolve(struct polynomial* obj, double solutions[], double left, double right)
{   // Решение полинома методом деления пополам на заданном интервале.

}

char derivative(struct polynomial left)
{   // производная от полинома

}

char draw_polynomial(struct polynomial left, double scale_x, double scale_y)
{

}

void polynomial()
{
    srand(1);
    printf("Laboratory 4. Polynomials with signed integer constants and unsigned exponents.\n");
    // Таблицу с размерностями
    printf("Size of char is %u, short is %u, int is %u bytes, address %u bits;\n",
           sizeof(char), sizeof(short), sizeof(int), sizeof(char*) * CHAR_BIT);
    // Вывод базовой информации об объетке и размерах его полей.
    struct polynomial poly_a = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial poly_b = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial poly_c = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial* ptr_a = &poly_a;
    struct polynomial* ptr_b = &poly_b;
    struct polynomial* ptr_c = &poly_c;
    printf("Size of structure polynomial %u bytes and information;\n", sizeof(poly_a));
    printf("Offset:\tType:\t\tSize:\t\tComment:\n");
    printf("%u\tUnsigned char\t%u\t\tSize of polynomial in elements;\n",
           (char*)(&poly_a.size) - (char*)(&poly_a), sizeof(poly_a.size));
    printf("%u\tChar pointer\t%u\t\tDynamic array of constants;\n",
           (char*)(&poly_a.constants) - (char*)(&poly_a), sizeof(poly_a.constants));
    printf("%u\tUChar pointer\t%u\t\tDynamic array of exponents;\n",
           (char*)(&poly_a.exponents) - (char*)(&poly_a), sizeof(poly_a.exponents));
    // Создание, копирование, перемещение и уничтожение объектов полином.
    const char data_size = 5;
    char data_consts[] = {3, -1, 0, 0, -6};
    unsigned char data_exponents[] = {1, 3, 0, 2, 1};
    printf("\n\nCreate, copy, move, resize, compact and destroy polynomial objects;\n");
    create_polynomial(&poly_a, data_size, data_consts, data_exponents, 0);
    print_polynomial(&poly_a, 2);
    copy_polynomial(&poly_b, &poly_a);
    print_polynomial(&poly_b, 2);
    move_polynomial(&ptr_c, &ptr_b);
    print_polynomial(ptr_c, 2);
    printf("Check source polynomial after moving, must be empty;\n");
    print_polynomial(ptr_b, 2);
    printf("Resize to size more than source 5 to 7;\n");
    resize_polynomial(ptr_a, 7);
    print_polynomial(ptr_a, 2);
    printf("Compact empty elements from polynomial;\n");
    compact_polynomial(ptr_a);
    print_polynomial(ptr_a, 2);
    printf("Resize to less than source 3 to 1;\n");
    resize_polynomial(ptr_a, 1);
    print_polynomial(ptr_a, 2);
    printf("Destroy this polynomial and free memory;\n");
    destroy_polynomial(ptr_a);
    // Функции конвертации из одного формата в другой и установки констант и экспонент.
    printf("\n\nConversions and sets functions tests;\n");
    printf("Source polynomial to string;\n");
    print_polynomial(ptr_c, 2);
    char txt[DATA_MAX];
    to_string_polynomial(ptr_c, txt);
    printf("Converted string is '%s';\n", txt);
    //char input[DATA_MAX] = "Debug: [-5^3] and [3^+5], plus incorrect [2^+5).";
    char input[DATA_MAX] = "Debug: [-5^3] and [3^+5], plus incorrect [2^+5).";
    from_string_polynomial(ptr_b, input);
    print_polynomial(&poly_b, 2);
    // Чтение, запись, вывод и вычисление полинома.

    // Все арифметические функции.

    // Функции сравнения.

    // Дополнительные функции полинома.
    printf("Free memory before exit %hu bytes;\n", memory);
    destroy_polynomial(&poly_a);
    destroy_polynomial(&poly_b);
    destroy_polynomial(&poly_c);
}




