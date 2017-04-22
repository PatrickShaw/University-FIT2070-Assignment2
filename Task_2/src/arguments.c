/**
    @author Patrick Shaw (Patrick.Leong.Shaw@gmail.com)
*/
#include "arguments.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
/**
 * Splits input from a buffer into command arguments.<br/>
 * E.g.<br/>
 *     cp "hello world.c"hello world.cpp<br/>
 *     becomes:<br/>
 *         cp<br/>
 *         "hello world.c"<br/>
 *         helloworld.cpp<br/>
 * Note that quotations can be used to include spaces in a command
 * @param commandArgs
 * Place where the command args will be put inside.
 * @param buffer
 * Input from the user
 * @param characterCount
 * Number of input characters that are in the buffer
 * @return
*/
int getCommandArgs(int startingIndex, char** commandArgs, int commandArgsSize, char* buffer, int bufferSize) {
    int commandArgC = startingIndex;
    int quotationCount = 0;
    int hasQuotes = 0;
    // First, do we have quotes in the first place? If we don't then forget about passing arguments with quotes
    for(int i = 0; i < bufferSize; i++) {
        if(buffer[i] == '\"') {
            hasQuotes = 1;
            break;
        }
    }
    for(int i = 0; i < bufferSize; i++) {
        if(commandArgC >= commandArgsSize) {
            return -1;
        }
        //printf("Checking for %d: %c\n", commandArgC, buffer[i]);
        int currentArgLen = strlen(commandArgs[commandArgC]);
        if(buffer[i] == '\"') {
            commandArgs[commandArgC][currentArgLen] = buffer[i];
            commandArgs[commandArgC][currentArgLen + 1] = '\0';
            quotationCount++;
            if(quotationCount % 2 == 0) {
                // If it's a closing quote then we're done with this command arg
                commandArgC++;
            }
            continue;
        } else if (buffer[i] == '\0' || buffer[i] == '\n') {
            //printf("Ended with: %s\n", commandArgs[commandArgC]);
            //printf("Command: %s\n", commandArgs[0]);
            if(commandArgs[commandArgC][0] != '\0') {
                commandArgC++;
                //printf("Not empty, increasing commandArgC\n");
            }
            break;
        } else if(isspace(buffer[i]) && quotationCount % 2 == 0) {
            // White space & outside of a quotation marked command. But have we actually done anything with the current commandArg?
            if(strcmp(commandArgs[commandArgC], "") != 0) {
                // Yes, we did do something with the current commandArg, time to start a new one.
                commandArgs[commandArgC][currentArgLen] = '\0';
                //printf("Resultant arg: %s\n", commandArgs[commandArgC]);
                commandArgC++;
            }
        } else {
            // Wasn't a command arg terminating char. Add the char to the command arg.
            commandArgs[commandArgC][currentArgLen] = buffer[i];
            commandArgs[commandArgC][currentArgLen + 1] = '\0';
        }
    }
    return commandArgC;
}
