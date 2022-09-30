#include <iostream>
#include <string>
#include <vector>
#include "jhash.cpp"


static void print_help()
{
    printf("JHash is a sponge function for data fingerprinting.\n");
    printf("Usage:\n");
    printf("\t--help, -h\tPrint this message\n");
    printf("\t--length\tDigest length (default is 256 bit)\n");
    printf("\t--iter\t\tNumber of iterations (default is 1)\n");
    printf("\t--seed\t\tSeed for initial state. Also based on message. (Default is 0). Can also be a string.\n");
    printf("Note:\n");
    printf("\tIf seed is a string then the chaarcodes are summed and used instead.\n");
}

static bool isNumber(const std::string &str)
{
    for (char const &c : str)
    {
        if (std::isdigit(c) == 0)
            return false;
    }
    return true;
}


int main(int argc, char **argv)
{
    std::vector<std::string> args(argv + 1, argv + argc);
    if (args.size() > 0)
    {
        for (int i = 0; i < args.size(); i++)
        {
            if (args[i] == "-h" || args[i] == "--help")
            {
                print_help();
                return 0;
            }
        }
    }
    std::string input;
    char ch;
    while (std::cin.get(ch))
    {
        input += ch;
    }
    int default_length = 256;
    int default_iterations = 1;
    int default_seed = 0;
    bool compute_state_mode = false;
    int length_bytes = 0;
    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] == "--iter")
        {
            if (args.size() >= i)
            {
                if (isNumber(args[i + 1]))
                {
                    default_iterations = stoi(args[i + 1]);
                }
                else
                {
                    printf("Not a valid number of iterations.\n");
                }
            }
        }
        if (args[i] == "--seed")
        {
            if (args.size() >= i)
            {
                if (isNumber(args[i + 1]))
                {
                    default_seed = stoi(args[i + 1]);
                }
                else
                {
                    printf("Not a valid seed.\n");
                }
            }
        }
        if (args[i] == "--length")
        {
            if (args.size() >= i)
            {
                if (isNumber(args[i + 1]))
                {
                    default_length = stoi(args[i + 1]);
                }
                else
                {
                    printf("Not a valid length\n");
                }
            }
        }
        if (args[i] == "--compute-state")
        {
            compute_state_mode = true;
        }
    }
    length_bytes = (int)round(default_length / 8);
    if (compute_state_mode)
    {
        std::string initial_state = jhash::compute_state(input, length_bytes, default_seed);
        std::string hex_initial_state = jhash::chars_to_hex(initial_state);
        printf(hex_initial_state.c_str());
    }
    else
    {
        std::string hash = jhash::compute_hash(input, length_bytes, default_iterations, default_seed);
        std::string output_hash = jhash::format_hash(hash, default_iterations);
        printf(output_hash.c_str());
    }
    return 0;
}