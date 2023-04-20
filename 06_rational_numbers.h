#ifndef RATIONAL_NUMBERS
#define RATIONAL_NUMBERS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TEXT_MAX 0x100

struct rational {               // Простая структура представления рационального числа.
    int numerator;              // Числитель со знаком. Или можно было знак реализовать отдельным параметром.
    unsigned int denominator;   // Знаменатель. Любое положительное число, выше 0. При нуле выходим с сообщением.
};

int* get_numerator(struct rational* r);
unsigned int* get_denominator(struct rational* r);
void set_numerator(struct rational* r, int* num);
void set_denominator(struct rational* r, unsigned int* den);
int is_correct(struct rational* r);
void print(struct rational* r, int endl);
void create(struct rational* r, int num, unsigned int den);
void copy(struct rational* dst, struct rational* src);
void move(struct rational** dst, struct rational** src);
unsigned int gcd2(unsigned int a, unsigned int b);
unsigned int lcm2(unsigned int a, unsigned int b);
int normalize(struct rational* r);
void inverse(struct rational* r);
int is_int_overflow(unsigned int n, int sign);
void negative(struct rational* r);
int from_double(struct rational* r, double n, int MaxGuess);
int from_string(struct rational* r, char src[]);
int input(struct rational* r);
void inc(struct rational* dst);
void dec(struct rational* dst);
int to_common_den(struct rational* left, struct rational* right);
void add(struct rational* dst, struct rational* src);
void sub(struct rational* dst, struct rational* src);
void multiply2(struct rational* dst, struct rational* src);
void divide(struct rational* dst, struct rational* src);
void modulus(struct rational* dst, struct rational* src);
void power(struct rational* dst, int pow);
int is_equal(struct rational* left, struct rational* right);
int is_more(struct rational* left, struct rational* right);
int is_less(struct rational* left, struct rational* right);
int compare(struct rational* left, struct rational* right,
            int(*cmp)(struct rational*, struct rational*));
double to_double(struct rational* src);
int to_string(struct rational* src, char* string);

#endif
