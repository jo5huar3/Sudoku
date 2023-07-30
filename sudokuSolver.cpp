#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
/*
    ThreeByThree

    3x3 Matrix, where stillNeed refers to values the 3x3 matrix is missing. SmallSquare inner struct
    represents an individual square from the matrix.
*/
struct ThreeByThree
{
    struct
    {
        int answere;
        std::vector<int> possibleAnsweres;
    } smallSquare[3][3];
    std::vector<int> stillNeed;
};
/*
    Sudoku

    9x9 matrix of game to be solved.
*/
struct Sudoku
{
    ThreeByThree game[9];
};
/*
    MultithreadSoduku

    Variables that threads need.
*/
struct MultithreadSudoku
{
    Sudoku *gamePtr;
    int identifier;
    int *activeThreads;
    pthread_cond_t *activeThreadsCondition;
    pthread_mutex_t *threadSem;
    sem_t *sudokuCopySem;
};
/*
    buildGame

    Insert values into the struct
*/
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
/*
    toString

    Return string representation of Sudoku struct (9x9 Matrix of smallSquare[][].answere values).
*/
std::string toString(const Sudoku &sudoku)
{
    int bigSquare = 0;
    std::string returnString;
    for (int i = 0; i < 9; ++i)
    {
        if (i && !(i % 3))
            bigSquare += 3;
        for (int j = 0; j < 9; ++j)
        {
            if (j && !(j % 3))
                ++bigSquare;
            returnString +=
                std::to_string(sudoku.game[bigSquare].smallSquare[i % 3][j % 3].answere) + " ";
        }
        returnString += "\n";
        bigSquare -= 2;
    }
    return returnString;
}
/*
    buildPossibleVec

    Create a vector of numbers 3x3 matrix is missing.
*/
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
/*
    eliminateHorizontalPossibles

    Erase the values from smallSquare[][].possibleAnsweres that are allready in the row.
*/
void eliminateHorizontalPossibles(
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
/*
    eliminateVerticalPossibles

    Erase values from smallSquare[][].possibleAnsweres that are allready in the column.
*/
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
/*
    checkCompletion

    Return true if 3x3 matrix has all nonzero values.
*/
bool checkCompletion(const ThreeByThree &game)
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (!(game.smallSquare[i][j].answere))
                return false;
        }
    }
    return true;
}
/*
    multithreadSolveGame

    Thread's starting point, each thread will solve one of the 3x3 matrices from the 9x9 matrix.
*/
void *multithreadSolveGame(void *voidPtr)
{
    MultithreadSudoku sudoku = *((MultithreadSudoku *)voidPtr);
    sem_post(sudoku.sudokuCopySem);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (!(sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].answere))
            {
                //  Build a vector of possible answeres only using three by three square
                sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres =
                    buildPossibleVec(sudoku.gamePtr->game[sudoku.identifier]);
                //  Eliminate possible answeres according to row
                eliminateHorizontalPossibles(*sudoku.gamePtr, sudoku.identifier, i,
                                            sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres);
                //  Eliminate possible answeres according to col
                eliminateVerticalPossibles(*sudoku.gamePtr, sudoku.identifier, j,
                                           sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres);
                // If small square only has one possible answere then that is the correct answere
                if (sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres.size() == 1)
                {
                    sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].answere =
                        sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres.front();
                    sudoku.gamePtr->game[sudoku.identifier].smallSquare[i][j].possibleAnsweres.pop_back();
                }
            }
        }
        if (i == 2)
        {   //  If not complete restart the outer most loop
            if (!checkCompletion(sudoku.gamePtr->game[sudoku.identifier]))
            {
                i = -1;
                //  No thread shall set i back to 0 until all threads have completed the current iteration
                pthread_mutex_lock(sudoku.threadSem);
                --(*sudoku.activeThreads);
                if(*sudoku.activeThreads)
                    pthread_cond_wait(sudoku.activeThreadsCondition, sudoku.threadSem);
                else//  Last thread will unblock all threads waiting
                    pthread_cond_broadcast(sudoku.activeThreadsCondition);
                ++(*sudoku.activeThreads);
                pthread_mutex_unlock(sudoku.threadSem);
            } 
             else
             {  
                 pthread_mutex_lock(sudoku.threadSem);
                 --(*sudoku.activeThreads);
                 // Last thread will unblock all threads waiting
                 if(!(*sudoku.activeThreads))
                     pthread_cond_broadcast(sudoku.activeThreadsCondition);
                 pthread_mutex_unlock(sudoku.threadSem);
             }
        }
    }
    return nullptr;
}
/*
    start

    Initialize all variables needed for multithreading and start solving the puzzle.
*/
int start(Sudoku *sudoku)
{
    pthread_t threads[9];
    static MultithreadSudoku multithreadSudoku;
    static char sudokuCopySem[] = "sudokuCopySem";
    static pthread_cond_t activeThreadsCondition = PTHREAD_COND_INITIALIZER;
    static pthread_mutex_t threadSem;
    static int activeThreads = 9;
    pthread_mutex_init(&threadSem, NULL);
    multithreadSudoku.sudokuCopySem = sem_open(sudokuCopySem, O_CREAT, 0600, 0);
    multithreadSudoku.gamePtr = sudoku;
    multithreadSudoku.activeThreadsCondition = &activeThreadsCondition;
    multithreadSudoku.threadSem = &threadSem;
    multithreadSudoku.activeThreads = &activeThreads;
    for (int i = 0; i < 9; ++i)
    {
        multithreadSudoku.identifier = i;
        if (pthread_create(&threads[i], NULL, multithreadSolveGame, (void *)&multithreadSudoku))
        {
            return 1;
        }
        sem_wait(multithreadSudoku.sudokuCopySem);
    }
    for (int i = 0; i < 9; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    sem_unlink(sudokuCopySem);
    return 0;
}
int main()
{
    static Sudoku sudoku;
    buildGame(sudoku);
    std::cout << toString(sudoku) << std::endl;
    if(start(&sudoku))
    {
        std::cerr << "Error creating threads" << std::endl;
        return 1;
    }
    std::cout << toString(sudoku) << std::endl;
    return 0;
}