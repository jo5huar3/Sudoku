#include <iostream>

struct ThreeByThree
{
    struct
    {
        int answere;
        std::vector<int> possibleAnsweres;
    } smallSquare[3][3];
    std::vector<int> stillNeed;
};
struct Sudoku
{
    ThreeByThree game[9];
};
void buildGame(Sudoku &sudoku)
{
    int bigSquare = 0;
    int input;
    for (int i = 0; i < 9; ++i)
    {
        if (i && !(i % 3))
            bigSquare += 3;
        for (int j = 0; j < 9; ++j)
        {
            if (j && !(j % 3))
                ++bigSquare;
            std::cin >> input;
            sudoku.game[bigSquare].smallSquare[i % 3][j % 3].answere = input;
        }
        bigSquare -= 2;
    }
}
void printGame(const Sudoku &sudoku)
{
    int bigSquare = 0;
    for (int i = 0; i < 9; ++i)
    {
        if (i && !(i % 3))
            bigSquare += 3;
        for (int j = 0; j < 9; ++j)
        {
            if (j && !(j % 3))
                ++bigSquare;
            std::cout << sudoku.game[bigSquare].smallSquare[i % 3][j % 3].answere << " ";
        }
        std::cout << std::endl;
        bigSquare -= 2;
    }
}
std::vector<int> buildPossibleVec(ThreeByThree &game)
{
    std::vector<int> possibleAnsweres;
    for (int x = 1; x < 10; ++x)
    {
        bool found = false;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (game.smallSquare[i][j].answere == x)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (!found)
            possibleAnsweres.push_back(x);
    }
    return possibleAnsweres;
}
void elimnateHorizontalPossibles(
    Sudoku &sudoku, int bigSquare, int row, std::vector<int> &possibleAnsweres)
{
    int diffA, diffB;
    if (!(bigSquare % 3))
        diffA = 1, diffB = 2;
    else if (bigSquare % 3 == 1)
        diffA = -1, diffB = 1;
    else
        diffA = -1, diffB = -2;
    for (std::vector<int>::iterator it = possibleAnsweres.begin();
        it != possibleAnsweres.end(); ++it)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (sudoku.game[bigSquare + diffA].smallSquare[row][j].answere == *it)
            {
                possibleAnsweres.erase(it--);
                break;
            }
            if (sudoku.game[bigSquare + diffB].smallSquare[row][j].answere == *it)
            {
                possibleAnsweres.erase(it--);
                break;
            }
        }
    }
}
void eliminateVerticalPossibles(
    Sudoku &sudoku, int bigSquare, int col, std::vector<int> &possibleAnsweres)
{
    int diffA, diffB;
    if (bigSquare < 3)
        diffA = 3, diffB = 6;
    else if (bigSquare < 6)
        diffA = -3, diffB = 3;
    else
        diffA = -3, diffB = -6;
    for (std::vector<int>::iterator it = possibleAnsweres.begin();
        it != possibleAnsweres.end(); ++it)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (sudoku.game[bigSquare + diffA].smallSquare[i][col].answere == *it)
            {
                possibleAnsweres.erase(it--);
                break;
            }
            if (sudoku.game[bigSquare + diffB].smallSquare[i][col].answere == *it)
            {
                possibleAnsweres.erase(it--);
                break;
            }
        }
    }
}
void solveGame(Sudoku &sudoku)
{
    int bigSquare = 0;
    for (int i = 0; i < 9; ++i)
    {
        bool madeChange = false;
        if (i && !(i % 3))
            bigSquare += 3;
        for (int j = 0; j < 9; ++j)
        {
            if (j && !(j % 3))
                ++bigSquare;
            if (!(sudoku.game[bigSquare].smallSquare[i % 3][j % 3].answere))
            {
                //  Build a vector of possible answeres only using three by three square
                sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres =
                    buildPossibleVec(sudoku.game[bigSquare]);
                //  Eliminate possible answeres according to row
                elimnateHorizontalPossibles(sudoku, bigSquare, i % 3,
                                            sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres);
                //  Eliminate possible answeres according to col               
                eliminateVerticalPossibles(sudoku, bigSquare, j % 3,
                                            sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres);
                if(sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres.size() == 1)
                {
                    sudoku.game[bigSquare].smallSquare[i % 3][j % 3].answere =
                        sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres.front();
                    sudoku.game[bigSquare].smallSquare[i % 3][j % 3].possibleAnsweres.pop_back();
                    madeChange = true;
                    break;
                }
            }
        }
        if(madeChange)
            i = -1, bigSquare = 0;
        else
            bigSquare -= 2;
    }
}
int main()
{
    Sudoku sudoku;
    buildGame(sudoku);
    printGame(sudoku);
    std::cout << std::endl;
    solveGame(sudoku);
    printGame(sudoku);
    std::cout << std::endl;
    return 0;
}