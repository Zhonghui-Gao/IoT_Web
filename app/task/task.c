#include "task.h"
#include <mqueue.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <log.h>

// 任务执行器线程函数
static pthread_t *executor_ptr;
static int executors_count = 0;
const char *queue_name = "/gataway_queue";
static mqd_t mq;

static void *task_executors(void *argv)
{
    int count = (int)(long)argv;
    log_info("Executor %d start!", count);
    struct TaskStruct task_struct;

    while (1) {
        if (mq_receive(mq, (char *)&task_struct, sizeof(struct TaskStruct), 0) < 0) {
            continue;
        }
        task_struct.task(task_struct.argv);
    }
    return NULL;
}

int task_init(int executor)
{
    int i = 0;
    struct mq_attr attr;
    //配置消息队列
    attr.mq_flags   = O_NONBLOCK;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof(struct TaskStruct);
    attr.mq_curmsgs = 0;

    mq = mq_open(queue_name, O_RDWR | O_CREAT, 0644, &attr);

    if (mq < 0) {
        log_error("mq_open error");
        goto EXIT;
    }

    //后台启动
    executors_count = executor;
    executor_ptr = malloc(executors_count * (sizeof(pthread_t)));
    if (!executor_ptr) {
        log_error("Not enough memory for task manager");
        goto MQ_EXIT;
    }
    memset(executor_ptr, 0, executors_count * (sizeof(pthread_t)));

    for (i = 0; i < executors_count; i++) {
        if (pthread_create(executor_ptr + i, NULL, task_executors, (void *) i) < 0) {
            goto FREE_EXIT;
        }
    }

    log_info("Task manager started~!");
    return 0;

FREE_EXIT:
    for (i = 0; i < executors_count; i++) {
        if (executor_ptr[i]) {
            pthread_cancel(executor_ptr[i]);
            pthread_join(executor_ptr[i], NULL);
        }
    }

MQ_EXIT:
    mq_unlink(queue_name);

EXIT:
    return -1;
}

int task_register(Task task, void *args)
{
    struct TaskStruct task_struct = {
        .task = task,
        .argv = args,
    };
    //任务结构体发送到消息队列中
    int ret = mq_send(mq, (char *)&task_struct, sizeof(struct TaskStruct), 0);
    log_debug("Task %p registered", task);
    return ret;
}

//等待任务完成
void task_wait(void)
{
    for (int i = 0; i < executors_count; i++) {
        if (executor_ptr[i]) {
            pthread_join(executor_ptr[i], NULL);
        }
    }

    free(executor_ptr);
    mq_unlink(queue_name);
    log_info("Task manager closed");
}

void task_close()
{
    log_info("Closing task manager...");
    for (int i = 0; i < executors_count; i++) {
        if (executor_ptr[i]) {
            pthread_cancel(executor_ptr[i]);
        }
    }
}