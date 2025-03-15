#include "07_Polynomial.h"
#include <ctype.h>
#include <math.h>
#include <float.h>
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
//  Ширирна и высота виртуального экрана для консольного рисования графиков. Если будут изменения, то заглянуть в 'print_screen'.
#define SCR_WIDTH 0x20
#define SCR_HEIGHT 0x20

// Вспомогательное перечисление для всех частей полинома и операции сравнения.
enum flags : unsigned short {
    flag_ok, ply_nop = 0x0000, err_incorrect = 0x0001, err_memory = 0x0002, err_convert = 0x0003, err_range = 0x0004,
    err_compare = 0x0005,
    ply_constant = 0x0008, ply_exponent = 0x0010, ply_equal = 0x0020,
    ply_less = 0x0040, ply_more = 0x0080, ply_full = 0x0100, ply_max = 0x0200, ply_min = 0x0400,
    ply_rand = 0x0800, ply_compact = 0x1000, ply_debug = 0x2000
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
static char screen[SCR_HEIGHT][SCR_WIDTH];                      // Буффер виртуального текстового экрана.
static const unsigned int pencil_colors = 5;                           // Всего условных цветов или градаций черного.
static const char pencil[] = { 32, 176, 177, 178, 219 };               // Коды таблицы ASCII для рисования в буффере.

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

enum flags inc_dec_polynomial(struct polynomial* obj, unsigned char index, unsigned char count,
                              short offs, enum flags parts)
{   // Увеличение константы или экспоненты полинома по индексу. Тип увеличения по отдельности или вместе.
    if (is_correct_polynomial(obj) != flag_ok || !((parts & ply_constant) || (parts & ply_exponent))) {
        printf("error inc/dec: polynomial [%X] is incorrect or flags not sets;\n", to_hw(obj));
        return err_incorrect;
    }
    if (index + count > size_polynomial(obj)) {
        printf("error inc/dec: polynomial [%X], index %hhu, counter %hhu is out of range;\n",
               to_hw(obj), index, count);
        return err_range;
    }
    printf("Inc/dec polynomial [%X] from index %hhu, counter %hhu, offset %hd, flags %X in hex;\n",
           to_hw(obj), index, count, offs, parts);
    while (count--) {
        if ((parts & ply_constant) != 0) {
            char* consts = get_constants(obj, index);
            if ((short)(*consts) + offs > CHAR_MAX || (short)(*consts) + offs < CHAR_MIN)
                printf("warning inc polynomial: constant at %hhu overflow %hd;\n",
                       index, (short)(*consts) + offs);
            (*consts) = (char)((short)(*consts) + offs);
        }
        if ((parts & ply_exponent) != 0) {
            unsigned char* exps = get_exponents(obj, index);
            if ((short)(*exps) + offs > UCHAR_MAX || (short)(*exps) + offs < 0)
                printf("warning inc polynomial: exp at %hhu overflow %hd;\n",
                       index, (short)(*exps) + offs);
            (*exps) = (unsigned char)((short)(*exps) + offs);
        }
        index++;
    }
    return flag_ok;
}

unsigned char degree(struct polynomial* obj, enum flags type)
{   // Вычисление степеней полинома, без учёта кратности корней.
    // ply_max - максимальная степень при x не равной нулю.
    // ply_min - минимальная степень при x не равной нулю.
    // ply_full - полная степень. Сумма всех степеней при x не равной нулю.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0 ||
        !(type & ply_full || type & ply_max || type & ply_min)) {
        printf("error degree: polynomial [%X] or type is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    unsigned char max_deg = 0, min_deg = UCHAR_MAX, full_deg = 0, counter = 0;
    printf("Degree of a polynomial [%X], flags %X in hex;", to_hw(obj), type);
    for (unsigned char i = 0; i < size; ++i)
        if (consts[i] != 0) {
            if (exps[i] < min_deg)
                min_deg = exps[i];
            if (exps[i] > max_deg)
                max_deg = exps[i];
            full_deg += exps[i];
            counter++;
        }
    printf("Total parameter:");
    if (counter != 0) {
        if (type & ply_full)
            printf("\tfull degree %hhu;\n", full_deg);
        if (type & ply_max)
            printf("\tmax degree %hhu;\n", max_deg);
        if (type & ply_min)
            printf("\tmin degree %hhu;\n", min_deg);
        printf("\tquantity %hhu;\n", counter);
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
    print_polynomial(obj, 1);
    size = compact_polynomial(obj);
    if (size > 0) {
        if ((type & ply_less) || (type & ply_more)) {
            print_polynomial(obj, 0);
            printf(", linear sort as flags\n");
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

enum flags add_polynomial(struct polynomial* left, struct polynomial* right, enum flags monic)
{   // Сложение полиномов, правосторонний прибавляется к левостороннему, нули автоматически удалаются.
    // Допустимо сложение одного и того же объекта.
    // Дополнительный флаг - приведение к каноническому виду.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("error add: polynomials left [%X] and right [%X] is incorrect;\n",
               to_hw(left), to_hw(right));
        return err_incorrect;
    }
    printf("Add left [%X] and right [%X] polynomials, flags %X in hex;\n",
           to_hw(left), to_hw(right), monic);
    print_polynomial(left, 1);
    print_polynomial(right, 1);
    unsigned char left_size = size_polynomial(left);
    unsigned char right_size = size_polynomial(right);
    if ((short)left_size + (short)right_size > UCHAR_MAX) {
        printf("error add: sizes left %hhu and right %hhu is out of range;\n", left_size, right_size);
        return err_range;
    }
    if (right_size == 0) {
        printf("Right polynomial is empty, nothing add to left;\n");
        return flag_ok;
    }
    if (left_size == 0) {
        printf("Left polynomial is empty, just move function right to left;\n");
        return move_polynomial(&left, &right);      // проверить
    }
    resize_polynomial(left, left_size + right_size);
    unsigned char* r_exps = get_exponents(right, 0);
    char* r_consts = get_constants(right, 0);
    set_constants(left, r_consts, right_size, left_size);
    set_exponents(left, r_exps, right_size, left_size);
    printf("\n");
    print_polynomial(left, 2);
    to_monic(left, monic);
    return flag_ok;
}

enum flags sub_polynomial(struct polynomial* left, struct polynomial* right, enum flags monic)
{   // Умножить константы правого на -1 и сложить. Правый сохранить как было.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("error sub: polynomials left [%X] and right [%X] is incorrect;\n",
               to_hw(left), to_hw(right));
        return err_incorrect;
    }
    if (left == right) {
        printf("Sub polynomials left [%X] and right [%X] are same, destroy left polynomial;\n",
               to_hw(left), to_hw(right));
        return destroy_polynomial(left);
    }
    unsigned char r_size = size_polynomial(right);
    if (r_size == 0) {
        printf("right polynomial is empty, nothing to sub;\n");
        return flag_ok;
    }
    printf("Sub left [%X] and right [%X], negative ", to_hw(left), to_hw(right));
    struct polynomial neg_right = {.constants = NULL, .exponents = NULL, .size = 0};
    copy_polynomial(&neg_right, right);
    printf("[%X];\n", to_hw(&neg_right));
    char* consts = get_constants(&neg_right, 0);
    for (unsigned char i = 0; i < r_size; ++i)
        consts[i] *= -1;
    add_polynomial(left, &neg_right, monic);
    return flag_ok;
}

char mul_polynomial(struct polynomial* left, struct polynomial* right)
{   // Умножение полиномов, правосторонний прибавляется к левостороннему изменяя его значение.
    // Допустимо умножение одного и того же объекта.
    /*
    Есть второй вариант решения где не будет такого сильного ограничения по размерам,
    но там придётся мудрить.
    В нём to_monic буду вызывать после кажного вложенного цикла
    for (j = 0; j < left_size; ++j, ++left_mul_counter)
     */
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("error mul: polynomials left [%X] or right [%X] is incorrect;\n",
               to_hw(left), to_hw(right));
        return err_incorrect;
    }
    printf("Multiplication left [%X] and right [%X] same polynomials;\n", to_hw(left), to_hw(right));
    struct polynomial left_mul = {.size = 0, .constants = NULL, .exponents = NULL};
    struct polynomial right_mul = {.size = 0, .constants = NULL, .exponents = NULL};
    print_polynomial(left, 1);
    print_polynomial(right, 1);
    copy_polynomial(&left_mul, left);
    copy_polynomial(&right_mul, right);
    //to_monic(&left_mul, ply_more);
    //to_monic(&right_mul, ply_more);
    unsigned char l_size = size_polynomial(&left_mul);
    unsigned char r_size = size_polynomial(&right_mul);
    if ((short)l_size * (short)r_size > UCHAR_MAX) {
        printf("error mul: sizes left %hhu and right %hhu is out of range;\n", l_size, r_size);
        destroy_polynomial(&left_mul);
        destroy_polynomial(&right_mul);
        return err_range;
    }
    if (r_size > 0 && l_size > 0) {
        printf("Polynomials has elements, multiply elements to new size %hhu;\n", l_size * r_size);
        struct polynomial res_mul = {.size = 0, .constants = NULL, .exponents = NULL};
        struct polynomial* res_ptr = &res_mul;
        enum flags rf = create_polynomial(&res_mul, l_size * r_size, NULL, NULL, ply_rand);
        if (rf != flag_ok) {
            printf("error mul: error creating temporary polynomial;\n");
            return rf;
        }
        unsigned char i, j, k = 0;
        char *right_consts = get_constants(right, 0);
        unsigned char *right_exps = get_exponents(right, 0);
        char *left_consts = get_constants(left, 0);
        unsigned char *left_exps = get_exponents(left, 0);
        printf("result polynomial: ");
        for (i = 0, k = 0; i < r_size; ++i)
            for (j = 0; j < l_size; ++j, ++k) {
                if ((short)(left_consts[i]) * (short)(right_consts[i]) > CHAR_MAX ||
                    (short)(left_consts[i]) * (short)(right_consts[i]) < CHAR_MIN)
                        printf("warning: multiply constant at [%hhu] is out of range;\n", i);
                char res_consts = left_consts[j] * right_consts[i];
                set_constants(&res_mul, &res_consts, 1, k);
                if ((short)(left_exps[i]) + (short)(right_consts[i]) > UCHAR_MAX)
                    printf("warning: multiply constant at [%hhu] is out of range;\n", i);
                unsigned char res_exps = left_exps[j] + right_exps[i];
                set_exponents(&res_mul, &res_exps, 1, k);
            }
        print_polynomial(&res_mul, 2);
        destroy_polynomial(left);
        move_polynomial(&left, &res_ptr);
    } else {
        printf("Mul polynomials right [%X] is empty, destroy left polynomial;\n", to_hw(right));
        destroy_polynomial(left);
    }
    //to_monic(&left_mul, ply_more);
    destroy_polynomial(&left_mul);
    destroy_polynomial(&right_mul);
    return flag_ok;
}

/*
 * char mul_polynomial(struct polynomial* left, struct polynomial* right)
{   // Умножение полиномов, правосторонний прибавляется к левостороннему изменяя его значение.
    // Допустимо умножение одного и того же объекта. В столбик.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("error mul: polynomials left [%X] or right [%X] is incorrect;\n", to_hw(left), to_hw(right));
        return err_incorrect;
    }
    printf("\nMultiplication left[%X] and right[%X] same polynomials;\n", to_hw(left), to_hw(right));
    unsigned char left_size = size_polynomial(left);
    unsigned char right_size = size_polynomial(right);
    if (right_size == 0) {
        printf("Mul polynomials right [%X] is empty, destroy left polynomial;\n", to_hw(right));
        return destroy_polynomial(left);
    }
    struct polynomial left_mul = {.size = 0};
    copy_polynomial(&left_mul, left);
    struct polynomial right_mul = {.size = 0};
    copy_polynomial(&right_mul, right);
    unsigned char i, j;
    char *right_consts = get_constants(&right_mul, 0);
    unsigned char *right_exps = get_exponents(&right_mul, 0);
    char *left_consts = get_constants(left, 0);
    unsigned char *left_exps = get_exponents(left, 0);
    for (i = 0; i < left_size; ++i) {
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
        add_polynomial(left, &tmp, ply_more);
        destroy_polynomial(&tmp);
    }
    destroy_polynomial(&left_mul);
    destroy_polynomial(&right_mul);
    return flag_ok;
}
 */

char div_polynomial(struct polynomial* left, struct polynomial* right, struct polynomial* remain)
{   // деление столбиком, возможно с отстатком.

}

double calculate_polynomial(struct polynomial* obj, double value, enum flags is_debug)
{   // Вычисление полинома, по заданному значению. Пока что полином целиком.
    double result = NAN;
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("error calculate: polynomials [%X] is incorrect or empty;\n", to_hw(obj));
        return result;
    }
    printf("Calculate of polynomial [%X] value for X = %.2f(const, element, result): ",
           to_hw(obj), value);
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    result = 0.0;
    for (unsigned char i = 0; i < size; ++i) {
        result += (double)consts[i] * pow(value, (double)exps[i]);
        if (is_debug & ply_debug)
            printf("(%.2f, %.2f, %.2f) ", (double)consts[i], pow(value, (double)exps[i]), result);
    }
    printf("%.2f\n", result);
    return result;
}

enum flags compare_polynomial(struct polynomial* left, struct polynomial* right)
{   // Сравнение полиномов. Размеры должны совпадать. Сравниваются только константы при равных степенях.
    // Не является математически верным решением.
    if (is_correct_polynomial(left) != flag_ok || is_correct_polynomial(right) != flag_ok) {
        printf("Error compare: polynomials left [%X] or right [%X] is incorrect;\n",
               to_hw(left), to_hw(right));
        return err_incorrect;
    }
    printf("Compare of polynomials left [%X] and right [%X]\n", to_hw(left), to_hw(right));
    struct polynomial monic_right = {.constants = NULL, .exponents = NULL, .size = 0};
    struct polynomial monic_left = {.constants = NULL, .exponents = NULL, .size = 0};
    copy_polynomial(&monic_right, right);
    copy_polynomial(&monic_left, left);
    to_monic(&monic_right, ply_more);
    to_monic(&monic_left, ply_more);
    unsigned char l_size = size_polynomial(&monic_left);
    unsigned char r_size = size_polynomial(&monic_right);
    printf("\nLeft (%hhu) and right (%hhu) sizes converting into monic:\n", l_size, r_size);
    print_polynomial(&monic_left, 1);
    print_polynomial(&monic_right, 1);
    if (l_size == 0 && r_size == 0) {
        printf("warning compare: both polynomials are nulls, so they equal;\n");
        return ply_equal;
    }
    if (l_size != r_size) {
        printf("error compare: polynomials of different sizes (%hhu:%hhu), comparison is impossible;\n",
               l_size, r_size);
        return err_compare;
    }
    char* l_consts = get_constants(&monic_left, 0);
    unsigned char* l_exps = get_exponents(&monic_left, 0);
    char* r_consts = get_constants(&monic_right, 0);
    unsigned char* r_exps = get_exponents(&monic_right, 0);
    unsigned char more = 0, less = 0, equal = 0, i = 0;
    for (i = 0; i < l_size && l_exps[i] == r_exps[i]; ++i) {
        if (l_consts[i] == r_consts[i])
            equal++;
        if (l_consts[i] > r_consts[i])
            more++;
        if (l_consts[i] < r_consts[i])
            less++;
        //printf("le: %hhu, re: %hhu;\n",l_exps[i + 1], r_exps[i + 1]);
    }
    //printf("i = %hhu, %hhu;\n", i, l_size);
    if (i == l_size) {
        printf("more %hu, less %hu, equal %hu;\n", more, less, equal);
        if (equal == i) {
            printf("The left and right polynomial are equal;\n");
            return ply_equal;
        }
        if (more == i) {
            printf("The left polynomial is more than the right polynomial;\n");
            return ply_more;
        }
        if (less == i) {
            printf("The left polynomial is less than the right polynomial;\n");
            return ply_less;
        }
    }
    printf("warning compare, not comparable, exponents not equal;\n");
    return err_compare;
}

/*
enum flags resolve(struct polynomial* obj, double solutions[], double left, double right)
{   // Решение полинома методом деления пополам на заданном интервале.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) < 1
        || ((right - left) > epsilon)) {
        printf("error resolve: polynomial [%X] or left (%.2f) or right (%.2f) is incorrect;\n",
               to_hw(obj), left, right);
        return err_incorrect;
    }
    unsigned char sol_count = 0;
    double left_val = calculate_polynomial(obj, left, );
    double right_val = calculate_polynomial(obj, right);
    printf("Resolve polynomial [%X], left = %.2f, right = %.2f;\n", to_hw(obj), left, right);
    if (left_val < epsilon)
        solutions[sol_count++] = left;
    if (right_val < epsilon)
        solutions[sol_count++] = right;
    if (left_val * right_val > 0) {
        printf("There are no roots in a given interval;\n");
        return flag_ok;
    }
    double middle = 0;
    while ((right - left) >= epsilon) {
        middle = (left + right) / 2.0;
        double mid_val = calculate_polynomial(obj, middle);
        if (mid_val < epsilon) {
            solutions[sol_count++] = middle;
            break;
        } else if (mid_val * left_val < epsilon)
            right = middle;
        else
            left = middle;
    }
    solutions[sol_count++] = middle;
    return flag_ok;
}
*/

enum flags derivative(struct polynomial* obj, enum flags monic)
{   // производная от полинома, сохранение в тот же самый объект
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("\nerror derivative: polynomial [%X] is incorrect;\n", to_hw(obj));
        return err_incorrect;
    }
    //print_polynomial(obj, 1);
    printf("Derivative of a polynomial [%X]\n", to_hw(obj));
    char* consts = get_constants(obj, 0);
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char size = size_polynomial(obj);
    for (unsigned char i = 0; i < size; ++i) {
        if ((short)(consts[i]) * (short)(exps[i]) > CHAR_MAX ||
            (short)(consts[i]) * (short)(exps[i])  < CHAR_MIN)
            printf("warning: derivative constant at [%hhu] is out of range;\n", i);
        consts[i] *= exps[i];
        if (exps[i] > 0)
            exps[i]--;
    }
    print_polynomial(obj, 1);
    to_monic(obj, monic);
    return flag_ok;
}

void clear_screen_ply(char c, int is_debug)
{   // Функция очистки буфера виртуального экрана заданным символом. Циклами или через адрес по вкусу.
    // Если флаг отладки, то заполнить буфер всеми "цветами", кроме 0-го. Можно по классике, прямоугольниками.
    if (is_debug)
        for (unsigned int i = 0, k = 1; i < SCR_HEIGHT; ++i)
            for (unsigned int j = 0; j < SCR_WIDTH; ++j)
                screen[i][j] = pencil[k + j / ((SCR_WIDTH / (pencil_colors - 1)))];
    else
        for (unsigned int i = 0; i < SCR_HEIGHT; ++i)
            for (unsigned int j = 0; j < SCR_WIDTH; ++j)
                screen[i][j] = c;
}

void print_screen_ply(void)
{   // Функция вывода буфера в консоль. Переход на следующую строку после экрана.
    //for (unsigned int i = 0; i < SCR_HEIGHT; ++i)               // Быстрый вывод через форматированную строку,
     //   printf("%.128s\n", screen[i]);                          //но требуется ручная подстановка параметра поля строки.

    for (unsigned int i = 0; i < SCR_HEIGHT; ++i) {
        for (unsigned int j = 0; j < SCR_WIDTH; ++j)
                putchar(screen[i][j]);                        // Более медленный но также рабочий вывод.
        printf("\n");                                           // Раскоментировать, если требуется менять постоянно ширину.
    }

}

short is_homogeneous(struct polynomial* obj)
{   // проверка на однородность степеней полинома.
    if (is_correct_polynomial(obj) != flag_ok || size_polynomial(obj) == 0) {
        printf("error homogeneous: polynomial [%X] is incorrect or empty;\n", to_hw(obj));
        return -1;
    }
    printf("Checking homogeneity of a polynomial [%X];\n", to_hw(obj));
    unsigned char* exps = get_exponents(obj, 0);
    unsigned char obj_size = size_polynomial(obj), i = 0;
    while (obj_size && exps[i] == *exps)
        ++i;
    if (i == obj_size) {
        printf("The polynomial is homogeneous;\n");
        return 1;
    } else {
        printf("The polynomial isn't homogeneous;\n");
        return 0;
    }
}

enum flags draw_polynomial(struct polynomial* obj, double lx, double rx)
{   // рисование полинома в виртуальный экран на заданном интервале, масштаб по Y автоматом.
    // Шаг локальный, чтобы график был плотный.
    /* Вариант решения
    1) масштабирование по X: разница (модулей r_x (макс) и l_x (мин) / (кол-во позиций - 1)). Первый элемент l_x, последний r_x
    2) Если разница между макс и мин меньше чем размер по Y, то просто оставляем пустое пространство выше
    (или можно сместить - тогда ниже). Если разница больше, чем размер экрана по Y, то в верхний - макс, в нижний - мин.
    Значения между ними, как разница (модулей макс и мин / (кол-во позиций - 1))
    3) шаг по x будет расчитан по масштабированию, если только точки ставить, то просто.
    //  придумать полиномы пологие и x - коэфф.
    */
    const double step = 0.1;
    lx = -1.0, rx = 3.0;
    if (is_correct_polynomial(obj) != flag_ok || rx - lx < step || lx == NAN || rx == NAN) {
        printf("error draw: polynomial [%X] is incorrect or left (%.2f) X more than right (%.2f);\n",
               to_hw(obj), lx, rx);
        return err_incorrect;
    }
    printf("Draw polinomial [%X], to screen (%hu, %hu);\n",
           to_hw(obj), SCR_WIDTH, SCR_HEIGHT);
    double max = DBL_MIN, min = DBL_MAX, x = lx, val = 0.0;
    while (x < rx) {
        val = calculate_polynomial(obj, x, ply_nop);
        if (max < val)
            max = val;
        if (min > val)
            min = val;
        x += step;
    }
    if (max == DBL_MIN || min == DBL_MAX || fabs(max - min) < epsilon) {
        printf("error draw: some values are incorrect, can't min or max, so scale Y;\n");
        return err_range;
    }
    double scale_x = (double)(SCR_WIDTH) / (rx - lx);
    double scale_y = (double)(SCR_HEIGHT) / (max - min) - epsilon;
    //printf("max = %.2f;\n", (max - min) * scale_y);
    double offs_x = ((rx - lx) < epsilon) ? 0.0 : rx - ((rx - lx) / 2.0);
    printf("range x to draw (%.2f, %.2f), step %.2f, offset %.2f, min %.2f, max %.2f;\n",
           lx, rx, step, offs_x, min, max);
    printf("scale x %.2f y %0.2f;\n", scale_x, scale_y);
    unsigned short cx = ((double)SCR_WIDTH / 2.0) - ((double)(SCR_WIDTH) * (offs_x / (rx - lx))),
        counter = 0;    // - offs_x
    printf("point (0.0) is center at (%hu), center y from %.2f to %.2f;\n",
           cx, min * scale_y, max * scale_y);
    clear_screen_ply(pencil[0], 0);
    x = lx;
    while (x < rx) {
        short sx = floor(x * scale_x);
        short sy = floor((calculate_polynomial(obj, x, ply_nop) - min) * scale_y);
        printf("step: %d, cx + sx: %d + %d = %d, sy: %d\n", counter, cx, sx, cx + sx, sy);
        x += step;
        counter++;
        screen[SCR_HEIGHT - 1 - sy][cx + sx] = pencil[pencil_colors - 1];
    }
    print_screen_ply();
    return flag_ok;


    unsigned char i = 0 , j = 0, idx = 0;
    double x_nums_on_y[SCR_WIDTH];          // Посчитанные значения полинома для каждой X координаты
    double x_nums[SCR_WIDTH] = {[0] = lx, [SCR_WIDTH - 1] = rx};  // X координаты. Масштабируем по ширине экран.
    // Масштабирование по X
    printf("Step x = %.10f;\nScaled values by X coordinate: ", step);
    printf("%0.5f ", x_nums[0]);
    for (i = 1; i < SCR_WIDTH - 1; ++i) {
        x_nums[i] = x_nums[i - 1] + step;
        printf("%0.5f ", x_nums[i]);
    }
    printf("%0.5f\n", x_nums[SCR_WIDTH - 1]);
    // Расчёт значений полинома для каждого значения X + поиск минимального и максимального для Y координаты
    x_nums_on_y[0] = calculate_polynomial(obj, x_nums[0], ply_nop);
    double max_y = x_nums_on_y[0];
    double min_y = x_nums_on_y[0];
    for (i = 1; i < SCR_WIDTH; ++i) {
        x_nums_on_y[i] = calculate_polynomial(obj, x_nums[i], ply_nop);
        if (max_y < x_nums_on_y[i])
            max_y = x_nums_on_y[i];
        if (min_y > x_nums_on_y[i])
            min_y = x_nums_on_y[i];
    }
    printf("Calculated values of the polynomial for each number from the range of the X coordinate:\n");
    for (i = 0; i < SCR_WIDTH; ++i)
        printf("%.5f ", x_nums_on_y[i]);
    printf("\nmax = %.5f, min = %.5f;\n", max_y, min_y);
    // Масштабирование по Y: Вверх - max_y, низ - min_y
    if (fabs(max_y - min_y) < epsilon) {
        printf("error draw: step y divided by zero;\n");
        return err_incorrect;
    }
    double step_y = fabs(max_y - min_y) / (double)(SCR_HEIGHT - 1);
    double y_nums[SCR_HEIGHT] = {[0] = min_y, [SCR_HEIGHT - 1] = max_y};     // Y координата. Масштабируем по высоте экрана.
    printf("step y = %.10f;\nScaled values by Y coordinate: ", step_y);
    printf("%0.5f, ", y_nums[0]);
    for (i = 1; i < SCR_HEIGHT - 1; ++i) {
        y_nums[i] = y_nums[i - 1] + step_y;
        printf("%0.5f ", y_nums[i]);
    }
    printf("%0.5f\n", y_nums[SCR_HEIGHT - 1]);
    // Формирование виртуального экрана
    clear_screen_ply(pencil[0], 0);
    double diff = 0;
    for (i = 0; i < SCR_WIDTH; ++i) {     // X
        for (j = 0, idx = 0, diff = max_y; j < SCR_HEIGHT; ++j) {  // Y, идём по Y координатам для каждого X. Ищем число с самой меньшей разницой от посчитанного X и ставим точку графика
            double y_diff = fabs(y_nums[j] - x_nums_on_y[i]);
            //printf("dif = %.5f, y = %.5f, x = %.5f, coord(%d,%d)\n", y_diff, y_nums[j], x_nums_on_y[i], j ,i);
            if (diff > y_diff) {
                diff = y_diff;
                idx = j;
            }
        }
        //printf("point on Y = %d, X = %d;\n\n", idx, i);
        screen[SCR_HEIGHT - idx - 1][i] = pencil[3];
    }
    printf("Coordinate range: X(%.2f:%.2f) Y(%.2f:%.2f)\n", lx, rx, min_y, max_y);
    print_polynomial(obj, 1);
    print_screen_ply();
    return flag_ok;
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
    char data_consts_b[] = {2, -1, -2, 5, 0};
    unsigned char data_exps_b[] = {2, 1, 3, 3, 2};
    create_polynomial(ptr_a, 5, data_consts_b, data_exps_b, ply_nop);
    print_polynomial(ptr_a, 2);
    inc_dec_polynomial(ptr_a, 0, size_polynomial(ptr_a), -1, ply_constant | ply_exponent);
    print_polynomial(ptr_a, 2);
    to_monic(ptr_a, ply_more);  // nop more (min max)
    printf("\nCompare different polymonials include self compare;\n\n");
    enum flags result;
    char data_consts_c[] = {5, -7, 8, -6};
    unsigned char data_exps_c[] = {5, 3, 5, 3};
    resize_polynomial(ptr_a, 2);
    resize_polynomial(ptr_c, 2);
    set_constants(ptr_a, &data_consts_c[2], 2, 0);
    set_exponents(ptr_a, &data_exps_c[2], 2, 0);
    set_constants(ptr_c, data_consts_c, 2, 0);
    set_exponents(ptr_c, data_exps_c, 2, 0);
    //print_polynomial(ptr_a, 2);
    //print_polynomial(ptr_b, 2);
    //print_polynomial(ptr_c, 2);
    result = compare_polynomial(ptr_b, ptr_a);
    printf("result of not comparable in hex %X as flags;\n\n", result);
    result = compare_polynomial(ptr_a, ptr_c);
    printf("result of more in hex %X as flags;\n\n", result);
    result = compare_polynomial(ptr_b, ptr_b);
    printf("result of self compare in hex %X as flags;\n\n", result);
    printf("\nAddition and substraction;\n\n");
    resize_polynomial(ptr_a, 5);
    resize_polynomial(ptr_b, 4);
    destroy_polynomial(ptr_c);
    set_constants(ptr_a, data_consts_b, 5, 0);
    set_exponents(ptr_a, data_exps_b, 5, 0);
    set_constants(ptr_b, data_consts_c, 4, 0);
    set_exponents(ptr_b, data_exps_c, 4, 0);
    printf("\n");
    add_polynomial(ptr_a, ptr_b, ply_more);
    print_polynomial(ptr_a, 2);
    add_polynomial(ptr_c, ptr_b, ply_nop);
    print_polynomial(ptr_c, 2);
    sub_polynomial(ptr_a, ptr_c, ply_nop);
    print_polynomial(ptr_a, 2);
    sub_polynomial(ptr_a, ptr_a, ply_nop);
    print_polynomial(ptr_a, 2);
    print_polynomial(ptr_c, 2);
    degree(ptr_c, ply_min);
    degree(ptr_c, ply_max);
    degree(ptr_c, ply_full);
    print_polynomial(ptr_c, 2);
    derivative(ptr_c, ply_nop);
    is_homogeneous(ptr_c);
    char data_consts_d[] = {1, -1, -1, 1, 0};
    unsigned char data_exps_d[] = {3, 3, 2, 3, 2};
    create_polynomial(ptr_a, 2, data_consts_d, data_exps_d, 0);
    create_polynomial(ptr_b, 2, &data_consts_d[2], &data_exps_d[2], 0);
    printf("\n");
    mul_polynomial(ptr_a, ptr_b);
    resize_polynomial(ptr_c, 1);
    set_constants(ptr_c, data_consts_d, 1, 0);
    set_exponents(ptr_c, data_exps_d, 1, 0);
    print_polynomial(ptr_c, 2);
    draw_polynomial(ptr_c, -1.0, 1.0);
    return;
    calculate_polynomial(ptr_c, 1.0, ply_nop);
    //clear_screen_ply(' ', 1);
    //print_screen_ply();
    create_polynomial(ptr_a, 4, data_consts_b, data_exps_b, ply_nop);
    resize_polynomial(ptr_c, 4);
    print_polynomial(ptr_c, 2);
    print_polynomial(ptr_a, 2);
    mul_polynomial(ptr_c, ptr_a);
    print_polynomial(ptr_c, 2);
    is_homogeneous(ptr_c);
    print_polynomial(ptr_a, 3);
    draw_polynomial(ptr_c, -1.0, 1.0);
    mul_polynomial(ptr_a, ptr_a);
    print_polynomial(ptr_a, 2);
    return;
    print_polynomial(ptr_b, 2);
    add_polynomial(ptr_a, ptr_b, 1);
    print_polynomial(ptr_a, 2);
    //derivative(ptr_a);            ?? для построения графиков
    mul_polynomial(ptr_a, ptr_a);
    print_polynomial(ptr_a, 2);

    // Функции сравнения.
    //to_monic(ptr_a);
    print_polynomial(ptr_a, 2);
    print_polynomial(ptr_c, 2);
    compare_polynomial(ptr_a, ptr_c);
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




