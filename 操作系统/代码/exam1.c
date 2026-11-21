/*
 * exam1.c — 基于缓冲技术的消息通信发送/接收命令的模拟实现
 * 编译：gcc exam1.c -o exam1 -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define N           3       /* 缓冲池个数 */
#define M           32      /* 缓冲区大小(bytes) */
#define SENDERS     3       /* 发送进程数 */
#define RECEIVERS   2       /* 接收进程数 */
#define TOTAL_MSG   8       /* 总消息数 */
#define DELAY_MS    80      /* 延迟(ms) */

typedef struct buf_node {
    char data[M];
    int  sender_id;
    struct buf_node *next;
} BufNode;

/* ---- 全局共享数据 ---- */
BufNode *pool = NULL, *pool_tail = NULL;
BufNode *mq   = NULL, *mq_tail   = NULL;
int pool_cnt = N, mq_cnt = 0, msg_seq = 0;

/* ---- 信号量 ---- */
sem_t mutex;    /* 互斥量, 初值1 */
sem_t empty;    /* 空闲缓冲数, 初值N */
sem_t full;     /* 消息数, 初值0 */

/* ---- 输出互斥锁 ---- */
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
int done_send = 0;  /* 发送完成标志 */

/* ---- 工具 ---- */
void delay_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void log_ln(const char *fmt, ...) {
    va_list ap;
    pthread_mutex_lock(&print_lock);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    pthread_mutex_unlock(&print_lock);
}

/* 在持有 mutex 时调用，打印临界区内资源快照 */
void show_snap(const char *who, const char *step) {
    char t[16]; BufNode *p; int i;
    time_t now = time(NULL);
    strftime(t, sizeof(t), "%H:%M:%S", localtime(&now));

    pthread_mutex_lock(&print_lock);
    printf("[%s] %-24s | 缓冲池[%d]:", t, who, pool_cnt);
    if (!pool) printf("空");
    else { p = pool; i = 0; while (p) { printf("b%d", i++); if (p->next) printf("→"); p = p->next; } }

    printf(" | 消息链[%d]:", mq_cnt);
    if (!mq) printf("空");
    else { p = mq; i = 0; while (p) { printf("M%d(P%d)", i++, p->sender_id); if (p->next) printf("→"); p = p->next; } }

    printf(" | %s\n", step);
    pthread_mutex_unlock(&print_lock);
}

/* P/V 宏 */
#define P(s) sem_wait(&(s))
#define V(s) sem_post(&(s))

/* ---- 链操作（调用者需持有 mutex） ---- */
BufNode *pool_get() {
    BufNode *b = pool; if (!b) return NULL;
    pool = b->next; if (!pool) pool_tail = NULL;
    b->next = NULL; pool_cnt--; return b;
}
void pool_put(BufNode *b) {
    b->next = NULL; b->sender_id = 0; memset(b->data, 0, M);
    if (!pool_tail) pool = pool_tail = b;
    else { pool_tail->next = b; pool_tail = b; }
    pool_cnt++;
}
void mq_put(BufNode *b) {
    b->next = NULL;
    if (!mq_tail) mq = mq_tail = b;
    else { mq_tail->next = b; mq_tail = b; }
    mq_cnt++;
}
BufNode *mq_get() {
    BufNode *b = mq; if (!b) return NULL;
    mq = b->next; if (!mq) mq_tail = NULL;
    b->next = NULL; mq_cnt--; return b;
}

/* ==================== 发送进程 p_send ====================
 * 执行流程：P(empty) → P(mutex) → pool_get → 填内容 → mq_put → V(mutex) → V(full)
 */
void *p_send(void *arg) {
    int id = *(int *)arg;
    char who[32];
    sprintf(who, "send 发送进程 p%d", id);

    while (1) {
        pthread_mutex_lock(&print_lock);
        if (msg_seq >= TOTAL_MSG) { pthread_mutex_unlock(&print_lock); break; }
        msg_seq++;
        pthread_mutex_unlock(&print_lock);

        /* Step 1: P(empty) — 申请空闲缓冲区，无空闲则阻塞 */
        log_ln("[%s] >>> P(empty) 等待空闲缓冲区...", who);
        P(empty);

        /* Step 2: P(mutex) — 进入临界区 */
        log_ln("[%s] >>> P(mutex) 申请进入临界区...", who);
        P(mutex);

        /* Step 3: 取缓冲区、填消息、入消息链 */
        BufNode *b = pool_get();
        b->sender_id = id;
        snprintf(b->data, M, "Msg#%d", msg_seq - 1);
        mq_put(b);

        show_snap(who, "V(mutex) 离开临界区 → V(full) 唤醒接收者");
        V(mutex);
        V(full);
        delay_ms(DELAY_MS);
    }

    log_ln("[send 发送进程 p%d 结束]", id);
    return NULL;
}

/* ==================== 接收进程 p_receive ====================
 * 执行流程：P(full) → P(mutex) → mq_get → 读内容 → pool_put → V(mutex) → V(empty)
 */
void *p_receive(void *arg) {
    int id = *(int *)arg;
    char who[32];
    sprintf(who, "recv 接收进程 r%d", id);

    while (1) {
        /* 检查退出条件 */
        pthread_mutex_lock(&print_lock);
        if (done_send && mq_cnt == 0) {
            pthread_mutex_unlock(&print_lock);
            break;
        }
        pthread_mutex_unlock(&print_lock);

        /* Step 1: P(full) — 等待消息，无消息则阻塞 */
        log_ln("[%s] >>> P(full) 等待消息...", who);

        /* 带超时的 P(full)：每 40ms 轮询，避免永久阻塞 */
        {
            int ok = 0;
            for (int w = 0; w < 25; w++) {  /* 最长等 1 秒 */
                if (sem_trywait(&full) == 0) { ok = 1; break; }
                delay_ms(40);
            }
            if (!ok) continue;  /* 超时，重新检查退出条件 */
        }

        /* Step 2: P(mutex) — 进入临界区 */
        log_ln("[%s] >>> P(mutex) 申请进入临界区...", who);
        P(mutex);

        /* Step 3: 取消息、归还缓冲区 */
        BufNode *b = mq_get();
        if (!b) { V(mutex); continue; }

        show_snap(who, "V(mutex) 离开临界区 → V(empty) 唤醒发送者");
        pool_put(b);
        V(mutex);
        V(empty);
        delay_ms(DELAY_MS);
    }

    log_ln("[recv 接收进程 r%d 结束]", id);
    return NULL;
}

/* ==================== 初始化 / 清理 ==================== */
void init() {
    int i; BufNode *prev = NULL;
    for (i = 0; i < N; i++) {
        BufNode *b = (BufNode *)malloc(sizeof(BufNode));
        memset(b->data, 0, M); b->sender_id = 0; b->next = NULL;
        if (!prev) pool = b; else prev->next = b;
        prev = b;
    }
    pool_tail = prev;
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, N);
    sem_init(&full,  0, 0);
}
void cleanup() {
    BufNode *p, *q;
    p = pool; while (p) { q = p; p = p->next; free(q); }
    p = mq;   while (p) { q = p; p = p->next; free(q); }
    sem_destroy(&mutex); sem_destroy(&empty); sem_destroy(&full);
    pthread_mutex_destroy(&print_lock);
}

/* ==================== 主函数 ==================== */
int main() {
    pthread_t st[SENDERS], rt[RECEIVERS];
    int si[SENDERS], ri[RECEIVERS], i;

    printf("\n");
    printf("============================================================\n");
    printf("  基于缓冲技术的消息通信模拟实现\n");
    printf("  缓冲池:%d个  发送进程(send):%d个  接收进程(recv):%d个  总消息:%d\n",
           N, SENDERS, RECEIVERS, TOTAL_MSG);
    printf("  send 流程: P(empty)→P(mutex)→取缓冲→填消息→入消息链→V(mutex)→V(full)\n");
    printf("  recv 流程: P(full)→P(mutex)→取消息→归还缓冲→V(mutex)→V(empty)\n");
    printf("============================================================\n\n");
    init();

    /* 初始状态 */
    P(mutex);
    show_snap("初始状态", "");
    V(mutex);
    delay_ms(300);

    /* 启动所有线程 */
    for (i = 0; i < RECEIVERS; i++) {
        ri[i] = i + 1;
        pthread_create(&rt[i], NULL, p_receive, &ri[i]);
    }
    delay_ms(50);
    for (i = 0; i < SENDERS; i++) {
        si[i] = i + 1;
        pthread_create(&st[i], NULL, p_send, &si[i]);
    }

    /* 等发送线程结束 */
    for (i = 0; i < SENDERS; i++) pthread_join(st[i], NULL);

    /* 标志发送完成，让接收者退出 */
    delay_ms(800);
    done_send = 1;
    for (i = 0; i < RECEIVERS; i++) sem_post(&full);
    for (i = 0; i < RECEIVERS; i++) pthread_join(rt[i], NULL);

    /* 最终状态 */
    P(mutex);
    show_snap("最终状态", "");
    V(mutex);
    printf("\n===== 程序结束 =====\n\n");

    cleanup();
    return 0;
}
