#ifndef GRAPHVIZ_H
#define GRAPHVIZ_H

#include "bin_tree.h"

static const char *CONST_FILL_COLOR = "#E3F2FD";
static const char *CONST_COLOR = "#1E88E5";

static const char *VAR_FILL_COLOR = "#f3fde3";
static const char *VAR_COLOR = "#39e51e";

static const char *OP_FILL_COLOR = "#fce3fd";
static const char *OP_COLOR = "#c71ee5";

static const char *ERROR_FILL_COLOR = "#fde3e3";
static const char *ERROR_COLOR = "#e51e1e";

static const char *NODE_FONT_COLOR = "#011a31";

const int NODE_FONT_SIZE = 16;
const int ARROW_HINT_FONT_SIZE = 14;

static const char *LEFT_EDGE_COLOR = "#c87d2c";
static const char *RIGHT_EDGE_COLOR = "#15cac7";

int write_graph(BinTree *tree, const char *filename);
int generate_graph_file(BinTree *tree, const char *left_label, const char *right_label);
int generate_debug_graph_file(BinTree *tree);

#endif // GRAPHVIZ_H
