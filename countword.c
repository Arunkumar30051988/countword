#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#define MAX_BUFFER 1024
sem_t sem;
char Buffer[MAX_BUFFER]={0};

FILE *fp=NULL;

void *countword(void *arg)
{
	int count=0;
	while(1)
	{
		char ch;
		int error=0;
		sem_wait(&sem);
		printf("Thread 2 \n");
		if(fp == NULL)
		{
			printf("File pointer is NULL\n");
			sem_post(&sem);
			continue ;
		}
		error=fseek(fp,count,SEEK_SET);
		while((ch=fgetc(fp))!=EOF)
		{
			count++;
			printf("%c \n",ch);
		}
		printf("count : %d \n",count);
		sem_post(&sem);
		sleep(1);
	}
}
void *Getword(void *arg)
{
	int errnum=0;
	int errorcmp=0;
	int index=0;
	fp=fopen("/home/arunkumar/Program/text.txt","r+");
	if(NULL == fp)
	{
		errnum=errno;
		printf("Thread_2 started \n");
		printf("uanble to create file\n");
		fprintf(stderr,"error value : %d\n",errno);
		perror("error printed by perror");
		fprintf(stderr,"%s",strerror(errnum));
		return 0;
	}
	printf("Thread 1 : %u \n",fp);	
	while(1)
	{
		sem_wait(&sem);
		printf("Enter the string : \n");
		fgets(Buffer,MAX_BUFFER,stdin);
		fwrite(Buffer,strlen(Buffer),1,fp);
		printf("Fp :%u \n",fp);
		sem_post(&sem);
		sleep(1);
	}
}
int main()
{
	int error=0;
	pthread_t thread,thread_2;
	pthread_attr_t attr;
	struct sched_param sche_param;

	pthread_attr_init(&attr);
	pthread_attr_getschedparam(&attr,&sche_param);
	sche_param.sched_priority=10;
	

	pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
	pthread_attr_setschedparam(&attr,&sche_param);

	printf("%d\n",__LINE__);	
	void *thread_result=NULL;
	sem_init(&sem,0,1);
	error=pthread_create(&thread,&attr,Getword,NULL);
	pthread_attr_getschedparam(&attr,&sche_param);
	printf("Thread _1 : %d \n",sche_param.sched_priority);
	error=pthread_create(&thread_2,&attr,countword,NULL);
	pthread_join(thread,thread_result);
	pthread_join(thread_2,thread_result);	
}

