#include "diff.h"
#include <assert.h>
#include <math.h>
#include "log.h"
#include "bin_tree.h"
#include "my_string.h"
#include "tree_io.h"
#include "dsl.h"
#include "tex.h"

#define diff_tree_check_error(err_msg, ...) \
    {                                       \
        if (error)                          \
        {                                   \
            LOG(err_msg, ##__VA_ARGS__);    \
            return (BinTree){};             \
        }                                   \
    }

#define diff_check_error(err_msg, ...)   \
    {                                    \
        if (error)                       \
        {                                \
            LOG(err_msg, ##__VA_ARGS__); \
            return NULL;                 \
        }                                \
    }

static int is_zero(Node *node);
static int is_one(Node *node);
static int is_minus_one(Node *node);
static int is_e(Node *node);
static int is_pi(Node *node);

BinTree diff_tree(BinTree *tree, enum Variables diff_var)
{
    assert(tree);

    int error = 0;

    error = write_text_to_tex(TEX_FILE_NAME, "Вычислим первую производную функции\n");
    diff_tree_check_error("Error while writing text to tex");

    dump_tree_to_tex(tree, TEX_FILE_NAME, TM_FUNCTION, VAR_X, 0, 0);
    diff_tree_check_error("Error while writing tree to tex");

    error = write_text_to_tex(TEX_FILE_NAME, "\nДля этого:\n");
    diff_tree_check_error("Error while writing text to tex");

    BinTree tmp_tree = {};
    tmp_tree.root = diff(tree->root, diff_var);

    FLOG("<h1>Tree after diff without constants folding:</h1>");
    tree_dump(&tmp_tree);

    BinTree dtree = {};

    dtree.root = fold_consts(tmp_tree.root);
    dtree.n_nodes = count_tree_size(&dtree);

    error = write_text_to_tex(TEX_FILE_NAME, "\nТеперь выполним свертку констант. Очевидно, что\n");
    diff_tree_check_error("Error while writing text to tex");

    dump_equal_nodes_to_tex(tmp_tree.root, dtree.root, TEX_FILE_NAME);
    diff_tree_check_error("Error while writing text to tex");

    tree_destroy(&tmp_tree);

    FLOG("<h1>Tree after diff with constant folding:</h1>");
    tree_dump(&dtree);

    return dtree;
}

Node *diff(Node *node, enum Variables diff_var)
{
    assert(node);

    int error = 0;

    error = write_text_to_tex(TEX_FILE_NAME, "\nВычислим производную функции\n");
    diff_check_error("Error while writing text to tex");

    BinTree t = {.root = node};

    error = dump_tree_to_tex(&t, TEX_FILE_NAME, TM_NO_FUNCTION, diff_var, 0, 0);
    diff_check_error("Error while writing tree to tex");

    Node *ret_node = NULL;

    switch (node->type)
    {
    case DT_CONST:
        error = write_text_to_tex(TEX_FILE_NAME, "\nПроизводная константы равна 0, \n");
        diff_check_error("Error while writing text to tex");

        ret_node = const_node(0);

        error = dump_equal_nodes_to_tex(node, ret_node, TEX_FILE_NAME);
        diff_check_error("Error writing equal nodes to tex");

        break;

    case DT_VAR:
    {
        enum Variables var = (enum Variables)node->data;
        ret_node = const_node(var == diff_var);

        if (var == diff_var)
            error = write_text_to_tex(TEX_FILE_NAME, "\nПроизводная переменной по самой себе равна 1, \n");
        else if (var == VAR_E || var == VAR_PI)
            error = write_text_to_tex(TEX_FILE_NAME, "\nПроизводная константы равна 0, \n");
        else
            error = write_text_to_tex(TEX_FILE_NAME, "\nПроизводная переменной, по которой не дифференцируем, равна производной константы, то есть 0, \n");

        diff_check_error("Error while writing text to tex");

        error = dump_equal_nodes_to_tex(node, ret_node, TEX_FILE_NAME);
        diff_check_error("Error writing equal nodes to tex");

        break;
    }

    case DT_OP:
        ret_node = diff_operator(node, node->left, node->right, (enum Operators)node->data, diff_var);
        break;

    default:
        LOG("HUETAAAAAAA");
    }

    return ret_node;
}

Node *diff_operator(Node *node, Node *node_left, Node *node_right, enum Operators op, enum Variables diff_var)
{
    if (is_unary_op(op))
        assert(!node_left);
    else
        assert(node_left);
    assert(node_right);

    Node *ret_node = NULL;

    switch (op)
    {
    case OP_ADD:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная суммы равна сумме производных, \n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(u + v)}' = {u}' + {v}'\\]\n");

        ret_node = add_node(DL, DR);
        break;

    case OP_SUB:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная разности равна разности производных, \n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(u - v)}' = {u}' - {v}'\\]\n");
        ret_node = sub_node(DL, DR);
        break;

    case OP_MUL:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная произведения считается по формуле\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(u \\cdot v)}' = {u}'{v} + {u}{v}'\\]\n");
        ret_node = add_node(mul_node(DL, CR), mul_node(CL, DR));
        break;

    case OP_DIV:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная частного вычисляется по формуле\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\frac{u}{v})}' = \\frac{{u}'{v} - {u}{v}}{v^{2}}'\\]\n");

        Node *sub = sub_node(mul_node(DL, CR), mul_node(CL, DR));
        Node *pow = pow_node(CR, const_node(2));
        ret_node = div_node(sub, pow);
        break;
    }
    case OP_POW:
    {
        if (node_right->type == DT_CONST || is_e(node_right) || is_pi(node_right))
        {
            write_text_to_tex(TEX_FILE_NAME, "\nПроизводная степенной функции считается как\n");
            write_text_to_tex(TEX_FILE_NAME, "\\[{({x}^{a})}' = a \\cdot {x}^{a-1}\\]\n");

            Node *pow = pow_node(CL, const_node(node_right->data - 1));

            ret_node = mul_node(mul_node(CR, pow), DL);
        }
        else
        {
            write_text_to_tex(TEX_FILE_NAME, "\nПроизводная степенной функции, у которой и основание, и показатель - функции, считается по формуле\n");
            write_text_to_tex(TEX_FILE_NAME, "\\[{({u}^{v})}' = ({u}^{v}) \\cdot {(u \\cdot \\ln{v})}'\\]\n");
            Node *pow = pow_node(CL, CR);

            if (node_left->type == DT_VAR && is_e(node_left))
            {
                write_text_to_tex(TEX_FILE_NAME, "\nТак как в основании лежит $e$, на логарифм можно забить, получим формулу\n");
                write_text_to_tex(TEX_FILE_NAME, "\\[{({e}^{v})}' = ({e}^{v}) \\cdot {u}'\\]\n");
                return mul_node(pow, diff(CR, diff_var));
            }

            Node *mul = mul_node(CR, ln_node(CL));
            ret_node = mul_node(pow, diff(mul, diff_var));
        }
        break;
    }
    case OP_ROOT:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводную корня действительной степени вычислим по формуле\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(sqrt[n]{x})}' = {({x}^\\frac{1}{x})}'\\]\n");

        Node *pow = div_node(const_node(1), CL);
        ret_node = diff(pow_node(CR, pow), diff_var);
        break;
    }
    case OP_SQRT:
    {

        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная квадратного корня вычисляется как производная действительной степени, \n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\sqrt{x})}' = \\frac{1}{2 \\cdot \\sqrt{x}}\\]\n");

        Node *two_sqrts = mul_node(const_node(2), sqrt_node(CR));
        ret_node = mul_node(div_node(const_node(1), two_sqrts), DR);
        break;
    }
    case OP_LN:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная натурального логарифма знакома каждому школьнику:\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\ln{x})}' = \\frac{1}{x}\\]\n");

        ret_node = div_node(DR, CR);
        break;
    }
    case OP_SIN:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная синуса - косинус\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\sin{x})}' = \\cos{x}\\]\n");

        ret_node = mul_node(cos_node(CR), DR);
        break;

    case OP_COS:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная косинуса - синус с другим знаком\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\cos{x})}' = -\\sin{x}\\]\n");

        ret_node = mul_node(mul_node(const_node(-1), sin_node(CR)), DR);
        break;

    case OP_TG:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная тангенса может быть посчитана как производная частного, но для тупых можно и запомнить формулу\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\tan{x})}' = \\frac{1}{(\\cos({x}))^{2}}\\]\n");

        Node *cos2 = pow_node(cos_node(CR), const_node(2));
        ret_node = mul_node(div_node(const_node(1), cos2), DR);
        break;
    }
    case OP_CTG:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная котангенса может быть посчитана как производная частного, но для тупых можно и запомнить формулу\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\ctan{x})}' = -\\frac{1}{(\\sin({x}))^{2}}\\]\n");

        Node *sin2 = pow_node(sin_node(CR), const_node(2));
        ret_node = mul_node(div_node(const_node(-1), sin2), DR);
        break;
    }
    case OP_ARCSIN:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная арксинуса получается из теоремы об обратной функции:\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\arcsin{x})}' = \\frac{1}{sqrt{1 - x^2}}\\]\n");

        Node *sub = sub_node(const_node(1), pow_node(CR, const_node(2)));
        Node *div = div_node(const_node(1), sqrt_node(sub));
        ret_node = mul_node(div, DR);
        break;
    }
    case OP_ARCCOS:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная арккосинуса может быть получена из его связи с арксинусом или просто по формуле:\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\arcsin{x})}' = \\frac{-1}{sqrt{1 - x^2}}\\]\n");

        Node *sub = sub_node(const_node(1), pow_node(CR, const_node(2)));
        Node *div = div_node(const_node(-1), sqrt_node(sub));
        ret_node = mul_node(div, DR);
        break;
    }
    case OP_ARCTG:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная арктангенса:\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\arctan{x})}' = \\frac{1}{1 + x^2}\\]\n");

        Node *add = add_node(const_node(1), pow_node(CR, const_node(2)));
        ret_node = mul_node(div_node(const_node(1), add), DR);
        break;
    }
    case OP_ARCCTG:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная арккотангенса:\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\arcctan{x})}' = \\frac{-1}{1 + x^2}\\]\n");

        Node *add = add_node(const_node(1), pow_node(CR, const_node(2)));
        ret_node = mul_node(div_node(const_node(-1), add), DR);
        break;
    }
    case OP_SH:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная шинуса - чосинус\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\sinh{x})}' = \\cosh{x}\\]\n");

        ret_node = mul_node(ch_node(CR), DR);
        break;

    case OP_CH:
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная чосинуса - шинус\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\cosh{x})}' = \\sinh{x}\\]\n");

        ret_node = mul_node(sh_node(CR), DR);
        break;

    case OP_TH:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная штангенса может быть посчитана как производная частного, но для тупых можно и запомнить формулу\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\tanh{x})}' = \\frac{1}{(\\cosh({x}))^{2}}\\]\n");

        Node *ch2 = pow_node(ch_node(CR), const_node(2));
        ret_node = mul_node(div_node(const_node(1), ch2), DR);
        break;
    }
    case OP_CTH:
    {
        write_text_to_tex(TEX_FILE_NAME, "\nПроизводная кочангенса может быть посчитана как производная частного, но для тупых можно и запомнить формулу\n");
        write_text_to_tex(TEX_FILE_NAME, "\\[{(\\cotanh{x})}' = \\frac{-1}{(\\sinh({x}))^{2}}\\]\n");

        Node *sh2 = pow_node(sh_node(CR), const_node(2));
        ret_node = mul_node(div_node(const_node(-1), sh2), DR);
        break;
    }
    default:
        FLOG("AAAAA HUIIIIIIII !!!!");
        return NULL;
    }

    write_text_to_tex(TEX_FILE_NAME, "\nТогда получим:\n");
    dump_equal_nodes_to_tex(node, ret_node, TEX_FILE_NAME);
    return ret_node;
}

Node *fold_consts(Node *node)
{
    if (!node)
        return NULL;

    if (node->type != DT_OP)
        return copy_node(node);

    enum Operators op = (enum Operators)node->data;

    Node *node_left = fold_consts(node->left);
    Node *node_right = fold_consts(node->right);

    // кибермув, кто прочитал тот гей
    if (is_unary_op(op))
        goto ret;

    assert(node_left);
    assert(node_right);

    if (node_left->type == DT_CONST && node_right->type == DT_CONST)
    {
        tree_el_t res = execute_bin_op(op, node_left->data, node_right->data);
        return const_node(res);
    }

    if (op == OP_ADD)
    {
        if (is_zero(node_left))
            return CR;
        if (is_zero(node_right))
            return CL;
    }
    else if (op == OP_SUB)
    {
        if (is_zero(node_left))
            return mul_node(const_node(-1), CR);
        if (is_zero(node_right))
            return CL;
    }
    else if (op == OP_MUL)
    {
        if (is_zero(node_left) || is_zero(node_right))
            return const_node(0);
        if (is_one(node_left))
            return CR;
        if (is_one(node_right))
            return CL;
        else if (node_left->type != DT_CONST && node_right->type == DT_CONST)
            return mul_node(CR, CL);
    }
    else if (op == OP_DIV)
    {
        if (is_zero(node_left))
            return const_node(0);
        if (is_one(node_right))
            return CL;
        if (is_minus_one(node_right))
            return mul_node(const_node(-1), CL);
    }
    else if (op == OP_POW)
    {
        if (is_zero(node_left))
            return const_node(0);
        if (is_zero(node_right))
            return const_node(1);
        if (is_one(node_left))
            return const_node(1);
        if (is_one(node_right))
            return CL;
        if (is_minus_one(node_right))
            return div_node(const_node(1), CL);
    }
    else if (op == OP_ROOT)
    {
        if (is_zero(node_right))
            return const_node(0);
        if (is_one(node_right))
            return const_node(1);
    }

ret:
    return make_new_node(DT_OP, op, node_left, node_right);
}

tree_el_t execute_bin_op(enum Operators op, tree_el_t left, tree_el_t right)
{
    assert(!is_unary_op(op));

    switch (op)
    {
    case OP_ADD:
        return left + right;
    case OP_SUB:
        return left - right;
    case OP_MUL:
        return left * right;
    case OP_DIV:
        return left / right;
    case OP_POW:
        return pow(left, right);
    case OP_ROOT:
        return pow(right, 1. / left);
    default:
        assert("HUIIIIIII" && 0);
        return 0.;
    }
}

int is_unary_op(enum Operators op)
{
    return (int)op >= 6;
}

const char *get_str_operator(enum Operators op)
{
    return STR_OPERATORS[op];
}

const char *get_str_tex_operator(enum Operators op)
{
    return STR_OPERATORS_TEX[op];
}

const char *get_str_variable(enum Variables var)
{
    if (var == VAR_PI)
        return "\\pi";
    return STR_VARIABLES[var];
}

const char *get_str_node_data_type(NodeDataType type)
{
    return STR_NODE_DATA_TYPES[type];
}

int is_equal(double a, double b)
{
    return fabs(a - b) < EPSILON;
}

static int is_zero(Node *node)
{
    assert(node);

    if (node->type != DT_CONST)
        return 0;

    return is_equal(node->data, 0);
}

static int is_one(Node *node)
{
    assert(node);

    if (node->type != DT_CONST)
        return 0;

    return is_equal(node->data, 1);
}

static int is_minus_one(Node *node)
{
    assert(node);

    if (node->type != DT_CONST)
        return 0;

    return is_equal(node->data, -1);
}

static int is_e(Node *node)
{
    assert(node);

    if (node->type != DT_VAR)
        return 0;

    return (enum Variables)node->data == VAR_E;
}

static int is_pi(Node *node)
{
    assert(node);

    if (node->type != DT_VAR)
        return 0;

    return (enum Variables)node->data == VAR_PI;
}
