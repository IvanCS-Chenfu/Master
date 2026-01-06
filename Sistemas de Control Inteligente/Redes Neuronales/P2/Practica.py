# -*- coding: utf-8 -*-
"""
Editor de Spyder

Este es un archivo temporal.
"""

import numpy as np
import matplotlib.pyplot as plt


from rna_competitiva import mayor_pot_sinaptico, entrenamiento_competivo



def visualizar_patrones(patrones, num_bloques_alto, num_bloques_ancho, flag_lineas):

    
   if patrones.ndim == 2:
       filas_patrones, num_ventanas = patrones.shape
   else:
       filas_patrones, num_ventanas, _ = patrones.shape
       
       
   tam_cuadrado = int(np.sqrt(filas_patrones))

   fig, axes = plt.subplots(num_bloques_alto, num_bloques_ancho, figsize=(2*num_bloques_ancho, 2*num_bloques_alto))

   idx = 0
   for i in range(num_bloques_alto):
       for j in range(num_bloques_ancho):
           
           ax = axes[i, j]
           if patrones.ndim == 2:
               
               vector = patrones[:, idx]
               subimagen = vector.reshape((tam_cuadrado, tam_cuadrado))
    
               ax.imshow(subimagen, cmap='gray', interpolation='nearest', vmin = 0.0, vmax = 1.0)
               
               
           else:
               
               patch = patrones[:, idx, :]
               subimagen = patch.reshape((tam_cuadrado, tam_cuadrado, 3))
               
               ax.imshow(subimagen, interpolation='nearest', vmin=0.0, vmax=1.0)
               
           ax.axis('off')
           idx += 1

   plt.tight_layout()
   
   if not flag_lineas:
       plt.subplots_adjust(wspace=0, hspace=0)
       
   plt.show()

def imagen_a_patrones(imagen,tam_cuadrado, flag_mostrar):
    
    alto, ancho = imagen.shape
    
    respuesta = 0
    
    if alto % tam_cuadrado != 0 or ancho % tam_cuadrado != 0:
        respuesta = -1
        
    else:
        num_bloques_alto = alto // tam_cuadrado
        num_bloques_ancho = ancho // tam_cuadrado
        
        num_ventanas = num_bloques_alto * num_bloques_ancho
        
        patrones = np.zeros((tam_cuadrado * tam_cuadrado, num_ventanas))
        
        
        columna = 0
        
        for i_bloque in range(num_bloques_alto):
            for j_bloque in range(num_bloques_ancho):
    
                # Coordenadas de la esquina superior izquierda de la ventana
                fila_ini = i_bloque * tam_cuadrado
                col_ini  = j_bloque * tam_cuadrado
    
                # Extraemos la ventana [fila_ini : fila_ini+tam, col_ini : col_ini+tam]
                ventana = imagen[fila_ini:fila_ini + tam_cuadrado,
                                 col_ini:col_ini + tam_cuadrado]
    
                # Aplanamos la ventana en un vector columna
                # (el orden por defecto es por filas, que es lo habitual)
                patrones[:, columna] = ventana.reshape(-1)
    
                columna += 1
        
        if flag_mostrar:
            visualizar_patrones(patrones, num_bloques_alto, num_bloques_ancho, True)
            
        respuesta = patrones
        
    return respuesta
                

def comprimir_patrones(W, patrones):
    """
    Sustituye cada patrón por el prototipo (columna de W) de su neurona ganadora.
    """
    dim, num_patrones = patrones.shape
    dimW, num_neuronas = W.shape

    patrones_comp = np.zeros_like(patrones)

    for j in range(num_patrones):
        patron = patrones[:, j]
        # neurona ganadora usando tu función
        r = mayor_pot_sinaptico(W, patron, num_neuronas)
        # copiamos el prototipo correspondiente
        patrones_comp[:, j] = W[:, r]

    return patrones_comp

#######################################################################

img = plt.imread("Imagen1.png")

plt.imshow(img, cmap='gray')

n_sub_imagenes = 100
tam_cuadrado = 16
n_epocas = 100
tasa_aprendizaje = 0.5

patrones = imagen_a_patrones(img , tam_cuadrado, True)

W = entrenamiento_competivo(tasa_aprendizaje, n_epocas, n_sub_imagenes, patrones)

patrones_comp = comprimir_patrones(W, patrones)

visualizar_patrones(patrones_comp, int(480/tam_cuadrado), int(640/tam_cuadrado), False)

########################################################################

"""
img = plt.imread("Imagen2.png")

plt.imshow(img)
plt.show()

R = img[:, :, 0]
G = img[:, :, 1]
B = img[:, :, 2]

n_sub_imagenes = 64
tam_cuadrado = 16
n_epocas = 5
tasa_aprendizaje = 0.5

pat_R = imagen_a_patrones(R, tam_cuadrado, False)
W_R   = entrenamiento_competivo(tasa_aprendizaje, n_epocas, n_sub_imagenes, pat_R)
comp_R = comprimir_patrones(W_R, pat_R)

pat_G = imagen_a_patrones(G, tam_cuadrado, False)
W_G   = entrenamiento_competivo(tasa_aprendizaje, n_epocas, n_sub_imagenes, pat_G)
comp_G = comprimir_patrones(W_G, pat_G)

pat_B = imagen_a_patrones(B, tam_cuadrado, False)
W_B   = entrenamiento_competivo(tasa_aprendizaje, n_epocas, n_sub_imagenes, pat_B)
comp_B = comprimir_patrones(W_B, pat_B)

patrones_color = np.stack([comp_R, comp_G, comp_B], axis=2)

visualizar_patrones(patrones_color, int(480/tam_cuadrado), int(640/tam_cuadrado), flag_lineas=False)
"""