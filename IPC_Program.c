#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/sem.h>

#include <time.h>
#include "IPC_Functions.h"

// Semaphores for IPC - Writer-Readers
#define father   0 // father
#define children 1 // children
#define lock     2 // lock

int main(int argc,char* argv[])
{
	int n = -1; // number of processes
	int M = -1; // size of array
	int d = 0; // debug

	int i;
	// get arguments for the size of array and the number of processes we are going to use
	for (i = 0; i < argc; i++){
		if(!strcmp(argv[i],"-d"))
			d = 1;
		if(!strcmp(argv[i],"-n")){
			n = atoi(argv[++i]);
			if(n<=0)
				printf("number of processes should be >= 1\n");
		}
		if(!strcmp(argv[i],"-m")) {
			M = atoi(argv[++i]); 
			if(M<=3000)
				printf("array size should be >= 3000\n");
		}
	}
	printf("Running IPC program with %d processes for an %d sized array\n",n,M);

	// fill feeder's array with random values
	srand(time(NULL));
	int Internal_Array[M];
	for (i = 0; i < M; i++)
		Internal_Array[i] = rand() % 100;

	if(d){
		printf("------------------------\n");
		printf("Feeder Starting Array\n");
		print_array(Internal_Array,M,getpid());
		printf("------------------------\n");
	}

	// open our report file
	FILE *f = fopen("REPORT.txt", "w");
	if (f == NULL)
	    printf("Error opening file!\n");

	// creation of shared memory
	int shmid = shmget(IPC_PRIVATE,sizeof(packet),0666|IPC_CREAT); // packet contains one integer and a timestamp
	packet* pmem = shmat(shmid, NULL, 0);

	int semid = sem_create(); 	// create semaphores

	pid_t current_process;
	pid_t parent;
	for (i = 0; i < n; i++){	// create Cn processes
		current_process = fork();
		if (current_process<0) printf("Error in fork\n");
		else if (current_process == 0 ){
			current_process += i;
			break;
		}
		else parent = getpid();
	}

	int Array[M]; // array of Ci's process
	struct timeval sum_delay; // the sum of all delays goes here
	sum_delay.tv_sec = 0;
	sum_delay.tv_usec = 0;

	// Writer starts writing on shared memory , Readers read from it
	for(i = 0; i < M ; i++){
		// Writer - Feeder
		if(parent == getpid()){
			// write
			pmem->value = Internal_Array[i]; // write current integer
			gettimeofday(&(pmem->time_sent),NULL); // write current time sent
			
			// semaphores take care of IPC		
			sem_wait(semid,father,n); // father , wait for children to receive the knock
			sem_signal(semid,lock,n); // lock children in , till father finish writing 
			sem_wait(semid,father,n); // father waits for n resources (children reads)
			sem_signal(semid,children,n); // father give resources to n children
		}
		// Readers - Cn processes
		else{
			sem_signal(semid,father,1); // tell father that we heared the knock
			sem_wait(semid,lock,1); // locked till father finish the writing

			// read
			struct timeval time_received,time_diff;
			Array[i] = pmem->value; // read value
			gettimeofday(&time_received,NULL); // check current time

			// calculate delay , time difference
    		timeval_subtract(&time_diff, &time_received, &(pmem->time_sent)); // calculate time difference
    		sum_delay.tv_sec += time_diff.tv_sec;
    		sum_delay.tv_usec += time_diff.tv_usec;

			sem_signal(semid,father,1); // children returns one resource back (father will wait for n-1 childrens now to read)
			sem_wait(semid,children,1); // each children wait for its resource to read the next packet
		}
	}

	if(parent != getpid()){
		if (  current_process == n - 1){ // last process writes
			fprintf(f, "Integer Sequence\n---------------- \n");
			for(i = 0; i < M ; i++) // write integer sequence
				fprintf(f, "%d \n",Array[i]);
			
			fprintf(f, "\nPID: \n---------------- \n");
			fprintf(f, "%d \n",getpid());
			
			double sum = (sum_delay.tv_sec * 1000000.0 + sum_delay.tv_usec)/1000000; // from timeval to double
			double running_average = sum/M; // calculate running average
			
			//print_array(Array,M,getpid());
			printf("PID:%d , Running Average:%f\n", getpid(),running_average);

			fprintf(f, "\nRunning Average: \n---------------- \n");
			fprintf(f, "%f \n", running_average);
		}
		else{
			if (d){
				print_array(Array,M,getpid());
				double sum = (sum_delay.tv_sec * 1000000.0 + sum_delay.tv_usec)/1000000; // from timeval to double
				double running_average = sum/M; // calculate running average
				printf("PID:%d , Running Average:%f\n", getpid(),running_average);
			}
		}

	}
	else{
		// parent wait for your childrens
		for (i = 0; i < n; i++){
			wait(NULL);
		}

		// destruction of shared memory
		int err =  shmdt(pmem);
		int err1 = shmctl(shmid,0,IPC_RMID);

		// destruction of semaphores
		if (semctl (semid, 3, IPC_RMID) < 0){
		  	perror ("error in removing semaphore from the system");
	    	exit (1);
	    }
	    printf ("Semaphore cleanup complete.\n");
	}
}
