
import sys
import time
from typing import List

import numpy as np
import numba 


class IMDCalculator:
    def __init__(self, transmit_freqs: List[float]):
        # Validate the transmit frequencies
        if any([not isinstance(freq, (int, float)) for freq in transmit_freqs]):
            raise ValueError(
                "All transmit frequencies must be integers or floats. Got {}".format(
                    transmit_freqs
                )
            )
        self.transmit_freqs = transmit_freqs

        # Declare these now ready to make the code look nicer
        self.order = None
        self.products = None

    def calculate(self, order: int):
        coefficients = np.zeros(len(self.transmit_freqs), "int8")
        if order % 2 == 0:
            raise ValueError("order must be odd. Got {}".format(order))
        elif order < 3:
            raise ValueError("order must be 3 or higher. Got {}".format(order))
        self.order = order
        
        # pass in the first product so NUMBA can infer the types
        # this is a hacky way to get around the fact that NUMBA doesn't support lists
        # Remove it from the final result
        self.products: list = [(np.array([1], dtype=np.int8), 1.2)] 
        self.midpoint = int(np.ceil(order / 2.0))
        self.add_positive_signal(coefficients)
        
        return self.products


    def add_positive_signal(
        self,
        coefficients: np.ndarray,
        depth: int = 1,
        sum: float = 0,
        last_index: int = 0,
    ):
        return add_positive_signal(coefficients, self.transmit_freqs, self.midpoint, self.order, self.products, depth, sum, last_index)
    


@numba.jit(nopython=True)
def add_positive_signal(
    coefficients: np.ndarray,
    transmit_freqs: np.ndarray,
    midpoint: int,
    order: int,
    products: list,
    depth: int = 1,
    sum: float = 0,
    last_index: int = 0,
):
    for index in range(last_index, len(coefficients)):
        # Ignore harmonics
        # ? If we've only added to the same index, the signal is a harmonic
        if coefficients[index] == order - 1:
            continue

        updated_coefficients = coefficients.copy()
        updated_sum = sum

        updated_coefficients[index] += 1
        updated_sum += transmit_freqs[index]

        # If we aren't at the deepest depth, recurse some more
        if depth < order:
            add_positive_signal(
                updated_coefficients,
                transmit_freqs,
                midpoint,
                order,
                products,
                depth=depth + 1,
                sum=updated_sum,
                last_index=index,
            )
            # We only need to take the negative path when we at half the depth
            if depth >= midpoint:
                add_negative_signal(
                    updated_coefficients,
                    transmit_freqs,
                    midpoint,
                    order,
                    products,
                    depth=depth + 1,
                    sum=updated_sum,
                    last_index=index,
                )
        # If we are as deep as we can go, add the intermodulation
        else:
            products.append((updated_coefficients, round(updated_sum, 10)))

            
@numba.jit(nopython=True)
def add_negative_signal(
        coefficients: np.ndarray,
        transmit_freqs: np.ndarray,
        midpoint: int,
        order: int,
        products: list,
        depth: int = 1,
        sum: float = 0,
        last_index: int = 0,
    ):
        # If the last index to be added to was negative, skip all previous indices (there's a reason for this)
        last_index = last_index if coefficients[last_index] < 0 else 0

        for index in range(last_index, len(coefficients)):
            # Skip any terms that have already been added with positive interference
            if coefficients[index] > 0:
                continue

            updated_coefficients = coefficients.copy()
            updated_sum = sum

            updated_coefficients[index] -= 1
            updated_sum -= transmit_freqs[index]

            if depth < order:
                # We don't add positive signals because then we'd double up on results
                add_negative_signal(
                    updated_coefficients, 
                    transmit_freqs,
                    midpoint,
                    order,
                    products,
                    depth + 1, 
                    sum=updated_sum, 
                    last_index=index
                )
            else:
                products.append((updated_coefficients, round(updated_sum, 10)))


if __name__ == "__main__":

    if (len(sys.argv) < 4):
        print("Usage: python3 calculator.py <order> <transmit_freq_1> <transmit_freq_2> ...", file=sys.stderr)
        exit(1)

    order = int(sys.argv[1])
    transmit_freqs = [float(freq) for freq in sys.argv[2:]]

    # Print order
    print("ORDER: {}".format(order))
    # Print frequencies
    print("FREQUENCIES: {}".format(transmit_freqs))

    calc = IMDCalculator(transmit_freqs)
    start = time.time()
    result = calc.calculate(order)[1:] # Remove the first item - it's a hacky way to get around NUMBA's lack of list support
    end = time.time()
    print("Time taken: {} seconds".format(round(end - start, 5)))
    print("Found {} IMD products".format(len(result)))

    exit(0)
    for product in result:
        print("{} -> {}".format(*product))
