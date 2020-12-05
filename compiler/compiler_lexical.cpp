#include "translator.h"
#include <string>
#include <iostream>
using namespace std;
Logger logger;
std::string in_file_name("testfile.txt");
std::string out_file_name("output.txt");
int main()
{
    logger.set_level("fatal");

    GrammarTranslator trans;

    trans.load_lexical("language.txt");

    trans.translate_lexical(in_file_name, out_file_name);

    return 0;
}
