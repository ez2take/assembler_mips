#include  "assembler.h"
Label *label_table;
void add_label_table(int address,char *p,int len);
void whatisthis_mnemonic(InstrType *ip,FunctType *fp,string mnemonic);
int whatisthis_register(string register);
Instr *get_designated_operands(InstrType itype,FunctType ftype,char **pp);


int count_strlen(char *p, int mode){
    int len = 0;
    if(mode == 0){              //for label
        while(('a'<= *p && *p <='z')||
              ('A'<= *p && *p <='Z')||
              ('0'<= *p && *p <='9')||
              (*p == '_')){
            len++;
            p++;
        }
    }else if(mode == 1){        //for mnemonic
        while(('a'<= *p && *p <='z')){
            len++;
            p++;
        }
    }else if(mode == 2){        //for register
        while(('a'<= *p && *p <='z')||
              ('0'<= *p && *p <='9')){
            len++;
            p++;
        }
    }
    return len;
}
void next_line(char *p){
    while(*p != '\n')p++;
    p++;
    return;
}
bool skip_blank_line(char **pp){          //if really skipped the line, "p" rewroted
    char *forward_p = *pp;
    while(*forward_p == ' '|| *forward_p == '\t' )forward_p++;
    if(*forward_p == '\n'){
        *pp = forward_p + 1;
        return true;
    }
    if(!strncmp(forward_p,"//",2)){
        while(*forward_p != '\n')forward_p++;
        *pp = forward_p +1;
        return true;
    }
    return false;
}
char *expect_space(char *p){
    if(*p != ' '){
        error_at(p,"at least one space or tab required");
    }
    while(*p == ' '|| *p == '\t' )p++;
    return p;
}
char *expect(char *p,char op){
    if(*p = op){
        return p+1;
    }else error_at(p,"not %c",op);
}
void parse(char *p,Instr *memory[]){  //recognize labels and parse line into label,mnemonic,and operands
    int address = -1;
    while(*p){         //loop per line
        
        //skip a line with nothing or comment
        if(skip_blank_line(&p))continue;

        //if labelled get and add it to label_table
        if('a'<=*p && *p<='z' || 'A'<=*p && *p<='Z' || *p == '_'){
            int len = count_strlen(p,0);
            if(p[len] == ':')
                add_label_table(address,p,len);
            else
                error_at(p,"this is not a valid label");
            p += len + 1;   // increment as "[label]:"

            if(skip_blank_line(&p))
                continue;        //if the line only have a label
        }
        
        //expect instruction line
        address++;
        if( !(0 <= address && address < depth) )
            error_at(p,"address %d is invalid",address);

        p = expect_space(p);

        //get a mnemonic
        if( !('a'<=*p && *p<='z') )
            error_at(p,"alphabet of mnemonic name needed here");
        int len = count_strlen(p,1);
        string mnemonic = {p,len};
        InstrType itype = INONE;
        FunctType ftype = FNONE;
        whatisthis_mnemonic(&itype,&ftype,mnemonic);
        p += len;

        //get operands
        if(itype != INONE){
            p = expect_space(p);
            Instr *code = get_designated_operands(itype,ftype,&p);
            if(0 <= address && address < depth)
                memory[address] = code;
            else
                error_at(p,"address %d is invalid",address);
            skip_blank_line(&p);
            continue;
        }
        error_at(p,"failed at parsing");
    }
    return;
}

void add_label_table(int address,char *p,int len){
    Label* label= calloc(1,sizeof(label));
    label->next = label_table;
    label->address = address;
    label->name = calloc(1,sizeof(string));
    label->name->str = p;
    label->name->len  = len;
    label_table = label;
    return ;
}
bool compare_string(string target,char *str){
    if(target.len == strlen(str) &&
        !strncmp(target.str,str,target.len))
       return true;
    return false;
}
void whatisthis_mnemonic(InstrType *ip,FunctType *fp,string mnemonic){
    if(compare_string(mnemonic,"add")){
        *ip = RTYPE; *fp = ADD;
    }else if(compare_string(mnemonic,"sub")){
        *ip = RTYPE; *fp = SUB;
    }else if(compare_string(mnemonic,"and")){
        *ip = RTYPE; *fp = AND;
    }else if(compare_string(mnemonic,"or")){
        *ip = RTYPE; *fp = OR;
    }else if(compare_string(mnemonic,"slt")){
        *ip = RTYPE; *fp = SLT;
    }else if(compare_string(mnemonic,"j")){
        *ip = J; *fp = FNONE;
    }else if(compare_string(mnemonic,"jal")){
        *ip = JAL; *fp = FNONE;
    }else if(compare_string(mnemonic,"beq")){
        *ip = BEQ; *fp = FNONE;
    }else if(compare_string(mnemonic,"bne")){
        *ip = BNE; *fp = FNONE;
    }else if(compare_string(mnemonic,"addi")){
        *ip = ADDI; *fp = FNONE;
    }else if(compare_string(mnemonic,"lw")){
        *ip = LW; *fp = FNONE;
    }else if(compare_string(mnemonic,"sw")){
        *ip = SW; *fp = FNONE;
    }else{
        error_at(mnemonic.str,"mnemonic %.*s is invalid",mnemonic.len,mnemonic.str);
    }
    return;
}

int whatisthis_register(string reg){
    int regnum = strtol(reg.str,NULL,OP_RADIX);  //if string, return 0
    if(0 < regnum && regnum <= 31)return regnum;

    if(compare_string(reg,"zero")||
       compare_string(reg,"0"))     return 0;
    if(compare_string(reg,"at"))    return 1;
    if(compare_string(reg,"v0"))    return 2;
    if(compare_string(reg,"v1"))    return 3;
    if(compare_string(reg,"a0"))    return 4;
    if(compare_string(reg,"a1"))    return 5;
    if(compare_string(reg,"a2"))    return 6;
    if(compare_string(reg,"a3"))    return 7;
    if(compare_string(reg,"t0"))    return 8;
    if(compare_string(reg,"t1"))    return 9;
    if(compare_string(reg,"t2"))    return 10;
    if(compare_string(reg,"t3"))    return 11;
    if(compare_string(reg,"t4"))    return 12;
    if(compare_string(reg,"t5"))    return 13;
    if(compare_string(reg,"t6"))    return 14;
    if(compare_string(reg,"s7"))    return 15;
    if(compare_string(reg,"s0"))    return 16;
    if(compare_string(reg,"s1"))    return 17;
    if(compare_string(reg,"s2"))    return 18;
    if(compare_string(reg,"s3"))    return 19;
    if(compare_string(reg,"s4"))    return 20;
    if(compare_string(reg,"s5"))    return 21;
    if(compare_string(reg,"s6"))    return 22;
    if(compare_string(reg,"s7"))    return 23;
    if(compare_string(reg,"t8"))    return 24;
    if(compare_string(reg,"t9"))    return 25;
    if(compare_string(reg,"k0"))    return 26;
    if(compare_string(reg,"k1"))    return 27;
    if(compare_string(reg,"gp"))    return 28;
    if(compare_string(reg,"sp"))    return 29;
    if(compare_string(reg,"fp"))    return 30;
    if(compare_string(reg,"ra"))    return 31;

    error_at(reg.str,"register %.*s or %d is invalid\n",reg.len,reg.str,regnum);
}
Instr *get_designated_operands(InstrType itype,FunctType ftype,char **pp){
    char *p = *pp;
    string operand = {p,0};
    Instr *instr = calloc(1,sizeof(Instr));
    instr->instr = itype;
    instr->funct = ftype;

    switch(itype){  
    case    RTYPE:     //ex) add $3,$3, $4
        //read rd
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rd = whatisthis_register(operand);
        p+=operand.len;
        while(*p == ' '|| *p == '\t' || *p == ',')p++;
        //read rs
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rs = whatisthis_register(operand);
        p+=operand.len;
        while(*p == ' '|| *p == '\t' || *p == ',')p++;
        //read rt
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rt = whatisthis_register(operand);
        p+=operand.len;
        break;
    case        J:     //ex) j 0
    case      JAL:     //ex) jal func 
        if(isdigit(*p)){
            instr->jaddress = strtol(p,&p,OP_RADIX);
            if( !(0 <= instr->jaddress && instr->jaddress < depth) )
                error_at(p,"%d is invalid address\n",instr->jaddress);
        }else{
            instr->label = calloc(1,sizeof(string));
            instr->label->str = p;
            instr->label->len = count_strlen(p,0);
            p += instr->label->len;
        }
        break;    
    case      BEQ:     //ex) beq $3, $0, loop
    case      BNE:
        //read rt
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rt = whatisthis_register(operand);
        p+=operand.len;
        p = expect(p,',');
        while(*p == ' '|| *p == '\t' )p++;
        //read rs
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rs = whatisthis_register(operand);
        p+=operand.len;
        p = expect(p,',');
        while(*p == ' '|| *p == '\t' )p++;
    
        instr->label = calloc(1,sizeof(string));    
        instr->label->str = p;
        instr->label->len = count_strlen(p,0);
        p += instr->label->len;
        break;
    case     ADDI:     //ex) addi $9 ,$9, 78 
        //read rt
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rt = whatisthis_register(operand);
        p+=operand.len;
        p = expect(p,',');
        while(*p == ' '|| *p == '\t' )p++;
        //read rs
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rs = whatisthis_register(operand);
        p+=operand.len;
        p = expect(p,',');
        while(*p == ' '|| *p == '\t' )p++;
        
        //read immadiate number
        instr->imm = strtol(p,&p,OP_RADIX);
        break;    
    case       LW:     //ex) lw $4, 100($2)
    case       SW:
        //read rt
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rt = whatisthis_register(operand);
        p+=operand.len;
        p = expect(p,',');
        while(*p == ' '|| *p == '\t' )p++;
        
        //read offset
        instr->imm = strtol(p,&p,OP_RADIX);

        p = expect(p,'(');
        //read rs
        p = expect(p,'$');
        operand.str = p;
        operand.len = count_strlen(p,2);
        instr->rs = whatisthis_register(operand);
        p+=operand.len;

        p = expect(p,')');
        break;    
    default:
        error_at(p,"InstrType:%d is unsupported",(int)itype);
        break;
    }
    *pp = p; 
    return instr;
}