#include "pch.h"

int main()
{
    std::cout << "This is a test\n";
    GnuplotPipe gp;
    gp.sendLine("set output 'file.png'");
    gp.sendLine("set pointsize 20");
    gp.sendLine("plot \"./data.dat\" u 1:2:3 with points palette");
}