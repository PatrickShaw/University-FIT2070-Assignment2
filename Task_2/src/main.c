/**
 * A program that utelises the permutative properties of the factorial number system to
 * generate and print out all permutations of a set of strings inputed by the user.
 * Note that every permutation is generated asynchronously with the use of a thread pool. Note that a
 * thread pool was implemented to avoid the overhead associated with using the fork(...) functionality.
 * All threads work on the same string before moving onto the second string and so forth.
 * @author Patrick Leong Shaw
 * @studentid 26898187
 * @datecreated 12/9/2016
 * @datemodified 16/9/2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include "arguments.h"
#define INPUT_BUFFER_SIZE 2048
#define MAX_COMMAND_ARGS 1024
/**
 * The mutex lock variable, prevents race conditions associated with multiple threads trying to increase and read the
 * current permutation index at the same time.
 */
pthread_mutex_t currentPermutationMutex;
/**
 * Struct used to store the arguments of each thread.
 */
struct permute_thread_args {
    char** ppBuffers; // The buffer of each permuted string
    int* pInputSizes; // The size of each permuted string
    long long* pTotalPermutationsPerString; // The total number of permutations for each string
    long long* pCurrentPermutation; // The next permutation that needs to be computed for the current string
    int totalStrings; // The number of permuted strings
    int* pCurrentString; // The index of the string that is currently being processed
};
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
    //printf("%s\n", buffer);
    // Now use the factorial digit to retrieve each index of the permuted string
    for(int i = 0; i < inputSize; i++) {
        int consumedIndex = index_to_add_to_permutations_array[i];
        //printf("%s\n",permutationBuffer);
        permutation[i] = permutationBuffer[consumedIndex];
        // Shuffle elements down the array (equivalent to deleting an element in this case)
        for(int j = consumedIndex; j < inputSize - 1; j++) {
            permutationBuffer[j] = permutationBuffer[j + 1];
        }
    }
    permutation[inputSize] = '\0';
    //printf("Perm index %d %s\n", debugPermutationIndex, permutation); 
    printf("%s\n", permutation);
}
/**
 * A method that is to be invoked by threads. The method will continue to run until all permutations have been found for all
 * strings
 * @param bufferArg
 * Input itself in the form of a buffer
 * @param inputSizeArg
 * Size of the input
 * @param totalPermutationsArg
 * Total number of permutations to be generated (should be the factorial of the input size)
 * @param pCurrentPermutationArg
 * Current permutations
 */
void* permuteWithThread(
    void* args
) {
    // Unpack some of the thread args struct; we will need them for later
    struct permute_thread_args* pThreadArgs = (struct permute_thread_args*)args;
    char** ppBuffers = pThreadArgs->ppBuffers;
    int totalStrings = pThreadArgs->totalStrings;
    long long* pTotalPermutationsPerString = pThreadArgs->pTotalPermutationsPerString;
    long long currentPermutation = *(pThreadArgs->pCurrentPermutation);
    int currentString = *(pThreadArgs->pCurrentString);
    int* pInputSizes = pThreadArgs->pInputSizes;
    // Keep going until we the threads end up at the last permutation of the last string
    while(*(pThreadArgs->pCurrentPermutation) < pTotalPermutationsPerString[currentString] && *(pThreadArgs->pCurrentString) < totalStrings) {
        // Need to lock it so that no one is going to take the same permutation is this thread
        pthread_mutex_lock(&currentPermutationMutex);
        currentPermutation = *(pThreadArgs->pCurrentPermutation); // This thread will take the current permutation
        currentString = *(pThreadArgs->pCurrentString);
        // Make sure that the next thread is going to have a valid string and permutation to work with
        if(currentPermutation + 1 >= pTotalPermutationsPerString[currentString]) {
            (*(pThreadArgs->pCurrentPermutation)) = 0; // Reset the current permutation
            (*(pThreadArgs->pCurrentString))++; // Move onto a new string
        } else {
            (*(pThreadArgs->pCurrentPermutation))++; // Move onto the next permutation
        }
        // Okay, the rest of the threads can go take a permutation now that we've done figuring out which index we're taking
        pthread_mutex_unlock(&currentPermutationMutex);
        // Now let's finally calculate the permutation
        permute(ppBuffers[currentString], pInputSizes[currentString], currentPermutation);
    }
    // At this point every single permutation has been generated for all strings, we can now close the thread
    pthread_exit(NULL);
    return NULL;
}
/**
 * Prints all permutations for a given number of strings in no particular order (although ideally tries to print from the
 * 1st permutation to the nth permutation, the 1st string to the jth string)
 * @param buffer
 * The permuted string in the form of a buffer
 * @param inputSize
 * The size of the input within the buffer
 * @param threadCount
 * The number of threads permuted string
 */
void permuteAllStringsThreadedUnordered(int startingCommandIndex, char** ppCommandArgs, int commandArgSize, int threadCount) {
    // Prepare for packaging int the thread args struct
    long long currentPermutation = 0; 
    long long* pCurrentPermutation = &currentPermutation;
    int currentString = startingCommandIndex;
    int* pCurrentString = &currentString;
    long long totalPermutations[commandArgSize];
    int inputSizes[commandArgSize];
    // Initialise the input size and total permutations for each string
    for(int c = 0; c < commandArgSize; c++) {
        int inputSize = strlen(ppCommandArgs[c]);
        totalPermutations[c] = factorial(inputSize);
        inputSizes[c] = inputSize;
    }
    pthread_t threads[threadCount];
    // Pack all the information we need into a single struct to send off to each thread
    struct permute_thread_args thread_args = {ppCommandArgs, inputSizes, totalPermutations, pCurrentPermutation, commandArgSize	, pCurrentString};
    // Start firing off threads (as many as the user specified)
    for (int i = 0; i < threadCount; i++) {
        //printf("Created thread %d\n", i);
        if (pthread_create(&threads[i], NULL, &permuteWithThread, (void*)&thread_args)) {
            fprintf(stderr, "Could not create thread.");
        }
    }
    // Wait till all threads are finished before we go out of this method
    for(int i =0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
}
int main() {
    char buffer[INPUT_BUFFER_SIZE];
    int argumentsCount = 0;
    // Allocate memory for the arguments
    char** commandArgs = malloc(MAX_COMMAND_ARGS * sizeof(char *));
    for(int i = 0; i < MAX_COMMAND_ARGS; i++) {
        commandArgs[i] = malloc(INPUT_BUFFER_SIZE * sizeof(char));
    }
    // Wait until we have at least 1 string to permute (argC > 1)
    //printf("%s\n",buffer);
    while(argumentsCount <= 1) {
        //printf("Argument count: %d\nBuffer: %s\n", argumentsCount, buffer);
        fgets(buffer, INPUT_BUFFER_SIZE, stdin);
        //printf("Buffer after fgets(...): %s\n", buffer);
        argumentsCount = getCommandArgs(argumentsCount, commandArgs, MAX_COMMAND_ARGS, buffer, INPUT_BUFFER_SIZE);
        if(argumentsCount == -1) {
            // -1 Means we never found a new line, nor a null terminator
            fprintf(stderr, "Buffer overflow\n");
            return -1;
        }
        //printf("%d arguments\n", argumentsCount);
    }
    //printf("Command arg 0: %s\n", commandArgs[0]);
    //printf("Finished retrieving arguments: Arguments count: %d\n", argumentsCount);
    // Try convert the first command to an integer
    int threadCount = atoi(commandArgs[0]);
    if(threadCount <= 0) {
        fprintf(stderr, "Error: The thread count cannot be 0 or less\n");
        return 1;
    }
    // Start from the second argument because the first one is the thread count
    permuteAllStringsThreadedUnordered(1, commandArgs, argumentsCount, threadCount);
    return 0;
}
