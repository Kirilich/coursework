#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

#define AUTHOR "***** ******"
#define OPTION "4.19"

void print_author_info() {
    printf("Course work for option %s, created by %s.\n", OPTION, AUTHOR);
}

void print_help() {
    printf("Commands:\n");
    printf("0 - Print the original text\n");
    printf("1 - Remove characters around '$'\n");
    printf("2 - Sort sentences by word count (descending)\n");
    printf("3 - Count and print character frequencies\n");
    printf("4 - Remove sentences with even indexes\n");
    printf("5 - Show this help\n");
}

int main() {
    char *text = NULL;
    size_t text_size = 0;
    print_author_info();
    printf("Enter a command number (0-5): ");
    int command;
    if (scanf("%d", &command) != 1 || command < 0 || command > 5) {
        printf("Error: Invalid command\n");
        return 1;
    }
    if (command >= 0 && command <= 4) {
        printf("Enter the text (end with two newlines):\n");
        getchar();
        text = read_text(&text_size);
        if (!text || text_size == 0) {
            printf("Error: No text provided\n");
            free(text);
            return 1;
        }
    }
    switch (command) {
        case 0:
            printf(text);
            break;
        case 1:
            if (!process_dollar_symbols(text)) {
                printf("Error: No '$' found in the text\n");
            } else {
                printf(text);
            }
            break;
        case 2:
            sort_sentences_by_word_count(text);
            printf(text);
            break;
        case 3:
            print_character_frequencies(text);
            break;
        case 4:
            remove_even_index_sentences(&text);
            printf(text);
            break;
        case 5:
            print_help();
            break;
        default:
            printf("Error: Unknown command\n");
    }
    free(text);
    return 0;
}
