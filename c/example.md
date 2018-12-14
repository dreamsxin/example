* 网易云音乐解密

```c
// 命令格式: ./可执行文件 输入.uc! 输出.mp3
#include<stdio.h>
int main(int argc,char* arg[]){
    char temp_bytes[1024];
    unsigned int read_count,wrote_size=0;
    if(argc<3){
        return 1;
    }
    FILE* i=fopen(arg[1],"r");
    FILE* o=fopen(arg[2],"w");
    printf("InputFile:%s\r\n",arg[1]);
    printf("OutputFile:%s\r\n",arg[2]);
    if(!i || !o){
        printf("Cannot open either file!\r\n");
        return 1;
    }
    
    while((read_count=fread(temp_bytes,sizeof(char),1024,i))>0){
        unsigned int p;
        wrote_size+=read_count;
        printf("Cracking position:%u\r\n",wrote_size);
        for(p=0;p<read_count;p++){
            temp_bytes[p]^=0xa3;
        }

        fwrite(temp_bytes,sizeof(char),read_count,o);
        fflush(o);
        if(read_count!=1024){
            printf("This file has been cracked!\r\n");
            break;
        }
    }
    printf("Done!\r\n");
    fclose(i);
    fclose(o);
}
```