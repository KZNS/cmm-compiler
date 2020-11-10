#include "translator.h"
#include <string>
#include <iostream>
using namespace std;
Logger logger;
std::string in_file_name;
std::string out_file_name("a.out");
int load_option(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    logger.set_level("fatal");

    if (load_option(argc, argv))
    {
        return -1;
    }

    //GrammarTranslator trans;

    //trans.load_lexical("language.txt");

    return 0;
}
int option(int argc, char *argv[], int &x)
{
    if (strlen(argv[x]) > 2)
    {
        logger.log("fatal", "unknow option \"%s\"", argv[x]);
        return -1;
    }
    switch (argv[x][1])
    {
    case 'l':
        if (x + 1 < argc && argv[x + 1][0] != '-')
        {
            if (logger.set_level(argv[x + 1]))
            {
                return -1;
            }
            x++;
        }
        else
        {
            logger.log("fatal", "missing log level.");
            return -1;
        }
        break;

    case 'o':
        if (x + 1 < argc && argv[x + 1][0] != '-')
        {
            out_file_name = argv[x + 1];
            x++;
        }
        else
        {
            logger.log("fatal", "missing output file name.");
            return -1;
        }
        break;

    default:
        logger.log("fatal", "unknow option \"%s\"", argv[x]);
        return -1;
    }
    return 0;
}
int load_option(int argc, char *argv[])
{
    cout << "load_option" << endl;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (option(argc, argv, i))
            {
            cout << "1" << endl;
                return -1;
            }
        }
        else
        {
            cout << "2" << endl;
            if (in_file_name == "")
            {
                in_file_name = argv[i];
            }
            else
            {
                logger.log("fatal", "unknow option \"%s\"", argv[i]);
                return -1;
            }
        }
    }
    if (in_file_name == "")
    {
        logger.log("fatal", "no input file.");
        return -1;
    }
    return 0;
}