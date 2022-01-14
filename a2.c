#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include "a2_helper.h"

typedef struct {
    sem_t *sem;
    int id;
}TH_STRUCT;

sem_t sem1, sem2;


void * thread_fn_1(void * param)
{
    TH_STRUCT *s = (TH_STRUCT*) param;
 
    if(s->id == 3)
    {
       sem_wait(&sem1);
    }

    info(BEGIN, 5, s->id);
    
    if(s->id == 4){
        sem_post(&sem1);
        sem_wait(&sem2);
    }

    info (END, 5, s->id);
    if(s->id == 3)
    {
        sem_post(&sem2);
    }
    
    return NULL;
}
void * thread_fn_2(void * param)
{
    TH_STRUCT *s = (TH_STRUCT*) param;
    sem_wait(s->sem);
    info(BEGIN, 4, s->id);
    info(END, 4, s->id);
    sem_post(s->sem);
    return NULL;
}
void * thread_fn_3(void * param)
{
    TH_STRUCT *s = (TH_STRUCT*) param;
    if(s->id == 3)
    {
        info(BEGIN, 7, s->id);
        info(END, 7, s->id);
    }
    else if(s->id == 4){
        info(BEGIN, 7, s->id);
        info(END, 7, s->id);
    }
    else{
        info(BEGIN, 7, s->id);
        info(END, 7, s->id);
    }
    return NULL;
}
int main(){
    init();
    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9;

    info(BEGIN, 1, 0);
    
    pid7 = fork();   
    if(pid7 < 0)
    {
        perror("Could not create process");
        return -1;
    }
    else if(pid7 == 0)
    {
        info(BEGIN, 7, 0);
        pthread_t t[4];
        TH_STRUCT cnt[4];
        for(int i = 0; i < 4; i++)
        {
            cnt[i].id = i + 1;
            pthread_create(&t[i], NULL, thread_fn_3, &cnt[i]);
        }
        for(int i = 0; i < 4; i++)
        {
            pthread_join(t[i], NULL);
        }                             
        info(END, 7, 0);
    }
    else{
        wait(NULL);
        pid3 = fork();
        if(pid3 < 0)
        {
            perror("Could not create process");
            return -1;
        }
        else if(pid3 == 0)
        {
            info(BEGIN, 3, 0);

           
            info(END, 3, 0);
        }
        else{
            wait(NULL);
            pid2 = fork();
            if(pid2 < 0)
            {
                perror("Could not create process");
                return -1;
            }
            else if(pid2 == 0)
            {
                info(BEGIN, 2, 0);
                pid4 = fork();
                if(pid4 < 0)
                {
                    perror("Could not create process");
                    return -1;
                }
                else if(pid4 == 0)
                {
                    info(BEGIN, 4, 0);

                    pid9 = fork();
                    if(pid9 < 0)
                    {
                        perror("Could not create process");
                        return -1;
                    }
                    else if(pid9 == 0)
                    {
                        info(BEGIN, 9, 0);
                        
                    
                        info(END, 9, 0);
                    }
                    else{
                        wait(NULL);
                        pid8 = fork();
                        if(pid8 < 0)
                        {
                            perror("Could not create process");
                            return -1;
                        }
                        else if(pid8 == 0)
                        {
                            info(BEGIN, 8, 0);
                            
                        
                            info(END, 8, 0);
                        }
                        else{
                            wait(NULL);
                            pid5 = fork();
                            if(pid5 < 0)
                            {
                                perror("Could not create process");
                                return -1;
                            }
                            else if(pid5 == 0)
                            {
                                info(BEGIN, 5, 0);
                                pid6 = fork();
                                if(pid6 < 0)
                                {
                                    perror("Could not create process");
                                    return -1;
                                }
                                else if(pid6 == 0)
                                {
                                    info(BEGIN, 6, 0);
                                    
                                
                                    info(END, 6, 0);
                                }
                                else{
                                    wait(NULL);
                                    sem_init(&sem1, 0, 0);
                                    sem_init(&sem2, 0, 0);
                                    pthread_t t[4];
                                    TH_STRUCT cnt[4];
                                    for(int i = 0; i < 4; i++)
                                    {
                                        cnt[i].id = i + 1;
                                        pthread_create(&t[i], NULL, thread_fn_1, &cnt[i]);
                                    }
                                    for(int i = 0; i < 4; i++)
                                    {
                                        pthread_join(t[i], NULL);
                                    }
                                    info(END, 5, 0);
                                }
                            }
                            else{
                                wait(NULL);
                                sem_t sem;
                                sem_init(&sem, 0, 4);
                                pthread_t t[36];
                                TH_STRUCT cnt[36];
                                for(int i = 0; i < 36; i++)
                                {
                                    cnt[i].id= i + 1;
                                    cnt[i].sem = &sem;
                                    pthread_create(&t[i], NULL, thread_fn_2, &cnt[i]);
                                }
                                for(int i = 0; i < 36; i++)
                                {
                                    pthread_join(t[i], NULL);
                                }
                                info(END, 4, 0);
                            }
                        }
                    }
                }
                else{
                    wait(NULL);
                    info(END, 2, 0);
                }
            }
            else{
                wait(NULL);
                info(END, 1, 0);
            }
        }
    }
    
    return 0;
}
