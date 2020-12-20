#include "translator.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
Logger logger;
std::string in_file_name("testfile.txt");
std::string out_file_name("output.txt");
string language = "\n\
define words\n\
CONSTTK const\n\
INTTK int\n\
CHARTK char\n\
VOIDTK void\n\
MAINTK main\n\
\n\
IFTK if\n\
ELSETK else\n\
DOTK do\n\
WHILETK while\n\
FORTK for\n\
SCANFTK scanf\n\
PRINTFTK printf\n\
RETURNTK return\n\
PLUS +\n\
MINU -\n\
MULT *\n\
DIV /\n\
LSS <\n\
LEQ <=\n\
GRE >\n\
GEQ >=\n\
EQL ==\n\
NEQ !=\n\
\n\
ASSIGN =\n\
SEMICN ;\n\
COMMA ,\n\
LPARENT (\n\
RPARENT )\n\
LBRACK [\n\
RBRACK ]\n\
LBRACE {\n\
RBRACE }\n\
\n\
IDENFR _a~zA~Z|_a~zA~Z0~9\n\
INTCON 0~9\n\
CHARCON '|+-*/_a~zA~Z0~9|'\n\
STRCON \"|\\032\\033\\035~\\126|\"\n\
";

int main()
{
    logger.set_level("fatal");

    GrammarTranslator trans;

    istringstream lang(language);
    
    trans.load_lexical(lang);

    trans.translate(in_file_name, out_file_name, "lexical");

    return 0;
}
