#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

enum class Difficulty { EASY, INTERMEDIATE, ADVANCED };

class Game {
public:
    virtual void play() = 0;
};

class Cell {
public:
    bool isMine;
    bool isExposed;
    bool isFlagged;
    int adjacentMines;

    Cell() : isMine(false), isExposed(false), isFlagged(false), adjacentMines(0) {}
};

class Board {
protected:
    vector<vector<Cell>> cells;
    int numRows;
    int numCols;
    int numMines;

public:
    Board(int rows, int cols, int mines) : numRows(rows), numCols(cols), numMines(mines) {
        cells.resize(rows, vector<Cell>(cols));
    }

    virtual void initializeMines() = 0;
    virtual void display(bool showMines = false) = 0;
    virtual void exposeCell(int row, int col) = 0;
    virtual void flagCell(int row, int col) = 0;
    virtual bool checkWin() = 0;
};

class MinesweeperBoard : public Board {
public:
    MinesweeperBoard(int rows, int cols, int mines) : Board(rows, cols, mines) {}

    void initializeMines() override {
        int minesPlaced = 0;
        while (minesPlaced < numMines) {
            int row = rand() % numRows;
            int col = rand() % numCols;
            if (!cells[row][col].isMine) {
                cells[row][col].isMine = true;
                minesPlaced++;
            }
        }
    }

    void calculateAdjacentMines() {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (!cells[i][j].isMine) {
                    cells[i][j].adjacentMines = countAdjacentMines(i, j);
                }
            }
        }
    }

    int countAdjacentMines(int row, int col) {
        int count = 0;
        for (int i = row - 1; i <= row + 1; ++i) {
            for (int j = col - 1; j <= col + 1; ++j) {
                if (i >= 0 && i < numRows && j >= 0 && j < numCols && cells[i][j].isMine) {
                    count++;
                }
            }
        }
        return count;
    }

    void display(bool showMines = false) override {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (cells[i][j].isExposed || showMines) {
                    if (cells[i][j].isMine) {
                        cout << "* ";
                    }
                    else {
                        cout << cells[i][j].adjacentMines << " ";
                    }
                }
                else if (cells[i][j].isFlagged) {
                    cout << "F ";
                }
                else {
                    cout << ". ";
                }
            }
            cout << endl;
        }
    }

    void exposeCell(int row, int col) override {
        if (row < 0 || row >= numRows || col < 0 || col >= numCols || cells[row][col].isExposed || cells[row][col].isFlagged) {
            return;
        }
        cells[row][col].isExposed = true;
        if (cells[row][col].isMine) {
            cout << "Game Over! You clicked on a mine." << endl;
            display(true);
            exit(0);
        }
        if (cells[row][col].adjacentMines == 0) {
            exposeAdjacentCells(row, col);
        }
    }

    void exposeAdjacentCells(int row, int col) {
        if (row < 0 || row >= numRows || col < 0 || col >= numCols || cells[row][col].isExposed || cells[row][col].isFlagged) {
            return;
        }
        cells[row][col].isExposed = true;
        if (cells[row][col].adjacentMines == 0) {
            for (int i = row - 1; i <= row + 1; ++i) {
                for (int j = col - 1; j <= col + 1; ++j) {
                    exposeAdjacentCells(i, j);
                }
            }
        }
    }

    void flagCell(int row, int col) override {
        if (row < 0 || row >= numRows || col < 0 || col >= numCols || cells[row][col].isExposed) {
            return;
        }
        cells[row][col].isFlagged = !cells[row][col].isFlagged;
    }

    bool checkWin() override {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (!cells[i][j].isExposed && !cells[i][j].isMine) {
                    return false;
                }
            }
        }
        return true;
    }
};

class MinesweeperGame : public Game {
private:
    MinesweeperBoard board;
    time_t startTime;

public:
    MinesweeperGame(int rows, int cols, int mines) : board(rows, cols, mines), startTime(0) {}

    void startTimer() {
        startTime = time(nullptr);
    }

    void displayTimer() {
        time_t currentTime = time(nullptr);
        int elapsedTime = currentTime - startTime;
        cout << "Time elapsed: " << elapsedTime << " seconds" << endl;
    }

    void play() override {
        board.initializeMines();
        board.calculateAdjacentMines();
        startTimer();

        while (true) {
            board.display();
            displayTimer();

            int row, col;
            char clickType;
            cout << "Enter row, column, and click type (L/R): ";
            cin >> row >> col >> clickType;

            if (clickType == 'L') {
                board.exposeCell(row, col);
            }
            else if (clickType == 'R') {
                board.flagCell(row, col);
            }

            if (board.checkWin()) {
                cout << "Congratulations! You've won the game!" << endl;
                break;
            }
        }
    }
};

int main() {
    srand(time(nullptr));

    int choice;
    cout << "-------------MINESWEEPER-----------" << endl;
    cout << "Select difficulty level: 1. Easy 2. Intermediate 3. Advanced: " << endl;
    cin >> choice;
    Difficulty difficulty = Difficulty::EASY;
    switch (choice) {
    case 1:
        difficulty = Difficulty::EASY;
        break;
    case 2:
        difficulty = Difficulty::INTERMEDIATE;
        break;
    case 3:
        difficulty = Difficulty::ADVANCED;
        break;
    default:
        cout << "Invalid choice. Defaulting to Easy." << endl;
    }

    int rows, cols, mines;

    switch (difficulty) {
    case Difficulty::EASY:
        rows = 8;
        cols = 8;
        mines = 10;
        break;
    case Difficulty::INTERMEDIATE:
        rows = 16;
        cols = 16;
        mines = 40;
        break;
    case Difficulty::ADVANCED:
        rows = 24;
        cols = 24;
        mines = 99;
        break;
    }

    MinesweeperGame game(rows, cols, mines);
    game.play();


    return 0;
}
