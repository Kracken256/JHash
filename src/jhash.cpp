#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <sstream>

namespace jhash
{
    static std::string chars_to_hex(std::string data)
    {
        char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        std::string output;
        for (int i = 0; i < data.length(); ++i)
        {
            unsigned char const byte = data[i];

            output += hex_chars[(byte & 0xF0) >> 4];
            output += hex_chars[(byte & 0x0F) >> 0];
        }
        return output;
    }

    static std::string createRandByteArray(int size, int64_t seed)
    {
        std::string pt;
        std::mt19937_64 gen(seed);
        for (int i = 0; i < size; i++)
        {
            pt = pt + (char)(((int)gen()) % 256);
        }
        return pt;
    }

    std::string compute_state(std::string data, int length, int seed = 0)
    {
        data += std::to_string(length);
        int64_t new_seed = seed;
        for (int i = 0; i < data.length(); i++)
        {
            new_seed += (char)data[i];
        }
        return createRandByteArray(length, new_seed);
    }

    static std::string Encode(std::string data)
    {
        static constexpr char sEncodingTable[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'};

        size_t in_len = data.size();
        size_t out_len = 4 * ((in_len + 2) / 3);
        std::string ret(out_len, '\0');
        size_t i;
        char *p = const_cast<char *>(ret.c_str());

        for (i = 0; i < in_len - 2; i += 3)
        {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
            *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
            *p++ = sEncodingTable[data[i + 2] & 0x3F];
        }
        if (i < in_len)
        {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            if (i == (in_len - 1))
            {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                *p++ = '=';
            }
            else
            {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
                *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
            }
            *p++ = '=';
        }

        return ret;
    }

    std::string format_hash(std::string hash, int iterations)
    {
        std::stringstream sstream;
        sstream << std::hex << iterations;
        std::string iter_in_hex = sstream.str();
        std::string output = "$2c$" + iter_in_hex + "$" + Encode(hash);
        return output;
    }

    std::string compute_hash(std::string data, int length, int iterations = 1, int seed = 0)
    {
        // Compute initial state
        std::string initial_state = compute_state(data, length, seed);
        std::string hex_initial_state = chars_to_hex(initial_state);
        // Pad data
        std::string padded_data;
        int size_of_padded_data = 0;
        if (data.length() < length)
        {
            int padding_size = length - data.length();
            std::string padding = "";
            for (int i = 0; i < padding_size; i++)
            {
                padding = padding + (char)0x00;
            }
            padded_data = data + padding;
            size_of_padded_data = padded_data.length();
        }
        else
        {
            int t1 = data.length() % length;
            if (t1 != 0)
            {
                int padding_size = length - t1;
                std::string padding;
                for (int i = 0; i < padding_size; i++)
                {
                    padding = padding + (char)0x00;
                }
                padded_data = data + padding;
                size_of_padded_data = padded_data.length();
            }
        }
        // Split Chunks
        int number_of_chunks = size_of_padded_data / length;
        std::vector<std::string> chunks;
        for (size_t i = 0; i < padded_data.size(); i += length)
        {
            std::string sub = padded_data.substr(i, length);
            chunks.push_back(sub);
        }
        // Compute sponge function
        for (int i = 0; i < iterations; i++)
        {
            for (int j = 0; j < number_of_chunks; j++)
            {
                for (int k = 0; k < length; k++)
                {
                    //initial_state[k] = initial_state[k] ^ initial_state[(k - 1) % length];
                    initial_state[k] = initial_state[k] ^ chunks[j][k];
                }
            }
        }
        return initial_state;
    }

}
