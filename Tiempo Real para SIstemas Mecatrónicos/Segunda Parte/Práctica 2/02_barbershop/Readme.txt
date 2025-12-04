+--------------------------------
| File: Readme.txt
|
| Author: Sergio Romero Montiel
|
| Created on October 21th, 2016
+--------------------------------


Problema de la barberia
=======================
Incluye dos implementaciones para la coordinación de threads clientes
y servidores (cajero y barberos):

El codigo barbershop_1.c es una solucion incorrecta cuando hay mas de
un servidor barbero, ya que los clientes esperan en el semaforo
'terminacion' en el orden de llegada, pero los barberos pueden acabar
en un orden diferente, liberando a los clientes de forma incorrecta.

El codigo barbershop_2.c resuelve el problema descrito al bloquearse
cada cliente en su propio semaforo de terminacion (variable local),
al servidor se le pasa un puntero a este junto con el numero en el
ticket y el barbero usa el puntero para desbloquear al cliente
correspondiente.

