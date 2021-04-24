#include "assembler.h"
int two_complement(int val, int digit){
    val = val % (1<<digit);                 //maybe not perfect. for example -2billion
    if(val >= 0) 
        return val;
    else{
        unsigned int cmp_val = (1<<digit) + val;
    }
}
int reverse_two_complement(int val,int digit){
    val = val % (1<<digit); 
    if(val < 1<<(digit-1)) return val;
    else{
        return val - (1<<digit);
    }
}
int find_label(string *label){
    for(Label *var = label_table;var;var = var->next){
        if(var->name->len == label->len && !strncmp(var->name->str,label->str,label->len))
            return var->address;
    }   
    error_at(label->str,"label: %.*s was not declared",label->len,label->str);
}

void codegen(Instr *memory[]){
    for(int address = 0;address < depth;address++){
        
        int machine_code = 0;                   //output 

        Instr *instr = memory[address];
        switch (instr->instr){
        case RTYPE:
            machine_code += (int)(instr->instr) << 26;  //[31:26]
            machine_code += instr->rs           << 21;  //[25:21]
            machine_code += instr->rt           << 16;  //[20:16] 
            machine_code += instr->rd           << 11;  //[15:11]
            machine_code += (int)(instr->funct);
            break;
        case        J:     //ex) j 0
        case      JAL:     //ex) jal func 
            if(instr->label){
                instr->jaddress = find_label(instr->label);
            }
            instr->jaddress = ((address >> 28) << 28) | ((instr->jaddress) % (1<<28));    //PC[31:28].jaddress[27:0]

            machine_code += (int)(instr->instr) << 26;  //[31:26]
            machine_code += instr->jaddress;     //[25:0]
            break;
        case      BEQ:     //ex) beq $3, $0, loop
        case      BNE:
            instr->jaddress = two_complement(find_label(instr->label) - address ,16);
            
            machine_code += (int)(instr->instr) << 26;  //[31:26]
            machine_code += instr->rs           << 21;  //[25:21]
            machine_code += instr->rt           << 16;  //[20:16] 
            machine_code += instr->jaddress;  //[15:0]
            break;
        case     ADDI:     //ex) addi $9 ,$9, 78  
        case       LW:        //ex) lw $4, 100($2)
        case       SW:
            machine_code += (int)(instr->instr) << 26;  //[31:26]
            machine_code += instr->rs           << 21;  //[25:21]
            machine_code += instr->rt           << 16;  //[20:16] 
            machine_code += two_complement(instr->imm ,16);  //[15:0]
            break;
        default://INONE
            if(address < depth-1){
                if(memory[address+1]->instr == INONE){
                    int isnone = address+1;
                    while(memory[isnone]->instr == INONE)isnone++;
                    printf("[%d..%d]:     00000000;\n",address,isnone-1);
                    address = isnone-1;
                }
            }else{
                printf("%d        :     00000000;\n",address);
            }
            break;
        }
        if(memory[address]->instr != INONE)
            printf("%d      :     %08x;\n",address,machine_code);
        
        #ifdef DEBUG
            //printf("DEBUG...instr={itype:%d, rs:%d, rt:%d, rd:%d, ftype:%d, imm:%d, jaddress:%d}\n",
              //  (int)instr->instr,instr->rs,instr->rt,instr->rd,(int)instr->funct,instr->imm,instr->jaddress);
            if(instr->instr == RTYPE)
                printf("                    ->DEBUG...mcode.RTYPE:%d %d %d %d %d %d\n",
                machine_code>>26,
                (machine_code>>21)%(1<<5),
                (machine_code>>16)%(1<<5),
                (machine_code>>11)%(1<<5),
                (machine_code>>6)%(1<<5),
                (machine_code)%(1<<6));
            else if(instr->instr == J || instr->instr == JAL)
                printf("                    ->DEBUG...mcode.JTYPE:%d %d\n",
                machine_code>>26,
                (machine_code)%(1<<26));
            else
                printf("                    ->DEBUG...mcode.ITYPE:%d %d %d %d\n",
                machine_code>>26,
                (machine_code>>21)%(1<<5),
                (machine_code>>16)%(1<<5),
                reverse_two_complement(machine_code%(1<<16),16));
        #endif
    }
    return;
}