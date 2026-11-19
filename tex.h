#ifndef TEX_H
#define TEX_H

#include <stdio.h>
#include "bin_tree.h"

int dump_tree_to_tex(BinTree *tree, const char *tex_filename);
void dump_node_to_tex(Node *node, FILE *file);

#endif // TEX_H