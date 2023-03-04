#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 50

// Структура для представления узла связного списка
typedef struct node {
    char word[MAX_WORD_LENGTH];
    int count;
    struct node *next;
} Node;

// Проверяем, является ли символ символом слова
int is_word_character(char c) {
    return isalnum(c) || c == '\'';
}

// Добавляем слово в связный список
void add_word(Node **queue, char *word) {
    Node *current = *queue;
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            return;
        }
        current = current->next;
    }
    Node *new_node = (Node *) malloc(sizeof(Node));
    strncpy(new_node->word, word, MAX_WORD_LENGTH);
    new_node->count = 1;
    new_node->next = NULL;
    if (*queue == NULL) {
        *queue = new_node;
    } else {
        Node *last = *queue;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_node;
    }
}

// Печатаем содержимое связного списка
void print_queue(Node *queue) {
    while (queue != NULL) {
        printf("%s: %d\n", queue->word, queue->count);
        queue = queue->next;
    }
}

// Находим самое популярное длинное слово, длина которого превышает min_length
Node *find_most_popular_long_word(Node *queue, int min_length) {
    Node *current = queue;
    Node *most_popular = NULL;
    while (current != NULL) {
        if (strlen(current->word) > min_length && (most_popular == NULL || current->count > most_popular->count)) {
            most_popular = current;
        }
        current = current->next;
    }
    return most_popular;
}

// Находим самое непопулярное короткое слово, длина которого меньше max_length
Node *find_least_popular_short_word(Node *queue, int max_length) {
    Node *current = queue;
    Node *least_popular = NULL;
    while (current != NULL) {
        if (strlen(current->word) < max_length && (least_popular == NULL || current->count < least_popular->count)) {
            least_popular = current;
        }
        current = current->next;
    }
    return least_popular;
}

// Меняем найденные слова во входном файле и записываем результат в выходной файл
//queue - указатель на первый элемент очереди слов
void compress(Node *queue, int min_length, int max_length, char *input_filename, char *output_filename) {
    Node *most_popular_long_word = find_most_popular_long_word(queue, min_length);
    Node *least_popular_short_word = find_least_popular_short_word(queue, max_length);

    if (most_popular_long_word == NULL || least_popular_short_word == NULL) {
        printf("Could not find suitable words to swap.\n");
        return;
    }

    FILE *input_fp = fopen(input_filename, "r");
    if (input_fp == NULL) {
        printf("Error opening input file.\n");
        return;
    }

    FILE *output_fp = fopen(output_filename, "w");
    if (output_fp == NULL) {
        printf("Error opening output file.\n");
        fclose(input_fp);
        return;
    }

    char buffer[MAX_WORD_LENGTH + 1];
    //читаем файл строка за строкой и разбиваем строки на отдельные слова с помощью функции strtok.
    while (fgets(buffer, MAX_WORD_LENGTH + 1, input_fp) != NULL) {
        char *token = strtok(buffer, " \t\n\r");
        while (token != NULL) {
            //Если текущее слово равно наиболее популярному длинному слову, оно заменяется на наименее популярное короткое слово
            if (strcmp(token, most_popular_long_word->word) == 0) {
                fprintf(output_fp, "%s ", least_popular_short_word->word);
                // Если текущее слово равно наименее популярному короткому слову, оно заменяется на наиболее популярное длинное слово.
            } else if (strcmp(token, least_popular_short_word->word) == 0) {
                fprintf(output_fp, "%s ", most_popular_long_word->word);
                //В противном случае, слово записывается в выходной файл без изменений.
            } else {
                fprintf(output_fp, "%s ", token);
            }
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(input_fp);
    fclose(output_fp);
    printf("Words swapped and saved to file '%s'.\n", output_filename);
}

//Тоже самое но наоборот
void decompress(Node *queue, int min_length, int max_length, char *input_filename, char *output_filename) {
    Node *most_popular_long_word = find_most_popular_long_word(queue, min_length);
    Node *least_popular_short_word = find_least_popular_short_word(queue, max_length);

    if (most_popular_long_word == NULL || least_popular_short_word == NULL) {
        printf("Could not find suitable words to swap.\n");
        return;
    }

    FILE *swapped_fp = fopen(input_filename, "r");
    if (swapped_fp == NULL) {
        printf("Error opening input file.\n");
        return;
    }

    FILE *output_fp = fopen(output_filename, "w");
    if (output_fp == NULL) {
        printf("Error opening output file.\n");
        fclose(swapped_fp);
        return;
    }

    char buffer[MAX_WORD_LENGTH + 1];
    while (fgets(buffer, MAX_WORD_LENGTH + 1, swapped_fp) != NULL) {
        char *token = strtok(buffer, " \t\n\r");
        while (token != NULL) {
            if (strcmp(token, least_popular_short_word->word) == 0) {
                fprintf(output_fp, "%s ", most_popular_long_word->word);
            } else if (strcmp(token, most_popular_long_word->word) == 0) {
                fprintf(output_fp, "%s ", least_popular_short_word->word);
            } else {
                fprintf(output_fp, "%s ", token);
            }
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(swapped_fp);
    fclose(output_fp);
    printf("Original file restored and saved to file '%s'.\n", output_filename);
}


int main(int argc, char *argv[]) {

    FILE *fp = fopen("C:\\Users\\Nikita\\CLionProjects\\proj\\file.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", argv[1]);
        return 1;
    }
    //Создаем массив символов buffer, размером MAX_WORD_LENGTH + 1, и указатель на структуру Node.
    char buffer[MAX_WORD_LENGTH + 1];
    Node *queue = NULL;

    //В цикле читаем слова из файла fp и помещаем их в buffer
    while (fscanf(fp, "%s", buffer) == 1) {
        int i;
        for (i = 0; i < strlen(buffer); i++) {
            //Приводим все символы слова к нижнему регистру.
            buffer[i] = tolower(buffer[i]);
        }
        int j = 0;
        //Ищем индекс первого символа, являющегося буквой в слове.
        while (j < strlen(buffer) && !is_word_character(buffer[j])) {
            j++;
        }
        int k = j;
        //Ищем индекс первого символа, не являющегося буквой в слове.
        while (k < strlen(buffer) && is_word_character(buffer[k])) {
            k++;
        }
        //Устанавливаем в buffer символ конца строки (\0) после последнего символа слова, чтобы слово было корректно записано в структуру Node.
        buffer[k] = '\0';
        //Добавляем слово в структуру Node, пропуская первые j символов (не являющиеся буквами).
        add_word(&queue, buffer + j);
    }
    print_queue(queue);

    //Ищем самое популярное длинное слово (с более чем 6 символами) в структуре Node.
    Node *most_popular_long_word = find_most_popular_long_word(queue, 6);
    //Если нашли, выводим его и количество его вхождений в структуру Node.
    if (most_popular_long_word != NULL) {
        printf("Most popular long word: %s (%d)\n", most_popular_long_word->word, most_popular_long_word->count);
    }
    //Ищем наименее популярное короткое слово (с меньше чем 4 символами) в структуре Node.
    Node *least_popular_short_word = find_least_popular_short_word(queue, 4);
    //Если нашли, выводим его и количество его вхождений в структуру Node.
    if (least_popular_short_word != NULL) {
        printf("Least popular short word: %s (%d)\n", least_popular_short_word->word, least_popular_short_word->count);
    }

    //Сжимаем файл file.txt, используя структуру Node, минимальную длину слова 6, максимальное количество слов в словаре 10, записываем результат в файл compress.txt.
    compress(queue, 6, 10, "C:\\Users\\Nikita\\CLionProjects\\proj\\file.txt",
             "C:\\Users\\Nikita\\CLionProjects\\proj\\compress.txt");

    //Распаковываем файл compress.txt, используя структуру Node, минимальную длину слова 6, максимальное количество слов в словаре 10, записываем результат в файл decompress.txt.
    decompress(queue, 6, 10, "C:\\Users\\Nikita\\CLionProjects\\proj\\compress.txt",
               "C:\\Users\\Nikita\\CLionProjects\\proj\\decompress.txt");

    fclose(fp);
    return 0;
}
