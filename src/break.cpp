#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "jhash.cpp"

using namespace std;

void write_to_table(std::vector<std::string> v)
{
    std::ofstream file;
    file.open("table.bin", ios::binary);
    for (int i = 0; i < v.size(); ++i)
    {
        file << v[i];
    }
    file.close();
}

static std::string file_contents(const char *path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
        return {};

    // Read contents
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    // Close the file
    file.close();

    return content;
}

void read_table(vector<string> &v, int length)
{
    string data = file_contents("table.bin");
    for (size_t i = 0; i < data.length(); i += length)
    {
        std::string sub = data.substr(i, length);
        v.push_back(sub);
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);
    int length = 64;

    std::vector<std::string> table;
    string test_input = "Secret{12d271e0de6a164}";
    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] == "--generate-table")
        {
            printf("Generating table...\n");
            for (int i = 0; i < 3000000; i++)
            {
                std::string state = jhash::createRandByteArray(length, i);
                table.push_back(state);
            }
            write_to_table(table);
            printf("Done.\n");
            return 0;
        }
        if (args[i] == "--input")
        {
            if (args.size() > i+1)
            {
                test_input = string(args[i + 1]);
            } else {
                printf("Please specify input.\n");
                return 0;
            }
        }
    }

    read_table(table, length);
    string hash = jhash::compute_hash(test_input, length, 1, 0);
    for (int i = 0; i < table.size(); i++)
    {
        string initial_state = table[i];
        for (int k = length - 1; k >= 0; k--)
        {
            initial_state[k] = initial_state[k] ^ hash[k];
        }
        if (initial_state.rfind(test_input, 0) == 0)
        {
            printf(jhash::chars_to_hex(table[i]).c_str());
            printf("\n");
            printf("Cracked!! Here is the password:\n");
            printf(initial_state.c_str());
            return 0;
        }
    }
    return 0;
}