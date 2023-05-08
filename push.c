#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

#define MAX_COMMIT_MESSAGE_LENGTH 512
#define MAX_INPUT_LENGTH 256
#define MAX_COMMAND_LENGTH 1024 // Increased buffer size for git commit command

int fileExists(const char *filename)
{
    return access(filename, F_OK) != -1;
}

int main()
{
    int fileCount = 0;
    char files[MAX_FILES][MAX_FILENAME_LENGTH];

    while (1)
    {
        char filename[MAX_INPUT_LENGTH];
        printf("Enter the file name: ");
        fgets(filename, MAX_INPUT_LENGTH, stdin);

        // Remove trailing newline character
        filename[strcspn(filename, "\n")] = 0;

        if (!fileExists(filename))
        {
            printf("File '%s' does not exist. Please enter a valid file name.\n", filename);
            continue;
        }

        if (fileCount >= MAX_FILES)
        {
            printf("Maximum number of files reached.\n");
            break;
        }

        // Add the file to the list
        strcpy(files[fileCount], filename);
        fileCount++;

        printf("File added successfully.\n");

        // Prompt to continue adding more files
        char choice;
        printf("Do you want to add more files? (y/n): ");
        scanf(" %c", &choice);
        getchar(); // Consume the newline character from previous input

        if (choice != 'y' && choice != 'Y')
            break;
    }

    // Commit message input
    char commitMessage[MAX_COMMIT_MESSAGE_LENGTH];
    printf("Enter the commit message: ");
    fgets(commitMessage, MAX_COMMIT_MESSAGE_LENGTH, stdin);
    commitMessage[strcspn(commitMessage, "\n")] = 0; // Remove trailing newline

    // Add files to Git
    for (int i = 0; i < fileCount; i++)
    {
        char gitAddCommand[MAX_COMMAND_LENGTH];
        snprintf(gitAddCommand, sizeof(gitAddCommand), "git add %s", files[i]);
        system(gitAddCommand);
    }

    // Commit the changes with the custom commit message
    char gitCommitCommand[MAX_COMMAND_LENGTH];
    snprintf(gitCommitCommand, sizeof(gitCommitCommand), "git commit -m \"%s\"", commitMessage);
    system(gitCommitCommand);

    // Push the changes to Git
    system("git push");
    printf("Files updated succesfully");

    return 0;
}

