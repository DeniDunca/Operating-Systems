#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define RESP_PIPE "RESP_PIPE_16944"
#define REQ_PIPE "REQ_PIPE_16944"
#define CREATE "CREATE_SHM"
#define ERROR "ERROR"
#define SUCCESS "SUCCESS"

unsigned int connect = strlen("CONNECT");
unsigned int sizerq = 0;
unsigned int variant = 16944;
unsigned int shm = 2967607;
unsigned int create = strlen(CREATE);
unsigned int error = strlen(ERROR);
unsigned int success = strlen(SUCCESS);

int main()
{
    if(access(RESP_PIPE, 0) == 0) {
        unlink(RESP_PIPE);
    }
//2.2
    //1
    if(mkfifo(RESP_PIPE, 0600) != 0)
    {
        printf("ERROR\ncannot create the response pipe\n");
    
        return 1;
    }

    //2
    int fd_rd = open(REQ_PIPE, O_RDONLY);
    if(fd_rd == -1)
    {
        printf("ERROR\ncannot open the request pipe\n");
        return 1;
    }

    //3
    int fd_wt = open(RESP_PIPE, O_WRONLY);
    if(fd_wt == -1)
    {
        printf("ERROR\ncannot open the response pipe\n");
        close(fd_rd);
        return 1;
    }

    //4
    if((write(fd_wt, &connect, 1) != -1) && (write(fd_wt, "CONNECT", connect) != -1)) {
        printf("SUCCESS\n");
    }

//2.3
    int fd_shm = -1;
    int fd_map = -1;
    char * data;
    while(1)
    {
        read(fd_rd, &sizerq, 1);
        char* c = (char*)malloc(sizerq * sizeof(char) + 1);
        read(fd_rd, c, sizerq);
        c[sizerq] = 0;
    
        if(strcmp(c, "PING") == 0)
        {
            write(fd_wt, &sizerq, 1);
            write(fd_wt, "PING", 4);
            write(fd_wt, &sizerq, 1);
            write(fd_wt, "PONG", 4);
            write(fd_wt, &variant, sizeof(variant));
        }
        //2.10
        if(strcmp(c, "EXIT") == 0)
        {
            close(fd_wt);
            close(fd_rd);
            free(c);
            c = NULL;
            unlink(RESP_PIPE);
            return 0;
        }
        //2.4
        if(strcmp(c, "CREATE_SHM") == 0)
        {
            read(fd_rd, &shm, sizeof(int));
            fd_shm = shm_open("/elm5Wj3", O_CREAT | O_RDWR, 0664);
            
            if(fd_shm < 0)
            {
                write(fd_wt, &create ,1);
                write(fd_wt, CREATE, create);
                write(fd_wt, &error,1);
                write(fd_wt, ERROR, error);
                
            }
            else{
                ftruncate(fd_shm, shm);
                write(fd_wt, &create ,1);
                write(fd_wt, CREATE, create);
                write(fd_wt, &success,1);
                write(fd_wt, SUCCESS, success);
            }
        }

        //2.5
        if(strcmp(c, "WRITE_TO_SHM") == 0)
        {
            int offset, value;
            read(fd_rd, &offset, sizeof(unsigned int));
            read(fd_rd, &value, sizeof(unsigned int));

            if(offset > 0 && offset < 2967607 - sizeof(int))
            {
                lseek(fd_shm, offset, SEEK_SET);
                write(fd_shm, &value, sizeof(int));
                write(fd_wt, &sizerq, 1);
                write(fd_wt, c, sizerq);
                write(fd_wt, &success , 1);
                write(fd_wt, SUCCESS, success);
            }
            else{
                write(fd_wt, &sizerq, 1);
                write(fd_wt, c, sizerq);
                write(fd_wt, &error , 1);
                write(fd_wt, ERROR, error);
            }
        }

        //2.6
        if(strcmp(c, "MAP_FILE") == 0)
        {
            int size_f = 0;
            read(fd_rd, &size_f, 1);
            char * file = (char*)malloc(size_f * sizeof(char) + 1);
            read(fd_rd, file, size_f);
            file[size_f] = 0;
            fd_map = open(file, O_RDONLY);

            if(fd_map == -1)
            {
                write(fd_wt, &sizerq, 1);
                write(fd_wt, c, sizerq);
                write(fd_wt, &error , 1);
                write(fd_wt, ERROR, error);
            }
            else{
                int size = lseek(fd_map, 0, SEEK_END);
                lseek(fd_map, 0, SEEK_SET);
                data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd_map, 0);
                if(data == (void*) - 1)
                {
                    write(fd_wt, &sizerq, 1);
                    write(fd_wt, c, sizerq);
                    write(fd_wt, &error , 1);
                    write(fd_wt, ERROR, error);
                }
                else{
                    write(fd_wt, &sizerq, 1);
                    write(fd_wt, c, sizerq);
                    write(fd_wt, &success , 1);
                    write(fd_wt, SUCCESS, success);
                }
            }
        }

        //2.7
        if(strcmp(c, "READ_FROM_FILE_OFFSET") == 0)
        {
            off_t offset = 0;
            int nr = 0;

            read(fd_rd, &offset, sizeof(int));
            read(fd_rd, &nr, sizeof(int));
            write(fd_wt, &sizerq, sizeof(char));
            write(fd_wt, c, sizerq);
            int size = lseek(fd_map, 0, SEEK_END);
            lseek(fd_map, 0, SEEK_SET);

            if( offset + nr > size)
            {
                write(fd_wt, ERROR, error);
            }
            else{
                //printf("%lx\n", offset);
                //printf("%c\n", data[offset]);
                char* str = (char*)malloc(sizeof(char) * nr + 1);
                strncpy(str,data + offset, nr);
                str[nr] = 0;
                //printf("data: %s\n", str);
                if(write(fd_shm, str, nr) != nr)
                {
                    write(fd_wt, ERROR, error);
                }
                else{
                    write(fd_wt, SUCCESS, success);
                }
                lseek(fd_shm, 0, SEEK_SET);
                free(str);
            }
            
        }
    }
}