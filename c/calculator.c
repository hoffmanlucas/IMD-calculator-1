#include <stdio.h>
#include <math.h>

#include <glib.h>

typedef struct product {
    int *coefficients;
    double sum;
} product_t;

struct constants_t {
    int order;
    int midpoint;
    double* transmitFreqs;
    size_t freqsLen;
};

void add_positive_signal(GArray* products, int* coefficients, int depth, double sum, int lastIndex, struct constants_t* constants);
void add_negative_signal(GArray* products, int* coefficients, int depth, double sum, int lastIndex, struct constants_t* constants);

void calculate(double* transmitFreqs, size_t freqsLen, int order) {
    int midpoint;
    int *coefficients;

    printf("Calculating...\n");

    // Check that the order is odd
    if (order % 2 == 0) {
        fprintf(stderr, "Error: order must be odd\n");
        exit(1);
    } else if (order < 3) {
        fprintf(stderr, "Error: order must be at least 3\n");
        exit(1);
    }

    // Calculate the midpoint
    midpoint = (int) ceil(order/2.0);
    // Allocate a coefficients array to track the coefficients for each freq
    coefficients = (int*)malloc(freqsLen * sizeof(int));

    GArray* products = g_array_new(FALSE, FALSE, sizeof(product_t*));
    // product_t* product;
    // product->sum = 0;
    // g_array_append_val(products, product);

    // Store some values as constants
    struct constants_t constants;
    constants.freqsLen = freqsLen;
    constants.order = order;
    constants.midpoint = midpoint;
    constants.transmitFreqs = transmitFreqs;

    add_positive_signal(products, coefficients, 1, 0, 0, &constants);

    printf("Printing products\n");
    // Print the array of products
    printf("%i\n", products->len);
    product_t* product;
    for (int i = 0; i < products->len; i++) {
        product = g_array_index(products, product_t*, i);
        // Print the array of coefficients
        printf("[");
        for (int j = 0; j < freqsLen; j++) {
            printf("%d ", product->coefficients[j]);
        }
        printf("] %f\n", product->sum);
    }
}

void add_positive_signal(GArray* products, int* coefficients, int depth, double sum, int lastIndex, struct constants_t* constants) {
    // Iterate over all coefficients, starting from the last index
    for (int i = lastIndex; i < constants->freqsLen; i++) {
        // Skip harmonics, which occur when one coefficient is equal to the order
        if (coefficients[i] == constants->order - 1) {
            continue;
        }

        // Create a copy of the coeffiecients and sum so we don't modify the original each loop
        int updatedCoefficients[constants->freqsLen];
        memcpy(updatedCoefficients, coefficients, constants->freqsLen * sizeof(int));
        double updatedSum = sum;

        // Increment the current coefficient
        updatedCoefficients[i] += 1;
        // Add the transmit frequency to the sum
        updatedSum += constants->transmitFreqs[i];

        // If we haven't reached the deepest depth, recurse some more
        if (depth < constants->order) {
            add_positive_signal(products, updatedCoefficients, depth + 1, updatedSum, i, constants);

            // If we have passed the midpoint, add a negative signal
            if (depth >= constants->midpoint) {
                add_negative_signal(products, updatedCoefficients, depth + 1, updatedSum, i, constants);
            }
        } else {
            // If we have reached the deepest depth, add the product to products
            // Note that the coefficients will not be modified as we are not recursing any more
            product_t* product = (product_t*)malloc(sizeof(product_t));
            
            product->coefficients = updatedCoefficients;
            product->sum = updatedSum;
            // print the coefficients
            for (int j = 0; j < constants->freqsLen; j++) {
                printf("%d ", product->coefficients[j]);
            }
            printf("-> %f\n", product->sum);
            g_array_append_val(products, product);
        }
    }

}

void add_negative_signal(GArray* products, int* coefficients, int depth, double sum, int lastIndex, struct constants_t* constants) {
    // If the coefficient at the last index was positive, we can subtract from earlier indices
    // This is very important as it allows us to avoid generating invalid products
    // If the coefficient is negative, this means we have started performing add_negative_signal
    // which is a function that can only call itself, meaning we should start iterating from
    // 0 if (and only if) the last function in the stack was add_positive_signal
    lastIndex = coefficients[lastIndex] < 0 ? lastIndex : 0;

    for (int i = lastIndex; i < constants->freqsLen; i++) {
        // Skip any coefficients that have already been added in add_positive_signal
        if (coefficients[i] > 0) {
            continue;
        }

        // Create a copy of the coeffiecients and sum so we don't modify the original each loop
        int updatedCoefficients[constants->freqsLen];
        memcpy(updatedCoefficients, coefficients, constants->freqsLen * sizeof(int));
        double updatedSum = sum;

        // Decrement the current coefficient
        updatedCoefficients[i] -= 1;
        // Subtract the transmit frequency from the sum
        updatedSum -= constants->transmitFreqs[i];

        // If we haven't reached the deepest depth, recurse some more
        if (depth < constants->order) {
            // We do not add positive signal (see above), so we only call add_negative_signal
            add_negative_signal(products, updatedCoefficients, depth + 1, updatedSum, i, constants);
        } else {
            // If we have reached the deepest depth, add the product to products
            // Note that the coefficients will not be modified as we are not recursing any more
            product_t* product = (product_t*)malloc(sizeof(product_t));
            product->coefficients = updatedCoefficients;
            product->sum = updatedSum;
            // print the coefficients
            for (int j = 0; j < constants->freqsLen; j++) {
                printf("%d ", product->coefficients[j]);
            }
            printf("-> %f\n", product->sum);
            g_array_append_val(products, product);
        }
    }
}

void main() {
    double transmitFreqs[] = {101.0, 101.5, 102.0};
    
    // Calculate the number of frequencies
    size_t freqsLen = sizeof(transmitFreqs) / sizeof(transmitFreqs[0]);
    int order = 3;

    calculate(transmitFreqs, freqsLen, order);
}