#define OEMRESOURCE  // 定义OEMRESOURCE常量

#include <windows.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
    return 0;
}

