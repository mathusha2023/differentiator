#ifndef DIFF_H
#define DIFF_H

enum Variables
{
    VAR_X,
    VAR_Y,
    VAR_T,
};

static const char *STR_VARIABLES[] = {
    "x",
    "y",
    "t",
};

enum Operators
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_LOG,
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
    "LOG",
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

typedef enum NodeDataType
{
    DT_NUM,
    DT_VAR,
    DT_OP,
} NodeDataType;

static const char *STR_NODE_DATA_TYPES[] = {
    "DT_NUM",
    "DT_VAR",
    "DT_OP",
};

const char *get_str_operator(enum Operators op);
const char *get_str_variable(enum Variables var);
const char *get_str_node_data_type(NodeDataType type);

#endif // DIFF_H
