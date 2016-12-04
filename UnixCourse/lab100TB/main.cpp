#include <iostream>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    mmap(NULL, (size_t)(atoll(argv[1])), PROT_READ , MAP_PRIVATE | MAP_ANON,-1 , 0);
    std::getchar();
    return 0;
}