#include "simple_expr.h"
#include <string.h>
#include <ctype.h>
#include <dolib.h>
#include <locale.h>


typedef enum {
    lx_minus,
    lx_plus,
    lx_multi,
    lx_div,
    lx_number,
    lx_left_bracket,
    lx_right_bracket,
    lx_end,
    lx_error
} lex_t;

typedef enum {
    oper_minus,
    oper_plus,
    oper_multi,
    oper_div,
    oper_const,
} oper_t;

static const oper_t oper_lex[lx_div + 1] = {oper_minus, oper_plus, oper_multi, oper_div};
static const int    oper_priority[oper_const + 1] = {1,1,2,2,10};


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
    int    bracket;
    double value;
};

static lex_t   scanner_step(scanner_t *scanner);
static void    scanner_init(scanner_t *scanner, const char *text);
static expr_t *parser(const char *text);
static double calc(expr_t *expr, int *error);
static void   expr_free(expr_t *expr);

double simple_expr_calc(const char *expression, int *error)
{
    expr_t *expr;
    double retval;

    if ((expr = parser(expression)) == NULL) {
        *error = 1;
        return 0;
    }
    *error = 0;
    retval = calc(expr, error);
    expr_free(expr);

    return retval;
}

static void scanner_init(scanner_t *scanner, const char *text)
{
    scanner->crnt = 0;
    scanner->crntln = 1;
    scanner->text = text;
    scanner->lexem[0] = '\0';
    struct lconv *s = localeconv();
    if (s)
        scanner->decimal_point = s->decimal_point[0];
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
            return scanner->lex = lx_number;
        }
        scanner->lexem[len++] = scanner->decimal_point;//scanner->text[(scanner->crnt)++];
        (scanner->crnt)++;
        for (; isdigit(scanner->text[scanner->crnt]); (scanner->crnt)++)
            scanner->lexem[len++] = scanner->text[scanner->crnt];
        scanner->lexem[len] = '\0';
        return scanner->lex = lx_number;
    }
    (scanner->crnt) ++;
    return scanner->lex = lx_error;
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
        case lx_number:
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_const;
            retval->value = atof(scan->lexem);
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
    if (!left)
        return NULL;
    scanner_copy(&scan1, scan);
    lex = scanner_step(scan);
    switch (lex) {
        case lx_end:
            return left;
        case lx_right_bracket:
            scanner_copy(scan, &scan1);
            return left;
        case lx_minus:
        case lx_plus:
        case lx_multi:
        case lx_div:
            right = expression(scan);
            if (!right) {
                expr_free(left);
                return NULL;
            }
            retval = do_malloc0(sizeof(expr_t));
            retval->oper = oper_lex[lex];
            retval->left = left;
            if (right->bracket == 0 && // not bracket
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
        default:
            expr_free(left);
            return NULL;
    }
}
static expr_t *parser(const char *text)
{
    expr_t *retval;
    scanner_t scan;
    scanner_init(&scan, text);
    retval = expression(&scan);

    if (retval && scanner_step(&scan) != lx_end) {
        expr_free(retval);
        return NULL;
    }
    return retval;

}

static double calc(expr_t *expr, int *error)
{
    if (!expr) {
        *error = 1;
        return 0;
    }

    double left, right, retval = 0;
    switch (expr->oper) {
        case oper_minus:
            if (expr->left)
                left = calc(expr->left, error);
            else
                left = 0;
            if (*error == 0)
                right = calc(expr->right, error);
            if (*error == 0)
                retval = left - right;
            break;
        case oper_plus:
            if (expr->left)
                left = calc(expr->left, error);
            else
                left = 0;
            if (*error == 0)
                right = calc(expr->right, error);
            if (*error == 0)
                retval = left + right;
            break;
        case oper_multi:
            left = calc(expr->left, error);
            if (*error == 0)
                right = calc(expr->right, error);
            if (*error == 0)
                retval = left * right;
            break;
        case oper_div:
            left = calc(expr->left, error);
            if (*error == 0)
                right = calc(expr->right, error);
            if (*error == 0)
                retval = left / right;
            break;
        case oper_const:
            retval = expr->value;
            break;
    }
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
    do_free(expr);
}

double simple_expr_tof(const char *expression)
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
