/*
 * exam2.c — 基于无缓冲技术的消息通信发送/接收命令的模拟实现
 * 编译：gcc exam2.c -o exam2 -lpthread
 *
 * 无缓冲 = 会合（rendezvous）：无中间缓冲队列，send 与 recv 必须握手。
 * 二信号量模型：
 *   s1(初值1) — send 控制信号量，同时起到互斥作用
 *   s2(初值0) — recv 控制信号量
 * send: P(s1) → 写槽 → V(s2)           （等槽空闲→写→唤醒recv）
 * recv:          P(s2) → 读槽 → V(s1)   （等数据→读→唤醒send）
 *
 * 每次传递：send 消费 s1、生产 s2 → recv 消费 s2、生产 s1
 * 同一时刻只有一个 sender 和一个 receiver 在会合
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

#define M           32
#define SENDERS     2
#define RECEIVERS   2
#define TOTAL_MSG   8
#define DELAY_MS    120

/* ---- 共享消息槽（无缓冲，仅一个槽位） ---- */
char shared_msg[M];
int  shared_sender = 0;
int  msg_sent = 0;        /* 已发送消息计数 */
int  msg_recv = 0;        /* 已接收消息计数 */

/* ---- 信号量 ---- */
sem_t s1;   /* 初值1，send 的 P(s1) 阻塞等槽空闲 */
sem_t s2;   /* 初值0，recv 的 P(s2) 阻塞等数据到达 */

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;  /* 保护计数器、打印 */
volatile int exiting = 0;

void delay_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void log_msg(const char *role, int id, const char *action) {
    char t[16];
    time_t now = time(NULL);
    strftime(t, sizeof(t), "%H:%M:%S", localtime(&now));
    pthread_mutex_lock(&mtx);
    printf("[%s] %s p%d %s\n", t, role, id, action);
    pthread_mutex_unlock(&mtx);
}

/* ==================== 发送进程 p_send ====================
 * P(s1) → 写槽 → V(s2)
 * s1 初值=1，既是同步信号量也是互斥锁：保证只有一个 send 在用槽
 */
void *p_send(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&mtx);
        if (msg_sent >= TOTAL_MSG) { pthread_mutex_unlock(&mtx); break; }
        int seq = msg_sent++;
        pthread_mutex_unlock(&mtx);

        log_msg("send", id, "P(s1) 等待共享槽空闲...");
        sem_wait(&s1);                              /* ① P(s1)：无空闲则阻塞 */

        snprintf(shared_msg, M, "Msg#%d", seq);
        shared_sender = id;
        log_msg("send", id, "写入消息 → V(s2) 唤醒接收者");
        sem_post(&s2);                              /* ② V(s2)：通知 recv */
        delay_ms(DELAY_MS);
    }
    log_msg("send", id, "结束");
    return NULL;
}

/* ==================== 接收进程 p_receive ====================
 * P(s2) → 读槽 → V(s1)
 * s2 初值=0，无消息时阻塞
 */
void *p_receive(void *arg) {
    int id = *(int *)arg;
    while (1) {
        /* 检查退出条件 */
        pthread_mutex_lock(&mtx);
        if (exiting && msg_recv >= msg_sent) {
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);

        log_msg("recv", id, "P(s2) 等待消息...");
        /* P(s2) 带超时轮询，以便能检查退出条件 */
        {
            int ok = 0;
            for (int w = 0; w < 15; w++) {
                if (sem_trywait(&s2) == 0) { ok = 1; break; }
                pthread_mutex_lock(&mtx);
                if (exiting && msg_recv >= msg_sent) {
                    pthread_mutex_unlock(&mtx);
                    goto quit;
                }
                pthread_mutex_unlock(&mtx);
                delay_ms(100);
            }
            if (!ok) continue;
        }

        /* 读取共享槽 */
        log_msg("recv", id, "收到消息 → V(s1) 唤醒发送者");
        pthread_mutex_lock(&mtx);
        msg_recv++;
        pthread_mutex_unlock(&mtx);
        sem_post(&s1);                              /* V(s1)：归还槽，唤醒 send */
        delay_ms(DELAY_MS);
    }
quit:
    log_msg("recv", id, "结束");
    return NULL;
}

/* ==================== 初始化 / 清理 ==================== */
void init() {
    sem_init(&s1, 0, 1);    /* 槽空闲，允许 send */
    sem_init(&s2, 0, 0);    /* 无数据，阻塞 recv */
}
void cleanup() {
    sem_destroy(&s1);
    sem_destroy(&s2);
    pthread_mutex_destroy(&mtx);
}

/* ==================== 主函数 ==================== */
int main() {
    pthread_t st[SENDERS], rt[RECEIVERS];
    int si[SENDERS], ri[RECEIVERS], i;

    printf("\n============================================================\n");
    printf("  基于无缓冲技术的消息通信模拟实现 (rendezvous)\n");
    printf("  模型: 1个共享槽(无缓冲)  %d个send  %d个recv  %d条消息\n",
           SENDERS, RECEIVERS, TOTAL_MSG);
    printf("  信号量: s1=1(send用)  s2=0(recv用)\n");
    printf("  send: P(s1)→写槽→V(s2)    recv: P(s2)→读槽→V(s1)\n");
    printf("============================================================\n\n");
    init();

    /* 先启 recv */
    for (i = 0; i < RECEIVERS; i++) {
        ri[i] = i + 1;
        pthread_create(&rt[i], NULL, p_receive, &ri[i]);
    }
    delay_ms(50);
    /* 再启 send */
    for (i = 0; i < SENDERS; i++) {
        si[i] = i + 1;
        pthread_create(&st[i], NULL, p_send, &si[i]);
    }

    /* 等 send 结束 */
    for (i = 0; i < SENDERS; i++) pthread_join(st[i], NULL);

    /* 通知 recv 退出 */
    delay_ms(500);
    exiting = 1;
    for (i = 0; i < RECEIVERS; i++) sem_post(&s2);   /* 唤醒可能阻塞的 recv */
    for (i = 0; i < RECEIVERS; i++) pthread_join(rt[i], NULL);

    printf("\n===== 程序结束 =====\n\n");
    cleanup();
    return 0;
}
