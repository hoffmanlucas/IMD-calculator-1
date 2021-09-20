#include <stdio.h>
#include <math.h>

#include <glib.h>

typedef struct product {
    double *coefficients;
    double sum;
} product_t;

void calculate(double *transmit_freqs, int order) {
    int freqs_len;
    int midpoint;
    double *coefficients;

    // Check that the order is odd
    if (order % 2 == 0) {
        fprintf(stderr, "Error: order must be odd\n");
        exit(1);
    } else if (order < 3) {
        fprintf(stderr, "Error: order must be at least 3\n");
        exit(1);
    }

    // Calculate the number of frequencies
    freqs_len = sizeof(transmit_freqs) / sizeof(transmit_freqs[0]);
    // Calculate the midpoint
    midpoint = (int) ceil(order/2.0);
    // Allocate a coefficients array to track the coefficients for each freq
    coefficients = (double*)malloc(freqs_len * sizeof(double));

    GArray* products = g_array_new(FALSE, FALSE, sizeof(product_t*));
    // product_t* product;
    // product->sum = 0;
    // g_array_append_val(products, product);
}

void add_positive_signal(GArray* products, double* coefficients, int depth, double sum, int last_index) {

}

void add_negative_signal(GArray* products, double* coefficients, int depth, double sum, int last_index) {

}

void main() {
    double transmit_freqs[] = {101.12, 101.15, 100.97, 99.64, 112.54, 102.32, 101.74};
    int order = 15;

    calculate(transmit_freqs, order);
}