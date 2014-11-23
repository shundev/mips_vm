#include <iostream>
#include <cstdlib>
#include <string.h>
#include <unistd.h>


#define MEM 10
#define CLOCK 500000
#define REG 32


using namespace std;

class MIPS
{
private:
    int mem[MEM];
    int m_size;

    int reg[REG];

    int pc;

    void setup_memory(int argc, char** argv);
    void dump_memory();
    int fetch_memory();
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
    unsigned int word = 0xFFFFFFFF;
    cout << ((word >> 21) & 0x1F) << endl;
}


MIPS::MIPS(int argc, char** argv)
{
    m_size = MEM;
    setup_memory(argc, argv);
    // dump_memory();
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
        op = word >> 26;
        switch (op)
        {
            case 0:
                funct = (word & 0x3F);
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                rd = (word & 0xFFFF) >> 11;
                shamt = (word & 0x7FF) >> 6;
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
                // 下26桁
                address = word & 0x3FFFFFF;
                validate_address(address);
                cout << "J\t" << address; // Jump
                break;
            case 4:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                address = word & 0xFFFF;
                validate_address(address);
                if (reg[rs] == reg[rt]) pc = address;
                cout << "BEQ\t"; // Branch if equal
                cout << rs << "\t";
                cout << rt << "\t";
                cout << address << "\t";
                break;
            case 5:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                address = word & 0xFFFF;
                validate_address(address);
                if (reg[rs] != reg[rt]) pc = address;
                cout << "BNE\t"; // Branch not equal
                cout << rs << "\t";
                cout << rt << "\t";
                cout << address << "\t";
                break;
            case 12:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                i_val = word & 0xFFFF;
                cout << "ANDI\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << i_val << "\t";
            case 13:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                i_val = word & 0xFFFF;
                cout << "ORI\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << i_val << "\t";
            case 35:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                offset = word & 0xFFFF;
                cout << "LW\t";
                cout << rt << "\t";
                cout << rs << "\t";
                cout << offset << "\t";
            case 43:
                rs = (word & 0x3FFFFFF) >> 21;
                rt = (word & 0x1FFFFF) >> 16;
                offset = word & 0xFFFF;
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

int MIPS::fetch_memory()
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
