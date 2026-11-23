#include "diff.h"
#include <assert.h>
#include <math.h>
#include "log.h"
#include "bin_tree.h"
#include "my_string.h"
#include "tree_io.h"
#include "dsl.h"

static int is_zero(Node *node);
static int is_one(Node *node);
static int is_minus_one(Node *node);
static int is_e(Node *node);

BinTree diff_tree(BinTree *tree, enum Variables diff_var)
{
    BinTree tmp_tree = {};
    tmp_tree.root = diff(tree->root, diff_var);

    FLOG("<h1>Tree after diff without constants folding:</h1>");
    tree_dump(&tmp_tree);

    BinTree dtree = {};

    dtree.root = fold_consts(tmp_tree.root);
    dtree.n_nodes = count_tree_size(&dtree);

    tree_destroy(&tmp_tree);

    FLOG("<h1>Tree after diff with constant folding:</h1>");
    tree_dump(&dtree);

    return dtree;
}

Node *diff(Node *node, enum Variables diff_var)
{
    assert(node);

    switch (node->type)
    {
    case DT_CONST:
        return const_node(0);

    case DT_VAR:
        return const_node((enum Variables)node->data == diff_var);

    case DT_OP:
        return diff_operator(node->left, node->right, (enum Operators)node->data, diff_var);

    default:
        LOG("HUETAAAAAAA");
        return NULL;
    }
}

Node *diff_operator(Node *node_left, Node *node_right, enum Operators op, enum Variables diff_var)
{
    if (is_unary_op(op))
        assert(!node_left);
    else
        assert(node_left);
    assert(node_right);

    switch (op)
    {
    case OP_ADD:
        return add_node(DL, DR);

    case OP_SUB:
        return sub_node(DL, DR);

    case OP_MUL:
        return add_node(mul_node(DL, CR), mul_node(CL, DR));

    case OP_DIV:
    {
        Node *sub = sub_node(mul_node(DL, CR), mul_node(CL, DR));
        Node *pow = pow_node(CR, const_node(2));
        return div_node(sub, pow);
    }
    case OP_POW:
    {
        if (node_right->type == DT_CONST)
        {
            Node *pow = pow_node(CL, const_node(node_right->data - 1));

            return mul_node(mul_node(CR, pow), DL);
        }
        else
        {
            Node *pow = pow_node(CL, CR);

            if (node_left->type == DT_VAR && is_e(node_left))
                return mul_node(pow, diff(CR, diff_var));

            Node *mul = mul_node(CR, ln_node(CL));
            return mul_node(pow, diff(mul, diff_var));
        }
    }
    case OP_ROOT:
    {
        Node *pow = div_node(const_node(1), CL);
        return diff(pow_node(CR, pow), diff_var);
    }
    case OP_SQRT:
    {
        Node *two_sqrts = mul_node(const_node(2), sqrt_node(CR));
        return mul_node(div_node(const_node(1), two_sqrts), DR);
    }
    case OP_LN:
    {
        return div_node(DR, CR);
    }
    case OP_SIN:
        return mul_node(cos_node(CR), DR);

    case OP_COS:
        return mul_node(mul_node(const_node(-1), sin_node(CR)), DR);

    case OP_TG:
    {
        Node *cos2 = pow_node(cos_node(CR), const_node(2));
        return mul_node(div_node(const_node(1), cos2), DR);
    }
    case OP_CTG:
    {
        Node *sin2 = pow_node(sin_node(CR), const_node(2));
        return mul_node(div_node(const_node(-1), sin2), DR);
    }
    case OP_ARCSIN:
    {
        Node *sub = sub_node(const_node(1), pow_node(CR, const_node(2)));
        Node *div = div_node(const_node(1), sqrt_node(sub));
        return mul_node(div, DR);
    }
    case OP_ARCCOS:
    {
        Node *sub = sub_node(const_node(1), pow_node(CR, const_node(2)));
        Node *div = div_node(const_node(-1), sqrt_node(sub));
        return mul_node(div, DR);
    }
    case OP_ARCTG:
    {
        Node *add = add_node(const_node(1), pow_node(CR, const_node(2)));
        return mul_node(div_node(const_node(1), add), DR);
    }
    case OP_ARCCTG:
    {
        Node *add = add_node(const_node(1), pow_node(CR, const_node(2)));
        return mul_node(div_node(const_node(-1), add), DR);
    }
    case OP_SH:
        return mul_node(ch_node(CR), DR);

    case OP_CH:
        return mul_node(sh_node(CR), DR);

    case OP_TH:
    {
        Node *ch2 = pow_node(ch_node(CR), const_node(2));
        return mul_node(div_node(const_node(1), ch2), DR);
    }
    case OP_CTH:
    {
        Node *sh2 = pow_node(sh_node(CR), const_node(2));
        return mul_node(div_node(const_node(-1), sh2), DR);
    }
    default:
        FLOG("AAAAA HUIIIIIIII !!!!");
        return NULL;
    }
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
