#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <time.h>

using namespace std;

struct bootStruct{
    string fileName;
    bool exitProgram;
    bool debug;
    int debugDelay;
    int error;
};

bootStruct bootLoop(){
    cin.clear();

    string buffer;
    vector <string> argv;

    bootStruct bootU;
    bootU.fileName = "";
    bootU.exitProgram = false;
    bootU.debug = false;
    bootU.debugDelay = 200;
    bootU.error = 0;

    do{
        cout << ">>> ";

        getline(cin, buffer);

        argv.clear();
        argv.push_back("");

        for(unsigned int i = 0; i < buffer.size(); i++){
            if(buffer[i] == ' '){
                if(argv.back() == "")
                    continue;
                argv.push_back("");
            }
            else
                argv.back() += buffer[i];
        }
        if(argv.back() == "")
            argv.pop_back();

        if(argv[0] == "open" && argv.size() == 2){
            bootU.fileName = argv[1];
            return bootU;
        }
        else if(argv[0] == "debug" && argv.size() >= 2){
            bootU.debug = true;
            bootU.fileName = argv[1];
            if(argv.size() == 3){
                bootU.debugDelay = atoi(argv[2].c_str());
                cout << "Delay: " << bootU.debugDelay << "\n";
            }
            return bootU;
        }
        else if(argv[0] == "exit" && argv.size() == 1){
            bootU.exitProgram = true;
            return bootU;
        }
        else if(argv[0] == "clear" && argv.size() == 1){
            system("cls");
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
    unsigned int head;
    vector <string> fileLines;
    vector <label> labels;
    vector <label> tokens;
    int reg[4];
    void load(){
        head = 0;
        string labelName = "";
        fileLines.clear();
        labels.clear();
        tokens.clear();
        while(!descriptor.eof()){
            fileLines.push_back(string());
            getline(descriptor, fileLines[fileLines.size()-1]);
            labelName = "";
            for(unsigned int i = 0; i < fileLines[fileLines.size()-1].size(); i++){
                if(fileLines[fileLines.size()-1][i] == '"'){
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
    if(reg == "e" || reg == "E" || reg == "sreg1" || reg == "sr1")
        return 0;
    if(reg == "f" || reg == "F" || reg == "sreg2" || reg == "sr2")
        return 1;
    if(reg == "g" || reg == "G" || reg == "sreg3" || reg == "sr3")
        return 2;
    if(reg == "h" || reg == "H" || reg == "sreg4" || reg == "sr4")
        return 3;
    return -1;
}

int parser(bootStruct bootU){

    vector <File> Files;
    string buffer, buffer2;
    int temp1;
    int sReg[4]; //system registers, from user's perspective read only

    vector <string> argv;
    unsigned int i;
    bool debug;
    int flag;
    bool noEndlDebug;
    ofstream writeOnlyFile;

    Files.push_back(File());
    Files[0].descriptor.open(bootU.fileName+".txt");
    if(!Files[0].descriptor){
        cout << "Problem with the file!\n";
        return -2;
    }
    Files[0].load();
    debug = bootU.debug;
    flag = 0;
    noEndlDebug = false;

    bool dollarSignFound = false;
    bool artificialJump = false;

    for(int fileStack = 0; fileStack > -1; fileStack--){
        for(Files[fileStack].head; Files[fileStack].head < Files[fileStack].fileLines.size(); Files[fileStack].head++){
            if(!artificialJump)
                buffer = Files[fileStack].fileLines[Files[fileStack].head];
            else
                artificialJump = false;
            if(buffer == "end write"){
                flag = 0;
                if(writeOnlyFile)
                    writeOnlyFile.close();
            }

            if(flag == 1){
                if(debug){
                    cout << "\n[writing]" << fileStack << "|"<< Files[fileStack].head << "> " << buffer;
                    Sleep(bootU.debugDelay);
                }
                if(buffer.find("$") != std::string::npos){
                    string dolar = "";
                    for(auto chara : buffer){
                        if(chara != '$' && !dollarSignFound){
                            writeOnlyFile << chara;
                        }
                        else if(dollarSignFound){
                            if(chara != ' ' && chara != '"'){
                                dolar += chara;
                            }
                            else{
                                if(regIndex(dolar) != -1)
                                    writeOnlyFile << Files[fileStack].reg[regIndex(dolar)];
                                if(sRegIndex(dolar) != -1)
                                    writeOnlyFile << sReg[sRegIndex(dolar)];
                                writeOnlyFile << chara;
                                dollarSignFound = false;
                                dolar = "";
                            }
                        }
                        else{
                            dollarSignFound = true;
                            continue;
                        }
                    }
                    if(dollarSignFound){
                        if(regIndex(dolar) != -1)
                            writeOnlyFile << Files[fileStack].reg[regIndex(dolar)];
                        if(sRegIndex(dolar) != -1)
                            writeOnlyFile << sReg[sRegIndex(dolar)];
                        dollarSignFound = false;
                    }
                }
                else{
                    writeOnlyFile << buffer;
                }
                writeOnlyFile << "\n";
                continue;
            }

            if(debug){
                if(!noEndlDebug){
                    cout << "\n";
                }
                else{
                    noEndlDebug = false;
                }
                cout << fileStack << "|"<< Files[fileStack].head << "> " << buffer << ":";
                Sleep(200);
            }


            argv.clear();
            argv.push_back("");

            for(i = 0; i < buffer.size(); i++){
                if(buffer[i] == ' '){
                    if(argv.back() == "")
                        continue;
                    argv.push_back("");
                }
                else
                    argv.back() += buffer[i];
            }

            if(argv.back() == "")
                argv.pop_back();


            if(argv[0] == "return"){
                for(i = 0; i < 4; i++){
                    sReg[i] = Files[fileStack].reg[i];
                }
                break;
            }
            else if(buffer[0] == '"' && buffer[buffer.size()-1] == '"'){
                if(!debug){
                    for(unsigned int i = 1; i < buffer.size()-1; i++){
                        cout << buffer[i];
                    }
                }
            }
            else if(argv[0] == "endl"){
                if(!debug)
                    cout << "\n";
            }
            else if(argv[0] == "set" || argv[0] == "add" || argv[0] == "sub" || argv[0] == "mul" || argv[0] == "div"){
                if(argv.size() >= 3 && regIndex(argv[1]) != -1){
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
                else if(sRegIndex(argv[1]) != -1){
                    cout << sReg[sRegIndex(argv[1])];
                }
            }
            else if(argv[0] == "jmpt"){
                if(Files[fileStack].tokens.size() > 0){
                    Files[fileStack].head = Files[fileStack].tokens.back().line;
                    Files[fileStack].tokens.pop_back();
                }
            }
            else if(argv[0] == "jmp"){
                if(argv.size() >= 2){
                    for(i = 0; i < Files[fileStack].labels.size(); i++){
                        if(argv[1] == Files[fileStack].labels[i].name){
                            if(argv.size() >= 3){
                                if(argv[2] == "t" || argv[2] == "token"){
                                    Files[fileStack].tokens.push_back(label());
                                    Files[fileStack].tokens.back().line = Files[fileStack].head;
                                    Files[fileStack].tokens.back().name = argv[1];
                                }
                            }
                            Files[fileStack].head = Files[fileStack].labels[i].line;
                        }
                    }
                }
            }
            else if(argv[0] == "je" || argv[0] == "jge" || argv[0] == "jle" || argv[0] == "jg" || argv[0] == "jl"){
                if(argv.size() >= 4 && regIndex(argv[1]) != -1){
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
                                if(argv.size() >= 5){
                                    if(argv[4] == "t" || argv[4] == "token"){
                                        Files[fileStack].tokens.push_back(label());
                                        Files[fileStack].tokens.back().line = Files[fileStack].head;
                                        Files[fileStack].tokens.back().name = argv[3];
                                    }
                                }
                                Files[fileStack].head = Files[fileStack].labels[i].line;
                                break;
                            }
                        }
                    }
                }
            }
            else if(argv[0] == "jex" || argv[0] == "jnex"){
                if(argv.size() >= 3){
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
                if(argv.size() >= 2 && regIndex(argv[1]) != -1){
                    cin >> buffer2;
                    Files[fileStack].reg[regIndex(argv[1])] = atoi(buffer2.c_str());
                    cin.clear();
                    cin.ignore();
                    if(debug){
                        noEndlDebug = true;
                    }
                }
            }
            else if(argv[0] == "rand"){
                if(argv.size() >= 2 && regIndex(argv[1]) != -1){
                    Files[fileStack].reg[regIndex(argv[1])] = rand();
                }
            }
            else if(argv[0] == "open"){
                if(argv.size() >= 2){
                    Files.push_back(File());
                    Files.back().descriptor.open(argv[1]+".txt");
                    if(!Files.back().descriptor){
                        Files.pop_back();
                        continue;
                    }
                    flag = 2;
                    Files[fileStack].head++;
                    Files.back().load();
                    fileStack += 2;
                    if(argv.size() >= 3){
                        buffer = "jmp " + argv[2];
                        artificialJump = true;
                    }
                    
                    break;
                }
            }
            else if(argv[0] == "write"){
                if(argv.size() >= 2){
                    writeOnlyFile.open(argv[1]+".txt", ios::app);
                    if(writeOnlyFile.good()){
                        flag = 1;
                    }
                }
            }
            else if(argv[0] == "rewrite"){
                if(argv.size() >= 2){
                    writeOnlyFile.open(argv[1]+".txt");
                    if(writeOnlyFile.good()){
                        flag = 1;
                    }
                }
            }
            else if(argv[0] == "remove"){
                if(argv.size() >= 2){
                    remove((argv[1]+".txt").c_str());
                }
            }
            else if(argv[0] == "sleep"){
                if(argv.size() >= 2){
                    if(regIndex(argv[1]) != -1){
                        Sleep(Files[fileStack].reg[regIndex(argv[1])]);
                    }
                    else if(sRegIndex(argv[1]) != -1){
                        Sleep(sReg[sRegIndex(argv[1])]);
                    }
                    else{
                        Sleep(atoi(argv[1].c_str()));
                    }

                }
            }
            else if(argv[0] == "clear"){
                system("cls");
            }
        }
        if(flag != 2){
            Files.back().descriptor.close();
            Files.pop_back();
        }
        else{
            flag = 0;
        }
    }
    return -1;
}

int main()
{
    srand(time(NULL));

    bootStruct bootU;
    while(1){
        bootU = bootLoop();

        if(bootU.exitProgram == true){
            cout << "Exiting...\n";
            return 0;

        }
        if(bootU.error != 0 || bootU.fileName == ""){
            cout << "There's a problem!\n";
            return bootU.error;
        }

        parser(bootU);
        cout << "\n";
    }

    system("pause");

    return 0;
}