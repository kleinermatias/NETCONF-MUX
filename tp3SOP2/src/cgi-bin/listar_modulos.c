/**
 * @file listar_modulos.c
 * @brief Permite mostrar los módulos cargados en el kernel del sistema
 		operativo del sistema embebido. (Equivale a ejecutar el 
 		comando sudo lsmod en la Raspberry).
 * @author Lopez, Gaston
 */

/* Importacion de librerias */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Constantes */

#define MAX_BUFFER 4096


/* Declaracion de funciones */



/*
 * @brief Funcion listar_modulos. Brinda en la página web
 		la lista de módulos que se encuentran cargados en el kernel.
 		Equivale a ejecutar el comando "sudo lsmod". Algunas partes de
 		esta función son una adaptación del código que se encuentra en el 
 		repositorio de GitHub:
 		https://github.com/vadmium/module-init-tools/blob/master/lsmod.c
*/
void listar_modulos ();





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
	

	listar_modulos ();

	printf ("<a href=\"../index.lighttpd.html\"><button style=\"FONT-SIZE: 13pt\">"
		"<p>Volver a la Pagina Principal</button></a></p></center></body></html>");

	return 0;
}



/* Implementacion de funciones */

void listar_modulos ()
{
	printf ("</b><h3>Listado de modulos:</h3></b></b>");
	FILE *fichero;
	fichero = fopen ("/proc/modules","r");
	if (fichero == NULL)
	{
		printf ("</b>%s</br></br>", "Error en apertura del archivo.");
		exit (1);
	}
    else
    {
    	
    	printf("<table border=\"1\" style=\"margin: 0 auto;\">");
    	printf("<tr><td></b><h4>Module</h4></b></td> <td></b><h4>Size</h4></b></td> <td></b><h4>Used by</h4></b></td></b></tr>");
    	fflush (stdout);
    	char *buffer = malloc (sizeof (char) * MAX_BUFFER);
		
		
		while (!feof (fichero)){
			
			char *info_a_imprimir = fgets (buffer, MAX_BUFFER, fichero);
			
			if (info_a_imprimir != NULL)
			{
				info_a_imprimir = strtok (buffer, " \t"); /* Nombre modulo */
				printf ("<tr><td>%-19s</td>", info_a_imprimir); 
				/* El %- es por alineación a la izquierda */
				info_a_imprimir = strtok (NULL, " \t\n"); /* Size modulo */
				printf ("<td>%8s</td>", info_a_imprimir);
				info_a_imprimir = strtok (NULL, " \t\n");
				if (info_a_imprimir != NULL ) /* Puede no ser usado por nadie */
				{
					printf ("<td> %2s", info_a_imprimir); /* Es usado */
					info_a_imprimir = strtok (NULL, "\n");
					/* A continuacion se verifica si lo que sigue interesa o no, de acuerdo a
					la salida original del comando sudo lsmod. */
					/* Que aparezca un guion - implica que lo que sigue a continuacion
					no debe figurar en la salida. La 'L' es de la palabra Live 
					(no debe figurar). */
					if ((info_a_imprimir == NULL) || 
						((info_a_imprimir[0] == '-') &&
						 ((info_a_imprimir[1] == '\0') || (info_a_imprimir[2]=='L'))))
					{
						info_a_imprimir = "";
					}
					/* Que aparezca una coma ',' significa que lo que sigue si
					son campos importantes que debe figurar en la salida */
					else if (strchr (info_a_imprimir, ',')) /* Deteccion de coma */
					{
						info_a_imprimir = strtok (info_a_imprimir, "\t ");
						/* Termina con una coma y arrancan campos sin importancia
						para la salida de la pagina web. */
						if (info_a_imprimir[ strlen (info_a_imprimir) - 1] == ',')
							info_a_imprimir[ strlen (info_a_imprimir) - 1] = '\0';

						printf (" %s</td></tr>", info_a_imprimir);
					}
					
				}	
			}
		}
		
		free (buffer);
	    int e = fclose (fichero);
		if ( e != 0) /* Error en cierre del archivo */
		{
			printf ("</b>%s</br></br>", "Error en cierre del archivo.");
			exit (1);
		}

		
		printf ("</table><p>");

		
	}

}
