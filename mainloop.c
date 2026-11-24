#include "mainloop.h"
#include "log.h"
#include "bin_tree.h"
#include "tree_io.h"
#include "tex.h"

static void print_greeting(void);
static enum Commands get_command(void);
static enum Variables get_variable(void);
static int get_number(void);
static int diff_func(BinTree *tree, const char *tex_file_name);
static int diff_n_func(BinTree *tree, const char *tex_file_name);
static void flush_console(void);
static int is_correct_cmd(int cmd);
static int is_correct_var(char *str_var);

#define mainloop_check_error(err_msg, ...) \
    {                                      \
        if (error)                         \
        {                                  \
            LOG(err_msg, ##__VA_ARGS__);   \
            return error;                  \
        }                                  \
    }

int mainloop(const char *data_file_name, const char *tex_file_name)
{
    assert(data_file_name);
    assert(tex_file_name);

    BinTree tree = {};
    {
        TreeIOError error = parse_tree_from_file_by_filename(&tree, data_file_name);
        if (error != TREE_IO_OK)
        {
            LOG("Error while reading tree from file '%s': %s", data_file_name, tree_io_get_str_error(error));
            return error;
        }
    }

    print_greeting();
    enum Commands cmd = get_command();
    int res = 0;

    switch (cmd)
    {
    case CMD_EXIT:
        res = 0;
        break;
    case CMD_DIFF:
        res = diff_func(&tree, tex_file_name);
        break;
    case CMD_N_DIFF:
        res = diff_n_func(&tree, tex_file_name);
        break;
    case CMD_TAYLOR:
        res = 0;
        break;
    case CMD_MAKLOREN:
        res = 0;
        break;
    default:
        LOG("Все пошло по пизде, такой команды вообще быть не может!");
        res = 1;
    }

    tree_destroy(&tree);

    return res;
}

static void print_greeting(void)
{
    printf("Выберите режим использования:\n");
    printf("[1]. Найти первую производную функции\n");
    printf("[2]. Найти n-ю производную функции\n");
    printf("[3]. Разложить функцию по формуле Тейлора\n");
    printf("[4]. Разложить функцию по формуле Маклорена (Тейлор в нуле)\n");
    printf("[0]. Выйти\n");
}

static int diff_func(BinTree *tree, const char *tex_file_name)
{
    assert(tree);
    assert(tex_file_name);

    enum Variables diff_var = get_variable();

    init_tex(tex_file_name);

    int error = 0;

    error = write_text_to_tex(tex_file_name, "\\textbf{Задание:} найти производную функции\n");
    mainloop_check_error("Error while writing text to tex");

    error = dump_tree_to_tex(tree, tex_file_name, TM_FUNCTION, VAR_X, 0, 0);
    mainloop_check_error("Error while dumping function tree to tex");

    error = write_text_to_tex(tex_file_name, "\nпо переменной $%s$.\n", get_str_variable(diff_var));
    mainloop_check_error("Error while writing text to tex");

    error = write_text_to_tex(tex_file_name, "\n\\begin{center}\n{\\Large\\textbf{Решение:}}\n\\end{center}\n\n");
    mainloop_check_error("Error while writing text to tex");

    BinTree dtree = diff_tree(tree, diff_var);

    error = write_text_to_tex(tex_file_name, "\n\\begin{center}\n{\\Large\\textbf{Ответ:}}\n\\end{center}\n\n");
    mainloop_check_error("Error while writing text to tex");

    dump_tree_to_tex(&dtree, tex_file_name, TM_DERIVATIVE, diff_var, 1, 1);

    tree_destroy(&dtree);

    close_tex(tex_file_name);
    generate_tex_pdf(tex_file_name);

    printf("Готово! Результат записан в %s, также был сгенерирован .pdf файл с аналогичным названием\n", tex_file_name);

    return 0;
}

static int diff_n_func(BinTree *tree, const char *tex_file_name)
{
    assert(tree);
    assert(tex_file_name);

    enum Variables diff_var = get_variable();

    printf("Введите порядок производной:\n");
    int n = get_number();

    init_tex(tex_file_name);

    int error = 0;

    error = write_text_to_tex(tex_file_name, "\\textbf{Задание:} найти %d-ю производную функции\n", n);
    mainloop_check_error("Error while writing text to tex");

    error = dump_tree_to_tex(tree, tex_file_name, TM_FUNCTION, VAR_X, 0, 0);
    mainloop_check_error("Error while dumping function tree to tex");

    error = write_text_to_tex(tex_file_name, "\nпо переменной $%s$.\n", get_str_variable(diff_var));
    mainloop_check_error("Error while writing text to tex");

    error = write_text_to_tex(tex_file_name, "\n\\begin{center}\n{\\Large\\textbf{Решение:}}\n\\end{center}\n\n");
    mainloop_check_error("Error while writing text to tex");

    if (n == 0)
    {
        write_text_to_tex(tex_file_name, "0-я производная функции - это сама эта функция, поэтому сразу запишем ответ\n");
        error = write_text_to_tex(tex_file_name, "\n\\begin{center}\n{\\Large\\textbf{Ответ:}}\n\\end{center}\n\n");
        mainloop_check_error("Error while writing text to tex");

        dump_tree_to_tex(tree, tex_file_name, TM_N_DERIVATIVE, diff_var, 0, 1);
    }
    else
    {
        BinTree dtree = *tree;
        for (int i = 1; i <= n; i++)
        {
            write_text_to_tex(tex_file_name, "\\textbf{%d. Вычислим %d-ю производную исходной функции}\n", i, i);
            mainloop_check_error("Error while writing text to tex");
            error = dump_tree_to_tex(tree, tex_file_name, TM_N_DERIVATIVE, VAR_X, i, 0);
            mainloop_check_error("Error while dumping function tree to tex");
            dtree = diff_tree(&dtree, diff_var);
        }

        error = write_text_to_tex(tex_file_name, "\n\\begin{center}\n{\\Large\\textbf{Ответ:}}\n\\end{center}\n\n");
        mainloop_check_error("Error while writing text to tex");

        dump_tree_to_tex(&dtree, tex_file_name, TM_N_DERIVATIVE, diff_var, n, 1);

        tree_destroy(&dtree);
    }

    close_tex(tex_file_name);
    generate_tex_pdf(tex_file_name);

    printf("Готово! Результат записан в %s, также был сгенерирован .pdf файл с аналогичным названием\n", tex_file_name);

    return 0;
}

static enum Commands get_command(void)
{
    printf("> ");
    int c = getchar();
    flush_console();

    while (!is_correct_cmd(c - '0'))
    {
        printf("Некорректный ввод! Выберите один из приведенных вариантов!\n");
        printf("> ");
        c = getchar();
        flush_console();
    }

    return (enum Commands)(c - '0');
}

static enum Variables get_variable(void)
{
    printf("Выберите переменную для дифференцирования:\n");
    for (size_t i = 0; i < sizeof(STR_VARIABLES) / sizeof(STR_VARIABLES[1000 - 7]) - 2; i++)
    {
        printf("[%s] ", get_str_variable((enum Variables)i));
    }
    printf("\n");
    printf("> ");

    char buff[MAX_NODE_DATA_LENGTH] = {};
    scanf("%s", buff);
    flush_console();

    int var = is_correct_var(buff);

    while (var < 0)
    {
        printf("Некорректный ввод! Выберите одну из допустимых переменных!\n");
        printf("> ");
        scanf("%s", buff);
        flush_console();
        var = is_correct_var(buff);
    }

    return (enum Variables)var;
}

static int get_number(void)
{
    printf("> ");
    int num = 0;
    int res = scanf("%d", &num);
    flush_console();

    while (res != 1 || num < 0)
    {
        printf("Некорректный ввод! Должно быть неотрицательное число!\n");
        printf("> ");
        res = scanf("%d", &num);
        flush_console();
    }

    return num;
}

static void flush_console(void)
{
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static int is_correct_cmd(int cmd)
{
    // int i специально для более простого сравнения
    for (int i = 0; i < (int)CMD_LIST_LENGTH; i++)
    {
        if (cmd == i)
            return 1;
    }
    return 0;
}

// вернет номер переменной если она корректна и -1 в случае некорректного значения
static int is_correct_var(char *str_var)
{
    for (size_t i = 0; i < sizeof(STR_VARIABLES) / sizeof(STR_VARIABLES[1000 - 7]) - 2; i++)
    {
        if (!nonregister_strcmp(str_var, STR_VARIABLES[i]))
            return (int)i;
    }
    return -1;
}
