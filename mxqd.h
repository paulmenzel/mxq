#ifndef __MXQ_SERVER_H__
#define __MXQ_SERVER_H__ 1


struct mxq_job_list {
    struct mxq_group_list *group;
    struct mxq_job_list   *next;

    struct mxq_job job;

    pid_t pid;
};

struct mxq_group_list {
    struct mxq_user_list  *user;
    struct mxq_group_list *next;

    struct mxq_group group;

    struct mxq_job_list *jobs;

    unsigned long job_cnt;

    long double   memory_per_thread;
    unsigned long slots_per_job;
    long double   memory_max_available;
    unsigned long memory_max;
    unsigned long slots_max;
    unsigned long jobs_max;

    unsigned long jobs_running;
    unsigned long threads_running;
    unsigned long slots_running;
    unsigned long memory_used;

    short orphaned;
};

struct mxq_user_list {
    struct mxq_server    *server;
    struct mxq_user_list *next;

    struct mxq_group_list *groups;
    unsigned long group_cnt;
    unsigned long job_cnt;

    unsigned long jobs_running;
    unsigned long threads_running;
    unsigned long slots_running;
    unsigned long memory_used;
};

struct mxq_server {
    struct mxq_user_list *users;

    unsigned long user_cnt;
    unsigned long group_cnt;
    unsigned long job_cnt;

    unsigned long jobs_running;
    unsigned long threads_running;
    unsigned long slots_running;
    unsigned long memory_used;

    unsigned long slots;
    unsigned long memory_total;
    long double   memory_avg_per_slot;
    unsigned long memory_max_per_slot;

    struct mxq_mysql mmysql;
    MYSQL *mysql;

    char *hostname;
    char *server_id;
    char *lockfilename;
    char *pidfilename;
    struct mx_flock *flock;

    int is_running;
};


#endif