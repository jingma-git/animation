#pragma once
#include <ctype.h>
#include <vector>

class Tokenizer
{
public:
    Tokenizer();
    ~Tokenizer();

    bool open(const char *file);
    bool close();

    bool abort(char *error);

    // Tokenization
    char get_char();
    char check_char();
    int get_int();
    float get_float();
    bool get_token(char *str);
    bool find_token(const char *tok);
    bool skip_white_space();
    bool skip_line();
    bool reset();

    // Access
    char *get_file_name() { return file_name; }
    int get_line_num() { return line_num; }

private:
    void *file;
    char file_name[256];
    int line_num;
};