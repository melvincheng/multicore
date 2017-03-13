#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#define NUM_THREADS 4
#define NUM_STOCKS 3
#define NUM_BROKER 3

//struct for stock on the exchange
struct stock {
  char name;
  float price;
  int count;
};

//struct for investor stock details
struct investor_stock {
  char broker_name;
  char name;
  float buy_price;
  float buy_count;
  float sell_price;
  float sell_count;
};

struct investor_portfolio {
  struct investor_stock s[NUM_STOCKS];
};

//declare shared variables and corresponding
//mutexes and condition variables
struct stock my_stock[NUM_STOCKS] = {{'A', 0.01, 1000},
                           {'B', 0.01, 500},
                           {'C', 0.01, 1000}} ;
pthread_mutex_t my_stock_mutex[NUM_STOCKS];
pthread_cond_t my_stock_price_cond[NUM_STOCKS];

int stock_exchange_exit = 1;

//declare routines for threads...
void *stock_broker(void *t);
void *stock_exchange(void *t);
void *stock_monitor(void *t);

void update_stock(int id, int chance, float price_var);


int main(int argc, char *argv[]) {
  struct investor_portfolio test[NUM_BROKER];
  struct investor_stock s0 = {'A', 'A', 1.10, 5, 100.00, 5};
  test[0].s[0] = s0;
  struct investor_stock s1 = {'A', 'B', -1, -1, -1, -1}; //no buy or sell
  test[0].s[1] = s1;
  struct investor_stock s2 = {'A', 'C', 5.00, 10, 10.00, 6};
  test[0].s[2] = s2;

  struct investor_stock s3 = {'B', 'A', 2.10, 3, 50.00, 1};
  test[1].s[0] = s3;
  struct investor_stock s4 = {'B', 'B', 4.20, 4, 42.00, 2};
  test[1].s[1] = s4;
  struct investor_stock s5 = {'B', 'C', 7.00, 9, 223.00, 8};
  test[1].s[2] = s5;

  struct investor_stock s6 = {'C', 'A', 9.00, 9, 90.00, 9};
  test[2].s[0] = s6;
  struct investor_stock s7 = {'C', 'B', 10.00, 7, 20.00, 3};
  test[2].s[1] = s7;
  struct investor_stock s8 = {'C', 'C', -1, -1, -1, -1};
  test[2].s[2] = s8;

  //dan.s[1] = {'B', 0.50, 242, 15.00, 242};

  int i, error;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;

  //Initialize mutex and condition variables
  for (i=0; i < NUM_STOCKS; i++) {
    pthread_mutex_init(&my_stock_mutex[i], NULL);
    pthread_cond_init(&my_stock_price_cond[i], NULL);
  }

  //Create threads that are joinable
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  error = pthread_create(&threads[0], &attr, stock_exchange, NULL);

  if (error) {
    printf("ERROR: create() monitor_stock_price: %d\n", error);
    exit(-1);
  }
  //Activity #3
  for(i=1; i < NUM_BROKER + 1; i++){
    error = pthread_create(&threads[i], &attr, stock_broker, (void *) &test[i - 1]);
    if (error) {
      printf("ERROR: create() buy_stock: %d\n", error);
      exit(-1);
    }
  }

  //wait for threads to complete/join
  for (i=0; i < NUM_THREADS; i++) {
    error = pthread_join(threads[i], NULL);
    if (error) {
      printf("ERROR: join() broker %d: %d\n", i, error);
      exit(-1);
    }
  }

  //reinitialize/destroy pthread variables
  pthread_attr_destroy(&attr);
  for (i=0; i < NUM_STOCKS; i++) {
    pthread_mutex_destroy(&my_stock_mutex[i]);
    pthread_cond_destroy(&my_stock_price_cond[i]);
    }
  pthread_exit(NULL);
}

void *stock_broker(void *t) { 
  struct investor_portfolio *data;
  int error;
  pthread_t threads[NUM_STOCKS];
  pthread_attr_t attr;

  data = (struct investor_portfolio*) t;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for(int i = 0; i < NUM_STOCKS; i++){
    error = pthread_create(&threads[i], &attr, stock_monitor, (void *) &data->s[i]);
    if (error) {
      printf("ERROR: create() stock_monitor: %d\n", error);
      exit(-1);
    }
  }
  for(int i = 0; i < NUM_STOCKS; i++){
    error = pthread_join(threads[i], NULL);
    if (error) {
      printf("ERROR: join() monitor %d: %d\n", i, error);
      exit(-1);
    }
  }
  pthread_exit(NULL);
}

void *stock_exchange(void *t) {
  //check the value of a stock and signal waiting threads
  //when a price is reached
  printf("Starting of trading on Stock \n");
  int day;
  srand(time(NULL));
  sleep(2); // waits for the monitor threads to start
  for (day=0; day < 365; day++) {
    printf("day = %d\n", day);
    update_stock(0, rand()%3, (rand()%10 - 3.6)/2.3);
    update_stock(1, rand()%7, (rand()%12 - 5)/2.3);
    update_stock(2, rand()%6, (rand()%7 - 1)/2.1);
    sleep(2);
  }
  stock_exchange_exit = 0;
  for(int i = 0; i < NUM_BROKER; i++){
    for(int j = 0; j < NUM_STOCKS; j++){
      pthread_cond_broadcast(&my_stock_price_cond[j]);
    }
  }
  pthread_exit(NULL);
}

void *stock_monitor(void *t){
  struct investor_stock *data;
  int id = -1;
  data = (struct investor_stock*) t;
  for(int i = 0; i < NUM_STOCKS; i++){
    if(data->name == my_stock[i].name){
      id = i;
      break;
    }
  }

  while(stock_exchange_exit){
    pthread_mutex_lock(&my_stock_mutex[id]);
    pthread_cond_wait(&my_stock_price_cond[id], &my_stock_mutex[id]);
    if(data->buy_price > 0 && my_stock[id].price < data->buy_price){
      if(my_stock[id].count > data->buy_count){
        my_stock[id].count -= data->buy_count;
      }
      printf("Broker: %c Stock %c: bought \n", data->broker_name, data->name);
    }else if(data->sell_count > 0 && my_stock[id].price > data->sell_price){
      my_stock[id].count += data->sell_count;
      printf("Broker: %c Stock %c: sold \n", data->broker_name, data->name);
    }else if(stock_exchange_exit){
      pthread_mutex_unlock(&my_stock_mutex[id]);  
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&my_stock_mutex[id]);
  }
  pthread_exit(NULL);
}

void update_stock(int id, int chance, float price_var) {
  if (chance == 0) {
    pthread_mutex_lock(&my_stock_mutex[id]);
    if(my_stock[id].price + price_var < 0.01){
      my_stock[id].price = 0.01;
    }else{
      my_stock[id].price += price_var;
    }

    pthread_cond_broadcast(&my_stock_price_cond[id]);
    printf("Stock price of %c: $%f\n", my_stock[id].name, my_stock[id].price);
    pthread_mutex_unlock(&my_stock_mutex[id]);
  }
}
