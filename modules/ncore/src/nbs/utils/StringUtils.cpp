#include "StringUtils.hpp"

namespace nbs {

    void replaceAll(std::string& str, const char* from, const char* to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, strlen(from), to);
            start_pos += strlen(to);
        }
    }


    void fastConcat(const char* a, const char* b, char* d)
    {
        size_t len1 = strlen(a);
        size_t len2 = strlen(b);

        if (len1 + len2 >= strlen(d)) {
            return;
        }

        strcpy(d, a);
        strcpy(d + len1, a);
    }


    void fastConcat(const char* a, char* d)
    {
        size_t len1 = strlen(a);

        if (len1 >= strlen(d)) {
            return;
        }

        strcpy(d, a);
    }


    void fastInt2Str(int num, char* buffer)
    {
        int i = 0;
        int isNegative = 0;

        if (num < 0) {
            isNegative = 1;
            num = -num;
        }

        do {
            buffer[i++] = num % 10 + '0';
            num /= 10;
        } while (num > 0);

        if (isNegative) {
            buffer[i++] = '-';
        }

        int left = 0, right = i - 1;
        while (left < right) {
            char temp = buffer[left];
            buffer[left] = buffer[right];
            buffer[right] = temp;
            left++;
            right--;
        }

        buffer[i] = '\0';
    }

}