/**
* @file baash.c
* @brief Bash desarrollado en Sistemas Operativos I y con modificaciones 
	efectuadas para el TP3 de Sistemas Operativos II.
* @author Lopez, Gaston
*/


/* No todo el bash va a contener el GNU code style. 
Solamente la parte agregada para Sistemas Operativos II. */



/* Importación de liberias. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>    
#include <errno.h>


/* Funciones presentes en el codigo. (Sist. Op I). */
void getPaths(char* paths []);
int leerTeclado(char* argv[], char* cadena);
void buscarArchivo(char* arch, char* paths[], char* execPath);
int background(char* argv[], int argc);
int checkRedirect(char* argv[], char fileName[]);
int checkPipe(char* argv[], char* argv1[], char* argv2[]);
void outPut(char fileName[]);
void inPut(char fileName[]);
void doPipeline(char* argv1[], char* argv2[], char* paths[]);
char* concat ( const char *s1, const char *s2);
char *acondicionarHome ( char directorioDeTrabajo[] );

/* Impresion en la consola adaptadas para el TP3. (Sist. Op II). */
#define SEPARADOR "<p>"      
#define LOG_BLUE(X) fprintf(stdout, "%s ", X) 
#define LOG_RED(X,Z) fprintf(stdout, "%s%s@%s" , SEPARADOR , X , Z) 


/* Definicion de constantes y variables globales. (Sist. Op II). */

#define TAM 256

/* No se puede utilizar getenv en la Raspberry. Devuelve NULL cuando se 
	desea acceder a la variable de entorno PATH. (Sist. Op II). */
char pathVar[138] = "/home/raspi/bin:/home/raspi/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin";




/* Sistemas Operativos I */


/**
* Obtiene los Paths de la variable global pathVar, y almacena uno por cada elemento del array paths.
* @param paths Arreglo de punteros a caracter donde se almacena cada path.
*/
void getPaths( char* paths[] ) {
	int pathCounter;
	paths[0] = strtok( pathVar , ":" );
	for( pathCounter = 1 ; pathCounter < 20 ; pathCounter++ ) {
		paths[pathCounter] = strtok ( NULL , ":" );    
		if ( paths[pathCounter] == NULL ) {
			break;
		}
	}
	strtok ( NULL , ":" );
}


/**
 * Se encarga de parsear el comando ingresado como un conjunto de argumentos. Se usa la funcion strtok para separar la cadena por espacios.
 * @param argv Arreglo donde se colocarán los argumentos hallados.
 * @param cadena Comando ingresado.
 * @return int Numero de argumentos.
 */
int parse_Command ( char* argv[] , char* cadena ) {
	int argscounter = 0;
	argv[0] = strtok( cadena , " \n" );
	for ( argscounter = 1 ; argscounter < 150 ; argscounter++ ) {
		argv[argscounter] = strtok ( NULL , " \n" ); //Para la separación se usa el salto de línea.
		if ( argv[argscounter] == NULL )
			break;
		else if (argscounter>=20)
		{
			fprintf(stdout,"%s\n", "Exceso de argumentos. Maximo: 20");
			exit (1);
		}
	}
	return argscounter; //Cantidad de argumentos. (argc).
}




/**
* Gestor de operaciones del comando interno cd.
* @param argumento Puntero al directorio al cual se quiere ir.
* @return void
*/
void cdBuiltin ( char* argumento ) {
	int changeDir;
	changeDir = chdir ( argumento ); //Cambio de directorio.
	if( changeDir != 0 ) { //Manejo de errores del cambio de directorio.
		fprintf (stdout, "Error al ir al directorio:" );
		fprintf (stdout, " %s\n", argumento);
	}

}


/**
* Lee la cadena almacenada en 'arch' (quien seria el argumento 0), busca el ejecutable, y muestra el directorio donde se encuentra
* @param arch Es el argumento del cual se quiere averiguar su ruta de ejecucion
* @param paths Arreglo con todos los directorios del PATH de linux
* @param execPath Es el directorio completo al ejecutable que se encontro. Si no se encuentra el archivo, se pone 'X' en execPath[0] para indicar su error
* o 'D' en su defecto para indicar que es un directorio
*/
void buscarArchivo ( char* arch , char* paths[] , char* execPath ) {
	
	int result;
	char searchDir[150] = "";
	char* archivo;
	
	int flagPuntosBarras=1;
	int flagTresPuntos=0;
	int contTresPuntos=0;
	int indiceAnteriorTresPuntos=0;
	for ( int i = 0 ; i < strlen ( arch ) ; i++ ) {	    //serie de comparaciones para saber si el argumento ingresado es un directorio, en ese caso no hacer nada.
		if ( ( arch[i] ) == '.' ) {	
			if( i - indiceAnteriorTresPuntos == 1 ) {
				contTresPuntos++;
			}
			indiceAnteriorTresPuntos=i;
			if ( contTresPuntos == 2 ) {
				contTresPuntos=0;
				flagTresPuntos=1;
			}
		}
		else {
			indiceAnteriorTresPuntos=0;
			contTresPuntos=0;
			}
		if( arch[i]!='/' && arch[i]!='.' ) {
			flagPuntosBarras=0;
		}
			
	}
	
	if ( flagPuntosBarras && !flagTresPuntos ) {
		execPath[0] = 'D';
		return;
	}
	if ( flagTresPuntos ) {
		execPath[0] = 'X';
		return;
	}
	if ( arch[0] == '/' || ( arch[0] == '.' && arch[1] == '.' && arch[2] == '/' ) ) {
		char* dir;
		char* nextDir;
		int pathReady = 0; //Bandera que controla si ya se obtuvo el path completo
		if ( arch[0] == '/' )
			searchDir[0] = '/';
		dir = strtok ( arch , "/" );
		nextDir = strtok ( NULL , "/" );
		if ( nextDir != NULL ) //Si el comando no es de la forma /archivo
			strcat( searchDir , dir );
		else {
			nextDir = dir; //Si archivo si es NULL, entonces pongo en archivo lo que hay despues de la ultima /
			pathReady = 1;
		}
		while ( (nextDir != NULL) && !pathReady ) {
			dir = nextDir;
			nextDir = strtok ( NULL , "/" );
			strcat ( searchDir , "/" );
			if ( nextDir != NULL )
				strcat ( searchDir , dir );
		}
		archivo = dir;
	}
	
	else if ( arch[0] == '.' && arch[1] == '/' ) { //Es un path relativo, tomando como path el directorio actual.
		getcwd ( searchDir , 150 );
		if(searchDir==NULL){
			fprintf (stdout, "Error en obtener directorio actual.");
			exit(1);
		}
		else{
			strcat ( searchDir , "/" );
			const char ch = '/';
   			archivo = strchr(arch, ch);
   		}

	}

	else { //Tiene que buscar en todos los directorios del path.
		int i;
		char aux[150];
		for ( i = 0 ; i < 20 ; i++ ) {
			if(paths[i] == NULL)
				break;
			strcpy ( aux , paths[i] );
			strcat ( aux , "/" );
			strcat ( aux , arch );

			
			result = access ( aux , X_OK );

			if ( !result ) {
				strcpy ( execPath , aux );
				return;
			}

		}
		execPath[0] = 'X';
		
		return;
	}

	strcat ( searchDir , archivo );

	result = access ( searchDir , X_OK );
	if( !result ){
		strcpy ( execPath , searchDir );
		execPath[0] = 'D';
		
		return;
	}
	else
		execPath[0] = 'X';

	
	
}


/**
* Verifica si se quiere lanzar el proceso en background ante la presencia de un caracter &.
* @param argv Vector de argumentos que pasa el usuario. Se necesita para verificar si el ultimo es un &.
* @return Devuelve 0 si el proceso no debe ejecutarse en background, y 1 en caso de hacerlo porque el ultimo argumento es & y 2 si es argumento&.
*/
int background ( char* argv[] , int argc ) {
	int i;
	const char ch = '&'; //Para caso especial por ejemplo:  'ls&'.
	char *resto;
	resto = strrchr(argv[argc-1], ch);
	for ( i = 0 ; i < 20 ; i++ ) {
		if ( argv[i] == NULL )
			break;
	}
	if( !strcmp ( argv[i-1] , "&" ) ) {
		return 1;
	}

	if ( resto!=NULL ) {
		char *token;   
		token = strtok( argv[argc-1] , "&\n" );
		argv[argc-1] = token;
		return 2;
	}
	return 0;
}


/**
* Verifica si se debe redireccionar la entrada o la salida estandar.
* @param argv Arreglo que contiene el comando y los argumentos ingresados.
* @param fileName Almacena el nombre del archivo del que se lee, o en el que se escribe la salida.
* @return Devuelve 0 si no hay que redireccionar, 1 si hay que redireccionar la entrada, y 2 si hay que redireccionar la salida.
*/
int checkRedirect ( char* argv[] , char fileName[] ) {
	int i;
	for ( i = 0 ; i < 20 ; i++ ) {
		if ( argv[i] == NULL ) {
			fileName = NULL;
			return 0;
		}
		else if ( !strcmp ( argv[i] , "<" ) ) {
			strcpy ( fileName , argv[i+1] );
			argv[i] = NULL;   
			argv[i+1] = NULL;
			return 1;
		}
		else if (!strcmp(argv[i], ">")){
			strcpy ( fileName , argv[i+1] );
			argv[i] = NULL; 
			argv[i+1] = NULL;
			return 2;
		}
	}
	return 0;
}


/**
* Modifica la salida estándar.
* @param fileName es la direccion al archivo donde se envía la salida.
*/
void outPut ( char fileName[] ) {
	int fid;
	int flags,perm;
	flags = O_WRONLY|O_CREAT|O_TRUNC;
	perm = S_IWUSR|S_IRUSR;
	fid = open ( fileName , flags , perm );  
	if ( fid <0 ) {
		fprintf (stdout, "Error en apertura de archivo." );
		exit ( 1 );
	}
	int valor=close( STDOUT_FILENO );
	if(valor<0){
		fprintf (stdout,"Error en cierre de archivo.");
		exit(1);
	}
	if ( dup ( fid ) < 0 ) {
		fprintf (stdout,"Error en duplicacion de ficheros.");
		exit(1);
	}
	valor=close(fid);
	if(valor<0){
		fprintf (stdout, "Error en cierre de archivo.");
		exit(1);
	}

}

/**
* Modifica la entrada estándar.
* @param fileName es la direccion de la cual se van a tomar los datos de ese archivo..
*/
void inPut ( char fileName[] ) {
	int fid;
	int flags,perm;
	flags = O_RDONLY;
	perm = S_IWUSR|S_IRUSR;
	int valor=close ( STDIN_FILENO );  //cierro entrada estandar
	if(valor<0){
		fprintf (stdout, "Error en cierre de archivo.");
		exit(1);
	}
	fid = open ( fileName , flags , perm );  //trato de abrir archivo de entrada secundaria fileName
	if ( fid < 0 ) {
		fprintf (stdout, "Error intentando abrir el archivo de entrada." );
		exit ( 1 );
	} 
	if ( dup ( fid ) < 0 ) {
		fprintf (stdout, "Error en duplicacion de ficheros." );
		exit ( 1 );
	}
	valor=close ( fid );  //cierro el archivo
	if(valor<0){
		fprintf (stdout, "Error en cierre de archivo.");
		exit(1);
	}
}


/**
* Verifica si debe realizarse un pipeline ante la presencia de un caracter '|'.
* @param argv Argumentos del comando ingresado por el usuario.
* @param argv1 Array donde se guardarán los argumentos del comando 1.
* @param argv2 Array donde se guardarán los argumentos del comando 2.
* @return Devuelve 1 si se debe ejecutar el pipeline. 0 en caso contrario.
*/
int checkPipe ( char* argv[] , char* argv1[] , char* argv2[] ) {
	int indexArgumento, indexArgumento2;
	for( indexArgumento = 0 ; argv[indexArgumento] != NULL ; indexArgumento++ ) {
		int aux = strcmp ( "|" , argv[indexArgumento] );
		if( aux == 0 )
			break;
		argv1[indexArgumento] = argv[indexArgumento];
	}
	argv1[indexArgumento] = '\0';
	//Si no encontró |, se devuelve un 0, si no, se obtienen los dos argv de los comandos.
	if ( argv[indexArgumento] == NULL )
		return 0;
	indexArgumento++; //aumento para saltear el | 
	for ( indexArgumento2 = 0 ; argv[indexArgumento] != NULL ; indexArgumento2++ ) {
		if ( argv[indexArgumento2+indexArgumento] == NULL )
			break;
		argv2[indexArgumento2] =  argv[indexArgumento+indexArgumento2] ;
	}
	indexArgumento++;
	argv2[indexArgumento2] = '\0';
	return 1;
}

/**
* Ejecuta el Pipeline
* @param argv1 Argumentos del comando 1.
* @param argv2 Argumentos del comando 2.
* @param paths Paths donde puede buscar los archivos que ejecutan los comandos.
*/
void doPipeline ( char* argv1[] , char* argv2[] , char* paths[] ) {
	char executePath[400];
	int fd[2]; //pipe!
	pipe ( fd );
	if ( fork () == 0 ) { //codigo del hijo
		int err4=close ( fd[0] );
		if(err4<0){
			fprintf (stdout, "Error en el cierre del archivo.");
			exit(1);
		}
		int err8=dup2 ( fd[1] , 1 ); // redireccion de la salida al pipe.
		if(err8<0){
			fprintf (stdout, "Error en la duplicacion del fichero.");
			exit(1);
		}
		int err5=close ( fd[1] );
		if(err5<0){
			fprintf (stdout, "Error en el cierre del archivo.");
			exit(1);
		}
		buscarArchivo ( argv1[0] , paths , executePath );
		execv ( executePath , argv1 );
		fprintf (stdout, "%s\n", executePath );
		exit ( 1 );
	} 
	else {
		int err6= close ( fd[1] ); //codigo del padre
		if(err6<0){
			fprintf (stdout, "Error en el cierre del archivo.");
			exit(1);
		}
		int err9= dup2 ( fd[0] , 0 );
		if(err9<0){
			fprintf (stdout, "Error en la duplicacion del fichero.");
			exit(1);
		}
		int err7= close ( fd[0] );
		if(err7<0){
			fprintf (stdout, "Error en el cierre del archivo.");
			exit(1);
		}

		buscarArchivo ( argv2[0] , paths, executePath );
		execv ( executePath , argv2 );
		
		fprintf (stdout, "%s", executePath );
		exit ( 1 );
	}
}


/**
* Remplaza el /home/username por el caracter ~.
* @param directorioDeTrabajo Arreglo que contiene el directorio actual de trabajo.
* @return Devuelve el directorio a imprimir por consola.
*/
char *acondicionarHome ( char directorioDeTrabajo[] ) {
	char *dir = directorioDeTrabajo;
	char *pointer;
	int barra = ( int ) '/';
	pointer = strstr ( dir , "/home/" ); //Detecta presencia de /home/ en el directorio.
	if ( pointer != NULL ) {
		char *pointer2;
		pointer2 = strchr ( dir , barra );
		pointer2 = strchr ( pointer2+1 , barra );
		if ( pointer2 != NULL ) {
			pointer2 = strchr ( pointer2+1 , barra ); //Detecta la cantidad de barras "/".
			char *enie = "~";
			if ( pointer2 != NULL ) {
				
				strcpy(dir,enie);
				strcat(dir,pointer2);
				return dir;
			}
			else {
				
				strcpy(dir,enie);
				strcat(dir,"$");
				return dir;
			}

		}
		else {
			return dir;
		}
	}
	else {
		return dir;
	}
}









extern int errno ;

/* 
 * @brief Funcion bash. Implementa la funcionalidad del bash propiamente
		dicho. 
*  @param user (char []) Username. (No funcionan las funciones para obtenerlo).
*  @param comando (char []) Comando a ejecutar.
*  @return 0 en caso de finalizacion correcta de la funcion.
*/
int bash (char user[], char comando[]) 
{
	/* Sistemas Operativos I */
	int pid, flagWaitPID, bgProcess;
	int argC;
	char* argV[20];
	char* argv1[20];       //Para el caso que se haga un pipeline.
	char* argv2[20];
	char executePath[TAM]; //Path del proceso que se va a ejecutar.
	//char comando [256];    //Comando que ingresa el usuario.

	char actualdir[TAM];
	//Almaceno nombre de usuario logeado y nombre del equipo.
	char hostname [100];
	int err0=gethostname ( hostname ,100);
	if(err0<0){
		fprintf (stdout, "Error en obtencion de hostname.");
		exit(1);
	}
	strcat(hostname, ":");
	//Obtengo todos los paths que están en la variable de entorno PATH
	char *paths[20];
	getPaths ( paths );
	int volatile contadorHijos = 0; //Para saber el numero de hijos.

	short flag_while = 1; /* Para asegurar la ejecucion una sola vez y 
							aprovechar los continue.*/
	while (flag_while) 
	{
		flag_while = 0; /* Para aprovechar los continue. */
		flagWaitPID = 0; //Limpio las banderas
		getcwd ( actualdir , TAM );  //Directorio de trabajo almacenado en actualdir.
		if(actualdir==NULL){
			fprintf (stdout, "Error en obtener del directorio actual.");
			exit(1);
		}

		LOG_RED ( user , hostname ); //Imprime el uid/gid

		strcat ( actualdir , "$" );
		char *dir = acondicionarHome ( actualdir );
		strcat (actualdir, " ");
		strcat (actualdir, comando); /* Imprimo comando introducido. */ 
		strcat (actualdir, SEPARADOR); /* Imprimo <p> para HTML */
		LOG_BLUE ( dir );
		
		//memset( comando, '\0', 256 );
		fflush (stdout);
		
		//Si comando tiene solo un salto de línea, no hago nada.
		if( (!strncmp ( comando , "\n", 1)) ) {
			continue;
		}







		//Esto es lo que se ejecuta si se ingresa algun comando
		else {
			argC = parse_Command ( argV , comando );
		//Comandos internos del bash. 
			if ( !strcmp ( comando , "exit" ) )
				return 0;
			
			

			if( !strcmp (argV[0] , "cd" ) || !strcmp (argV[0] , "~" )) {
				contadorHijos=0;
				if ( argC==1 ) {
					cdBuiltin ( "/home/raspi" ); /* No se puede acceder
													a la variable de entorno
													HOME */
				}
				if(argC>=2){ // Comando interno cd con directorios que presentan espacios en blanco (va a intentar unir todos los argumentos y abrir el directorio).
					char carpeta[100] = "";
					for ( int i = 1 ; i < argC ; ++i ) {
						strcat ( carpeta , argV[i] );
						if ( argC != i+1 ) {
						strcat ( carpeta , " " );
						}
					}
					cdBuiltin ( carpeta );
					continue;
				}
				continue;
			}

			if ( strcmp ( argV[0] , ".." ) == 0 ) {  //Ingreso de .. (Basado en Shell de Linux).
				contadorHijos=0;
				fprintf(stdout, "%s\n" , "..: No se encontro la orden" );
				continue;
			}



			if (strcmp ( argV[0] , "&" ) == 0 ) {  //Ingreso de .. (Basado en Shell de Linux).
				contadorHijos=0;
				fprintf(stdout, "%s\n" , "bash : error sintactico cerca del elemento inesperado \"&\"");
				continue;
			}

			int flagRedirect = 0;
			int doPipe = 0;
			char fileName[50];
			

			//Verifico si el proceso se tiene que ejecutar en Background.
			bgProcess = background ( argV , argC );
			if ( bgProcess == 1 ) {
				argV[argC-1] = NULL;
				argC--;
			}
			if ( bgProcess == 2 ) { //no hay que cortar los argumentos ni reducir argc
				bgProcess=1;
			}
			doPipe = checkPipe ( argV , argv1 , argv2 );
			flagRedirect = checkRedirect ( argV , fileName );
			buscarArchivo ( argV[0] , paths , executePath );
			if ( executePath[0] == 'D' ){
				contadorHijos=0;
				fprintf(stdout,"baash: %s : Is a directory.\n",argV[0]);
				continue;
			}
			if ( executePath[0] == 'X' ){
				fprintf (stdout,"baash: %s : No se encontro el archivo.\n",argV[0] );
				contadorHijos=0;
			}
			else {
				int pipeExecuted = 0;
				contadorHijos++;
				pid = fork();
				if ( pid < 0 ) {
					fprintf (stdout, "Error al hacer el fork");
					exit ( 1 );
				}
				else if ( pid == 0 ) {  //Proceso hijo.
					if ( flagRedirect == 2 ) {   //comprueba las banderas de redireccion de entrada/salida
						outPut ( fileName );
					}
					else if ( flagRedirect == 1 ) {           
						freopen(fileName,"r",stdin);

					}
					else if ( doPipe == 1 ) {
						doPipeline ( argv1 , argv2 , paths );
						pipeExecuted = 1;
					}
					if ( !pipeExecuted ) {
						usleep(500);
						execv ( executePath , argV );
						
						fprintf (stdout, "%s", executePath ); /* Imprimo salida
															    de ejecucion del
															    comando. */
						sleep ( 1 ) ;
						exit (1);
					}
				}
				//Proces padre.
				else { 
					flagWaitPID = -1;
					waitpid( -1 , NULL , WNOHANG );

				}
				if ( bgProcess ) {
					fprintf(stdout, "[%d]   %d\n", contadorHijos , pid ); // para imprimir [Id] hijo
				
				}
				else {
					waitpid ( pid , &flagWaitPID , 0 ); // No espero!
					contadorHijos=0;
				
				}
			}
		}
	}
	
	
	
	getcwd ( actualdir , TAM );  //Directorio de trabajo almacenado en actualdir.
	
	if(actualdir==NULL){
		fprintf (stdout, "Error en obtener directorio actual.");
		exit(1);
	}


	LOG_RED ( user , hostname ); //Imprime el uid/gid

	/* Para poder llevar a cabo correctamente la ejecucion del comando
	cd (builtin) para este TP, se debio recurrir a almacenar en un archivo 
	el working directory donde finaliza la ejecucion el baash */
	FILE *fichero;
	fichero = fopen ("/var/www/html/cgi-bin/dir_actual","w");
	if (fichero == NULL)
	{
		printf ("</b>%s</br></br>", "Error en apertura del archivo.");
		exit (1);
	}
    else
    {
    	
    	
		fprintf (fichero,"%s", actualdir);
	    int e = fclose (fichero);
		if ( e != 0) /* Error en cierre del archivo */
		{
			printf ("</b>%s</br></br>", "Error en cierre del archivo del dir_actual.");
			exit (1);
		}
	}

	strcat ( actualdir , "$" );
	char *dir = acondicionarHome ( actualdir );
	LOG_BLUE ( dir );
	fprintf(stdout,"\n");

	
	return 0;	
}







/* Funcion principal */
int main (int argc, char *argv[])
{
	
	/* Header de Mime */
	fprintf (stdout, "Content-type: text/html;charset=utf-8\n\n");
	
	/* Titulo */
	fprintf (stdout, "<html><head><title>TP3 Lopez Gaston</title></head><body>"
		"<center></br><h1>Trabajo Practico 3 de Sistemas Operativos II</h1>"
		"</br><h2>Baash:</h2>"
		"</br>");
	
	fflush (stdout);

	/* Recibir comando del formulario. */
	char *info_formulario = getenv ("QUERY_STRING");
	if (info_formulario == NULL)
	{
		printf ("</br>Comando nulo. Ingrese un comando.</br></br>");
	}
	else 
	{
		char buffer [TAM];
		/* Conversion de formato */
		sscanf (info_formulario, "comando=%256s", &buffer[0]); 
		
		/* Se requiere un procesamiento previo antes de ingresar 
		el comando como parametro al baash debido a que los espacios en 
		blanco llegan como signo mas (de suma) y, por ejemplo, el signo 
		pesos ($) como "%24" */
		char comando [TAM];
		int i; /* Para recorrer comando del formulario */
		int cont_caracteres = 0; /* Para recorrer comando final */
		short flag_parametro = 0; /* Util cuando llega un + */
		short flag_signo_pesos = 0; /* Util cuando llega %24 */
		short cont_signo_pesos = 0; /* Se deben ignorar el '2' y el '4'
										cuando llega %24 */
		for (i = 0; i < TAM; i++)
		{
			if (flag_signo_pesos)
			{
				cont_signo_pesos++;
				if (cont_signo_pesos > 1)
				{
					cont_signo_pesos = 0;
					flag_signo_pesos = 0;
				}
			}
			else
			{
				if ((buffer[i] != '+') && (buffer[i] != '%'))
				{
					comando[cont_caracteres] = buffer[i];
					cont_caracteres++;
				}

				else if (buffer[i] == '%')
				{
					if (i < (TAM - 3))
					{
						if ((buffer[i+1] == '2') && (buffer[i+2] == '4'))
						{
							comando[cont_caracteres]= '$';
							cont_caracteres++;
							flag_signo_pesos = 1;
						}
					}
				}

				else if (!flag_parametro)
				{
					flag_parametro = 1;
					comando[cont_caracteres]= ' ';
					cont_caracteres++;
				}
			}		
		}
		comando [cont_caracteres] = '\n';

		
		
		
		fprintf (stdout, "<hr><h3>Salida del baash.</h3><p>");
		fflush (stdout);

		/* Leo el fichero dir_actual para obtener el ultimo working
		directory en el que se encontro el baash durante su ultima 
		ejecucion. Esto se realiza para lograr el funcionamiento 
		del comando cd (builtin) */
		FILE *fichero;
		fichero = fopen ("/var/www/html/cgi-bin/dir_actual","r");
		if (fichero == NULL)
		{
			printf ("</b>%s</br></br>", "Error en apertura del archivo dir_actual.");
			exit (1);
		}
	    else
	    {	    	
	    	
	    	while (!feof (fichero))
	    	{
				char *buf_aux = malloc (sizeof (char) * TAM);
				char *info_cd = fgets (buf_aux, TAM, fichero);
				if (info_cd != NULL)
				{
					cdBuiltin (buf_aux); /* Change directory (chdir) */
				}
				free (buf_aux);
			}
	    	
		    int e = fclose (fichero);
			if ( e != 0) /* Error en cierre del archivo */
			{
				printf ("</b>%s</br></br>", "Error en cierre del archivo dir_actual.");
				exit (1);
			}
		}

		char user[6]= "raspi"; /* No funcionan las funciones para 
								obtener el username */
		bash (user, comando); /* Llamo a la funcion bash con el comando
								procesado. */
	}

	/* Formulario para enviar otro comando */
	fprintf (stdout, "<hr><form action=\"/cgi-bin/baash.cgi\" method=\"get\"><p>"
		"Ingrese un comando para ejecutar en el Baash de Sistemas Operativos I y presione en Enviar:<p>"
		"<input type=\"text\" name=\"comando\" size=\"50\" maxlength=\"256\" required></br>"
		"</br><input type=\"submit\" value=\"Enviar\"></br></br></form><hr>");
	/* Boton para volver a la pagina principal */
	fprintf (stdout, "<a href=\"../index.lighttpd.html\"><button style=\"FONT-SIZE: 13pt\">"
		"<p>Volver a la Pagina Principal</button></a></p></center></body></html>");
	
	fflush (stdout);
	return 0;
}