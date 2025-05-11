#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int pwd_main() {
    // Write your code here
    // Do not write a main() function. Instead, deal with pwd_main() as the main function of your program.
    char *path = getcwd(NULL , 0);
    if(path){
        printf("%s\n" , path);
        free(path);
    }else{
        printf("Couldn't PWD\n");
    }
}

int echo_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with echo_main() as the main function of your program.
    for(int i = 1;i < argc;i++){
        if(argv[i] == NULL)break;
        printf("%s",argv[i]);
        if(i != argc - 1)printf(" ");
    }
    puts("");
    return 0;
}
int cp_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with cp_main() as the main function of your program.
    if(argc != 3)return -1;
    char *src = argv[1];
    char *dst = argv[2];
    
    int src_fd = open(src , O_RDONLY);
    if(src_fd < 0)return -1;
    
    int dst_fd = open(dst , O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(dst_fd < 0)return -1;
    
    const int BUFF_SIZE = 1<<12; // 4kb
    char BUFF[BUFF_SIZE];
    int bytes_read = 0;
    while((bytes_read = read(src_fd ,BUFF , BUFF_SIZE)) > 0){
        while(bytes_read > 0){
            int bytes_written = 0;
            char *ptr = BUFF;
            bytes_written = write(dst_fd ,ptr ,bytes_read);
            if(bytes_written == 0)return -1;
            ptr += bytes_written;
            bytes_read -= bytes_written;
        }
    }
    return 0;
}

int mv_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with mv_main() as the main function of your program.
    if(argc != 3)return -1;
    char *src = argv[1];
    char *dst = argv[2];
    
    int src_fd = open(src , O_RDONLY);
    if(src_fd < 0)return -1;
    
    int dst_fd = open(dst , O_WRONLY | O_CREAT | O_TRUNC , 0666);
    if(dst_fd < 0)return -1;
    
    int BS = 1<<12;
    char BUFF[BS];
    int bytes_read = 0;
    while((bytes_read = read(src_fd , BUFF ,BS)) > 0){
        char *ptr = BUFF;
        int bytes_written = 0;
        while(bytes_read > 0){
            bytes_written = write(dst_fd , ptr , bytes_read);
            if(bytes_written == 0)return -1;
            ptr += bytes_written;
            bytes_read -= bytes_written;
        }
    }
    if(unlink(src) < 0)return -1;
    return 0;
}
