#include "../include/parser.h"

std::vector<Op> link_ops(std::vector<Op> ops)
{
    std::vector<int> ip_stack;
    for (int ip = 0; ip < ops.size(); ip++)
    {
        Op current_op = ops.at(ip);

        if (current_op.type == OP_IF)
            ip_stack.push_back(ip);

        else if (current_op.type == OP_ELSE)
        {
            if (ip_stack.size() == 0)
            {
                print_token_error(current_op.token, "Unexpected 'else' keyword");
                exit(1);
            }

            int linker_ip = ip_stack.back();
            ip_stack.pop_back();

            Op linker_op = ops.at(linker_ip);
            if (linker_op.type != OP_IF)
            {
                print_token_error(current_op.token, "Unexpected 'else' keyword");
                exit(1);
            }

            linker_op.reference_ip = ip;

            ops.at(linker_ip) = linker_op;
            ip_stack.push_back(ip);
        }

        else if (current_op.type == OP_END)
        {
            if (ip_stack.size() == 0)
            {
                print_token_error(current_op.token, "Unexpected 'end' keyword");
                exit(1);
            }

            int linker_ip = ip_stack.back();
            ip_stack.pop_back();

            Op linker_op = ops.at(linker_ip);
            if (linker_op.type != OP_IF || linker_op.type != OP_ELSE)
            {
                print_token_error(current_op.token, "Unexpected 'end' keyword");
                exit(1);
            }

            linker_op.reference_ip = ip;
            ops.at(linker_ip) = linker_op;
        }
    }

    return ops;
}

std::vector<Op> parse_tokens(std::vector<Token> tokens)
{
    std::vector<Op> program;

    for (Token tok : tokens)
    {
        // arithmetic
        if (tok.value == "+") 
            program.push_back(Op(OP_PLUS, tok));

        else if (tok.value == "-") 
            program.push_back(Op(OP_MINUS, tok));

        else if (tok.value == "*")
            program.push_back(Op(OP_MUL, tok));

        else if (tok.value == "/")
            program.push_back(Op(OP_DIV, tok));

        // comparisons
        else if (tok.value == "=")
            program.push_back(Op(OP_EQUAL, tok));

        // keywords
        else if (tok.value == ".")
            program.push_back(Op(OP_PRINT, tok));

        else if (tok.value == ",")
            program.push_back(Op(OP_POP, tok));

        else if (tok.value == "if")
            program.push_back(Op(OP_IF, tok));

        else if (tok.value == "end")
            program.push_back(Op(OP_END, tok));

        else if (tok.value == "else")
            program.push_back(Op(OP_ELSE, tok));

        // push
        else if (is_number(tok.value)) 
            program.push_back(Op(OP_PUSH, atoi(tok.value.c_str()), tok));

        else
        {
            print_token_error(tok, "Unknown keyword '" + tok.value + "'");
            exit(1);
        }
    }

    return link_ops(program);
}
