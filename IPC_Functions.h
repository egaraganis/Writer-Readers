// Packet that share memory transfers
typedef struct{
	int value; // the integer value that can be passed
	struct timeval time_sent; // the timestamp
}packet;

int sem_create(void);

void sem_wait(int semid, int index , int n);

void sem_signal(int semid, int index , int n);

void print_array(int* array,int size,pid_t pid);

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

void timeval_print(struct timeval *tv);