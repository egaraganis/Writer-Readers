#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>

#include "IPC_Functions.h"

#define father   0 // father
#define children 1 // children
#define lock     2 // lock

// creation of semaphores
int sem_create(void)
{  
  int semid;

  // create new semaphore set of 3 semaphores 
  if ((semid = semget (IPC_PRIVATE, 3, IPC_CREAT | 0600)) < 0)
  {  
    perror ("error in creating semaphore");/* 0600 = read/alter by user */
    exit (1);
  }

  // initialization of semaphores
  if (semctl (semid, father, SETVAL, 0) < 0)
    {  perror ("error in initializing first semaphore");
       exit (1);
    }

  if (semctl (semid, children, SETVAL, 0) < 0)
    {  perror ("error in initializing second semaphore");
       exit (1);
    }

  if (semctl (semid, lock, SETVAL, 0) < 0)
    {  perror ("error in initializing second semaphore");
       exit (1);
    }

  return semid;
}

// procedure to perform a sem_wait or wait operation on a semaphore of given index
void sem_wait(int semid, int index , int n)
{
  struct sembuf sops[1];  /* only one semaphore operation to be executed */

   sops[0].sem_num = index;/* define operation on semaphore with given index */
   sops[0].sem_op  = -1*n;   /* subtract n to value for P operation */
   sops[0].sem_flg = 0;    /* type "man semop" in shell window for details */

   if (semop (semid, sops, 1) == -1)
     {  perror ("error in semaphore operation");
        exit (1);
     }
}

// procedure to perform a sem_signal or signal operation on semaphore of given index
void sem_signal(int semid, int index , int n)
{
   struct sembuf sops[1];  /* define operation on semaphore with given index */

   sops[0].sem_num = index;/* define operation on semaphore with given index */
   sops[0].sem_op  = n;    /* add n to value for V operation */
   sops[0].sem_flg = 0;    /* type "man semop" in shell window for details */

   if (semop (semid, sops, 1) == -1)
     {  perror ("error in semaphore operation");
        exit (1);
     }
}

// helping function to print an array along with pid of caller
void print_array(int* array,int size,pid_t pid)
{
	int i;
	for (i = 0; i < size; i++)
	{
		printf("pid: %d , %d \n",pid,array[i]);
	}
}


// return 1 if the difference is negative, otherwise 0
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}