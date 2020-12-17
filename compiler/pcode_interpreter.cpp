#include "interpreter.h"
#include <iostream>
Logger logger;
using namespace std;
int main(void){
    logger.set_level("debug");
    PcodeInterpreter interp;
    interp.interpret("p8.txt");
    return 0;
}