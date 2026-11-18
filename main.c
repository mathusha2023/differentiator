#include "tree_io.h"
#include "bin_tree.h"
#include "config.h"
#include "log.h"

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

    close_log();

    return 0;
}