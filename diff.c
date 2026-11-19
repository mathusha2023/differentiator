#include "diff.h"
#include "bin_tree.h"
#include "my_string.h"
#include "tree_io.h"

int is_unary_op(enum Operators op)
{
    return (int)op >= 5;
}

const char *get_str_operator(enum Operators op)
{
    return STR_OPERATORS[op];
}

const char *get_str_tex_operator(enum Operators op)
{
    return STR_OPERATORS_TEX[op];
}

const char *get_str_variable(enum Variables var)
{
    return STR_VARIABLES[var];
}

const char *get_str_node_data_type(NodeDataType type)
{
    return STR_NODE_DATA_TYPES[type];
}
