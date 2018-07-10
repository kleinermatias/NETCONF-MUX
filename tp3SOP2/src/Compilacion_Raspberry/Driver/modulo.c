/**
 * @file modulo.c
 * @brief Modulo (driver) simple y vacío del TP3 de Sistemas Operativos II,
 	que solamente imprime “Hello World” al instalarse y “Good ByeWorld” al 
 	ser removido del kernel. 
 * @author López, Gastón
*/


/* Importacion de librerias */

#include <linux/init.h> 	/* Macros para funciones __init  __exit. */
#include <linux/module.h> 	/* Header para agregar module al kernel. */

/* Autor. Se puede ver en modinfo */
MODULE_AUTHOR ("Lopez Gaston"); 
/* Descripción del módulo. */   	
MODULE_DESCRIPTION ("Modulo. Trabajo Practico 3 de Sistemas Operativos II");  


/**
 *  @brief Función de inicialización. Imprime "Hello World"
 		cuando se inserta el modulo. (Observar con dmesg).
 *  @return retorna 0 si se completo exitosamente.
 */
static int __init inicializacion_modulo (void)
{
    printk (KERN_INFO "Hello World.\n");
    return 0;
}


/**
 *  @brief Funcion de finalizacion. Imprime "Good ByeWorld"
 		cuando se remueve el modulo del kernel. (Observar con dmesg).
 */
static void __exit finalizacion_modulo (void)
{
    printk (KERN_INFO "Good ByeWorld.\n");
}

module_init (inicializacion_modulo);
module_exit (finalizacion_modulo);
