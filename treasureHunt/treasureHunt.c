#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 5
#define TREASURES 3
#define TRAPS 3

char grid[SIZE][SIZE];
char visibleGrid[SIZE][SIZE];
int playerX = 0, playerY = 0;
int treasuresCollected = 0;
int moveCount = 0;

void initializeGrid();
void displayGrid();
void placeTreasuresAndTraps();
void movePlayer(char direction);
void saveGame(const char *filename);
void loadGame(const char *filename);
void updateLeaderboard(const char *playerName);
void displayLeaderboard();

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "p") != 0) {
        printf("Usage: ./treasureHunt.exe p <player_name> [load <filename>] [leaders]\n");
        return 1;
    }

    char playerName[50];
    strcpy(playerName, argv[2]);
    srand(time(NULL));
    initializeGrid();
    placeTreasuresAndTraps();

    int showLeaderboard = 0;
    int loadGameFlag = 0;
    char loadFilename[50];


    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "leaders") == 0) {
            showLeaderboard = 1;
        }
        if (strcmp(argv[i], "load") == 0 && i + 1 < argc) {
            loadGameFlag = 1;
            strcpy(loadFilename, argv[i + 1]);
        }
    }


    if (showLeaderboard) {
        displayLeaderboard();
    }


    if (loadGameFlag) {
        loadGame(loadFilename);
    }

    char command;
    while (treasuresCollected < TREASURES) {
        displayGrid();
        printf("Enter move (u/d/l/r) or 's' to save: ");
        scanf(" %c", &command);
        if (command == 's') {
            char saveFile[50];
            printf("Enter save file name: ");
            scanf("%s", saveFile);
            saveGame(saveFile);
            continue;
        }
        movePlayer(command);
        moveCount++;
    }

    printf("Congratulations, %s! You collected all treasures in %d moves!\n", playerName, moveCount);
    updateLeaderboard(playerName);
    return 0;
}

void initializeGrid() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = ' ';
            visibleGrid[i][j] = '?';
        }
    }
    visibleGrid[0][0] = 'P';
}

void displayGrid() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", visibleGrid[i][j]);
        }
        printf("\n");
    }
    printf("Treasures collected: %d\n", treasuresCollected);
}

void placeTreasuresAndTraps() {
    int placed = 0;
    while (placed < TREASURES) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;
        if (grid[x][y] == ' ' && !(x == 0 && y == 0)) {
            grid[x][y] = 'T';
            placed++;
        }
    }
    placed = 0;
    while (placed < TRAPS) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;
        if (grid[x][y] == ' ' && !(x == 0 && y == 0)) {
            grid[x][y] = 'X';
            placed++;
        }
    }
}

void movePlayer(char direction) {
    int newX = playerX, newY = playerY;
    switch (direction) {
        case 'u': newX--; break;
        case 'd': newX++; break;
        case 'l': newY--; break;
        case 'r': newY++; break;
        default: printf("Invalid move!\n"); return;
    }

    if (newX < 0 || newX >= SIZE || newY < 0 || newY >= SIZE) {
        printf("Move out of bounds!\n");
        return;
    }


    if (visibleGrid[playerX][playerY] != 'T') {
        visibleGrid[playerX][playerY] = '=';
    }

    playerX = newX;
    playerY = newY;

    if (grid[playerX][playerY] == 'T') {
        treasuresCollected++;
        grid[playerX][playerY] = ' ';
        visibleGrid[playerX][playerY] = 'T';
    } else if (grid[playerX][playerY] == 'X') {
        printf("Game Over! You hit a trap.\n");
        exit(0);
    } else {
        visibleGrid[playerX][playerY] = 'P';
    }
}

void saveGame(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error saving game!\n");
        return;
    }
    fprintf(file, "%d %d %d %d\n", playerX, playerY, treasuresCollected, moveCount);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fprintf(file, "%c", grid[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    printf("Game saved!\n");
}

void loadGame(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error loading game!\n");
        return;
    }
    fscanf(file, "%d %d %d %d", &playerX, &playerY, &treasuresCollected, &moveCount);
    for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
        fscanf(file, " %c", &grid[i][j]);
    }
}
    fclose(file);
    printf("Game loaded!\n");
}

void updateLeaderboard(const char *playerName) {
    FILE *file = fopen("leaderboard.txt", "a");
    if (!file) {
        printf("Error updating leaderboard!\n");
        return;
    }
    fprintf(file, "%s %d\n", playerName, moveCount);
    fclose(file);
}

void displayLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("No leaderboard available.\n");
        return;
    }

    char names[100][50];
    int scores[100];
    int count = 0;

    while (fscanf(file, "%s %d", names[count], &scores[count]) != EOF) {
        count++;
    }
    fclose(file);


    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j] > scores[j + 1]) {
                int temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
                char tempName[50];
                strcpy(tempName, names[j]);
                strcpy(names[j], names[j + 1]);
                strcpy(names[j + 1], tempName);
            }
        }
    }

    printf("Leaderboard (Top Scores - Low to High):\n");
    for (int i = 0; i < count; i++) {
        printf("%s - %d moves\n", names[i], scores[i]);
    }
}
