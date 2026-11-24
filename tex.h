#ifndef TEX_H
#define TEX_H

#include <stdio.h>
#include "bin_tree.h"

enum TexMode
{
    TM_NO_FUNCTION,
    TM_FUNCTION,
    TM_DERIVATIVE,
    TM_N_DERIVATIVE,
};

void init_tex(const char *tex_filename);
void close_tex(const char *tex_filename);
int write_text_to_tex(const char *tex_filename, const char *const text, ...);
void generate_tex_pdf(const char *tex_filename);
int dump_tree_to_tex(BinTree *tree, const char *tex_filename, enum TexMode tex_mode, enum Variables diff_var, int extra, int boxed);
void dump_node_to_tex(Node *node, FILE *file);
int dump_equal_nodes_to_tex(Node *node1, Node *node2, const char *tex_filename);

#endif // TEX_H