#include "interpreter.h"
#include <iostream>
using namespace std;
int main(void){
    PcodeInterpreter interp;
    interp.interpret("pcode.txt");
    return 0;
}