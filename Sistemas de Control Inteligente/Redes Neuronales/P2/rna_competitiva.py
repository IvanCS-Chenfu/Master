"""
MODULO: rna_competitiva.py
Implementa aprendizaje no supervisado con una red neuronal competitiva
"""
import random
import numpy as np

#DEFINICIÓN DE FUNCIONES

def mayor_pot_sinaptico(W,patron,num_neuronas):
    """
    Parametros
    ----------
    W : matriz de pesos sinápticos. Cada columna es un vector pesos sinápticos.
    patron : patrón de entrada a la red (vector columna). 
    num_neuronas : numero de neuronas de la red

    Retorno
    -------
    Devuelve el número de la neurona ganadora (la de mayor pot. sináptico)
    """
    mayor_potencial=neurona_ganadora=0
    for i in range(num_neuronas): #se recorren todas las neuronas
        hi=W[:,i]@patron-(W[:,i]@W[:,i])/2 #se calcula su potencial sinaptico
        if hi>mayor_potencial: #se determina el mayor
            mayor_potencial=hi
            neurona_ganadora=i
    return neurona_ganadora #se devuelve el numero de neurona ganadora


def entrenamiento_competivo(tasa_inicial,num_epocas,num_neuronas,patrones):
    """
    Parametros
    ----------
    tasa_inicial : tasa inicial de aprendizaje
    num_epocas : número de épocas de entrenamiento
    num_neuronas : número de neuronas de la red competitiva
    patrones : matriz con los patrones de entrenamiento dispuestos en columnas
               (cada columna es un patrón)

    Retorno
    -------
    Devuelve la matriz W de pesos sinápticos resultante del entrenamiento, 
    donde cada columna corresponde a un vector de pesos sinápticos.
    """
    
    #INICIALIZACIÓN DE LOS PESOS SINÁPTICOS
    #se determina el número de patrones de entrenamiento y su dimensión    
    dim_patrones,num_patrones=patrones.shape
    #se crea la matriz de pesos sinápticos inicialmente con zeros
    W=np.zeros((dim_patrones,num_neuronas))
    #se genera una lista que contiene el índice de los patrones
    lista_indices_patrones=np.arange(0,num_patrones)
    #se crea una lista aleatoria con tantos índices de patrones
    #de entrada como número de neuronas de la red
    indices=random.sample(list(lista_indices_patrones),num_neuronas)

    #se inicializan los pesos sinápicos de la red con los patrones 
    #seleccionados aleatoriamente
    for i in range(num_neuronas):
        W[:,i]=patrones[:,indices[i]]
    
    #ENTRENAMIENTO DE LA RED  
    T=num_epocas*num_patrones #iteraciones totales entrenamiento
    k=0 #k es el número de iteración en curso, inicialmente cero
    #Primer Bucle: tantas iteraciones como épocas
    print("\nINICIO ENTRENAMIENTO DE LAS", num_epocas,"ÉPOCAS")
    for epoca in range(num_epocas):
        print("Época",epoca+1)
        #se baraja la lista de índices para que los patrones se introduzcan
        #en cada época de forma aleatoria
        random.shuffle(lista_indices_patrones)
        #Segundo Bucle: se introducen todos los patrones aleatoriamente
        for indice_patron in lista_indices_patrones: 
            #se determina la tasa de aprendizaje de la iteración
            tasa=tasa_inicial*(1-(k/T)) 
            #se toma un patrón
            patron=patrones[:,indice_patron]
            #se computa cual es la neurona ganadora
            ganadora=mayor_pot_sinaptico(W, patron, num_neuronas)
            Wr=W[:,ganadora]
            #se modifica su vector de pesos sinápticos
            W[:,ganadora]=Wr+tasa*(patron-Wr)    
            k+=1 #se contabiliza la siguiente iteracion
    #se devuelve la matriz de pesos sinápticos entrenada
    return W

