#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAB_WIDTH 4

typedef enum {
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_ASSIGN,
    TOKEN_COLON,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_NEWLINE,
    TOKEN_DEF,
    TOKEN_PRINT,
    TOKEN_NAME,
    TOKEN_NUMBER,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char value[20];
} Token;

typedef struct {
    char *text;
    int pos;
    int current_indent;
} Lexer;

Token get_token(Lexer *lexer);
void get_indentation(Lexer *lexer);
Token get_next_token(Lexer *lexer);
void factor(Lexer *lexer);
void term(Lexer *lexer);
void expression(Lexer *lexer);
void comparison(Lexer *lexer);
void statement(Lexer *lexer);
void block(Lexer *lexer);
void program(Lexer *lexer);

Token get_token(Lexer *lexer) {
    Token token;
    while (isspace(lexer->text[lexer->pos])) {
        lexer->pos++;
    }

    if (lexer->text[lexer->pos] == '\0') {
        token.type = TOKEN_END;
    } else if (lexer->text[lexer->pos] == '(') {
        token.type = TOKEN_LPAREN;
        strcpy(token.value, "(");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == ')') {
        token.type = TOKEN_RPAREN;
        strcpy(token.value, ")");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '=') {
        token.type = TOKEN_ASSIGN;
        strcpy(token.value, "=");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == ':') {
        token.type = TOKEN_COLON;
        strcpy(token.value, ":");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '>') {
        token.type = TOKEN_GT;
        strcpy(token.value, ">");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '<') {
        token.type = TOKEN_LT;
        strcpy(token.value, "<");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '+') {
        token.type = TOKEN_PLUS;
        strcpy(token.value, "+");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '-') {
        token.type = TOKEN_MINUS;
        strcpy(token.value, "-");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '*') {
        token.type = TOKEN_MULTIPLY;
        strcpy(token.value, "*");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '/') {
        token.type = TOKEN_DIVIDE;
        strcpy(token.value, "/");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '\n') {
        token.type = TOKEN_NEWLINE;
        strcpy(token.value, "\n");
        lexer->pos++;
    } else if (lexer->text[lexer->pos] == '#') {
        while (lexer->text[lexer->pos] != '\n' && lexer->text[lexer->pos] != '\0') {
            lexer->pos++;
        }
        return get_token(lexer);
    } else if (isalpha(lexer->text[lexer->pos])) {
        int i = 0;
        while (isalnum(lexer->text[lexer->pos])) {
            token.value[i] = lexer->text[lexer->pos];
            i++;
            lexer->pos++;
        }
        token.value[i] = '\0';

        // Check if the identifier is a keyword or a name
        if (strcmp(token.value, "def") == 0) {
            token.type = TOKEN_DEF;
        } else if (strcmp(token.value, "print") == 0) {
            token.type = TOKEN_PRINT;
        } else {
            token.type = TOKEN_NAME;
        }
    } else if (isdigit(lexer->text[lexer->pos])) {
        int i = 0;
        while (isdigit(lexer->text[lexer->pos])) {
            token.value[i] = lexer->text[lexer->pos];
            i++;
            lexer->pos++;
        }
        token.value[i] = '\0';
        token.type = TOKEN_NUMBER;
    } else {
        printf("Invalid character: %c\n", lexer->text[lexer->pos]);
        exit(1);
    }

    return token;
}

void get_indentation(Lexer *lexer) {
    int current_indent = 0;
    while (isspace(lexer->text[lexer->pos])) {
        if (lexer->text[lexer->pos] == '\t') {
            current_indent += TAB_WIDTH;
        } else {
            current_indent++;
        }
        lexer->pos++;
    }
    lexer->current_indent = current_indent;  // Update current_indent
}

Token get_next_token(Lexer *lexer) {
    get_indentation(lexer);  // Update current_indent
    return get_token(lexer);
}

void factor(Lexer *lexer) {
    Token token = lexer->current_token;
    if (token.type == TOKEN_LPAREN) {
        lexer->current_token = get_next_token(lexer);
        expression(lexer);
        if (lexer->current_token.type != TOKEN_RPAREN) {
            printf("Mismatched parentheses\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
    } else if (token.type == TOKEN_NUMBER) {
        lexer->current_token = get_next_token(lexer);
    } else if (token.type == TOKEN_NAME) {
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type == TOKEN_GT || lexer->current_token.type == TOKEN_LT) {
            lexer->current_token = get_next_token(lexer);
            factor(lexer);
        }
    } else {
        printf("Unexpected token: %s\n", token.value);
        exit(1);
    }
}

void term(Lexer *lexer) {
    factor(lexer);
    while (lexer->current_token.type == TOKEN_MULTIPLY || lexer->current_token.type == TOKEN_DIVIDE) {
        lexer->current_token = get_next_token(lexer);
        factor(lexer);
    }
}

void expression(Lexer *lexer) {
    term(lexer);
    while (lexer->current_token.type == TOKEN_PLUS || lexer->current_token.type == TOKEN_MINUS) {
        lexer->current_token = get_next_token(lexer);
        term(lexer);
    }
}

void comparison(Lexer *lexer) {
    expression(lexer);
    while (lexer->current_token.type == TOKEN_GT || lexer->current_token.type == TOKEN_LT) {
        lexer->current_token = get_next_token(lexer);
        expression(lexer);
    }
}

void statement(Lexer *lexer) {
    Token token = lexer->current_token;
    if (token.type == TOKEN_DEF) {
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_NAME) {
            printf("Invalid function name\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_LPAREN) {
            printf("Missing opening parenthesis\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_NAME) {
            printf("Invalid parameter name\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_RPAREN) {
            printf("Missing closing parenthesis\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_COLON) {
            printf("Missing colon\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        block(lexer);
    } else if (token.type == TOKEN_PRINT) {
        lexer->current_token = get_next_token(lexer);
        expression(lexer);
    } else if (token.type == TOKEN_NAME) {
        lexer->current_token = get_next_token(lexer);
        if (lexer->current_token.type != TOKEN_ASSIGN) {
            printf("Invalid assignment\n");
            exit(1);
        }
        lexer->current_token = get_next_token(lexer);
        expression(lexer);
    } else {
        printf("Invalid statement\n");
        exit(1);
    }
}

void block(Lexer *lexer) {
    int indent_level = lexer->current_indent;
    lexer->current_token = get_next_token(lexer);
    while (lexer->current_indent > indent_level) {  // Use > instead of <
        statement(lexer);
    }
}

void program(Lexer *lexer) {
    lexer->current_token = get_next_token(lexer);
    while (lexer->current_token.type != TOKEN_END) {
        block(lexer);
    }
}

int main() {
    char text[] = "def factorial(n):\n    if n == 0:\n        return 1\n    else:\n        return n * factorial(n - 1)\n\nnum = 5\nresult = factorial(num)\nprint(result)";
    Lexer lexer;
    lexer.text = text;
    lexer.pos = 0;
    lexer.current_indent = 0;

    program(&lexer);

    return 0;
}
