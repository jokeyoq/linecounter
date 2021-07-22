#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "utils/strlist.h"
#define PERROR(info) {printf(info);exit(1);}
#define FORMATTIPS "Please enter -h to show help info.\nLike this:\nlinecounter -h\n"
#define LMODE int
#define LTRUE 1 /*有-l参数*/
#define LFALSE 0
void count_dir(struct strlist* args, LMODE l);/*递归计算目录中文件的行数*/
int count_file(char* fname, LMODE l);/*计算单个文件，如果LMODE 为 LTRUE 则计算同时打印单个文件行数*/
void print_format(void);/*打印命令格式要求*/
void countd_exts(char* dname, struct strlist* exts, LMODE l);/*递归计算目录中由exts后缀名过滤后的文件行数*/
char* get_ext(char* fname);/*获取后缀名*/
char* combine_path(char*, char*);/* dname & fname -> dname/fname */
int cnt_total;
int main(int argc, char* argv[])
{
    struct strlist* list, *p;
    struct stat st;
    int i;
    char* chk1, *chk2;/*检查参数*/
    LMODE lmode;
    char* fname;
    int ret;
    list = create_str_list();
    cnt_total = 0;
    fname = NULL;

    /*Fill in list with args*/
    for(i = 0; i < argc; i++)
    {
        insert_back(list, argv[i]);
    }
    if(argc == 1)
    {
        PERROR(FORMATTIPS);
    }
    else if(argc == 2)
    {
        /*这里有三种可能-h; filename; 或者错误的参数*/
        if(strcmp("-h", argv[1]) == 0)
        {
            print_format();
        }
        else
        {
            ret = stat(argv[1], &st);
            if(ret == -1)
            {
                printf("File [%s] does not exist.\n", argv[1]);
                exit(1);
            }
            else if(S_ISREG(st.st_mode))
            {
                cnt_total = count_file(argv[1], LFALSE);
                printf("Total lines:%d\n", cnt_total);
            }
            else
            {
                printf(" %s is not a regular file.You want to count a directory? \n -h to check correct format.\n", argv[1]);
                exit(1);
            }
        }
    }
    else if(argc == 3)
    {
        /*这里有 prog + fname + -l的可能或者错误的可能*/
        chk1 = get_str_in_list(list, "-l");
        chk2 = get_str_in_list(list, "-d");
        if(chk1 != NULL && chk2 == NULL)
        {
            /*找到fname*/
            p = list;
            while((p = get_next_item(p)) != NULL)
            {
                if(strcmp("linecounter", p->str) != 0 && strcmp("-l", p->str) != 0)
                {
                    fname = (char*)malloc(strlen(p->str+1));
                    strcpy(fname, p->str);
                }
            }
        }
        else
        {
            PERROR(FORMATTIPS);
        }
        ret = stat(fname, &st);
        if(ret == -1)
        {
            printf("file %s does not exist.\n", fname);
            exit(1);
        }
        else
        {
            cnt_total = count_file(fname, LTRUE);
            printf("Total lines:%d\n", cnt_total);
        }

    }
    else if(argc == 4)
    {
        /*错误 fname 和 dname都只允许一个*/
        PERROR(FORMATTIPS);
    }
    else
    {
        /*起码是 prog -d dname -f ext 五个参数的情况  -d 和 -f 必须同时出现*/
        /*也考虑-l和无-l*/
        chk1 = get_str_in_list(list, "-l");
        lmode =(chk1 == NULL ? LFALSE : LTRUE);
        if(lmode == LTRUE) delete_str(list, "-l");/*no longer needed*/;
        chk1 = get_str_in_list(list, "-d");
        chk2 = get_str_in_list(list, "-f");
        if(chk1 == NULL || chk2 == NULL) PERROR(FORMATTIPS);
        count_dir(list, lmode);/*此时已经保证了-d -f;-l是不确定的;同时也不确定 -d -f后面的参数的正确性*/
    }

    free(fname);
    clear_all(list);
    free(list);
    return 0;
}
char* get_ext(char* fname)
{
    char* m;
    m = strrchr(fname, '.');
    if(m == NULL) return " ";
    else return m+1;
}
void count_dir(struct strlist* args, LMODE l)
{
    struct strlist* p;
    struct strlist* exts;/*后缀列表*/
    char* dname;
    p = args;
    exts = create_str_list();
    printf("****************************************************\n");
    /*找到-f*/
    while((p = get_next_item(p)) != NULL)
    {
        if(strcmp("-f", p->str) == 0) break;
    }
    /*装填exts*/
    while((p = get_next_item(p)) != NULL)
    {
        if(strcmp(p->str, "-d") == 0 || strcmp(p->str, "-h") == 0) break;
        insert_back(exts, p->str);
    }

    /*找到-d*/
    p = args;
    while((p = get_next_item(p)) != NULL)
    {
        if(strcmp("-d", p->str) == 0) break;
    }
    dname = p->next->str;
    if(strcmp("-f", dname) == 0) PERROR(FORMATTIPS);
    countd_exts(dname, exts, l);
    if(strcmp(dname, ".") == 0) dname = "[current directory]";
    printf("\nTotal %d line(s)", cnt_total);
    printf(" with extensions: ");
    p = exts;
    while((p = get_next_item(p)) != NULL)
        printf("[%s] ", p->str);
    printf(" in directory %s \n", dname);
    printf("\n****************************************************\n");
}
int count_file(char* fname, LMODE l)
{
    int cnt, c;
    int flag_2n;/*是否连着两个\n*/
    FILE* fp;
    cnt = 0;
    fp = fopen(fname, "r");
    flag_2n = 0;
    while((c = fgetc(fp)) != EOF)
    {
        if(c == '\n')
        {
            flag_2n++;
            if(flag_2n == 2)
            {
                flag_2n = 1;
            }
            else
            {
                cnt++;
            }
        }
        else
        {
            flag_2n = 0;
        }
    }
    if(flag_2n == 1)
    {
        ;/*/n+EOF 不计入*/
    }
    else
    {
        cnt++;
    }
    if(l == LTRUE) printf("\nFile %s : %d lines \n", fname, cnt);
    fclose(fp);
    return cnt;
}
void countd_exts(char* dname, struct strlist* exts, LMODE l)
{
    DIR* dir;
    struct dirent* direntp;
    struct strlist* p;
    struct stat st;
    char* fullpath;
    dir = opendir(dname);
    if(dir == NULL)
    {
        printf("dir does not exist[%s]\n", dname);
    }
    else
    {
        while((direntp = readdir(dir)) != NULL)
        {
            fullpath = combine_path(dname, direntp->d_name);
            stat(fullpath, &st);
            if(strcmp(fullpath, combine_path(dname, ".")) == 0 || strcmp(fullpath, combine_path(dname, "..")) == 0) continue;
            if(S_ISDIR(st.st_mode))
            {
                countd_exts(fullpath, exts, l);
            }
            else if(S_ISREG(st.st_mode))
            {
                p = exts;
                while((p = get_next_item(p)) != NULL)
                {
                    if(strcmp("ALL", p->str) == 0)
                    {
                        cnt_total = cnt_total + count_file(fullpath, l);
                        break;
                    }
                    else if(strcmp(get_ext(fullpath), p->str) == 0)
                    {

                        cnt_total = cnt_total + count_file(fullpath, l);
                    }
                }

            }
        }
    }
    free(fullpath);
    closedir(dir);
}
char* combine_path(char* parent, char* cname)
{
    int i, j;
    int fplen = strlen(parent)+strlen(cname)+1;
    char* fullpath = (char*)malloc(fplen+1);
    for(i = 0; i < strlen(parent); i++)
    {
        fullpath[i] = parent[i];
    }
    fullpath[i++] = '/';
    for(j = 0; j < strlen(cname); j++)
    {
        fullpath[i++] = cname[j];
    }
    fullpath[i] = '\0';
    return fullpath;
}
void print_format()
{
    printf("************************************************************************\n");
    printf("Linecounter\n");
    printf("Author name: Agustin Kim\n");
    printf("Author email: agustinkim2020@hotmail.com\n");
    printf("Format:\n");
    printf("linecounter -d [dname] -f [extension1] [extension2] [extensions...] [-l]\n");
    printf("-d for count a directory\n");
    printf("-f only count files with specified extensions\n");
    printf("-l count for each file\n");
    printf("***********************************************************************\n");
    printf("linecounter filename [-l] count for single file\n");
    printf("linecounter -d . -f ALL: count current dir for any extension\n");
    printf("***********************************************************************\n");
    printf("Note: the '*' is not supported and is expected to cause errors.\n");
}
