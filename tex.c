#include "tex.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include "bin_tree.h"

static void process_mul_operator(Node *node, FILE *file);
static void process_pow_operator(Node *node, FILE *file);

void init_tex(const char *tex_filename)
{
    assert(tex_filename);

    FILE *file = fopen(tex_filename, "w");
    assert(file);

    fprintf(file, "\\documentclass{article}\n\\usepackage{amsmath}\n");
    fprintf(file, "\\usepackage[utf8]{inputenc}\n\\usepackage[T2A]{fontenc}\n\\usepackage[russian]{babel}\n");
    fprintf(file, "\\usepackage[a%upaper, hmargin=1cm, vmargin=2cm]{geometry}\n\\usepackage{mdframed}\n\\usepackage{breqn}\n", PDF_LIST_FORMAT);
    fprintf(file, "\\setlength{\\parindent}{0pt}\n");
    fprintf(file, "\\mdfsetup{\nlinewidth=1pt,\ninnertopmargin=6pt,\ninnerbottommargin=6pt,\ninnerleftmargin=8pt,\ninnerrightmargin=8pt\n}\n\n");

    fprintf(file, "\\begin{document}\n\n");

    fclose(file);
}

void close_tex(const char *tex_filename)
{
    assert(tex_filename);

    FILE *file = fopen(tex_filename, "a");
    assert(file);

    fprintf(file, "\n\n\\end{document}");

    fclose(file);
}

void generate_tex_pdf(const char *tex_filename)
{
    assert(tex_filename);

    char buff[MAX_SYS_COMMAND_LENGTH] = {};
    FLOG("Beginning .pdf generating...");
    FLOG("TEX output:");

    const char *const command = "pdflatex -interaction=nonstopmode -halt-on-error %s >> %s &";
    if (snprintf(buff, MAX_SYS_COMMAND_LENGTH, command, tex_filename, LOGFILE_NAME) < 0)
    {
        LOG("Error while generating .pdf from %s file", tex_filename);
        return;
    }

    system(buff);

    FLOG("Generating ended!");
}

int write_text_to_tex(const char *tex_filename, const char *const text, ...)
{
    assert(tex_filename);
    assert(text);

    FILE *file = fopen(tex_filename, "a");
    if (!file)
    {
        FLOG("Error: can not open tex file '%s'", tex_filename);
        return 1;
    }

    va_list args = {};
    va_start(args, text);

    vfprintf(file, text, args);

    va_end(args);
    fclose(file);

    return 0;
}

// extra - поле для дополнительной информации. например, через него передается порядок производной
// при распечатке как производной n-го порядка
int dump_tree_to_tex(BinTree *tree, const char *tex_filename, enum TexMode tex_mode, enum Variables diff_var, int extra, int boxed)
{
    assert(tree);
    assert(tex_filename);

    FILE *file = fopen(tex_filename, "a");
    if (!file)
    {
        FLOG("Error: can not open tex file '%s'", tex_filename);
        return 1;
    }

    if (boxed)
        fprintf(file, "\\begin{mdframed}\n");

    fprintf(file, "\\begin{dmath*}\n");

    switch (tex_mode)
    {
    case TM_NO_FUNCTION:
        fprintf(file, "({");
        break;
    case TM_FUNCTION:
        fprintf(file, "f \\hiderel{=} ");
        break;
    case TM_DERIVATIVE:
        fprintf(file, "f'_{%s} \\hiderel{=} ", get_str_variable(diff_var));
        break;
    case TM_N_DERIVATIVE:
        fprintf(file, "f^{(%d)}_{%s} \\hiderel{=} ", extra, get_str_variable(diff_var));
        break;
    default:
        LOG("ERROR: incorrect TexMode");
        return 1;
    }

    if (tree->root)
        dump_node_to_tex(tree->root, file);
    else
    {
        LOG("WARNING: tree will be written in file '%s' is void", tex_filename);
        fprintf(file, "0");
    }

    if (tex_mode == TM_NO_FUNCTION)
        fprintf(file, "})'");

    fprintf(file, "\n\\end{dmath*}\n");

    if (boxed)
        fprintf(file, "\n\\end{mdframed}\n");

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
    else if (op == OP_ROOT)
    {
        fprintf(file, "%s[", str_op);
        dump_node_to_tex(node->left, file);
        fprintf(file, "]{");
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
    else if (op == OP_SQRT)
    {
        fprintf(file, "%s{", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, "}");
    }
    else if (is_unary_op(op))
    {
        fprintf(file, "%s(", str_op);
        dump_node_to_tex(node->right, file);
        fprintf(file, ")");
    }
    else
    {
        dump_node_to_tex(node->left, file);
        fprintf(file, " %s ", str_op);
        if (node->right->type == DT_CONST && node->right->data < 0)
        {
            fprintf(file, "(");
            dump_node_to_tex(node->right, file);
            fprintf(file, ")");
        }
        else
            dump_node_to_tex(node->right, file);
    }
}

int dump_equal_nodes_to_tex(Node *node1, Node *node2, const char *tex_filename)
{
    assert(node1);
    assert(node2);
    assert(tex_filename);

    FILE *file = fopen(tex_filename, "a");
    if (!file)
    {
        FLOG("Error: can not open tex file '%s'", tex_filename);
        return 1;
    }

    fprintf(file, "\\begin{dmath*}\n");

    fprintf(file, "{(");
    dump_node_to_tex(node1, file);
    fprintf(file, ")}'");

    fprintf(file, " = ");
    dump_node_to_tex(node2, file);

    fprintf(file, "\n\\end{dmath*}\n");

    fclose(file);
    return 0;
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
    else if (node->left->type == DT_CONST && is_equal(node->left->data, -1))
        fprintf(file, "-");
    else
        dump_node_to_tex(node->left, file);

    // решаем когда можно опустить знак умножения
    if (node->left->type == DT_CONST && node->right->type == DT_VAR)
        ;
    else if (node->left->type == DT_VAR && node->right->type == DT_VAR)
        ;
    else if (node->left->type == DT_CONST && is_equal(node->left->data, -1))
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