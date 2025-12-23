#if !defined(__APP_TASK_H__)
#define __APP_TASK_H__

//消息队列 线程池
typedef void (*Task)(void *);

typedef enum TaskStatusTypeEnum {
    TASK_PENDING,   // 任务待处理
    TASK_RUNNING,   // 任务正在运行
    TASK_COMPLETED, // 任务已完成
    TASK_FAILED     // 任务执行失败
} TaskStatusType;

struct TaskStruct {
    Task task;
    void *argv;
};

int task_init(int executors);

int task_register(Task task, void *args);

void task_wait(void);

void task_close();

#endif //__APP_TASK_H__