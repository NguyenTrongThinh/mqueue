#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_NAME  "queuedemo"

void *sender();
void *receiver();

int main(int argc, char *argv[])
{
    pthread_t sender_id, receiver_id;

    pthread_create(&receiver_id, NULL, &receiver, NULL);
    pthread_create(&sender_id, NULL, &sender, NULL);

    pthread_join(sender_id, NULL);
    pthread_join(receiver_id, NULL);

    return 0;
}
void *receiver()
{
    mqd_t mq;
    struct mq_attr attr;
    char buffer[255];
    int length;
    char *exit = "exit";
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 255;
    attr.mq_curmsgs = 0;
    mq = mq_open("/liveshow", O_CREAT | O_RDONLY, 644, &attr);
    if (mq != ((mqd_t) -1))
    {
        while(strcmp(buffer, exit) != 0)
        {
              memset(buffer, 0, sizeof(buffer));
              length = mq_receive(mq, buffer, 255, NULL);
              if (length > 0)
              {
                  printf("Receive: %s\n", buffer);
                  fflush(stdout);
              }
        }
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
    }
    else
    {
        printf("Can't create queue");
        fflush(stdout);
    }
}
void *sender()
{
    mqd_t mq;
    char buffer[255];
    int count = 0;
    mq = mq_open("/liveshow", O_WRONLY);
    if (mq != ((mqd_t) -1))
    {
        while(1)
        {
            count++;
            memset(buffer, 0, sizeof(buffer));
            if (count >=10)
                strcpy(buffer, "exit");
            else
                sprintf(buffer, "Count: %d", count);
            mq_send(mq, buffer, strlen(buffer), 0);
            sleep(1);
        }
        mq_close(mq);
    }
    else
    {
        printf("Can't connect queue");
        fflush(stdout);
    }
}
