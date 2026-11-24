#ifndef DIFF_H
#define DIFF_H

typedef struct Node Node;
typedef struct BinTree BinTree;
typedef double tree_el_t;

enum Variables
{
    VAR_X,
    VAR_Y,
    VAR_T,
    VAR_E,
    VAR_PI,
};

static const char *STR_VARIABLES[] = {
    "x",
    "y",
    "t",
    "e",
    "pi",
};

enum Operators
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_ROOT,
    OP_SQRT,
    OP_LN,
    OP_SIN,
    OP_COS,
    OP_TG,
    OP_CTG,
    OP_ARCSIN,
    OP_ARCCOS,
    OP_ARCTG,
    OP_ARCCTG,
    OP_SH,
    OP_CH,
    OP_TH,
    OP_CTH,
};

static const char *STR_OPERATORS[] = {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "POW",
    "ROOT",
    "SQRT",
    "LN",
    "SIN",
    "COS",
    "TG",
    "CTG",
    "ARCSIN",
    "ARCCOS",
    "ARCTG",
    "ARCCTG",
    "SH",
    "CH",
    "TH",
    "CTH",
};

static const char *STR_OPERATORS_TEX[] = {
    "+",                      // ADD
    "-",                      // SUB
    "\\cdot",                 // MUL
    "\\frac",                 // DIV
    "^",                      // POW
    "\\sqrt",                 // ROOT
    "\\sqrt",                 // SQRT
    "\\ln",                   // LOG
    "\\sin",                  // SIN
    "\\cos",                  // COS
    "\\tan",                  // TG
    "\\cot",                  // CTG
    "\\arcsin",               // ARCSIN
    "\\arccos",               // ARCCOS
    "\\arctan",               // ARCTG
    "\\operatorname{arccot}", // ARCCTG
    "\\sinh",                 // SH
    "\\cosh",                 // CH
    "\\tanh",                 // TH
    "\\coth"                  // CTH
};

typedef enum NodeDataType
{
    DT_CONST,
    DT_VAR,
    DT_OP,
} NodeDataType;

static const char *STR_NODE_DATA_TYPES[] = {
    "DT_CONST",
    "DT_VAR",
    "DT_OP",
};

BinTree diff_tree(BinTree *tree, enum Variables diff_var);
Node *diff(Node *node, enum Variables diff_var);
Node *diff_operator(Node *node, Node *node_left, Node *node_right, enum Operators op, enum Variables diff_var);
Node *fold_consts(Node *node);

tree_el_t execute_bin_op(enum Operators op, tree_el_t left, tree_el_t right);
int is_unary_op(enum Operators op);

const char *get_str_operator(enum Operators op);
const char *get_str_tex_operator(enum Operators op);
const char *get_str_variable(enum Variables var);
const char *get_str_node_data_type(NodeDataType type);

int is_equal(double a, double b);

#endif // DIFF_H
