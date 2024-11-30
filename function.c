#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "function.h"
#include <ctype.h>
#include <strings.h>

/**Разделяет текст на предложения по точке (.) и возвращает массив строк с этими предложениями
Параметры:
const char *text: Исходный текст.
char ***sentences: Указатель на массив указателей, куда будут записаны предложения.
size_t *count: Количество предложений.
Создается массив строк для хранения предложений.
Проходится по символам текста.
Когда встречается точка (.), выделяется новое предложение (с удалением начальных пробелов).
Если массив заполнен, его размер увеличивается с помощью realloc.
Предложения записываются в массив, а счетчик увеличивается.
Используемые функции:
malloc и realloc (из <stdlib.h>): Выделение и перераспределение памяти.
isspace (из <ctype.h>): Проверка, является ли символ пробельным.
strncpy и strlen (из <string.h>): Копирование строки и определение ее длины.
free: Освобождение памяти. */

void split_into_sentences(const char *text, char ***sentences, size_t *count) {
    char **result = NULL;
    size_t capacity = 10;
    size_t sentence_count = 0;
    result = (char **)malloc(capacity * sizeof(char *));
    if (!result) return;
    const char *start = text;
    while (*text) {
        if (*text == '.') {
            size_t length = text - start + 1;
            char *sentence = (char *)malloc((length + 1) * sizeof(char));
            if (sentence) {
                strncpy(sentence, start, length);
                sentence[length] = '\0';
                if (sentence_count >= capacity) {
                    capacity *= 2;
                    char **new_result = realloc(result, capacity * sizeof(char *));
                    if (!new_result) {
                        for (size_t i = 0; i < sentence_count; i++) {
                            free(result[i]);
                        }
                        free(result);
                        return;
                    }
                    result = new_result;
                }
                result[sentence_count++] = sentence;
            }
            start = text + 1;
        }
        text++;
    }
    *sentences = result;
    *count = sentence_count;
}

/**Объединяет массив предложений в одну строку, разделяя их символом новой строки (\n).
char **sentences: Массив строк с предложениями.
size_t count: Количество предложений.
char *result: Буфер, куда записывается итоговый текст.
Работа:
Итоговый текст обнуляется.
По очереди добавляются предложения с добавлением символа новой строки между ними.
Используемые функции:
strcat (из <string.h>): Конкатенация строк. */

void merge_sentences(char **sentences, size_t count, char *result) {
    result[0] = '\0';
    for (size_t i = 0; i < count; i++) {
        strcat(result, sentences[i]);
        if (i < count - 1) {
            strcat(result, "\n");
        }
    }
}

/**Считает количество слов в предложении.
const char *sentence: Строка-предложение.
Работа:
Проходится по символам строки.
Слово считается при встрече непробельного символа, за которым следует пробел или конец строки.
Используемые функции:
isspace (из <ctype.h>): Проверка пробельных символов. */

size_t count_words(const char *sentence) {
    size_t count = 0;
    int in_word = 0;
    while (*sentence) {
        if (isspace((unsigned char)*sentence) || *sentence == '.' || *sentence == ',') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        sentence++;
    }
    return count;
}

/**Сравнивает два предложения по количеству слов. Используется для сортировки.
const void *a и const void *b: Указатели на элементы массива строк.
Работа:
Вызывается функция count_words для подсчета слов в обоих предложениях.
Возвращается разность количества слов.
Используемые функции:
count_words: Подсчитывает слова в предложении. */

int compare_sentence_word_count(const void *a, const void *b) {
    const char *sentence_a = *(const char **)a;
    const char *sentence_b = *(const char **)b;
    size_t words_a = count_words(sentence_a);
    size_t words_b = count_words(sentence_b);
    return (int)(words_b - words_a);
}

/**Считывает текст из входного потока до двух подряд идущих символов новой строки.
size_t *text_size: Указатель для записи длины считанного текста.
Работа:
Выделяется начальный буфер, который увеличивается при необходимости.
Символы считываются до двух подряд идущих символов новой строки.
Дубликаты предложений удаляются с помощью remove_duplicate_sentences.
Используемые функции:
malloc и realloc: Управление памятью.
getchar (из <stdio.h>): Чтение символов из стандартного ввода.
memmove (из <string.h>): Перемещение блоков памяти. */

char* read_text(size_t *text_size) {
    char *text = NULL;
    size_t capacity = 1024;
    size_t length = 0;
    text = (char *)malloc(capacity);
    if (!text) {
        free(text);
        return NULL;
    }
    char c;
    int newline_count = 0;
    while ((c = getchar()) != EOF) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *new_text = realloc(text, capacity);
            if (!new_text) {
                free(text);
                return NULL;
            }
            text = new_text;
        }
        text[length++] = c;
        if (c == '\n') {
            newline_count++;
            if (newline_count == 2) break;
        } else {
            newline_count = 0;
        }
    }
    length-=2;
    text[length] = '\0';
    *text_size = length;
    remove_duplicate_sentences(&text);
    return text;
}

/**Удаляет символы $ из текста. Если их два, удаляет текст между ними.
char *text: Исходный текст.
Работа:
Ищет первое и последнее вхождение $.
Удаляет $ или текст между ними.
Используемые функции:
strchr и strrchr (из <string.h>): Поиск первого/последнего вхождения символа в строке.
memmove: Перемещение текста. */

int process_dollar_symbols(char *text) {
    char *start = strchr(text, '$');
    char *end = strrchr(text, '$');
    if (!start || !end) return 0;
    if (start == end) {
        memmove(start, start + 1, strlen(start));
    } else {
        memmove(text, start + 1, strlen(start + 1) + 1);
        end = strrchr(text, '$'); 
        if (end) {
            *end = '\0';
        }
    }
    return 1;
}

/**Сортирует предложения в тексте по количеству слов.
char *text: Исходный текст.
Работа:
Разделяет текст на предложения.
Сортирует предложения с помощью qsort.
Объединяет обратно в текст.
Используемые функции:
split_into_sentences: Разделение текста на предложения.
qsort (из <stdlib.h>): Быстрая сортировка массива.
merge_sentences: Объединение предложений. */

void sort_sentences_by_word_count(char *text) {
    char **sentences = NULL;
    size_t sentence_count = 0;
    split_into_sentences(text, &sentences, &sentence_count);
    qsort(sentences, sentence_count, sizeof(char*), compare_sentence_word_count);
    merge_sentences(sentences, sentence_count, text);
    for (size_t i = 0; i < sentence_count; i++) {
        free(sentences[i]);
    }
    free(sentences);
}

/**Выводит количество вхождений каждого символа в тексте.
const char *text: Исходный текст.
Работа:
Считает количество вхождений каждого символа в массив frequencies.
Выводит результаты для уникальных символов.
Используемые функции:
isspace: Игнорирует пробельные символы.
printf (из <stdio.h>): Вывод информации. */

void print_character_frequencies(const char *text) {
    int frequencies[256] = {0};
    for (const char *p = text; *p; p++) {
        if (!isspace(*p)) {
            frequencies[(unsigned char)*p]++;
        }
    }
    for (const char *p = text; *p; p++) {
        if (!isspace(*p) && frequencies[(unsigned char)*p]) {
            printf("%c: %d\n", *p, frequencies[(unsigned char)*p]);
            frequencies[(unsigned char)*p] = 0;
        }
    }
}

/**Удаляет предложения с четными индексами из текста.
char **text: Указатель на текст.
Работа:
Разделяет текст на предложения.
Удаляет предложения с четными индексами.
Объединяет оставшиеся обратно в текст.
Используемые функции:
split_into_sentences: Разделение текста на предложения. */

void remove_even_index_sentences(char **text) {
    char **sentences = NULL;
    size_t sentence_count = 0;
    split_into_sentences(*text, &sentences, &sentence_count);
    size_t new_text_size = 0;
        for (size_t i = 0; i < sentence_count; i++) {
        if (i % 2 != 0) {
            new_text_size += strlen(sentences[i]) + 1;
        } else {
            free(sentences[i]);
        }
    }
    char *new_text = (char *)malloc(new_text_size + 1);
    if (!new_text) {
        free(sentences);
        return;
    }

    size_t offset = 0;
    for (size_t i = 0; i < sentence_count; i++) {
        if (i % 2 != 0) {
            strcpy(new_text + offset, sentences[i]);
            offset += strlen(sentences[i]);
            new_text[offset++] = '\n';
        }
    }
    new_text[offset] = '\0';
    free(*text);
    *text = new_text;
    free(sentences);
}

/**Удаляет повторяющиеся предложения из текста.
char **text: Указатель на текст.
Работа:
Сравнивает предложения с использованием strcasecmp.
Удаляет дубликаты, объединяя оставшиеся.
Используемые функции:
split_into_sentences: Разделение текста.
strcasecmp: Сравнение строк без учета регистра. */

void remove_duplicate_sentences(char **text) {
    char **sentences = NULL;
    size_t sentence_count = 0;
    split_into_sentences(*text, &sentences, &sentence_count);
    int *to_keep = (int *)malloc(sentence_count * sizeof(int));
    if (!to_keep) {
        for (size_t i = 0; i < sentence_count; i++) {
            free(sentences[i]);
        }
        free(sentences);
        return;
    }
    for (size_t i = 0; i < sentence_count; i++) {
        to_keep[i] = 1;
    }
    for (size_t i = 0; i < sentence_count; i++) {
        if (!to_keep[i]) continue;
        for (size_t j = i + 1; j < sentence_count; j++) {
            if (!to_keep[j]) continue;
            if (strcasecmp(sentences[i], sentences[j]) == 0) {
                to_keep[j] = 0;
            }
        }
    }
    size_t new_text_size = 0;
    for (size_t i = 0; i < sentence_count; i++) {
        if (to_keep[i]) {
            new_text_size += strlen(sentences[i]) + 1;
        } else {
            free(sentences[i]);
        }
    }
    char *new_text = (char *)malloc(new_text_size + 1);
    if (!new_text) {
        free(to_keep);
        free(sentences);
        return;
    }
    size_t offset = 0;
    for (size_t i = 0; i < sentence_count; i++) {
        if (to_keep[i]) {
            strcpy(new_text + offset, sentences[i]);
            offset += strlen(sentences[i]);
            new_text[offset++] = '\n';
        }
    }
    new_text[offset] = '\0';
    free(*text);
    *text = new_text;
    free(to_keep);
    free(sentences);
}
