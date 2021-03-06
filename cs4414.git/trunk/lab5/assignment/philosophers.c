// Jared Culp (jjc4fb) 
// Bryan Walsh (bpw7xx)
// CS 4414 (Lab5)

#define _BSD_SOURCE	// usleep is from BSD

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <sys/time.h>

#include <pthread.h>


typedef enum _state {
	STATE_THINKING = 'T',
	STATE_HUNGRY = 'H',
	STATE_EATING = 'E'
} state_t;

typedef struct _chopstick {
	bool down;
} chopstick_t;

typedef struct _philosopher {
	int id;
	state_t state;
	chopstick_t *left, *right;
	struct _philosopher *left_phil, *right_phil;
	double avg_wait;
	bool do_random;
	pthread_cond_t cv;
	pthread_mutex_t lock;
} philosopher_t;

static pthread_mutex_t stdout_lock;

#define LOCKED_PRINTF(fmt, args...) \
	pthread_mutex_lock(&stdout_lock); \
	printf(fmt, args); \
	fflush(stdout); \
	pthread_mutex_unlock(&stdout_lock)

static struct timeval time0;

// returns the approximate elapsed time since the first call (in seconds)
double elapsed_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (tv.tv_sec - time0.tv_sec + (tv.tv_usec - time0.tv_usec) / 1.e6 );
}

void update_state(philosopher_t *philosopher, state_t state) {
	philosopher->state = state;
	LOCKED_PRINTF("(t=%f) %d: %c\n", elapsed_time(), philosopher->id, philosopher->state);
}

void pickup_left(philosopher_t *philosopher) {
	philosopher->left->down = false;
	LOCKED_PRINTF("(t=%f) %d: LEFT UP\n", elapsed_time(), philosopher->id);
}

void pickup_right(philosopher_t *philosopher) {
	philosopher->right->down = false;
	LOCKED_PRINTF("(t=%f) %d: RIGHT UP\n", elapsed_time(), philosopher->id);
}

void putdown_left(philosopher_t *philosopher) {
	philosopher->left->down = true;
	LOCKED_PRINTF("(t=%f) %d: LEFT DOWN\n", elapsed_time(), philosopher->id);
}

void putdown_right(philosopher_t *philosopher) {
	philosopher->right->down = true;
	LOCKED_PRINTF("(t=%f) %d: RIGHT DOWN\n", elapsed_time(), philosopher->id);
}

void phil_sleep(philosopher_t *philosopher) {
	int wait_time_us = philosopher->avg_wait * 1e6;
	if (philosopher->do_random) {
		wait_time_us = rand() / (RAND_MAX + 1.0) * 2.0 * wait_time_us;
	}

	usleep(wait_time_us);
}

// main philosopher thread (naive solution)
void *philosopher_thread(void *arg) {
	philosopher_t *philosopher = (philosopher_t *) arg;
	while (true) {
		// I want to think ...
		phil_sleep(philosopher);

		// I'm hungry, I want to eat ...
		update_state(philosopher, STATE_HUNGRY);

		// gotta pick up the chopsticks before I can eat

		pthread_mutex_lock(&philosopher->lock);

		while (philosopher->left_phil->state == STATE_EATING
			&& philosopher->right_phil->state == STATE_EATING) {
			
			pthread_cond_wait(&philosopher->cv, &philosopher->lock);
		}

		// both chopsticks have to be available if we get here
		pickup_left(philosopher);
		pickup_right(philosopher);

		// FOOD!
		update_state(philosopher, STATE_EATING);
	
		// wait while I finish this rice
		phil_sleep(philosopher);

		// okay, put the chopsticks down
		putdown_left(philosopher);
		putdown_right(philosopher);

		update_state(philosopher, STATE_THINKING);				
		
		if (philosopher->left_phil->left_phil->state != STATE_EATING && philosopher->left_phil->state == STATE_HUNGRY)
			pthread_cond_signal(&philosopher->left_phil->cv);

		if (philosopher->right_phil->right_phil->state != STATE_EATING && philosopher->right_phil->state == STATE_HUNGRY)
			pthread_cond_signal(&philosopher->right_phil->cv);

		pthread_mutex_unlock(&philosopher->lock);

		// ready to think again
		phil_sleep(philosopher);
	}

	pthread_exit(NULL);
}

void usage(char *arg0) {
	fprintf(stderr, "Usage: %s num_philosophers avg_wait_seconds use_randomness\n"
					"Example: %s 5 0.5 1\n", arg0, arg0);
	exit(-1);
}

int main(int argc, char *argv[]) {
	gettimeofday(&time0, NULL);
	int n;
	double avg_wait;
	bool do_random;


	if (argc < 4) {
		usage(argv[0]);
	}

	n = atoi(argv[1]);
	avg_wait = atof(argv[2]);
	do_random = atoi(argv[3]);

	pthread_mutex_init(&stdout_lock, NULL);

	philosopher_t *philosophers = (philosopher_t *) malloc(sizeof(philosopher_t) * n);
	chopstick_t *chopsticks = (chopstick_t *) malloc(sizeof(chopstick_t) * n);

	for (int i = 0; i < n; i++) {
		chopsticks[i].down = true;
	}

	for (int i = 0; i < n; i++) {
		philosophers[i].id = i;
		philosophers[i].left = &chopsticks[i];
		philosophers[i].right = &chopsticks[(i + 1) % n];
		philosophers[i].left_phil = &philosophers[(i + n - 1) % n];
		philosophers[i].right_phil = &philosophers[(i + 1) % n];
		philosophers[i].avg_wait = avg_wait;
		philosophers[i].do_random = do_random;
		philosophers[i].state = STATE_THINKING;
		pthread_cond_init(&philosophers[i].cv, NULL);
		pthread_mutex_init(&philosophers[i].lock, NULL);
	}

	pthread_t *pids = (pthread_t *) malloc(sizeof(pthread_t) * n);
	for (int i = 0; i < n; i++) {
		pthread_create(&pids[i], NULL, philosopher_thread, &philosophers[i]);
	}

	for (int i = 0; i < n; i++) {
		pthread_join(pids[i], NULL);
	}

	free(pids);
	free(philosophers);
	free(chopsticks);

	pthread_mutex_destroy(&stdout_lock);
}

