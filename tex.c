#include "tex.h"
#include <assert.h>
#include <stdio.h>
#include "log.h"
#include "bin_tree.h"

int dump_tree_to_tex(BinTree *tree, const char *tex_filename)
{
    assert(tree);
    assert(tex_filename);

    FILE *file = fopen(tex_filename, "w");
    if (!file)
    {
        FLOG("Error: can not open tex file '%s'", tex_filename);
        return 1;
    }

    fprintf(file, "\\documentclass{article}\n\\usepackage{amsmath}\n\n");
    fprintf(file, "\\begin{document}\n\n");

    fprintf(file, "\\[\n");

    if (tree->root)
        dump_node_to_tex(tree->root, file);
    else
        FLOG("WARNING: tree will be written in file '%s' is void", tex_filename);

    fprintf(file, "\n\\]\n");

    fprintf(file, "\n\n\\end{document}");
    fclose(file);

    return 0;
}

void dump_node_to_tex(Node *node, FILE *file)
{
    assert(file);

    if (!node)
        return;

    switch (node->type)
    {
    case DT_NUM:
        fprintf(file, TREE_EL_SPECIFICATOR, node->data);
        return;
    case DT_VAR:
        fprintf(file, "%s", get_str_variable((enum Variables)node->data));
        return;
    case DT_OP:
    default:
        break;
    }

    // node->type == DT_OP
    enum Operators op = (enum Operators)node->data;
    const char *str_op = get_str_tex_operator(op);

    if (op == OP_DIV)
    {
        fprintf(file, "%s{", str_op);
        dump_node_to_tex(node->left, file);
        fprintf(file, "}{");
        dump_node_to_tex(node->right, file);
        fprintf(file, "}");
    }
    else if (op == OP_POW)
    {
        fprintf(file, "{");
        dump_node_to_tex(node->left, file);
        fprintf(file, "}%s{", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, "}");
    }
    else if (is_unary_op(op))
    {
        fprintf(file, "%s\\left(", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, "\\right)");
    }
    else
    {
        fprintf(file, "(");
        dump_node_to_tex(node->left, file);
        fprintf(file, " %s ", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, ")");
    }
}
