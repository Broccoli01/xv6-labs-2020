#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 打印用法并退出
static void usage(void)
{
    // 直接输出到 stderr（fd=2）
    fprintf(2, "usage: sleep seconds\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        usage();

    // 严格验证输入：非负整数、无溢出
    char *p = argv[1];
    long n = 0;
    for (; *p; p++)
    {
        if (*p < '0' || *p > '9')
        {
            fprintf(2, "sleep: invalid number '%s'\n", argv[1]);
            exit(1);
        }
        // 检查溢出：n * 10 + digit 可能超过 int 范围
        if (n > (__INT64_MAX__ - (*p - '0')) / 10)
        {
            fprintf(2, "sleep: number too large\n");
            exit(1);
        }
        n = n * 10 + (*p - '0');
    }
    // 调用内核 sleep；若返回 <0，说明被 kill 了
    if (sleep((int)n) < 0)
    {
        fprintf(2, "sleep: interrupted\n");
        exit(1);
    }

    exit(0);
}