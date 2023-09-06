/*
* Name: Kaitlynn Pham 
* Description: The program will build an assember and
interpreter by reading assembly language from an input file 
and storing the labels and instructions into memory. It will then
 simulate the instructions. 
* Input: input files 
* Output: It will create an output file that removed comments and
the first column. It will display the results of the instructions given. 
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
// Global constant variables

const int LABEL_COUNT = 100; ///< default number of labels
const int OPCODE_COUNT = 18; ///< default number of opcodes
const int MAX_CHARS = 5;     ///< maximum number of characters
const int STACK_SIZE = 1000; ///< maximum capacity of array
///< list of opcodes needed for interpretation
const std::string OPCODE_LIST = "const get put ld st add sub mul div "
"cmp jpos jz j jl jle jg jge halt"; 


//Structs 
struct Opcode {
    string name;
    int instruction;
};

struct Symbol {
    string name;
    int memory;
};
// Functions 

/*
* splitOpcodes: The function will separate a string into words
and store it into an array of opcodes. 
* parameters: string, Opcode *opcodes
* return value: no return 
*/
void splitOpcodes(string inst, Opcode *opcodes) {
    stringstream ss(inst);
    for (int i = 0; i < OPCODE_COUNT; i++) {
        ss >> opcodes[i].name;
        opcodes[i].instruction = i;
    }
}
/*
* isOpcode: This function will check if a string matches an element
in the array of opcodes. 
* parameters: string&, Opcode *opcodes
* return value: true or false 
*/
bool isOpcode(string &word, Opcode *opcodes) {
    for (int i = 0; i < OPCODE_COUNT; i++) {
        if (word == opcodes[i].name) {
            return true;
        }
    }
    return false;
}
/*
* getOpcodes: This function uses the function isOpcode to check
if the string is found. If it is found, it returns the value that
matches the opcode in the array. 
* parameters: string &,  Opcode *opcodes
* return value: returns an integer if found or -1 if not true
*/
int getOpcode(string &word, Opcode *opcodes) {
    if (isOpcode(word, opcodes) == true) {
        for (int i = 0; i < OPCODE_COUNT; i++) {
            if (word == opcodes[i].name) {
                return opcodes[i].instruction;
            }
        }
    }
    return -1;
}
/*
* isNumber: This function determines if a string represents a 
number. It checks all characters in the string if it is a digit. 
* parameters: string&
* return value: true or false
*/
bool isNumber(string &str) {
    for (int i = 0; i < str.length(); i++)
        if (isdigit(str[i]) == 0) {

            return false;
        }

    return true;
}
/*
* getLocation: This function will get a label's location in memory. 
It will first check if the string matches an element in 
the current array.If it does not, it will check if it is a
 number. It will return -1 if both are false.
* parameters: string &, Symbol *labels, int 
* return value: integer that represents the label or -1 
*/
int getLocation(string &name, Symbol *labels, int nextLabel) {
    for (int i = 0; i < LABEL_COUNT; i++) {
        if (name == labels[i].name) {
            return labels[i].memory;
        }
        if (isNumber(name)) {
            int labelnum = stoi(name);
            return labelnum;
        }
    }
    return -1;
}

// Helper functions

/// Returns the number of spaced needed for padding.
/// @param str the string that needs padding.
/// @param len the maximum number of characters.
/// @return the number of spaces needed.
int padding(std::string str, int len) {
    int n = static_cast<int>(str.length());
    return len + abs(len - n);
}

/// Dumps the name and numerical value that each instrcution represents
/// @param op the array of opcodes.
/// @param count the number of opcodes.
/// @return void.
/*void dumpOpcodes(Opcode *opcodes, int count) {
    std::cout << "Opcodes" << std::endl;
    int pad = 0;
    for (int i = 0; i < count; ++i) {
        pad = padding(opcodes[i].name, MAX_CHARS);
        //  std::cout << "> " 
        << opcodes[i].name << std::string(pad, ' ') 
        << opcodes[i].code << std::endl;
    }
}*/

/// Dumps the name of a label and its corresponding location in memory.
/// @param sym an array of labels.
/// @param count the number of labels in the array.
/// @return void.
/*void dumplabels(Symbol *labels, int count) {
    std::cout << "labels" << std::endl;
    int pad = 0;
    for (int i = 0; i < count; ++i) {
        pad = padding(labels[i].name, MAX_CHARS);
        std::cout << "> ";
        //  << "[" << labels[i].name << "] = " 
        << labels[i].mem << std::endl;
    }
}*/

/// Dumpds the addressmem of each instruction and label.
/// @param memory the array of addresses.
/// @param count the number of addresses.
/// @return void.
void dumpMemory(int *memory, int count) {
    std::cout << "Memory" << std::endl;
    std::cout.fill('0');
    for (int i = 0; i < count; ++i) {
        std::cout << std::setw(8) 
        << memory[i] << std::endl;
    }
}

/// Entry point of the main program.
/// @param argc the number of command-line arguments.
/// @param argv the vector of command-line arguments.
/// @return 0 if the program runs to complition.
int main(int argc, char *argv[]) {
   
    // table of opcodes
    //array of opcode structs
    Opcode opcodes[OPCODE_COUNT];
    // parse the list of opcodes
    splitOpcodes(OPCODE_LIST, opcodes);
   
    //array of labels structs
    Symbol labels[LABEL_COUNT];

    // check command-line arguments

    //fstream to open and write files
    ifstream iFile;
    ofstream oFile;
    // check command-line arguments
    for (unsigned int i = 1; i < argc; i++) {
        string filename = argv[i];
        //open input file
        iFile.open(filename);
        //open output file
        oFile.open("output");
        //check if file opens
        if (!iFile) {
            cout << "break\nERROR: invalid input!\n**"
             "Program terminated with an error code **" << endl;
            return 0;
        }

        //--------------------------------------------
        // PASS #1

        //variables to read file
        string line;
        int row = 0;
        //array for labels in column 3
         string col3[STACK_SIZE];
        //array for instructions in col2 
        string instructions[STACK_SIZE];   
        
        //read each line in file
        while (getline(iFile, line)) {
            string semi = ";";
        // (1) Removing comments
            if (line.find(semi) == 0 || line.length() == 0) {
                continue;
            }
        // line before ;
            line = line.substr(0, line.find(semi));

            stringstream ss(line);
            //if line starts with space
            if (line.find(' ') == 0 || line.find('\t') == 0) {
                ss >> instructions[row];
                ss >> ws;
                if (!ss.eof()) {
                    ss >> col3[row];
                }
            }
            else {
            // (2) Remember labels
            // (3) Save instruction/opcode and addressmem
                ss >> labels[row].name;
                labels[row].memory = row;
                ss >> instructions[row];
                ss >> ws;
                if (!ss.eof()) {
                    ss >> col3[row];
                }
            }
            // (4) Write generated code to an output file
            oFile << instructions[row] 
            << "\t" << col3[row] << '\t' << endl;
            row++;
        }
        //close files 
        iFile.close();
        oFile.close();
    }

    //
    // --------------------------------------------
    // PASS #2
    
    //variables to read output file 
    //array for instructions in col1
    string col1[STACK_SIZE] = {};
    //array for labels in col2 
    string col2[STACK_SIZE] = {};
    //memory array for locations of instructions and labels
    int memory[STACK_SIZE] = {};
    string temp; //# of variables in array 
    int count = 0; //# of variables in array 

    // (1) Read output file
    iFile.open("output");
    while (getline(iFile, temp)) {
        stringstream st(temp);
        st >> col1[count];
        //check if col2 exists 
        if (!st.eof()) {
            st >> ws;
            st >> col2[count];
        }
        count++;
    }
    //close file
    iFile.close();

    // (2) Replace numeric values and pack them into
    //arrays to store memory locations 
    int address[STACK_SIZE] = {};
    int opcodemem[STACK_SIZE] = {};
   
    for (int i = 0; i <= count; i++) {
        //opcode memory locations
        opcodemem[i] = getOpcode(col1[i], opcodes);
        memory[i] = STACK_SIZE * opcodemem[i] + address[i];
        //check if col2 is empty, label address=0
        if (col2[i].length() == 0) {
            continue;
        }
        //save label address
        else {
            address[i] = getLocation(col2[i], labels, LABEL_COUNT);
            memory[i] = STACK_SIZE * opcodemem[i] + address[i];
        }
    }

    // --------------------------------------------
    //
    // INTEPRETER
    //
    std::cout << "Running program..." << std::endl;
    int reg = 0; // register variable
    int ip = 0; //instruction pointer variable
    int input = 0; //input from user variable 
    int addressmem = 0; 
    int instructmem = 0;
    // -->

    do {
        //interpret code 
        addressmem = memory[ip] % STACK_SIZE;//label address
        instructmem = memory[ip] / STACK_SIZE;//opcode value 

        // (1) Execute instructions in sequential order
        switch (instructmem) {
        //const
        case 0:
            break;
        //get: get input from user 
        case 1:
            cin >> input;
                //terminate program if error 
                if (cin.fail()) {
                    cout << "break\nERROR: invalid input!\n**" 
                    "Program terminated with an error code **" << endl;
                    cin.clear();
                    cin.ignore(256, '\n');
                    return 0; 
                }else{
                    cout << "read: " << input << endl;
                    reg = input;
                }
        break;
        // put : write output 
        case 2:
            cout << "result: " << reg << endl;
            break;
        // ld : load register 
        case 3:
            reg = address[addressmem];
            break;
        // st : store to register
        case 4:
            address[addressmem] = reg;
            break;
        // add 
        case 5:
            reg = reg + address[addressmem];
            break;
        // sub
        case 6:
            reg = reg - address[addressmem];
            break;
        // mul
        case 7:
            reg = reg * address[addressmem];
            break;
        // div
        case 8:
            if (reg==0){
                ip =-1;
            }else{
                 reg = address[addressmem] /reg;
            }
        // cmp :compare 
        case 9:
            reg = reg - address[addressmem];
            break;
        // jpos: jump if pos
        case 10:
            if (reg > 0) {
                ip = addressmem;
                continue;
            }
            break;
        // jz: jump if zero 
        case 11:
            if (reg == 0) {
                ip = addressmem;
                continue;
            }
            break;
        //j : jump
        case 12:
            ip = addressmem;
            continue;
        // jl: jump if <0
        case 13:
            if (reg < 0) {
                ip = addressmem;
                continue;
            }
            break;
        // jle: jump if <=0
        case 14:
            if (reg <= 0) {
                ip = addressmem;
                continue;
            }
            break;
        // jg : jump if >0
        case 15:
            if (reg > 0)  {
                ip = addressmem;
                continue; 
            }
            break;
        // jge: jump if >=0
        case 16:
            if (reg >= 0) {
                ip = addressmem;
                continue; 
            }
            break;
        // halt
        case 17:
            ip = -1;
            continue;
        default:
            ip = -1;
        }
        ip++;
    } while (ip >= 0);
    cout <<"** Program terminated **"<< endl;
    return EXIT_SUCCESS;
}
