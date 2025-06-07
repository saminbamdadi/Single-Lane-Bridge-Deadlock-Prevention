#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_NORTH 5
#define NUM_SOUTH 5

pthread_mutex_t direction_mutex;
sem_t bridge_sem;

int current_direction = -1;
int on_bridge = 0;

void enter_bridge(int dir)
{
    while (1)
    {
        pthread_mutex_lock(&direction_mutex);
        if (current_direction == -1 || current_direction == dir)
        {
            if (on_bridge == 0)
                current_direction = dir;

            on_bridge++;
            pthread_mutex_unlock(&direction_mutex);
            break;
        }
        pthread_mutex_unlock(&direction_mutex);
        usleep(1000);
    }

    sem_wait(&bridge_sem);
}

void exit_bridge(int dir)
{
    pthread_mutex_lock(&direction_mutex);
    on_bridge--;
    if (on_bridge == 0)
        current_direction = -1;
    pthread_mutex_unlock(&direction_mutex);

    sem_post(&bridge_sem);
}

void *north_farmer(void *arg)
{
    int id = *(int *)arg;
    printf("North farmer %d wants to cross the bridge.\n", id);
    enter_bridge(0);
    printf("North farmer %d is crossing the bridge...\n", id);
    sleep(rand() % 3 + 1);
    printf("North farmer %d has crossed the bridge.\n", id);
    exit_bridge(0);
    return NULL;
}

void *south_farmer(void *arg)
{
    int id = *(int *)arg;
    printf("South farmer %d wants to cross the bridge.\n", id);
    enter_bridge(1);
    printf("South farmer %d is crossing the bridge...\n", id);
    sleep(rand() % 3 + 1);
    printf("South farmer %d has crossed the bridge.\n", id);
    exit_bridge(1);
    return NULL;
}

int main()
{
    srand(time(NULL));
    pthread_t north[NUM_NORTH], south[NUM_SOUTH];
    int ids_north[NUM_NORTH], ids_south[NUM_SOUTH];

    pthread_mutex_init(&direction_mutex, NULL);
    sem_init(&bridge_sem, 0, 1);

    for (int i = 0; i < NUM_NORTH; i++)
    {
        ids_north[i] = i + 1;
        pthread_create(&north[i], NULL, north_farmer, &ids_north[i]);
    }

    for (int i = 0; i < NUM_SOUTH; i++)
    {
        ids_south[i] = i + 1;
        pthread_create(&south[i], NULL, south_farmer, &ids_south[i]);
    }

    for (int i = 0; i < NUM_NORTH; i++)
        pthread_join(north[i], NULL);

    for (int i = 0; i < NUM_SOUTH; i++)
        pthread_join(south[i], NULL);

    pthread_mutex_destroy(&direction_mutex);
    sem_destroy(&bridge_sem);

    return 0;
}
