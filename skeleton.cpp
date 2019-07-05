/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the "ADD", "SUB" and "ADDI"instructions and prints
	"Unkown Instruction" for all other instructions!

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <cstring>

using namespace std;

int regs[32]={0};
unsigned int pc = 0x0, tp=0x0;

char memory[8*1024];	// only 8KB of memory located at address 0

void emitError(const char *s)
{
	cout << s;
	exit(0);
}


unsigned int translateTo16Bit(unsigned short compressedI) {
    unsigned int funct3, funct4, rd, rs1, rs2, imm, offset, target;

    funct3 = (compressedI>>13)&0x7;
    funct4 = funct3<<1 & (compressedI>>12&0x1);
}

void printPrefix(unsigned int instA, unsigned int instW){
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec16(unsigned int instHalf){
    unsigned int rd, rs1, rs2, funct3, funct4, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

    unsigned int instPC = pc - 2;

    opcode = instHalf & 0x00000003;
    funct3 = (instHalf >> 13) & 0x00000007;
    rd = rs1 = (instHalf >> 7) & 0x0000001F;
    rs2 = (instHalf >> 2) & 0x0000001F;
    funct4 = (instHalf>>12) & 0x0000000F;

    //if(opcode==)
}
void instDecExec(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

    unsigned int instPC = pc - 4;
	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	U_imm = (instWord & 0xFFFFF000);
	J_imm = ((((instWord >> 21) & 0x1FF) | (((instWord >> 20) & 0x1)<<10) | (((instWord >> 12) & 0xFF)<<11) | (((instWord >> 31) & 0x1)<<20))<<1);

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    // — inst[31] — inst[30:25] inst[24:21] inst[20]
    I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

    printPrefix(instPC, instWord);

    if (opcode == 0x33) {        // R Instructions
        switch (funct3) {
            case 0:
                if (funct7 == 32) {
                    cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    regs[rd] = regs[rs1] - regs[rs2];
                } else {
                    cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    regs[rd] = regs[rs1] + regs[rs2];
                }
                break;
            case 1:
                cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = (regs[rs1]) << regs[rs2];
                break;
            case 2:
                cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] < regs[rs2] ? 1 : 0;
                break;
            case 3:
                cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = (unsigned int) regs[rs1] < (unsigned int) regs[rs2] ? 1 : 0;
                break;
            case 4:
                cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] ^ regs[rs2];
                break;
            case 5:
                if (funct7 == 32) {
                    cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    for (int i = 0; i < regs[rs2]; i++) {
                        regs[rd] = regs[rs1] >> 1 | (((regs[rs1] >> 31) ? 0xFFFFF800 : 0x0));
                    }
                } else {
                    cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    for (int i = 0; i < regs[rs2]; i++) {
                        regs[rd] = regs[rs1] >> 1 & (0x7FFFFFFF);
                    }
                }
                break;
            case 6:
                cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] | regs[rs2];
                break;
            case 7:
                cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] & regs[rs2];
                break;
            default:
                cout << "\tUnkown R Instruction \n";
        }
    } else if (opcode == 0b0100011) { //S instructions
        S_imm = rd & (funct7 << 5);
        switch (funct3) {
            case 0:
                cout << "\tSB\tx" << rs1 << ", x" << rs2 << ", x" << S_imm << "\n";
                memory[rs2 + S_imm] = rs1 & 0x000000FF;
                break;
            case 1:
                cout << "\tSH\tx" << rs1 << ", x" << rs2 << ", x" << S_imm << "\n";
                memory[rs2 + S_imm] = rs1 & 0x0000FFFF;
                break;
            case 2:
                cout << "\tSW\tx" << rs1 << ", x" << rs2 << ", x" << S_imm << "\n";
                memory[rs2 + S_imm] = rs1;
                break;
            default:
                cout << "\tUnkown S Instruction \n";
        }
    } else if (opcode == 0b1100011) {
        B_imm = (((rd >> 1) << 1) & 0x0000001F) | (funct7 & 0x0000003F) << 5 | (rd & 0x1 << 11) |
                ((funct7 >> 6) & 0x1) << 12;
        switch (funct3) {
            case 0:
                cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = rs1 == rs2 ? B_imm : pc;
                break;
            case 1:
                cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = rs1 != rs2 ? B_imm : pc;
                break;
            case 4:
                cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = rs1 < rs2 ? B_imm : pc;
                break;
            case 5:
                cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = rs1 >= rs2 ? B_imm : pc;
                break;
            case 6:
                cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = (unsigned int) rs1 < (unsigned int) rs2 ? B_imm : pc;
                break;
            case 7:
                cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int) B_imm << "\n";
                pc = (unsigned int) rs1 >= (unsigned int) rs2 ? B_imm : pc;
                break;
            default:
                cout << "\tUnkown B Instruction \n";

        }
	} else if(opcode == 0x13){	// I instructions
		switch(funct3){
			case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
					regs[rd] = regs[rs1] + (int)I_imm;
					break;

			case 2: cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				if ((int)I_imm > regs[rs1]) regs[rd] = 1;
				else regs[rd] = 0;
					break;

			case 3: cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				if ((unsigned int)I_imm > (unsigned int)regs[rs1]) regs[rd] = 1;
				else regs[rd] = 0;
				break;

			case 4: cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				regs[rd] = regs[rs1] ^ (int)I_imm;
				break;

			case 6: cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				regs[rd] = regs[rs1] | (int)I_imm;
				break;

			case 7: cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				regs[rd] = regs[rs1] & (int)I_imm;
				break;

			case 5: 
				unsigned int t0, t1;
				t0 = ((instWord >> 25) & 0x7F);
				t1 = ((instWord >> 20) & 0x1F);

				if (t0 == 0) {
					cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
					for (int i = 0; i < t1; i++) {
						regs[rd] = regs[rs1] >> 1 | (((regs[rs1] >> 31) ? 0xFFFFF800 : 0x0));
					}
				}
				else {
					cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
					for (int i = 0; i < t1; i++) {
						regs[rd] = regs[rs1] >> 1 & (((regs[rs1] >> 31) ? 0xFFFFF800 : 0x0));
					}
				}
				break;

			case 1:
				unsigned int t2;
				t2 = ((instWord >> 20) & 0x1F);
				cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				for (int i = 0; i < t1; i++) {
					regs[rd] = (regs[rs1] << 1) & (((regs[rs1] << 31) ? 0xFFFFF800 : 0x0));
				}
				break;
		}
	}
	else if (opcode == 0x3) {
		switch (funct3) {

            case 2:
                cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                if ((int) I_imm > regs[rs1]) regs[rd] = 1;
                else regs[rd] = 0;
                break;

            case 3:
                cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                if ((unsigned int) I_imm > (unsigned int) regs[rs1]) regs[rd] = 1;
                else regs[rd] = 0;
                break;


            case 4:
                cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                regs[rd] = regs[rs1] ^ (int) I_imm;
                break;

            case 6:
                cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                regs[rd] = regs[rs1] | (int) I_imm;
                break;

            case 7:
                cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                regs[rd] = regs[rs1] & (int) I_imm;
                break;

            case 5:
                unsigned int t0, t1;
                t0 = ((instWord >> 25) & 0x7F);
                t1 = ((instWord >> 20) & 0x1F);

                if (t0 == 0) {
                    cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                    for (int i = 0; i < t1; i++) {
                        regs[rd] = regs[rs1] >> 1 | (((regs[rs1] >> 31) ? 0xFFFFF800 : 0x0));
                    }
                } else {
                    cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                    for (int i = 0; i < t1; i++) {
                        regs[rd] = regs[rs1] >> 1 & (((regs[rs1] >> 31) ? 0xFFFFF800 : 0x0));
                    }
                }
                break;

            case 1:
                unsigned int t2;
                t2 = ((instWord >> 20) & 0x1F);
                cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int) I_imm << "\n";
                for (int i = 0; i < t1; i++) {
                    regs[rd] = regs[rs1] << 1 & (((regs[rs1] << 31) ? 0xFFFFF800 : 0x0));
                }
                break;
        }
    } else if (opcode == 0x3) {
        switch (funct3) {

            case 4:
                cout << "\tLBU\tx" << rd << "," << hex << "0x" << (int) I_imm << "(x" << rs1 << ")" << "\n";
                regs[rd] = (unsigned char) memory[rs1 + ((int) I_imm)];
                break;

            case 0:
                cout << "\tLB\tx" << rd << "," << hex << "0x" << (int) I_imm << "(x" << rs1 << ")" << "\n";
                regs[rd] = (unsigned char) memory[rs1 + ((int) I_imm)] |
                           ((((unsigned char) memory[rs1 + ((int) I_imm)] >> 7) ? 0xFFFFFF00 : 0x0));
                break;

            case 1:
                cout << "\tLH\tx" << rd << "," << hex << "0x" << (int) I_imm << "(x" << rs1 << ")" << "\n";
                regs[rd] = ((unsigned char) memory[rs1 + ((int) I_imm)] |
                            (unsigned char) memory[rs1 + ((int) I_imm + 1) << 8]) |
                           ((((unsigned char) memory[rs1 + ((int) I_imm) + 1] >> 7) ? 0xFFFF0000 : 0x0));
                break;

            case 5:
                cout << "\tLHU\tx" << rd << "," << hex << "0x" << (int) I_imm << "(x" << rs1 << ")" << "\n";
                regs[rd] = ((unsigned char) memory[rs1 + ((int) I_imm)] |
                            (unsigned char) memory[rs1 + ((int) I_imm + 1) << 8]);
                break;

            case 2:
                cout << "\tLW\tx" << rd << "," << hex << "0x" << (int) I_imm << "(x" << rs1 << ")" << "\n";
                regs[rd] = ((unsigned char) memory[rs1 + ((int) I_imm)] |
                            (unsigned char) memory[rs1 + ((int) I_imm + 1) << 8 |
                                                   (unsigned char) memory[rs1 + ((int) I_imm) + 2] << 16 |
                                                   (unsigned char) memory[rs1 + ((int) I_imm) + 3] << 24]);

        }
    }else if (opcode==0b1110011){
        if((rs2 | funct7<<5)==0) {
            char* buffer;
            switch (regs[17]) {
                case 1:
                    cout << regs[10];
                    break;
                case 4:
                    cout << memory[regs[10]];
                    break;
                case 5:
                    cin >> regs[10];
                    break;
                case 8: //TODO
                    buffer = (char*) calloc(regs[11]+1, sizeof(char));
                    cin >> buffer;
                    if(buffer[regs[11]]!=0) buffer[regs[11]]=0;
                    strcpy(&memory[regs[10]],buffer);
                    break;
                case 10:
                    exit(0);
                    break;
                default:
                    cout<< "Unsupported Environment call\n";

        }
    } else {

            switch()
		case 1:	cout << "\tLH\tx" << rd << "," << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = ((unsigned char)memory[rs1 + ((int)I_imm)] | (((unsigned char)memory[rs1 + ((int)I_imm + 1)] )<< 8) | ((((unsigned char)memory[rs1 + ((int)I_imm) + 1]) >> 7) ? 0xFFFF0000 : 0x0));
			break;

		case 5:	cout << "\tLHU\tx" << rd << "," << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = (unsigned char)memory[rs1 + ((int)I_imm)] | (((unsigned char)memory[rs1 + ((int)I_imm + 1)]) << 8);
			break;

		case 2: cout << "\tLW\tx" << rd << "," << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = (unsigned char)memory[rs1 + ((int)I_imm)] | (((unsigned char)memory[rs1 + ((int)I_imm + 1)]) << 8) | (((unsigned char)memory[rs1 + ((int)I_imm) + 2]) << 16) | (((unsigned char)memory[rs1 + ((int)I_imm) + 3]) << 24);
			break;

		}
	}
	else if (opcode == 55) {
		cout << "\tLUI\tx" << rd << "," << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = regs[rd] + (int)U_imm;
	}

	else if (opcode == 23) {
		cout << "\tAUIPC\tx" << rd << "," << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = pc + (int)U_imm;
	}

	else if (opcode == 111) {
		cout << "\tJAL\tx" << rd << "," << hex << "0x" << (int)J_imm << "\n";
		J_imm = J_imm | (((J_imm >> 20) ? 0xFFFFF800 : 0x0));
		pc = (unsigned char)memory[(int)J_imm] | (((unsigned char)memory[(int)J_imm+1]) << 8) | (((unsigned char)memory[(int)J_imm+2] )<< 16) | (((unsigned char)memory[(int)J_imm+3]) <<24);
		regs[rd] = pc + 4;
	}

	else if (opcode == 103) {
		cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
		memory[pc] = ((int)I_imm + regs[rs1]) & 0xFFFFFFFE;
		regs[rd] = memory[pc+4] ;

	}
	
	
	else {
        cout << "\tUnkown Instruction \n";
    }

}

int main(int argc, char *argv[]){

	unsigned int instWord = 0;
	unsigned short check1=0 , check2=0, bit16=0;
	ifstream inFile;
	ofstream outFile;

	if(argc<1) emitError("use: rv32i_sim <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if(inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if(!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");

		while(true){

			tp = pc;
			check1 = (((unsigned char)memory[tp] |
				(((unsigned char)memory[tp + 1]) << 8)) & 0x3);

			if (check1 == 0x3) {
				instWord = (unsigned char)memory[pc] |
					(((unsigned char)memory[pc + 1]) << 8) |
					(((unsigned char)memory[pc + 2]) << 16) |
					(((unsigned char)memory[pc + 3]) << 24);
				pc += 4;
				instDecExec(instWord);
			}
			else {
				check2 = ((((unsigned char)memory[tp + 2]) << 16) |
					(((unsigned char)memory[tp + 3]) << 24) & 0x3);
				if (check2 == 0x3) {
					instWord = (unsigned char)memory[pc] |
						(((unsigned char)memory[pc + 1]) << 8) |
						(((unsigned char)memory[pc + 2]) << 16) |
						(((unsigned char)memory[pc + 3]) << 24);
					pc += 4
						instDecExec(instWord);
				}
				else {
					bit16 = (unsigned char)memory[pc] |
						(((unsigned char)memory[pc + 1]) << 8);
					pc += 2;
					translateTo16Bit(bit16);
				}
			}
						


				
				// pc += 4;
				// remove the following line once you have a complete simulator
				if(pc==32) break;			// stop when PC reached address 32
				//instDecExec(instWord);
		}

		// dump the registers
		for(int i=0;i<32;i++)
				cout << "x" << dec << i << ": \t"<< "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";

	} else emitError("Cannot access input file\n");
}
