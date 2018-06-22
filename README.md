# NETCONF - Datastores

NETCONF define un protocolo para administracion y configuracion de dispositivos de red, haciendo diferencia entre **datos de estado** y **datos de configuracion**.  

- **Datos de configuracion** : Aquellos datos que van a ser provistos por el usuario y que pueden ser escritos en alguna de las tres datastores definidas por NETCONF (Startup, Candidate y Running) excluyendo aquellos datos que el dispositivo aprendera por si mismo (datos de estado). Pueden obtenerse/modificarse mediante la operacion < get-config > y < edit-config > que define NETCONF. 

- **Datos de estado** : Son datos de solo lectura, no pueden ser escritos por el usuario. Son provistos por el dispositivo para informes o estadisticas. Estos datos no se encuentran en algun datastore, puesto a que no es dato de configuracion. Pueden obtenerse mediante la operacion < get > que define NETCONF. 



# Concepto de Datastore

NETCONF define a un datastore (de configuracion) como un lugar conceptual donde almacenar y acceder a informacion de configuracion para llevar al dispositivo desde un estado inicial a un estado operativo deseado. En NETCONF, se definen tres datastores (Startup - Candidate - Running) y permite al usuario definir mas datastores si lo desea.

- **Startup** : Almacen de datos de configuracion que contiene datos que seran cargadas al dispositivo cuando este inicie. Este datastore es opcional. Para guardar datos de configuracion al startup desde un estado de configuracion actual, es necesario realizar la operacion < copy-config > desde el datastore running al datastore startup.

- **Running** ****(1)*** : Es un almacen de datos de configuracion que contiene la configuracion aplicada al dispositivo actualmente. NETCONF requiere que este datastore se encuentre presente, al contrario de startup y candidate que son opcionales (estos ultimos pueden no ser soportados por el dispositivo) .

- **Candidate** : Al igual que startup, este datastore es opcional. Es util para manipular datos de configuracion que no son aplicados instantaneamente al dispositivo (como es el caso del datastore running). Los cambios aplicados a esta datastore no son visibles desde otras sesiones hasta que se aplique la operacion < commit > (al aplicar esta operacion, se copia la configuracion desde el DS candidate al DS running). NETCONF reconoce el concepto de "trabajo en curso", por lo que no se aplica validacion de sintaxis en este datastore con respecto a lo que se encuentra definido en el modulo YANG hasta que se haga el commit. Alternativamente, NETCONF provee una operacion para la validacion de sintaxis bajo demanda con < validate > ****(2)***.

> **Nota 1:** En Sysrepo, este DS (running) no sera visible a menos que exista una aplicacion suscripta a los cambios del modulo YANG instalado.
> 
> **Nota 2:** La version actual de Sysrepo soporta la operacion < commit > pero no la operacion < cancel-commit >. En otras palabras, el uso del DS candidate esta limitada a ser usada para modificar varios modulos y commitear los cambios (copiar estos cambios a running)  pero no verifica la nueva configuracion y no pueden revertirse los cambios con un < cancel-commit > en caso de error. Como describen en su '[TODO List](https://github.com/sysrepo/sysrepo/wiki/TODO-List)', esta funcionalidad se encuentra en desarrollo. 

Fuente: https://tools.ietf.org/html/draft-tp-netconf-datastore-00
