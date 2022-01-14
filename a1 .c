#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

int ebine = 0;

int displayAllFiles(char * path)
{
    DIR* dir;
    struct dirent *entry;

    if ((dir=opendir(path)) != NULL) 
    {
        printf("SUCCESS\n");
    
        while ( (entry = readdir(dir)) != NULL) 
        {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            {
                printf("%s/%s\n", path, entry->d_name);
            }  
        } 
    }
    else{
        perror("Could not open this file!");
        return -1;
    }
    closedir(dir);
    return 0;  
}

void displayRecursiveAllFiles( char * path)

{
    DIR* dir;
    struct dirent *entry;
    char fullPath[512];
    struct stat statbuf;

    if ((dir=opendir(path)) == NULL) 
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }
    if(ebine == 0)
    {
        printf("SUCCESS\n");
        ebine = 1;
    }
    while ( (entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".")!=0 && strcmp(entry->d_name, "..")!=0)
        {
            snprintf(fullPath,512,"%s/%s", path, entry->d_name);
            stat(fullPath, &statbuf);
            printf("%s\n", fullPath);
            if(S_ISDIR(statbuf.st_mode))
            {
                displayRecursiveAllFiles(fullPath);
            }
            
        }  
    } 
    closedir(dir);
    
}

void sizeSmaller(char * path, int s, int rec)
{
    DIR* dir;
    struct dirent *entry;
    char fullPath[512];
    struct stat statbuf;
   
    if ((dir=opendir(path)) == NULL) 
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }
    if(ebine == 0)
    {
        printf("SUCCESS\n");
        ebine = 1;
    }
    while ( (entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
                if(S_ISREG(statbuf.st_mode))
                {
                    if(statbuf.st_size <=s )
                    {
                        printf("%s\n",fullPath);
                    }
                }
                if(rec == 1)
                {
                    if(S_ISDIR(statbuf.st_mode))
                    {
                        sizeSmaller(fullPath, s, rec);
                    }  
                }
            }
        }  
    } 
    closedir(dir);
}

void permWrite(char * path, int rec)
{
    DIR* dir;
    struct dirent *entry;
    char fullPath[512];
    struct stat statbuf;
   
    if ((dir=opendir(path)) == NULL) 
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }
    if(ebine == 0)
    {
        printf("SUCCESS\n");
        ebine = 1;
    }
    while ( (entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
              
                if(statbuf.st_mode & S_IWUSR)
                {
                    printf("%s\n",fullPath);
                }
                
                if(rec == 1)
                {
                    if(S_ISDIR(statbuf.st_mode))
                    {
                        permWrite(fullPath, rec);
                    }  
                }
            }
        }  
    } 
    closedir(dir);
}

void parsare( char * path)
{
    int fd= open(path, O_RDONLY);
    if (fd==-1){
        perror("Could not open file");
        return;
    }
    int header_size = 0;
    int no_of_section = 0;
    int version = 0;
    char sect_name[11];
    int sect_type = 0;
    int sect_offset = 0;
    int sect_size = 0;
    char magic[5] ={0};

    lseek(fd, -6 ,SEEK_END);
    if(read(fd, &header_size, 2)!=2)
    {
        printf("Could not read!\n");
        return;
    }
    if(read(fd, &magic ,4) != 4)
    {
        printf("Could not read!\n");
        return;
    }
    magic[4] = 0;
    //printf("%s\n", magic);
    if(strcmp(magic,"cwTI") != 0 )
    {   
        printf("ERROR\nwrong magic");
        return;
    }
     
    lseek(fd, -header_size,SEEK_END);
    if(read(fd, &version, 1) !=1)
    {
        printf("Could not read!\n");
        return;
    }
    
    if(version < 33 || version > 78)
    {
        printf("ERROR\nwrong version");
        return;
    }
    if(read(fd, &no_of_section, 1) != 1)
    {
        printf("Could not read!\n");
        return;
    }
    
    if(no_of_section < 2 || no_of_section > 13)
    {
        printf("ERROR\nwrong sect_nr");
        return;
    }
    
    for(int i = 0; i < no_of_section; i++)
    {
        if(read(fd, &sect_name,11) != 11)
        {
            printf("Could not read!\n");
            return;
        } 
        if(read(fd, &sect_type, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_offset, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_size, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(sect_type != 87 && sect_type != 38 && sect_type != 40)
        {
            printf("ERROR\nwrong sect_types");
            return;
        } 
    }
    
    lseek(fd, -header_size, SEEK_END);
    lseek(fd, 2, SEEK_CUR);
    printf("SUCCESS\nversion=%d\nnr_sections=%d\n",version,no_of_section);

    for(int i = 0; i < no_of_section; i++)
    {
        if(read(fd, &sect_name,11) != 11)
        {
            printf("Could not read!\n");
            return;
        } 
        if(read(fd, &sect_type, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_offset, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_size, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        printf("section%d: %s %d %d\n", i+1,sect_name, sect_type,sect_size);
        
    }
}

void extract(char * path, int section, int line)
{
    int fd= open(path, O_RDONLY);
    if (fd==-1){
        perror("ERROR\ninvalid file");
        return;
    }
    int header_size = 0;
    int no_of_section = 0;
    int version = 0;
    char sect_name[11];
    int sect_type = 0;
    int sect_offset = 0;
    int sect_size = 0;
    char magic[5] ={0};

    lseek(fd, -6 ,SEEK_END);
    if(read(fd, &header_size, 2)!=2)
    {
        printf("Could not read!\n");
        return;
    }
    if(read(fd, &magic ,4) != 4)
    {
        printf("Could not read!\n");
        return;
    }
    magic[4] = 0;
    
    if(strcmp(magic,"cwTI") != 0 )
    {   
        printf("ERROR\ninvalid file");
        return;
    }
     
    lseek(fd, -header_size,SEEK_END);
    if(read(fd, &version, 1) !=1)
    {
        printf("Could not read!\n");
        return;
    }
    
    if(version < 33 || version > 78)
    {
        printf("ERROR\ninvalid file");
        return;
    }
    if(read(fd, &no_of_section, 1) != 1)
    {
        printf("Could not read!\n");
        return;
    }
    
    if(no_of_section < 2 || no_of_section > 13)
    {
        printf("ERROR\ninvalid section");
        return;
    }
    
    for(int i = 0; i < section; i++)
    {
        if(read(fd, &sect_name,11) != 11)
        {
            printf("Could not read!\n");
            return;
        } 
        if(read(fd, &sect_type, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_offset, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(read(fd, &sect_size, 4) != 4)
        {
            printf("Could not read!\n");
            return;
        }
        if(sect_type != 87 && sect_type != 38 && sect_type != 40)
        {
            printf("ERROR\ninvalid section");
            return;
        } 
    }
    lseek(fd, sect_offset + sect_size -1, SEEK_SET);
    char c1, c2;
    int j = 1;
    int ebine = 0;
    for(int i = 0; i < sect_size; i++)
    {
        read(fd, &c1, 1);
        read(fd, &c2, 1);
        lseek(fd, -3, SEEK_CUR);
        if(c1 == 13 && c2 == 10)
        {
            j++;
        }
        if(j == line)
        {
            ebine = 1;
            //printf("%c", c1);
        }
    }
    if(ebine == 1)
    {
        printf("SUCCESS\n");
    }
    else{
        printf("ERROR\ninvalid line\n");
        return;
    }
    lseek(fd, sect_offset + sect_size -1, SEEK_SET);
    j = 1;
    for(int i=0; i< sect_size; i++)
    {
        read(fd, &c1, 1);
        read(fd, &c2, 1);
        lseek(fd, -3, SEEK_CUR);
        if(c1 == 13 && c2 == 10)
        {
            j++;
        }
        if(j == line)
        {
            printf("%c", c1);
        }
    }  
    close(fd);     
}

int cateLinii(char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd ==-1){
        perror("Could not open file");
        return -1;
    }

    int fd2= open(path, O_RDONLY);
    if (fd2==-1){
        perror("Could not open file");
        return -1;
    }
    int header_size = 0;
    int no_of_section = 0;
    int version = 0;
    char sect_name[11];
    int sect_type = 0;
    int sect_offset = 0;
    int sect_size = 0;
    char magic[5] ={0};

    lseek(fd, -6 ,SEEK_END);
    if(read(fd, &header_size, 2)!=2)
    {
        printf("Could not read!\n");
        return -1;
    }
    if(read(fd, &magic ,4) != 4)
    {
        printf("Could not read!\n");
        return -1;
    }
    magic[4] = 0;
    //printf("%s\n", magic);
    if(strcmp(magic,"cwTI") != 0 )
    {   
        printf("ERROR\nwrong magic");
        return -1;
    }
     
    lseek(fd, -header_size,SEEK_END);
    if(read(fd, &version, 1) !=1)
    {
        printf("Could not read!\n");
        return -1;
    }
    
    if(version < 33 || version > 78)
    {
        printf("ERROR\nwrong version");
        return -1;
    }
    if(read(fd, &no_of_section, 1) != 1)
    {
        printf("Could not read!\n");
        return -1;
    }
    
    if(no_of_section < 2 || no_of_section > 13)
    {
        printf("ERROR\nwrong sect_nr");
        return -1;
    }
    
    for(int i = 0; i < no_of_section; i++)
    {
        if(read(fd, &sect_name,11) != 11)
        {
            printf("Could not read!\n");
            return -1;
        } 
        if(read(fd, &sect_type, 4) != 4)
        {
            printf("Could not read!\n");
            return -1;
        }
        if(read(fd, &sect_offset, 4) != 4)
        {
            printf("Could not read!\n");
            return -1;
        }
        if(read(fd, &sect_size, 4) != 4)
        {
            printf("Could not read!\n");
            return -1;
        }
        if(sect_type != 87 && sect_type != 38 && sect_type != 40)
        {
            printf("ERROR\nwrong sect_types");
            return -1;
        } 
    }
    lseek(fd2, sect_offset + sect_size -1, SEEK_SET);
    char c1, c2;
    int j = 1;
    
    for(int i = 0; i < sect_size; i++)
    {
        read(fd2, &c1, 1);
        read(fd2, &c2, 1);
        lseek(fd2, -3, SEEK_CUR);
        if(c1 == 13 && c2 == 10)
        {
            j++;
        }
    }
    return j;
    printf("%d\n",j);
    close(fd);
    close(fd2);

}

int findall(char * path)
{ 
    DIR* dir;
    struct dirent *entry;
    char fullPath[512];
    struct stat statbuf;
    
    if ((dir=opendir(path)) == NULL) 
    {
        printf("ERROR\ninvalid directory path\n");
        return -1;
    }
    if(ebine == 0)
    {
        printf("SUCCESS\n");
        ebine = 1;
    }
    while ( (entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
                if(S_ISREG(statbuf.st_mode))
                {
                    printf("%s\n",fullPath);
                }
                if(S_ISDIR(statbuf.st_mode))
                {
                    findall(fullPath);  
                }
            }
        }  
    } 
    closedir(dir);
    return 1;
}

int main(int argc, char **argv){
    if(argc >= 2){

        if(strcmp(argv[1], "variant") == 0){
            printf("16944\n");
        }
        if(strcmp(argv[1],"list") == 0)
        {
            char * p = (char*) malloc(strlen(argv[argc -1])*sizeof(char)-4);
            int size1 =-1;
            int i = 2;
            int rec = 0;
            int ss = 0;
            int wr = 0;
            while(i<argc)
            {
                if((strcmp(argv[i],"recursive")==0))
                {
                    rec = 1;
                }
                if(strstr(argv[i], "size_smaller=") !=NULL){
                    ss = 1;
                    size1 = atoi(argv[i] + 13);
                }
                if(strcmp(argv[i],"has_perm_write") == 0)
                {
                    wr = 1;
                }
                i++;
            }
            
            strcpy(p,argv[argc-1]+5);
            
            if( ss == 1)
            {
                sizeSmaller(p,size1,rec);
                
            }
            else if(wr == 1)
            {
                permWrite(p,rec);
            }
            else{
                if(rec == 1)
                {
                    displayRecursiveAllFiles(p);
                }
                else{
                    displayAllFiles(p);
                }
            }
            free(p);
        }
    }

    if(strcmp(argv[1],"parse") == 0)
    {
        char * p = (char*) malloc(strlen(argv[2])*sizeof(char)-4);
        if(strstr(argv[2], "path="))
        {
            strcpy(p, argv[2] + 5);
            parsare(p);
        }
        free(p);
    }

    if(strcmp(argv[1], "extract") == 0)
    {
        char * p = (char*) malloc(strlen(argv[2])*sizeof(char)-4);
        if(strstr(argv[2],"path="))
        {
            strcpy(p, argv[2] + 5);
            if(strstr(argv[3], "section="))
            {
                if(strstr(argv[4],"line="))
                {
                    extract(p,atoi(argv[3] + 8),atoi(argv[4] + 5));
                }
            }
        }
        free(p);
    }
    if(strcmp(argv[1],"findall")==0)
    {
        char * p = (char*) malloc(strlen(argv[2])*sizeof(char)-4);
        if(strstr(argv[2],"path="))
        {
            strcpy(p, argv[2] + 5);
            if(findall(p) == 0)
            {
                printf("ERROR\ninvalid dyrectory path\n");
            }
        }
        free(p);
    }
    
    return 0;
}