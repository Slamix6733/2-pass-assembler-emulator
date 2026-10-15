/*Name : Anurag Nath
Roll No: 2301CS07
*/

#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace __gnu_pbds;

typedef tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> oset;

#define fast                          \
    ios_base::sync_with_stdio(false); \
    cin.tie(NULL);                    \
    cout.tie(NULL);
// #define int long long
#define pb push_back
#define pii pair<int, int>
#define vi vector<int>
#define all(x) (x).begin(), (x).end()
#define fr(i, n) for (int i = 1; i <= n; i++)

struct listing
{
    string prctr, mcode, text;
};
map<int, string> warnings;                              // Data structure for displaying the warnings if the code is error-free
vector<string> inputlines;                              // To get line by line from the file
map<string, vi> label_occ;                              // To check at which lines label is used as an operand
map<string, pair<string, string>> opcodes;              // To store the instruction set in an archivable format
map<string, pair<int, int>> labelCatalog;               // To get the points at which labels are initialised or last used
map<int, string> errors;                                // To write the errors in an appropriate format
vector<string> machine_code;                            // To store the hex values which have to be written into the object file
map<int, pair<string, pair<string, string>>> line_info; // A map which contains information about the labels,machine code and operand used in each line
vector<listing> listi;                                  // To store the instructions at which program counter is being updated
map<string, string> setvar;                             // Used in handling the set instructions
string file_name;                                       // String for taking the file name as input
int file_size;                                          // Variable for storing the size of the name of the file

// Function for initializing the opcode table by reading through a file
void init()
{
    ifstream file("data.txt");
    string line;
    opcodes["data"] = {"", "value"};
    opcodes["SET"] = {"", "value"};
    string s = "", s1 = "", s2 = "";
    while (getline(file, line))
    {
        istringstream iss(line);
        string value;
        while (iss >> value)
        {
            if (s == "")
                s = value;
            else if (s1 == "")
                s1 = value;
            else
                s2 = value;
        }
        opcodes[s] = {s1, s2};
        s = s1 = s2 = "";
    }
    file.close();
}

// Checks if the entered operand or label is fit to be that for the program
class checker
{
public:
    // Checks if a character is a numeric digit (0-9).
    bool digit(char c)
    {
        if (c >= '0' && c <= '9')
            return true;
        return false;
    }

    // Checks if a character is an alphabetic character (a-z, A-Z) or an underscore (_).
    bool alphabet(char c)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
            return true;
        else
            return false;
    }

    // Checks if a given string represents a valid decimal number.
    // A valid decimal:
    // - Cannot start with '0' if it has more than one digit.
    // - Must contain only digits (0-9).
    bool decimal(string s)
    {
        if (!s.size() || (s.size() > 1 && s[0] == '0')) // Invalid if empty or has leading '0' with more than one digit.
            return false;
        int n = s.size() - 1;
        for (int i = 0; i <= n; i++) // Loop through each character.
        {
            if (!(s[i] >= '0' && s[i] <= '9')) // Check if character is not a digit.
                return false;
        }
        return true; // All characters are digits.
    }

    // Checks if a given string represents a valid octal number.
    // A valid octal:
    // - Must start with '0'.
    // - Must only contain digits from 0 to 7.
    bool octal(string s)
    {
        int n = s.size() - 1;
        if (n < 1 || s[0] != '0') // Invalid if length < 2 or does not start with '0'.
            return false;
        for (int i = 0; i <= n; i++) // Loop through each character.
        {
            if (!(s[i] >= '0' && s[i] <= '7')) // Check if character is outside 0-7 range.
                return false;
        }
        return true; // All characters are valid octal digits.
    }

    // Checks if a given string represents a valid hexadecimal number.
    // A valid hexadecimal:
    // - Must start with "0x" or "0X".
    // - Can contain digits (0-9) and letters (a-f, A-F) after "0x".
    bool hexadecimal(string s)
    {
        int n = s.size();
        if (s.size() < 3 || s[0] != '0' || s[1] != 'x') // Invalid if size < 3 or does not start with "0x".
            return false;
        for (int i = 2; i < n; ++i) // Loop through each character after "0x".
        {
            if ((s[i] >= '0' && s[i] <= '9') || (tolower(s[i]) >= 'a' && tolower(s[i]) <= 'f'))
                continue; // Valid hexadecimal character.
            else
                return false; // Invalid character found.
        }
        return true; // All characters are valid hexadecimal digits.
    }

    // Checks if a given string is a valid label.
    // A valid label:
    // - Must start with an alphabetic character or underscore.
    // - Can contain alphabetic characters, digits, or underscores.
    bool label(string s)
    {
        int n = s.size();
        if (!n || !alphabet(s[0])) // Invalid if empty or does not start with an alphabetic character.
            return false;
        for (int i = 1; i < n; i++) // Loop through each character after the first.
        {
            if (!alphabet(s[i]) && !digit(s[i])) // Check if character is neither alphabetic nor a digit.
                return false;
        }
        return true; // All characters are valid label characters.
    }
} check;

class Converter
{
public:
    string octdec(string s)
    {
        int ans = 0;
        fr(i, s.size() - 1)
        {
            ans = ans * 8 + (s[i] - '0');
        }
        return to_string(ans);
    }

    string hexdec(string s)
    {
        int n = s.size() - 1;
        int ans = 0;
        for (int i = 0; i <= n; ++i)
        {
            ans *= 16;
            if (s[i] >= '0' && s[i] <= '9')
            {
                ans += (s[i] - '0');
            }
            else
                ans += ((tolower(s[i]) - 'a') + 10);
        }
        return to_string(ans);
    }

    string dechex(int num)
    {
        string res;
        for (int i = 0; i < 8; i++, num >>= 4)
            res += "0123456789abcdef"[num & 0xF];
        reverse(res.begin(), res.end());
        return res;
    }
} conv;

// Set of functions for transforming the input line into suitable form and processses them into the respective elements
class solver
{
public:
    // Validates and stores a label if it meets naming conventions.
    // Parameters:
    // - lab: The label to validate.
    // - prct: A percentage or address associated with the label.
    // - line_num: The line number where the label is defined.
    void validateAndStoreLabel(string lab, int prct, int line_num)
    {
        // Check if the label name is valid according to syntax rules.
        if (!check.label(lab))
        {
            errors[line_num] = "Invalid name for a label"; // Error if the label is not valid.
        }
        // Check if the label already exists with a valid entry in the label catalog.
        else if (labelCatalog.find(lab) != labelCatalog.end() && labelCatalog[lab].first != -1)
        {
            errors[line_num] = "Label name in " + to_string(line_num) + " is already used."; // Error if duplicate.
        }
        else
        {
            // If valid and unique, store the label with its associated address and line number.
            labelCatalog[lab] = {prct, line_num};
        }
    }

    // Converts an operand to its decimal representation if it is in octal or hexadecimal.
    // Parameters:
    // - s: The operand string to convert.
    // Returns: The decimal representation as a string, or the original operand if already in decimal format.
    string convertOperandToDecimal(string s)
    {
        string ans = "";
        if (check.octal(s))                 // Check if the operand is in octal format.
            ans = conv.octdec(s);           // Convert octal to decimal.
        else if (check.hexadecimal(s))      // Check if the operand is in hexadecimal format.
            ans = conv.hexdec(s.substr(2)); // Convert hexadecimal to decimal, skipping "0x" prefix.
        else
            ans = s; // Assume the operand is already in decimal format.
        return ans;
    }

    // Processes a mnemonic and its operand, performing validation and conversion if necessary.
    // Parameters:
    // - mnemonic: The mnemonic instruction to process.
    // - operand: The operand associated with the mnemonic.
    // - prct: A percentage or address associated with the mnemonic.
    // - line_num: The line number where the instruction occurs.
    // Returns: True if processing is successful, otherwise false with error logged.
    bool processMnemonicWithOperand(string &mnemonic, string &operand, int prct, int line_num)
    {
        string kind = opcodes[mnemonic].second; // Identify the type of operand expected for the mnemonic.
        int size_of_operand = operand.size();

        // Check if the mnemonic expects an operand.
        if (kind != "")
        {
            string ans = "";
            if (check.label(operand)) // If the operand is a label.
            {
                // Check if label already exists; if not, add it as a placeholder.
                if (labelCatalog.find(operand) == labelCatalog.end())
                    labelCatalog[operand] = {-1, line_num}; // Placeholder with unknown address.
                label_occ[operand].pb(line_num);            // Track line where the label is used.
                ans += operand;                             // Append label to result.
            }
            else // Process operand as a numeric value.
            {
                if (operand[0] == '+' || operand[0] == '-') // If operand has a sign.
                {
                    ans += operand[0];                  // Append the sign.
                    string s1 = operand.substr(1);      // Strip the sign for conversion.
                    ans += convertOperandToDecimal(s1); // Convert the remaining part to decimal.
                }
                else
                {
                    ans += convertOperandToDecimal(operand); // Convert operand directly to decimal.
                }
            }
            // Validate the final operand format.
            if (!ans.size() || (ans.size() == 1 && !check.digit(ans[0])))
            {
                errors[line_num] = "Invalid Format"; // Error if format is invalid.
                return false;
            }
            else
            {
                operand = ans; // Update operand with validated and converted result.
                return true;
            }
        }
        else if (kind == "" && size_of_operand) // Check if an operand was given but is unexpected.
        {
            errors[line_num] = "Unexpected operand"; // Error if operand is provided but not expected.
            return false;
        }
        return true; // Return true if no issues encountered.
    }
} sol;

// Function for extracting out the words present in a line
vector<string> parseLine(string line)
{
    int n = line.size();
    if (!n)
        return {};
    //   cout << n << endl;
    string word;
    vector<string> words;
    stringstream ss(line);
    while (ss >> word)
    {
        // cout << word << " ";
        int word_size = word.size();
        if (!word_size)
            continue;
        if (word[0] == ';')
            break;
        if (find(all(word), ':') != word.end() && word.back() != ':') // If the colon occurs somewhere in between the string
        {
            int i = find(all(word), ':') - word.begin();
            words.pb(word.substr(0, i + 1));
            word = word.substr(i + 1);
        }
        if (word.back() == ';')
        {
            word.pop_back();
            words.push_back(word);
            break;
        }
        words.push_back(word);
    }
    // cout << endl;
    // for (auto &val : words)
    //     cout << val << " ";
    // cout << endl;
    return words;
}

void pass1()
{
    int line_num = 0, prct = 0; // Initialize line number and program counter.

    // Iterate through each line in the input.
    for (string line : inputlines)
    {
        line_num++; // Increment the line number for each new line.

        vector<string> curr = parseLine(line); // Parse the line into tokens.
        int it = 0, n = curr.size();           // Initialize iterator and count of tokens.

        if (!n) // Skip if the line is empty.
            continue;

        string lab = "", mnem = "", oper = ""; // Initialize label, mnemonic, and operand as empty.

        // Check if the first token is a label (ends with ':').
        if (curr[it].back() == ':')
        {
            lab = curr[it]; // Store the label.
            it++;           // Move to the next token.
        }

        // Extract mnemonic if available.
        if (it < n)
        {
            mnem = curr[it];
            it++;
        }

        // Extract operand if available.
        if (it < n)
        {
            oper = curr[it];
            it++;
        }

        // If there is a label, validate and store it without the trailing ':'.
        if (lab != "")
            sol.validateAndStoreLabel(lab.substr(0, find(all(lab), ':') - lab.begin()), prct, line_num);

        bool check = false; // Flag to track if mnemonic processing succeeds.

        if (mnem.size()) // If there is a mnemonic to process.
        {
            if (opcodes.find(mnem) == opcodes.end()) // Check if mnemonic is valid.
            {
                errors[line_num] = "Bogus Mnemonic"; // Error for unrecognized mnemonic.
            }
            else
            {
                // Check operand requirements based on the mnemonic's expected operand type.
                if (opcodes[mnem].second != "")
                {
                    if (oper == "")
                        errors[line_num] = "Missing operand"; // Error if operand is required but missing.
                    else if (n - it > 0)
                        errors[line_num] = "Extra on end of line"; // Error if additional tokens exist after operand.
                }
            }
            // Process the mnemonic and operand, and store result in line_info if valid.
            check = sol.processMnemonicWithOperand(mnem, oper, prct, line_num);
            line_info[prct] = {lab, {mnem, oper}}; // Store line information with program counter.
            if (check)                             // If processing succeeded, increment program counter.
                prct++;
        }

        // Handle the special case of "SET" instruction.
        if (check && mnem == "SET")
        {
            if (lab.empty())
            {
                errors[line_num] = "label name or variable name missing"; // Error if "SET" has no label.
            }
            else
            {
                setvar[lab] = oper; // Store the "SET" variable and its value.
            }
        }
    }

    // Check for unused or undefined labels after processing all lines.
    for (auto it : labelCatalog)
    {
        if (it.second.first == -1) // Label found in code but not defined.
        {
            for (auto linenum : label_occ[it.first])
                errors[linenum] = "No such label"; // Error for undefined label.
        }
        else if (label_occ.find(it.first) == label_occ.end()) // Label defined but not used.
        {
            warnings[it.second.second] = "Label declared but not used"; // Warning for unused label.
        }
    }
}

void bubbleSort(vector<int> &arr)
{
    int n = arr.size();
    bool swapped;

    // Outer loop to control the number of passes
    for (int i = 0; i < n - 1; i++)
    {
        swapped = false;

        // Inner loop to perform comparisons and swaps
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                // Swap if the current element is greater than the next
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        // If no elements were swapped, the array is sorted
        if (!swapped)
            break;
    }
}

// Function to get info about the labels stored
void showTable()
{
    for (auto &val : labelCatalog)
    {
        cout << val.first << " " << val.second.first << " " << val.second.first << endl;
    }
}

// Function to know which label is used as an operand in which line
void showLabel()
{
    for (auto &val : label_occ)
    {
        cout << val.first << endl;
        for (auto &it : val.second)
            cout << it << " ";
        cout << endl;
    }
}

void pass2()
{
    // Iterate over each entry in line_info, which contains parsed line information.
    for (auto &val : line_info)
    {
        int pc = val.first;   // Program counter (address) for the current instruction.
        auto pa = val.second; // Parsed line information (label, mnemonic, operand).

        string lab = "", mnem = "", oper = ""; // Initialize label, mnemonic, and operand as empty.
        lab = pa.first;                        // Extract label.
        mnem = pa.second.first;                // Extract mnemonic.
        oper = pa.second.second;               // Extract operand.

        string kind = "   "; // Initialize kind to hold operand type information.
        if (!mnem.empty())   // If mnemonic is not empty, get operand type.
            kind = opcodes[mnem].second;

        string mcode = "       "; // Placeholder for the machine code output.

        // If the mnemonic has no specific operand type (empty "kind"), set a default machine code format.
        if (kind == "")
            mcode = "000000" + opcodes[mnem].first;

        // Process mnemonics that use a "value" type operand (e.g., "data" or "SET").
        else if (kind == "value")
        {
            if (mnem == "data" || mnem == "SET")
            {
                mcode = conv.dechex(stoi(oper)); // Convert operand directly to hex for data/SET.
            }
            else
            {
                int node;
                // Check if operand is a label; if not, assume it's an immediate integer value.
                if (labelCatalog.find(oper) == labelCatalog.end())
                {
                    node = (int)stoi(oper); // Convert operand to integer if not found in label catalog.
                }
                else
                {
                    node = labelCatalog[oper].first; // Get address from label catalog if label exists.
                }
                // Generate machine code by combining operand and opcode.
                mcode = conv.dechex(node).substr(2) + opcodes[mnem].first;

                // Special handling if the operand is defined by a SET directive.
                if (setvar.count(oper))
                {
                    mcode = conv.dechex(stoi(setvar[oper])).substr(2) + opcodes[mnem].first;
                }
            }
        }
        // Process mnemonics that use an "offset" type operand.
        else if (kind == "offset")
        {
            int offset;
            // Check if operand is a label; if not, assume it's an immediate integer offset.
            if (labelCatalog.find(oper) == labelCatalog.end())
            {
                offset = (int)stoi(oper); // Convert operand to integer if not found in label catalog.
            }
            else
                offset = labelCatalog[oper].first - (pc + 1); // Calculate relative offset to label.

            // Generate machine code by combining the offset and opcode.
            mcode = conv.dechex(offset).substr(2) + opcodes[mnem].first;
        }

        machine_code.push_back(mcode); // Store generated machine code for the line.

        // Format label and mnemonic with trailing spaces for readability.
        if (!lab.empty())
            lab += " ";
        if (!mnem.empty())
            mnem += " ";

        // Concatenate label, mnemonic, and operand for a human-readable assembly output line.
        string text = lab + mnem + oper;

        // Store the line's address (in hex), machine code, and assembly text in listi.
        listi.push_back({conv.dechex(pc), mcode, text});
    }
}

// Function to write the errors and warnings if present
void writeErrors()
{
    ofstream printErrors(file_name.substr(0, file_size - 4) + ".log"); // Opening the file in which errors are to be written
    if (!errors.size())                                                // If no errors are present then warnings will be outputted
    {
        printErrors << "Your program is Error-free" << endl;
        //  printErrors << warnings.size() << endl;
        for (auto &it : warnings)
        {
            printErrors << "Line_no. " << it.first << " WARNING: " << it.second << endl;
        }
    }
    else // If the errors are present
    {
        for (auto &it : errors)
        {
            printErrors << "Line_no. " << it.first << " ERROR: " << it.second << endl;
        }
    }
    printErrors.close(); // closing the errors file
}

int main()
{
    init();

    // Open the input file and read lines
    cout << "Enter the name of file you wnat to give as input" << endl;
    cin >> file_name;
    file_size = file_name.size();
    ifstream inpfile(file_name);
    if (!inpfile)
    {
        cerr << "Error: Input file 'test1.asm' does not exist.\n";
        return 1;
    }
    string line;
    while (getline(inpfile, line))
        inputlines.push_back(line);
    inpfile.close();

    // Perform first pass and write errors
    pass1();
    writeErrors();
    cout << "Error File Generated\n";

    // If no errors, proceed to second pass and write outputs
    if (errors.empty())
    {
        pass2();

        // Write the .lst file
        ofstream lst_file(file_name.substr(0, file_size - 4) + ".lst");
        if (!lst_file)
        {
            cerr << "Error: Unable to open 'listfile.lst' for writing.\n";
            return 1;
        }
        for (const auto &entry : listi)
            lst_file << entry.prctr << " " << entry.mcode << " " << entry.text << "\n";
        cout << "List file generated successfully.\n";
        lst_file.close();

        // Write machine code .o file in binary
        ofstream printmcode(file_name.substr(0, file_size - 4) + ".o", ios::binary);
        if (!printmcode)
        {
            cerr << "Error: Unable to open 'machinecode.o' for writing.\n";
            return 1;
        }
        for (auto code : machine_code)
        {
            if (code.empty() || (code == "        "))
                continue;
            unsigned int cur = (unsigned int)stoi(conv.hexdec(code));
            static_cast<int>(cur);
            printmcode.write((const char *)&cur, sizeof(unsigned int));
        }
        printmcode.close();
        cout << "Machine code file generated\n";
    }
    else
    {
        cout << "Errors encountered in pass1, see 'logfile.log' for details.\n";
    }

    return 0;
}
