# spbau-term4-os

К заданию относятся memory_map.h и memory_map.c, остальное скопировано из https://github.com/krinkinmu/os-course/tree/master/sol
В memory_map.h находится описание необходимых структур, взятые из https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
В memory_map.c находятся следующие функции: 
validate_map() -- проверяет корректность mboot_info;
get_map() -- сохраняет карту памяти в entries и добавляет к карте память ядра;
print_map() -- выводит карту памяти;
