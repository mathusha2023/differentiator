#include "tree_io.h"
#include "bin_tree.h"
#include "config.h"
#include "log.h"
#include "diff.h"
#include "tex.h"

int main(void)
{
    restart_log();
    init_tex(TEX_FILE_NAME);

    BinTree tree = {};
    BinTree dtree = {};

    TreeIOError error = parse_tree_from_file_by_filename(&tree, DATA_FILE_NAME);
    if (error != TREE_IO_OK)
    {
        LOG("Error while reading tree from file '%s': %s", DATA_FILE_NAME, tree_io_get_str_error(error));
        return error;
    }

    dump_tree_to_tex(&tree, TEX_FILE_NAME);

    diff_tree(&tree, &dtree, VAR_X);

    dump_tree_to_tex(&dtree, TEX_FILE_NAME);

    tree_destroy(&tree);
    tree_destroy(&dtree);

    close_tex(TEX_FILE_NAME);
    generate_tex(TEX_FILE_NAME);

    close_log();

    return 0;
}