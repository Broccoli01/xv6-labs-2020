#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 比较文件名与目标名是否匹配
char *fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;
    // 定位最后一个'/'后的文件名
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0; // 确保字符串终止
    return buf;
}

void find(char *path, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE: // 普通文件
        if (strcmp(fmtname(path), target) == 0)
        {
            printf("%s\n", path);
        }
        break;

    case T_DIR: // 目录
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            fprintf(2, "find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/'; // 添加路径分隔符

        // 遍历目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue; // 跳过空目录项

            // 跳过"."和".."
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memmove(p, de.name, DIRSIZ); // 构造完整路径
            p[DIRSIZ] = 0;

            find(buf, target); // 递归查找子目录
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}