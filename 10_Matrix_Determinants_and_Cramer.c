#include "10_matrix_determinants_and_cramer.h"


// Небольшая лабораторная на квадратные матрицы и свободные элементы, с ограничениями по размеру и стандартными методами.
// Для использования более универсального метода вычисления определителя в методе Крамера, раскоментировать строчку.
// По умолчанию универсальный метод только тестируется отдельно, а решения находятся простым. Для улучшения вывода.

// Если строка и/или столбец параметров функции равны -1, то выводим их полностью, иначе выводится только
// одна строка или столбец. Если заданы оба, то получается вывод единственного элемента. Элемент в столбце
// 'size' считается свободной константой. Вместо размерностей, на вход можно подать просто адрес первого элемента по вкусу.
// Если на входе функции size равняется нулю, то ошибкой не считается и просто выводится информация.

void print_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col, unsigned int endl)
{   // Вывод системы на экран заданного размера, ограничение в два числа после точки. Если размер 0, то информация.
    if (size > MX_MAX_SIZE || row >= MX_MAX_SIZE || col >= MS_MAX_SIZE || row < -1 || col < -1) {
        printf("Error print matrix, size %u, row %d or column %d incorrect.\n", size, row, col);
        return;
    }
    printf("Matrix with size %u, row %d and column %d at address [%p]:\n", size, row, col, matrix);
    unsigned int i = (row == -1) ? 0 : row, i_max = (row == -1) ? size - 1: row;
    for (; i <= i_max; ++i) {                                   // Меньше или равно для вывода одной строки.
        unsigned int j = (col == -1) ? 0 : col, j_max = (col == -1) ? size - 1 : col;
        for (; j <= j_max; ++j)
            printf("%7.2f\t", matrix[i][j]);
        printf("| %7.2f\n", matrix[i][size]);
    }
    while (endl--)
        printf("\n");
}

void create_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col, double value)
{   // Создание матрицы заданного размера и заполнением элементов значением value.
    // Если value NAN, то используется генератор случайных чисел с ограничением статичной переменной 'rnd_mod_max'.
    if (size > MX_MAX_SIZE || row >= MX_MAX_SIZE || col >= MS_MAX_SIZE || row < -1 || col < -1) {
        printf("Error create matrix, size %u, row %d or column %d incorrect.\n", size, row, col);
        return;
    }
    if (isnan(value))
        printf("Create matrix size %u, row %d, column %d and random max is %d, at address [%p].\n",
               size, row, col, rnd_mod_max, matrix);
    else
        printf("Create matrix size %u, row %d, column %d and set all elements to %.2f, at address [%p].\n",
               size, row, col, value, matrix);
    unsigned int i = (row == -1) ? 0 : row, i_max = (row == -1) ? size - 1: row;
    for (; i <= i_max; ++i) {
        unsigned int j = (col == -1) ? 0 : col, j_max = (col == -1) ? size : col;
        for (; j <= j_max; ++j)
            if (isnan(value))                                   // Случайная генерация целой и дробных частей.
                matrix[i][j] =  (double)(rand() % rnd_mod_max) +
                                ((double)(rand() % rnd_mod_max) / (double)rnd_mod_max);
            else
                matrix[i][j] = value;
    }
}

void copy_matrix(double dst[MX_MAX_SIZE][MS_MAX_SIZE], double src[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col)
{   // Копирование всех или части элементов матрицы исходника в матрицу назначения, поэлементно или блоками памяти.
    if (size > MX_MAX_SIZE || row >= MX_MAX_SIZE || col >= MS_MAX_SIZE || row < -1 || col < -1) {
        printf("Error copy matrix, size %u, row %d or column %d incorrect.\n", size, row, col);
        return;
    }
    printf("Copy matrix size %u, row %d and column %d from address [%p] to [%p].\n", size, row, col, src, dst);
    unsigned int i = (row == -1) ? 0 : row, i_max = (row == -1) ? size - 1: row;
    for (; i <= i_max; ++i) {
        unsigned int j = (col == -1) ? 0 : col, j_max = (col == -1) ? size : col;
        for (; j <= j_max; ++j)
            dst[i][j] = src[i][j];
    }
}

void input_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col)
{   // Ввод матрицы с клавиатуры, все числа интерпретируются как числа с плавающей точкой.
    // Можно сгенерировать всю матрицу автоматически заполнив значениями или случайными числами.
    printf("Input matrix, choose action, F - full input, R - random values and S - set to one value: ");
    int key = toupper(getchar());
    switch (key) {
    case 'S': {
        printf("Enter floating point value to set to all elements in matrix: ");
        double value;
        scanf("%lf", &value);
        create_matrix(matrix, size, row, col, value);
        break;
    } case 'R': {
        create_matrix(matrix, size, row, col, NAN);
        break;
    } case 'F': {
        unsigned int i = (row == -1) ? 0 : row, i_max = (row == -1) ? size - 1: row;
        for (; i <= i_max; ++i) {
            unsigned int j = (col == -1) ? 0 : col, j_max = (col == -1) ? size : col;
            for (; j <= j_max; ++j) {
                double value = 0.0;
                printf("Enter element at %u row, %u column, or empty to default 0.0: ", i, j);
                scanf("%lf", &value);
                matrix[i][j] = value;
            }
        }
        break;
    } default:
        printf("Key '%c' is not an action, exit.\n", key);
        return;
    }
}

double determinant_simple(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power, unsigned int* it)
{   // Вычисления определителя по простым формулам, минимального порядка не выше 3.
    if (power > 3 || power == 0) {
        printf("Simple determinant can't calculate above power 3 or 0, actual %u.\n", power);
        return NAN;
    }
    double det;
    printf("Determinant using simple preset calculation, iterations 1.\n");
    (*it)++;
    switch (power) {
    case 1:                                                     // Один элемент.
        det = matrix[0][0];
        break;
    case 2:                                                     // Два на два.
        det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        break;
    case 3:                                                     // Три на три.
        det = matrix[0][0] * matrix[1][1] * matrix[2][2];
        det -= matrix[0][0] * matrix[1][2] * matrix[2][1];
        det -= matrix[0][1] * matrix[1][0] * matrix[2][2];
        det += matrix[0][1] * matrix[1][2] * matrix[2][0];
        det += matrix[0][2] * matrix[1][0] * matrix[2][1];
        det -= matrix[0][2] * matrix[1][1] * matrix[2][0];
        break;
    default:
        printf("Error in simple determinant, return NaN.\n");
        return NAN;
    }
    return det;
}

double determinant_slow(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power)
{   // Вычисление определителя с построением перестановок простым возрастающим перебором. Быть осторожнее с высокими порядками.
    // Т.к. алгоритм экспоненциально расходует память и достаточно медленный. Проверки по примерам из учебников.
    double det = 0.0;
    unsigned int permut[MX_MAX_SIZE * 2];   // coords.
    unsigned int permut_data[MX_MAX_FACTORIAL][MX_MAX_SIZE * 2];    // prev
    unsigned int i, j, k, permut_size = 0, inversions, is_equal = 0, is_pow = 0;
    printf("\nDeterminant using slow and memory greedy algorithm, source matrix:\n");
    print_matrix(matrix, power, -1, -1, 1);
    for (i = 0; i < MX_MAX_FACTORIAL; ++i)                      // Инициализация данных перестановок.
        for (j = 0; j < MX_MAX_SIZE * 2; ++j) {
            permut_data[i][j] = 0;
            permut[j] = 0;
        }
    while (!is_pow) {                                           // От условного нуля и до максимальных индексов.
        is_equal = 0;
        for (i = 0; i < power && !is_equal; ++i)
            for (j = 0; j < power && !is_equal; ++j)
                if (i != j && (permut[i * 2] == permut[j * 2] || permut[i * 2 + 1] == permut[j * 2 + 1]))
                        is_equal = 1;
        if (!is_equal) {
            printf("Permutation with unique indexes: ");
            for (i = 0; i < power; ++i)
                printf("[%u,%u] ", permut[i * 2], permut[i * 2 + 1]);
            for (i = 0; i < permut_size && is_equal < power; ++i) {
                is_equal = 0;
                for (j = 0; j < power && is_equal < power; ++j)
                    for (k = 0; k < power && is_equal < power; ++k)
                        if (permut[j * 2] == permut_data[i][k * 2] && permut[j * 2 + 1] == permut_data[i][k * 2 + 1])
                            is_equal++;
            }
            if (is_equal < power) {
                printf("- no previous data, add to permutations data.\n");
                for (i = 0; i < power * 2; ++i)
                    permut_data[permut_size][i] = permut[i];
                permut_size++;
            } else
                printf("- permutation is already in data, at %u.\n", i - 1);
        }
        i = 0;
        while ((++permut[i]) == power)
            permut[i++] = 0;
        is_pow = 1;                                             // Заменить на расчет.
        for (i = 1, k = 0; i < power * 2; ++i, ++k)
            if (permut[i] != permut[k])
                is_pow = 0;
    }
    printf("\nPermutations to compute and inversions in indexes: ");
    for (i = 0; i < permut_size; ++i) {
        printf("\n%3u: ", i);
        for (j = 0; j < power; ++j)
            printf("[%u,%u] ", permut_data[i][j * 2], permut_data[i][j * 2 + 1]);
        inversions = 0;
        for (j = 0; j < power; ++j)
            for (k = 0; k < power; ++k)
                if (k != j && permut_data[i][j * 2] == permut_data[i][k * 2 + 1] &&
                        permut_data[i][j * 2 + 1] == permut_data[i][k * 2])
                    inversions++;
        inversions >>= 0x01;
        if (inversions % 2 == 0)
            printf("- %u inversions even, sign will be +1, ", inversions);
        else
            printf("- %u inversions odd, sign will be -1, ", inversions);
        double sign = (inversions % 2 == 0) ? +1.0 : -1.0, adder = 1.0;
        for (j = 0; j < power; j++) {
            unsigned int row = permut_data[i][j * 2], col = permut_data[i][j * 2 + 1];
            adder *= matrix[row][col];
        }
        det += (sign * adder);
        printf("adder %.2f, determinant %.2f;", adder, det);
    }
    printf("\n\n");
    return det;
}

unsigned int matrix_cramer(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power, double* result)
{   // Решение матрицы методом Крамера, используются все методы вычисления определителя.
    if (matrix == NULL || result == NULL || power == 0 || power > MX_MAX_SIZE) {
        printf("Error Cramer's rule, addresses [%p] or power %u incorrect.\n", result, power);
        return 0;
    }
    unsigned int it = 0;
    printf("Resolve matrix using Cramer's rule, source matrix:\n");
    print_matrix(matrix, power, -1, -1, 0);
    double det = determinant_simple(matrix, power, &it);
    // double det = determinant_slow(matrix, power, &it);
    if (fabs(det) < eps) {
        printf("Determinant is %.5f and less than %.5f of zero 'vincinity', no solutions.\n", det, eps);
        return 0;
    }
    printf("Common determinant for matrix is %.2f, copy columns of constants and get other determninants.\n", det);
    double mx[MX_MAX_SIZE][MS_MAX_SIZE];
    for (unsigned int column = 0, row; column < power; ++column) {
        copy_matrix(mx, matrix, power, -1, -1);
        for (row = 0; row < power; ++row)                       // Подстановка столбца или через копирование матриц.
            mx[row][column] = matrix[row][power];
        print_matrix(mx, power, -1, -1, 0);
        double det_col = determinant_simple(mx, power, &it);
        // double det_col = determinant_slow(mx, power);
        printf("Determinant for column %u is %.2f.\n", column, det_col);
        result[column] =  det_col / det;
        ++it;
    }
    return it;
}

int matrix_determinants_and_cramer(void)
{
    printf("Resolve simple matrix using Cramer's and Gaussian's algorithms, 32 bits.\n");
    printf("Size of char %u, short %u, int %u, double %u bytes and address width is %u bits.\n",
           sizeof(char), sizeof(short), sizeof(int), sizeof(double), sizeof(char*) * CHAR_BIT);
    printf("Maximum matrix size is %u plus one column of constants %u, if row or column argument is -1, mean 'all'.\n\n",
           MX_MAX_SIZE, MS_MAX_SIZE);
    // Матрица с элементами, которые тестируют порядок и корректность вывода.
    double mx_a[MX_MAX_SIZE][MS_MAX_SIZE] = { {1, 2, 3, 11}, {4, 5, 6, 22}, {7, 8, 9, 33} };
    // Определитель 13, результат 1/2 и 1/3.
    double mx_b[MX_MAX_SIZE][MS_MAX_SIZE] = { {3, -2, 5.0/6.0, 4}, {2, 3, 2, 1}, {3, -1, 0, 1} };
    // Определитель -7, результат 1, 2 и 3.
    double mx_c[MX_MAX_SIZE][MS_MAX_SIZE] = { {1, 0, 1, 4}, {0, 2, -1, 1}, {3, -1, 0, 1} };
    // Дополнительная система и вектор для решения.
    double mx_d[MX_MAX_SIZE][MS_MAX_SIZE], rv[MX_MAX_SIZE];
    unsigned int mx_a_size = 3, mx_b_size = 2, mx_c_size = 3, mx_d_size = 3;
    for (unsigned int i = 0; i < MX_MAX_SIZE; ++i)
        rv[i] = 0;
    // Тестирование вывода матрицы по умолчанию, чтобы проверить, все-ли элементы выводтся.
    printf("Print preset matrix to debug print function, matrix elements from [1..9] and constants are 11,22,33.\n");
    print_matrix(mx_a, MX_MAX_SIZE, -1, -1, 2);
    // Тестирование элементарных функций матриц и систем уравнений.
    printf("Calling basic functions for create(reset) and copy matrix.\n\n\n");
    create_matrix(mx_a, mx_a_size, -1, -1, 0);
    print_matrix(mx_a, mx_a_size, -1, -1, 1);
    create_matrix(mx_a, mx_a_size, -1, -1, NAN);
    print_matrix(mx_a, mx_a_size, -1, -1, 1);
    copy_matrix(mx_d, mx_a, mx_a_size, -1, -1);
    print_matrix(mx_d, mx_d_size, -1, -1, 2);
    printf("Print only one row, column or element in matrix at 1:1.\n\n\n");
    print_matrix(mx_a, mx_a_size, 1, -1, 1);
    print_matrix(mx_a, mx_a_size, -1, 1, 1);
    print_matrix(mx_a, mx_a_size, 1, 1, 2);
    // Тестирование ввода, можно закоментировать, если вызов утомляет.
    input_matrix(mx_b, mx_b_size, -1, -1);
    print_matrix(mx_b, mx_b_size, -1, -1, 2);
    // Тестирование простой функции вычисления определителя со всеми минимальными порядками.
    printf("Simple determinant function for matrix and sizes from [1..3].\n\n\n");
    print_matrix(mx_a, mx_a_size, -1, -1, 1);
    unsigned int p, it;
    for (p = 1, it = 0; p <= MX_MAX_SIZE; ++p)
        printf("Determinant power %d: %.2f.\n", p, determinant_simple(mx_a, p, &it));
    // Тестирование более универсального алгоритма для определителя.
    printf("\n\nDeterminant slower function for more than %u matrix sizes.\n\n", MX_MAX_SIZE);
    printf("Determinant for preset matrix must be -7.0, result is %.2f.\n", determinant_slow(mx_c, 3));
    // Алгоритм Крамера для вычисления решения системы, используя уже более универсальный алгоритм.
    printf("\n\nCramer's rule for preset matrix's. First matrix determinant is 13 and results must be (1/2, 1/3). "
           "Second matrix determinant -7 and results (1, 2, 3). If more than 3 matrix power needed, then "
           "uncomment 'determinant_slow' function in Cramer's method.\n\n\n");
    it = matrix_cramer(mx_b, mx_b_size, rv);
    printf("Result of Cramer's Rule for matrix B, iterations %u, result must be (1/2, 1/3): ", it);
    for (p = 0; p < mx_b_size; ++p)
        printf("%.2f ", rv[p]);
    printf("\n\n");
    // Вычисления для 3-его порядка.
    it = matrix_cramer(mx_c, mx_c_size, rv);
    printf("Result of Cramer's Rule for matrix C, iterations %u, result must be (1, 2, 3): ", it);
    for (p = 0; p < mx_c_size; ++p)
        printf("%.2f ", rv[p]);
    printf("\n");
    return 0;
}
