//
//  main.cpp
//  Tetris_MAC
//
//  Created by Tony Yuan on 2023/4/9.
//
// This is a implementation of tetris
// I used this to learn cpp
// It's not fully from myself
// I followed this tut: https://www.youtube.com/watch?v=8OK8_tHeCIA&list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO

#include <iostream>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Seven shapes
wstring tetromino[7];
// Play field
int fieldWidth = 12;
int fieldHeight = 18;
// Storing all elements in field
unsigned char *Field = nullptr;


int Rotate(int x, int y, int r)
{
    int i = 0;
    switch (r % 4)
    {
        // No rotate
        case 0:
            i = 4 * y + x;
            break;
        // 90 degrees
        case 1:
            i = 12 + y - (x * 4);
            break;
        // 180 degrees
        case 2:
            i = 15 - (y * 4) - x;
            break;
        // 270 degrees
        case 3:
            i = 3 - y + (x * 4);
            break;
    }
    return i;
}

bool DosePieceFit(int nTetromino, int rotation, int posX, int posY)
{

    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
        {
            // Get index into piece
            int i = Rotate(x, y, rotation);
            
            // Get index into field
            int fi = (posY + y) * fieldWidth + (posX + x);
            
            // Checking dose blocks overlaps
            if (posX + x >= 0 && posX + x < fieldWidth)
                if (posY + y >= 0 && posY + y < fieldHeight)
                    if (tetromino[nTetromino][i] != L'.' && Field[fi] != 0)
                        return false;
        }
    
    return true;
}


int main(int argc, const char * argv[])
{
    
    initscr();
    srand(static_cast<unsigned int>(time(nullptr)));
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Normally if no key pressed, it will stop the process;
    // Add these:
    nodelay(stdscr, true);
    timeout(50);
    
    // insert code here...
    // Create assets:
    tetromino[0].append(L"..X."
                        "..X."
                        "..X."
                        "..X."); // Tetronimos 4x4
    
    tetromino[1].append(L"..X."
                        ".XX."
                        "..X."
                        "....");
    
    tetromino[2].append(L"...."
                        ".XX."
                        ".XX."
                        "....");
    
    tetromino[3].append(L"..X."
                        ".XX."
                        ".X.."
                        "....");
    
    tetromino[4].append(L".X.."
                        ".XX."
                        "..X."
                        "....");
    
    tetromino[5].append(L".X.."
                        ".X.."
                        ".XX."
                        "....");
    
    tetromino[6].append(L"..X."
                        "..X."
                        ".XX."
                        "....");
    
    // Use new to allocate memory to Field
    // Get the start of the memory address that allocated, assign to Field
    Field = new unsigned char[fieldWidth * fieldHeight];
    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            // Set boundary
            Field[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;
    
    bool bGameOver = false;
    int currentPiece = 0;
    int currentRotation = 0;
    // The initial shapes comes form top of the field
    int currentX = fieldWidth/2;
    int currentY = 0;
    bool bKey[4] = {false, false, false, false};
    int speed = 20;
    int speedCounter = 0;
    // Here Originally using bool, but I'm using int to represent it,
    // Same effect but more obvious.
    int forceDown = 0;
    int fallCounter = 0;
    int fallInterval = 10;
    vector<int> lines;
    int pieceCounter = 0;
    int currentScore = 0;
    
    
    // Game Loop
    
    while (!bGameOver)
    {
        // Game Timing ==================
        
        speedCounter++;
        forceDown = (speedCounter == speed);
        
        
        
        // Input ===================
        
        int ch = getch();
        bKey[0] = (ch == KEY_RIGHT);
        bKey[1] = (ch == KEY_LEFT);
        bKey[2] = (ch == KEY_DOWN);
        bKey[3] = (ch == 'z');
        // Game Logic ===================
        
        if (bKey[1])
        {
            if (DosePieceFit(currentPiece, currentRotation, currentX - 1, currentY))
                currentX = currentX - 1;
        }
        
        if (bKey[0])
        {
            if (DosePieceFit(currentPiece, currentRotation, currentX + 1, currentY))
                currentX = currentX + 1;
        }
        
        if (bKey[2])
        {
            if (DosePieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                currentY = currentY + 1;
        }
        
        if (bKey[3])
        {
            if (DosePieceFit(currentPiece, currentRotation + 1, currentX, currentY))
                currentRotation = currentRotation + 1;
        }
        
        if (forceDown)
        {
            speedCounter = 0;
            if (DosePieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                currentY++;
            else
            {
                // Lock the currentPiece
                for (int x = 0; x < 4; x++)
                    for (int y = 0; y < 4; y++)
                        if (tetromino[currentPiece][Rotate(x, y, currentRotation)] != L'.')
                            Field[(currentY + y) * fieldWidth + (currentX + x)] = currentPiece + 1;
                
                pieceCounter++;
                if (pieceCounter % 10 == 0)
                    if (speed >= 10)
                        speed--;
                
                // Check if they can cancel each other
                for (int y = 0; y < 4; y++)
                {
                    if (currentY + y < fieldHeight - 1)
                    {
                        bool isLine = true;
                        for (int x = 1; x < fieldWidth - 1; x++)
                        {
                            isLine &= (Field[(currentY + y) * fieldWidth + x]) != 0;
                            // Equavlient to isLine = isLine && (Field[(currentY + y) * fieldWidth + x]) != 0
                        }
                        if (isLine)
                        {
                            for (int x = 1; x < fieldWidth - 1; x++)
                                // Replace them with "="
                                Field[(currentY + y) * fieldWidth + x] = 8;
                            lines.push_back(currentY + y);
                        }
                    }
                        
                }
                
                
                currentScore += 25;
                if (!lines.empty()) currentScore += (1 << lines.size()) * 100;

                // Choose next piece

                currentX = fieldWidth/2;
                currentY = 0;
                currentRotation = rand() % 4;
                currentPiece = rand() % 7;

                // If next piece can not fit in field, gameover
                
                bGameOver = !(DosePieceFit(currentPiece, currentRotation, currentX, currentY));
            }
        }
        
        
        // Render Output ================
        
        
    
        
        // Display ======================
        
        // Draw Field
        for (int x = 0; x < fieldWidth; x++)
            for (int y = 0; y < fieldHeight; y++)
                                     //0123456789
                mvaddch(y + 2, x + 2, " ABCDEFG=#"[Field[y * fieldWidth + x]]);
    
        // Draw Current Piece
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                // Here tetromino is not a 2D array, cause it includes wstring obj,
                // tetromino[currentPiece] defult is tetromino[0], this equavlent to "..X...X...X...X."
                // [Rotate(x, y, currentRotation)] will be index
                if (tetromino[currentPiece][Rotate(x, y, currentRotation)] != L'.')
                    mvaddch(currentY + y + 2, currentX + x + 2, currentPiece + 65);  // 65 in ASCII is A
        
        // Display CUrrent Score
        mvprintw(0, fieldWidth + 6, "SCORE: %d", currentScore);
        
        // Animate Line Completion
        if (!lines.empty())
        {
            // Display Frame
            refresh();
            this_thread::sleep_for(milliseconds(400));
            // this_thread::sleep_for(milliseconds(400)); // Delay a bit
            
            for (auto &v : lines)
                for (int x = 1; x < fieldWidth - 1; x++)
                {
                    for (int y = v; y > 0; y--)
                        Field[y * fieldWidth + x] = Field[(y - 1) * fieldWidth + x];
                    Field[x] = 0;
                }
            
            lines.clear();
        }
        
        
        // Display Frame
        refresh();
    }
    
    endwin();
    
    // Simple output at the end
    cout << "Game Over! Your final score is: " << currentScore << endl;
}
