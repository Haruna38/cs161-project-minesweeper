#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "MineField.h"

using namespace std;

struct highScore
{
    string name;
    int score;
};

class MinesweeperGameManager {
    public:

    vector <MineField*> records;

    MineField* currentData;

    vector <string> startMenuOptions {"Start a new game", "Load an existing game", "Leaderboard", "Quit"};

    MinesweeperUtils Utils;
    
    vector <highScore> scoreBoard;

    void load (MineField* data);
    // load game from given data

    void create();
    // Create new game

    void fetchRecords();
    // get Records from file

    void start();
    // start the game

    void gameOver ();
    // Gameover the player

    void render ();
    // render the minefield to the console

    void startProcess ();
    // start the user input reccursive process

    void save ();
    // save the current record

    void chooseRecord ();
    // userInput choose record

    void exportRecords();
    // export records to file

    void removeRecord (MineField* data);
    // clear the specified record from the records list

    void quit(bool needSave);
    // quit the game

    void win ();
    // user wins the game

    void endGameSelection (string text, bool isWin);
    // display endgame with text

    void fetchHighscores();
    // load highscore from file

    void sortHighscores();
    // sort highscores in the descending order

    void exportHighscores();
    // export highscores to file

    void printHighscores();
    // print scoreboard to the console

    int getScore(MineField* data);
    // get the score from the MineField
};

void MinesweeperGameManager::load (MineField* data)
{
    records.push_back(data);
    currentData = data;
    startProcess();
};

void MinesweeperGameManager::create ()
{
    Utils.clearConsole();
    int mapSize, bombs;
 cout << "Please Enter field size (minimum 2, -1 to back to menu): ";
    Utils.readInt(mapSize);
    if (mapSize < 0) {
        start();
        return;
    }
    mapSize = max(mapSize, 2);
    
    cout << "Please Enter the amount of bombs (minimum 1, maximum  "<< pow(mapSize,2) - 1 <<", - 1 to back to menu): ";
    Utils.readInt(bombs);
    if (bombs < 0) 
    {
        start();
        return;
    }
    bombs = max(1, min(bombs, (int)pow(mapSize, 2) - 1));
    load(new MineField(mapSize, bombs));
};

void MinesweeperGameManager::fetchRecords ()
{
    records.clear();
    ifstream recordsFile ("MinesweeperRecords.txt");
    string content( (istreambuf_iterator<char>(recordsFile) ), (istreambuf_iterator<char>()    ) );
    vector <string> rawRecords = Utils.splitString(content, "\n\n");
    for (string rawRecord : rawRecords) {
        vector <string> recordInfo = Utils.splitString(rawRecord, "\n");
        while (recordInfo.size() < 4) recordInfo.push_back("0");
        for (string &info : recordInfo) {
            if (info == "") info = "0";
        }
        MineField* MF = new MineField((long)Utils.stoi(recordInfo[0]), (long)Utils.stoi(recordInfo[1]), Utils.stoi(recordInfo[2]), recordInfo[3]);
        if (MF->valid) records.push_back(MF);
    }
    recordsFile.close();
    exportRecords();
};

void MinesweeperGameManager::fetchHighscores()
{
    scoreBoard.clear();
    ifstream recordsFile("MinesweeperHighscores.txt");
    string content((istreambuf_iterator<char>(recordsFile)), (istreambuf_iterator<char>()));
    vector <string> rawRecords = Utils.splitString(content, "\n\n");
    for (string rawRecord : rawRecords) {
        vector <string> recordInfo = Utils.splitString(rawRecord, "\n");
        highScore data;
        data.name = recordInfo[0];
        data.score = Utils.stoi(recordInfo[1]);
        scoreBoard.push_back(data);
    }
    recordsFile.close();
    sortHighscores();
}

void MinesweeperGameManager::exportRecords () 
{
    ofstream recordsFile ("MinesweeperRecords.txt");
    for (MineField* MF : records) {
        recordsFile << MF->exportData() << endl << endl;
    }
    recordsFile.close();
}

void MinesweeperGameManager::exportHighscores () 
{
    ofstream recordsFile ("MinesweeperHighscores.txt");
    for (highScore hS : scoreBoard) {
        recordsFile << hS.name << endl << hS.score << endl << endl;
    }
    recordsFile.close();
}

void MinesweeperGameManager::chooseRecord () 
{
    Utils.clearConsole();
    int i = 0, selection;
    for (MineField* fieldData : records) 
    {
        cout << i++ << ". " << Utils.toDateString(fieldData->savedTimestamp) << " | Field size: " << fieldData->Size << ", Bombs: " << fieldData->bombsCount << endl; 
    }
    if (records.size() == 0) cout << "NO RECORDS SAVED" << endl;
    cout << endl << "Choose a game from your saved records, -1 to go back: ";
    Utils.readInt(selection);
    if (selection < 0) start();
    else if (selection >= 0 && selection < records.size()) load(records[selection]);
    else chooseRecord();
};

void MinesweeperGameManager::printHighscores () {
    Utils.clearConsole();
    int i = 1;
    cout << "LEADERBOARD:" << endl;
    for (highScore data : scoreBoard) {
        cout << i++ << ". " << data.name << " (" << data.score << " points)" << endl;
    }
    if (scoreBoard.size() == 0) cout << "NO DATA" << endl;
    int x;
    cout << "Input anything to go back to home screen...";
    Utils.readInt(x);
    start();
}

// small component function

bool compareHighscores(highScore i1, highScore i2)
{
    return (i1.score > i2.score);
}

void MinesweeperGameManager::sortHighscores() {
    sort(scoreBoard.begin(), scoreBoard.end(), compareHighscores);
    while (scoreBoard.size() > 10) scoreBoard.pop_back();
    exportHighscores();
}

void MinesweeperGameManager::removeRecord (MineField* data) 
{
    auto it = find(records.begin(), records.end(), data);
    if (it != records.end()) records.erase(it);
}

void MinesweeperGameManager::save ()
{
    currentData->save();
};

void MinesweeperGameManager::start () 
{
    Utils.clearConsole();
    fetchRecords();
    fetchHighscores();
    cout << "Welcome to Minesweeper!" << endl;
    cout << "Please choose a number below to start:" << endl;
    int menuSize = startMenuOptions.size();
    for (int i = 0; i < menuSize; ++i) cout << i << ". " << startMenuOptions[i] << endl;
    int option;
    cout << "Your option: ";
    Utils.readInt(option);
    if (option < 0 || option >= menuSize) start();
    else switch (option) {
        case 0:
            create();
            break;
        case 1:
            chooseRecord();
            break;
        case 2:
            printHighscores();
            break;
        case 3:
            quit(false);
            break;
    }
};

void MinesweeperGameManager::endGameSelection (string str, bool isWin)
{
    currentData->save();
    render();
    removeRecord(currentData);
    cout << str << endl;
    cout << "Times played: " << Utils.convertTime(currentData->timesPlayed) << endl;
    if (isWin) {
        int score = getScore(currentData);
        cout << "Score: " << score << endl;
        if (score > 0 && currentData->timesPlayed > 0 && (scoreBoard.size() < 10 || score > scoreBoard.back().score)) {
            string name;
            cout << "Looks like that you get a high score! Please Enter your name: ";
            cin.ignore();
            getline(cin, name);
            highScore Score;
            Score.name = name;
            Score.score = score;
            scoreBoard.push_back(Score);
            sortHighscores();
            cout << "Highscore saved!" << endl;
        }
    }
    cout << "Type anything to back to menu, 0 to quit: ";
    int selection;
    Utils.readInt(selection);
    if (selection == 0) quit(false);
    else start();
}

void MinesweeperGameManager::gameOver () 
{
    currentData->revealAllBombs();
    endGameSelection("Oops! You digged deeper and caught a bomb! Too bad!", false);
}

int MinesweeperGameManager::getScore(MineField* data) {
    if (data->timesPlayed == 0) return 0;
    double size_2 = pow(data->Size, 2);
    double mine = data->bombsCount;
    double difficulty = 5.0 / (pow(size_2, 2.0) / 4.0 - size_2 + 1.0) * (mine - 1.0) * (size_2 - 1.0 - mine);
    return round(difficulty * data->clicks / data->timesPlayed * size_2);
}

void MinesweeperGameManager::win ()
{
    endGameSelection("You win!", true);
}

void MinesweeperGameManager::quit (bool needSave)
{
    if (needSave) 
    {
        cout << "Save this game? (-1: Cancel, 0: No, 1: Yes): ";
        int prom;
        Utils.readInt(prom);
        if (prom < 0) {
            startProcess();
            return;
        }
        if (prom == 0) removeRecord(currentData);
        else save();
        exportRecords();
    }
    cout << "Quitting game" << endl;
    cout << "Thanks for playing!";
}

void MinesweeperGameManager::render () 
{
    Utils.clearConsole();
    currentData->render();
}

void MinesweeperGameManager::startProcess ()
{
    render();
    cout << "Input row, column position of a block (from 0 to " << currentData->Size - 1 << ") and a flagged number (0 to open, 1 to flag), -1 to exit: ";
    int row, column, flagged;
    Utils.readInt(row);
    if (row < 0) {
        quit(true);
        return;
    }
    Utils.readInt(column);
    if (column < 0) {
        quit(true);
        return;
    }
    Utils.readInt(flagged);
    if (flagged < 0) {
        quit(true);
        return;
    }
    bool hasBomb = currentData->reveal(row, column, false, flagged);
    if (hasBomb) gameOver();
    else {
        if (currentData->unrevealedCellsCount == currentData->bombsCount) win();
        else startProcess();
    }
}

