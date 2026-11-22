#ifndef DSL_H
#define DSL_H

#include "diff.h"
#include "bin_tree.h"

#define DL diff(node_left, diff_var)
#define DR diff(node_right, diff_var)
#define CL copy_node(node_left)
#define CR copy_node(node_right)

Node *const_node(tree_el_t data);

Node *add_node(Node *left, Node *right);
Node *sub_node(Node *left, Node *right);
Node *mul_node(Node *left, Node *right);
Node *div_node(Node *left, Node *right);

Node *pow_node(Node *left, Node *right);
Node *root_node(Node *left, Node *right);
Node *sqrt_node(Node *node);
Node *ln_node(Node *node);

Node *sin_node(Node *node);
Node *cos_node(Node *node);
Node *tg_node(Node *node);
Node *ctg_node(Node *node);

Node *arcsin_node(Node *node);
Node *arccos_node(Node *node);
Node *arctg_node(Node *node);
Node *arcctg_node(Node *node);

Node *sh_node(Node *node);
Node *ch_node(Node *node);
Node *th_node(Node *node);
Node *cth_node(Node *node);

#endif // DSL_H
