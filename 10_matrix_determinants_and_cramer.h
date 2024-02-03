#ifndef MATRIX_CRAMER
#define MATRIX_CRAMER

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// Максимальный размер матрицы без учета свободных элентов.
#define MX_MAX_SIZE 0x03
// Максимальный размер матрицы и плюс свобододные константы, дополнительный параметр для читабельности.
#define MS_MAX_SIZE 0x04
// Максимально возможный факториал для размера матрицы и её  перестановок.
#define MX_MAX_FACTORIAL 6
// Максимальный размер определителя, дополнительно, если не потребуется высоких порядков.
#define DET_MAX_SIZE 0x03
// Максимальное случайное число по модулю, как для целой части так и для дробной.
static int rnd_mod_max = 10;
// Предел, за которым определитель в окрестности нуля считается нулем и система не имеет решений.
static double eps = 0.01;

// Если строка и/или столбец параметров функции равны -1, то выводим их полностью, иначе выводится только
// одна строка или столбец. Если заданы оба, то получается вывод единственного элемента. Элемент в столбце
// 'size' считается свободной константой. Вместо размерностей, на вход можно подать просто адрес первого элемента по вкусу.
// Если на входе функции size равняется нулю, то ошибкой не считается и просто выводится информация.

void print_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col, unsigned int endl);
void create_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col, double value);
void copy_matrix(double dst[MX_MAX_SIZE][MS_MAX_SIZE], double src[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col);
void input_matrix(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], int size, int row, int col);
double determinant_simple(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power, unsigned int* it);
double determinant_slow(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power);
unsigned int matrix_cramer(double matrix[MX_MAX_SIZE][MS_MAX_SIZE], unsigned int power, double* result);
int matrix_determinants_and_cramer(void);

#endif
