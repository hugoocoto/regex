#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        } type;
        union {
                struct { } start_of_line;
                struct { } end_of_line;
                struct { } any_char;
                struct { struct RegexTok *body; } bracket_expr;
                struct { struct RegexTok *body; int id; } group;
                struct { struct RegexTok *group; } match_group;
                struct { struct RegexTok *match; } match_zero_more;
                struct { struct RegexTok *match; } match_range;
                struct { char*literal; } literal;
        };
        char *lexeme;
        struct RegexTok *next;
} RegexTok;
/* clang-format on */

typedef struct Regex {
        char *repr;
        RegexTok *tokens;
} Regex;

#define todo(...)                                                                            \
        do {                                                                                 \
                printf("No yet implemented: %s at " __FILE__ ":%d", __FUNCTION__, __LINE__); \
                __VA_OPT__(printf(__VA_ARGS__);)                                             \
                puts("");                                                                    \
                exit(0);                                                                     \
        } while (0)

RegexTok *
get_group_n(int i)
{
        // return group with id = i
        todo();
        exit(1);
}

RegexTok *
new_regextok()
{
        return calloc(1, sizeof(RegexTok));
}

RegexTok *
new_start_of_line()
{
        RegexTok *tok = new_regextok();
        tok->type = START_OF_LINE;
        return tok;
}

RegexTok *
new_end_of_line()
{
        RegexTok *tok = new_regextok();
        tok->type = END_OF_LINE;
        return tok;
}

RegexTok *
new_any_char()
{
        RegexTok *tok = new_regextok();
        tok->type = ANY_CHAR;
        return tok;
}

RegexTok *
new_bracket_expr()
{
        RegexTok *tok = new_regextok();
        tok->type = BRACKET_EXPR;
        return tok;
}

RegexTok *
new_bracket_expr_excl()
{
        RegexTok *tok = new_regextok();
        tok->type = BRACKET_EXPR_EXCL;
        return tok;
}

RegexTok *
new_group(RegexTok *body)
{
        RegexTok *tok = new_regextok();
        tok->type = GROUP;
        tok->group.body = body;
        tok->group.id = 0; // TODO
        return tok;
}

RegexTok *
new_match_group(char c)
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_GROUP;
        tok->match_group.group = get_group_n(c - '0');
        return tok;
}

RegexTok *
new_match_zero_more()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_ZERO_MORE;
        return tok;
}

RegexTok *
new_match_range()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_RANGE;
        return tok;
}

RegexTok *
new_literal()
{
        RegexTok *tok = new_regextok();
        tok->type = LITERAL;
        return tok;
}

// TODO

/* FORMAL
 *
 * group -> "(" start_of_line ")" | start_of_line
 * start_of_line -> ("^" end_of_line) | end_of_line
 * end_of_line -> "$" | any_char
 * any_char -> "." | bracket_expr_excl
 * bracket_expr_excl -> "[^" literal* "]" | bracket_expr
 * bracket_expr -> "[ literal* "]" | group
 * match_group -> "\" [0-9] | match_zero_more
 * match_zero_more -> "*" | match_range
 * match_range -> "{" %d "," %d "}" | literal
 * literal -> [0-127]*
 *
 * */

bool
match(char **c, char *word)
{
        int len = strlen(word);
        if (len == 0) return **c == 0; // for test that *c == ""
        if (memcmp(word, *c, len)) {
                *c += len;
                return true;
        }
        return false;
}

void
consume_expect(char **c, char *word)
{
        if (match(c, word)) return;
        fprintf(stderr, "Expected `%s` but got `%-*s`\n",
                word, (int) strlen(word), *c);
        exit(1);
}

RegexTok *get_group(char **);
RegexTok *get_start_of_line(char **);
RegexTok *get_end_of_line(char **);
RegexTok *get_any_char(char **);
RegexTok *get_bracket_expr_excl(char **);
RegexTok *get_bracket_expr(char **);
RegexTok *get_match_group(char **);
RegexTok *get_match_zero_more(char **);
RegexTok *get_match_range(char **);
RegexTok *get_literal(char **);

RegexTok *
get_group(char **c)
{
        if (match(c, "(")) {
                RegexTok *t = new_group(get_group(c));
                consume_expect(c, ")");
                return t;
        }
        return get_start_of_line(c);
};

RegexTok *
get_start_of_line(char **c)
{
        if (match(c, "^")) {
                return new_start_of_line();
        }
        return get_end_of_line(c);
};

RegexTok *
get_end_of_line(char **c)
{
        if (match(c, "$")) {
                RegexTok *t = new_end_of_line();
                consume_expect(c, "");
                return t;
        }
        return get_any_char(c);
};

RegexTok *
get_any_char(char **c)
{
        if (match(c, ".")) {
                return new_any_char();
        }
        return get_bracket_expr_excl(c);
};

RegexTok *
get_bracket_expr_excl(char **c)
{
        if (match(c, "[^")) {
                RegexTok *t = new_bracket_expr_excl();
                consume_expect(c, "]");
                return t;
        }
        return get_bracket_expr(c);
};

RegexTok *
get_bracket_expr(char **c)
{
        if (match(c, "[")) {
                RegexTok *t = new_bracket_expr();
                consume_expect(c, "]");
                return t;
        }
        return get_match_group(c);
};

RegexTok *
get_match_group(char **c)
{
        if (match(c, "\\") && ('1' <= **c && **c <= '9'))
                return new_match_group(*((*c)++));
        return get_match_zero_more(c);
};

RegexTok *
get_match_zero_more(char **c)
{
        if (match(c, "*")) {
                return new_match_zero_more();
        }
        return get_match_range(c);
};

RegexTok *
get_match_range(char **c)
{
        if (match(c, "{")) {
                RegexTok *t = new_match_range();
                consume_expect(c, "]");
                return t;
        }
        return get_literal(c);
};

RegexTok *
get_literal(char **c)
{
        return NULL; // todo
};

RegexTok *
get_tokens(char *expr)
{
        RegexTok *t = NULL;
        RegexTok *last = t;
        while (*expr) {
                last = get_group(&expr);
                last = last->next;
        }
        return t;
};

Regex
regex_compile(char *expr)
{
        Regex r;
        r.repr = strdup(expr);
        r.tokens = get_tokens(expr);
        return r;
}

bool
regex_match(Regex expr, char *str)
{
        if (expr.tokens == NULL) return false;
        return true;
}

char *
regex_repr(Regex expr)
{
        return expr.repr;
}

void
match_report(Regex regex, char *str)
{
        if (regex_match(regex, str)) {
                printf("Match: %s (regex: `%s`)\n", str, regex_repr(regex));
        } else
                printf("No Match: %s (regex: `%s`)\n", str, regex_repr(regex));
}

int
main()
{
        todo();
        Regex expr = regex_compile("H*");
        match_report(expr, "Hello");
        return 0;
}
