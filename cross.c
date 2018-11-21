#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>

#define maxCar 300
int arrive = 0;
pthread_mutex_t mutexNorth, mutexSouth, mutexWest, mutexEast, mutexDeadlock;
pthread_mutex_t mutexCondNorth, mutexCondSouth, mutexCondWest, mutexCondEast;
pthread_cond_t firstNorth, firstSouth, firstWest, firstEast, condDeadlock;

struct queue
{
    pthread_t thread[maxCar];
    int carsId[maxCar];
    int head, tail, totalNum;
};
void init(struct queue *q){
    q->head = q->tail = q->totalNum = 0;
}

void push(struct queue *q, int n){
    q->totalNum++;
    q->carsId[q->tail++] = n;
}
int pop(struct queue *q){
    q->totalNum--;
    return q->carsId[q->head++];
}
int head(struct queue *q){
    return q->carsId[q->head];
}

struct queue northCar, southCar, westCar, eastCar;

// void wake_up(){
//  printf("I'M WAKEUP!\n");
//  if(northCar.totalNum > 0) pthread_cond_signal(&firstNorth);
//  if(southCar.totalNum > 0) pthread_cond_signal(&firstSouth);
//  if(eastCar.totalNum > 0) pthread_cond_signal(&firstEast);
//  if(westCar.totalNum > 0) pthread_cond_signal(&firstWest);
// }

void solve_deadlock(){
    pthread_mutex_lock(&mutexDeadlock);
    printf("DEADLOCK:car jam detected,signalling North to go\n");
    pthread_cond_signal(&firstNorth);
    pthread_mutex_unlock(&mutexDeadlock);   
}

void *north_pass(void *arg){
//jia huchisuo fangzhi qita xiancheng genggai
    // printf("I'm here......!\n");
    pthread_mutex_lock(&mutexNorth);    
    // pthread_cond_wait(&firstNorth, &mutexNorth); 
    printf("car %d from North arrives at crossing\n",head(&northCar));
    arrive++;
    if(northCar.totalNum > 0 && southCar.totalNum  > 0 && westCar.totalNum > 0 && eastCar.totalNum > 0){
        if(4 == arrive){
            solve_deadlock();
        }
        else{
            pthread_mutex_lock(&mutexCondNorth);
            pthread_cond_wait(&firstNorth, &mutexCondNorth);
            pthread_mutex_unlock(&mutexCondNorth);
            printf("car %d from North leaving crossing\n",pop(&northCar));
            arrive--;
            pthread_cond_signal(&firstEast);
            pthread_mutex_unlock(&mutexNorth);
            return NULL;
        }
    }
    else if(westCar.totalNum > 0){
        pthread_cond_signal(&firstWest);

        pthread_mutex_lock(&mutexCondNorth);
        pthread_cond_wait(&firstNorth, &mutexCondNorth);
        pthread_mutex_unlock(&mutexCondNorth);
    }
    arrive--;
    printf("car %d from North leaving crossing\n",pop(&northCar));
//zhunbei houshi
    if(eastCar.totalNum > 0) pthread_cond_signal(&firstEast);
    else if(northCar.totalNum > 0) pthread_cond_signal(&firstNorth);
    pthread_mutex_unlock(&mutexNorth);
}

void *south_pass(void *arg){
//jia huchisuo fangzhi qita xiancheng genggai
    pthread_mutex_lock(&mutexSouth);
    // pthread_cond_wait(&firstSouth, &mutexSouth);
    // pthread_mutex_unlock(&mutexSouth);
    printf("car %d from South arrives at crossing\n",head(&southCar));
    arrive++;
    if(northCar.totalNum > 0 && southCar.totalNum  > 0 && westCar.totalNum > 0 && eastCar.totalNum > 0){
        if(4 == arrive){//yongyu panduan 4ge fangxiang de dengdai xiancheng shifoujiuxu 
            solve_deadlock();
        }

        pthread_mutex_lock(&mutexCondSouth);
        pthread_cond_wait(&firstSouth, &mutexCondSouth);
        pthread_mutex_unlock(&mutexCondSouth);
        printf("car %d from South leaving crossing\n",pop(&southCar));
        arrive--;
//wulun zenyang douyao dengyoubian de che zoule caineng jixu
        pthread_cond_signal(&firstWest);
        pthread_mutex_unlock(&mutexSouth);
        return NULL;
    }
    else if(eastCar.totalNum != 0){
        pthread_cond_signal(&firstEast);

        pthread_mutex_lock(&mutexCondSouth);
        pthread_cond_wait(&firstSouth, &mutexCondSouth);
        pthread_mutex_unlock(&mutexCondSouth);
    }
    arrive--;
    printf("car %d from South leaving crossing\n",pop(&southCar));
//zhunbei houshi
    if(westCar.totalNum > 0) pthread_cond_signal(&firstWest);
    else if(southCar.totalNum > 0) pthread_cond_signal(&firstSouth);
    pthread_mutex_unlock(&mutexSouth);
}

void *west_pass(void *arg){
//jia huchisuo fangzhi qita xiancheng genggai
    pthread_mutex_lock(&mutexWest);
    // pthread_cond_wait(&firstWest, &mutexWest);
    // pthread_mutex_unlock(&mutexSouth);
    printf("car %d from West arrives at crossing\n",head(&westCar));
    arrive++;
    if(northCar.totalNum > 0 && southCar.totalNum  > 0 && westCar.totalNum > 0 && eastCar.totalNum > 0){
        if(4 == arrive){
            solve_deadlock();
        }

        pthread_mutex_lock(&mutexCondWest);
        pthread_cond_wait(&firstWest, &mutexCondWest);
        pthread_mutex_unlock(&mutexCondWest);
        printf("car %d from West leaving crossing\n",pop(&westCar));
        arrive--;
//wulun zenyang douyao dengyoubian de che zoule caineng jixu
        pthread_cond_signal(&firstNorth);
        pthread_mutex_unlock(&mutexWest);
        return NULL;
    }
    else if(southCar.totalNum != 0){
        pthread_cond_signal(&firstSouth);

        pthread_mutex_lock(&mutexCondWest);
        pthread_cond_wait(&firstWest, &mutexCondWest);
        pthread_mutex_unlock(&mutexCondWest);
    }
    arrive--;
    printf("car %d from West leaving crossing\n",pop(&westCar));
//zhunbei houshi
    if(northCar.totalNum > 0) pthread_cond_signal(&firstNorth);
    else if(westCar.totalNum > 0) pthread_cond_signal(&firstWest);
    pthread_mutex_unlock(&mutexWest);
}

void *east_pass(void *arg){
//jia huchisuo fangzhi qita xiancheng genggai
    pthread_mutex_lock(&mutexEast);
    // pthread_cond_wait(&firstEast, &mutexEast);
    // pthread_mutex_unlock(&mutexSouth);
    printf("car %d from East arrives at crossing\n",head(&eastCar));
    arrive++;
    if(northCar.totalNum > 0 && southCar.totalNum  > 0 && westCar.totalNum > 0 && eastCar.totalNum > 0){
        if(4 == arrive){
            solve_deadlock();
        }

        pthread_mutex_lock(&mutexCondEast);
        pthread_cond_wait(&firstEast, &mutexCondEast);
        pthread_mutex_unlock(&mutexCondEast);
        printf("car %d from East leaving crossing\n",pop(&eastCar));
        arrive--;
//wulun zenyang douyao dengyoubian de che zoule caineng jixu
        pthread_cond_signal(&firstSouth);
        pthread_mutex_unlock(&mutexEast);
        return NULL;
    }
    else if(northCar.totalNum != 0){
        pthread_cond_signal(&firstNorth);
        
        pthread_mutex_lock(&mutexCondEast);
        pthread_cond_wait(&firstEast, &mutexCondEast);
        pthread_mutex_unlock(&mutexCondEast);
    }
    arrive--;
    printf("car %d from East leaving crossing\n",pop(&eastCar));    
//zhunbei houshi
    if(southCar.totalNum > 0) pthread_cond_signal(&firstSouth);
    else if(eastCar.totalNum > 0) pthread_cond_signal(&firstEast);
    pthread_mutex_unlock(&mutexEast);
}

int main(int argc,char** argv){
    pthread_mutex_init(&mutexNorth, NULL);
    pthread_mutex_init(&mutexSouth, NULL);
    pthread_mutex_init(&mutexWest, NULL);
    pthread_mutex_init(&mutexEast, NULL);
    pthread_mutex_init(&mutexCondNorth, NULL);
    pthread_mutex_init(&mutexCondSouth, NULL);
    pthread_mutex_init(&mutexCondEast, NULL);
    pthread_mutex_init(&mutexCondWest, NULL);
    pthread_mutex_init(&mutexDeadlock, NULL);

    pthread_cond_init(&firstNorth, NULL);
    pthread_cond_init(&firstSouth, NULL);
    pthread_cond_init(&firstWest, NULL);
    pthread_cond_init(&firstEast, NULL);
    // pthread_cond_init(&condDeadlock, NULL);
    init(&northCar);
    init(&southCar);
    init(&eastCar);
    init(&westCar);

    pthread_mutex_lock(&mutexNorth);
    pthread_mutex_lock(&mutexSouth);
    pthread_mutex_lock(&mutexWest);
    pthread_mutex_lock(&mutexEast);

    char* s = argv[1];
    int carNum = strlen(s);
    // for(int i = 0;i < carNum;i++){
    //  printf("%c",s[i]);
    // }
    for(int i = 0;i < carNum;i++){
        switch(s[i]){
            case 'n':{
                push(&northCar,i+1);//carId starts at 1
                pthread_create(&northCar.thread[northCar.tail-1], NULL, north_pass, NULL);
                break;
            }
            case 's':{
                push(&southCar,i+1);//carId starts at 1
                pthread_create(&southCar.thread[southCar.tail-1], NULL, south_pass, NULL);
                break;
            }
            case 'w':{
                push(&westCar,i+1);//carId starts at 1
                pthread_create(&westCar.thread[westCar.tail-1], NULL, west_pass, NULL);
                break;
            }
            case 'e':{
                push(&eastCar,i+1);//carId starts at 1
                pthread_create(&eastCar.thread[eastCar.tail-1], NULL, east_pass, NULL);
                break;
            }
        }
        //(i == carNum-1) || (northCar.totalNum == 1 && southCar.totalNum == 1 && westCar.totalNum == 1 && eastCar.totalNum == 1)
        if((i == carNum-1) || i == 3){
            // printf("%d\n",i);
            pthread_mutex_unlock(&mutexNorth);
            pthread_mutex_unlock(&mutexSouth);
            pthread_mutex_unlock(&mutexWest);
            pthread_mutex_unlock(&mutexEast);
        }
    }

    // pthread_t solveDeadlock;
    // pthread_create(&solveDeadlock, NULL, solve_deadlock, NULL);
    // pthread_create(&wakeUp, NULL, wake_up, NULL);
    // for(int i = 0;i< 1000000; i++);
    // usleep(4);
    // wake_up();
    // printf("I'm here in main!\n");
    for(int i = 0; i < northCar.totalNum;i++) pthread_join(northCar.thread[i], NULL);
    for(int i = 0; i < southCar.totalNum;i++) pthread_join(southCar.thread[i], NULL);
    for(int i = 0; i < westCar.totalNum;i++) pthread_join(westCar.thread[i], NULL);
    for(int i = 0; i < eastCar.totalNum;i++) pthread_join(eastCar.thread[i], NULL);
}