# -*- coding: utf-8 -*-
"""
Editor de Spyder

Este es un archivo temporal.
"""

import numpy as np
import matplotlib.pyplot as plt


from patrones_lippman import patrones



# Recibe una matriz 2D y devuelve un vector 1D con todos sus elementos.
def matriz_a_vector(matriz):

    return np.ravel(matriz)



# Actualiza la matriz de pesos actual añadiendole un nuevo patrón
def actualizar_pesos(vector, W_actual):

    matriz_patron = np.outer(vector, vector)    # Producto externo: v * v^T -> matriz

    W_nueva = W_actual + matriz_patron          # Sumamos a la matriz de pesos

    np.fill_diagonal(W_nueva, 0)                # Quitamos la diagonal (ponerla a cero)

    return W_nueva



# Crea una matriz de pesos de Hopfield dados unos patrones.
def crear_pesos_hopfield(patrones):

    num_patrones = patrones.shape[0]                # Número de patrones

    num_elementos = patrones[0].size                # Número de elementos (neuronas) por patrón

    W = np.zeros((num_elementos, num_elementos))    # Matriz de pesos inicial (todo ceros)

    # Recorremos todos los patrones
    for i in range(num_patrones):
        patron_matriz = patrones[i]
        v = matriz_a_vector(patron_matriz)
        W = actualizar_pesos(v, W)

    return W



def distorsionar_patron(matriz, n):

    matriz = np.array(matriz, copy=True)                # Evitamos modificar la original
    
    filas, columnas = matriz.shape

    for i in range(filas):
        for _ in range(n):
            # Número aleatorio entre 0 y columnas-1
            idx = np.random.randint(0, columnas)
            
            # Cambiar signo del elemento seleccionado
            matriz[i, idx] *= -1

    return matriz



    
def aplicar_hopfield(W, patron, i_max, mostrar):

   filas, columnas = patron.shape

   # Pasamos a vector
   v = matriz_a_vector(patron)

   iteracion = 0
   fin = False

   while not fin:
       # Multiplicación W * v
       v_nuevo = W @ v

       # Cada elemento se cambia por su signo
       v_nuevo = np.sign(v_nuevo)

       iteracion += 1
       
       if mostrar:
           
            plt.figure()
            img = v_nuevo.reshape(filas, columnas)
            plt.imshow(img, cmap='gray', vmin=-1, vmax=1)
            plt.title(f"Iteración {iteracion}")
            plt.axis('off')
            plt.show()
            
       # Condiciones de parada:
       # 1) se alcanza el máximo de iteraciones
       # 2) el vector no cambia respecto al anterior
       if iteracion >= i_max or np.array_equal(v_nuevo, v):
           fin = True

       # Actualizamos el vector para la siguiente iteración
       v = v_nuevo

   # Volvemos a forma de matriz
   matriz_final = v.reshape(filas, columnas)

   return matriz_final




# Crear la matriz de pesos de Hopfield
W = crear_pesos_hopfield(patrones)

fig, ax = plt.subplots(1, 3, figsize=(12, 4), sharey=True)

# Elegimos un patrón, por ejemplo el 0
patron_original = patrones[6]
ax[0].imshow(patron_original, cmap='gray', vmin=-1, vmax=1)
ax[0].set_title("Original")

# Lo distorsionamos
patron_ruidoso = distorsionar_patron(patron_original, n=4)
ax[1].imshow(patron_ruidoso, cmap='gray', vmin=-1, vmax=1)
ax[1].set_title("Ruidoso")

# Aplicamos Hopfield
patron_recuperado = aplicar_hopfield(W, patron_ruidoso, i_max=20, mostrar=False)
ax[2].imshow(patron_recuperado, cmap='gray', vmin=-1, vmax=1)
ax[2].set_title("Recuperado")

plt.show()

_ = aplicar_hopfield(W, patron_ruidoso, i_max=20, mostrar=True)