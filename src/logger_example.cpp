#include "logging.h"


int main(int, char**){
    using namespace Common;

    char c = 'd';
    int i = 3;
    unsigned long ul = 65;
    float f = 3.4;
    double d = 3.56;
    const char *s = "Test C-String";
    std::string ss = "Test string";

    Logger logger("logging_example.log");
    logger.log("Logging char: %, int: %, unsigned long: %\n", c, i, ul);
    logger.log("Logging float: %, double: %\n", f, d);
    logger.log("Logging C-String: %, std::string: %\n", s, ss);


    return 0;
}