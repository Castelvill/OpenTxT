#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <stdio.h>

using namespace std;

struct bootUnion{
    string fileName;
    bool exitProgram;
    bool debug;
    int error;
};

bootUnion bootLoop(){
    string buffor, word;
    bool bootFromFile = false;

    bootUnion bootU;
    bootU.fileName = "";
    bootU.exitProgram = false;
    bootU.debug = false;
    bootU.error = 0;

    do{
        while(cin >> buffor){
            if(buffor == "boot"){
                if(bootFromFile == true){
                    bootU.error = 1;
                    return bootU;
                }
                bootFromFile = true;
                break;
            }
            else if(buffor == "debug"){
                if(bootFromFile == true){
                    bootU.error = 1;
                    return bootU;
                }
                bootU.debug = true;
                bootFromFile = true;
                break;
            }
            else if(buffor == "exit"){
                if(bootU.exitProgram == true){
                    bootU.error = 2;
                    bootU.exitProgram = false;
                    return bootU;
                }
                bootU.exitProgram = true;
            }
            else{
                if(bootFromFile){
                    bootU.fileName = buffor;

                    return bootU;
                }
                if(bootU.exitProgram == true){
                    bootU.error = 2;
                    bootU.exitProgram = false;
                    return bootU;
                }
            }
        };
        if(bootU.exitProgram == true){
            return bootU;
        }
    }while(1);

    bootU.error = -1;
    return bootU;
}

struct label{
    string name;
    unsigned int line;
};

class File{
public:
    ifstream descriptor;
    int head;
    vector <string> fileLines;
    vector <label> labels;
    int reg[4];
    void load(){
        head = 0;
        string labelName = "";
        while(!descriptor.eof()){
            fileLines.push_back(string());
            getline(descriptor, fileLines[fileLines.size()-1]);
            labelName = "";
            for(unsigned int i = 0; i < fileLines[fileLines.size()-1].size(); i++){
                if(fileLines[fileLines.size()-1][i] == '/"'){
                    break;
                }
                if(fileLines[fileLines.size()-1][i] == ' '){
                    if(labelName != "")
                        break;
                    continue;
                }
                if(fileLines[fileLines.size()-1][i] == ':'){
                    labels.push_back(label());
                    labels[labels.size()-1].name = labelName;
                    labels[labels.size()-1].line = fileLines.size()-1;
                    break;
                }
                labelName += fileLines[fileLines.size()-1][i];

            }
        }
    }
};

int regIndex(string reg){
    if(reg == "a" || reg == "A" || reg == "reg1" || reg == "r1")
        return 0;
    if(reg == "b" || reg == "B" || reg == "reg2" || reg == "r2")
        return 1;
    if(reg == "c" || reg == "C" || reg == "reg3" || reg == "r3")
        return 2;
    if(reg == "d" || reg == "D" || reg == "reg4" || reg == "r4")
        return 3;
    return -1;
}

int sRegIndex(string reg){
    if(reg == "sreg1" || reg == "sr1")
        return 0;
    if(reg == "sreg2" || reg == "sr2")
        return 1;
    if(reg == "sreg3" || reg == "sr3")
        return 2;
    if(reg == "sreg4" || reg == "sr4")
        return 3;
    return -1;
}

int main()
{
    bootUnion bootU = bootLoop();

    if(bootU.exitProgram == true){
        cout << "Exiting...\n";
        return 0;
    }
    if(bootU.error != 0 || bootU.fileName == ""){
        cout << "There's a problem!\n";
        return bootU.error;
    }

    vector <File> Files;
    Files.push_back(File());
    Files[0].descriptor.open(bootU.fileName+".txt");


    if(!Files[0].descriptor){
        cout << "Problem with the file!\n";
        return -2;
    }


    string buffor, buffor2, argv[10];
    int argHead, temp1, temp2;
    int sReg[4]; //system registers, from programmer's perspective read only

    unsigned int i;
    for(unsigned int i = 0; i < 10; i++){
        argv[i] = "";
    }

    Files[0].load();

    bool debug = bootU.debug;

    int flag = 0;

    ofstream writeOnlyFile;

    for(int fileStack = 0; fileStack > -1; fileStack--){
        for(Files[fileStack].head; Files[fileStack].head < Files[fileStack].fileLines.size(); Files[fileStack].head++){
            buffor = Files[fileStack].fileLines[Files[fileStack].head];

            if(buffor == "end write"){
                flag = 0;
                if(writeOnlyFile)
                    writeOnlyFile.close();
            }

            if(flag == 1){
                if(debug){
                    cout << "\n[writing]" << fileStack << "|"<< Files[fileStack].head << "> " << buffor;
                    Sleep(200);
                }
                writeOnlyFile << buffor << "\n";
            }

            if(flag == 0){
                if(debug){
                    cout << "\n" << fileStack << "|"<< Files[fileStack].head << "> " << buffor << ":";
                    Sleep(200);
                }
                if(buffor == "return"){
                    for(i = 0; i < 4; i++){
                        sReg[i] = Files[fileStack].reg[i];
                    }
                    break;
                }
                else if(buffor[0] == '"' && buffor[buffor.size()-1] == '"'){
                    if(!debug){
                        for(unsigned int i = 1; i < buffor.size()-1; i++){
                            cout << buffor[i];
                        }
                    }
                }
                else if(buffor == "endl"){
                    cout << "\n";
                }
                else{
                    argHead = 0;
                    for(i = 0; i < 10; i++){
                        argv[i] = "";
                    }
                    for(i = 0; i < buffor.size(); i++){
                        if(buffor[i] == ' '){
                            if(argv[argHead] == "")
                                continue;
                            argHead++;
                        }
                        else
                            argv[argHead] += buffor[i];
                    }
                    if(argv[0] == "set" || argv[0] == "add" || argv[0] == "sub" || argv[0] == "mul" || argv[0] == "div"){
                        if(argv[1] != "" && argv[2] != "" && regIndex(argv[1]) != -1){
                            if(regIndex(argv[2]) != -1){
                                if(argv[0] == "set")
                                    Files[fileStack].reg[regIndex(argv[1])] = Files[fileStack].reg[regIndex(argv[2])];
                                else if(argv[0] == "add")
                                    Files[fileStack].reg[regIndex(argv[1])] += Files[fileStack].reg[regIndex(argv[2])];
                                else if(argv[0] == "sub")
                                    Files[fileStack].reg[regIndex(argv[1])] -= Files[fileStack].reg[regIndex(argv[2])];
                                else if(argv[0] == "mul")
                                    Files[fileStack].reg[regIndex(argv[1])] *= Files[fileStack].reg[regIndex(argv[2])];
                                else if(argv[0] == "div")
                                    Files[fileStack].reg[regIndex(argv[1])] /= Files[fileStack].reg[regIndex(argv[2])];
                            }
                            else if(sRegIndex(argv[2]) != -1){
                                if(argv[0] == "set")
                                    Files[fileStack].reg[regIndex(argv[1])] = sReg[sRegIndex(argv[2])];
                                else if(argv[0] == "add")
                                    Files[fileStack].reg[regIndex(argv[1])] += sReg[sRegIndex(argv[2])];
                                else if(argv[0] == "sub")
                                    Files[fileStack].reg[regIndex(argv[1])] -= sReg[sRegIndex(argv[2])];
                                else if(argv[0] == "mul")
                                    Files[fileStack].reg[regIndex(argv[1])] *= sReg[sRegIndex(argv[2])];
                                else if(argv[0] == "div")
                                    Files[fileStack].reg[regIndex(argv[1])] /= sReg[sRegIndex(argv[2])];
                            }
                            else{
                                if(argv[0] == "set")
                                    Files[fileStack].reg[regIndex(argv[1])] = atoi(argv[2].c_str());
                                else if(argv[0] == "add")
                                    Files[fileStack].reg[regIndex(argv[1])] += atoi(argv[2].c_str());
                                else if(argv[0] == "sub")
                                    Files[fileStack].reg[regIndex(argv[1])] -= atoi(argv[2].c_str());
                                else if(argv[0] == "mul")
                                    Files[fileStack].reg[regIndex(argv[1])] *= atoi(argv[2].c_str());
                                else if(argv[0] == "div")
                                    Files[fileStack].reg[regIndex(argv[1])] /= atoi(argv[2].c_str());
                            }

                        }
                    }
                    else if(argv[0] == "neg"){
                        if(regIndex(argv[1]) != -1){
                            Files[fileStack].reg[regIndex(argv[1])] *= -1;
                        }
                    }
                    else if(argv[0] == "print"){
                        if(regIndex(argv[1]) != -1){
                            cout << Files[fileStack].reg[regIndex(argv[1])];

                        }
                        else if(sRegIndex(argv[1]) == -1){
                            cout << sReg[sRegIndex(argv[1])];
                        }
                        if(!debug)
                            cout << "\n";
                    }
                    else if(argv[0] == "jmp"){
                        if(argv[1] != ""){
                            for(i = 0; i < Files[fileStack].labels.size(); i++){
                                if(argv[1] == Files[fileStack].labels[i].name){
                                    Files[fileStack].head = Files[fileStack].labels[i].line;
                                }
                            }
                        }
                    }
                    else if(argv[0] == "je" || argv[0] == "jge" || argv[0] == "jle" || argv[0] == "jg" || argv[0] == "jl"){
                        if(argv[1] != "" && argv[2] != "" && argv[3] != "" && regIndex(argv[1]) != -1){
                            bool canJump = false;
                            if(regIndex(argv[2]) != -1){
                                temp1 = Files[fileStack].reg[regIndex(argv[2])];
                            }
                            else if(sRegIndex(argv[2]) != -1){
                                temp1 = sReg[sRegIndex(argv[2])];
                            }
                            else{
                                temp1 = atoi(argv[2].c_str());
                            }
                            if(Files[fileStack].reg[regIndex(argv[1])] == temp1 && argv[0] == "je"){
                                canJump = true;
                            }
                            if(Files[fileStack].reg[regIndex(argv[1])] >= temp1 && argv[0] == "jge"){
                                canJump = true;
                            }
                            if(Files[fileStack].reg[regIndex(argv[1])] <= temp1 && argv[0] == "jle"){
                                canJump = true;
                            }
                            if(Files[fileStack].reg[regIndex(argv[1])] > temp1 && argv[0] == "jg"){
                                canJump = true;
                            }
                            if(Files[fileStack].reg[regIndex(argv[1])] < temp1 && argv[0] == "jl"){
                                canJump = true;
                            }
                            if(canJump){
                                for(i = 0; i < Files[fileStack].labels.size(); i++){
                                    if(argv[3] == Files[fileStack].labels[i].name){
                                        Files[fileStack].head = Files[fileStack].labels[i].line;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if(argv[0] == "jex" || argv[0] == "jnex"){
                        if(argv[1] != "" && argv[2] != ""){
                            ifstream test(argv[1]+".txt");
                            if((test.good() && argv[0] == "jex") || (!test.good() && argv[0] == "jnex")){
                                for(i = 0; i < Files[fileStack].labels.size(); i++){
                                    if(argv[2] == Files[fileStack].labels[i].name){
                                        Files[fileStack].head = Files[fileStack].labels[i].line;
                                        break;
                                    }
                                }
                            }
                            test.close();
                        }
                    }
                    else if(argv[0] == "input"){
                        if(argv[1] != "" && regIndex(argv[1]) != -1){
                            cin.ignore();
                            cin>>buffor2;
                            Files[fileStack].reg[regIndex(argv[1])] = atoi(buffor2.c_str());
                            cin.clear();
                        }
                    }
                    else if(argv[0] == "open"){
                        if(argv[1] != ""){
                            Files.push_back(File());
                            Files[Files.size()-1].descriptor.open(argv[1]+".txt");
                            if(!Files[Files.size()-1].descriptor){
                                Files.pop_back();
                                cout << "SHIT\n";
                                continue;
                            }
                            flag = 2;
                            Files[fileStack].head++;
                            Files[Files.size()-1].load();
                            fileStack += 2;
                            break;
                        }
                    }
                    else if(argv[0] == "write"){
                        if(argv[1] != ""){
                            writeOnlyFile.open(argv[1]+".txt", ios::app);
                            if(writeOnlyFile.good()){
                                flag = 1;
                            }
                        }
                    }
                    else if(argv[0] == "rewrite"){
                        if(argv[1] != ""){
                            writeOnlyFile.open(argv[1]+".txt");
                            if(writeOnlyFile.good()){
                                flag = 1;
                            }
                        }
                    }
                    else if(argv[0] == "remove"){
                        if(argv[1] != ""){
                            remove((argv[1]+".txt").c_str());
                        }
                    }
                }
            }
        }
        if(flag != 2){
            Files.pop_back();
        }
        else{
            flag = 0;
        }
    }

    system("pause");
    Files[0].descriptor.close();
    return 0;
}
