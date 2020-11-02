#include "tokenizer.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

Tokenizer::Tokenizer()
{
}

Tokenizer::~Tokenizer()
{
}

bool Tokenizer::open(const char *fname)
{
    file = (void *)fopen(fname, "r");
    line_num = 1;
    if (file == 0)
    {
        printf("ERROR: Tokenzier::Open()- Can't open file '%s'\n", fname);
        return false;
    }
    strcpy(file_name, fname);
    return true;
}

bool Tokenizer::close()
{
    if (file)
    {
        fclose((FILE *)file);
    }
    else
    {
        return false;
    }
    file = 0;
    return true;
}

bool Tokenizer::abort(char *error)
{
    printf("ERROR '%s' line %d: %s\n", file_name, line_num, error);
    close();
    return false;
}

// Tokenization
char Tokenizer::get_char()
{
    char c = char(getc((FILE *)file));
    if (c == '\n')
    {
        line_num++;
    }
    return c;
}

char Tokenizer::check_char()
{
    int c = getc((FILE *)file);
    ungetc(c, (FILE *)file);
    return char(c);
}

// BUG: can't parse ".2", "f", or "F"
// Uses: [-]I[.[I]][(e|E)[+|-]I]
// Should use: [+|-](I|I.|.I|I.I)[(e|E)[+|-]I][f|F]
float Tokenizer::get_float()
{
    skip_white_space();
    int pos = 0;
    char temp[256];

    char c = check_char();
    if (c == '-')
    {
        temp[pos++] = get_char();
        c = check_char();
    }

    if (!isdigit(c))
    {
        printf("ERROR: Tokenizer::GetFloat()- Expecting float on line %d of '%s'\n", line_num, file_name);
        return 0;
    }
    temp[pos++] = get_char();
    // integer part
    while (isdigit(c = check_char()))
    {
        temp[pos++] = get_char();
    }
    // fraction part
    if (c == '.')
    {
        temp[pos++] = get_char();
        while (isdigit(c = check_char()))
        {
            temp[pos++] = get_char();
        }
    }
    // get exponent
    if (c == 'e' || c == 'E')
    {
        temp[pos++] = get_char();
        c = check_char();
        if (c == '+' || c == '-')
        {
            temp[pos++] = get_char();
            c = check_char();
        }
        if (!isdigit(c))
        {
            printf("ERROR: Tokenizer::GetFloat()- Poorly formatted float exponent on line %d of '%s'\n",
                   line_num, file_name);
            return 0.0f;
        }
        while (isdigit(c = check_char()))
        {
            temp[pos++] = get_char();
        }
    }
    temp[pos++] = '\0';
    return float(atof(temp));
}

int Tokenizer::get_int()
{
    skip_white_space();
    int pos = 0;
    char temp[256];

    char c = check_char();
    if (c == '-')
    {
        temp[pos++] = get_char();
        c = check_char();
    }

    if (!isdigit(c))
    {
        printf("ERROR: Tokenizer::GetInt()- Expecting int on line %d of '%s'\n", line_num, file_name);
        return 0;
    }
    temp[pos++] = get_char();
    while (isdigit(c = check_char()))
    {
        temp[pos++] = get_char();
    }
    temp[pos++] = '\0';
    return atoi(temp);
}

bool Tokenizer::get_token(char *str)
{
    skip_white_space();

    int pos = 0;
    char c = check_char();
    while (c != ' ' && c != '\n' && c != '\t' && c != '\r' && !feof((FILE *)file))
    {
        str[pos++] = get_char();
        c = check_char();
    }
    str[pos] = '\0';
    return true;
}

bool Tokenizer::find_token(const char *tok)
{
    int pos = 0;
    while (tok[pos] != '\0')
    {
        if (feof((FILE *)file))
        {
            return false;
        }
        char c = get_char();
        if (c == tok[pos])
        {
            pos++;
        }
        else
        {
            pos = 0;
        }
    }
    return true;
}

bool Tokenizer::skip_white_space()
{
    char c = check_char();
    bool white = false;

    while (isspace(c))
    {
        get_char();
        c = check_char();
        white = true;
    }
    return white;
}

bool Tokenizer::skip_line()
{
    char c = get_char();
    while (c != '\n')
    {
        if (feof((FILE *)file))
            return false;
        c = get_char();
    }
    return true;
}

bool Tokenizer::reset()
{
    if (fseek((FILE *)file, 0, SEEK_SET))
        return false;
    return true;
}
