#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include "main.h"

#define MAX_COMMIT_MESSAGE_LENGTH 512
#define MAX_INPUT_LENGTH 256
#define MAX_COMMAND_LENGTH 1024

int fileExists(const char *filename)
{
    return access(filename, F_OK) != -1;
}

char **autocompleteFilename(const char *text, int start, int end)
{
    char **matches = NULL;
    int count = 0;

    rl_attempted_completion_over = 1;

    if (start == 0)
    {
        // Generate file name suggestions based on the current directory
        DIR *dir;
        struct dirent *entry;

        dir = opendir(".");
        if (dir != NULL)
        {
            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_REG)
                {
                    if (strncmp(entry->d_name, text, end - start) == 0)
                    {
                        matches = (char **)realloc(matches, sizeof(char *) * (count + 2));
                        matches[count] = strdup(entry->d_name);
                        matches[count + 1] = NULL;
                        count++;
                    }
                }
            }

            closedir(dir);
        }
    }

    return matches;
}

int main()
{
    int fileCount = 0;
    char files[MAX_FILES][MAX_FILENAME_LENGTH];

    rl_attempted_completion_function = autocompleteFilename;

    while (1)
    {
        char *filename = readline("Enter the file name: ");
        add_history(filename);

        if (!fileExists(filename))
        {
            printf("File '%s' does not exist. Please enter a valid file name.\n", filename);
            free(filename);
            continue;
        }

        if (fileCount >= MAX_FILES)
        {
            printf("Maximum number of files reached.\n");
            free(filename);
            break;
        }

        // Add the file to the list
        strncpy(files[fileCount], filename, MAX_FILENAME_LENGTH - 1);
        files[fileCount][MAX_FILENAME_LENGTH - 1] = '\0';
        fileCount++;

        printf("File added successfully.\n");
        free(filename);

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
        snprintf(gitAddCommand, sizeof(gitAddCommand), "git add %s > /dev/null 2>&1", files[i]);
        system(gitAddCommand);
    }

        // Commit the changes with the custom commit message
    char gitCommitCommand[MAX_COMMAND_LENGTH];
    snprintf(gitCommitCommand, sizeof(gitCommitCommand), "git commit -m \"%s\" > /dev/null 2>&1", commitMessage);
    system(gitCommitCommand);

    // Push the changes to Git
    system("git push > /dev/null 2>&1");

    printf("Git operations completed successfully.\n");

    return 0;
}


