#include "interpreter.h"
#include <iostream>
Logger logger;
using namespace std;
int main(void){
    logger.set_level("debug");
    PcodeInterpreter interp;
    interp.interpret("pcode.txt");
    return 0;
}