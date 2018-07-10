/**
 * @file remove_file.c
 * @brief Permite eliminar el modulo del kernel que anteriormente
 		fue insertado a través de la página web. 
 		(Equivale a ejecutar el comando "sudo rmmod 
 		/var/www/html/Driver/nombre_modulo" en la Raspberry).
 		Directorio donde se encuentra el modulo a remover:
 		/var/www/html/Driver/
 * @author Lopez, Gaston
 */

/* Importacion de librerias */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* Constantes */

#define MAX_BUFFER 4096


/* Declaracion de funciones */


/*
 * @brief Funcion remover_modulos. Permite eliminar el modulo del kernel 
 		previamente insertado desde la pagina web. 
 		Ejecuta el comando "sudo rmmod /var/www/html/Driver/nombre_modulo" 
 		en la Raspberry.
*/
void remover_modulos ();





/* Funcion principal */
int main (int argc, char *argv[])
{
	
	/* Header de Mime */
	printf ("Content-type: text/html\n\n");
	
	/* Titulo */
	printf ("<html><head><title>TP3 Lopez Gaston</title></head><body>"
		"<center></br><h1>Trabajo Practico 3 de Sistemas Operativos II</h1>"
		"</br><h2>Operaciones sobre modulos</h2>"
		"</br>");
	

	remover_modulos ();

	printf ("<a href=\"../index.lighttpd.html\"><button style=\"FONT-SIZE: 13pt\">"
		"<p>Volver a la Pagina Principal</button></a></p></center></body></html>");

	return 0;
}



/* Implementacion de funciones */

void remover_modulos ()
{
	printf ("</b>Modulo removido:");

	/* Recibir nombre del modulo a remover */
	char *nombre_modulo = getenv ("QUERY_STRING");
	if (nombre_modulo == NULL)
	{
		printf ("</br>Error al intentar remover el modulo.</br></br>");
	}
	else 
	{
		char buffer [MAX_BUFFER];
		sscanf (nombre_modulo, "modulo=%19s", &buffer[0]); /* Conversion de formato */
		
		/* Genero comando */
		char *comando = malloc (sizeof (char) * MAX_BUFFER);
		strcpy (comando, "rmmod ");
		strcat (comando, "/var/www/html/Driver/");	
		strcat (comando, buffer);
		strcat (comando, ".ko"); /* En este punto la extension .ko se borro */
		//printf("%s\n",comando);

		system (comando); /* Ejecuto comando */
		printf ("</br>Modulo removido con exito.</br></br>");
		free (comando);
	}
}
