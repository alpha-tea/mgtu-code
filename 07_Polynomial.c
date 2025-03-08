#include "07_Polynomial.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Лабораторная №4. Реализовать объект многочлена с одной переменной и функции работы с ним.
 * Список и описание функций приведены ниже. Все функции сделать максимально надежными и подробным выводом в консоль.
 * Полином классический, коэффициенты целые числа со знаком, степени безнаковые целые.
 * Обработать флаг переполнения или неверного значения, если это возможно.
 * Использовать динамическую память. В случае отказа, выйти с exit(-1).
 * Ограничения для полинома 8 бит для элементов, условное ограничение по памяти - 15 бит, знак - флаг переполнения.
 * Компиляция и запуск GCC, MSVS, x86-64, ограничения к условным 16 бит.
 * */

// Ограничение на глобальный размер данных включая текст, необязятельно, но для учебного примера.
#define DATA_MAX 0x100
#define MEM_MAX 0x0400

// Вспомогательное перечисление для всех частей полинома и операции сравнения.
enum flags : unsigned short {
    flag_ok, ply_nop = 0x0000, err_incorrect = 0x0001, err_memory = 0x0002, err_convert = 0x0003, err_range = 0x0004,
    err_compare = 0x0005,
    ply_constant = 0x0008, ply_exponent = 0x0010, ply_equal = 0x0020,
    ply_less = 0x0040, ply_more = 0x0080, ply_full = 0x0100, ply_max = 0x0200, ply_min = 0x0400,
    ply_rand = 0x0800, ply_compact = 0x1000
};

struct polynomial {                                         // Структура полинома с одной переменной.
    unsigned char size;                                      // Количество элементов в полиноме.
    char* constants;                                         // Динамический массив констант, целые числа со знаком.
    unsigned char* exponents;                                // Динамический массив степеней, целые числа без знака.
};

// Статичные константы для всей программы.
static const char* ply_flags_text[] = {"No errors.", "Incorrect parameters.", "Not enough memory.",
                                       "Conversion failed.", "Indexes out of range."};
static const unsigned char element_size = sizeof(char) + sizeof(unsigned char);
static const unsigned char rnd_consts_max = 5, rnd_exps_max = 3;
static const double epsilon = 0.001;    // Заданная точность или приближение.

// Статичные переменные для всей программы.
static unsigned short memory = MEM_MAX;
static char element_fmt[] = "[%C%%%E]";  // Формат элемента при преобразовании по умолчанию. | [%C^%E]
//разделитель пробел или табуляция

unsigned short to_hw(void* ptr)
{
    return ((unsigned short)((long long)ptr));      // Преобразование адреса для удобства вывода.
}

char is_correct_polynomial(struct polynomial* obj)
{   // проверка на пустоту и на NULL, если размер полинома больше нуля то проверить константы или экспоненты
    if (obj != NULL) {
        if ((obj->size == 0 && (obj->constants != NULL || obj->exponents != NULL)) ||
            (obj->size > 0 && (obj->constants == NULL || obj->exponents == NULL))) {
            return err_incorrect;
        } else
            return flag_ok;
    } else
        return err_incorrect;
}

unsigned char size_polynomial(struct polynomial* obj)
{   // Возвращение размера полинома в элементах.
    if (is_correct_polynomial(obj) != flag_ok) {
        printf("size error: polynomial [%X] is incorrect;\n", to_hw(obj));
        return 0;
    }
    return obj->size;
}

char* get_constants(struct polynomial* obj, unsigned char index)
{   // Получение адреса первой константы полинома. Количество только требуется для проверки.
    if (is_correct_polynomial(obj) != flag_ok || index >= size_polynomial(obj)) {
        printf("error get constants: polynomial [%X] is incorrect or empty, or wrong index;\n",
               to_hw(obj));
        return NULL;
    }
    return &(obj->constants[index]);
}

unsigned char* get_exponents(struct polynomial* obj, unsigned char index)
{   // Получение адреса первой экспоненты полинома. Количество требуется только для проверки.
    if (is_correct_polynomial(obj) != flag_ok || index >= size_polynomial(obj)) {
        printf("error get exponents: polynomial [%X] is incorrect or empty, or wrong index;\n",
               to_hw(obj));
        return NULL;
    }
    return &(obj->exponents[index]);
}

enum flags create_polynomial(struct polynomial* obj, unsigned char size, char* consts,
                             unsigned char* exps, enum flags is_rnd)
{   // Создание полинома, простая инициализация, без добавления элементов.
    if (obj == NULL || size == 0 || (!(is_rnd & ply_rand) && (consts == NULL || exps == NULL))
        || is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) != 0) {
        printf("create error: polynomial [%X] addr, constants or exponents are NULL or size is 0, "
               "or polynomial not empty;\n", to_hw(obj));
        return err_incorrect;
    }
    if (element_size * size > memory) {
        printf("error: unable to create a new object - not enough memory;\n");
        printf("Need memory: %hu;\nMemory free: %hu\n", (short)(element_size * size), memory);
        return err_memory;
    }
    printf("Create polynomial [%X] size %hhu, random flag is %hX, constants[%X] and exponents[%X] sources;\n",
           to_hw(obj), size, is_rnd, to_hw(consts), to_hw(exps));
    obj->constants = (char*)calloc(size, sizeof(char));
    obj->exponents = (unsigned char*)calloc(size, sizeof(unsigned char));
    if (obj->constants == NULL || obj->exponents == NULL) {
        printf("Error memory allocating for polynomial, exit(-1);\n");
        exit(-1);
    }
    // При копировании компонентов можно использовать memcpy.
    for (unsigned char i = 0; i < size; ++i) {
        if (is_rnd & ply_rand) {
            obj->constants[i] = rand() % rnd_consts_max;
            obj->exponents[i] = rand() % rnd_exps_max;
        } else {
            obj->constants[i] = consts[i];
            obj->exponents[i] = exps[i];
        }
    }
    obj->size = size;
    memory -= element_size * size;
    printf("Polynomial created, address [%X], constants [%X] or exponents [%X], memory free: %hu\n",
           to_hw(obj), to_hw(obj->constants), to_hw(obj->exponents), memory);
    return flag_ok;
}

enum flags copy_polynomial(struct polynomial* dst, struct polynomial* src)
{   // Копирование полинома с полным копированием всех компонентов и динамической памяти в пустом назначении.
    if (is_correct_polynomial(dst) != flag_ok || is_correct_polynomial(src) != flag_ok
        || src == dst || size_polynomial(src) == 0 || size_polynomial(dst) != 0) {
        printf("copy error: polynomial dst [%X] or polynomial src [%X] is incorrect,"
               " or src and dst are the same, or size incorrect;\n", to_hw(dst), to_hw(src));
        return err_incorrect;
    }
    enum flags r = create_polynomial(dst, size_polynomial(src), get_constants(src, 0),
                                     get_exponents(src, 0), 0);
    if (r == flag_ok)
        printf("Copy polynomial - ok! from [%X] to [%X], elements %hhu, free memory %hu bytes;\n",
               to_hw(src), to_hw(dst), size_polynomial(src), memory);
    else
        printf("Copy polynimail error, can't create copy;\n");
    return r;
}

enum flags destroy_polynomial(struct polynomial* obj)
{   // Уничтожение объекта полинома и освобождение динамической памяти.
    if (is_correct_polynomial(obj) != flag_ok || obj->size == 0) {
        printf("destroy error: polynomial [%X] is incorrect or empty;\n", to_hw(obj));
        return err_incorrect;
    }
    memory += element_size * obj->size;
    printf("Destroying polynomial - [%X], size %hhu bytes, memory free: %hu\n",
           to_hw(obj), obj->size, memory);
    obj->size = 0;
    free(obj->exponents);
    free(obj->constants);
    obj->exponents = NULL;
    obj->constants = NULL;
    return flag_ok;
}

enum flags move_polynomial(struct polynomial** dst, struct polynomial** src)
{   // Перемещение полинома, без копирования элементов. Объект назначения если не пустой, то очищается.
    if ((dst == NULL || src == NULL)) {
        printf("move error: dst [%X] or src [%X] is NULL;\n", to_hw(dst), to_hw(src));
        return err_incorrect;
    }
    if (is_correct_polynomial(*dst) != flag_ok || is_correct_polynomial(*src) != flag_ok
        || *dst == *src || size_polynomial(*src) == 0 || size_polynomial(*dst) != 0) {
        printf("Error move: src empty or dst no empty or addresses equal;\n");
        return err_incorrect;
    }
    unsigned char src_size = size_polynomial(*src);
    printf("Move polynomial from [%X] to [%X] size %hhu;\n", to_hw(*src), to_hw(*dst), src_size);
    printf("Move from addreses constants from [%X] to [%X], exponents from [%X] to [%X],"
           " memory free: %hu\n", to_hw(get_constants(*src, 0)), to_hw(NULL),
           to_hw(get_exponents(*src, 0)), to_hw(NULL), memory);
    (*dst)->size = (*src)->size;
    (*dst)->constants = (*src)->constants;
    (*dst)->exponents = (*src)->exponents;
    (*src)->size = 0;
    (*src)->constants = NULL;
    (*src)->exponents = NULL;
    //(*dst) = (*src);
    return flag_ok;
}

enum flags resize_polynomial(struct polynomial* obj, unsigned char new_size)
{   // Изменение размера полинома, если больше, то нули.
    if (is_correct_polynomial(obj) != flag_ok || new_size == 0) {
        printf("resize error: polynomial [%X] is incorrect or new size is 0, use destroy instead;\n",
               to_hw(obj));
        return err_incorrect;
    }
    unsigned char size = size_polynomial(obj);
    printf("Resize polynomial [%X], constants [%X], exponents [%X], size %u, new size = %u, ",
           to_hw(obj), to_hw(obj->constants), to_hw(obj->exponents), size, new_size);
    if (new_size == size) {
        printf("resize polynomial is equal to new size %hhu;\n", new_size);
        return err_incorrect;
    }
    obj->constants = (char*)realloc(obj->constants, new_size * sizeof(char));
    obj->exponents = (unsigned char*)realloc(obj->exponents, new_size * sizeof(unsigned char));
    if (obj->constants == NULL || obj->exponents == NULL) {
        printf("Error memory reallocating for polynomial constants[%X] or exponents[%X];\n",
               to_hw(obj->constants), to_hw(obj->exponents));
        exit(-1);
    }
    // memset(obj->constants + size, 0, (new_size - size) * sizeof(char));
    // memset(obj->exponents + size, 0, (new_size - size) * sizeof(unsigned char));
    for (unsigned char i = obj->size; i < new_size; ++i) {
        obj->constants[i] = 0;
        obj->exponents[i] = 0;
    }
    obj->size = new_size;
    memory -= element_size * (new_size - size);
    printf("memory: %hu\n", memory);
    return flag_ok;
}

// 1 2 3 4 5 6

enum flags set_constants(struct polynomial* obj, char* src, unsigned char src_size, unsigned char index)
{   // Установка констант в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    if (is_correct_polynomial(obj) != flag_ok || src == NULL || src_size == 0) {
        printf("error set constants: polynomial [%X] is incorrect or src is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    unsigned char poly_size = size_polynomial(obj);
    if (src_size + index > poly_size) {
        printf("error set constants: outside the polynomial [%X];\n", to_hw(obj));
        return err_range;
    }
    printf("Set constants [%X] in object [%X], size %hhu, from index %hhu: ",
           to_hw(src), to_hw(obj), src_size, index);
    for (unsigned char i = 0; i < src_size; ++i) {
        obj->constants[i + index] = src[i];
        printf("%hhd[%hhu] ", src[i], (unsigned char)(index + i));
    }
    printf("\n");
    return flag_ok;
}

enum flags set_exponents(struct polynomial* obj, unsigned char* src, unsigned char src_size, unsigned char index)
{   // Установка экспонент в полином. Стартовый индекс и количество элементов в нём из исходного массива.
    if (is_correct_polynomial(obj) != flag_ok || src == NULL || src_size == 0) {
        printf("error set exponents: polynomial [%X] is incorrect or src is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    unsigned char poly_size = size_polynomial(obj);
    if (src_size + index > poly_size) {
        printf("error set exponents: outside the polynomial;\n");
        return err_range;
    }
    printf("Set exponents [%X] in object [%X], size %hhu, from index %hhu: ",
           to_hw(src), to_hw(obj), src_size, index);
    for (unsigned char i = 0; i < src_size; ++i) {
        obj->exponents[i + index] = src[i];
        printf("%hhu[%hhu] ", src[i], (unsigned char)(index + i));
    }
    printf("\n");
    return flag_ok;
}

enum flags print_polynomial(struct polynomial* obj, unsigned char endl)
{   // Вывод полинома в виде: "3*X^2 - 5*X^7". Дополнительный параметр количество перевода строк после вывода.
    if (is_correct_polynomial(obj) != flag_ok) {
        printf("error print: polynomial [%X] is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    unsigned char elements = size_polynomial(obj);
    printf("polynomial at [%X], size %hhu: ", to_hw(obj), elements);
    if (elements > 0) {
        char* consts_ptr = get_constants(obj, 0);
        unsigned char* exps_ptr = get_exponents(obj, 0);
        for (unsigned char i = 0; i < elements; ++i)
            printf("%+hhd*(X)^%hhu ", consts_ptr[i], exps_ptr[i]);
    } else
        printf("is empty");
    while (endl--)
        printf("\n");
    return flag_ok;
}

enum flags to_string_polynomial(struct polynomial* obj, char* dst)
{   // Преобразование полинома в строку, формат по умолчанию в статичной переменной.
    // Корректность длины строки на стороне вызова. Максимальная длина по памяти. (Использовать spritf, sprintf_s)
    if (is_correct_polynomial(obj) != flag_ok || dst == NULL) {
        printf("to string error: polynomial [%X] is incorrect or dst string is NULL;\n", to_hw(obj));
        return err_incorrect;
    }
    unsigned char size = size_polynomial(obj);
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned short len = 0;
    printf("Polynomial[%X] to string[%X] conversion;\n", to_hw(obj), to_hw(dst));
    for (short i = 0, j, k; i < size; ++i) {
        for (j = 0, k = 1; element_fmt[j] != '\0'; ++j, k = 1) {
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
            if (k == -1) {
                printf("Error to string while convert parameters, with 'sprintf_s';\n");
                return err_convert;
            }
            len += k;
        }
        if (i < size - 1)
            dst[len++] = ' ';
    }
    dst[len] = '\0';
    return flag_ok;
}

short from_string_polynomial(struct polynomial* obj, char* src)
{   // Преобразование строки в полином. Без библиотечных функций, лишние или неверные символы пропускать.
    if (obj == NULL || src == NULL || size_polynomial(obj) > 0) {
        printf("from string error: polynomial [%X] is incorrect or dst string is NULL;\n", to_hw(obj));
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
    return flag_ok;
}

enum flags inc_polynomial(struct polynomial* obj, unsigned char index, unsigned char count, enum flags parts)
{   // Увеличение константы или экспоненты полинома по индексу. Тип увеличения по отдельности или вместе.
    if (is_correct_polynomial(obj) != flag_ok || !((parts & ply_constant) || (parts & ply_exponent))) {
        printf("error inc: polynomial [%X] is incorrect or flags not sets;\n", to_hw(obj));
        return err_incorrect;
    }
    if (index + count > size_polynomial(obj)) {
        printf("error inc: polynomial [%X], index %hhu, counter %hhu is out of range;\n",
               to_hw(obj), index, count);
        return err_range;
    }
    printf("Inc polynomial [%X] from index %hhu, counter %hhu, flags %X in hex;\n",
           to_hw(obj), index, count, parts);
    while (count--) {
        if ((parts & ply_constant) != 0) {
            char* consts = get_constants(obj, index);
            if (*consts == CHAR_MAX)
                printf("warning inc polynomial: constants at %hhu overflow!;\n", index);
            (*consts)++;
        }
        if ((parts & ply_exponent) != 0) {
            unsigned char* exps = get_exponents(obj, index);
            if (*exps == UCHAR_MAX)
                printf("warning inc polynomial: exps at %hhu overflow!;\n", index);
            (*exps)++;
        }
        index++;
    }
    return flag_ok;
}

short dec_polynomial(struct polynomial* obj, unsigned char index, unsigned char count, enum flags parts)
{   // Уменьшение константы или экспоненты полинома по индексу. Тип уменьшения по отдельности или вместе.
    if (is_correct_polynomial(obj) != flag_ok  || !(parts & ply_constant || parts & ply_exponent)) {
        printf("error dec: polynomial [%X] is incorrect or flags not sets;\n", to_hw(obj));
        return err_incorrect;
    }
    if (index + count > size_polynomial(obj)) {
        printf("error dec: polynomial [%X], index %hhu, counter %hhu is out of range;\n",
               to_hw(obj), index, count);
        return err_range;
    }
    printf("Dec polynomial [%X] from index %hhu, counter %hhu, flags %X in hex;\n",
           to_hw(obj), index, count, parts);
    while (count--) {
        if ((parts & ply_constant) != 0) {
            char* consts = get_constants(obj, index);
            if (*consts == CHAR_MIN)
                printf("warning dec polynomial: constants at %hhu overflow!;\n", index);
            (*consts)--;
        }
        if ((parts & ply_exponent) != 0) {
            unsigned char* exps = get_exponents(obj, index);
            if (*exps == 0)
                printf("warning dec polynomial: exps at %hhu overflow!;\n", index);
            (*exps)--;
        }
        index++;
    }
    return flag_ok;
}

unsigned char degree(struct polynomial* obj, enum flags type)
{   // Вычисление степеней полинома
    // ply_max - максимальная степень при x не равной нулю.
    // ply_min - минимальная степень при x не равной нулю.
    // ply_full - полная степень. Сумма всех степеней при x не равной нулю.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0 ||
        !(type & ply_full || type & ply_max || type & ply_min)) {
        printf("degree polynomial error: polynomial ot type is incorrect;\n");
        return err_incorrect;
    }
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    unsigned char max_deg = 0, min_deg = UCHAR_MAX, sum_deg = 0, counter = 0;
    printf("Calculating the degrees of a polynomial[%hX];", obj);
    for (unsigned char i = 0; i < size; ++i)
        if (consts[i] != 0) {
            if (exps[i] < min_deg)
                min_deg = exps[i];
            if (exps[i] > max_deg)
                max_deg = exps[i];
            sum_deg += exps[i];
            counter++;
        }
    printf("Total parameter:\n");
    if (counter != 0) {
        if (type & ply_full)
            printf("\tSum of degrees = %u;\n", sum_deg);
        if (type & ply_max)
            printf("\tMax degrees = %u;\n", max_deg);
        if (type & ply_min)
            printf("\tMin degrees = %u;\n", min_deg);
        printf("\tQuantity = %u;\n", counter);
    } else
        printf("No parameters with non-zero x;\n");
    return flag_ok;
}

unsigned char compact_polynomial(struct polynomial* obj)
{   // Пока работаем с одним и тем же объектом. Удаляются все элементы, константы которых равны нулю,
    // уплотнение памяти и возврат нового размера.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("compact error: polynomial [%X] is incorrect, or empty;\n", to_hw(obj));
        return 0;
    }
    unsigned char size = size_polynomial(obj), i = 0, compact = 0, k = 0;
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    printf("Compact zeroes constants, size %hhu, indexes to delete: ", size);
    for (i = 0; i < size - compact;) {
        if (consts[i] == 0) {
            printf("[%hhu] ", (unsigned char)(i + compact));
            for (k = i; k < size - 1; ++k) {
                consts[k] = consts[k + 1];
                exps[k] = exps[k + 1];
            }
            ++compact;
        } else
            ++i;
    }
    printf("\nSize after compact is %hhu;\n", (unsigned char)(size - compact));
    if (compact > 0) {
        if (size - compact > 0)
            resize_polynomial(obj, size - compact);
        else
            destroy_polynomial(obj);
    }
    return size - compact;
}

short add_polynomial(struct polynomial* left, struct polynomial* right, char is_compact)
{   // Сложение полиномов, правосторонний прибавляется к левостороннему, изменяя его значение.
    // Допустимо сложение одного и того же объекта.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("add polynomial error: polynomials is incorrect;\n");
        return err_incorrect;
    }
    if (left != right)
        printf("\nAdd left[%hX] and right[%hX] different polynomials;\n", left, right);
    else
        printf("\nAdd left[%hX] and right[%hX] same polynomials;\n", left, right);
    print_polynomial(left, 1);
    print_polynomial(right, 1);
    unsigned char left_size = size_polynomial(left);
    unsigned char right_size = size_polynomial(right);
    if (right_size == 0) {
        printf("Right polynomial is empty, nothing add to left;\n");
        return flag_ok;
    }
    char consts[DATA_MAX], is_found[DATA_MAX];
    unsigned char exps[DATA_MAX], founded = 0;
    for (short i = 0; i < DATA_MAX; ++i)
        consts[i] = is_found[i] = 0;
    unsigned char i, j;
    for (i = 0; i < left_size; i++) {
        char *left_consts = get_constants(left, i);
        unsigned char *left_exps = get_exponents(left, i), *right_exps;
        if (left != right) {
            consts[i] = *left_consts;
            exps[i] = *left_exps;
            for (j = 0; j < right_size; ++j) {
                right_exps = get_exponents(right, j);
                if (*left_exps == *right_exps && !is_found[j]) {
                    consts[i] += *get_constants(right, j);
                    is_found[j] = 1;
                    founded++;
                }
            }
        } else {
            consts[i] = *left_consts << 1;
            exps[i] = *left_exps;
            is_found[i] = 1;
            founded++;
        }
    }
    printf("Add %hhu byte elements from left and founded %hhu equal exponents from right;\n",
           i, founded);
    for (j = 0; j < right_size; ++j)
        if (is_found[j] == 0) {
            consts[i] = *get_constants(right, j);
            exps[i] = *get_exponents(right, j);
            i++;
        }
    printf("Size of new polynomail is %hhu, ", i);
    if (left_size == 0) {
        printf("left destination is empty, just create new;\n");
        create_polynomial(left, i, consts, exps, 0);
    } else {
        printf("left destination size is %hhu, ", left_size);
        if (i > left_size) {
            printf("resize and set data;\n");
            char err = resize_polynomial(left, i);
            if (err != flag_ok) {
                printf("add polynomial: resize left polynomial error!;\n");
                return err_memory;
            }
        } else
            printf("size is equal, just set data;\n");
        set_constants(left, consts, i, 0);
        set_exponents(left, exps, i, 0);
    }
    if (is_compact)
        compact_polynomial(left);
    return flag_ok;
}

//Вещание

short sub_polynomial(struct polynomial* left, struct polynomial* right)
{   // Умножить константы правого на -1 и сложить. Правый сохранить как было.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {  // А если left == right?
        printf("sub polynomial error: polynomials is incorrect;\n");
        return err_incorrect;
    }
    printf("\nSub left[%hX] and right[%hX] polynomials;\n", left, right);
    unsigned char r_size = size_polynomial(right);
    struct polynomial neg_right = {.constants = NULL, .exponents = NULL, .size = 0};
    copy_polynomial(&neg_right, right);
    char* consts = get_constants(&neg_right, 0);
    for (unsigned char i = 0; i < r_size; ++i)
        consts[i] *= -1;
    add_polynomial(left, &neg_right, 0);
    return flag_ok;
}

void print_for_mul(struct polynomial* obj)
{
    char* consts_ptr = get_constants(obj, 0);
    unsigned char* exps_ptr = get_exponents(obj, 0);
    unsigned char poly_size = size_polynomial(obj);
    for (unsigned char i = 0; i < poly_size; ++i)
        printf("%+hhd*(X)^%hhu ", consts_ptr[i], exps_ptr[i]);
    printf("\n");
}

char mul_polynomial(struct polynomial* left, struct polynomial* right)
{   // Умножение полиномов, правосторонний прибавляется к левостороннему изменяя его значение.
    // Допустимо умножение одного и того же объекта. В столбик.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok || size_polynomial(left) == 0) {
        printf("Mul polynomial error: polynomials is incorrect;\n");
        return err_incorrect;
    }
    if (left != right)
        printf("\nMultiplication left[%hX] and right[%hX] different polynomials;\n", left, right);
    else
        printf("\nMultiplication left[%hX] and right[%hX] same polynomials;\n", left, right);
    unsigned char left_size = size_polynomial(left);
    unsigned char right_size = size_polynomial(right);
    print_for_mul(left);
    print_for_mul(right);
    printf("\n");
    if (right_size == 0) {
        printf("Right polynomial is empty, nothing mul to left;\n");
        return flag_ok;
    }
    struct polynomial left_mul = {.size = 0}; // для промежуточных вычислений
    copy_polynomial(&left_mul, left);
    struct polynomial right_mul = {.size = 0};
    copy_polynomial(&right_mul, right);
    unsigned char i, j;
    char *right_consts = get_constants(&right_mul, 0);          // для умножения самого на себя
    unsigned char *right_exps = get_exponents(&right_mul, 0);
    char *left_consts = get_constants(left, 0);
    unsigned char *left_exps = get_exponents(left, 0);
    for (i = 0; i < left_size; ++i) {           // первое умножение сразу в left, дальше с помощью left_mul
        left_consts[i] *= right_consts[0];
        left_exps[i] += right_exps[0];
    }
    struct polynomial tmp = {.size = 0};
    for (j = 1; j < right_size; ++j) {
        copy_polynomial(&tmp, &left_mul);
        char *tmp_consts = get_constants(&tmp, 0);
        unsigned char *tmp_exps = get_exponents(&tmp, 0);
        for (i = 0; i < left_size; ++i) {
            tmp_consts[i] *= right_consts[j];
            tmp_exps[i] += right_exps[j];
        }
        add_polynomial(left, &tmp, 0);
        destroy_polynomial(&tmp);
    }
    destroy_polynomial(&left_mul);
    destroy_polynomial(&right_mul);
    return flag_ok;
}

char div_polynomial(struct polynomial* left, struct polynomial* right, struct polynomial* remain)
{   // деление столбиком, возможно с отстатком.

}

enum flags to_monic(struct polynomial* obj, enum flags type)
{   // Преобразование полинома к приведённому виду с потерями данных и по типу упорядочивания
    // Флаги: more, less во возрастанию или убыванию степеней;
    // max, min привести к наибольшей или наименьшей.
    // Если флаги отсутствуют то оставить порядок и коэффициенты как есть.
    // Compact вызывается всегда.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("to monic error: polynomials [%X] is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    printf("Converting a polynomial [%X] to monic: ", to_hw(obj));
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    for (unsigned char i = 0; i < size - 1; ++i) {
        printf("%+hhd( ", consts[i]);
        for (unsigned char j = i + 1; j < size && consts[i] != 0; ++j)
            if (exps[i] == exps[j] && consts[j] != 0) {
                if (((short)consts[i] + (short)consts[j]) > CHAR_MAX
                    || ((short)consts[i] + (short)consts[j]) < CHAR_MIN)
                    printf("\twarning: merge constants is out of range\t");
                consts[i] += consts[j];
                printf("%+hhd ", consts[j]);
                consts[j] = 0;
            }
        printf(") ");
    }
    printf("\n");
    print_polynomial(obj, 0);
    size = compact_polynomial(obj);
    if (size > 0) {
        print_polynomial(obj, 0);
        printf(", linear sort as flags");
        if ((type & ply_less) || (type & ply_more)) {
            unsigned char i = 0, j = 0, idx_max = 0;
            for (i = 0, idx_max = 0; i < size - 1; idx_max = ++i) {
                for (j = i + 1; j < size; ++j)
                    if (((type & ply_more) && exps[j] > exps[idx_max])
                        || ((type & ply_less) && exps[j] < exps[idx_max]))
                        idx_max = j;
                if (idx_max != i) {
                    unsigned char tmp_exp = exps[idx_max];
                    char tmp_const = consts[idx_max];
                    exps[idx_max] = exps[i];
                    consts[idx_max] = consts[i];
                    exps[i] = tmp_exp;
                    consts[i] = tmp_const;
                }
            }
            print_polynomial(obj, 1);
        }
        if ((type & ply_min) || (type & ply_max)) {
            unsigned char i = 0, j = 0;
            for (i = 0, j = 0; i < size; ++i)
                if (((type & ply_min) && exps[i] < exps[j])
                    || ((type & ply_max) && exps[i] > exps[j]))
                    j = i;
            printf("min or max exponent at [%hhu], divider is %hhd and compact: ", j, consts[j]);
            char k = consts[j];
            if (k != 0) {
                for (i = 0; i < size; ++i)
                    consts[i] /= k;
                printf("\n");
                print_polynomial(obj, 1);
                compact_polynomial(obj);
            } else
                printf("warning divider is zero, polynomial as is;\n");
        }
    }
    return flag_ok;
}

double calculate_polynomial(struct polynomial* obj, double value)
{   // Вычисление полинома, обычное целое число. Пока что полином целиком.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("calculate polynomial error: polynomials is incorrect;\n");
        return err_incorrect;
    }
    printf("Calculate of polynomial[%hX] value for X = %.2f;\n", obj, value);
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    double total = 0;
    for (unsigned char i = 0; i < size; ++i) {
        //printf("%.2f, %.2f, %.2f\n", (double)consts[i], pow(value, (double)exps[i]), total);
        total += (double)consts[i] * pow(value, (double)exps[i]);
    }
    printf("Calculated polynomial value = %.2f\n\n", total);
    return total;
}

enum flags compare_polynomial(struct polynomial* left, struct polynomial* right)
{   // Сравнение полиномов. Размеры должны совпадать. На вход тип сравнения: равно, меньше или больше.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok ||
        type < ply_equal || type > ply_more) {
        printf("compare polynomial error: polynomials or type is incorrect;\n");
        return err_incorrect;
    }
    printf("Comparison of polynomials[%hX] and [%hX];\n", left, right);
    struct polynomial monic_right = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial monic_left = {.constants = NULL, .exponents = NULL, .size = 0};
    copy_polynomial(&monic_right, right);
    copy_polynomial(&monic_left, left);
    to_monic(&monic_right);
    to_monic(&monic_left);
    unsigned char l_size = size_polynomial(&monic_left);
    unsigned char r_size = size_polynomial(&monic_right);
    unsigned char size = l_size;
    if (size < r_size)
        size = r_size;
    char* l_consts = get_constants(&monic_left, 0);
    unsigned char* l_exps = get_exponents(&monic_left, 0);
    char* r_consts = get_constants(&monic_right, 0);
    unsigned char* r_exps = get_exponents(&monic_right, 0);
    for (unsigned i = 0; i < size; ++i) {
        if (i >= l_size) {
            if (r_consts[i] > 0)
                return -1;
            else
                return 1;
        }
        if (i >= r_size) {
            if (l_consts[i] > 0)
                return 1;
            else
                return -1;
        }
        if (l_exps[i] == r_exps[i]) {
            if (l_consts[i] > r_consts[i])
                return 1;
            else if (l_consts[i] < r_consts[i])
                return -1;
        } else if (l_exps[i] > r_exps[i])
            return 1;
        else if (l_exps[i] < r_exps[i])
            return -1;
    }
}

short resolve(struct polynomial* obj, double solutions[], double left, double right)
{   // Решение полинома методом деления пополам на заданном интервале.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) < 1
        || ((right - left) > 0.00000001)) {
        printf("resolve polynomial error: polynomials or left or right is incorrect;\n");
        return err_incorrect;
    }
    const double epsilon = 0.001;
    const double cmp_epsilon = 0.00000001;
    unsigned char sol_count = 0;
    double left_val = calculate_polynomial(obj, left);
    double right_val = calculate_polynomial(obj, right);
    printf("Resolve polynomial[%hX], left = %.2f, right = %.2f;\n", obj, left, right);
    if (left_val < cmp_epsilon)
        solutions[sol_count++] = left;
    if (right_val < cmp_epsilon)
        solutions[sol_count++] = right;
    if (left_val * right_val > 0) {
        printf("There are no roots in a given interval;\n");
        return flag_ok;
    }
    double middle = 0;
    while ((right - left) >= epsilon) {
        middle = (left + right) / 2.0;
        double mid_val = calculate_polynomial(obj, middle);
        if (mid_val < cmp_epsilon) {
            solutions[sol_count++] = middle;
            break;
        } else if (mid_val * left_val < cmp_epsilon)
            right = middle;
        else
            left = middle;
    }
    solutions[sol_count++] = middle;
    return flag_ok;
}

short derivative(struct polynomial* obj)
{   // производная от полинома
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("\nDerivative polynomial error: polynomials is incorrect;\n");
        return err_incorrect;
    }
    print_polynomial(obj, 1);
    printf("\nDerivative of a polynomial[%hX]: ", obj);
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    for (unsigned char i = 0; i < size; ++i)
        if (exps[i] == 0)
            printf("+0 ");
        else if (exps[i] == 1)
            printf("%+hhd ", consts[i]);
        else
            printf("%+d*(X)^%u ", consts[i] * exps[i], exps[i] - 1);
    printf("\n");
    return flag_ok;
}

char draw_polynomial(struct polynomial left, double scale_x, double scale_y)
{

}

void polynomial()
{
    srand(1);
    printf("Laboratory 4. Polynomials with signed integer constants and unsigned exponents.\n");
    // Таблицу с размерностями
    printf("Size of char is %zu, short is %zu, int is %zu bytes, address %zu bits;\n",
           sizeof(char), sizeof(short), sizeof(int), sizeof(char*) * CHAR_BIT);
    printf("Only lower 16-bits of [addresses] will display as '[%%hhX]' format;\n");
    printf("Default format to print element is '%s';\n", element_fmt);
    printf("Free dynamic memory: %u bytes, max data objects: %u;\n\n", MEM_MAX, DATA_MAX);
    // Вывод базовой информации об объетке и размерах его полей.
    struct polynomial poly_a = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial poly_b = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial poly_c = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial* ptr_a = &poly_a;
    struct polynomial* ptr_b = &poly_b;
    struct polynomial* ptr_c = &poly_c;
    printf("Size of structure polynomial %zu bytes and dynamic element %hu bytes;\n",
           sizeof(poly_a), element_size);
    printf("Offset:\tType:\t\tSize:\t\tComment:\n");
    printf("%u\tUnsigned char\t%zu\t\tSize of polynomial in elements;\n",
           to_hw(&poly_a.size) - to_hw(&poly_a), sizeof(poly_a.size));
    printf("%u\tChar pointer\t%zu\t\tDynamic array of constants;\n",
           to_hw(&poly_a.constants) - to_hw(&poly_a), sizeof(poly_a.constants));
    printf("%u\tUChar pointer\t%zu\t\tDynamic array of exponents;\n",
           to_hw(&poly_a.exponents) - to_hw(&poly_a), sizeof(poly_a.exponents));
    // Создание, копирование, перемещение и уничтожение объектов полином.
    const char data_size = 5;
    char data_consts[] = {3, -1, 0, 0, -6};
    unsigned char data_exponents[] = {1, 3, 0, 2, 1};

    printf("\n\nCreate, copy, move, resize, compact and destroy polynomial objects;\n\n\n");
    create_polynomial(ptr_a, data_size, data_consts, data_exponents, 0);
    print_polynomial(ptr_a, 2);
    copy_polynomial(ptr_b, ptr_a);
    print_polynomial(ptr_b, 2);
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
    printf("\n\n\nConversions and sets functions tests;\n\n\n");
    printf("Source polynomial to string;\n");
    print_polynomial(ptr_c, 2);
    char txt[DATA_MAX];
    to_string_polynomial(ptr_c, txt);
    printf("Converted string is '%s';\n", txt);
    //char input[DATA_MAX] = "Debug: [-5^3] and [3^+5], plus incorrect [2^+5).";
    char input[DATA_MAX] = "Debug: [-5%2] and [3%+5], plus incorrect [2%%+5).";
    from_string_polynomial(ptr_b, input);
    print_polynomial(ptr_b, 2);
    char new_consts = 3;
    print_polynomial(ptr_b, 2);
    printf("Set correct constant and incorrect exponent;\n");
    set_constants(ptr_b, &new_consts, 1, 1);
    unsigned char new_exps = 2;
    set_exponents(ptr_b, &new_exps, 1, 3);
    print_polynomial(ptr_b, 2);

    // Все арифметические функции, приведение и сравнение.
    printf("\n\nArithmetic operators to monic polynomial and compare;\n\n\n");
    /*
    print_polynomial(ptr_b, 2);
    inc_polynomial(ptr_b, 1, 1, ply_constant);
    print_polynomial(ptr_b, 2);
    dec_polynomial(ptr_b, 1, 1, ply_exponent);
    print_polynomial(ptr_b, 2);
    */
    char data_consts_b[] = {1, -2, -3, 3, -1};
    unsigned char data_exps_b[] = {1, 0, 2, 2, 1};
    create_polynomial(ptr_a, 5, data_consts_b, data_exps_b, ply_nop);
    print_polynomial(ptr_a, 2);
    to_monic(ptr_a, ply_more | ply_min);  // nop more (min max)
    print_polynomial(ptr_a, 2);
    return;
    resize_polynomial(ptr_b, 3);
    new_consts = 5;
    new_exps = 1;
    set_constants(ptr_b, &new_consts, 1, 2);
    set_exponents(ptr_b, &new_exps, 1, 2);
    print_polynomial(ptr_b, 2);
    print_polynomial(ptr_c, 2);

    add_polynomial(ptr_b, ptr_c, 1);
    print_polynomial(ptr_b, 2);
    add_polynomial(ptr_a, ptr_b, 1);
    print_polynomial(ptr_a, 2);
    derivative(ptr_a);
    mul_polynomial(ptr_a, ptr_a);
    print_polynomial(ptr_a, 2);

    // Функции сравнения.
    //to_monic(ptr_a);
    print_polynomial(ptr_a, 2);
    print_polynomial(ptr_c, 2);
    compare_polynomial(ptr_a, ptr_c, ply_less);
    /*
    const char data_size = 2;
    char data_consts[] = {3, -1, 0, 0, -6};
    unsigned char data_exponents[] = {1, 3, 0, 2, 1};
    printf("!!!!!!!!!!!!\n");
    create_polynomial(ptr_a, data_size, data_consts, data_exponents, 0);
    print_polynomial(ptr_a, 2);
    mul_polynomial(ptr_a, ptr_a);
    print_polynomial(ptr_a, 2);
    calculate_polynomial(ptr_a, 1);
    print_polynomial(ptr_a, 2);
*/
    // Дополнительные функции полинома.
    printf("Free memory before exit %hu bytes;\n", memory);
    destroy_polynomial(ptr_a);
    destroy_polynomial(ptr_b);
    destroy_polynomial(&poly_c);
}




