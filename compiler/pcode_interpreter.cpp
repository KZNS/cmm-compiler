#include "interpreter.h"
#include <iostream>
Logger logger;
using namespace std;
int main(void){
    logger.set_level("fatal");
    PcodeInterpreter interp;
    interp.interpret("p7.txt");
    return 0;
}