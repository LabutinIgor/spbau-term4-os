# spbau-term4-os

К заданию относятся file_system.c и file_system.h, в main.c добавились тесты.

В file_system.c находятся следующие функции:

setup_fs() -- инициализирует начальные значения для работы файловой системы

open(path, name) -- открывает файл с именем name, находящийся в пути path

read -- читает из файла в переданный буфер

write -- пишет в файл из переданного буфера

mkdir(path, name) -- создает директорию name, находящиююся в пути path

readdir(path) -- возвращает fs_node*, по которому можно восстановить все файлы директории, пройдя по указателям next_node
