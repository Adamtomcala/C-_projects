#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

int MAX = 10000;
int MIN = -10000;

int maxDepth = 6;

int finalIndex;

atomic<bool> noMove;

char myPlayer;

char columns[8] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

int directions[8][2] = {
    {-1, 0}, {-1,  1}, {0,  1}, { 1,  1},
    { 1, 0}, { 1, -1}, {0, -1}, {-1, -1},
};

int corners[4] = {0, 7, 56, 63};

int border[16] = {2, 3, 4, 5, 23, 31, 39, 47, 61, 60, 59, 58, 40, 32, 24, 16};

int badPoints[12] = {1, 9, 8, 6, 14, 15, 55, 54, 62, 57, 49, 48};

mutex mtx_timer;

condition_variable cv_timer;

void print(string table) {
    cout << "  ABCDEFGH" << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (j == 0) {
                cout << i + 1 << " " << table[i*8 + j];
            }
            else if(j == 7){
                cout << table[i * 8 + j] << endl;
            }
            else {
                cout << table[i * 8 + j];
            }
        }
    }
}

// Function checks if the first command (START..) is valid or not
string inicialization(string command) {
    // This loop erases white spaces in the beginning of the string
    for (int i = 0; i < int(command.length()); i++) {
        if (isspace(command[i])) {
            command.erase(command.begin() + i);
            --i;
        }
        else {
            break;
        }
    }

    // Here i am checking if the first 5 letters create word START
    if (command.length() < 5 || command.substr(0, 5).compare("START") != 0) {
        clog << "Invalid command" << endl;
        exit(1);
    }

    int numOfSpaces = 0;
    // This loop erases white spaces after the word START
    for (int i = 5; i < int(command.length()); i++) {
        if (isspace(command[i])) {
            command.erase(command.begin() + i);
            --i;
            ++numOfSpaces;
        }
        else {
            break;
        }
    }

    if (numOfSpaces == 0) {
        clog << "Invalid command" << endl;
        exit(1);
    }

    // Here i am checking if the player is B or W
    if (command.length() < 6 || (command[5] != 'B' && command[5] != 'W')) {
        clog << "Invalid command" << endl;
        exit(1);
    }

    numOfSpaces = 0;
    // This loop erasing white spaces after player.
    for (int i = 6; i < int(command.length()); i++) {
        if (isspace(command[i])) {
            command.erase(command.begin() + i);
            --i;
            ++numOfSpaces;
        }
        else {
            break;
        }
    }

    if (numOfSpaces == 0) {
        clog << "Invalid command" << endl;
        exit(1);
    }
    // Checking if there is something.
    if (command.length() < 7) {
        clog << "Invalid command" << endl;
        exit(1);
    }

    int sizeOfNum = 0;
    for (int i = 6; i < int(command.length()); i++) {
        if (isdigit(command[i])) {
            ++sizeOfNum;
            continue;
        }
        else if(isspace(command[i])){
            break;
        }
        else {
            clog << "Invalid command" << endl;
            exit(1);
        }
    }

    for (int i = 6 + sizeOfNum; i < int(command.length()); i++) {
        if(!isspace(command[i])){
            clog << "Invalid command" << endl;
            exit(1);
        }
    }

    myPlayer = command[5];

    return command.substr(6, 6 + sizeOfNum);
}

// Function checks, if i am outside of table or inside
bool checkBorder(int x, int y) {

    return (x > 7 || x < 0 || y > 7 || y < 0);

}

// Function checks if the specific square is flippable or not
bool isFlippable(int posX, int posY, string table, char player, char opponent) {
    // iterating over all directions (8)

    for (auto dir : directions) {
        // checking if it is a border square
        if (checkBorder(posX + dir[0], posY + dir[1])) {
            continue;
        }

        if (table[(posX + dir[0]) * 8 + posY + dir[1]] == opponent) {

            int currX = posX + dir[0];
            int currY = posY + dir[1];

            while (table[currX * 8 + currY] == opponent) {

                currX += dir[0];
                currY += dir[1];

                if (checkBorder(currX, currY)) {
                    currX -= dir[0];
                    currY -= dir[1];
                    break;
                }
            }

            if (table[currX * 8 + currY] == player) {
                return true;
            }
        }
    }

    return false;
}

// Function gets exact index in the table (which is flippable) and does move for the player 
string doMove(int index, string table, char player, char opponent) {

    int row = index / 8;
    int col = index % 8;

    string newTable = table;
    newTable[index] = player;

    // Iterating over the all posibble directions
    for (auto dir : directions) {
        // Checking if i am not outsite of table
        if (checkBorder(row + dir[0], col + dir[1])) {
            continue;
        }

        // If there is a opponent in this direction, figuring out if it is a legit move
        if (newTable[(row + dir[0]) * 8 + col + dir[1]] == opponent) {
            int actCount = 0;
            int currX = row + dir[0];
            int currY = col + dir[1];

            // Iterating over this direction
            while (newTable[currX * 8 + currY] == opponent) {
                currX += dir[0];
                currY += dir[1];

                if (checkBorder(currX, currY)) {
                    currX -= dir[0];
                    currY -= dir[1];
                    break;
                }
                actCount++;
            }

            // If the last stone is player`s, move is legit
            if (newTable[currX * 8 + currY] == player) {
                int actRow = row;
                int actCol = col;

                for (int i = 0; i < actCount; i++) {
                    actRow += dir[0];
                    actCol += dir[1];
                    newTable[actRow * 8 + actCol] = player;
                }
            }

        }
    }
    // returning updated table
    return newTable;
}

// Function finds all legit moves in specific game state for the player
vector<int> findLegitMove(string table, char player, char opponent) {
    int row;
    vector<int> indexes;
    // Iterating over the table
    for (int i = 0; i < 8; i++) {
        row = i * 8;
        for (int j = 0; j < 8; j++) {
            // If the square is empty (-) and is flippable, adding index to vector
            if (table[row + j] == '-' && isFlippable(row / 8, j, table, player, opponent)) {
                indexes.push_back(row + j);
            }
        }
    }

    return indexes;
}

// Function checks if any player can do a move, if they can not, game is over.
bool isGameOver(string table, char player, char opponent) {
    return findLegitMove(table, player, opponent).size() == 0 && findLegitMove(table, opponent, player).size() == 0;
}

// Function that calculates value of actual move 
int heuristic(string table) {

    // First, counting all stones of every colour
    int white = int (count(table.begin(), table.end(), 'O'));
    int black = int (count(table.begin(), table.end(), 'X'));

    // Next parameter of my heuristic is number of legit moves for every player
    white += int(findLegitMove(table, 'X', 'O').size());
    black += int(findLegitMove(table, 'O', 'X').size());

    // Iterating over the corners, corners have bigger priority
    for (int corner : corners) {
        if (table[corner] == 'O') {
            white += 10;
        }
        else if (table[corner] == 'X') {
            black += 10;
        }
    }

    // In my function, i consider border squares also with higher priority, but less than corners
    for (int point : border) {
        if (table[point] == 'O') {
            white += 5;
        }
        else if (table[point] == 'X') {
            black += 5;
        }
    }

    // I inicializied "bad corners", this corners are dangerous, because they allow acces to corners for opponet.
    int helpIndex = 0;
    for (int i = 0; i < 12; i++) {
        if (i != 0 && i % 3 == 0) {
            ++helpIndex;
        }
        if (table[badPoints[i]] == 'O' && table[corners[helpIndex]] == '-') {
            white -= 5;
        }
        else if (table[badPoints[i]] == 'O' && table[corners[helpIndex]] == '-') {
            black -= 5;
        }
    }
    /*
    for (int badPoint : badPoints) {
        if (table[badPoint] == 'O') {
            white -= 5;
        }
        else if (table[badPoint] == 'X') {
            black -= 5;
        }
    }
    */
    // Player is black
    if (myPlayer == 'X') {
        return black - white;
    }
    // Player is white
    else {
        return white - black;
    }
}


// Minimax algorithm with alpha & beta pruning
int minimax(string table, bool maximize, int depth, int alpha, int beta, char player, char opponent) {
    
    // First check if depth is 0 or if game is over or if time expires
    if (noMove || depth == 0 || isGameOver(table, player, opponent)) {
        //cout << noMove;
        return heuristic(table);
    }

    // Find every move for specif game table
    vector<int> indexes = findLegitMove(table, player, opponent);
    int size = int(indexes.size());

    // Branch for maximizin my player
    if (maximize) {
        // If my player does not have anz turn, exit(1)
        if (depth == maxDepth && size == 0) {
            exit(1);
        }

        // If programs will not find any better index until time ends, i choose the first
        if (depth == maxDepth) {
            finalIndex = indexes[0];
        }

        int maximum = MIN;
        // Iterating over all valid moves
        for (int i = 0; i < int(indexes.size()); i++) {
            //Creating new table
            string newTable = doMove(indexes[i], table, player, opponent);
            // Nesting deeper, changin players, because next function will search for oppenent
            int val = minimax(newTable, false, depth - 1, alpha, beta, opponent, player);

            // If the new value is greater than old maximum and depth is MAXDEPTH -> i found a new better move.
            //CHECKNUT
            if ((val > maximum) && depth == maxDepth) { 
                finalIndex = indexes[i];
            }

            maximum = max(maximum, val);
            alpha = max(alpha, val);

            // alpha - beta prunning
            if (beta <= alpha) {
                break;
            }

        }
        return maximum;
    }
    // Branch for minimizing enemy player
    else {
        int minimum = MAX;
        // Iterating over all valid moves
        for (int i = 0; i < int(indexes.size()); i++) {
            //Creating new table
            string newTable = doMove(indexes[i], table, player, opponent);
            // Nesting deeper, changin players, because next function will search for oppenent
            int val = minimax(newTable, true, depth - 1, alpha, beta, opponent, player);
            
            minimum = min(minimum, val);
            beta = min(beta, val);

            // alpha - beta prunning
            if (beta <= alpha) {
                break;
            }

        }
        return minimum;
    }
}

void threadTimer(time_t time) {
    unique_lock<mutex> lock(mtx_timer);
    chrono::seconds secs(time);

    noMove.store(false);
    cv_timer.wait_for(lock, secs);
    noMove.store(true);

    lock.unlock();
    cv_timer.notify_all();
}

// Function creates the final answer from selected index.
string result(int index) {
    string result = "";
    int col = index % 8;
    int row = index / 8 + 1;

    result.push_back(columns[col]);
    result.append(to_string(row));

    return result;
}


int main()
{
    string command;
    char player;
    char opponent;
    time_t time;

    getline(cin, command);

    string t = "";

    t = inicialization(command);

    player = myPlayer == 'B' ? 'X' : 'O';

    opponent = player == 'X' ? 'O' : 'X';

    time = stoi(t);

    if (time <= 0) {
        clog << "Invalid time" << endl;
        exit(1);
    }

    cout << "1" << endl;

    
    string table = "";
    string finalPosition = "";
    vector<int> indexes;


    while (std::getline(std::cin, command))
    {

        if (command.compare("STOP") == 0) {
            return 0;
        }

        if (command.length() != 69 || command.substr(0, 4).compare("MOVE") != 0 || command[4] != ' ') {
            clog << "Invalid command." << endl;
            exit(1);
        }

        for (int i = 5; i < 69; i++) {
            if (command[i] != '-' && command[i] != 'O' && command[i] != 'X') {
                clog << "Invalid command." << endl;
                exit(1);
            }
        }
        
        //finalIndex = -1;

        table = command.substr(5);


        if (isGameOver(table, player, opponent)) {
            clog << "Game is over." << endl;
            return 0;
        }

        thread secondThread(threadTimer, time);

        int val = minimax(table, true, maxDepth, MIN, MAX, player, opponent);

        secondThread.join();

        noMove = false;

        finalPosition = result(finalIndex);
        
        cout << finalPosition << endl;

        val;
    }

    return 0;
}
