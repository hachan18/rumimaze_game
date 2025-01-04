#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 19  // 迷路の幅
#define HEIGHT 11 // 迷路の高さ

// 迷路データ
wchar_t maze[HEIGHT][WIDTH];
int playerX = 1, playerY = 1; // プレイヤーの初期位置
int score = 0;               // スコア
int steps = 0;               // ステップ数

// 迷路の初期データ
wchar_t initialMaze[HEIGHT][WIDTH] = {
    {L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█'},
    {L'█', L'・', L'・', L'・', L'█', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'█', L'・', L'・', L'・', L'█'},
    {L'█', L'・', L'█', L'・', L'█', L'・', L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'・', L'█', L'・', L'█', L'・', L'█'},
    {L'█', L'・', L'█', L'・', L'・', L'・', L'█', L'・', L'・', L'・', L'・', L'・', L'█', L'・', L'・', L'・', L'█', L'・', L'█'},
    {L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'・', L'█'},
    {L'█', L'・', L'・', L'・', L'█', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'⭐', L'█'},
    {L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'█', L'█'},
    {L'█', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'█'},
    {L'█', L'・', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'█', L'█', L'・', L'█', L'█', L'█', L'█', L'█', L'・', L'█'},
    {L'█', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'・', L'█'},
    {L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█'}
};

// 画面クリア
void clearScreen() {
    wprintf(L"\033[H\033[J");
    fflush(stdout);
}

// 迷路の表示
void displayMaze() {
    wprintf(L"====================\n");
    wprintf(L"  移動方法:\n");
    wprintf(L"  W: 上    S: 下\n");
    wprintf(L"  A: 左    D: 右\n");
    wprintf(L"====================\n\n");

    wprintf(L"スコア: %d\n", score);
    wprintf(L"ステップ数: %d\n", steps);
    wprintf(L"目標: ゴール (⭐) に到達！\n\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            wprintf(L"%lc", maze[i][j]);
        }
        wprintf(L"\n");
    }
}

// キーボード入力
char getInput() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// プレイヤーの移動
void movePlayer(char direction) {
    int newX = playerX, newY = playerY;

    switch (direction) {
        case 'w': newX--; break;
        case 's': newX++; break;
        case 'a': newY--; break;
        case 'd': newY++; break;
        default: return;
    }

    if (maze[newX][newY] != L'█') {
        // ドットを食べる処理
        if (maze[newX][newY] == L'・') {
            score += 10; // スコア加算
        }

        // ゴールに到達した場合
        if (maze[newX][newY] == L'⭐') {
            wprintf(L"\n🎉 ゴールに到達しました！ 🎉\n");
            wprintf(L"スコア: %d, ステップ数: %d\n", score, steps);
            fflush(stdout);
            sleep(3); // 3秒待機
            // 新しいゲームの初期化
            memcpy(maze, initialMaze, sizeof(initialMaze));
            playerX = 1;
            playerY = 1;
            steps = 0;
            score = 0;
            maze[playerX][playerY] = L'🙂';
            return;
        }

        // 移動処理
        maze[playerX][playerY] = L' '; // 元の位置を空白に
        playerX = newX;
        playerY = newY;
        maze[playerX][playerY] = L'🙂'; // 新しい位置にプレイヤーを配置
        steps++; // ステップ数を加算
    }
}

int main() {
    // ロケール設定
    setlocale(LC_ALL, "");

    // プレイヤーの初期位置を設定
    memcpy(maze, initialMaze, sizeof(initialMaze));
    maze[playerX][playerY] = L'🙂';

    char input;
    while (1) {
        clearScreen();
        displayMaze();
        input = getInput();
        movePlayer(input);
    }
    return 0;
}
