# @file Makefile
# @brief Makefile para el Trabajo Practico 3 de Sistemas Operativos 2, versión 1 
# @author  Lopez, Gaston

# usar tabulador (no espacios) en make
# Flags de compilacion:
# -Wall mostrará todos los avisos que produzca el compilador, no solamente los errores. 
#       Los avisos indican dónde y/o porqué podría surgir algún error en el programa.
# -pedantic aporta más información sobre los errores y los avisos mostrados por GCC.
# -Werror tratara a cualquier warning como si un error se tratase. 
# -std=gnu99 se escribe para el correcto funcionamiento en la Raspberry.

all: info_recursos.cgi listar_modulos.cgi remove_file.cgi insertar_file.cgi baash.cgi

info_recursos.cgi: info_recursos.c
	gcc -Wall -Werror -pedantic -o info_recursos.cgi info_recursos.c -std=gnu99


listar_modulos.cgi:listar_modulos.c
	gcc -Wall -Werror -pedantic -o listar_modulos.cgi listar_modulos.c -std=gnu99

remove_file.cgi: remove_file.c
	gcc -Wall -Werror -pedantic -o remove_file.cgi remove_file.c -std=gnu99

insertar_file.cgi: insertar_file.c
	gcc -Wall -Werror -pedantic -o insertar_file.cgi insertar_file.c -std=gnu99

baash.cgi: baash.c
	gcc -Wall -Werror -pedantic -o baash.cgi baash.c -std=gnu99

clean:
	rm info_recursos.cgi listar_modulos.cgi remove_file.cgi insertar_file.cgi baash.cgi
