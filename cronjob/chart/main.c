#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  int i;
  pid_t p;
  for(i=0; i<2; i++){
    if((p = fork()) == 0){
      sleep(10);
      exit(0);
    }
    sleep(1);
    printf("Process %i created\n",p);
  }
  while((p = wait(&i))!=-1){
    printf("pid: %i\n",p);
    }
printf("alle Prozesse beendet\n");
  exit(0);
}
