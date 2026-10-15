/*Name: Anurag Nath
Roll No: 2301CS07*/

#include <bits/stdc++.h>
using namespace std;

// This function converts the decimal to hex and returns tring
std::string dechex(int num)
{
    std::string res;
    for (int i = 0; i < 8; i++, num >>= 4)
        res += "0123456789abcdef"[num & 0xF];
    std::reverse(res.begin(), res.end());
    return res;
}

// this array stores the array content
int mem[20000005];
int pc, sp, a, b;         // stores the value of 4 registers
vector<int> machinecodes; // stores the machinecodes
struct reading
{
    int add;
    int value;
};
vector<reading> read1; // stores memory read

struct writing
{
    int add;
    int previous;
    int present;
};
vector<writing> write1; // stores memory write
// Follwing function traces the .o file whole and last tells total instruction or segmentation fault
void trace(int siz)
{
    pc = 0;                         // Initialize program counter (PC) to the start.
    int cnt = 0;                    // Instruction count for tracking the number of executed instructions.
    bool chk = false, chk1 = false; // Flags for termination conditions.

    // Main execution loop for tracing instructions until PC exceeds program size.
    while (pc < siz)
    {
        int instr = machinecodes[pc];         // Fetch the current instruction from machine code.
        int type = instr & 0xFF;              // Extract opcode (lower 8 bits).
        int offset = (instr >> 8) & 0x7FFFFF; // Extract offset (next 23 bits).

        // Sign extend the offset if the 31st bit indicates a negative value.
        if (instr & (1 << 31))
            offset -= (1 << 23);

        int prev = -1; // Variable to store previous memory value for write tracking.

        // Decode and execute based on opcode type.
        if (type == 0) // Type 0: Load immediate value.
        {
            b = a;
            a = offset;
        }
        else if (type == 1) // Type 1: Add offset to accumulator.
        {
            a += offset;
        }
        else if (type == 2) // Type 2: Load from memory using stack pointer and offset.
        {
            b = a;
            a = mem[sp + offset];
            read1.push_back({sp + offset, a}); // Log the memory read operation.
        }
        else if (type == 3) // Type 3: Store accumulator to memory at stack pointer offset.
        {
            prev = mem[sp + offset];
            mem[sp + offset] = a;
            a = b;
            write1.push_back({sp + offset, prev, mem[sp + offset]}); // Log the memory write operation.
        }
        else if (type == 4) // Type 4: Load from memory using accumulator and offset.
        {
            prev = a;
            a = mem[a + offset];
            read1.push_back({prev + offset, a}); // Log the memory read operation.
        }
        else if (type == 5) // Type 5: Store `b` to memory at address `a + offset`.
        {
            prev = mem[a + offset];
            mem[a + offset] = b;
            write1.push_back({a + offset, prev, b}); // Log the memory write operation.
        }
        else if (type == 6) // Type 6: Add `b` to `a`.
        {
            a = b + a;
        }
        else if (type == 7) // Type 7: Subtract `a` from `b`.
        {
            a = b - a;
        }
        else if (type == 8) // Type 8: Shift `b` left by `a` bits.
        {
            a = b << a;
        }
        else if (type == 9) // Type 9: Shift `b` right by `a` bits.
        {
            a = b >> a;
        }
        else if (type == 10) // Type 10: Adjust stack pointer by offset.
        {
            sp += offset;
        }
        else if (type == 11) // Type 11: Load stack pointer into `a` and restore `b`.
        {
            sp = a;
            a = b;
        }
        else if (type == 12) // Type 12: Move `sp` to `a`, store `a` in `b`.
        {
            b = a;
            a = sp;
        }
        else if (type == 13) // Type 13: Jump to offset while saving `pc` in `a`.
        {
            b = a;
            a = pc;
            pc += offset;
        }
        else if (type == 14) // Type 14: Unconditional jump to address in `a`.
        {
            pc = a;
            a = b;
        }
        else if (type == 15) // Type 15: Conditional jump (if `a == 0`) by offset.
        {
            if (a == 0)
            {
                pc += offset;
            }
        }
        else if (type == 16) // Type 16: Conditional jump (if `a < 0`) by offset.
        {
            if (a < 0)
            {
                pc += offset;
            }
        }
        else if (type == 17) // Type 17: Unconditional jump by offset.
        {
            pc += offset;
        }
        else if (type == 18) // Type 18: Set `chk` flag to terminate.
        {
            chk = true;
        }

        pc++; // Increment program counter to next instruction.

        // Display current status of PC, SP, A, and B registers.
        std::cout << "PC= " << dechex(pc) << " SP= " << dechex(sp)
                  << " A= " << dechex(a) << " B= " << dechex(b) << "\n";

        // Check for potential errors or looping conditions.
        if (pc < 0 || cnt > (1 << 24)) // Ensure PC is valid and prevent infinite loops.
        {
            chk1 = true;
            break;
        }

        cnt++; // Increment instruction count.

        if (chk) // Terminate if `chk` flag is set.
            break;
    }

    // Handle error or abnormal termination.
    if (chk1)
    {
        std::cout << "Segmentation fault or some other error\n";
        exit(0);
    }

    // Display total number of executed instructions.
    std::cout << "Total instructions ran are\n"
              << cnt << "\n";
}

vector<string> all_possible_commands; // vector for storing all the instruction set commands

// Function for storing all the instruction set commands when needed
void init()
{
    ifstream file("command.txt");
    string line;
    while (getline(file, line))
    {
        all_possible_commands.push_back(line);
    }
    file.close();
}

// Following function displays all possible commands
void showisa()
{
    for (auto &val : all_possible_commands)
        cout << val << endl;
}

int32_t main()
{
    init(); // Initialize emulator environment and settings.

    std::string nameoffile; // File name for the input machine code file.
    std::cout << "Enter the File name for which you want to run emulator (with extension):\n";
    std::cin >> nameoffile;

    // Open the specified file in binary read mode.
    std::ifstream file(nameoffile, std::ios::in | std::ios::binary);
    unsigned int cur; // Variable to store each machine code instruction temporarily.
    int counter = 0;  // Counter to track memory index for loading machine codes.

    // Load machine codes from the file into memory and store in `machinecodes` vector.
    while (file.read(reinterpret_cast<char *>(&cur), sizeof(int)))
    {
        machinecodes.push_back(cur); // Store the machine code in vector.
        mem[counter++] = cur;        // Load the machine code into memory.
    }
    int siz = machinecodes.size(); // Size of loaded machine codes.

    // Menu-driven interface for user interaction.
    while (true)
    {
        std::cout << "MENU\n1: Trace\n2: Display ISA\n3: Reset Flags\n"
                  << "4: Show Memory Dump (before)\n5: Show Memory Dump (after)\n"
                  << "6: Show Memory Reads\n7: Show Memory Writes\n8+: Exit\n\n"
                  << "Choose an option:\n";

        bool flag = true; // Flag to track if menu should continue or exit.

        int type;
        std::cin >> type; // Get the user's choice.

        switch (type)
        {
        case 1:
            // Trace the execution of instructions loaded in memory.
            trace(siz);
            break;
        case 2:
            // Display the Instruction Set Architecture (ISA) for reference.
            showisa();
            break;
        case 3:
            // Reset key emulator registers (flags).
            a = b = pc = sp = 0;
            break;
        case 4:
            // Display a memory dump of initial machine codes (before execution).
            std::cout << "Memory dump before execution:\n";
            for (int i = 0; i < siz; i += 4) // Display 4 machine code instructions per line.
            {
                std::cout << dechex(i) << " "; // Print memory address in hexadecimal.
                for (int j = i; j < std::min(siz, i + 4); ++j)
                {
                    std::cout << dechex(machinecodes[j]) << " "; // Print machine code in hexadecimal.
                }
                std::cout << "\n";
            }
            break;
        case 5:
            // Display a memory dump after execution showing modified memory.
            std::cout << "Memory dump after execution:\n";
            for (int i = 0; i < siz; i += 4) // Display 4 memory entries per line.
            {
                std::cout << dechex(i) << " "; // Print memory address in hexadecimal.
                for (int j = i; j < std::min(siz, i + 4); ++j)
                {
                    std::cout << dechex(mem[j]) << " "; // Print current memory content in hexadecimal.
                }
                std::cout << "\n";
            }
            break;
        case 6:
            // Display all memory reads performed during execution.
            for (const auto &read : read1)
            {
                std::cout << "Read memory[" << dechex(read.add) << "] found " << dechex(read.value) << "\n";
            }
            break;
        case 7:
            // Display all memory writes performed during execution.
            for (const auto &write : write1)
            {
                std::cout << "Wrote memory[" << dechex(write.add) << "] was " << dechex(write.previous)
                          << " now " << dechex(write.present) << "\n";
            }
            break;

        default:
            // Exit if the option is outside valid menu choices (e.g., 8+).
            flag = false;
            break;
        }

        if (!flag) // Break the loop if the user chooses to exit.
            break;
    }

    return 0; // End of the program.
}