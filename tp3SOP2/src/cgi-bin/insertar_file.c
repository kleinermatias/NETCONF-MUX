/**
 * @file insertar_file.c
 * @brief Permite insertar un modulo en el kernel del sistema embebido, 
 		previamente subido desde el cliente hacia el servidor. 
 		(Equivale a ejecutar el comando "sudo insmod	
 		/var/www/html/Driver/nombre_modulo" en la Raspberry).
 		Directorio donde se encuentra el modulo a insertar:
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
 * @brief Funcion insertar_modulos. Permite insertar un modulo del kernel.
 		Ejecuta el comando "insmod /var/www/html/Driver/nombre_modulo".
*/
void insertar_modulos ();


/*
 * @brief Funcion verificar_extension. Verifica que el nombre del archivo que
 		se desea cargar como modulo tenga la extension ".ko".
 * @param  nombre_modulo (char *) Nombre del archivo que se desea cargar como
 		modulo.
 * @return 0 en caso de extension valida. -1 en caso contrario.
*/
int verificar_extension (char *nombre_modulo);





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
	

	insertar_modulos ();

	printf ("<a href=\"../index.lighttpd.html\"><button style=\"FONT-SIZE: 13pt\">"
		"<p>Volver a la Pagina Principal</button></a></p></center></body></html>");

	return 0;
}



/* Implementacion de funciones */

void insertar_modulos ()
{
	printf ("</b>Insercion de modulo:");

	/* Recibir nombre de modulo a insertar */
	char *info_formulario = getenv ("QUERY_STRING");
	if (info_formulario == NULL)
	{
		printf ("</br>Error en la insercion del modulo.</br></br>");
	}
	else 
	{
		char buffer [MAX_BUFFER];
		sscanf (info_formulario, "modulo=%19s", &buffer[0]); /* Conversion de formato */
		if (!verificar_extension (buffer)) /* Archivo con extension .ko*/
		{

			/* Genero comando */
			char *comando = malloc (sizeof (char) * MAX_BUFFER);
			strcpy (comando, "insmod ");
			strcat (comando, "/var/www/html/Driver/");
			strcat (comando, buffer);
			strcat (comando, ".ko"); /* En este punto la extension .ko se borro */

			system (comando); /* Ejecuto comando */
			//printf ("%s", comando);
			printf ("</br>Modulo insertado con exito.</br></br>");
			printf ("</br><h3>Eliminar modulo insertado:</h3></br>");
			
			/* Formulario para remover el modulo insertado */
			printf ("<form action=\"/cgi-bin/remove_file.cgi\" method=\"get\"><input name=\"modulo\""
				"hidden value=\"%s\"><button style=\"FONT-SIZE: 13pt\">Eliminar modulo insertado</button></form></br></br>", buffer);

			free (comando);

		}
		else /* Archivo con error de extension */
		{
			printf ("</br>Archivo incorrecto. Debe poseer extension \".ko\".</br></br>");
		}
	}
}


int verificar_extension (char *nombre_modulo)
{
	char *buf = strtok (nombre_modulo, ".");

	if (strcmp(buf, ",")); /* Linea colocada por warning de cppcheck. 
						      Variable 'buf' is reassigned a value before 
						      the old one has been used. */
	buf = strtok (NULL, "\n"); /* Almaceno extension */
	if (! strncmp (buf, "ko", 2)) /* Extension valida (.ko) */
		return 0;
	return -1;
}
