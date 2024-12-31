#include "tokens.hpp"
#include "output.hpp"
#include <iostream>
#include <stdlib.h>

#define MAX_STRING_SIZE 1024
#define START_OF_HEX 'x'
#define GERSHON '"'
#define SLASH '\\'
#define NULL_TER '\0'

#define ZERO '0'

#define LIN '\n'
#define ROW '\r'
#define TAB '\t'

#define R 'r'
#define N 'n'
#define T 't'

enum NUMBERS{ ONE = 1, TWO, THREE, FOUR ,TEN = 10, HEXABASE = 16 };

int switchToHex(int c1, int c2);
int numify(int c);
bool validHex(int c);
bool validNumber(char c);
bool validLetter(char c);
bool isValidValue(char c1, char c2);
void HandledString(char **string);

int main()
{
    enum tokentype token;
    char *string = new char[MAX_STRING_SIZE];
    // read tokens until the end of file is reached
    while ((token = static_cast<tokentype>(yylex())))
    {
        if (token != STRING)
        {
            output::printToken(yylineno, token, yytext);
        }
        // if the string is valid
        else if (yyleng > ONE)
        {
            HandledString(&string);
            output::printToken(yylineno, token, string);
        }
        std::fflush(stdout);
    }
    delete[] string;
    return 0;
}

bool validNumber(char c)
{
    return (c >= '0' && c <= '9');
}

bool validLetter(char c)
{
    return (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

bool isValidValue(char c1, char c2)
{
    return (validNumber(c1) || validLetter(c1)) &&
           (validNumber(c2) || validLetter(c2));
}

bool validHex(int c)
{
    return c == 9 || c == 10 || c == 13 || (c >= 20 && c <= 126);
}

int numify(int c)
{
    return validNumber(c) ? (c - '0') : (c > ('a'- ONE)) ?
                            (c - ('a'-'A')) - ('A' - TEN) : c - ('A' - TEN);
}

int switchToHex(int c1, int c2)
{
    return numify(c1) * HEXABASE + numify(c2);
}

void HandledString(char **string)
{
    int i = 0, j = 1;
    if (yyleng == ONE || yytext[yyleng - ONE] != GERSHON)
    {
        output::errorUnclosedString();
    }
    while (j <= yyleng)
    {
        if (yytext[j] == LIN || yytext[j] == ROW || yytext[j] == TAB)
        {
            delete[] *string;
            output::errorUnclosedString();
            break;
        }
        if (yytext[j] == SLASH)
        {
            if (yytext[j + ONE] == START_OF_HEX)
            {
                if ((yytext[j + TWO] != 0 && yytext[j + THREE] != 0) &&
                    (isValidValue((int)yytext[j + TWO], (int)yytext[j + THREE])))
                {
                    int trnsltdHex = switchToHex((int)yytext[j + TWO], (int)yytext[j + THREE]);
                    if (validHex(trnsltdHex))
                    {
                        // le validee!
                        (*string)[i] = (char)trnsltdHex;
                        i++;
                        j += FOUR;
                        continue;
                    }
                } // in case this bitch didnt work
                char invalidBackSlash[FOUR] = {START_OF_HEX, yytext[j + TWO],
                                                ((char)((int)yytext[j + TWO] == 0 ? 0 :
                                                ((int)yytext[j + THREE] == GERSHON ? 0 :
                                                (int)yytext[j + THREE]))), 0};
                delete[] *string;
                output::errorUndefinedEscape(invalidBackSlash);
                break;
            }
            switch (yytext[j + ONE])
            {
            case R:    (*string)[i] = ROW;       break;
            case N:    (*string)[i] = LIN;       break;
            case T:    (*string)[i] = TAB;       break;
            case SLASH:(*string)[i] = SLASH;     break; 
            case ZERO: (*string)[i] = NULL_TER;  break;
            case GERSHON:
                if (j != yyleng - TWO)
                {
                    (*string)[i] = GERSHON;
                }
                else
                {
                    delete[] *string;
                    output::errorUnclosedString();
                }
                break;

            default:
                const char character[TWO] = {yytext[j + ONE], 0};
                delete[] *string;
                output::errorUndefinedEscape(character);
                break;
            }
            i++;
            j += TWO;
        }
        else if (yytext[j] == GERSHON)
        {
            break;
        }
        else
        {
            (*string)[i] = yytext[j];
            i++;
            j++;
        }
    }

    (*string)[i] = NULL_TER;
}