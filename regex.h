#ifndef REGEX_H_
#define REGEX_H_

#include <stdbool.h>

/* clang-format off */
typedef struct RegexTok {
        enum {
                START_OF_LINE,
                END_OF_LINE,
                ANY_CHAR,
                BRACKET_EXPR,
                BRACKET_EXPR_EXCL,
                GROUP,
                MATCH_GROUP,
                MATCH_ZERO_MORE,
                MATCH_RANGE,
                LITERAL,
                MATCH_ZERO_ONE,
                MATCH_ONE_MORE,
                MATCH_OR,
        } type;
        union {
                struct {                                } start_of_line;
                struct {                                } end_of_line;
                struct {                                } any_char;
                struct { struct RegexTok *body;         } bracket_expr;
                struct { struct RegexTok *body; int id; } group;
                struct { struct RegexTok *group;        } match_group;
                struct { struct RegexTok *match;        } match_zero_more;
                struct { struct RegexTok *match;        } match_zero_one;
                struct { struct RegexTok *match;        } match_one_more;
                struct { struct RegexTok *match; struct RegexTok *range; } match_range;
                struct { struct RegexTok *left;  struct RegexTok *right; } match_or;
                struct { char*literal;                  } literal;
        };
        char *lexeme;
        struct RegexTok *next;
} RegexTok;

typedef struct Regex {
        char *repr;
        RegexTok *tokens;
} Regex;


/* Core functions */
Regex regex_compile(char *expr);
bool regex_match(Regex expr, char *str);

/* Info functions */
char * regex_repr(Regex expr);
void print_token_ast(Regex r);


#endif // !REGEX_H_
