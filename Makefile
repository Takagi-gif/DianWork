# 这是一个测试Makefile
all:
    gcc test.c -o test   # 编译测试程序
    
clean:  
    rm -f test  # 删除可执行文件