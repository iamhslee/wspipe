// 2024-1 Operating Systems (ITP30002) - HW #2
// File  : main.c
// Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>
//
// 'grep' like word search program using fork() and pipe()

/* MACROS */
// ANSI escape codes for terminal text color
#define TERM_DEFAULT "\033[0m"
#define TERM_RED_BOLD "\033[1;31m"
#define TERM_GREEN_BOLD "\033[1;32m"

// Buffer size
#define BUFFER_SIZE 819200

/* HEADERS */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* TYPE DEFINITIONS */
typedef struct {
    char* command;  // Command to execute
    char** paramv;  // Command arguments
    int paramc;     // Number of arguments
    char* keyword;  // Keyword to search
} input_t;

typedef struct {
    char** lines;    // Output lines
    int linesCount;  // Number of output lines
} output_t;

/* FUNCTION PROTOTYPES */
void help(char* errMsg);                               // Print help message
input_t argparse(int argc, char* argv[]);              // Parse command line arguments
output_t execute(input_t input);                       // Execute command and get output
void print(output_t output, char* keyword);            // Print output with keyword marked
int* mark(char* line, char* keyword, int lineLength);  // Create keyword index map

/* MAIN FUNCTION */
int main(int argc, char* argv[]) {
    input_t input = argparse(argc, argv);  // Parse command line arguments
#ifdef DEBUG                               // DEBUG: Parser test
    printf(TERM_GREEN_BOLD "[DEBUG]" TERM_DEFAULT " Parsed arguments:\n");
    printf("Command: %s\n", input.command);
    for (int i = 0; i < input.paramc; i++) {
        printf("Param %d: %s\n", i, input.paramv[i]);
    }
    printf("Keyword: %s\n\n", input.keyword);
#endif
    output_t output = execute(input);  // Execute command
#ifdef DEBUG                           // DEBUG: Executor test
    printf(TERM_GREEN_BOLD "[DEBUG]" TERM_DEFAULT " Execution result:\n");
    for (int i = 0; i < output.linesCount; i++) {
        printf("Line %d: %s\n", i, output.lines[i]);
    }
    printf("\n");
#endif
    print(output, input.keyword);  // Print output with keyword marked

    return 0;
}

/* FUNCTION DEFINITIONS */
void help(char* errMsg) {  // Print help message
    if (errMsg) {          // Print error message if errMsg is not NULL
        printf(TERM_RED_BOLD "Error:" TERM_DEFAULT " %s\n", errMsg);
    }

    printf("Usage:\t./wspipe [Command] [Keyword]\n");
    printf("\t- Command\t: Command to execute with arguments  (wrap with single (or double) quotes if contains whitespace or any escape character)\n");
    printf("\t- Keyword\t: Keyword to search  (wrap with single (or double) quotes if contains whitespace or any escape character)\n");

    if (errMsg) {  // Exit with error code 1 if error message is given
        exit(1);
    } else {
        exit(0);
    }
}

input_t argparse(int argc, char* argv[]) {
    // Argument count check
    if (argc < 3) {
        // If only one argument is given and it is '-h', print help message
        if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
            help(NULL);
        }
        help("Insufficient arguments.");
    }
    if (argc > 3) {
        help("Too many arguments.");
    }

    // Parse command and arguments
    int paramCount = 0;                            // argc for command arguments
    char** paramList = malloc(sizeof(char*) * 3);  // argv for command arguments (0: command, 1: argument 1, 2: NULL)
    char* token = argv[1];
    while (token != NULL) {  // Tokenize command arguments
        paramList[paramCount++] = token;
        char* nextToken = token;
        while (*nextToken != '\0' && *nextToken != ' ') {  // Find next token
            nextToken++;
        }
        if (*nextToken == ' ') {
            *nextToken = '\0';
            token = nextToken + 1;
        } else {
            token = NULL;
        }
    }

    return (input_t){paramList[0], paramList, paramCount, argv[2]};  // Return parsed arguments
}

output_t execute(input_t input) {
    pid_t pid;  // Process ID
    int fd[2];  // File descriptor for pipe

    int linesCount = 0;                                      // Number of output lines
    char** linesList = malloc(sizeof(char*) * BUFFER_SIZE);  // List of output lines

    pipe(fd);      // Create pipe
    pid = fork();  // Fork process

    if (pid == 0) {      // Child process
        close(fd[0]);    // Close read end of the pipe
        dup2(fd[1], 1);  // Redirect stdout to write end of the pipe
        close(fd[1]);    // Close write end of the pipe

        execvp(input.command, input.paramv);  // Execute command
    } else {                                  // Parent process
        close(fd[1]);                         // Close write end of the pipe
        wait(NULL);                           // Wait for child process to finish

        char buffer[BUFFER_SIZE];                              // Buffer to read output
        int bytesRead = read(fd[0], buffer, BUFFER_SIZE - 1);  // Read output from pipe
        buffer[bytesRead] = '\0';                              // Null-terminate buffer

        close(fd[0]);  // Close read end of the pipe

        // Tokenize output lines by '\n'
        char* token = buffer;
        while (token != NULL) {
            linesList[linesCount++] = token;
            char* nextToken = token;
            while (*nextToken != '\0' && *nextToken != '\n') {
                nextToken++;
            }
            if (*nextToken == '\n') {
                *nextToken = '\0';
                token = nextToken + 1;
            } else {
                token = NULL;
            }
        }
    }
    return (output_t){linesList, linesCount};  // Return output lines
}

void print(output_t output, char* keyword) {
    for (int i = 0; i < output.linesCount; i++) {  // Iterate over output lines
        int lineLength = 0;                        // Get line length
        while (output.lines[i][lineLength] != '\0') {
            lineLength++;
        }

        int* markedPosMap = mark(output.lines[i], keyword, lineLength);  // Get highlighted keyword index map
        if (markedPosMap == NULL) continue;                              // Skip if keyword is not found

#ifdef DEBUG  // DEBUG: Marking test
        printf(TERM_GREEN_BOLD "[DEBUG]" TERM_DEFAULT " lineIdx: %d\n", i);
        printf(TERM_GREEN_BOLD "[DEBUG]" TERM_DEFAULT " Content: %s\n", output.lines[i]);
        printf(TERM_GREEN_BOLD "[DEBUG]" TERM_DEFAULT " Marked:  ");
        for (int j = 0; j < lineLength; j++) {
            printf("%d", markedPosMap[j]);
        }
        printf("\n");
#endif

        printf(TERM_GREEN_BOLD "[%d]\t" TERM_DEFAULT, i + 1);
        for (int j = 0; j < lineLength; j++) {
            if (markedPosMap[j]) {
                printf(TERM_RED_BOLD "%c" TERM_DEFAULT, output.lines[i][j]);
            } else {
                printf("%c", output.lines[i][j]);
            }
        }
        printf("\n");
#ifdef DEBUG
        printf("\n");
#endif
    }
}

int* mark(char* line, char* keyword, int lineLength) {
    int* markedPosMap = malloc(sizeof(int) * lineLength);  // Index map cannot initialize in compile time (Line length is different for each line)
    for (int i = 0; i < lineLength; i++) {                 // So, allocate memory and initialize with 0 in runtime.
        markedPosMap[i] = 0;
    }

    int keywordLength = 0;  // Get keyword length
    while (keyword[keywordLength] != '\0') {
        keywordLength++;
    }

    int hasMatch = 0;  // Flag for keyword match

    for (int i = 0; i < lineLength; i++) {
        if (line[i] == keyword[0]) {  // Check if first character of keyword is found
            int match = 1;            // Assume match
            for (int j = 1; j < keywordLength; j++) {
                if (line[i + j] != keyword[j]) {  // Check if characters after first character are matched
                    match = 0;                    // If character does not match after first character, break
                    break;
                }
            }
            if (match) {                                   // If keyword is found, mark the position
                hasMatch = 1;                              // Set flag for keyword match
                for (int j = 0; j < keywordLength; j++) {  // Mark keyword position
                    markedPosMap[i + j] = 1;
                }
            }
        }
    }

    if (hasMatch) {  // Return marked position map if keyword match flag is set
        return markedPosMap;
    } else {
        return NULL;
    }
}

/* END OF FILE */