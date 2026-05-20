#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <limits>
using namespace std;
using namespace std::chrono;

struct Record {
    string name;
    string cnic;
    string difficulty;
    int moves;
    double timeTaken;
    double score;
};

struct Player {
    string name;
    string cnic;
    string difficulty;
    int moves;
    double timeTaken;
    double score;
};

struct Point { int row, col; };
Point playerPos;


bool isAllDigits(const string &s) {
    if (s.empty()) return false;
    for (char c : s) if (c < '0' || c > '9') return false;
    return true;
}

bool containsLetter(const string &s) {
    for (char c : s) if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return true;
    return false;
}

int safeReadIntSimple() {
    string line;
    while (true) {
        if (!getline(cin, line)) return 0;
        if (line.empty()) { cout << "Invalid input. Enter a number: "; continue; }
        bool ok = true; size_t start = 0;
        if (line[0] == '+' || line[0] == '-') start = 1;
        for (size_t i = start; i < line.size(); ++i) if (line[i] < '0' || line[i] > '9') { ok = false; break; }
        if (!ok) { cout << "Invalid input. Enter a number: "; continue; }
        int value = 0;
        size_t index = start;
        for (; index < line.size(); ++index) value = value * 10 + (line[index] - '0');
        if (line[0] == '-') value = -value;
        return value;
    }
}

string getValidatedCNIC() {
    string value;
    while (true) {
        cout << "Enter your CNIC (exactly 13 digits): ";
        if (!getline(cin, value)) value = "";
        if (value.size() != 13) { cout << "CNIC must be of 13 digits.\n"; continue; }
        if (!isAllDigits(value)) { cout << "CNIC must contain digits only.\n"; continue; }
        return value;
    }
}

string getValidatedName() {
    string text;
    while (true) {
        cout << "Enter your name (spaces allowed, no commas): ";
        if (!getline(cin, text)) text = "";
        if (text.empty()) { cout << "Name cannot be empty.\n"; continue; }
        if (text.find(',') != string::npos) { cout << "Name cannot contain commas.\n"; continue; }
        if (!containsLetter(text)) { cout << "Name must contain letters.\n"; continue; }
        return text;
    }
}

void setColor(int fg, int bg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

COORD getGridStartCoord(int gridSize) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(h, &csbi);

    int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    int gridWidth = gridSize * 2; 
    int leftPad = (cols - gridWidth) / 2;
    if (leftPad < 0) leftPad = 0;

    int topPad = (rows - gridSize) / 2 - 1; 
    if (topPad < 0) topPad = 0;

    COORD start;
    start.X = csbi.srWindow.Left + (SHORT)leftPad;
    start.Y = csbi.srWindow.Top  + (SHORT)topPad;
    return start;
}

void clearRegion(COORD start, int width, int height) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    for (int r = 0; r < height; ++r) {
        COORD rowStart = { start.X, (SHORT)(start.Y + r) };
        FillConsoleOutputCharacter(h, ' ', width, rowStart, &written);
        FillConsoleOutputAttribute(h, 7, width, rowStart, &written); 
    }
}

void clearConsoleWindow() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(h, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    COORD start = { csbi.srWindow.Left, csbi.srWindow.Top };
    clearRegion(start, width, height);
    SetConsoleCursorPosition(h, start);
}

void showAlignedMessage(int gridSize, const string &msg, bool wait) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD start = getGridStartCoord(gridSize);
    COORD msgCoord = { start.X, (SHORT)(start.Y + gridSize + 1) };
    clearRegion(msgCoord, gridSize * 2, 2);

    SetConsoleCursorPosition(h, msgCoord);
    setColor(12, 0); 
    cout << msg;
    setColor(7, 0);
    if (wait) {
        cout << "  (press any key)";
        _getch();
        clearRegion(msgCoord, gridSize * 2, 2);
    }
}

void printGameUI(int gridSize, int moves) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD start = getGridStartCoord(gridSize);
    COORD line1 = { start.X, (SHORT)(start.Y + gridSize) };
    COORD line2 = { start.X, (SHORT)(start.Y + gridSize + 1) };

    clearRegion(line1, gridSize * 2, 2);

    SetConsoleCursorPosition(h, line1);
    cout << "Moves: " << moves;
    SetConsoleCursorPosition(h, line2);
    cout << "Use Arrow keys, 'T' regenerate, 'Q' quit.";
}

double calculateDistance(Point a, Point b) {
    return sqrt(pow(double(b.row - a.row),2.0) + pow(double(b.col - a.col),2.0));
}

Point getRandomPoint(int size) { return {1 + rand() % (size - 2), 1 + rand() % (size - 2)}; }

bool insideBounds(int r, int c, int size) { return (r >= 0 && r < size && c >= 0 && c < size); }

void printMaze(char** maze, int size, Point start, Point end) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD gridStart = getGridStartCoord(size);
    int gridWidth = size * 2;

    clearRegion(gridStart, gridWidth, size);

    for (int i = 0; i < size; ++i) {
        COORD rowStart = { gridStart.X, (SHORT)(gridStart.Y + i) };
        SetConsoleCursorPosition(h, rowStart);
        for (int j = 0; j < size; ++j) {
            char cell = maze[i][j];
            if (i == playerPos.row && j == playerPos.col) { setColor(14,0); cout << "P "; setColor(7,0); continue; }
            if      (cell == '#') setColor(1,0);
            else if (cell == 'S') setColor(2,0);
            else if (cell == 'E') setColor(4,0);
            else if (cell == 'X') setColor(5,0);
            else if (cell == '@') setColor(3,0);
            else                  setColor(7,0);
            cout << cell << " ";
            setColor(7,0);
        }
    }
    COORD afterGrid = { gridStart.X, (SHORT)(gridStart.Y + size + 2) };
    SetConsoleCursorPosition(h, afterGrid);
}

void regenerateObstacles(char** maze,int size, Point start, Point end){
    for(int i=1;i<size-1;i++)
        for(int j=1;j<size-1;j++)
            if(maze[i][j]!='#') maze[i][j]='.';
    for(int i=1;i<size-1;i++)
        for(int j=1;j<size-1;j++){
            if((i==start.row && j==start.col)||(i==end.row && j==end.col)) continue;
            int r=rand()%10;
            if(r<3) maze[i][j] = (rand()%2==0)?'X':'@';
        }
    maze[start.row][start.col]='S';
    maze[end.row][end.col]='E';
}

char** createMaze(int size, Point start, Point end){
    char** maze = new char*[size];
    for(int i=0;i<size;i++) maze[i]=new char[size];
    for(int i=0;i<size;i++)
        for(int j=0;j<size;j++)
            if(i==0||j==0||i==size-1||j==size-1) maze[i][j]='#';
            else maze[i][j]='.';
    int r=start.row, c=start.col;
    while(r!=end.row || c!=end.col){
        maze[r][c]='.';
        if(r<end.row) ++r;
        else if(r>end.row) --r;
        else if(c<end.col) ++c;
        else if(c>end.col) --c;
    }
    regenerateObstacles(maze,size,start,end);
    return maze;
}

void saveRecord(const string &filename, const Player &p){
    ofstream fout(filename, ios::app);
    if(!fout){ cout<<"Error saving record!\n"; return; }
    fout<<"Name: "<<p.name<<"\n";
    fout<<"CNIC: "<<p.cnic<<"\n";
    fout<<"Difficulty: "<<p.difficulty<<"\n";
    fout<<"Moves: "<<p.moves<<"\n";
    fout<<"Time Taken: "<<p.timeTaken<<" seconds\n";
    fout<<"Score: "<<p.score<<"\n";
    fout<<"-----------------------------\n";
    fout.close();
}

Record* loadRecords(const string &filename, int &outCount){
    ifstream fin(filename);
    if(!fin){ outCount=0; return nullptr; }
    outCount=0;
    string line;
    while(getline(fin,line)) if(line.find("-------------")!=string::npos) outCount++;
    if(outCount==0) return nullptr;

    fin.clear(); fin.seekg(0);
    Record* recs = new Record[outCount];
    int index=0;
    while(getline(fin,line)){
        if(line.find("Name: ")==0) recs[index].name = line.substr(6);
        else continue;
        getline(fin,line); recs[index].cnic = line.substr(6);
        getline(fin,line); recs[index].difficulty = line.substr(11);
        getline(fin,line); recs[index].moves=stoi(line.substr(7));
        getline(fin,line); recs[index].timeTaken = stod(line.substr(12));
        getline(fin,line); recs[index].score=stoi(line.substr(7));
        getline(fin,line); 
        index++;
    }
    return recs;
}

bool findNameByCNIC(const string &cnic, string &nameOut, const string files[3]){
    for(int f=0; f<3; f++){
        int n; Record* recs = loadRecords(files[f], n);
        if(recs){
            for(int i=0;i<n;i++) if(recs[i].cnic==cnic){ nameOut=recs[i].name; delete[] recs; return true; }
            delete[] recs;
        }
    }
    return false;
}

int filterByCNIC(Record* recs, int n, const string &cnic, Record* &out){
    int count=0;
    for(int i=0;i<n;i++) if(recs[i].cnic==cnic) count++;
    if(count==0){ out=nullptr; return 0; }
    out = new Record[count];
    int idx=0;
    for(int i=0;i<n;i++) if(recs[i].cnic==cnic) out[idx++] = recs[i];
    return count;
}

void sortByMovesThenTimeAsc(Record* recs,int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++){
            bool better=false;
            if(recs[j].moves<recs[i].moves) better=true;
            else if(recs[j].moves==recs[i].moves && recs[j].timeTaken<recs[i].timeTaken) better=true;
            if(better){ Record time = recs[i]; recs[i]=recs[j]; recs[j]=time; }
        }
}

void showPersonalTopNInFile(const string &filename, const string &cnic, int topN){
    int n; Record* all = loadRecords(filename,n);
    if(!all || n==0){ cout<<"No records.\n"; if(all) delete[] all; return; }
    Record* mine; int m = filterByCNIC(all,n,cnic,mine);
    delete[] all;
    if(!mine || m==0){ cout<<"No records for your CNIC.\n"; if(mine) delete[] mine; return; }
    sortByMovesThenTimeAsc(mine,m);
    if(topN==0 || topN>m) topN=m;
    for(int i=0;i<topN;i++)
        cout<<i+1<<". Moves: "<<mine[i].moves<<" | Time: "<<mine[i].timeTaken<<" | Score: "<<mine[i].score<<"\n";
    delete[] mine;
}

void showLeaderboard(const string &filename,int topN){
    int n; Record* all = loadRecords(filename,n);
    if(!all || n==0){ cout<<"No records.\n"; if(all) delete[] all; return; }
    sortByMovesThenTimeAsc(all,n);
    if(topN==0 || topN>n) topN=n;
    for(int i=0;i<topN;i++)
        cout<<i+1<<". "<<all[i].name<<" ("<<all[i].cnic<<") | Moves: "<<all[i].moves
            <<" | Time: "<<all[i].timeTaken<<" | Score: "<<all[i].score<<"\n";
    delete[] all;
}


void playAndRecord(Player &currentPlayer, int gridSize, const string &filename){
    Point start,end;
    do{ start = getRandomPoint(gridSize); end = getRandomPoint(gridSize); }
    while(calculateDistance(start,end)<double(gridSize-2));

    char** maze = createMaze(gridSize,start,end);
    playerPos = start;
    currentPlayer.moves=0;
    currentPlayer.timeTaken=0.0;

    auto tStart = high_resolution_clock::now();
    bool finished=false;

    while(true){
        clearConsoleWindow();

        printMaze(maze,gridSize,start,end);
        printGameUI(gridSize, currentPlayer.moves);

        int ch = _getch();
        int newRow = playerPos.row, newCol = playerPos.col;
        if(ch==224 || ch==0){ ch=_getch();
            if(ch==72) newRow--; else if(ch==80) newRow++; else if(ch==75) newCol--; else if(ch==77) newCol++;
        }
        else if(ch=='t'||ch=='T'){
            regenerateObstacles(maze,gridSize,start,end);
            playerPos=start; currentPlayer.moves=0; currentPlayer.timeTaken=0.0;
            tStart = high_resolution_clock::now();
            continue;
        }
        else if(ch=='q'||ch=='Q'){ break; }
        else continue;

        if(!insideBounds(newRow,newCol,gridSize)){
            showAlignedMessage(gridSize, "Out of bounds!", true); 
            continue;
        }
        if(maze[newRow][newCol]=='#'||maze[newRow][newCol]=='X'||maze[newRow][newCol]=='@'){
            showAlignedMessage(gridSize, "Blocked!", true);
            continue;
        }

        maze[playerPos.row][playerPos.col] = (playerPos.row==start.row && playerPos.col==start.col)?'S':'.';
        playerPos = {newRow,newCol};
        currentPlayer.moves++;

        if(playerPos.row==end.row && playerPos.col==end.col){
            auto tEnd = high_resolution_clock::now();
            duration<double> elapsed = tEnd-tStart;
            currentPlayer.timeTaken = round(elapsed.count()*100.0)/100.0;
            currentPlayer.score = currentPlayer.moves;

            clearConsoleWindow();
            printMaze(maze,gridSize,start,end);
            printGameUI(gridSize, currentPlayer.moves);

            COORD startCoord = getGridStartCoord(gridSize);
            COORD msgCoord = { startCoord.X, (SHORT)(startCoord.Y + gridSize + 2) };
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), msgCoord);

            setColor(10,0);
            cout << "Congrats! Finished in " << currentPlayer.moves 
                 << " moves, " << currentPlayer.timeTaken 
                 << " sec. Score=" << currentPlayer.score << "\n";
            setColor(7,0);  

            finished=true; break;
        }
    }

    if(finished) saveRecord(filename,currentPlayer);

    for(int i=0;i<gridSize;i++) delete[] maze[i];
    delete[] maze;
}


int main(){
    srand(time(0));

    Player currentPlayer;
    const string files[3] = {"easy.txt","medium.txt","hard.txt"};

    cout<<"==== Maze Game ====\n";
    currentPlayer.cnic = getValidatedCNIC();
    string existingName;
    if(findNameByCNIC(currentPlayer.cnic,existingName,files)){
        currentPlayer.name = existingName;
        cout<<"Welcome back, "<<currentPlayer.name<<"!\n";
    } 
    else {
        currentPlayer.name = getValidatedName();
        cout<<"Welcome "<<currentPlayer.name<<", Good luck!\n";
    }

    cout<<"\nSelect Difficulty:\n1. Easy (10x10)\n2. Medium (15x15)\n3. Hard (20x20)\nChoice: ";
    int choice = safeReadIntSimple();
    int gridSize = 10; string filename;
    while(!(choice==1 || choice ==2 || choice==3)){
        cout << "Invalid Difficulty mode! Enter again!" << endl;
        choice = safeReadIntSimple();
        }
    if(choice==1){ currentPlayer.difficulty="Easy"; gridSize=10; filename="easy.txt"; }
    else if(choice==2){ currentPlayer.difficulty="Medium"; gridSize=15; filename="medium.txt"; }
    else if(choice==3){ currentPlayer.difficulty="Hard"; gridSize=20; filename="hard.txt"; }
    
    playAndRecord(currentPlayer,gridSize,filename);

    while(true){
        cout<<"\nMenu:\n1. Play again\n2. View your scores\n3. Leaderboard\n4. Exit\nChoice: ";
        int option = safeReadIntSimple();
        if(option==1){
            cout<<"Choose difficulty \n 1.Easy \n 2.Medium \n 3.Hard:\n ";
            int modeChoice = safeReadIntSimple();
            if(modeChoice==1){ currentPlayer.difficulty="Easy"; gridSize=10; filename="easy.txt"; }
            else if(modeChoice==2){ currentPlayer.difficulty="Medium"; gridSize=15; filename="medium.txt"; }
            else if(modeChoice==3){ currentPlayer.difficulty="Hard"; gridSize=20; filename="hard.txt"; }
            else {
                while(!(modeChoice==1 || modeChoice==2 || modeChoice==3)){
                    cout << "Invalid input! Enter again: " << endl;
                    modeChoice = safeReadIntSimple();
                }
            }
            playAndRecord(currentPlayer,gridSize,filename);
        }
        else if(option==2){
        cout << "Choose difficulty \n1. Easy\n2. Medium\n3. Hard:\n";
        int modeChoice = safeReadIntSimple();
        while(modeChoice != 1 && modeChoice != 2 && modeChoice != 3){
            cout << "Invalid difficulty! Enter 1, 2, or 3: ";
            modeChoice = safeReadIntSimple();
        }

        string file = (modeChoice == 1 ? "easy.txt" :
                    (modeChoice == 2 ? "medium.txt" : "hard.txt"));

        cout << "Show top 5, 10 or 0 for all: ";
        int numberSelect = safeReadIntSimple();
        while(numberSelect != 5 && numberSelect != 10 && numberSelect != 0){
            cout << "Invalid number! Enter again: ";
            numberSelect = safeReadIntSimple();
        }
        showPersonalTopNInFile(file, currentPlayer.cnic, numberSelect);
    }
        else if(option==3){
        cout << "Choose difficulty \n1. Easy\n2. Medium\n3. Hard:\n";
        int modeChoice = safeReadIntSimple();
        while(modeChoice != 1 && modeChoice != 2 && modeChoice != 3){
            cout << "Invalid difficulty! Enter 1, 2, or 3: ";
            modeChoice = safeReadIntSimple();
        }

        string file = (modeChoice == 1 ? "easy.txt" :
                    (modeChoice == 2 ? "medium.txt" : "hard.txt"));

        cout << "Show top 5, 10 or 0 for all: ";
        int numberSelect = safeReadIntSimple();
        while(numberSelect != 5 && numberSelect != 10 && numberSelect != 0){
            cout << "Invalid number! Enter again: ";
            numberSelect = safeReadIntSimple();
        }

        showLeaderboard(file, numberSelect);
    }

        else if(option==4){ 
            cout<<"Goodbye!\n"; 
            break; 
        }
        else{
            cout<<"Invalid option.\n";
        }
    }
    return 0;
}
