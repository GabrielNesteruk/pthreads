#include<stdio.h>
#include<stdlib.h> 
#include<pthread.h>
#include<unistd.h>
#include <time.h>
#include <string.h> 
#include <stdbool.h>

int all_cars = 0;
int queue_from_city_A = 0;
int queue_from_city_B = 0;
int cars_in_city_A = 0;
int cars_in_city_B = 0;

bool debug = false;

pthread_mutex_t bridge_mutex;
pthread_mutex_t queue_A_mutex;
pthread_mutex_t queue_B_mutex;

enum QueueType
{
    QueueA,
    QueueB
};

enum City
{
    CityA,
    CityB
};

struct Car
{
    int index;
    enum QueueType queue;
    enum City city;
    bool in_queue;
    struct Car* cars_ptr;
};

void city(struct Car** car_detail);
void* bridge(void* args);
void populateCity(int cars, struct Car* cars_array);
void showAdvancedDetails(struct Car* cars_array);

int main(int argc, char*argv[]){

    if(argc < 2){
        printf("Usage: ./program [number of cars] [optional -debug]\n");
        exit(0);
    }
    if(argc > 3)
        if(!strcmp(argv[2], "-debug"))
            debug = true;

    const int number_of_cars = atoi(argv[1]);
    all_cars = number_of_cars;
    struct Car cars_details[number_of_cars];

    populateCity(number_of_cars, cars_details);

    pthread_t cars[number_of_cars];
    pthread_mutex_init(&bridge_mutex, NULL);
    pthread_mutex_init(&queue_A_mutex, NULL);
    pthread_mutex_init(&queue_B_mutex, NULL);
    
    for(int i = 0; i < number_of_cars; i++)
    {
        pthread_create(&cars[i], NULL, bridge, &cars_details[i]);
    }

    for(int i = 0; i < number_of_cars; i++)
        pthread_join(cars[i], NULL);

    pthread_mutex_destroy(&bridge_mutex);
    pthread_mutex_destroy(&queue_A_mutex);
    pthread_mutex_destroy(&queue_B_mutex);

}

void* bridge(void* args)
{
    struct Car* car_details = (struct Car*)args;
    if(car_details->city == CityA)
    {
        pthread_mutex_lock(&queue_A_mutex);
        queue_from_city_A++;
        car_details->in_queue = true;
        cars_in_city_A--;
        pthread_mutex_unlock(&queue_A_mutex);
    }
    else
    {
        pthread_mutex_lock(&queue_B_mutex);
        queue_from_city_B++;
        car_details->in_queue = true;
        cars_in_city_B--;
        pthread_mutex_unlock(&queue_B_mutex);
    }
    pthread_mutex_lock(&bridge_mutex);
    city(&car_details);
    pthread_mutex_unlock(&bridge_mutex);
}

void city(struct Car** car_detail)
{
    if((**car_detail).queue == QueueA)
    {
        queue_from_city_A--;
        (**car_detail).in_queue = false;
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n", cars_in_city_A, queue_from_city_A, (**car_detail).index, queue_from_city_B, cars_in_city_B);
        showAdvancedDetails((**car_detail).cars_ptr);
        cars_in_city_B++;
    }
    else
    {
        queue_from_city_B--;
        (**car_detail).in_queue = false;
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n", cars_in_city_A, queue_from_city_A, (**car_detail).index, queue_from_city_B, cars_in_city_B);
        showAdvancedDetails((**car_detail).cars_ptr);
        cars_in_city_A++;
    }
    sleep(2);
}

void populateCity(int cars, struct Car* cars_array)
{
    srand(time(NULL));
    int r = (rand() % (cars - 1 + 1)) + 1;
    cars_in_city_A = r;
    cars_in_city_B = cars - r;
    for(int i = 0; i < cars; i++)
    {
        if(i < cars_in_city_A)
        {
            cars_array[i].city = CityA;
            cars_array[i].queue = QueueA;
        }
        else
        {
            cars_array[i].city = CityB;
            cars_array[i].queue = QueueB;
        }
        cars_array[i].index = i;
        cars_array[i].in_queue = false;
        cars_array[i].cars_ptr = cars_array;
    }
}

void showAdvancedDetails(struct Car* cars_array)
{
    printf("Cars in A queue:\n");
    for(int i = 0; i < all_cars; i++)
    {
        if(cars_array[i].in_queue == true && cars_array[i].queue == QueueA)
            printf("Car's Index: %d\n", cars_array[i].index);
    }

    printf("Cars in B queue:\n");
    for(int i = 0; i < all_cars; i++)
    {
        if(cars_array[i].in_queue == true && cars_array[i].queue == QueueB)
            printf("Car's Index: %d\n", cars_array[i].index);
    }
}