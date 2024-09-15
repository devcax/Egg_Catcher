#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <fstream>
#include <thread>
#include "bitmapItems.h"

using namespace std;
using namespace std::chrono;
using namespace bitmapItems;

namespace gameEngine {

    /**
     * Function declarations.
     */
    void HideConsoleCursor(),
    ClearScreen(),
    Setup(),
    Draw(),
    Input(),
    LoadFrames(),
    DisplayFrame(),
    LaunchScreen(),
    Help(),
    Credits(),
    HandleMenuSelection(),
    GameOver(bool),
    Worker(),
    DrawLoadingAnimation(),
    RunGame();

    /**
     * Game variables and constants.
     */
    vector<string> frames;
    const int width = 40;
    const int height = 20;
    int basketX = width / 2 - 4;
    int eggX, eggY;
    int birdX;
    int score = 0;
    int level = 1;
    int speed = 200;
    bool eggDropped = false;
    bool gameOver = false;
    bool won = false;

    /**
     * Hides the console cursor.
     */
    void HideConsoleCursor() {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(consoleHandle, &cursorInfo);
        cursorInfo.bVisible = false; // Set the cursor visibility to false
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }

    /**
     * Clears the console screen.
     */
    void ClearScreen() {
        system("cls");
    }

    /**
     * Sets up the initial game state.
     */
    void Setup() {
        ClearScreen();
        HideConsoleCursor();
        srand(time(0));
        birdX = rand() % (width - 1);
        eggX = birdX;
        eggY = 1;
        basketX = width / 2 - 4;
        score = 0;
        level = 1;
        speed = 200;
        eggDropped = false;
        gameOver = false;
        won = false;
    }

    /**
     * Draws the game elements on the console.
     */
    void Draw() {

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});

        string scoreContent = "Score: " + to_string(score);
        string levelContent = "Level: " + to_string(level);
        int boxWidth = width; // Align with the game canvas width

        cout << "\n╔" << string(boxWidth, '=') << "╗\n";

        cout << "║" << string(boxWidth, ' ') << "║\n";

        int colWidth = boxWidth / 2;
        int paddingScore = (colWidth - scoreContent.length()) / 2;
        int paddingLevel = (colWidth - levelContent.length()) / 2;

        cout << "║" << string(paddingScore, ' ') << scoreContent << string(colWidth - scoreContent.length() - paddingScore, ' ')
             << "║" << string(paddingLevel, ' ') << levelContent << string((colWidth - levelContent.length() - paddingLevel) - 1, ' ') << "║\n";

        cout << "║" << string(boxWidth, ' ') << "║\n";

        cout << "╚" << string(boxWidth, '=') << "╝\n";

        for (int i = 0; i < width + 2; i++)
            cout << "█";
        cout << endl;

        for (int h = 0; h < height; h++) {
            cout << "█";
            for (int w = 0; w < width; w++) {
                if (h == 0 && w == birdX) cout << "v";
                else if (h == 1 && w == birdX - 1) cout << "/";
                else if (h == 1 && w == birdX) cout << "o";
                else if (h == 1 && w == birdX + 1) cout << "\\";
                else if (h == eggY && w == eggX) cout << "O";
                else if (h == height - 1 && w >= basketX && w <= basketX + 8) {
                    if (w == basketX) cout << " ";
                    else if (w == basketX + 1) cout << "\\";
                    else if (w < basketX + 8) cout << "_";
                    else cout << "/";
                } else cout << " ";
            }
            cout << "█\n";
        }

        for (int i = 0; i < width + 2; i++)
            cout << "█";
        cout << endl;
    }

    /**
     * Handles user input to move the basket left or right.
     */
    void Input() {
        if (_kbhit()) {
            switch (_getch()) {
                case 'a':
                    basketX -= 3;
                    break;
                case 'd':
                    basketX += 3;
                    break;
            }
        }
    }

    /**
     * Loads the frames for the game over and game won screens.
     */
    void LoadFrames(const string& folderPath, int frameCount) {
        for (int i = 0; i < frameCount; ++i) {
            ifstream file(folderPath + "/" + to_string(i) + ".txt");
            if (file.is_open()) {
                string frame((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                frames.push_back(frame);
                file.close();
            }
        }
    }

    /**
     * Displays a specific frame from the loaded frames.
     */
    void DisplayFrame(int frameIndex, int topMargin) {
        if (frameIndex >= 0 && frameIndex < frames.size()) {
            ClearScreen();
            for (int i = 0; i < topMargin; ++i) {
                cout << endl;
            }
            cout << frames[frameIndex] << endl;
        }
    }

    /**
     * Displays the launch screen.
     */
    void LaunchScreen() {
        ClearScreen();
        DisplayLaunchScreen();
    }

    /**
     * Displays the help screen and waits for user input to return to the launch screen.
     */
    void Help() {
        ClearScreen();
        DisplayHelp();
        while (true) {
            if (_kbhit()) {
                _getch();
                LaunchScreen();
                break;
            }
        }
    }

    /**
     * Displays the credits screen and waits for user input to return to the launch screen.
     */
    void Credits(){
        ClearScreen();
        DisplayCredits();
        while (true) {
            if (_kbhit()) {
                _getch();
                LaunchScreen();
                break;
            }
        }
    }

    /**
     * Handles user menu selection and navigates to the appropriate screen.
     */
    void HandleMenuSelection() {
        char choice;
        while (true) {
            if (_kbhit()) {
                choice = _getch();
                switch (choice) {
                    case '1':
                        Setup();
                        return;
                    case '2':
                        Help();
                        break;
                    case '3':
                        Credits();
                        break;
                    case '4':
                        exit(0);
                    default:
                        cout <<"Invalid choice. Please select again." << endl;
                }
            }
        }

    }

    /**
     * Displays the game over screen and handles user input for restarting or quitting.
     */
    void GameOver(bool won) {
        ClearScreen();
        if (won) {
            DisplayGameWon();
        }
        else{DisplayGameOver(score, level);}

        while (true) {
            if (_kbhit()) {
                char choice = _getch();

                if (choice == 'r' && !won) {
                    Setup();
                    break;
                } else if (choice == 'q' && !won) {
                    RunGame();
                }else{
                    RunGame();
                }
            }
        }
    }

    /**
     * Worker function to handle game logic.
     */
    void Worker() {

        if (!eggDropped) {
            eggX = birdX;
            eggDropped = true;
        }

        if (eggDropped) {
            eggY++;
        }

        if (eggY == height) {
            if (eggX >= basketX + 1 && eggX < basketX + 8) {
                score++;
                if (score % 10 == 0 && level <= 10) {
                    level++;
                    speed -= 20;
                }
                if (score == 100 && level == 11) {

                    LoadFrames("frames", 9);

                    int frameIndex = 0;
                    auto startTime = chrono::steady_clock::now();
                    while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() < 4) {
                        DisplayFrame(frameIndex, 4);
                        frameIndex = (frameIndex + 1) % frames.size();
                        auto frameStartTime = chrono::steady_clock::now();
                        while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - frameStartTime).count() < 100) {

                        }
                    }

                    ClearScreen();

                    won = true;
                    gameOver = true;

                }
            }else {
                gameOver = true;
            }

            birdX = rand() % (width - 1);
            eggY = 1;
            eggDropped = false;
        }

        if (basketX < -1)
            basketX = -1;
        if (basketX >= width - 9)
            basketX = width - 9;
    }


    /**
     * Draws a loading animation on the console.
     */
    void DrawLoadingAnimation() {
        int rows = 10, cols = 10;
        vector<vector<string>> matrix(rows, vector<string>(cols, " "));


        int xOffset = (20 - rows) / 2;
        int yOffset = (25 - cols) / 2;

        int x = rows / 2, y = cols / 2;
        int dx = 0, dy = -1;
        int steps = 1, stepCount = 0, directionChanges = 0;

        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD bufferSize = {static_cast<SHORT>(cols), static_cast<SHORT>(rows)};
        COORD bufferCoord = {0, 0};
        SMALL_RECT writeRegion = {static_cast<SHORT>(yOffset), static_cast<SHORT>(xOffset), static_cast<SHORT>(yOffset + cols - 1), static_cast<SHORT>(xOffset + rows - 1)};
        CHAR_INFO buffer[rows * cols];

        for (int i = 0; i < rows * cols; ++i) {
            if (x >= 0 && x < rows && y >= 0 && y < cols) {
                matrix[x][y] = "*";
            }

            x += dx;
            y += dy;
            stepCount++;

            if (stepCount == steps) {
                stepCount = 0;
                directionChanges++;
                if (directionChanges % 2 == 0) steps++;
                int temp = dx;
                dx = -dy;
                dy = temp;
            }

            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    buffer[r * cols + c].Char.AsciiChar = matrix[r][c][0];
                    buffer[r * cols + c].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                }
            }

            WriteConsoleOutput(consoleHandle, buffer, bufferSize, bufferCoord, &writeRegion);

            auto frameStartTime = chrono::steady_clock::now();
            while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - frameStartTime).count() < 20) {

            }
        }
        ClearScreen();
    }

    /**
     * Main game loop that runs the game.
     */
    void RunGame(){
        SetConsoleOutputCP(CP_UTF8);
        HideConsoleCursor();
        LaunchScreen();
        HandleMenuSelection();

        auto lastFrameTime = high_resolution_clock::now();

        DrawLoadingAnimation();

        while (true) {
            if (gameOver) {
                GameOver(won);
            } else {
                auto currentFrameTime = high_resolution_clock::now();
                auto frameDuration = duration_cast<milliseconds>(currentFrameTime - lastFrameTime);

                if (frameDuration.count() >= speed) {
                    Draw();
                    Input();
                    Worker();
                    lastFrameTime = currentFrameTime;
                }
            }
        }

    }
}