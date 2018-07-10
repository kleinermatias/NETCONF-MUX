/**
 * @file info_recursos.c
 * @brief Permite mostrar en la correspondiente pagina web datos de los 
 		recursos del sistema embebido utilizado (Raspberry Pi 3 Model B).
 * @author Lopez, Gaston
 */

/* Importacion de librerias */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* Constantes */

#define MAX_BUFFER 200


/* Declaracion de funciones */

/*
 * @brief Funcion show_datos. Permite mostrar en la correspondiente pagina web
		datos (memoria, procesador, uptime y fecha y hora actual) de los recursos 
		del sistema embebido utilizado.
*/
void show_datos ();


/*
 * @brief Funcion show_info_memoria. Brinda informacion de los siguientes
 		parametros de la memoria (/proc/meminfo):

 		- MemTotal.
 		- MemFree.
 		- MemAvailable.
 		- Buffers.
 		- Cached.

*/
void show_info_memoria ();


/*
 * @brief Funcion show_info_procesador. Brinda informacion de los siguientes
 		parametros del procesador (/proc/cpuinfo):

 		- proccesor.	
		- model name.	
		- Features.	
		- CPU architecture.

*/
void show_info_procesador ();

/*
 * @brief Funcion show_info_uptime. Brinda informacion acerca de la
 		cantidad de tiempo que transcurrio desde que se prendio o encendio 
 		el sistema embebido. (Primer valor del archivo /proc/uptime).
*/
void show_info_uptime ();

/*
 * @brief Funcion show_info_hora_fecha_actual. Brinda informacion
 		sobre la hora y la fecha actual que posee el sistema embebido.
*/
void show_info_hora_fecha_actual ();



/* Funcion principal */
int main (int argc, char *argv[])
{
	
	/* Header de Mime */
	printf ("Content-type: text/html\n\n");
	
	/* Titulo */
	printf ("<html><head><title>TP3 Lopez Gaston</title></head><body>"
		"<center></br><h1>Trabajo Practico 3 de Sistemas Operativos II</h1>"
		"</br><h2>Informacion sobre los recursos del sistema embebido</h2>"
		"</br>");
	

	show_datos ();

	printf ("<a href=\"../index.lighttpd.html\"><button style=\"FONT-SIZE: 13pt\">"
		"<p>Volver a la Pagina Principal</button></a></p></center></body></html>");


	return 0;
}



/* Implementacion de funciones */

void show_datos()
{
	show_info_memoria ();
	show_info_procesador ();
	show_info_uptime ();
	show_info_hora_fecha_actual();
}



void show_info_memoria ()
{
	printf ("<b><h3>Memoria:</h3></b></br>");
	FILE *fichero;
	fichero = fopen ("/proc/meminfo","r");
	if (fichero == NULL)
	{
		printf ("</b>%s</br></br>", "Error en apertura del archivo.");
		exit (1);
	}
    else
    {
    	
    	char palabras_claves[5][50]= { "MemTotal", "MemFree", "MemAvailable", 
    								"Buffers", "Cached"};
    	
    	
    	
    	char *buffer = malloc (sizeof (char) * MAX_BUFFER);
		
		short contador= 0;
		
		while (!feof (fichero))
		{
			
			char *info_a_imprimir = fgets (buffer, MAX_BUFFER, fichero);
			if (info_a_imprimir != NULL)
			{
				if (!strncmp (buffer, palabras_claves[contador], 
					strlen (palabras_claves[contador]))){
					contador++;
					if (contador > 8) 
						contador = 0;
					
					char *info_campo = strtok (buffer, ":");
					info_a_imprimir = strtok (NULL, "\n");
					
					printf ("%s: %s </br></br>", info_campo, info_a_imprimir);
				}
			}
		}
		

	    int e = fclose (fichero);
		if ( e != 0) /* Error en cierre del archivo */
		{
			printf ("</b>%s</br></br>", "Error en cierre del archivo.");
			exit (1);
		}

		free (buffer);
		
	}
}


void show_info_procesador ()
{
	printf ("<b><h3>Procesador:</h3></b></br>");
	FILE *fichero;
	fichero = fopen ("/proc/cpuinfo","r");
	if (fichero == NULL)
	{
		printf ("</b>%s</br></br>", "Error en apertura del archivo.");
		exit (1);
	}
    else
    {
    	char palabras_claves[4][50]= {"processor", "model name", "Features", "CPU architecture"};
    	
    	
    	char *buffer = malloc (sizeof (char) * MAX_BUFFER);
		
		short contador= 0;
		
		while (!feof (fichero))
		{
			
			char *info_a_imprimir = fgets (buffer, MAX_BUFFER, fichero);
			if (info_a_imprimir != NULL)
			{
				if (!strncmp (buffer, palabras_claves[contador], 
					strlen (palabras_claves[contador]))){
					contador++;
					if (contador > 3)
						contador = 0;
					
					char *info_campo = strtok (buffer, ":");
					info_a_imprimir = strtok (NULL, "\n");
					
					printf ("%s: %s </br></br>", info_campo, info_a_imprimir);
				}
			}
		}
		

	    int e = fclose (fichero);
		if ( e != 0) /* Error en cierre del archivo */
		{
			printf ("</b>%s</br></br>", "Error en cierre del archivo.");
			exit (1);
		}

		free (buffer);
		
	}
}


void show_info_uptime ()
{
	printf ("<b><h3>Uptime:</h3></b></br>");
	FILE *fichero;
	fichero = fopen ("/proc/uptime","r");
	if (fichero == NULL)
	{
		printf ("</b>%s</br></br>", "Error en apertura del archivo.");
		exit (1);
	}
    else
    {
    	
		int horas, minutos, segundos;
		
		char cadena_uptime [100];
		long int tiempo_uptime;

		char caracter = 'a';
		char caracter_divisor = ' '; /* Divide los dos numeros del archivo */
		int contador = 0;
	
		while ((!feof (fichero)) && (caracter != caracter_divisor))
		{
	    	cadena_uptime[contador] = fgetc (fichero); 
	    	contador++;
	    }
	    cadena_uptime[contador] = '\0';

	    int e = fclose (fichero);
		if ( e != 0) /* Error en cierre del archivo */
		{
			printf ("</b>%s</br></br>", "Error en cierre del archivo.");
			exit (1);
		}

		/* Calculo del tiempo */
	    tiempo_uptime = atol (cadena_uptime); /* En segundos */
	 
    	horas = tiempo_uptime / 3600;
    	minutos=  (tiempo_uptime % 3600) / 60;
    	segundos= (tiempo_uptime % 3600) % 60;
    	

    	printf("%d horas. %d minutos. %d segundos. </br></br>",
    		horas,minutos,segundos);
		
	}
}


void show_info_hora_fecha_actual ()
{
	printf ("<b><h3>Hora y fecha actual:</h3></b></br>");
	
	/* Calculo de la hora/fecha actual. */
	time_t tiempo = time (NULL); /* Devuelve la fecha/hora actual. */
	if (tiempo < 0)
	{
		printf ("</b>%s</br></br>", "Error en obtencion de fecha y hora actual");

	}
	else
	{
		/* Representacion del tiempo en formato de calendario (fecha/hora). */
		struct tm *time_calendar = localtime (&tiempo);
  		
  		char hora_fecha_actual[50];

  		/* Otorgar formato a la estructura time_calendar segun la cadena pasada
  		como parametro. El resultado se almacena en hora_fecha_actual hasta un
  		limite de len(hora_fecha_actual) caracteres. */
  		strftime (hora_fecha_actual, 
  				50,
  				"%d/%m/%y %H:%M:%S", 
  				time_calendar);
  		
  		/* Significados: 
  			%d: día del mes (1-31)
			%m: nombre completo del mes.
  			%y: año (AA).
			%H: hora en formato de 24 horas.
			%M: minutos.
			%S: segundos. 
  		*/
  		printf ("%s</br></br>", hora_fecha_actual);	
	}

}
