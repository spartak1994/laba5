#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
void getTime(struct tm *current_time)
{
time_t sec = 0;
time(&sec);
*current_time = *(localtime(&sec));
}
int main(int argc, char const *argv[])
{
struct tm time;
int status;
int shmid;
void *addr;
shmid = shmget(IPC_PRIVATE, sizeof(struct tm), IPC_CREAT | IPC_EXCL | 0666);
if(shmid == -1){
printf("Can't create SR MEM.\n");
return -1;
}
addr = shmat(shmid, NULL, 0);
if( addr == (void *)(-1) ){
printf("Can't attach SR MEM.\n");
return -1;
}
getTime(&time);
printf("Time in parent process %d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
switch( fork() ){
case -1:
printf("Can't create child process.\n");
return -1;
case 0:
sleep(1);
getTime(&time);
printf("Time in child process %d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
addr = shmat(shmid, NULL, 0);
if( addr == (void *)(-1) ){
printf("Can't attach SR MEM.\n");
exit(-1);
}
*((struct tm *)addr) = time;
exit(0);
default:
wait(&status);
time = *( (struct tm *)addr );
if( status == 0 ){
printf("Time from SR MEM %d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
}
shmdt(addr);
}
return 0;
}
