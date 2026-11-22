#include "dsl.h"
#include <assert.h>
#include "bin_tree.h"

Node *const_node(tree_el_t data)
{
    return make_new_node(DT_CONST, data, NULL, NULL);
}

Node *add_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_ADD, left, right);
}

Node *sub_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_SUB, left, right);
}

Node *mul_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_MUL, left, right);
}

Node *div_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_DIV, left, right);
}

Node *pow_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_POW, left, right);
}

Node *root_node(Node *left, Node *right)
{
    assert(left);
    assert(right);

    return make_new_node(DT_OP, OP_ROOT, left, right);
}

Node *sqrt_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_SQRT, NULL, node);
}

Node *ln_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_LN, NULL, node);
}

Node *sin_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_SIN, NULL, node);
}

Node *cos_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_COS, NULL, node);
}

Node *tg_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_TG, NULL, node);
}

Node *ctg_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_CTG, NULL, node);
}

Node *arcsin_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_ARCSIN, NULL, node);
}

Node *arccos_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_ARCCOS, NULL, node);
}

Node *arctg_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_ARCTG, NULL, node);
}

Node *arcctg_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_ARCCTG, NULL, node);
}

Node *sh_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_SH, NULL, node);
}

Node *ch_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_CH, NULL, node);
}

Node *th_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_TH, NULL, node);
}

Node *cth_node(Node *node)
{
    assert(node);

    return make_new_node(DT_OP, OP_CTH, NULL, node);
}
