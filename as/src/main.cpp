#include <iostream>
#include <cstring>

// Bison-generated interface
int yyparse();
extern int yydebug;

int main(int argc, char** arcv) {

    if (argc > 1 && strcmp(arcv[1], "-d") == 0)
        yydebug = 1;

    if (yyparse() == 0) {
        std::cout << "Done." << std::endl;
    } else {
        std::cout << "Parsing failed." << std::endl;
    }
    return 0;
}
