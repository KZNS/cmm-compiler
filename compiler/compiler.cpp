#include "automaton.h"
#include <string>
#include <iostream>
using namespace std;
Logger logger;
int main(int argc, char *argv[])
{
    logger.set_level("debug");
    logger.log("debug", "logger test.");
    return 0;
}