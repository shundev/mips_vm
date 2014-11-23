#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>


#define MEM 10
#define CLOCK 500000
#define REG 32


using namespace std;

unsigned int get_op(unsigned int word);
unsigned int get_funct(unsigned int word);
unsigned int get_rt(unsigned int word);
unsigned int get_rs(unsigned int word);
unsigned int get_rd(unsigned int word);
unsigned int get_shamt(unsigned int word);
unsigned int get_address(unsigned int word, int size);
unsigned int get_immediate(unsigned int word);

class MIPS
{
private:
    int mem[MEM];
    int m_size;

    int reg[REG];

    int pc;

    void setup_memory(int argc, char** argv);
    void dump_memory();
    unsigned int fetch_memory();
    void validate_address(unsigned int address);

public:
    MIPS(int argc, char** argv);
    ~MIPS();
    void up();
    void halt();
};


int main (int argc, char** argv)
{
    MIPS* mips = new MIPS(argc, argv);
    mips->up();
}


MIPS::MIPS(int argc, char** argv)
{
    m_size = MEM;
    setup_memory(argc, argv);
}


void MIPS::setup_memory(int argc, char** argv)
{
    unsigned int i = 0, word;

    while (argc > 1)
    {
        try
        {
            // stoi can't handle FFFFFFFF
            word = (unsigned int)stol(argv[i + 1], nullptr, 16);
            mem[i++] = word;
        }
        catch(invalid_argument e)
        {
            cerr << "Illegal hexadecimal found: " << argv[i+1] << endl;
            exit(1);
        }

        if (i > m_size)
        {
            cerr << "Memory overrun." << endl;
            exit(2);
        }
        argc--;
    }
}

void MIPS::dump_memory()
{
    int i = 0;
    while (mem[i] != 0)
    {
        cout << hex << i << ":\t" << hex << mem[i] << endl;
        i++;
    }
}


void MIPS::up()
{
    // size(bit)   32   6   5   5   5      5      6    16-26      16     16
    unsigned int word, op, rs, rt, rd, shamt, funct, address, offset, i_val;

    while (pc < m_size)
    {
        word = fetch_memory();
        // most significant 6 bits
        op = get_op(word);
        switch (op)
        {
            case 0:
                funct = get_funct(word);
                rs = get_rs(word);
                rt = get_rt(word);
                rd = get_rd(word);
                shamt = get_shamt(word);
                switch (funct)
                {
                    case 32:
                        cout << "ADD\t";
                        goto finally;
                    case 34:
                        cout << "SUB\t";
                        goto finally;
                    case 36:
                        cout << "AND\t";
                        goto finally;
                    case 37:
                        cout << "OR\t";
                        goto finally;
                    case 39:
                        cout << "NOR\t";
                        goto finally;
                    case 0:
                        cout << "SLL\t"; // Shift left logical
                        goto finally;
                    case 2:
                        cout << "SRL\t"; // Shift right logical

                }
                finally:
                    cout << rs << "\t";
                    cout << rt << "\t";
                    cout << rd << "\t";
                    cout << shamt << "\t";
                    cout << funct << "\t";
                    cout << word;
                    break;
            case 2:
                // least significant 26 bits
                address = get_address(word, 26);
                validate_address(address);
                cout << "J\t" << address; // Jump
                pc = address;
                break;
            case 4:
                rs = get_rs(word);
                rt = get_rt(word);
                address = get_address(word, 16);
                validate_address(address);
                //                      PC-relative addressing
                if (reg[rs] == reg[rt]) pc = pc + address;
                cout << "BEQ\t"; // Branch if equal
                cout << rs << "\t";
                cout << rt << "\t";
                cout << address << "\t";
                break;
            case 5:
                rs = get_rs(word);
                rt = get_rt(word);
                address = get_address(word, 16);
                validate_address(address);
                if (reg[rs] != reg[rt]) pc = address;
                cout << "BNE\t"; // Branch not equal
                cout << rs << "\t";
                cout << rt << "\t";
                cout << address << "\t";
                break;
            case 12:
                rs = get_rs(word);
                rt = get_rt(word);
                i_val = get_immediate(word);
                cout << "ANDI\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << i_val << "\t";
            case 13:
                rs = get_rs(word);
                rt = get_rt(word);
                i_val = get_immediate(word);
                cout << "ORI\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << i_val << "\t";
            case 35:
                rs = get_rs(word);
                rt = get_rt(word);
                offset = get_immediate(word);
                cout << "LW\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << offset << "\t";
            case 43:
                rs = get_rs(word);
                rt = get_rt(word);
                offset = get_immediate(word);
                cout << "SW\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << offset << "\t";
                break;
        }
        cout << endl;
        usleep(CLOCK);
    }

}

unsigned int MIPS::fetch_memory()
{
    int word;
    word = mem[pc];
    pc++;
    return word;
}


void MIPS::validate_address(unsigned int address)
{
    if (address >= m_size)
    {
        cerr << "Segmentation error. " << address << endl;
        exit(3);
    }
}


unsigned int get_op(unsigned word)
{
    return (word >> 26);
}

unsigned int get_funct(unsigned int word)
{
    return (word & 0x3F);
}

unsigned int get_rs(unsigned int word)
{
    return (word & 0x3FFFFFF) >> 21;
}

unsigned int get_rt(unsigned int word)
{
    return (word & 0x1FFFFF) >> 16;
}

unsigned int get_rd(unsigned int word)
{
    return (word & 0xFFFF) >> 11;
}

unsigned int get_shamt(unsigned int word)
{
    return (word & 0x7FF) >> 6;
}


unsigned int get_address(unsigned int word, int size)
{
    if (size == 26) return word & 0x3FFFFFF;
    else if (size == 16) return word & 0xFFFF;
    return 0;
}

unsigned int get_immediate(unsigned int word)
{
    return word & 0xFFFF;
}

