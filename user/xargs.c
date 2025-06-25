#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // 定义MAXARG

int main(int argc, char *argv[])
{
    char buf[512];      // 行缓冲区
    char *args[MAXARG]; // 参数数组
    int n, i, status;

    if (argc < 2)
    {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    // 初始化参数数组（保留第一个位置给命令名）
    for (i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }
    int base_args = argc - 1; // 基础参数数量
    while (1)
    {
        char *p = buf;
        // 逐字符读取直到换行符或EOF
        while ((n = read(0, p, 1)) > 0)
        {
            if (*p == '\n')
                break;
            p++;
        }

        // 检查读取状态
        if (n <= 0 && p == buf)
            break; // 无数据且缓冲区空则退出
        if (n < 0)
        {
            fprintf(2, "xargs: read error\n");
            exit(1);
        }

        // 终止当前行字符串
        *p = '\0';

        // 跳过空行
        if (p == buf)
            continue;

        // 添加当前行作为新参数
        args[base_args] = buf;
        args[base_args + 1] = 0; // 确保参数数组以NULL结尾

        // 创建子进程执行命令
        if (fork() == 0)
        {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        }

        // 父进程等待子进程完成
        wait(&status);
    }
    exit(0);
}