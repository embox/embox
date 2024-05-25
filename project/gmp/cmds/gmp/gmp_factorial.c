#include <stdio.h>
#include <gmp.h>

// Функция для вычисления факториала
void factorial(mpz_t result, unsigned int n) {
    mpz_set_ui(result, 1); // Инициализация result значением 1
    for (unsigned int i = 1; i <= n; i++) {
        mpz_mul_ui(result, result, i); // result *= i
    }
}

int main(int argc, char **argv) {
    // Инициализация больших чисел
    mpz_t a, b, sum, product, fact;
    mpz_inits(a, b, sum, product, fact, NULL);

    // Присваивание значений
    mpz_set_str(a, "123456789012345678901234567890", 10);
    mpz_set_str(b, "987654321098765432109876543210", 10);

    // Сложение a и b
    mpz_add(sum, a, b);

    // Умножение a и b
    mpz_mul(product, a, b);

    // Вычисление факториала числа 20
    factorial(fact, 20);

    // Вывод результатов
    gmp_printf("a + b = %Zd\n", sum);
    gmp_printf("a * b = %Zd\n", product);
    gmp_printf("20! = %Zd\n", fact);

    // Очистка памяти
    mpz_clears(a, b, sum, product, fact, NULL);

    return 0;
}
