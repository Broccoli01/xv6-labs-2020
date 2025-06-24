// pingpong.c — xv6 userland
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
die(const char *msg)
{
    // 统一输出到 stderr
    fprintf(2, "error: %s\n", msg);
    exit(1);
}

int main(int argc, char **argv)
{
    int p1[2], p2[2];
    // 1) 建管道并检查失败
    if (pipe(p1) < 0)
        die("pipe1 failed");
    if (pipe(p2) < 0)
        die("pipe2 failed");

    // 2) fork 并检查失败
    int pid = fork();
    if (pid < 0)
        die("fork failed");
    if (pid == 0)
    {
        // child —— 先从 p1 读，再向 p2 写
        close(p1[1]); // 关掉 p1 的写端
        close(p2[0]); // 关掉 p2 的读端

        char buf;
        // 3) 每次 read/write 都要检查返回值
        if (read(p1[0], &buf, 1) != 1)
            die("child read");
        printf("%d: received ping\n", getpid());

        if (write(p2[1], &buf, 1) != 1)
            die("child write");

        // 4) 用完的 fd 都关掉
        close(p1[0]);
        close(p2[1]);
        exit(0);
    }
    else
    {
        // parent —— 先向 p1 写，再从 p2 读
        close(p1[0]);
        close(p2[1]);

        char buf = 'X';
        if (write(p1[1], &buf, 1) != 1)
            die("parent write");

        if (read(p2[0], &buf, 1) != 1)
            die("parent read");
        printf("%d: received pong\n", getpid());

        close(p1[1]);
        close(p2[0]);

        // 等孩子退出（传 0 表示不想要子进程的 exit 状态）
        wait(0);
    }

    exit(0);
}