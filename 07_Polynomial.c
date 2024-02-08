#include "07_Polynomial.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/* Лабораторная №4. Реализовать объект многочлена с одной переменной и функции работы с ним.
 * Список и описание функций приведены ниже. Все функции сделать максимально надежными и подробным выводом в консоль.
 * Полином классический, коэффициенты целые числа со знаком, степени безнаковые целые.
 * Обработать флаг переполнения или неверного значения, если это возможно.
 * Использовать динамическую память. В случае отказа, выйти с exit(-1).
 * */

// Ограничение на глобальный размер данных включая текст, необязятельно, но для учебного примера.
#define DATA_SIZE 0x100
#define MEM_MAX 0x8000

// Вспомогательное перечисление для всех частей полинома и операции сравнения.
enum polynomial_extra { poly_nop, poly_con, poly_exp, poly_all, poly_equal, poly_less, poly_more };
// Код возвращаемой ошибки
enum errors_type {err_ok = 0, err_incorrect = 1, err_memory = 2, err_not = 3};
//5*x + 3*x^0  // [5,3] [1,0]

struct polynomial {                                         // Структура полинома с одной переменной.
    unsigned char size;                                      // Количество элементов в полиноме.
    char* constants;                                         // Динамический массив констант, целые числа со знаком.
    unsigned char* exponents;                                // Динамический массив степеней, целые числа без знака.
};

static unsigned short memory = MEM_MAX;
static const char* errors[] = {"No errors.", "Incorrect parameters.", "Not enough memory."};
static const unsigned char element_size = sizeof(char) + sizeof(unsigned char);
static const unsigned char rnd_consts_max = 10, rnd_exps_max = 4;

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

void set_constants(struct polynomial* obj, char* src, unsigned char index, unsigned char src_size)
{   // Установка констант в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    // Для изменеия размера вызвать функцию.

}

void set_exponents(struct polynomial* obj, unsigned char* src, unsigned char index, unsigned char src_size)
{   // Установка экспонент в полином. Стартовый индекс и количество элементов в еём из исходного массива.

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

void resize_polynomial(struct polynomial* obj, unsigned char index, unsigned char size)
{   // Изменение размера полинома, начиная от индекса и плюс размер.
    if (is_correct_polynomial(obj) != err_ok || index >= obj->size) {
        printf("resize polynomial error: obj is NULL or index incorrect;\n");
        return;
    }
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

int to_string_polynomail(struct polynomial* obj, char* dst)
{   // Преобразование полинома в строку, формат по умолчанию.

}

int from_string_polynomial(struct polynomial* obj, char* src)
{   // Преобразование строки в полином. Без библиотечных функций, лишние или неверные символы пропускать.

}

void inc_polynomial(struct polynomial* obj, unsigned char index, enum polynomial_extra parts)
{   // Увеличение константы или экспоненты полинома по индексу. Тип увеличения по отдельности или вместе.

}

void dec_polynomial(struct polynomial* obj, unsigned char index, enum polynomial_extra parts)
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

char compact_polynomial(struct polynomial* src)
{

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
    char data_consts[] = {3, -1, 2, 5, -6};
    unsigned char data_exponents[] = {1, 3, 0, 2, 1};
    printf("\n\nCreate, copy, move, resize, compact and destroy polynomial objects;\n");
    create_polynomial(&poly_a, data_size, data_consts, data_exponents, 0);
    print_polynomial(&poly_a, 2);
    copy_polynomial(&poly_b, &poly_a);
    print_polynomial(&poly_b, 2);
    move_polynomial(&ptr_c, &ptr_b);
    print_polynomial(ptr_c, 2);
    //destroy_polynomial(&poly_a);
    // Чтение, запись, вывод и вычисление полинома.

    // Функции конвертации из одного формата в другой.

    // Все арифметические функции.

    // Функции сравнения.

    // Дополнительные функции полинома.
    printf("Free memory before exit %hu bytes;\n", memory);
}
