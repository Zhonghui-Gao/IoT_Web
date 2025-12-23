#include "task/task.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void task_impl(void *argv)
{
    int *p = argv;
    pthread_mutex_lock(&mutex);
    *p += 1;
    pthread_mutex_unlock(&mutex);
}

int main(void)
{
    int a = 6;
    task_init(5);
    for (size_t i = 0; i < 10; i++) {
        task_register(task_impl, &a);
    }

    sleep(5);
    assert(a == 16);

    task_close();
    task_wait();

    return 0;
}