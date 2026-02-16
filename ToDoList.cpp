#include <iostream>
#include <fstream>
#include <windows.h>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;

const string fileName = "tasks.txt";
const string tempName= "temp.txt";
const string incompleteTag = "\\incomplete";
const string completeTag = "\\complete";
const string subtaskTag = "\\sub";

const int subtaskId = 1;
const int completionId = 0;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// sets colour of text, default parameter resets to default text colour
void SetCol(const int &k = 7) {
    SetConsoleTextAttribute(hConsole, k);
}
// opens file
void OpenFile(fstream &file, const string &mode = "in", const string &name = fileName) {
    try {
        if(mode == "in")
            file.open(name, fstream::in);
        else if (mode == "app")
            file.open(name, fstream::app);
        else if (mode == "out")
            file.open(name, fstream::out);
        else 
            cout << "Invalid mode for opening file\n";
    } catch(std::exception) {
        cout << "Couldn't read file: " << name << "\n";
    }
}

void OverwriteFile(fstream &outFile, fstream &inFile) {
    // rewrites changes from temp back to file
    outFile.close();
    OpenFile(outFile, "out");
    inFile.close();
    OpenFile(inFile, "in", tempName);

    string temp;
    while(getline(inFile, temp)) {
        outFile << temp << '\n';
    }

    outFile.close();
    inFile.close();
}

// appends task to text file 
void AddTask(string &task) {
    if(task == "") return;
    fstream file;
    OpenFile(file, "app");

    // removes any user added backslashes
    task.erase(std::remove(task.begin(), task.end(), '\\'), task.end());

    // creates task with with a default incomplete tag, and a subtask state of 0
    file << task << incompleteTag << subtaskTag << "0\n";
    file.close();
}

// returns list of task names as a map of integers
map<int, string> GetTaskList(const bool &removeTag = false) {
    map<int, string> taskList;
    fstream file;

    try {
        file.open(fileName, ios::in);
    } catch(std::exception) {
        cout << "Unable to open file: " << fileName << "\n";
        return taskList;
    }

    string temp;
    int index = 1;
    while(getline(file, temp)) {
        if(temp == "") continue;
        if(removeTag)
            taskList[index] = temp.substr(0, temp.find('\\'));
        else 
            taskList[index] = temp;

        index++;
    }

    return taskList;
}

// gets tag of the task, defaults to getting completion status tag
string GetTag(const int &line, const int &tagId) {
    fstream file;
    OpenFile(file, "in");

    // gets the last instance of '\\' in the line to get the tag, repeats multiple times to get other tags
    map<int, string> taskList = GetTaskList();
    string tag = taskList[line];
    for(int i = 0; i <= tagId; i++) {
        if(i == 0) {
            int index = tag.find('\\');
            if(index == tag.npos) return "";
            tag = tag.substr(tag.find('\\'));
        }
        else {
            string s = tag.substr(1);
            int index = s.find('\\');
            if(index == s.npos) return "";
            tag = s.substr(s.find('\\'));
        }

    }

    // trims any trailing tags
    int n = tag.substr(1).find('\\');
    if(n != -1) {
        tag = tag.substr(0, n+1);
    }

    // returns only the subtask depth if requested subtask tag
    if(tagId == subtaskId) {
        tag = tag[tag.length()-1];
    }

    file.close();
    return tag;
}

// tries to turn string to int, default return -1 if unable
int StrToInt(const string &text, const int &exception = -1) {
    try {
        int num = stoi(text);
        return num;
    } catch(std::invalid_argument) {
        return exception;
    }
}

// return list of subtasks underneath specified parent
vector<int> GetSubtasksOf(const int &parent) {
    fstream file;
    OpenFile(file, "in");

    int parentSubNum = StrToInt(GetTag(parent, subtaskId));
    vector<int> subtasks;

    string temp;
    int index = 1;
    bool isChild = false;
    while(getline(file, temp)) {
        // will continue loop only after the iteration the parent is found
        if(!isChild && index != parent) {
            index++;
            continue;
        } else if(index == parent) {
            isChild = true;
            index++;
            continue;
        }

        // add each child to vector until subtask is not underneath parent
        int childNum = StrToInt(GetTag(index, subtaskId));
        if(childNum > parentSubNum) 
            subtasks.push_back(index);
        else
            isChild = false;

        index++;
    }

    return subtasks;
}

// changes tag of specified task to either complete or incomplete
void CompleteTask(const int &index, const bool &complete, const bool &userAttempted = true) {
    fstream file;
    OpenFile(file, "in");
    fstream tempFile;
    OpenFile(tempFile, "out", tempName);

    if(userAttempted && GetSubtasksOf(index).size() != 0) return; 

    // puts all contents from file to tempFile, only changing the tag of specified task
    int currentInd = 1;
    string temp;
    while(getline(file, temp)) {
        if(temp == "") continue;

        if(currentInd == index) {
            string subTag = GetTag(index, subtaskId);
            string completionTag = ((complete) ? incompleteTag : completeTag);
            tempFile << temp.substr(0, temp.find('\\')) << completionTag << subtaskTag << subTag << "\n";
        }
        else 
            tempFile << temp << "\n";
        currentInd++;
    }

    // write changes to main file
    OverwriteFile(file, tempFile);
}

// completely removes specified task
void RemoveTask(int index) {
    fstream file;
    OpenFile(file, "in");
    fstream tempFile;
    OpenFile(tempFile, "out", tempName);

    vector<int> subtasks = GetSubtasksOf(index);

    // writes every line from file to temp except the specified line
    string temp;
    int currentInd = 1;
    while(getline(file, temp)) {
        if(currentInd == index || find(subtasks.begin(), subtasks.end(), currentInd) != subtasks.end()) {
            currentInd++;
            continue;
        };
        tempFile << temp << '\n';
        currentInd++;
    }

    // writes changes to main file
    OverwriteFile(file, tempFile);
}

// prints each line in the text file as a numbered list
void ReadTasks() {
    fstream file;
    OpenFile(file, "in");

    map<int, string> tasks = GetTaskList();
    // checks for any lines without proper tags
    for(int i = 1; i <= tasks.size(); i++) {
        string tag = GetTag(i, completionId);
        if(tag == "") {
            RemoveTask(i);
            continue;
        }
        tag = GetTag(i, subtaskId);
        if(tag == "") {
            RemoveTask(i);
            continue;
        } 
    }

    // completes any parents of subtasks if all subtasks of that parent are complete
    for(int i = tasks.size(); i > 0; i--) {
        vector<int> subtasks = GetSubtasksOf(i);
        if(subtasks.size() == 0) continue;

        bool allComplete = true;
        for(int n = 0; n < subtasks.size(); n++) {
            if(GetTag(subtasks[n], completionId) == incompleteTag)
                allComplete = false;
        }

        CompleteTask(i, !allComplete, false);
    }

    string temp;
    int index = 1;
    while(getline(file, temp)) {
        if(temp == "") continue;

        // sets status of complete or incomplete based on tag of task in text file
        string status;
        string completionTag = GetTag(index, completionId);
        if(completionTag == completeTag)
            status = "X ";
        else 
            status = "O ";

        // prints task to list, adding colour for fun
        int subNum = StrToInt(GetTag(index, subtaskId));
        if(subNum == -1) return;

        cout << '\t';
        if(subNum != -1){
            for(int i = 0; i < subNum-1; i++)
                cout << "  ";
            if(subNum != 0)
                cout << "- ";
        }
        string number;
        for(int i = 0; i < (index % 10); i++)
            number += '\b' + ' ' + '\b';
        SetCol(6);
        cout << index << '.';
        SetCol();
        cout << "|"; 
        SetCol((status == "O ") ? 4 : 10);
        cout << status; 
        SetCol();
        cout << temp.substr(0, temp.find('\\')) << "\n";
        index++;
    }
    file.close();
}

// returns user inputs for integers, default returns -1 if unable
int GetChoiceInt(const bool &loop = false, const int &exception = -1) {
    string choice;
    getline(cin, choice);
    while(true){
        try {
            int choiceInt = stoi(choice);
            return choiceInt;
        } catch(std::invalid_argument) {
            cin.clear();
            if(!loop)
                return exception;
        }
    }
}

// returns user input as string
string GetChoiceStr(const bool &autoLower = true) {
    string choice;
    getline(cin, choice);
    if(autoLower)
        for(int i = 0; i < choice.length(); i++) choice[i] = tolower(choice[i]);
    return choice;
}

// insert subtask under specified parent
void CreateSubtask(const int &parentId, const string &name) {

    // subtask depth of parent
    int parentSubNum = StrToInt(GetTag(parentId, subtaskId));
    if(parentSubNum == -1) return;

    fstream file;
    OpenFile(file, "in");
    fstream tempFile;
    OpenFile(tempFile, "out", tempName);

    // enters normal file into temp, but adding the subtask under the desired parent
    string temp;
    int index = 1;
    while(getline(file, temp)) {
        if(index == parentId) {
            tempFile << temp << '\n';
            tempFile << name << incompleteTag << subtaskTag << parentSubNum+1 << '\n';
            index++;
            continue;
        }

        tempFile << temp << '\n';
        index++;
    }

    // writes changes to main file
    OverwriteFile(file, tempFile);
}

// changes tag of user specified task
void TaskInteract(const int &index) {
    // gets index of task
    map<int, string> taskList = GetTaskList();
    string chosenTask;
    try {
        chosenTask = taskList.at(index);
    } catch(std::out_of_range) {
        return;
    }

    // check if task is already complete or not, changes action if so
    bool isComplete;
    string tag = GetTag(index, completionId);
    string action;
    if(tag == incompleteTag)  
        isComplete = false;
    else if(tag == completeTag) 
        isComplete = true;
    else
        cout << "Invalid tag: " << tag << "\n";
    
    // confirmation
    string choice;
    CompleteTask(index, isComplete);
}

// options that edit the state of the list
void OtherInteract(string choice) {
    // quits
    if(choice == "e")
        return;
    // adds a task to the text file
    else if (choice == "a") {
        cout << "\tEnter task name: ";
        string taskName = GetChoiceStr();
        if(taskName == "") return;
        AddTask(taskName);
    // if user types 'r' followed by a number, removes specified task via number
    } else if(choice[0] == 'r') {
        if(choice.length() != 1) {
            int num = StrToInt(choice.substr(1));
            if(num != -1)
                RemoveTask(num);
            else
                return;
        } else {
            cout << "\tEnter task number: ";
            RemoveTask(GetChoiceInt());
        }
    
    // creates a subtask under specified task
    } else if (choice[0] == 'c') {
        int num = -1;
        if(choice.length() != 1) {
            num = StrToInt(choice.substr(1));
            
        } else {
            cout << "\tEnter task parent: ";
            num = GetChoiceInt();
        }
        if(num == -1) return;

        cout << "\tEnter task name: ";
        string name = GetChoiceStr();
        if(name == "") return;
        CreateSubtask(num, name);
    }
}

void PrintHeader() {
    SetCol(14);
    cout << "\t====  ___  __  __||  ___    ||	 () //\" ====\n";
    cout << "\t ||  ||_||    ||_|| ||_||   ||__ || _//  ||\n";
    cout << "\t============================================\n\n";
    SetCol();
}

int main() {

    string choice = "e";
    do {
        // resets screen, lists tasks, prompts user for choice
        system("cls");
        PrintHeader();
        ReadTasks();
        SetCol(14);
        cout << "\n\t--- [A]dd Task ---\n\t--- [R]emove Task ---\n\t--- [C]reate Subtask---\n\t--- [E]xit ---\n\t";
        
        SetCol(9);
        choice = GetChoiceStr();

        // completes or undo's the specified task
        int intChoice = StrToInt(choice);
        if(intChoice != -1) 
            TaskInteract(intChoice);
        // other editing options
        else
            OtherInteract(choice);
        SetCol();

        
    } while (choice != "e");

    return 0;
}