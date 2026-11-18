#include "tree_io.h"
#include "bin_tree.h"
#include "config.h"
#include "log.h"
#include "diff.h"

int main(void)
{
    restart_log();

    BinTree tree = {};

    TreeIOError error = parse_tree_from_file_by_filename(&tree, DATA_FILE_NAME);
    if (error != TREE_IO_OK)
    {
        LOG("Error while reading tree from file '%s': %s", DATA_FILE_NAME, tree_io_get_str_error(error));
        return error;
    }

    // tree_insert_node(&tree, NULL, ROOT_CHILD, DT_OP, 2);
    // tree_insert_node(&tree, tree.root, LEFT_CHILD, DT_VAR, 0);
    // tree_insert_node(&tree, tree.root, RIGHT_CHILD, DT_NUM, -13.291);

    // tree_print(&tree);

    tree_destroy(&tree);

    close_log();

    return 0;
}