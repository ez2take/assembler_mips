#include "assembler.h"

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

    //print file name and line number and line
    int indent = fprintf(stderr,"error at %s:%d: ",filename,line_num);
    fprintf(stderr,"%.*s",(int)(end-line),line);

    //print error message
    int pos = indent + (loc - line);
    fprintf(stderr,"%*s^",pos,"");
    vfprintf(stderr,fmt,ap);
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
    buf[size++] = '\0';
    fclose(fp);
    return buf;
}
int main(int argc, char const *argv[]){
    
    return 0;
}
