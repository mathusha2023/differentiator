#include "tex.h"
#include <assert.h>
#include <stdio.h>
#include "log.h"
#include "bin_tree.h"

static void process_mul_operator(Node *node, FILE *file);
static void process_pow_operator(Node *node, FILE *file);

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

// логика дампа - еще на этапе выполнения операции позаботиться о том, чтобы у последующих записей
// было нужное количество скобок. Это позволяет убирать лишние скобки
void dump_node_to_tex(Node *node, FILE *file)
{
    assert(file);

    if (!node)
        return;

    switch (node->type)
    {
    case DT_CONST:
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
    else if (op == OP_MUL)
    {
        process_mul_operator(node, file);
    }
    else if (op == OP_POW)
    {
        process_pow_operator(node, file);
    }
    else if (is_unary_op(op))
    {
        fprintf(file, "%s\\left(", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, "\\right)");
    }
    else
    {
        dump_node_to_tex(node->left, file);
        fprintf(file, " %s ", str_op);
        dump_node_to_tex(node->right, file);
    }
}

static void process_mul_operator(Node *node, FILE *file)
{
    assert(node);
    assert(file);

    // если ниже умножения сразу же лежит сложение вычитание то ставим скобки
    if (node->left->type == DT_OP && ((enum Operators)node->left->data == OP_ADD || (enum Operators)node->left->data == OP_SUB))
    {
        fprintf(file, "(");
        dump_node_to_tex(node->left, file);
        fprintf(file, ")");
    }
    else
        dump_node_to_tex(node->left, file);

    // решаем когда можно опустить знак умножения
    if (node->left->type == DT_CONST && node->right->type != DT_CONST)
        ;
    else if (node->left->type == DT_VAR && node->right->type == DT_VAR)
        ;
    else
        fprintf(file, " \\cdot ");

    // если ниже умножения сразу же лежит сложение вычитание то ставим скобки
    if ((node->right->type == DT_OP &&
         ((enum Operators)node->right->data == OP_ADD ||
          (enum Operators)node->right->data == OP_SUB)) ||
        (node->right->type == DT_CONST && node->right->data < 0))
    {
        fprintf(file, "(");
        dump_node_to_tex(node->right, file);
        fprintf(file, ")");
    }
    else
        dump_node_to_tex(node->right, file);
}

static void process_pow_operator(Node *node, FILE *file)
{
    assert(node);
    assert(file);

    fprintf(file, "{");
    if ((node->left->type == DT_CONST && node->left->data < 0) || node->left->type == DT_OP)
    {
        fprintf(file, "(");
        dump_node_to_tex(node->left, file);
        fprintf(file, ")");
    }
    else
    {
        dump_node_to_tex(node->left, file);
    }
    fprintf(file, "}^{");
    dump_node_to_tex(node->right, file);
    fprintf(file, "}");
}