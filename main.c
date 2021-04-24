#include "assembler.h"
int depth = 128;
char *user_input;
char *filename;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
void error_at(char *loc,char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);       //get any arguments from next fmt

    //get start and end pointers of the line where the error occurred
    char *line = loc;
    char *end = loc;
    while(user_input < line && line[-1] != '\n')
        line--;
    while(*end != '\n')     //end is going to point '\n' 100%
        end++;
    
    //get the error line number
    int line_num = 1;
    for(char *p = user_input; p < line; p++){
        if(*p=='\n')
            line_num++;
    }

    //print file name ,line number ,and line
    int indent = fprintf(stderr,"error at %s:%d: ",filename,line_num);
    fprintf(stderr,"%.*s",(int)(end-line),line);
    fprintf(stderr, "\n");

    //print error message
    int pos = indent + (loc - line);
    fprintf(stderr,"%*s^",pos,"");
    vfprintf(stderr,fmt,ap);
    fprintf(stderr, "\n");
    exit(1);
}
char *read_file(char *path){
    FILE *fp = fopen(path, "r");
    if(!fp)
        error("cannot open %s : %s",path,strerror(errno));

    //check file size
    if(fseek(fp,0,SEEK_END) == -1)
        error("%s: fseek: %s",path,strerror(errno));
    size_t size = ftell(fp);
    if(fseek(fp,0,SEEK_SET) == -1)
        error("%s: fseek: %s",path,strerror(errno));
    
    //read file
    char *buf = calloc(1,size+2);
    fread(buf,size,1,fp);

    //set end of buffer "\n\0"
    if(size==0 || buf[size-1] != '\n')
        buf[size] = '\n';
    buf[size+1] = '\0';
    fclose(fp);
    return buf;
}
void get_filename(char *seek){
    char *tmp_filename;
    while(*seek){
        tmp_filename = seek;
        do{
            seek++;
        }while(*seek != '/'&&*seek);
    }
    if(*tmp_filename == '/')
        filename = tmp_filename+1;
    else filename = tmp_filename;
    return;
}

int main(int argc, char *argv[]){
    if(argc != 3 && argc != 2)
        error("we need only 1 or 2 arguments");

    user_input = read_file(argv[1]);
    get_filename(argv[1]);
    if(argc == 3) depth = strtol(argv[2],NULL,10);

    Instr *memory[depth];
    for(int i=0;i<depth;i++){
        memory[i] = calloc(1,sizeof(Instr));
        memory[i]->instr = INONE;
        memory[i]->funct = FNONE;
        memory[i]->label = NULL;
        if(memory[i] == NULL){
            for(int j=0;j<i;j++)    free(memory[i]);
            error("failed to get memory[%d]",i);
        } 
    }
    parse(user_input,memory);   

    printf("WIDTH=32;\n");
    printf("DEPTH=%d;\n",depth);
    printf("ADDRESS_RADIX=UNS;\n");
    printf("DATA_RADIX=HEX;\n\n");

    printf("CONTENT BEGIN\n");

        codegen(memory);
    
    printf("END;\n");

    for(int i=0;i<depth;i++) free(memory[i]);   
    return 0;
}
