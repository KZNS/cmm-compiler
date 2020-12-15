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

    GrammarTranslator trans;

    trans.load_lexical("language.txt");

    trans.translate(in_file_name, out_file_name);

    return 0;
}
int help()
{
    cout << "usage: ./compiler.exe <input> [-l <level>] [-o <output>]" << endl;
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
    case 'h':
        help();
        return 1;

    default:
        logger.log("fatal", "unknow option \"%s\"", argv[x]);
        return -1;
    }
    return 0;
}
int load_option(int argc, char *argv[])
{
    logger.debug("in load_option");
    int e;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            logger.debug("in load_option -");
            e = option(argc, argv, i);
            if (e)
            {
                return e;
            }
        }
        else
        {
            logger.debug("in load_option file_name");
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