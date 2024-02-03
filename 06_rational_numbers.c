#include "06_rational_numbers.h"

/*  Наш вариант для Си. Лабораторая работа №3. Рациональные числа и методы для работы с ними.
 *  Рациональное число, представленное числителем со знаком и знаменателем без знака, 32 бита.
 *  Представить рациональное число в виде структуры и реализовать функции работы с ним.
 *  Все арифметические операции исполнить математически, увеличение и уменьшение на единицу - только числитель.
 *  Быть внимательным, не теряем знак. Строковый формат: "числитель/знаменатель", остальное считать ошибкой.
 *  Код должен быть максимально надежным, отработать на тестах крайние и неверные значения.
 *  Функции стараться сделать как можно более компактными, используя только сам код.
 *  Во всех функциях вывести адреса объектов, если не указано отдельно, и подробный вывод по выполнению.
*/

int* get_numerator(struct rational* r)
{   // Получение адреса числителя рационального числа, без вывода.
    if (r == NULL) {
        printf("error: get numerator rational addr is NULL;\n");
        return NULL;
    }
    return &(r->numerator);
}

unsigned int* get_denominator(struct rational* r)
{   // Получение адреса знаменателя рационального числа, без вывода.
    if (r == NULL) {
        printf("error: get denominator rational addr is NULL;\n");
        return NULL;
    }
    return &(r->denominator);
}

void set_numerator(struct rational* r, int* num)
{   // Установка числителя рационального числа, через адрес.
    if (r == NULL || num == NULL) {
        printf("error: set numerator rational addr or num is NULL;\n");
        return;
    }
    r->numerator = *num;
}

void set_denominator(struct rational* r, unsigned int* den)
{   // Установка знаменателя рационального числа, через адрес.
    if ((r == NULL || den == NULL) || (den != NULL && *den == 0)) {
        printf("error: set denominator rational addr or num is NULL or den is 0;\n");
        return;
    }
    r->denominator = *den;
}

int is_correct(struct rational* r)
{   // Проверка числа на корректность. Без вывода, просто возвращаем результат.
    if ((r == NULL) || get_denominator(r) == NULL)    // Опасно, порядок действий.
        return 0;
    else
        return 1;
}

void print(struct rational* r, int endl)
{   // Вывести все данные в структуре по формату по-умолчанию, кроме адреса объекта. В виде: "N/M", для удобного использования.
    if (!is_correct(r)) {
        printf("error: print object is incorrect;\n");
        return;
    }
    int* num = get_numerator(r);
    unsigned int* den = get_denominator(r);
    printf("%d/%u", *num, *den);
    while (endl--)
        printf("\n");
}

void create(struct rational* r, int num, unsigned int den)
{   // Создание объекта заданными значениями, если знаменатель равен нулю, то создать дробь по-умолчанию 1/1.
    if (r == NULL) {
        printf("error: rational object addr is NULL;\n");
        return;
    }
    if (den == 0) {
        int value = 1;
        unsigned int uvalue = 1;
        set_numerator(r, &value);
        set_denominator(r, &uvalue);
        printf("warning: denominator is 0, rational number of the form 1/1 created;\n");
    } else {
        set_numerator(r, &num);
        set_denominator(r, &den);
        int* num_ptr = get_numerator(r);
        unsigned int* den_ptr = get_denominator(r);
        printf("rational number %d/%u created, at %p;\n", *num_ptr, *den_ptr, r);
    }
}

void copy(struct rational* dst, struct rational* src)
{   // Копирование объекта источника в объект назначения, исходник остается неизменным.
    if (!is_correct(dst) || !is_correct(src)) {
        printf("error: dst or src is incorrect;\n");
        return;
    }
    printf("Source object: ");
    print(src, 1);
    printf("Destination object: ");
    print(dst, 1);
    printf("copy object from %p to %p;\n", src, dst);
    //*dst = *src;      для простого варианта работает
    set_numerator(dst, get_numerator(src));
    set_denominator(dst, get_denominator(src));
}

void move(struct rational** dst, struct rational** src)
{   // Перемещение объекта  в объект назначения, приемник не обязателен к сохранению.
    if (dst == NULL || src == NULL) {
        printf("error: dst or src is NULL;\n");
        return;
    }
    printf("Source object: ");
    print(*src, 1);
    printf("Destination object: ");
    print(*dst, 1);
    printf("move object at %p to %p;\n", src, dst);
    *dst = *src;
}

unsigned int gcd2(unsigned int a, unsigned int b)
{   // Вспомогательная функция нахождения наибольшего общего делителя, если требуется, без вывода.
    while (a && b)  // дополнить
        if (a > b)
            a %= b;
        else
            b %= a;
    return (a + b);
}

unsigned int lcm2(unsigned int a, unsigned int b)
{   // Вспомогательная функция нахождения наименьшего общего кратного, если требуется, без вывода.
    int result = gcd2(a,b);
    if (!result) {
        printf("warning: gcd can't be zero;\n");
        return 0;
    }
    return ((a * b) / result);
}

int normalize(struct rational* r)
{   // Нормализация числа, с максимальным сокращением числителя и знаменателя.
    if (!is_correct(r)) {
        printf("error: normalize object is incorrect;\n");
        return 0;
    }
    int num = *get_numerator(r);
    int sign = (num > -1) ? 1 : -1;
    unsigned int den = *get_denominator(r), n = num * sign;
    unsigned int g = gcd2(n, den);
    printf("Rational number normalization, gcd = %u;\n", g);
    printf("Source object: ");
    print(r, 1);
    if (g > 1) {
        num = n / g * sign;
        den /= g;
        set_numerator(r, &num);
        set_denominator(r, &den);
    } else {
        printf("Nothing to normalize, gcd 0 or 1");
        return 0;
    }
    printf("Result object: ");
    print(r, 0);
    return 1;
}

void inverse(struct rational* r)
{   // Преобразование рационального числа в обратное.
    if (!is_correct(r) || *get_numerator(r) == 0) {     //fix from Andrew, danger code
        printf("error: inverse object is incorrect;\n");
        return;
    }
    printf("Rational number flip;\n");
    printf("Source object: ");
    print(r, 1);
    int num = *get_numerator(r);
    unsigned int den = *get_denominator(r);
    int sign = (num > -1) ? 1 : -1;
    num *= sign;
    if (den > INT_MAX)
        printf("warning: absolute denominator is more than integer max;\n");
    int tmp = num;
    num = den * sign;
    set_numerator(r, &num);
    den = tmp * sign;
    set_denominator(r, &den);
    printf("Result object: ");
    print(r, 1);
}

int is_int_overflow(unsigned int n, int sign)
{
    unsigned int abs_int_min = INT_MIN ^ ((unsigned int)-1);
    return ((sign == 1 && n > INT_MAX) || (sign == -1 && n > abs_int_min));
}

void negative(struct rational* r)
{   // Преобразование рационального числа в противоположное.
    if (!is_correct(r)) {
        printf("error: object is incorrect;\n");
        return;
    }
    printf("Changing the sign of a rational number;\n");
    printf("Source object: ");
    print(r, 1);
    int num = *get_numerator(r), sign = (num > 0) ? 1 : -1;
    unsigned int n = num * sign;
    if (is_int_overflow(n, sign))
        printf("warning, negative number has overflow integer type;\n");
    num *= -1;
    set_numerator(r, &num);
    printf("Result object: ");
    print(r, 1);
}

int from_double(struct rational* r, double n, int MaxGuess)
{   // Вещественное число подбирается как ближайшее к дроби. Через перебор или быстрей через шаг.
    // Возвращает 1, если преобразование прошло успешно, иначе 0.
    if (r == NULL || MaxGuess == 0) {
        printf("error from double, rational NULL or max guess 0;\n");
        return 0;
    }
    printf("Simple guess %.2f as closest rational;\n", n);
    int sign, num = MaxGuess;
    unsigned int den = MaxGuess;
    if (n < 0) {
        sign = -1;
        n *= -1.0;
    } else
        sign = 1;
    double offset = n;
    for (unsigned int i = 0, j; i < MaxGuess; ++i)
        for (j = 1; j < MaxGuess; ++j) {
            double guess = n - (double)(i) / (double)(j);
            if (guess < 0)
                guess *= -1.0;
            if (guess < offset) {
                offset = guess;
                num = i;
                den = j;
            }
        }
    if (offset != n) {  // опасное сравнение
        num *= sign;
        set_numerator(r, &num);
        set_denominator(r, &den);
        printf("Closest: ");
        print(r, 1);
        return 1;
    } else {
        num = den = 1;
        set_numerator(r, &num);
        set_denominator(r, &den);
        printf("Parameters not founded, set to default: ");
        print(r, 1);
        return 0;
    }
}

int from_string(struct rational* r, char src[])
{   // Преобразование исходной текстовой строки по формату, в объект. Проверку только для формата по-умолчанию.
    // Возвращает 1, если преобразование прошло успешно, иначе 0.
    if (r == NULL || src == NULL) {
        printf("Address of rational number or string is NULL;\n");
        return 0;
    }
    const int base = 10, pars = 2;
    const char *p_name[] = {"numerator", "denomenator"};
    int i = 0, j = 0, k = 0, num = 0, sign = 1;
    unsigned int l = 0, n = 0, den = 0, mult = 0, is_any = 0;
    while ((src[i] != '+' && src[i] != '-') && (src[i] < '0' || src[i] > '9') &&
           (src[i] != '\0') && (i < TEXT_MAX))
        ++i;
    if (src[i] == '-') {
        sign = -1;
        ++i;
    }
    if (src[i] == '+') {
        sign = 1;
        ++i;
    }
    printf("Convert string '%s' by format to rational, format '%s/%s';\n", src, p_name[0], p_name[1]);
    printf("Sign is %d, starting index = %d;", sign, i);
    for (l = 0, j = i; src[i] != '\0' && l < pars && j < TEXT_MAX; ++l, i = j) {
        printf("\n'%s' steps: ", p_name[l]);
        while(src[j] != '/' && src[j] != '\0' && j < TEXT_MAX)
            ++j;
        // printf
        k = j; mult = 1; n = 0;
        while (--k >= i) {
            if (src[k] >= '0' && src[k] <= '9') {
                // printf
                n += (src[k] - '0') * mult;
                mult *= base;
                is_any |= 0x01 << l;
                printf("%u ", n);
            } else
                printf("'%c' ", src[k]);
        }
        if (!l) {
            if (is_int_overflow(n, sign))
                printf("warning: overflow numerator is more than integer max or less than integer min;\n");
            num = (int)(n) * sign;
        } else
            den = n;
        if (src[j] == '/')
            ++j;
    }
    printf("\n");
    if (den != 0 && is_any == 0x03) {
        set_numerator(r, &num);
        set_denominator(r, &den);
        return 1;
    } else {
        printf("error: denominator is %u or rational incomplete. Nothing to set;\n", den);
        return 0;
    }

}

int input(struct rational* r)
{   // Ввод и конвертация в объект. С проверкой. Ограничиться одной строкой.
    // Возвращает 1, если преобразование прошло успешно, иначе 0.
    if (r == NULL) {
        printf("error: object is NULL;\n");
        return -1;
    }
    printf("Inputing rational value, as '+-numerator/denominator': ");
    const int params = 2;
    int len = 0, c = 0, p = 0;
    char r_text[TEXT_MAX];
    while ((c = getchar()) != '\n' && p < params && len < TEXT_MAX - 1) {
        if (len == 0 && (c == '-' || c == '+'))
            r_text[len++] = c;
        if ((c >= '0' && c <= '9') || c == '/')
            r_text[len++] = c;
        if (c == '/')
            p++;
    }
    if (len == TEXT_MAX - 1)
        printf("warning: buffer is full, trying to convert as is;\n");
    r_text[len] = '\0';
    return from_string(r, r_text);
}

void inc(struct rational* dst)
{   // Увеличение числителя объекта на единицу.
    if (!is_correct(dst)) {
        printf("error: inc object is incorrect;\n");
        return;
    }
    int num = *get_numerator(dst);
    printf("increasing the numerator by one: %d\n", num);
    if (num == INT_MAX)
        printf("warning: numerator has max value, increase cause overflow to integer min;\n");
    num += 1;
    set_numerator(dst, &num);
    printf("object after: ");
    print(dst, 1);
}

void dec(struct rational* dst)
{   // Уменьшение числителя объекта на единицу.
    if (!is_correct(dst)) {
        printf("error: dec object is incorrect;\n");
        return;
    }
    int num = *get_numerator(dst);
    printf("decreasing the numerator by one: %d\n", num);
    if (num == INT_MIN)
        printf("warning: numerator has min value, decrease cause overflow to integer max;\n");
    num -= 1;
    set_numerator(dst, &num);
    printf("object after: ");
    print(dst, 1);
}

int to_common_den(struct rational* left, struct rational* right)
{
    if (!is_correct(left) || !is_correct(right)) {
        printf("error: left or right is incorrect;\n");
        return 0;
    }
    int num_left = *get_numerator(left), num_right = *get_numerator(right);
    unsigned int den_left = *get_denominator(left), den_right = *get_denominator(right);
    unsigned int l = lcm2(den_left, den_right);
    int sign_l = (num_left < 0) ? -1 : 1;
    int sign_r = (num_right < 0) ? -1 : 1;
    unsigned int unl = num_left * sign_l;
    unsigned int unr = num_right * sign_r;
    printf("Pair to common denominator: (%d/%u), (%d/%u), lcm2 = %u;\n",
           num_left, den_left, num_right, den_right, l);
    unl *= (l / den_left);
    unr *= (l / den_right);
    if (is_int_overflow(unl, sign_l) || is_int_overflow(unr, sign_r))
        printf("warning: left or right numerator is more than integer max;\n");
    num_left = unl * sign_l;
    num_right = unr * sign_r;
    set_numerator(left, &num_left);
    set_numerator(right, &num_right);
    set_denominator(left, &l);
    set_denominator(right, &l);
    return 1;
}

void add(struct rational* dst, struct rational* src)
{   // Сложение объектов с сохранением результата, чтобы не вводить 3-й параметр.
    if (!is_correct(dst) || !is_correct(src)) {
        printf("error: add dst or src is NULL;\n");
        return;
    }
    printf("Addition of two object;\n");
    printf("First: ");
    print(dst, 1);
    printf("Second: ");
    print(src, 1);
    int num_dst = *get_numerator(dst);
    struct rational src_copy;
    copy(&src_copy, src);
    to_common_den(dst, &src_copy);
    num_dst = *get_numerator(dst) + *get_numerator(&src_copy);
    set_numerator(dst, &num_dst);
    print(dst, 1);
    printf("Result: ");
    print(dst, 1);
}

void sub(struct rational* dst, struct rational* src)
{   // Вычитание рациональных чисел.
    if (!is_correct(dst) || !is_correct(src)) {
        printf("error: sub dst or src is incorrect;\n");
        return;
    }
    printf("Subtraction of two object;\n");
    printf("First: ");
    print(dst, 1);
    printf("Second: ");
    print(src, 1);
    struct rational src_copy;
    copy(&src_copy, src);
    negative(&src_copy);
    add(dst, &src_copy);
    print(dst, 1);
    printf("Result: ");
    print(dst, 1);
}

void multiply2(struct rational* dst, struct rational* src)
{   // Умножение рациональных чисел.
    if (!is_correct(dst) || !is_correct(src)) {
        printf("error: mul dst or src is incorrect;\n");
        return;
    }
    printf("Multiplication of two object;\n");
    printf("First: ");
    print(dst, 1);
    printf("Second: ");
    print(src, 1);
    int num_dst = *get_numerator(dst);
    unsigned int den_dst = *get_denominator(dst), dn = num_dst * ((num_dst < 0) * -1);
    int num_src = *get_numerator(src);
    unsigned int den_src = *get_denominator(src), sn = num_src * ((num_src < 0) * -1);
    if (is_int_overflow(dn * sn, ((num_dst < 0) * -1) * ((num_src < 0) * -1)))
        printf("warning, numerator is overflow integer type;\n");
    num_dst *= num_src;
    den_dst *= den_src;
    set_numerator(dst, &num_dst);
    set_denominator(dst, &den_dst);
    printf("Result: ");
    print(dst, 1);
}

void divide(struct rational* dst, struct rational* src)
{   // Деление рациональных чисел.
    if (!is_correct(dst) || !is_correct(src)) {
        printf("error: mul dst or src is incorrect;\n");
        return;
    }
    printf("Dividing of two object;\n");
    printf("First: ");
    print(dst, 1);
    printf("Second: ");
    print(src, 1);
    inverse(src);
    multiply2(dst, src);
    inverse(src);
    printf("Result: ");
    print(dst, 1);
}

void modulus(struct rational* dst, struct rational* src)
{   // Остаток от деления заносится в числитель левостороннего объекта.
    if (!is_correct(dst) || !is_correct(src) || *get_numerator(dst) == 0) {
        printf("error: modulus dst or src is incorrect;\n");
        return;
    }
    int nl = *get_numerator(dst);
    int nr = *get_numerator(src);
    unsigned int dl = *get_denominator(dst);
    unsigned int dr = *get_denominator(src);
    printf("Modulus: %d/%u mod %d/%u, using div mul and sub;\n", nl, dl, nr, dr);
    struct rational dst_copy, src_copy, sub_int_div;
    copy(&dst_copy, dst);
    copy(&src_copy, src);
    divide(&dst_copy, &src_copy);
    nl = *get_numerator(&dst_copy);
    dl = *get_denominator(&dst_copy);
    int int_div = nl / dl;
    printf("Integer divider for rational is %d;\n", int_div);
    create(&sub_int_div, int_div, 1);
    multiply2(&sub_int_div, &src_copy);
    sub(dst, &sub_int_div);
    printf("Result: ");
    print(dst, 1);
}

void power(struct rational* dst, int pow)       // struct rational* pow - оставим на потом...
{   // Возведение в степень, по правилам математики.
    if (dst == NULL) {
        printf("error: dst is NULL;\n");
        return;
    }
    printf("Exponentiation of a rational number;\n");
    int value = 1;
    unsigned int uvalue = 1;
    if (!pow) {
        set_numerator(dst, &value);
        set_denominator(dst, &uvalue);
        return;
    }
    int s = (pow > -1) ? 1 : -1;
    pow *= s;
    if (s == -1)
        inverse(dst);
    int* num_dst = get_numerator(dst);
    unsigned int* den_dst = get_denominator(dst);
    int num_tmp = *num_dst;
    unsigned int den_tmp = *den_dst;
    for (int i = 0; i < pow - 1; ++i)
        *num_dst *= num_tmp;
    for (int i = 0; i < pow - 1; ++i)
        *den_dst *= den_tmp;
}

int is_equal(struct rational* left, struct rational* right)
{   // Проверка, если дроби равны. Возвращаем 1 или 0 в зависимости от результата.
    if (!is_correct(left) || !is_correct(right)) {
        printf("error: is equal left or right is incorrect;\n");
        return -1;      // или ноль
    }
    int num_left = *get_numerator(left);
    int num_right = *get_numerator(right);
    int sign_l = (num_left < 0) ? -1 : 1, sign_r = (num_right < 0) ? -1 : 1;
    unsigned int den_left = *get_denominator(left), den_right = *get_denominator(left);
    printf("Check if (%d/%u == %d/%u);\n", num_left, den_left, num_right, den_right);
    if (sign_l != sign_r) {
        printf("signs of left and right numerators are different, rationals not equal");
        return 0;
    }
    struct rational l_copy, r_copy;
    copy(&l_copy, left);
    copy(&r_copy, right);
    to_common_den(&l_copy, &r_copy);
    printf("With common denominator;\n");
    print(&l_copy, 1);
    print(&r_copy, 1);
    if (*get_numerator(&l_copy) == *get_numerator(&r_copy)) {
        printf("Is equal;\n");
        return 1;
    } else {
        printf("Not equal;\n");
        return 0;
    }
}

int is_more(struct rational* left, struct rational* right)
{   // Проверка, если первое число больше второго. Возвращаем 1 или 0, в зависимости от результата.
    if (!is_correct(left) || !is_correct(right)) {
        printf("error: is more left or right is incorrect;\n");
        return -1;
    }
    int num_left = *get_numerator(left);
    int num_right = *get_numerator(right);
    int sign_l = (num_left < 0) ? -1 : 1, sign_r = (num_right < 0) ? -1 : 1;
    unsigned int den_left = *get_denominator(left), den_right = *get_denominator(left);
    printf("Check if (%d/%u == %d/%u);\n", num_left, den_left, num_right, den_right);
    if (sign_l > sign_r) {
        printf("left sign is less than right, left rational more than right;\n");
        return 1;
    } else if (sign_l < sign_r) {
        printf("left sign is more than right, left rational less than right;\n");
        return 0;
    }
    struct rational l_copy, r_copy;
    copy(&l_copy, left);
    copy(&r_copy, right);
    to_common_den(&l_copy, &r_copy);
    printf("With common denominator;\n");
    print(&l_copy, 1);
    print(&r_copy, 1);
    if (*get_numerator(&l_copy) > *get_numerator(&r_copy)) {
        printf("Is more;\n");
        return 1;
    } else {
        printf("Not more;\n");
        return 0;
    }
}

int is_less(struct rational* left, struct rational* right)
{   // Проверка, если первое число меньше второго. Возвращаем 1 или 0, в зависимости от результата.
    if (!is_correct(left) || !is_correct(right)) {
        printf("error: is more left or right is incorrect;\n");
        return -1;
    }
    int num_left = *get_numerator(left);
    int num_right = *get_numerator(right);
    int sign_l = (num_left < 0) ? -1 : 1, sign_r = (num_right < 0) ? -1 : 1;
    unsigned int den_left = *get_denominator(left), den_right = *get_denominator(left);
    printf("Check if (%d/%u == %d/%u);\n", num_left, den_left, num_right, den_right);
    if (sign_l < sign_r) {
        printf("left sign is less than right, left rational more than right;\n");
        return 0;
    } else if (sign_l > sign_r) {
        printf("left sign is more than right, left rational less than right;\n");
        return 1;
    }
    struct rational l_copy, r_copy;
    copy(&l_copy, left);
    copy(&r_copy, right);
    to_common_den(&l_copy, &r_copy);
    printf("With common denominator;\n");
    print(&l_copy, 1);
    print(&r_copy, 1);
    if (*get_numerator(&l_copy) < *get_numerator(&r_copy)) {
        printf("Is less;\n");
        return 1;
    } else {
        printf("Not less;\n");
        return 0;
    }
}

int compare(struct rational* left, struct rational* right,
            int(*cmp)(struct rational*, struct rational*))
{   // Дополнительная функция сравнения, на вход дополнительно адрес функции для сравнения.
    printf("Calling compare function from pointer %p;\n", cmp);
    int r = cmp(left, right);
    return r;
}

double to_double(struct rational* src)
{   // Простое преобразование из рационального в действительное, точность без ограничения.
    return (double)(*get_numerator(src)) / (double)(*get_denominator(src));
}

int to_string(struct rational* src, char* string)
{   // Преобразование в строку, аналогичное функции print. На выходе - количество фактических символов.
    if (src == NULL || string == NULL) {
        printf("error: object or string is NULL;\n");
        return -1;
    }
    unsigned int i = 0, j = 0, mult = 0, dc, mc;
    int num = *get_numerator(src);
    unsigned int den = *get_denominator(src);
    const unsigned int base = 10, params = 2;
    if (num < 0) {
        string[i++] = '-';
        num *= -1;
    }
    for (j = 0; j < params; ++j) {
        if (j == 0)
            dc = num;
        else
            dc = den;
        mult = 1; mc = dc;
        while ((mc /= base))
            mult *= base;
        printf("%d dc = %d;\n", mult, dc);
        while (mult) {
            string[i++] = (dc / mult) % base + '0';
            printf("'%c' ", string[i - 1]);
            mult /= base;
        }
        if (j == 0)
            string[i++] = '/';
        else
            string[i++] = '\0';
    }
    return i;
}

int rational_numbers_tests()
{
    printf("Laboratory 3. Rational numbers and operators.\n\n");
    printf("Integer size is %u bytes and integer minimum %d and maximum %d;\n", sizeof(int), INT_MIN, INT_MAX);
    printf("Object of 'Rational' size is %u.\n", sizeof(struct rational));     // Общий размер объекта.
    struct rational r1, r2, r3;                                                 // Тестовые объекты.
    struct rational* ptr1 = &r1, *ptr2 = &r2, *ptr3 = &r3;                      // Адреса объектов для перемещения.
    printf("Offset:\tSize:\tComment:\n");                                       // Смещение данных в базовом объекте и их размеры, таблица.
    printf("%u\t%u\tNumerator for rational with sign.\n", (void*)&r1.numerator - (void*)&r1, sizeof(r1.numerator));
    printf("%u\t%u\tDenominator for rational unsigned.\n", (void*)&r1.denominator - (void*)&r1, sizeof(r1.denominator));
    // Вывести таблицу адресов для функций: is_equal, is_more, is_less.
    printf("\nAddress table, of compare functions.\n");
    printf("Name:\t\tAddr:\n");
    printf("is_equal:\t%p\n", &is_equal);
    printf("is_more:\t%p\n", &is_more);
    printf("is_less:\t%p\n", &is_less);

    // Создание объектов, их копирование и перемещение.
    int r = 0;
    unsigned int ur = 0;
    char string[TEXT_MAX];
    printf("\nCreate objects, include one incorrect, move and copy to other object;\n");
    ptr3 = NULL;
    create(ptr1, 1, 2);
    create(ptr2, 0, 0);
    create(ptr3, 2, 3);
    ptr3 = &r3;
    copy(ptr3, ptr1);
    move(&ptr2, &ptr1);
    printf("Result after moving, using print rational: ");
    print(ptr2, 1);

    //Вызываем методы для чтения, записи и проверки на корректность
    printf("\nTest set/get functions with one incorrect, trying to set 1/3;\n");
    r = 1; ur = 3;
    set_numerator(ptr3, &r);
    set_denominator(ptr3, &ur);
    get_denominator(NULL);
    set_numerator(ptr1, NULL);
    printf("Reading result of rational: %d/%u, ", *get_numerator(ptr3), *get_denominator(ptr3));
    printf("is correct rational %d;\n", is_correct(ptr3));

    //Функции конвертации и нормализации рационального числа.
    printf("\nConverting functions rational and normalize.\n");
    r = from_double(ptr2, 0.33, 10);
    printf("Converting from double to rational %d: ", r);
    print(ptr2, 1);
    r = from_double(ptr3, 0.0, 10);
    r = from_string(ptr3, "ABCDE--+221+-chto-3 /+ABC5   omg ?1");
    printf("Signs or other chars in any position, result %d: ", r);
    print(ptr3, 1);
    return 0;
    print(ptr3, 1);
    r = from_string(ptr3, "-2147483648/2");
    normalize(ptr3);
    printf("\nConvert integer minimum, result = %d: ", r);
    print(ptr3, 1);
    normalize(NULL);
    r = to_string(ptr3, string);
    printf("Convert to string, length %d: '%s'\n", r, string);
    r = to_string(NULL, string);

    //Функции инкементи и декримента, и арифметические.
    printf("\nAll arithmetics operations and inc dec;\n");
    printf("Check overflow with integer maximum %d and minimum %d;\n", INT_MAX, INT_MIN);
    int nums_add[] = {0, INT_MAX, INT_MIN, -1, INT_MIN + 1, INT_MAX, INT_MAX, 1};
    int nums_sub[] = {-1, INT_MIN, INT_MAX, -1, -1, INT_MAX, INT_MAX, INT_MIN + 1};
    int nums_mul[] = {-1, INT_MIN, INT_MAX, -1, INT_MIN, 2, INT_MAX, 0};
    int nums_div[] = {INT_MIN, -1, INT_MAX, INT_MAX, INT_MIN, INT_MAX, INT_MAX, 0};
    int nums_mod[] = {INT_MIN, INT_MAX};
    int nums_pow[] = {INT_MIN, -1, INT_MAX, -1};
    for (unsigned int i = 0, d = 1; i < sizeof(nums_add) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_add[i * 2]);
        set_denominator(ptr2, &d);
        set_numerator(ptr3, &nums_add[i * 2 + 1]);
        set_denominator(ptr3, &d);
        add(ptr2, ptr3);
        printf("\n");
    }
    for (unsigned int i = 0, d = 1; i < sizeof(nums_sub) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_sub[i * 2]);
        set_denominator(ptr2, &d);
        set_numerator(ptr3, &nums_sub[i * 2 + 1]);
        set_denominator(ptr3, &d);
        sub(ptr2, ptr3);
        printf("\n");
    }
    for (unsigned int i = 0, d = 1; i < sizeof(nums_mul) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_mul[i * 2]);
        set_denominator(ptr2, &d);
        set_numerator(ptr3, &nums_mul[i * 2 + 1]);
        set_denominator(ptr3, &d);
        multiply2(ptr2, ptr3);
        printf("\n");
    }
    for (unsigned int i = 0, d = 1; i < sizeof(nums_div) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_div[i * 2]);
        set_denominator(ptr2, &d);
        set_numerator(ptr3, &nums_div[i * 2 + 1]);
        set_denominator(ptr3, &d);
        divide(ptr2, ptr3);
        printf("\n");
    }
    for (unsigned int i = 0, d = 1; i < sizeof(nums_mod) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_mod[i * 2]);
        set_denominator(ptr2, &d);
        set_numerator(ptr3, &nums_mod[i * 2 + 1]);
        set_denominator(ptr3, &d);
        modulus(ptr2, ptr3);
        printf("\n");
    }
    for (unsigned int i = 0, d = 1; i < sizeof(nums_pow) / sizeof(int) / 2; ++i) {
        set_numerator(ptr2, &nums_pow[i * 2]);
        set_denominator(ptr2, &d);
        power(ptr2, nums_pow[i * 2 + 1]);
        printf("\n");
    }
    normalize(ptr3);
    //Ввод с клавиатуры работает, точно!
    //r = input(ptr3);
    //printf("Input result = %d, rational = ", r);
    //print(ptr3, 1);
    // Тестирование функций сравнения и вызов функций сравнения по адресу.
    printf("\nTest all compare functions;\n");
    is_equal(ptr1, ptr2);
    printf("\n");
    is_more(ptr1, ptr2);
    printf("\n");
    is_less(ptr1, ptr2);
    printf("\n");
    int(*cmp_ptr)(struct rational*, struct rational*) = &is_equal;
    compare(ptr1, ptr2, cmp_ptr);
    return 0;
}




