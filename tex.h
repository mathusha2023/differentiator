#ifndef TEX_H
#define TEX_H

#include <stdio.h>
#include "bin_tree.h"

enum TexMode
{
    TM_FUNCTION,
    TM_DERIVATIVE,
    TM_N_DERIVATIVE,
};

void init_tex(const char *tex_filename);
void close_tex(const char *tex_filename);
void generate_tex_pdf(const char *tex_filename);
int dump_tree_to_tex(BinTree *tree, const char *tex_filename, enum TexMode tex_mode, enum Variables diff_var, int extra);
void dump_node_to_tex(Node *node, FILE *file);

#endif // TEX_H