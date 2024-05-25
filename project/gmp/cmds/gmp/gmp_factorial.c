#include <stdio.h>
#include <gmp.h>

void factorial(mpz_t result, unsigned int n) {
    mpz_set_ui(result, 1);
    for (unsigned int i = 1; i <= n; i++) {
        mpz_mul_ui(result, result, i);
    }
}

int main(int argc, char **argv) {
    mpz_t a, b, sum, product, fact;
    mpz_inits(a, b, sum, product, fact, NULL);

    mpz_set_str(a, "123456789012345678901234567890", 10);
    mpz_set_str(b, "987654321098765432109876543210", 10);

    mpz_add(sum, a, b);

    mpz_mul(product, a, b);

    factorial(fact, 20);

    gmp_printf("a + b = %Zd\n", sum);
    gmp_printf("a * b = %Zd\n", product);
    gmp_printf("20! = %Zd\n", fact);

    mpz_clears(a, b, sum, product, fact, NULL);

    return 0;
}
