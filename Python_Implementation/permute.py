"""
A program that asynchronously generates permutations of a given string using a specified number of threads in a thread
pool.
:author: Patrick Leong Shaw
:student_id: 26898187
:date_created: 12/9/2016
:date_modified: 16/9/2016
"""
from collections import deque

import multiprocessing


def int_input():
    """
    Gets an integer as input.
    Note that if invalid input is entered, the method will ask the user for input again.
    :return:
    Returns an integer inputted by the user.
    """
    while True:
        try:
            return int(input())
        except Exception:
            pass


def permutations(thread_pool, permuted_array, permuted_array_len):
    """
    C style permutations function
    Complexity is (n!)*(n^2). But benefits from being extremely asynchronous:
        - (n!)  -   because of the number of permutations that need to be filled
        - (n)   -   because of the number of elements in each of permutation
        - (n)   -   because of the worst case (E.g. remove the first index always) when
                    removing items from the cloned permuted_array
    :param permuted_array
    The array or string that's going to the permutations are going to be generated from
    :param permuted_array_len
    The length of the permuted_array (see permutations_array_len for explanation)
    """
    """
    Note that every single permutation can actually be generated asynchronously but probably unwise to use a thread for
    every individual permutation since thread creation is expensive & we probably don't have that many CPU cores to
    work with anyway.
    """
    permutations_array_len = factorial(permuted_array_len)
    thread_pool.starmap(
        get_permutation,
        [(permuted_array, permuted_array_len, i) for i in range(permutations_array_len)]
    )


def get_permutation(permuted_array, permuted_array_len, permutation_index):
    """
    Fills a single array with a particular permutation
    :param permuted_array:
    The array that is being permuted
    :param permuted_array_len:
    The length of the permuted_array
    :param permutation_index:
    The index of the permutation:
    E.g. For a permuted array 'abc'
    0 = 'abc'
    5 = 'cba'
    """
    new_permutation = ['\0'] * permuted_array_len
    permuted_array = list(permuted_array)
    consume_index_queue = deque()
    for i in range(1, permuted_array_len + 1):
        index = permutation_index % i
        consume_index_queue.appendleft(index)
        permutation_index = permutation_index // i

    for i in range(permuted_array_len):
        index_to_add_to_permutations_array = consume_index_queue.popleft()
        new_permutation[i] = permuted_array[index_to_add_to_permutations_array]
        del permuted_array[index_to_add_to_permutations_array]
    print("".join(new_permutation))


def factorial(n):
    for i in range(n - 1, 1, -1):
        n *= i
    return n


if __name__ == "__main__":
    # Turn the string into an array since strings are immutable (don't want to keep recopying the string)
    thread_number = int_input()
    permuted_array = [x for x in input()]
    permuted_array_len = len(permuted_array)

    thread_pool = multiprocessing.Pool(thread_number)  # 1 thread for each core on the CPU (usually)
    permutations(thread_pool, permuted_array, permuted_array_len)
