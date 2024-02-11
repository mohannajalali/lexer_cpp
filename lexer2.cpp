
#include <cstdio>
#include <i386/limits.h>
#include <iostream>

using namespace std;

void lex(FILE *fp, FILE *fout);

int main() {
  FILE *fp = fopen("source.txt", "r");
  FILE *fout = fopen("out.txt", "w");
  lex(fp, fout);
  fclose(fp);
  fclose(fout);
  return 0;
}

int nextState(int state) {
  if (1 <= state && state <= 2)
    return 3;

  if (3 <= state && state <= 21)
    return 22;

  if (22 <= state && state <= 26)
    return 27;

  if (27 <= state && state <= 30)
    return 31;

  return -1;
}

bool isAlpha(char ch) {
  return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

bool isNumber(char ch) { return ('0' <= ch && ch <= '9'); }

bool isHex(char ch) {
  return ('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F') ||
         ('a' <= ch && ch <= 'f');
}

bool isOctal(char ch) { return ('0' <= ch && ch <= '7'); }

bool isBinary(char ch) { return (ch == '0' || ch == '1'); }

bool isDelimiter(char ch) {
  return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || ch == ',' ||
          ch == ';');
}

bool isOperator(char ch) {
  return (ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '%' ||
          ch == '=' || ch == '<' || ch == '>' || ch == '!' || ch == '&' ||
          ch == '|' || ch == '^' || ch == '~' || ch == '?' || ch == ':' ||
          ch == '.' || ch == '@' || ch == '`' || ch == '(' || ch == ')' ||
          ch == '[' || ch == ']' || ch == '{' || ch == '}');
}

const string keywords[] = {
    "alignas",      "alignof",      "and",           "and_eq",
    "asm",          "auto",         "bitand",        "bitor",
    "bool",         "break",        "case",          "catch",
    "char",         "char8_t",      "char16_t",      "char32_t",
    "class",        "compl",        "concept",       "const",
    "consteval",    "constexpr",    "constinit",     "const_cast",
    "continue",     "co_await",     "co_return",     "co_yield",
    "decltype",     "default",      "delete",        "do",
    "double",       "dynamic_cast", "else",          "enum",
    "explicit",     "export",       "extern",        "false",
    "float",        "for",          "friend",        "goto",
    "if",           "inline",       "int",           "long",
    "mutable",      "namespace",    "new",           "noexcept",
    "not",          "not_eq",       "nullptr",       "operator",
    "or",           "or_eq",        "private",       "protected",
    "public",       "reflexpr",     "register",      "reinterpret_cast",
    "requires",     "return",       "short",         "signed",
    "sizeof",       "static",       "static_assert", "static_cast",
    "struct",       "switch",       "template",      "this",
    "thread_local", "throw",        "true",          "try",
    "typedef",      "typeid",       "typename",      "union",
    "unsigned",     "using",        "virtual",       "void",
    "volatile",     "wchar_t",      "while",         "xor",
    "xor_eq"};

bool isKeyword(string word) {
  for (int i = 0; i < 107; i++)
    if (keywords[i].compare(word) == 0)
      return true;

  return false;
}

void lex(FILE *fp, FILE *fout) {
  char ch;
  int state = 1;
  unsigned int loc;
  string value = "";

  fprintf(fout, "POS     | TYPE           | VALUE\n");
  fprintf(fout, "--------------------------------\n");

  while (true) {
    // cout << "\t\t" << ch << "(" << (int)ch << ")\t" << state << endl;

    switch (state) {
    // ID -- START --
    case 1: {
      loc = ftell(fp);
      ch = getc(fp);
      value += ch;

      if (isAlpha(ch) || ch == '_')
        state = 2;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // ID -- FINAL --
    case 2: {

      ch = getc(fp);
      value += ch;
      if (isAlpha(ch) || isNumber(ch) || ch == '_')
        state = 2;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        if (isKeyword(value)) {
          cout << "KEYWORD\t\t" << value << endl;
          fprintf(fout, "%d\t\t| %s | %s\n", loc, "KEYWORD\t\t", value.c_str());
        } else {
          cout << "IDENTIFIER\t" << value << endl;
          fprintf(fout, "%d\t\t| %s | %s\n", loc, "IDENTIFIER\t",
                  value.c_str());
        }
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }
    // INT -- START --
    case 3: {
      ch = getc(fp);
      value += ch;
      if (ch == '+' || ch == '-')
        state = 4;
      else if (ch == '0')
        state = 5;
      else if ('1' <= ch && ch <= '9')
        state = 6;
      else if (ch == '.')
        state = 7;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // INT
    case 4: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 6;
      else if (ch == '.')
        state = 7;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
    }

    // INT -- FINAL --
    case 5: {
      ch = getc(fp);
      value += ch;
      if (ch == '.')
        state = 7;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "INTEGER\t\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "INTEGER\t\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // INT -- FINAL --
    case 6: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 6;
      else if (ch == '.')
        state = 7;
      else if (ch == 'E' || ch == 'e')
        state = 9;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "INTEGER\t\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "INTEGER\t\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // FLOAT
    case 7: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 8;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // FLOAT -- FINAL --
    case 8: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 8;
      else if (ch == 'E' || ch == 'e')
        state = 9;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "FLOAT\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "FLOAT\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // FLOAT
    case 9: {
      ch = getc(fp);
      value += ch;
      if (ch == '+' || ch == '-')
        state = 10;
      else if (isNumber(ch))
        state = 11;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // FLOAT
    case 10: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 11;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }
    // FLOAT -- FINAL --
    case 11: {
      ch = getc(fp);
      value += ch;
      if (isNumber(ch))
        state = 11;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "FLOAT\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "FLOAT\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // HEX -- START --
    case 12: {
      ch = getc(fp);
      value += ch;
      if (ch == 'x' || ch == 'X')
        state = 13;
      else if (ch == 'c' || ch == 'C')
        state = 16;
      else if (ch == 'b' || ch == 'B')
        state = 19;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // HEX
    case 13: {
      ch = getc(fp);
      value += ch;
      if (isHex(ch))
        state = 14;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // HEX -- FINAL --
    case 14: {
      ch = getc(fp);
      value += ch;
      if (isHex(ch))
        state = 14;
      else if (ch == '_')
        state = 15;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "HEX\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "HEX\t\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // HEX
    case 15: {
      ch = getc(fp);
      value += ch;
      if (isHex(ch))
        state = 14;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // OCTAL -- START --
    case 16: {
      ch = getc(fp);
      value += ch;
      if (isOctal(ch))
        state = 17;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // OCTAL -- FINAL --
    case 17: {
      ch = getc(fp);
      value += ch;
      if (isOctal(ch))
        state = 17;
      else if (ch == '_')
        state = 18;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "OCTAL\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "OCTAL\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // OCTAL
    case 18: {
      ch = getc(fp);
      value += ch;
      if (isOctal(ch))
        state = 17;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // BINARY -- START --
    case 19: {
      ch = getc(fp);
      value += ch;
      if (isBinary(ch))
        state = 20;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }
    // BINARY -- FINAL --
    case 20: {
      ch = getc(fp);
      value += ch;
      if (isBinary(ch))
        state = 20;
      else if (ch == '_')
        state = 21;
      else if (isDelimiter(ch) || isOperator(ch)) {
        ungetc(ch, fp);
        value.pop_back();
        state = -1;
        cout << "BINARY\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "BINARY\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // BINARY
    case 21: {
      ch = getc(fp);
      value += ch;
      if (isBinary(ch))
        state = 20;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // CHARACTER -- START --
    case 22: {
      ch = getc(fp);
      value += ch;
      if (ch == '\'')
        state = 23;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // CHARACTER
    case 23: {
      ch = getc(fp);
      value += ch;

      if (ch == '\\')
        state = 24;
      else
        state = 26;

      break;
    }

    // ------------------
    case 24: {
      ch = getc(fp);
      value += ch;

      if (ch == '\'')
        state = 25;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }

      break;
    }

    // CHARACTER -- FINAL --
    case 25: {
      // ch = getc(fp);
      // value += ch;
      // cout << "CHAR\t\t\t\t\t" << ch << endl;
      // if (ch == '\'')
      // {
      state = -1;
      cout << "CHARACTER\t" << value << endl;
      fprintf(fout, "%d\t\t| %s | %s\n", loc, "CHARACTER\t", value.c_str());
      value = "";
      // }
      // else
      // {
      //     state = nextState(state);
      //     fseek(fp, loc, SEEK_SET);
      // }
      break;
    }

    // CHARACTER -- FINAL --
    case -2: {
      ch = getc(fp);
      value += ch;
      if (ch == '\'') {
        state = -1;
        cout << "CHARACTER\t" << value << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "CHARACTER\t", value.c_str());
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // CHARACTER
    case 26: {
      ch = getc(fp);
      value += ch;
      state = 24;
      break;
    }

    // STRING -- START --
    case 27: {
      ch = getc(fp);
      value += ch;

      if (ch == '\"')
        state = 28;
      else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }

    // STRING -- FINAL --
    case 28: {
      ch = getc(fp);
      value += ch;

      if (ch == '\"')
        state = 29;
      else if (ch == '\\')
        state = 30;
      else
        state = 28;

      break;
    }

    // STRING
    case 29: {
      ch = getc(fp);
      value += ch;
      cout << "STRING\t\t" << value << endl;
      fprintf(fout, "%d\t\t| %s | %s\n", loc, "STRING\t\t", value.c_str());
      value = "";
      state = -1;
      break;
    }

    case 30: {
      ch = getc(fp);
      value += ch;
      state = 28;
      break;
    }

    // OPERATOR
    case 31: {
      ch = getc(fp);
      value += ch;
      if (isOperator(ch)) {
        cout << "OPERATOR\t" << ch << endl;
        fprintf(fout, "%d\t\t| %s | %s\n", loc, "OPERATOR\t\t", value.c_str());
        state = -1;
        value = "";
      } else {
        state = nextState(state);
        fseek(fp, loc, SEEK_SET);
      }
      break;
    }
    // ESCAPE DELIMITERS
    case -1: {
      ch = getc(fp);
      if (isDelimiter(ch))
        state = -1;
      else if (ch == EOF)
        exit(0);
      else {
        ungetc(ch, fp);
        state = 1;
      }
      break;
    }
    default:
      break;
    }
  }
}