#include <assert.h>
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

const char *TOKREPR[] = {
        [START_OF_LINE] = "START_OF_LINE",
        [END_OF_LINE] = "END_OF_LINE",
        [ANY_CHAR] = "ANY_CHAR",
        [BRACKET_EXPR] = "BRACKET_EXPR",
        [BRACKET_EXPR_EXCL] = "BRACKET_EXPR_EXCL",
        [GROUP] = "GROUP",
        [MATCH_GROUP] = "MATCH_GROUP",
        [MATCH_ZERO_MORE] = "MATCH_ZERO_MORE",
        [MATCH_RANGE] = "MATCH_RANGE",
        [LITERAL] = "LITERAL",
};

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
new_group()
{
        RegexTok *tok = new_regextok();
        tok->type = GROUP;
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
new_literal(char *lit)
{
        RegexTok *tok = new_regextok();
        tok->type = LITERAL;
        assert(tok->lexeme == NULL);
        /* for now it only accept a single char */
        tok->lexeme = strdup(" ");
        tok->lexeme[0] = *lit;
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
        if (memcmp(word, *c, len) == 0) {
                *c += len;
                return true;
        }
        return false;
}

void
expect_consume(char **c, char *word)
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
                RegexTok *t = new_group();
                RegexTok **last = &t->group.body;
                while (!match(c, ")")) {
                        *last = get_group(c);
                        last = &(*last)->next;
                }
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
                expect_consume(c, "");
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
                RegexTok **last = &t->bracket_expr.body;
                while (!match(c, "]")) {
                        if (**c == '\\') ++*c;
                        *last = get_literal(c);
                        last = &(*last)->next;
                }
                return t;
        }
        return get_bracket_expr(c);
};

RegexTok *
get_bracket_expr(char **c)
{
        if (match(c, "[")) {
                RegexTok *t = new_bracket_expr();
                RegexTok **last = &t->bracket_expr.body;
                while (!match(c, "]")) {
                        if (**c == '\\') ++*c;
                        *last = get_literal(c);
                        last = &(*last)->next;
                }
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
                RegexTok **last = &t->match_range.match;
                while (!match(c, "}")) {
                        *last = get_literal(c);
                        last = &(*last)->next;
                }
                return t;
        }
        return get_literal(c);
};

RegexTok *
get_literal(char **c)
{
        return new_literal((*c)++);
};

void
merge_literals(RegexTok *t)
{
        while (t && t->next) {
                switch (t->type) {
                case BRACKET_EXPR:
                case BRACKET_EXPR_EXCL:
                        merge_literals(t->bracket_expr.body);
                        break;
                case MATCH_RANGE:
                        merge_literals(t->match_range.match);
                        break;
                case MATCH_GROUP:
                        merge_literals(t->match_group.group);
                        break;
                case GROUP:
                        merge_literals(t->group.body);
                        break;
                case START_OF_LINE:
                case END_OF_LINE:
                case ANY_CHAR:
                case MATCH_ZERO_MORE:
                case LITERAL:
                        break;
                default:
                        todo("case for %s", TOKREPR[t->type]);
                }

                if (t->type != LITERAL || t->next->type != LITERAL) {
                        t = t->next;
                        continue;
                }

                t->lexeme = realloc(t->lexeme, strlen(t->lexeme) + strlen(t->next->lexeme) + 1);
                strcat(t->lexeme, t->next->lexeme);
                free(t->next->lexeme);
                t->next = t->next->next;
        }
}

RegexTok *
get_tokens(char *expr)
{
        RegexTok *t = new_regextok();
        RegexTok *last = t;
        char **cur = &expr;
        while (**cur) {
                last->next = get_group(cur);
                last = last->next;
        }
        last = t->next;
        merge_literals(last);
        free(t);
        return last;
};

#define INDENT 2
void
print_token_ast_branch(RegexTok *r, int indent)
{
        printf("%*s", indent, ""); // indentation
        switch (r->type) {
        case START_OF_LINE:
                printf("- Start of line `^`\n");
                break;
        case END_OF_LINE:
                printf("- End of line `$`\n");
                break;
        case ANY_CHAR:
                printf("- Any Char `.`\n");
                break;
        case BRACKET_EXPR:
                printf("- Bracket expression `[` ... `]`\n");
                for (RegexTok *t = r->bracket_expr.body; t; t = t->next)
                        print_token_ast_branch(t, indent + INDENT);
                break;
        case BRACKET_EXPR_EXCL:
                printf("- Bracket expression exclude `[^` ... `]`\n");
                for (RegexTok *t = r->bracket_expr.body; t; t = t->next)
                        print_token_ast_branch(t, indent + INDENT);
                break;
        case GROUP:
                printf("- Group `(` ... `)`\n");
                print_token_ast_branch(r->group.body, indent + INDENT);
                break;
        case MATCH_GROUP:
                printf("- Match Group `%d` <- TODO `\\%%d`\n", 0); // TODO
                break;
        case MATCH_ZERO_MORE:
                printf("- Match Zero or More `*`\n");
                break;
        case MATCH_RANGE:
                printf("- Match Range `{` ... `}`\n");
                print_token_ast_branch(r->match_range.match, indent + INDENT);
                break;
        case LITERAL:
                printf("- Literal `%s`\n", r->lexeme);
                break;
        default:
                todo("case for %s", TOKREPR[r->type]);
        }
}

void
print_token_ast(Regex r)
{
        RegexTok *t;
        for (t = r.tokens; t; t = t->next)
                print_token_ast_branch(t, 0);
}

Regex
regex_compile(char *expr)
{
        Regex r;
        r.repr = strdup(expr);
        r.tokens = get_tokens(expr);
        print_token_ast(r);
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
        Regex expr;
        expr = regex_compile("^[A-Za-z0-9._%+-]{1,}@[A-Za-z0-9.-]{1,}\\.[A-Za-z]{2,}$");
        // expr = regex_compile("^https?://[A-Za-z0-9.-]{1,}\\.[A-Za-z]{2,}(/[A-Za-z0-9._~!$&'()*+,;=:@%-]*)*\\/?$");
        // expr = regex_compile("^([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$");
        // expr = regex_compile("^[-+]?[0-9]+(\\.[0-9]+)?$");
        // expr = regex_compile("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
        // expr = regex_compile("^[A-Za-z0-9!@#$%^&*()_+\\-=\[\\]{};':\"\\\\|,.<>\\/?]{8,}$");
        match_report(expr, "hugocoto100305@gmail.com");
        return 0;
}
