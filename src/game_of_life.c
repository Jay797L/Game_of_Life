#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define VerticalSize 25                 //Размер массива по вертикали
#define HorizontalSize 80               //Размер массива по горизонтали

int input(int **a, int choise);                                 //Чтение начальной конфигурации клеток из файла
void generate_mass(char **rendArr, int **countArr);             //Обновление состояния клеток на основе правил игры
void renderer(char **rendArr);                                  //Отображение текущего состояния игрового поля на экране
void count_rend(char **rendArr, int **countArr);                //Подсчет количества живых соседей для каждой клетки
void life_or_die(int **countArr, int i, int j);                 //Обновление счетчика живых соседей для данной клетки
void activate(char **rendArr, int **countArr, int *speed);      //Основной цикл игры, который обновляет и отображает состояние игрового поля
void nulification(int **countArr);                              //Обнуление счетчика живых соседей
void sped(int *play, int *speed);                               //Управление скоростью обновления и завершение игры

int main() {
    FILE *file;
    int speed;
    file = fopen("Speed.txt", "r");
    if (fscanf(file, "%d", &speed) != 1) {
        perror("Error reading number from file");
        speed = 10000;
    }
    fclose(file);
    if (speed > 100000) speed = 100000;
    if (speed < 10000) speed = 10000;
    char **renderArr = malloc(VerticalSize * sizeof(char *));
    char *render = malloc(VerticalSize * HorizontalSize * sizeof(char));
    for (int i = 0; i < VerticalSize; i++) {
        renderArr[i] = render + HorizontalSize * i;
    }
    int **counterArr = malloc(VerticalSize * sizeof(int *));
    int *counter = malloc(VerticalSize * HorizontalSize * sizeof(int));
    for (int i = 0; i < VerticalSize; i++) {
        counterArr[i] = counter + HorizontalSize * i;
    }
    printf("Введите файл ");
    int file;
    scanf("%d", &file);
    if (file < 5 && file >= 0) {
        input(counterArr, file);
        activate(renderArr, counterArr, &speed);
    } else
        printf("n/a");
    free(renderArr);
    free(render);
    free(counterArr);
    free(counter);
    file = fopen("Speed.txt", "w");
    if (fprintf(file, "%d", speed) < 0) {
        perror("Error writing to file");
    }
    fclose(file);
    return 0;
}

int input(int **a, int choise) {
    const char *filenames[] = {"input.txt", "aut.txt", "Gosper_glider_gun.txt", "LWSS.txt", "21.txt"};
    FILE *file = fopen(filenames[choise], "r");
    int count = 0;
    for (int m = 0; m < VerticalSize; m++) {
        for (int n = 0; n < HorizontalSize; n++) {
            if (fscanf(file, "%d", &a[m][n])) {
                count += 1;
            } else {
                count = 0;
                break;
            }
        }
    }
    fclose(file);
    return count;
}

void generate_mass(char **rendArr, int **countArr) {
    for (int i = 0; i < VerticalSize; i++) {
        for (int j = 0; j < HorizontalSize; j++) {
            if ((countArr[i][j] == 3 || countArr[i][j] == 2) && rendArr[i][j] != ' ') {
                rendArr[i][j] = '*';
            } else if (countArr[i][j] == 3 && rendArr[i][j] == ' ') {
                rendArr[i][j] = '*';
            } else
                rendArr[i][j] = ' ';
        }
    }
}

void renderer(char **rendArr) {
    printw(" ");
    for (int j = 0; j < HorizontalSize; j++) {
        printw("-");
    }
    printw("\n");
    for (int i = 0; i < VerticalSize; i++) {
        printw("|");
        for (int j = 0; j < HorizontalSize; j++) {
            printw("%c", rendArr[i][j]);
        }
        if (i < VerticalSize) {
            printw("|\n");
        }
    }
    printw(" ");
    for (int j = 0; j < HorizontalSize; j++) {
        printw("-");
    }
    printw("\n");
}

void count_rend(char **rendArr, int **countArr) {
    for (int i = 0; i < VerticalSize; i++) {
        for (int j = 0; j < HorizontalSize; j++) {
            if (rendArr[i][j] == '*') {
                life_or_die(countArr, i, j);
            }
        }
    }
}

void life_or_die(int **countArr, int i, int j) {
    int i_minus = i - 1, i_plus = i + 1, j_minus = j - 1, j_plus = j + 1;

    if (i_plus > VerticalSize - 1) i_plus = 0;
    if (j_plus > HorizontalSize - 1) j_plus = 0;

    if (i_minus < 0) i_minus = VerticalSize - 1;
    if (j_minus < 0) j_minus = HorizontalSize - 1;

    countArr[i_plus][j] += 1;
    countArr[i_plus][j_plus] += 1;
    countArr[i_plus][j_minus] += 1;
    countArr[i][j_plus] += 1;
    countArr[i][j_minus] += 1;
    countArr[i_minus][j] += 1;
    countArr[i_minus][j_plus] += 1;
    countArr[i_minus][j_minus] += 1;
}

void nulification(int **countArr) {
    for (int m = 0; m < VerticalSize; m++) {
        for (int n = 0; n < HorizontalSize; n++) {
            countArr[m][n] = 0;
        }
    }
}

void activate(char **rendArr, int **countArr, int* speed) {
    int play = 0;
    while (play != 1) {
        initscr();
        nodelay(stdscr, TRUE);
        // curs_set(0);
        generate_mass(rendArr, countArr);
        renderer(rendArr);
        nulification(countArr);
        count_rend(rendArr, countArr);
        sped(&play, speed);
        usleep(speed + 50000);
        clear();
        // keypad(stdscr, true);
        nodelay(stdscr, TRUE);
        endwin();
    }
}

void sped(int *play, int *speed) {
    char fast_slow;
    fast_slow = getch();
    if (fast_slow == '-' && *speed < 100000) *speed += 5000;
    if (fast_slow == '=' && *speed > 10000) *speed -= 5000;
    if (fast_slow == 'q') *play = 1;
}
