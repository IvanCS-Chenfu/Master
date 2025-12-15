+--------------------------------
| File: Readme.txt
|
| Author: Sergio Romero Montiel
|
| Created on October 21th, 2016
+--------------------------------


Periodic Process
================

Incluye varios codigos de ejemplo para generar procesos periodicos.

Hay cinco versiones:

(basadas en pthreads y clock_nanosleep)

a) El codigo periodic_process_1 crea un thread de posix.
Como argumento se le pasa una estructura con la informacion del periodo y la
funcion a ejecutar. La funcion asociada al thread (periodicProcess) ejecuta
la función indicada (carga de pago) y después duerme un tiempo relativo
indicado (periodo). Tanto la deriva local como la acumulada son inaceptables

b) El codigo periodic_process_2 crea un thread de posix.
Como argumento se le pasa una estructura con la informacion del periodo y la
funcion a ejecutar. La funcion asociada al thread (periodicProcess) ejecuta
la función indicada (carga de pago) midiendo el tiempo que dedica al computo
(now-start), después duerme un tiempo relativo calculado como el periodo
indicado menos el tiempo dedicado. La deriva local se situa en unos valores
razonables, pero la deriva acumulativa sigue siendo inaceptable.

c) El codigo periodic_process_3 crea un thread de posix.
Como argumento se le pasa una estructura con la informacion del periodo y la
funcion a ejecutar. La funcion asociada al thread (periodicProcess) establece
el instante inicial y calcula cuando le corresponde ejecutarse en el siguiente
plazo, inmediatamente ejecuta la funcion inidicada (carga de pago) y se duerme
hasta el instante absoluto previamente calculado, para calcular la siguiente
activacion. Esta implementación resuelve el problema de la deriva acumulativa.


(basadas en timer con notificacion por signal o por thread)

d) El codigo periodic_process_4 arma un temporizador con el periodo indicado y
notificacion por una signal de tiempo real. La funcion objetivo (carga de pago)
se asocia como manejador (sigaction) de la signal de tiempo real.
A cada vencimiento del temporizador, se envia el signal definido al propio
proceso, activandose la funcion objetivo.
Los timers no provocan de deriva temporal por software.


e) El codigo periodic_process_5 arma un temporizador con el periodo indicado y
notificacion a traves de la ejecucion de una funcion como un thread.
En la configuración del timer se indica tanto el periodo como la funcion a
ejecutar.  Los timers no provocan de deriva temporal por software.
