#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regex.h"

static const char *TOKREPR[] = {
        [START_OF_LINE] = "START_OF_LINE",
        [END_OF_LINE] = "END_OF_LINE",
        [ANY_CHAR] = "ANY_CHAR",
        [BRACKET_EXPR] = "BRACKET_EXPR",
        [BRACKET_EXPR_EXCL] = "BRACKET_EXPR_EXCL",
        [GROUP] = "GROUP",
        [MATCH_GROUP] = "MATCH_GROUP",
        [MATCH_ZERO_MORE] = "MATCH_ZERO_MORE",
        [MATCH_ZERO_ONE] = "MATCH_ZERO_ONE",
        [MATCH_ONE_MORE] = "MATCH_ONE_MORE",
        [MATCH_RANGE] = "MATCH_RANGE",
        [LITERAL] = "LITERAL",
        [MATCH_OR] = "MATCH_OR",
};

#define todo(...)                                                                            \
        do {                                                                                 \
                printf("No yet implemented: %s at " __FILE__ ":%d", __FUNCTION__, __LINE__); \
                __VA_OPT__(printf(": "__VA_ARGS__);)                                         \
                puts("");                                                                    \
                exit(0);                                                                     \
        } while (0)

static RegexTok *
get_group_n(int i)
{
        todo("match group %d", i);
        exit(1);
}

static inline RegexTok *
new_regextok()
{
        return calloc(1, sizeof(RegexTok));
}

static RegexTok *
new_start_of_line()
{
        RegexTok *tok = new_regextok();
        tok->type = START_OF_LINE;
        return tok;
}

static RegexTok *
new_end_of_line()
{
        RegexTok *tok = new_regextok();
        tok->type = END_OF_LINE;
        return tok;
}

static RegexTok *
new_any_char()
{
        RegexTok *tok = new_regextok();
        tok->type = ANY_CHAR;
        return tok;
}

static RegexTok *
new_bracket_expr()
{
        RegexTok *tok = new_regextok();
        tok->type = BRACKET_EXPR;
        return tok;
}

static RegexTok *
new_bracket_expr_excl()
{
        RegexTok *tok = new_regextok();
        tok->type = BRACKET_EXPR_EXCL;
        return tok;
}

static RegexTok *
new_group()
{
        RegexTok *tok = new_regextok();
        tok->type = GROUP;
        tok->group.id = 0; // TODO
        return tok;
}

static RegexTok *
new_match_group(char c)
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_GROUP;
        tok->match_group.group = get_group_n(c - '0');
        return tok;
}

static RegexTok *
new_match_zero_more()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_ZERO_MORE;
        return tok;
}

static RegexTok *
new_match_zero_one()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_ZERO_ONE;
        return tok;
}

static RegexTok *
new_match_one_more()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_ONE_MORE;
        return tok;
}

static RegexTok *
new_match_range()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_RANGE;
        return tok;
}

static RegexTok *
new_match_or()
{
        RegexTok *tok = new_regextok();
        tok->type = MATCH_OR;
        return tok;
}

static RegexTok *
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

/* FORMAL
 *
 * group -> "(" start_of_line ")" | start_of_line
 * start_of_line -> ("^" end_of_line) | end_of_line
 * end_of_line -> "$" | any_char
 * any_char -> "." | bracket_expr_excl
 * bracket_expr_excl -> "[^" literal* "]" | bracket_expr
 * bracket_expr -> "[ literal* "]" | group
 * match_group -> "\" [0-9] | match_zero_more
 * match_zero_more -> "*" | match_zero_one
 * match_zero_one -> "?" | match_one_more
 * match_one_more -> "+" | match_or
 * match_one_or -> "|" group | match_range
 * match_range -> "{" %d "," %d "}" | literal
 * literal -> [0-127]*
 *
 */

static bool
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

static void
expect_consume(char **c, char *word)
{
        if (match(c, word)) return;
        fprintf(stderr, "Expected `%s` but got `%-*s`\n",
                word, (int) strlen(word), *c);
        exit(1);
}

static RegexTok *get_group(char **);
static RegexTok *get_start_of_line(char **);
static RegexTok *get_end_of_line(char **);
static RegexTok *get_any_char(char **);
static RegexTok *get_bracket_expr_excl(char **);
static RegexTok *get_bracket_expr(char **);
static RegexTok *get_match_group(char **);
static RegexTok *get_match_zero_more(char **);
static RegexTok *get_match_zero_one(char **);
static RegexTok *get_match_one_more(char **);
static RegexTok *get_match_range(char **);
static RegexTok *get_match_or(char **);
static RegexTok *get_literal(char **);

static RegexTok *
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

static RegexTok *
get_start_of_line(char **c)
{
        if (match(c, "^")) {
                return new_start_of_line();
        }
        return get_end_of_line(c);
};

static RegexTok *
get_end_of_line(char **c)
{
        if (match(c, "$")) {
                RegexTok *t = new_end_of_line();
                expect_consume(c, "");
                return t;
        }
        return get_any_char(c);
};

static RegexTok *
get_any_char(char **c)
{
        if (match(c, ".")) {
                return new_any_char();
        }
        return get_bracket_expr_excl(c);
};

static RegexTok *
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

static RegexTok *
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

static RegexTok *
get_match_group(char **c)
{
        if (match(c, "\\") && ('1' <= **c && **c <= '9')) {
                return new_match_group(*((*c)++));
        }
        return get_match_zero_more(c);
};

static RegexTok *
get_match_zero_more(char **c)
{
        if (match(c, "*")) {
                return new_match_zero_more();
        }
        return get_match_zero_one(c);
}

static RegexTok *
get_match_zero_one(char **c)
{
        if (match(c, "?")) {
                return new_match_zero_one();
        }
        return get_match_one_more(c);
}

static RegexTok *
get_match_one_more(char **c)
{
        if (match(c, "+")) {
                return new_match_one_more();
        }
        return get_match_or(c);
}

static RegexTok *
get_match_or(char **c)
{
        if (match(c, "|")) {
                RegexTok *t = new_match_or();
                t->match_or.right = get_group(c);
                return t;
        }
        return get_match_range(c);
}

static RegexTok *
get_match_range(char **c)
{
        if (match(c, "{")) {
                RegexTok *t = new_match_range();
                RegexTok **last = &t->match_range.range;
                while (!match(c, "}")) {
                        *last = get_literal(c);
                        last = &(*last)->next;
                }
                return t;
        }
        return get_literal(c);
};

static RegexTok *
get_literal(char **c)
{
        return new_literal((*c)++);
};

static void
merge_literals(RegexTok *t)
{
        while (t) {
                switch (t->type) {
                case BRACKET_EXPR:
                case BRACKET_EXPR_EXCL:
                        merge_literals(t->bracket_expr.body);
                        break;
                case MATCH_RANGE:
                        merge_literals(t->match_range.match);
                        merge_literals(t->match_range.range);
                        break;
                case MATCH_GROUP:
                        merge_literals(t->match_group.group);
                        break;
                case GROUP:
                        merge_literals(t->group.body);
                        break;
                case MATCH_OR:
                        merge_literals(t->match_or.left);
                        merge_literals(t->match_or.right);
                        break;
                case MATCH_ZERO_MORE:
                        merge_literals(t->match_zero_more.match);
                        break;
                case MATCH_ONE_MORE:
                        merge_literals(t->match_one_more.match);
                        break;
                case MATCH_ZERO_ONE:
                        merge_literals(t->match_zero_one.match);
                        break;
                case START_OF_LINE:
                case END_OF_LINE:
                case ANY_CHAR:
                case LITERAL:
                        break;
                default:
                        todo("case for %s", TOKREPR[t->type]);
                }

                if (t->next == NULL) break;

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

static void
swap_operators(RegexTok **first)
{
        /* swap .* to *(.) */
        RegexTok **t = first;
        while (*t && (*t)->next) {
                if ((*t)->next->type == MATCH_ZERO_MORE) {
                        (*t)->next->match_zero_more.match = *t;
                        *t = (*t)->next;
                        (*t)->match_zero_more.match->next = NULL;

                } else if ((*t)->next->type == MATCH_ZERO_ONE) {
                        (*t)->next->match_zero_one.match = *t;
                        *t = (*t)->next;
                        (*t)->match_zero_one.match->next = NULL;

                } else if ((*t)->next->type == MATCH_ONE_MORE) {
                        (*t)->next->match_one_more.match = *t;
                        *t = (*t)->next;
                        (*t)->match_one_more.match->next = NULL;

                } else if ((*t)->next->type == MATCH_RANGE) {
                        (*t)->next->match_range.match = *t;
                        *t = (*t)->next;
                        (*t)->match_range.match->next = NULL;

                } else if ((*t)->next->type == MATCH_OR) {
                        (*t)->next->match_or.left = *t;
                        *t = (*t)->next;
                        (*t)->match_or.left->next = NULL;
                }

                t = &(*t)->next;
        }
}

static RegexTok *
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
        swap_operators(&last);
        merge_literals(last);
        free(t);
        return last;
};

#define INDENT 4
static void
print_token_ast_branch(RegexTok *r, int indent)
{
        if (r == NULL) return;
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
                print_token_ast_branch(r->match_zero_more.match, indent + INDENT);
                break;
        case MATCH_ZERO_ONE:
                printf("- Match Zero or One `?`\n");
                print_token_ast_branch(r->match_zero_one.match, indent + INDENT);
                break;
        case MATCH_ONE_MORE:
                printf("- Match One or More `+`\n");
                print_token_ast_branch(r->match_one_more.match, indent + INDENT);
                break;
        case MATCH_RANGE:
                printf("- Match Range `{` a , b `}`\n");
                print_token_ast_branch(r->match_range.match, indent + INDENT);
                print_token_ast_branch(r->match_range.range, indent + INDENT);
                break;
        case MATCH_OR:
                printf("- Match Or `|`\n");
                print_token_ast_branch(r->match_or.left, indent + INDENT);
                print_token_ast_branch(r->match_or.right, indent + INDENT);
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
        printf("Expr: `%s`\n", regex_repr(r));
        for (t = r.tokens; t; t = t->next)
                print_token_ast_branch(t, 0);
}

Regex
regex_compile(char *expr)
{
        Regex r;
        r.repr = strdup(expr);
        r.tokens = get_tokens(expr);
        return r;
}

static bool
char_in_str(char chr, char *str)
{
        if (strchr(str, chr) == NULL) {
                // check ranges
                char *c = str;
                while ((c = strchr(c, '-')) && c != str && c[1]) {
                        if (c[-1] <= chr && chr <= c[1]) return true;
                        ++c;
                }
                return false;
        }
        return true;
}

static bool eval(RegexTok *t, char *str, int offset, int *result);

static bool
match_range(RegexTok *teval, RegexTok *tnext, char *str, int offset, int *result, int min, int max)
{
        int n;
        int ret;
        while (1) {
                ret = 0;
                for (int i = 0; i < max; i++) {
                        if (!eval(teval, str, offset + ret, &n)) {
                                max = i;
                                break;
                        }
                        ret += n;
                }
                if (max < min) return false;
                if (eval(tnext, str, offset + ret, NULL)) {
                        if (result) *result = ret;
                        return true;
                }
                max--;
        }
}

static bool
eval(RegexTok *t, char *str, int offset, int *result)
{
        if (result) *result = 0;
        if (t == NULL || offset > (int) strlen(str)) return t == NULL;

        switch (t->type) {
        case START_OF_LINE:
                if (offset == 0) {
                        return eval(t->next, str, 0, NULL);
                } else {
                        return 0;
                }

        case END_OF_LINE:
                return !str[offset];

        case ANY_CHAR:
                if (eval(t->next, str, offset + 1, NULL)) {
                        if (result) *result = 1;
                        return true;
                } else {
                        return false;
                }

        case LITERAL:
                if (memcmp(str + offset, t->lexeme, strlen(t->lexeme)) == 0) {
                        if (eval(t->next, str, offset + strlen(t->lexeme), NULL)) {
                                if (result) *result = strlen(t->lexeme);
                                return true;
                        } else {
                                return false;
                        }
                } else {
                        return false;
                }

        case BRACKET_EXPR:
                if (t->bracket_expr.body &&
                    char_in_str(str[offset], t->bracket_expr.body->lexeme)) {
                        bool ret = eval(t->next, str, offset + 1, NULL);
                        if (result) *result = ret ? 1 : 0;
                        return ret;
                } else {
                        return false;
                }

        case BRACKET_EXPR_EXCL:
                if (!char_in_str(str[offset], t->bracket_expr.body->lexeme)) {
                        bool ret = eval(t->next, str, offset + 1, NULL);
                        if (result) *result = ret ? 1 : 0;
                        return ret;
                } else {
                        return 0;
                }

        case GROUP: {
                int n;
                if (eval(t->group.body, str, offset, &n)) {
                        if (eval(t->next, str, offset + n, NULL)) {
                                if (result) *result = n;
                                return true;
                        }
                        return false;
                }
                return false;
        }

        case MATCH_ZERO_MORE:
                return match_range(t->match_zero_more.match, t->next, str, offset, result, 0, 999);

        case MATCH_ZERO_ONE:
                return match_range(t->match_zero_one.match, t->next, str, offset, result, 0, 1);

        case MATCH_ONE_MORE:
                return match_range(t->match_one_more.match, t->next, str, offset, result, 1, 999);

        case MATCH_RANGE: {
                char *start = t->match_range.range->lexeme;
                int max = 999;
                char *c;
                if ((c = strchr(start, ','))) {
                        *c = 0;
                        max = c[1] ? atoi(c + 1) : max;
                }
                int min = atoi(start);
                return match_range(t->match_range.match, t->next, str, offset, result, min, max);
        }

        case MATCH_OR: {
                int n;
                if (eval(t->match_or.left, str, offset, &n)) {
                        if (result) *result = n;
                        return eval(t->next, str, offset + n, NULL);
                }
                if (eval(t->match_or.right, str, offset, &n)) {
                        if (result) *result = n;
                        return eval(t->next, str, offset + n, NULL);
                }
                return false;
        }

        case MATCH_GROUP: // logic for this not implemented
        default:
                todo("case for %s", TOKREPR[t->type]);
        }
}

bool
regex_match(Regex expr, char *str)
{
        int o = 0;
        if (expr.tokens == NULL) return false;
        if (expr.tokens->type == START_OF_LINE)
                return eval(expr.tokens, str, 0, 0);
        do {
                if (eval(expr.tokens, str, o, 0)) return true;
                ++o;
        } while (str[o]);
        return false;
}

char *
regex_repr(Regex expr)
{
        return expr.repr;
}
