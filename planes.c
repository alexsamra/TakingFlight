#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

pthread_mutex_t mutex_1;
pthread_mutex_t mutex_2;
pthread_mutex_t mutex_3;
pthread_mutex_t mutex_4;
pthread_mutex_t mutex_5;
pthread_mutex_t mutex_6;
pthread_mutex_t checkrun_mutex;

int reverse(int x){
	int i = 0;
	int val = 0;
	while(x != 0){
		val = val * 10 + x%10;
		x = x/10;
	}
	return val;
}

void *plane(void* arg){
	int sleep_time = rand()%30;
	int size = (int)arg;
	if(size == 0){
		printf("(TID %d): I'm a small plane idling for %d seconds\n", (int) gettid(), sleep_time);
	}
	else{
		printf("(TID %d): I'm a large plane idling for %d seconds\n", (int) gettid(), sleep_time);
	}
	sleep(sleep_time);

	if(size == 0){
		int run1;
		int run2;
		int runway = smallrun();
		int dir = rand()%2;
		if(dir == 0){
			run1 = runway%10;
			run2 = (runway/10)%10;
			runway = (run1*10) + (run2);
		}

		printf("(TID %d): I'm a small plane awaiting take off at terminal for runway order %d\n", (int) gettid(), reverse(runway));

		getlocks(0, runway);
		printf("(TID %d): Takeoff possible! I'm a small plane taking off in runway order %d\n", (int) gettid(), reverse(runway));
		takeoff(0, runway);

		int fly_time = rand()%30 + 1;
		printf("(TID %d): I'm a small plane currently flying for %d seconds.\n", (int) gettid(), fly_time);
		sleep(fly_time);

		runway = smallrun();
		dir = rand()%2;
		if(dir == 0){
			run1 = runway%10;
			run2 = (runway/10)%10;
			runway = (run1*10) + (run2);
		}

		printf("(TID %d): Done flying, I'm a small plane awaiting landing in runway order %d\n", (int) gettid(), reverse(runway));
		getlocks(0, runway);
		printf("(TID %d): Landing possible! I'm a small plane landing in runway order %d\n", (int) gettid(), reverse(runway));
		landing(0, runway);
		//int terminal = rand()%30 + 10;
		printf("(TID %d): I'm a small plane waiting in terminal\n", (int) gettid());
		//sleep(terminal);
	}


	if(size == 1){
		int run1;
		int run2;
		int run3;
		int runway = largerun();
		int dir = rand()%2;
		if(dir == 0){
			run1 = runway%10;
			run2 = (runway/10)%10;
			run3 = ((runway/10)/10)%10;
			runway = (run1*100)+(run2*10)+(run3);
		}

		printf("(TID %d): I'm a large plane awaiting take off at terminal for runway order %d\n", (int) gettid(), reverse(runway));
		getlocks(1, runway);
		printf("(TID %d): Takeoff possible! I'm a large plane taking off in runway order %d\n", (int) gettid(), reverse(runway));
		takeoff(1, runway);

		int fly_time = rand()%30 + 1;
		printf("(TID %d): I'm a large plane currently flying for %d seconds. \n", (int) gettid(), fly_time);
		sleep(fly_time);

		runway = largerun();
		dir = rand()%2;
		if(dir == 0){
			run1 = runway%10;
			run2 = (runway/10)%10;
			run3 = ((runway/10)/10)%10;
			runway = (run1*100)+(run2*10)+(run3);
		}
		printf("(TID %d): Done flying, I'm a large plane awaiting to land in runway order %d\n", (int) gettid(), reverse(runway));

		getlocks(1, runway);
		printf("(TID %d): Landing possible! I'm a small plane landing in runway order %d\n", (int) gettid(), reverse(runway));
		landing(1, runway);
		//int terminal = rand()%30 + 10;
		printf("(TID %d): I'm a large plane waiting in terminal\n", (int) gettid());
		//sleep(terminal);
	}
	return NULL;
}

int smallrun(){
	int select = rand()%6;
	if(select == 0){
		return 12;
	}
	else if(select == 1){
		return 14;
	}
	else if(select == 2){
		return 23;
	}
	else if(select == 3){
		return 34;
	}
	else if(select == 4){
		return 35;
	}
	else if(select == 5){
		return 46;
	}
}

int largerun(){
	int select = rand()%2;
	if(select == 0){
		return 146;
	}
	else if(select == 1){
		return 235;
	}
}

int checklock(int unlockNum){
//	printf("checking lock %d\n", unlockNum);
	if (unlockNum == 1){
		return pthread_mutex_trylock(&mutex_1);
	}
	else if(unlockNum == 2){
		return pthread_mutex_trylock(&mutex_2);
	}
	else if(unlockNum == 3){
		return pthread_mutex_trylock(&mutex_3);
	}
	else if(unlockNum == 4){
		return pthread_mutex_trylock(&mutex_4);
	}
	else if(unlockNum == 5){
		return pthread_mutex_trylock(&mutex_5);
	}
	else if(unlockNum == 6){
		return pthread_mutex_trylock(&mutex_6);
	}
}

void getlocks(int size, int runway){
	int nolocks = 1;
	if(size == 0){
		// IF THIS DOESN"T WORK ADD A SIGNAL
		while(nolocks == 1){
			pthread_mutex_lock(&checkrun_mutex);
			if(checklock(runway%10) != 0){
				pthread_mutex_unlock(&checkrun_mutex);
			}else{
				if(checklock((runway/10)%10) != 0){
					unlock_sig(runway%10);
					pthread_mutex_unlock(&checkrun_mutex);
				}else{
					nolocks = 0;
					pthread_mutex_unlock(&checkrun_mutex);
				}
			}
			if(nolocks == 1){
				sched_yield();
			}
		}
	}
	else if(size == 1){
		while(nolocks == 1){
			pthread_mutex_lock(&checkrun_mutex);
			if(checklock(runway%10)!= 0){
				pthread_mutex_unlock(&checkrun_mutex);
			}else{
				if(checklock((runway/10)%10) != 0){
					unlock_sig(runway%10);
					pthread_mutex_unlock(&checkrun_mutex);
				}else{
					if(checklock((runway/100)%10) != 0){
						unlock_sig(runway%10);
						unlock_sig((runway/10)%10);
						pthread_mutex_unlock(&checkrun_mutex);
					}else{
						nolocks = 0;
						pthread_mutex_unlock(&checkrun_mutex);
					}
				}
			}
			if(nolocks == 1){
				sched_yield();
			}
		}
	}
}

void unlock_sig(int lockNum){
	if (lockNum == 1){
		pthread_mutex_unlock(&mutex_1);
	}
	else if(lockNum == 2){
		pthread_mutex_unlock(&mutex_2);
	}
	else if(lockNum == 3){
		pthread_mutex_unlock(&mutex_3);
	}
	else if(lockNum == 4){
		pthread_mutex_unlock(&mutex_4);
	}
	else if(lockNum == 5){
		pthread_mutex_unlock(&mutex_5);
	}
	else if(lockNum == 6){
		pthread_mutex_unlock(&mutex_6);
	}
}

void takeoff(int size, int runway){
	int time1 = rand()%3 + 2;
        printf("(TID %d): Currently on runway %d for %d seconds. \n", (int) gettid(), runway%10, time1);
        sleep(time1);
        int time2 = rand()%3 + 2;
	int cur = runway/10;
	cur = cur%10;
        printf("(TID %d): Currently on runway %d for %d seconds, just left %d. \n", (int) gettid(), cur, time2, runway%10);
        unlock_sig(runway%10);
        sleep(time2);

        if(size == 1){
                int time3 = rand()%3 + 2;
                printf("(TID %d): Taking off on runway %d for %d seconds, just left %d. \n", (int) gettid(), runway/100, time3, (runway/10)%10);
                unlock_sig(cur);
                sleep(time3);
                printf("(TID %d): Takeoff! leaving %d\n", (int) gettid(), runway/100);
                unlock_sig(runway/100);
        }
        else{
                printf("(TID %d): Takeoff! leaving %d\n", (int) gettid(), cur);
                unlock_sig(cur);
        }

}

void landing(int size, int runway){
	int time1 = rand()%3 + 2;
	printf("(TID %d): Landing on runway %d for %d seconds. \n", (int) gettid(), runway%10, time1);
	sleep(time1);
	int time2 = rand()%3 + 2;
	printf("(TID %d): Landing on runway %d for %d seconds, just left %d. \n", (int) gettid(), (runway/10)%10, time2, runway%10);
	unlock_sig(runway%10);
	sleep(time2);

	if(size == 1){
		int time3 = rand()%3 + 2;
		printf("(TID %d): Currently on runway %d for %d seconds, just left %d. \n", (int) gettid(), (runway/100)%10, time3, (runway/10)%10);
		unlock_sig((runway/10)%10);
		sleep(time3);
		printf("(TID %d): Runway cleared! leaving %d\n", (int) gettid(), (runway/100)%10);
		unlock_sig(((runway/10)/10)%10);
	}
	else{
		printf("(TID %d): Runway cleared! leaving %d\n", (int) gettid(), (runway/10)%10);
		unlock_sig((runway/10)%10);
	}
}

int main (int argc, char *argv[]) {
	FILE *seed = fopen("seed.txt", "r");
	char *stringSeed = malloc(255 * sizeof(char));
	fscanf(seed, "%s", stringSeed);
	srand(atoi(stringSeed));

	pthread_mutex_init(&mutex_1, NULL);
	pthread_mutex_init(&mutex_2, NULL);
	pthread_mutex_init(&mutex_3, NULL);
	pthread_mutex_init(&mutex_4, NULL);
	pthread_mutex_init(&mutex_5, NULL);
	pthread_mutex_init(&mutex_6, NULL);
	pthread_mutex_init(&checkrun_mutex, NULL);
	pthread_t *threadArr = malloc(45 * sizeof(pthread_t));

	//generate small planes
	for(int i = 0; i < 30; i++){
		pthread_create(&threadArr[i], NULL, plane, (void *) 0);
	}

	//generate large planes
	for(int i = 30; i < 45; i++){
		pthread_create(&threadArr[i], NULL, plane, (void *) 1);
	}

	//parent wait
	for(int i = 0; i < 45; i++){
//		printf("%d\n", i);
		pthread_join(threadArr[i], NULL);
	}
	printf("All planes are done flying!\n");
}
