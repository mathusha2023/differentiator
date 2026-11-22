#include "diff.h"
#include <assert.h>
#include "log.h"
#include "bin_tree.h"
#include "my_string.h"
#include "tree_io.h"
#include "dsl.h"

void diff_tree(BinTree *tree, BinTree *dtree, enum Variables diff_var)
{
    dtree->root = diff(tree->root, diff_var);

    FLOG("Tree after diff:");
    tree_dump(dtree);
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
    return STR_VARIABLES[var];
}

const char *get_str_node_data_type(NodeDataType type)
{
    return STR_NODE_DATA_TYPES[type];
}
