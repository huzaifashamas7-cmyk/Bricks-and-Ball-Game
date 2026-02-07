
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

//========== SETTINGS =========== 
const int WIDTH = 80;
const int HEIGHT = 25;
void hideCursor();
void showCursor();

//--------- GAME VARIABLES ---------
int paddleX = 30;
int paddleY = 21;

int ballX, ballY;
int ballDX = 1;
int ballDY = -1;
bool ballLaunched = false;
bool gameOver = false;
int score = 0;
int lives = 3; 

// ================= BRICKS ================= 
const int BRICK_COUNT = 13;
int brickX[BRICK_COUNT] = {5,15,25,35,45,55,65,10,20,30,40,50,60};
int brickY[BRICK_COUNT] = {3,3,3,3,3,3,3,6,6,6,6,6,6};
bool brickAlive[BRICK_COUNT];

// --------- FUNCTION PROTOTYPES --------
void gotoxy(int x,int y);
void setColor(int color);
void drawMaze();
void drawPaddle();
void erasePaddle();
void movePaddle();
void drawBall();
void eraseBall();
void drawBricks();
void eraseBrick(int i);
void drawScore();
void moveBall();
bool checkWin();
int mainMenu();
int pauseMenu();
void redrawGame();

// ================= MAIN ================= *
int main() {
    while(true) {  
        system("cls");
        int choice = mainMenu();

        if(choice == 1)
         {
            score = 0;
            lives = 3;
            ballLaunched = false;
            gameOver = false;
            ballDX = 1;
            ballDY = -1;
            paddleX = 30;   
            paddleY = 21;

            for(int i=0; i<BRICK_COUNT; i++) brickAlive[i] = true;

            system("cls");
            hideCursor();
            drawMaze();
            drawBricks();
            drawPaddle();
            drawScore();

            ballX = paddleX + 10;
            ballY = paddleY - 1;
            drawBall();

            while(!gameOver) {
                // Check Pause
                if(GetAsyncKeyState('P') & 0x8000 || GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    int pauseChoice = pauseMenu();
                    if(pauseChoice == 1) { hideCursor(); redrawGame(); }
                    else if(pauseChoice == 2) { 
                        score = 0; lives = 3; ballLaunched = false;
                        for(int i=0; i<BRICK_COUNT; i++) brickAlive[i] = true;
                        redrawGame(); 
                    }
                    else if(pauseChoice == 3) { gameOver = true; }
                    else if(pauseChoice == 4) { exit(0); }
                }

                movePaddle();

                if(!ballLaunched)
                 {
                    eraseBall();
                    ballX = paddleX + 10;
                    ballY = paddleY - 1;
                    drawBall();
                    gotoxy(25, HEIGHT + 2);
                    setColor(15);
                    cout << "Press SPACE to Launch Ball";

                    if(GetAsyncKeyState(VK_SPACE) & 0x8000) {
                        ballLaunched = true;
                        gotoxy(25, HEIGHT + 2);
                        cout << "                          "; 
                    }
                } else {
                    moveBall();
                }

                // --- WIN CONDITION CHECK ---
                if(checkWin()) {
                    Sleep(500);
                    system("cls");
                    showCursor();
                    setColor(10); // Green
                    cout << "\n\n\n\n\t\t\t      ======================" << endl;
                    cout << "\t\t\t             YOU WIN!" << endl;
                    cout << "\t\t\t      ======================" << endl;
                    cout << "\t\t\t          Final Score: " << score << endl;
                    cout << "\n\t\t\t Press any key to return to menu...";
                    
                    while(_kbhit()) _getch(); 
                    _getch(); 
                    gameOver = true;
                }
                
                if (gameOver) break;
                Sleep(35);
            }
        } else if (choice == 2) {
            system("cls");
            setColor(11);
            cout << "********* GAME INSTRUCTIONS *********" << endl << endl;
            cout << "1. Move: Use LEFT and RIGHT arrow keys." << endl;
            cout << "2. Launch: Press SPACE to start the ball." << endl;
            cout << "3. Goal: Break all bricks [#####] to win." << endl;
            cout << "4. Lives: You have 3 lives. Don't hit the floor!" << endl;
            cout << "\nPress any key to return...";
            _getch();
        } else if (choice == 3) {
            return 0;
        }
    }
}

// ================= FUNCTIONS ================= 

void moveBall()
 {
    // 1. Erase ball from current position
    eraseBall();

    // 2. WALL COLLISION (Look-ahead logic)
    // Left/Right walls: Bounce before hitting column 0 or WIDTH-1
    if (ballX + ballDX <= 3 || ballX + ballDX >= WIDTH - 1) {
        ballDX = -ballDX;
    }

    // Top wall: Bounce before hitting row 0
    if (ballY + ballDY <= 0) {
        ballDY = -ballDY;
    }

    // 3. PADDLE COLLISION
    if (ballY + ballDY == paddleY && ballX >= paddleX && ballX <= paddleX + 20) {
        ballDY = -ballDY;
    }

    // 4. BRICK COLLISION
    for (int i = 0; i < BRICK_COUNT; i++) {
        if (brickAlive[i]) {
            if (ballY + ballDY == brickY[i] && ballX >= brickX[i] && ballX <= brickX[i] + 6) {
                brickAlive[i] = false;
                eraseBrick(i);
                ballDY = -ballDY;
                score += 10;
                drawScore();
            }
        }
    }

    // 5. Update Position
    ballX += ballDX;
    ballY += ballDY;

    // 6. Draw Ball at new position
    drawBall();

    // 7. BOTTOM BOUNDARY (LOSE LIFE)
    if (ballY >= HEIGHT - 2) {
        lives--;
        ballLaunched = false;
        drawScore();
        
        if (lives <= 0) {
            system("cls");
            setColor(12); // Red
            cout << "\n\n\n\n\t\t\t      ======================" << endl;
            cout << "\t\t\t            GAME OVER" << endl;
            cout << "\t\t\t      ======================" << endl;
            cout << "\t\t\t          Final Score: " << score << endl;
            cout << "\n\t\t\t Press any key to return to menu...";
            while(_kbhit()) _getch();
            _getch();
            gameOver = true;
        } else {
            // Reset ball for next life
            eraseBall();
            ballX = paddleX + 10;
            ballY = paddleY - 1;
        }
    }
}

void drawMaze() {
    setColor(17); // Blue
    for(int i=0; i<WIDTH; i++) {
        gotoxy(i,0); cout<<"█";
        gotoxy(i,HEIGHT-1); cout<<"█";
    }
    for(int i=0; i<HEIGHT; i++) {
        gotoxy(0,i); cout<<"█";
        gotoxy(WIDTH-1,i); cout<<"█";
    }
}

void drawScore() {
    setColor(11);
    // UI is now placed BELOW the maze
    gotoxy(2, HEIGHT); 
    cout << "SCORE: " << score << "    LIVES: " << lives << "          ";
}

int mainMenu() {
    showCursor();
    setColor(14);
    cout << "===============================" << endl;
    cout << "     BRICKS AND BALL GAME" << endl;
    cout << "===============================" << endl;
    setColor(7);
    cout << "1. Start Game" << endl;
    cout << "2. Instructions" << endl;
    cout << "3. Quit" << endl;
    cout << "\nSelection: ";
    int choice;
    cin >> choice;
    cin.ignore(); // Clears the Enter key buffer
    return choice;
}

void redrawGame() {
    system("cls");
    drawMaze();
    drawBricks();
    drawPaddle();
    drawScore();
    drawBall();
}

bool checkWin() {
    for(int i=0; i<BRICK_COUNT; i++) {
        if(brickAlive[i]) return false;
    }
    return true;
}

// --- HELPER FUNCTIONS ---

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void hideCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(h, &c);
    c.bVisible = FALSE; SetConsoleCursorInfo(h, &c);
}

void showCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(h, &c);
    c.bVisible = TRUE; SetConsoleCursorInfo(h, &c);
}

void drawPaddle() {
    setColor(12);
    for(int i=0; i<2; i++) {
        gotoxy(paddleX, paddleY+i);
        cout<<"====================";
    }
}

void erasePaddle() {
    for(int i=0; i<2; i++) {
        gotoxy(paddleX, paddleY+i);
        cout<<"                    ";
    }
}

void movePaddle() {
    if(GetAsyncKeyState(VK_LEFT)&0x8000 && paddleX > 3) {
        erasePaddle(); paddleX--; drawPaddle();
    }
    if(GetAsyncKeyState(VK_RIGHT)&0x8000 && paddleX + 20 < WIDTH - 1) {
        erasePaddle(); paddleX++; drawPaddle();
    }
}

void drawBall() { setColor(14); gotoxy(ballX, ballY); cout << "O"; }
void eraseBall() { gotoxy(ballX, ballY); cout << " "; }

void drawBricks() {
    for(int i=0; i<BRICK_COUNT; i++) {
        if(brickAlive[i]) {
            if(i<4) setColor(13);
            else if(i<9) setColor(10);
            else setColor(14);
            gotoxy(brickX[i], brickY[i]);
            cout << "[#####]";
        }
    }
}

void eraseBrick(int i) {
    gotoxy(brickX[i], brickY[i]);
    cout << "       ";
}

int pauseMenu() {
    showCursor();
    setColor(15);
    gotoxy(25, 10); cout << "--- PAUSED ---";
    gotoxy(25, 11); cout << "1. Resume";
    gotoxy(25, 12); cout << "2. Restart";
    gotoxy(25, 13); cout << "3. Menu";
    gotoxy(25, 14); cout << "4. Quit";
    gotoxy(25, 16); cout << "Choice: ";
    int choice; cin >> choice; cin.ignore();
    return choice;
}