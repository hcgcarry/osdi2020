#include <stdio.h>
#include <stdlib.h>
#include "vfs.h"
#include "tmpfs.h"

#define assert(expr) if(!(expr)){printf("assert error!!!\n");} \

void testcase1()
{
    log("+++++ testcase1 +++++\n");
    struct file* a = vfs_open("hello", 0);
    assert(a == NULL);
    a = vfs_open("hello", O_CREAT);
    assert(a != NULL);
    vfs_close(a);
    struct file* b = vfs_open("hello", 0);
    assert(b != NULL);
    vfs_close(b);
}

void testcase2()
{
    log("+++++ testcase2 +++++\n");
    char buf[0x1000];
    struct file* a = vfs_open("hello", O_CREAT);
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("%s\n", buf); // should be Hello World!
}


int main()
{
    log("This is demo of VFS!\n");

    setup_tmpfs_filesystem();
    
    testcase1();
    testcase2();
     
    return 0;
}