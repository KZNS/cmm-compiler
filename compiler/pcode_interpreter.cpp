#include "interpreter.h"
#include <iostream>
Logger logger;
using namespace std;
int main(void){
    logger.set_level("error");
    PcodeInterpreter interp;
    interp.interpret("out.txt");
    return 0;
}