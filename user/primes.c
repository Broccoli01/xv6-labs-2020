#include "kernel/types.h"
#include "user/user.h"

// 重定向管道到标准输入输出
void redirect(int fd, int pd[])
{
    close(fd);    // 关闭原标准输入/输出
    dup(pd[fd]);  // 复制管道描述符
    close(pd[0]); // 关闭不需要的管道端
    close(pd[1]);
}

// 核心筛选函数（递归）

void sieve()
{
    int prime;
    if (read(0, &prime, sizeof(int)) > 0)
    {
        printf("prime %d\n", prime);
        int pd[2];
        pipe(pd); // 创建新管道

        if (fork() == 0)
        {
            // 子进程：准备读取新管道数据
            redirect(0, pd);
            sieve();
        }
        else
        {
            redirect(1, pd);
            int num;
            while (read(0, &num, sizeof(int)) > 0)
            {
                if (num % prime != 0)
                {
                    write(1, &num, sizeof(int));
                }
            }
            close(1);  // 关闭写端触发EOF
            wait(0);    // 等待子进程
        }
    }
}
int main() {
    int pd[2];
    pipe(pd);  // 初始管道
    
    if (fork() == 0) {
        // 子进程：生成数字
        redirect(1, pd);
        for (int i = 2; i <= 35; i++) {
            write(1, &i, sizeof(int));
        }
        close(1);  // 关闭触发EOF
    } else {
        // 父进程：开始筛选
        redirect(0, pd);
        sieve();
        wait(0);  // 等待生成进程结束
    }
    exit(0);
}