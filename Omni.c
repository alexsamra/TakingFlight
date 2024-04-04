#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t base;
sem_t foot;
sem_t soc;
sem_t field;
sem_t game;

int baseNum = 0;
int baseLength;
int footLength;
int avail = 1;
//int *baseArr = malloc(36 * sizeof(int));

int socNum = 0;
//int *socArr = malloc(44 * sizeof(int));

int footNum = 0;
//int *footArr = malloc(44 * sizeof(int));
int remPlay = 0;
int donePlay = 0;
int onField = 0;
int socArr[22];

struct timespec start, cur;

void *player(void* arg){
	int type = (int)arg;

	while(0 == 0) {
		sleep(rand()%60);
		if(avail == 1){
			checkTeams();
		}
		if(type == 0){
			printf("(TID %d): I am waiting to play baseball\n", (int) gettid());
			baseNum++;
			sem_wait(&base);
			int spot = 19 - remPlay;
			printf("(TID %d): I'm about to play baseball, I am taking spot %d\n", (int) gettid(), spot);
			remPlay--;
			if(remPlay > 0){
				sem_post(&base);
			}
			sleep(baseLength);
			sem_wait(&game);
			donePlay++;
			sem_post(&game);
			printf("(TID %d): Im done playing baseball, I'm leaving spot %d\n", (int) gettid(), spot);
			if(donePlay == 18){
				avail = 1;
				sem_post(&field);
				donePlay = 0;
			}
		}
		else if(type ==1){
			printf("(TID %d): I am waiting to play football\n", (int) gettid());
			footNum++;
                	sem_wait(&foot);
			int spot = 23 - remPlay;
			printf("(TID %d): I'm about to play football, I am taking spot %d\n", (int) gettid(), spot);
			remPlay--;
        		if(remPlay > 0){
                                sem_post(&foot);
                        }
                        sleep(footLength);
			sem_wait(&game);
                        donePlay++;
			sem_post(&game);
                        printf("(TID %d): Im done playing football, I'm leaving spot %d\n", (int) gettid(), spot);
                        if(donePlay == 22){
                                avail = 1;
				donePlay = 0;
                                sem_post(&field);
                        }

		}
	 	else if(type ==2){
			printf("(TID %d): I am waiting to play soccer\n", (int) gettid());
			sem_wait(&game);
			socNum++;
			sem_post(&game);
			clock_gettime(CLOCK_REALTIME, &cur);
//			printf("%ld\n", ((cur.tv_sec - start.tv_sec) + (cur.tv_nsec - start.tv_nsec)/1000000000));
			if(((cur.tv_sec - start.tv_sec) + (cur.tv_nsec - start.tv_nsec)/1000000000) > 5 || onField >= 22){
                		sem_wait(&soc);
			}
			else{
				sem_wait(&game);
				socNum--;
				sem_post(&game);
			}
			int spot = -1;
			sem_wait(&game);
			for(int i = 0; i < 22; i++){
				if(socArr[i] == 0){
					spot = 1 + i;
					socArr[i] = 1;
					break;
				}
			}
			sem_post(&game);
			printf("(TID %d): I'm about to play soccer, I am taking spot %d\n", (int) gettid(), spot);
			clock_gettime(CLOCK_REALTIME, &cur);
			if(socNum > 1 && ((cur.tv_sec - start.tv_sec) + (cur.tv_nsec - start.tv_nsec)/1000000000) <= 5.0 && onField < 22){
				sem_wait(&game);
				socNum = socNum -2;
				onField = onField + 2;
				sem_post(&game);
				sem_post(&soc);
				sem_post(&soc);
			}
			sleep(3 + rand()%4);
			sem_wait(&game);
			printf("(TID %d): I'm done playing soccer, leaving spot %d\n", (int) gettid(), spot);
			socArr[spot -1] = 0;
			onField--;
			sem_post(&game);
			if(onField == 0){
				avail = 1;
				sem_post(&field);
			}
        	}


	}
}

int checkTeams(){
	sem_wait(&field);
	int star = rand()%3;
	for(int i = 0; i < 3; i++){
		if(baseNum >= 18 && star == 0){
			avail = 0;
			remPlay = 18;
			baseNum = baseNum - 18;
			sem_post(&base);
			return 1;
		}
		else if(footNum >= 22 && star == 1){
			avail = 0;
			remPlay = 22;
			footNum = footNum -22;
			sem_post(&foot);
			return 1;
		}
		else if(socNum%2 == 0 && socNum >=2 && star == 2){
			avail = 0;
			clock_gettime(CLOCK_REALTIME, &start);
			socNum = socNum - 2;
			onField = 2;
			sem_post(&soc);
			sem_post(&soc);
			return 1;
		}
		star = star + 1;
		star = star %3;
	}
	printf("Not enough players \n");
	sem_post(&field);
	return 0;
}


int main (int argc, char *argv[]) {
        FILE *seed = fopen("seed.txt", "r");
        char *stringSeed = malloc(255 * sizeof(char));
        fscanf(seed, "%s", stringSeed);
	srand(atoi(stringSeed));

	sem_init(&base, 0, 0);
	sem_init(&foot, 0, 0);
	sem_init(&soc, 0, 0);
	sem_init(&field, 0, 1);
	sem_init(&game, 0, 1);

	baseLength = rand()%5 +4;
	footLength = rand()%5 + 4;
        pthread_t *threadArr = malloc(124 * sizeof(pthread_t));


        //generate baseball
        for(int i = 0; i < 36; i++){
                pthread_create((threadArr + i * sizeof(pthread_t)), NULL, player, (void *) 0);
        }

        //generate football
        for(int i = 0; i < 44; i++){
                pthread_create((threadArr + 36 + i * sizeof(pthread_t)), NULL, player, (void *) 1);
        }


        //generate soccer
        for(int i = 0; i < 44; i++){
                pthread_create((threadArr + 80 + i * sizeof(pthread_t)), NULL, player, (void *) 2);
        }

        //parent wait
        int status = 0;
        while(status != 1){
                wait(&status);
        }

}
