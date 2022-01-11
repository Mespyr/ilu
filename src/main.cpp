#include "../include/ops.h"
#include "../include/util.h"
#include "../include/compiler.h"
#include "../include/lexer.h"
#include "../include/error.h"
#include "../include/parser.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>

void usage(char* exec_name)
{
    std::cout << "USAGE: " << exec_name << " <FILENAME> <OPTIONS>" << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "\t-h, --help\tshow help"                    << std::endl;
    std::cout << "\t-f, --fasm\tuse fasm to compile programs" << std::endl;
    std::cout << "\t-n, --nasm\tuse nasm to compile programs" << std::endl;
}

struct Options
{
    ASSEMBLER assembler;
    std::string filename;
    bool help;
};
Options parse_argv(std::vector<std::string> argv)
{
    Options options;
    options.assembler = NASM;
    options.filename = "";
    options.help = false;
    for (long unsigned int i = 1; i < argv.size(); i++)
    {
        std::string arg = argv.at(i);
        if (arg == "-f" || arg == "--fasm")
            options.assembler = FASM;
        else if (arg == "-n" || arg == "--nasm")
            options.assembler = NASM;
        else if (arg == "-h" || arg == "--help")
            options.help = true;
        else if (options.filename == "")
            options.filename = arg;
        else
        {
            print_error_with_no_location("more than one file provided for compilation.");
            exit(1);
        }
    }

    return options;
}

int main(int argc, char* argv[]) 
{
    // parse argv
    Options options = parse_argv(std::vector<std::string>(argv, argv + argc));

    if (options.help)
    {
        usage(argv[0]);
        exit(0);
    }

    // no filename passed into argv
    if (options.filename == "")
    {
        usage(argv[0]);
        print_error_with_no_location("no file program for compilation.");
        exit(1);
    }

    // parse file
    std::vector<Token> tokens = tokenize_file(options.filename);
    std::map<std::string, Function> program = parse_tokens(tokens);

    // compile
    std::cout << INFO_COLOR << "[info] generating asm from '" << options.filename << "'." << RESET_COLOR << std::endl;
    compile_to_asm(program, "out.asm", options.assembler);

    // nasm or fasm
    std::cout << INFO_COLOR << "[info] compiling assembly." << RESET_COLOR << std::endl;

    if (options.assembler == FASM)
    {
        exit_on_error(std::system("fasm out.asm"));
    }
    else
    {
        exit_on_error(std::system("nasm -felf64 out.asm"));

        // link
        std::cout << INFO_COLOR << "[info] linking object file." << RESET_COLOR << std::endl;
        exit_on_error(std::system("ld -o out out.o"));
    
        // cleanup
        std::cout << INFO_COLOR << "[info] cleaning up object file" << RESET_COLOR << std::endl;
        exit_on_error(std::system("rm out.o"));
    }

    return 0;
}
