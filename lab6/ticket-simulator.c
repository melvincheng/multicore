// 5 100 10
//		real - 0.05s
//		cpu - 0.09s
// 5 2500 25
//		real - 0.340s
//		cpu - 0.350s
// 50 2500 25
//		real 0.370s
//		cpu - 0.394s

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int tickets_sold = 0;

pthread_mutex_t lock;

typedef struct {
	int tid;
	int available;
} info;

void * ticket_agent(void * arg);
void sell(info arg);

int main(int argc, char *argv[]){
	if(argc < 4){
		printf("Not enough arguments\n");
		return 1;
	}

	int num_agents = atoi(argv[1]);
	int seats = atoi(argv[2]);
	int over = atoi(argv[3]);

	pthread_t agent[num_agents];
	pthread_attr_t attr;
	info information[num_agents];

	pthread_mutex_init(&lock, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int tickets_available = seats + seats * over * 0.01;

	for(int i = 0; i < num_agents; i++){
		information[i].tid = i;
		information[i].available = tickets_available;
		pthread_create(&agent[i], &attr, ticket_agent, &information[i]);
	}

	for(int i = 0; i < num_agents; i++){
		pthread_join(agent[i], NULL);
	}

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&lock);

	printf("Total tickets sold: %d\n", tickets_sold);

	pthread_exit(NULL);
}

void * ticket_agent(void * arg){
	int ran_num;

	info * information = (info*) arg;

	while(1){
		ran_num = rand() % 100;
		sleep(ran_num/10000);
		pthread_mutex_lock(&lock);
		if(tickets_sold < information->available){
			if(((information->tid % 2) == 1) && (ran_num < 30)){
				sell(*information);
			}else if(((information->tid % 2) == 0) && (ran_num < 45)){
				sell(*information);
			}else{
				printf("Ticket agent <%d>: Unsuccessful transaction\n", information->tid);
			}
		}else{
			pthread_mutex_unlock(&lock);
			break;
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(NULL);
}

void sell(info arg){
	int sold = rand() % 3 + 1;
	if(tickets_sold + sold > arg.available){
		sold = arg.available - tickets_sold;
	}
	tickets_sold += sold;

	printf("Ticket agent <%d>: Successful transaction - <%d> tickets sold\n", arg.tid, sold);

}