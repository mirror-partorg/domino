#include <string.h>
#include <ctype.h>
#include <domino.h>
#include <locale.h>
#include "../misc/define.h"


typedef enum {
    R3_TYPE_STRING,
    R3_TYPE_INTEGER,
    R3_TYPE_REAL,
} do_r3_type_t;

struct _do_r3_var_t {
    do_r3_type_t type;
    union {
        char *string;
        double real;
        int integer;
    };
};

typedef enum {
    lx_less,
    lx_less_equal,
    lx_greate,
    lx_greate_equal,
    lx_equal,
    lx_not_equal,
    lx_minus,
    lx_plus,
    lx_multi,
    lx_and,
    lx_div,
    lx_colon,
    lx_question,
    lx_left_bracket,
    lx_right_bracket,
    lx_cstring,
    lx_cinteger,
    lx_ident,
    lx_creal,
    lx_end,
    lx_error
} lex_t;

typedef enum {
    oper_less,
    oper_less_equal,
    oper_greate,
    oper_greate_equal,
    oper_equal,
    oper_not_equal,
    oper_minus,
    oper_plus,
    oper_multi,
    oper_and,
    oper_div,
    oper_condition,
    oper_const,
    oper_ident
} oper_t;

static const oper_t oper_lex[lx_div + 1] = {oper_less, oper_less_equal, oper_greate, oper_greate_equal, oper_equal, oper_not_equal, oper_minus, oper_plus, oper_multi, oper_and, oper_div};
static const int    oper_priority[oper_ident + 1] = {0,0,0,0,0,0,1,1,2,2,2,10,10};

#define MAX_LEXEM_LEN 100

typedef struct {
    int crnt;
    int crntln;
    char lexem[MAX_LEXEM_LEN + 1];
    const char *text;
    lex_t lex;
    char decimal_point;
} scanner_t;

typedef struct _expr_t expr_t;

struct _expr_t{
    expr_t *left;
    expr_t *right;
    oper_t oper;
    expr_t *condition;
    int    bracket;
    do_r3_var_t *value;
};

static lex_t   scanner_step(scanner_t *scanner);
static void    scanner_init(scanner_t *scanner, const char *text, int local);
static expr_t *parser(const char *text);
static do_r3_var_t *calc(expr_t *expr, do_r3_var_get_value_func value_func, void *user_data);
static void   expr_free(expr_t *expr);

static do_r3_var_t *do_r3_var(do_r3_var_t *value)
{
    do_r3_var_t *var = do_malloc0(sizeof(do_r3_var_t));
    var->type = value->type;
    switch ( var->type ) {
        case R3_TYPE_INTEGER:
            var->integer = value->integer;
            break;
        case R3_TYPE_REAL:
            var->real = value->real;
            break;
        case R3_TYPE_STRING:
            var->string = do_strdup(value->string);
            break;
    }
    return var;
}
static char *ftoa_(const double value, char *buf)
{
    sprintf(buf, "%f", value);
    char* cp;
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        cp = buf;
        for (; *cp; cp++)
            if (*cp == decimal_point)
            {
                *cp = '.';
                break;
            };
    }
    cp = buf + strlen(buf) - 1;
    while ( cp > buf && *cp == '0') cp--;
    if ( *cp == '.' ) cp--;
    cp[1] = '\0';
    return buf;
}
static double atof_(const char *str)
{
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* str1 = do_strdup(str);
        char* cp = str1;
        for (; *cp; cp++)
            if (*cp == '.')
            {
                *cp = decimal_point;
                break;
            };
        double result = atof(str1);
        do_free(str1);
        return result;
    }
    else
        return atof(str);
}

static do_r3_var_t *simple_expr_calc(const char *expression, do_r3_var_get_value_func value_func, void *user_data)
{
    expr_t *expr;
    do_r3_var_t *retval;

    if ((expr = parser(expression)) == NULL)
        return NULL;
    retval = calc(expr, value_func, user_data);
    expr_free(expr);

    return retval;
}

static void scanner_init(scanner_t *scanner, const char *text, int local)
{
    scanner->crnt = 0;
    scanner->crntln = 1;
    scanner->text = text;
    scanner->lexem[0] = '\0';
    if ( local ) {
        struct lconv *s = localeconv();
        if (s)
            scanner->decimal_point = s->decimal_point[0];
        else
            scanner->decimal_point = '.';
    }
    else
        scanner->decimal_point = '.';
}
static void scanner_copy(scanner_t *dest, scanner_t *src)
{
    dest->crnt = src->crnt;
    dest->crntln = src->crntln;
    dest->text = src->text;
    strcpy(dest->lexem, src->lexem);
}


static lex_t scanner_step(scanner_t *scanner)
{
    int len = 0;
    for (; isspace(scanner->text[scanner->crnt]) || scanner->text[scanner->crnt] == '\r' || scanner->text[scanner->crnt] == '\n' ; (scanner->crnt)++ )
        if ( scanner->text[scanner->crnt] == '\n' ) (scanner->crntln)++;

    if (scanner->text[scanner->crnt] == '\0')
        return scanner->lex = lx_end;

    if (scanner->text[scanner->crnt] == '-') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_minus;
    }

    if ( (scanner->text[scanner->crnt] == '!' &&
          scanner->text[scanner->crnt + 1] == '=') ||
         (scanner->text[scanner->crnt] == '<' &&
          scanner->text[scanner->crnt + 1] == '>')
        ) {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = scanner->text[scanner->crnt+1];
        scanner->lexem[2] = '\0';
        (scanner->crnt)++;
        (scanner->crnt)++;
        return scanner->lex = lx_not_equal;
    }

    if (scanner->text[scanner->crnt] == '<') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        if ( scanner->text[scanner->crnt] == '=' ) {
            scanner->lexem[1] = scanner->text[scanner->crnt];
            scanner->lexem[2] = '\0';
            (scanner->crnt)++;
            return scanner->lex = lx_less_equal;
        }
        else
            return scanner->lex = lx_less;
    }
    if (scanner->text[scanner->crnt] == '>') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        if ( scanner->text[scanner->crnt] == '=' ) {
            scanner->lexem[1] = scanner->text[scanner->crnt];
            scanner->lexem[2] = '\0';
            (scanner->crnt)++;
            return scanner->lex = lx_greate_equal;
        }
        else
            return scanner->lex = lx_greate;
    }

    if (scanner->text[scanner->crnt] == '=') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_equal;
    }

    if (scanner->text[scanner->crnt] == '?') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_question;
    }

    if (scanner->text[scanner->crnt] == ':') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_colon;
    }

    if (scanner->text[scanner->crnt] == '+') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_plus;
    }

    if (scanner->text[scanner->crnt] == '*') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_multi;
    }
    if (scanner->text[scanner->crnt] == '&') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_and;
    }

    if (scanner->text[scanner->crnt] == '/') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_div;
    }

    if (scanner->text[scanner->crnt] == '(') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_left_bracket;
    }

    if (scanner->text[scanner->crnt] == ')') {
        scanner->lexem[0] = scanner->text[scanner->crnt];
        scanner->lexem[1] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_right_bracket;
    }

    if (isdigit(scanner->text[scanner->crnt])) {
        for (; isdigit(scanner->text[scanner->crnt]); (scanner->crnt)++)
            scanner->lexem[len++] = scanner->text[scanner->crnt];
        if (scanner->text[scanner->crnt] != '.' &&
            scanner->text[scanner->crnt] != ',') {
            scanner->lexem[len] = '\0';
            return scanner->lex = lx_cinteger;
        }
        scanner->lexem[len++] = scanner->decimal_point;//scanner->text[(scanner->crnt)++];
        (scanner->crnt)++;
        for (; isdigit(scanner->text[scanner->crnt]); (scanner->crnt)++)
            scanner->lexem[len++] = scanner->text[scanner->crnt];
        scanner->lexem[len] = '\0';
        return scanner->lex = lx_creal;
    }

    if (scanner->text[scanner->crnt] == '\'' ) {
        (scanner->crnt)++;
        for (;scanner->text[scanner->crnt] != '\0' ; (scanner->crnt)++) {
            if (scanner->text[scanner->crnt] == '\'') {
                if (scanner->text[scanner->crnt + 1] != '\'')
                    break;
                else
                    (scanner->crnt)++;
            }
            scanner->lexem[len++] = scanner->text[scanner->crnt];
        }
        if (scanner->text[scanner->crnt] != '\'')
            return scanner->lex = lx_error;

        scanner->lexem[len] = '\0';
        (scanner->crnt)++;
        return scanner->lex = lx_cstring;
    }
    // sad but true
    while ( !isspace(scanner->text[scanner->crnt]) &&
             scanner->text[scanner->crnt] != '\r' &&
             scanner->text[scanner->crnt] != '\n' &&
             scanner->text[scanner->crnt] != '\0' &&
             scanner->text[scanner->crnt] != '-' &&
             scanner->text[scanner->crnt] != '+' &&
             scanner->text[scanner->crnt] != '*' &&
             scanner->text[scanner->crnt] != '&' &&
             scanner->text[scanner->crnt] != '/' &&
             scanner->text[scanner->crnt] != '?' &&
             scanner->text[scanner->crnt] != ':' &&
             scanner->text[scanner->crnt] != '<' &&
             scanner->text[scanner->crnt] != '>' &&
             scanner->text[scanner->crnt] != '!' &&
             scanner->text[scanner->crnt] != '=' &&
             scanner->text[scanner->crnt] != '(' &&
             scanner->text[scanner->crnt] != ')' &&
             scanner->text[scanner->crnt] != '\'' ) {
        scanner->lexem[len++] = scanner->text[scanner->crnt];
        (scanner->crnt)++;
    }
    scanner->lexem[len] = '\0';
    if ( len )
        scanner->lex = lx_ident;
    else
        scanner->lex = lx_error;
    return scanner->lex;
}

static expr_t *expression(scanner_t *scan);


static expr_t *operand(scanner_t *scan)
{
    expr_t *retval, *right;
    lex_t lex;

    lex = scanner_step(scan);
    switch (lex) {
        case lx_left_bracket:
            retval = expression(scan);
            if (!retval)
                return NULL;
            lex = scanner_step(scan);
            if (lex != lx_right_bracket) {
                expr_free(retval);
                return NULL;
            }
            retval->bracket = 1;
            return retval;
        case lx_minus:
            right = operand(scan);
            if (!right)
                return NULL;
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_minus;
            retval->right = right;
            return retval;
        case lx_plus:
            right = operand(scan);
            if (!right)
                return NULL;
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_plus;
            retval->right = right;
            return retval;
        case lx_cinteger:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_const;
            retval->value = do_r3_var_int(atoi(scan->lexem));
            return retval;
        case lx_creal:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_const;
            retval->value = do_r3_var_double(atof_(scan->lexem));
            return retval;
        case lx_cstring:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_const;
            retval->value = do_r3_var_string(scan->lexem);
            return retval;
        case lx_ident:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_ident;
            retval->value = do_r3_var_string(scan->lexem);
            return retval;
        default:
            return NULL;
    }
}
static expr_t *expression(scanner_t *scan)
{
    expr_t *left, *right, *retval;
    lex_t lex;
    scanner_t scan1;

    left = operand(scan);
    if ( !left )
        return NULL;
    scanner_copy(&scan1, scan);
    lex = scanner_step(scan);
    switch ( lex ) {
        case lx_end:
            return left;
        case lx_colon:
            scanner_copy(scan, &scan1);
            return left;
        case lx_right_bracket:
            scanner_copy(scan, &scan1);
            return left;
        case lx_less:
        case lx_less_equal:
        case lx_greate:
        case lx_greate_equal:
        case lx_equal:
        case lx_not_equal:
        case lx_and:
        case lx_minus:
        case lx_plus:
        case lx_multi:
        case lx_div:
            right = expression(scan);
            if ( !right ) {
                expr_free(left);
                return NULL;
            }
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_lex[lex];
            retval->left = left;
            if ( right->bracket == 0 && // not bracket
                 right->left != NULL && // not unary
                 oper_priority[right->oper] < oper_priority[retval->oper]
                ) {
                retval->right = right->left;
                right->left = retval;
                retval = right;
            }
            else
                retval->right = right;

            return retval;
        case lx_question:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_condition;
            retval->condition = left;
            retval->left = expression(scan);
            if ( !retval->left ) {
                expr_free(retval);
                return FALSE;
            }
            if ( scanner_step(scan) != lx_colon ) {
                expr_free(retval);
                return FALSE;
            }
            retval->right = expression(scan);
            if ( !retval->right ) {
                expr_free(retval);
                return FALSE;
            }
            return retval;
        default:
            expr_free(left);
            return NULL;
    }
}
static expr_t *parser(const char *text)
{
    expr_t *retval;
    scanner_t scan;
    scanner_init(&scan, text, FALSE);

    if ( scanner_step(&scan) == lx_end ) {
        retval = do_malloc0(sizeof(expr_t));
        retval->oper = oper_const;
        retval->value = do_r3_var_string("");
        return retval;
    }

    scanner_init(&scan, text, FALSE);
    retval = expression(&scan);

    if (retval && scanner_step(&scan) != lx_end) {
        expr_free(retval);
        return NULL;
    }
    return retval;

}

static do_r3_var_t *calc(expr_t *expr, do_r3_var_get_value_func value_func, void *user_data)
{
    if ( !expr )
        return NULL;

    do_r3_var_t *left = NULL;
    do_r3_var_t *right = NULL;
    do_r3_var_t *retval = NULL;
    switch ( expr->oper ) {
        case oper_less:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) < 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) <
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) <
                                     do_r3_var_value_int(right)
                                    );
            break;
        case oper_less_equal:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) <= 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) <=
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) <=
                                     do_r3_var_value_int(right)
                                    );
            break;
        case oper_greate:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) > 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) >
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) >
                                     do_r3_var_value_int(right)
                                    );
            break;
        case oper_greate_equal:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) >= 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) >=
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) >=
                                     do_r3_var_value_int(right)
                                    );
            break;
        case oper_equal:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) == 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) ==
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) ==
                                     do_r3_var_value_int(right)
                                    );
            break;
        case oper_not_equal:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;
            if ( left->type == R3_TYPE_STRING &&
                 right->type == R3_TYPE_STRING ) {
                 retval = do_r3_var_int(strcmp( left->string, right->string ) != 0);
                 break;
            }
            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL ) {
                 retval = do_r3_var_int( do_r3_var_value_double(left) !=
                                         do_r3_var_value_double(right)
                                        );
                 break;
            }
            retval = do_r3_var_int( do_r3_var_value_int(left) !=
                                     do_r3_var_value_int(right)
                                    );
            break;

        case oper_minus:
            if ( expr->left )
                left = calc(expr->left, value_func, user_data);
            else
                left = do_r3_var_int(0);
            if ( !left ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;

            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL )
                 retval = do_r3_var_double(do_r3_var_value_double(left) -
                                           do_r3_var_value_double(right));
            else
                 retval = do_r3_var_int(do_r3_var_value_int(left) -
                                           do_r3_var_value_int(right));
            break;
        case oper_plus:
            if ( expr->left )
                left = calc(expr->left, value_func, user_data);
            else
                left = do_r3_var_int(0);
            if ( !left ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;

            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL )
                 retval = do_r3_var_double(do_r3_var_value_double(left) +
                                           do_r3_var_value_double(right));
            else
                 retval = do_r3_var_int(do_r3_var_value_int(left) +
                                           do_r3_var_value_int(right));
            break;
        case oper_multi:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;

            if ( left->type == R3_TYPE_REAL ||
                 right->type == R3_TYPE_REAL )
                 retval = do_r3_var_double(do_r3_var_value_double(left) *
                                           do_r3_var_value_double(right));
            else
                 retval = do_r3_var_int(do_r3_var_value_int(left) *
                                           do_r3_var_value_int(right));
            break;
        case oper_div:
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;

            retval = do_r3_var_double(do_r3_var_value_double(left) /
                                           do_r3_var_value_double(right));
            break;
        case oper_and: {
            char *l, *r, *b;
            if ( (left = calc(expr->left, value_func, user_data)) == NULL ||
                 (right = calc(expr->right, value_func, user_data)) == NULL )
                break;

            l = do_r3_var_value_string(left);
            r = do_r3_var_value_string(right);
            b = do_malloc0(strlen(l) + strlen(r) + 1);
            sprintf(b, "%s%s", l, r);
            retval = do_r3_var_string(b);
            do_free(l);do_free(r);do_free(b);
            break;
        }
        case oper_condition:
            if ( (left = calc(expr->condition, value_func, user_data)) == NULL )
                break;
            if ( do_r3_var_value_int(left) )
                right = calc(expr->left, value_func, user_data);
            else
                right = calc(expr->right, value_func, user_data);
            if ( !right )
                break;
            retval = do_r3_var(right);
            break;
        case oper_const:
            retval = do_r3_var(expr->value);
            break;
        case oper_ident:
            if ( !value_func )
                retval = NULL;
            else
                retval = value_func(expr->value->string, user_data);
    }
    if ( left )
        do_r3_var_free(left);
    if ( right )
        do_r3_var_free(right);
    return retval;
}
static void expr_free(expr_t *expr)
{
    if (!expr)
        return;
    if (expr->left)
        expr_free(expr->left);
    if (expr->right)
        expr_free(expr->right);
    if (expr->condition)
        expr_free(expr->condition);
    if (expr->value)
        do_r3_var_free(expr->value);
    do_free(expr);
}
/*
static double simple_expr_tof(const char *expression)
{
    char lexem[MAX_LEXEM_LEN];
    double retval = 0;
    int crnt = 0, len = 0;
    char decimal_point = '.';
    struct lconv *s = localeconv();
    if (s)
        decimal_point = s->decimal_point[0];
    if (expression[crnt] == '-')
            lexem[len++] = expression[crnt++];
    if (isdigit(expression[crnt]) || expression[crnt] == '-') {
        for (; isdigit(expression[crnt]); crnt++)
            lexem[len++] = expression[crnt];
        if (expression[crnt] != '.' &&
            expression[crnt] != ',') {
            lexem[len] = '\0';
        }
        else {
            lexem[len++] = decimal_point;//scanner->text[(scanner->crnt)++];
            crnt++;
            for (; isdigit(expression[crnt]); crnt++)
                lexem[len++] = expression[crnt];
            lexem[len] = '\0';
        }
        retval = atof(lexem);
    }
    return retval;
}
*/



DO_EXPORT do_r3_var_t *do_r3_expr_calc(const char *expression, do_r3_var_get_value_func value_func, void *user_data)
{
    do_r3_var_t *res;
    res = simple_expr_calc(expression, value_func, user_data);
    if ( !res )
        do_log(LOG_ERR, "Error parce R3 \"%s\"", expression);
    return res;
}
DO_EXPORT int do_r3_var_value_int(do_r3_var_t *var)
{
    switch ( var->type ) {
        case R3_TYPE_INTEGER:
            return var->integer;
        case R3_TYPE_REAL:
            return var->real;
        case R3_TYPE_STRING:
            return atoi(var->string);
    }
    return 0;
}
DO_EXPORT double do_r3_var_value_double(do_r3_var_t *var)
{
    switch ( var->type ) {
        case R3_TYPE_INTEGER:
            return var->integer;
        case R3_TYPE_REAL:
            return var->real;
        case R3_TYPE_STRING:
            return atof_(var->string);
    }
    return 0;
}
DO_EXPORT char *do_r3_var_value_string(do_r3_var_t *var)
{
    char *res=NULL;
    switch ( var->type ) {
        case R3_TYPE_INTEGER:
            res = do_malloc(50);
            sprintf(res, "%d", var->integer);
            break;
        case R3_TYPE_REAL:
            res = do_malloc(100);
            ftoa_(var->real, res);
            break;
        case R3_TYPE_STRING:
            return do_strdup(var->string);
    }
    return res;
}
DO_EXPORT void do_r3_var_free(do_r3_var_t *var)
{
    switch ( var->type ) {
        case R3_TYPE_INTEGER:
        case R3_TYPE_REAL:
            break;
        case R3_TYPE_STRING:
            do_free(var->string);
            break;
    }
    do_free(var);
}
DO_EXPORT do_r3_var_t *do_r3_var_int(int value)
{
    do_r3_var_t *var = do_malloc0(sizeof(do_r3_var_t));
    var->type = R3_TYPE_INTEGER;
    var->integer = value;
    return var;
}
DO_EXPORT do_r3_var_t *do_r3_var_double(double value)
{
    do_r3_var_t *var = do_malloc0(sizeof(do_r3_var_t));
    var->type = R3_TYPE_REAL;
    var->real = value;
    return var;
}
DO_EXPORT do_r3_var_t *do_r3_var_string(const char *value)
{
    do_r3_var_t *var = do_malloc0(sizeof(do_r3_var_t));
    var->type = R3_TYPE_STRING;
    var->string = do_strdup(value);
    return var;
}
