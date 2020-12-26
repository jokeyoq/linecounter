# 一个代码行数统计程序

- 示例

```sh
# kimy @ virtualbox in ~/Desktop/linecounter [16:17:32] 
$ ./linecounter -d . -f c -l  

****************************************************

File ./main.c : 285 lines 

File ./utils/strlist.c : 121 lines 

Total 406 line(s) with extensions: [c]  in directory [current directory] 

****************************************************


# kimy @ virtualbox in ~/Desktop/linecounter [16:17:42] 
$ ./linecounter main.c      
Total lines:285
```

- 安装

```sh
# kimy @ virtualbox in ~/Desktop/linecounter [16:21:54] 
$ sh install.sh 
[sudo] password for kimy: 

Install completed

************************************************************************
Linecounter
Author name: Agustin Kim
Author email: agustinkim2020@hotmail.com
Format:
linecounter -d [dname] -f [extension1] [extension2] [extensions...] [-l]
-d for count entrre directory
-f only count files with specified extensions
-l count for each file
***********************************************************************
linecounter filename [-l] count for single file
linecounter -d . -f ALL: count current dir for any extension
***********************************************************************
Note: the '*' is not supported and is expected to cause errors.
```

- 注意事项：不支持通配符'*'，-d 后面也只能跟一个目录，-f后接通配符可以用ALL替代，如

  ```sh
  $ linecounter -d dname -f ALL
  ```

- 后续功能(待实现):

  - 支持多目录 输入
  - 参数指定 统计 行 or 字数
  - 参数指定 忽略以指定字符串开头的行
  - 支持解析word
  - 移植windows

- 注意，暂且只能用于linux，测试环境为Ubuntu 20.04 LTS