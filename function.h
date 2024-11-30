#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <stddef.h>

char* read_text(size_t *text_size);
int process_dollar_symbols(char *text);
void sort_sentences_by_word_count(char *text);
void print_character_frequencies(const char *text);
void remove_even_index_sentences(char **text);
void split_into_sentences(const char *text, char ***sentences, size_t *count);
void merge_sentences(char **sentences, size_t count, char *result);
int compare_sentence_word_count(const void *a, const void *b);
void remove_duplicate_sentences(char **text);

#endif
