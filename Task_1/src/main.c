/**
 * The program prints out all permutations of an inputed string synchronously.
 * @author Patrick Leong Shaw
 * @studentid 26898187
 * @datecreated 12/9/2016
 * @datemodified 16/9/2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define INPUT_BUFFER_SIZE 64 // Small buffer size since there since we are dealing with n! permutations
/**
 * Performs the factorial operation on an integer
 * @param n
 * The integer associated with the factorial
 * @return
 * n!
 */
long long factorial(long long n) {
    //printf("n: %d\n", n);
    for(int i = n - 1; i > 1; i--) {
        n *= i;
        //printf("n: %d\n", n);
    }
    return n;
}
/**
 * Prints a permutation for a given index using the factorial number system.
 * @param buffer
 * Actual input in the form of a buffer
 * @param inputSize
 * Size of the input within the buffer
 * @param permutationIndex
 * The index of the generated permutation
 * E.g. For a permuted string "abc"
 *      0 will generate "abc"
 *      1 will generate "cba"
 */
void permute(char* buffer, int inputSize, int permutationIndex) {
    // Performing the factorial base conversion
    int index_to_add_to_permutations_array[inputSize];
    int debugPermutationIndex = permutationIndex;
    for(int i = 1; i <= inputSize; i++) {
        int index = permutationIndex % i;
        index_to_add_to_permutations_array[inputSize - i] = index;
        permutationIndex/= i;
    }
    char permutationBuffer[inputSize];
    char permutation[inputSize + 1];
    strcpy(permutationBuffer, buffer);
    // Now use the factorial digit to retrieve each index of the permuted string
    for(int i = 0; i < inputSize; i++) {
        int consumedIndex = index_to_add_to_permutations_array[i];
        permutation[i] = permutationBuffer[consumedIndex];
        // Shuffle elements down, analogous to deleting elements in this case
        for(int j = consumedIndex; j < inputSize - 1; j++) {
            permutationBuffer[j] = permutationBuffer[j + 1];
        }
    }
    permutation[inputSize] = '\0';
    //printf("Perm index %d %s\n", debugPermutationIndex, permutation); 
    printf("%s\n", permutation);
}
int main() {
    char buffer[INPUT_BUFFER_SIZE];
    // Fill the buffer with input
    fgets(buffer, INPUT_BUFFER_SIZE, stdin);
    // Find the new line and make it a null terminator
    int inputSize = 0;
    for(int b = 0; b < INPUT_BUFFER_SIZE; b++) {
        if(buffer[b] == '\n') {
            buffer[b] = '\0';
            inputSize = b;
            break;
        }
    }
    // The number of permutations = n! where n is the number of characters in the permuted string
    int totalPermutations = factorial(inputSize);
    // Print all permutations synchronously
    for(int i = 0; i < totalPermutations; i++) {
        permute(buffer, inputSize, i);
    }
    return 0;
}
