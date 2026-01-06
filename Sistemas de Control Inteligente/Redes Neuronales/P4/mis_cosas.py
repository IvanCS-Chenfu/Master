# -*- coding: utf-8 -*-
"""
Created on Fri Dec  5 11:06:51 2025

@author: Chenfu
"""
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.decomposition import PCA
from sklearn.metrics import confusion_matrix
import seaborn as sns

def informacion_var (X):
    
    dim = X.ndim
    
    match dim:
        case 1:
            print("El tamaño es de ", X.size, " elementos")
            
        case 2:
            
            print("El tamaño es de ", X.size, " elementos [", X.shape[0], "x" , X.shape[1], "]")
            
        case 3: 
            print("El tamaño es de ", X.size, " elementos [", X.shape[0], "x" , X.shape[1], "x" , X.shape[2], "]")
        
        case 4: 
            print("El tamaño es de ", X.size, " elementos [", X.shape[0], "x" , X.shape[1], "x" , X.shape[2], "x" , X.shape[3], "]")
            
    print("Los elementos siguen el rango [",np.min(X), ",", np.max(X), "]")
    print()
    
def split_all(dataset,porcentaje_train, porcentaje_test):
    
    X = dataset.data 
    Y = dataset.target 
    
    X_test = X_val = X_train = Y_test = Y_val = Y_train = None
    
    if porcentaje_train != 1.0:
        
        X_test_val, X_train, Y_test_val, Y_train = train_test_split(
            X, Y, test_size=porcentaje_train, shuffle=True)
    
        if porcentaje_test != 0.0:
            
            X_val, X_test, Y_val, Y_test = train_test_split(
                X_test_val, Y_test_val, test_size=(porcentaje_test/(1-porcentaje_train)), shuffle=True)
            
        else:
            
            X_val = X_test_val
            Y_val = Y_test_val
            
    else:
        
        X_train = X
        Y_train = Y
        
    return X_train, Y_train, X_val, Y_val, X_test, Y_test


def escalado(scaler, X, X_train, X_val, X_test):
    
    if X is not None:
        if X.ndim == 1:
            X = X.reshape(-1, 1)
            
        X_escalado = scaler.fit_transform(X)
    else:
        X_escalado = X
        
    if X_train is not None:
        if X_train.ndim == 1:
            X_train = X_train.reshape(-1, 1)
        X_train = scaler.transform(X_train)
        
    if X_val is not None:
        if X_val.ndim == 1:
            X_val = X_val.reshape(-1, 1)
        X_val   = scaler.transform(X_val)
    
    if X_test is not None:
        if X_test.ndim == 1:
            X_test = X_test.reshape(-1, 1)
        X_test  = scaler.transform(X_test)
        
    return X_escalado, X_train, X_val, X_test
    
  
def red_dim(dataset = None, X = None, Y = None, n_dim = 2, scaler = None):
    
    if dataset is not None:
        X = dataset.data 
        Y = dataset.target 
        target_names = dataset.target_names
        _, n_atributos = X.shape
        
    else:
        target_names = range(int(np.max(Y)))
        n_atributos = int(np.max(Y))
        
    if scaler is not None:
        X, _, _, _ = escalado(scaler, X, None, None, None)
    
    
    colores = [
                "navy", "turquoise", "darkorange", "red", "green",
                "purple", "brown", "pink", "gray", "olive",
                "cyan", "magenta", "gold", "teal", "coral",
                "lime", "indigo", "maroon", "peru", "darkgreen"
              ]
    
    
    n_atributos = n_atributos-1
    
    colores_usados = colores[:n_atributos]
    
    match n_dim:
        case 1:
            escalado_PCA = PCA(n_components=1)
            
            X_escalado = escalado_PCA.fit_transform(X)
            
            plt.figure()
            
            for color, i, target_name in zip(colores_usados, range(n_atributos), target_names):
                mask = (Y == i)
                plt.scatter(X_escalado[Y == i, 0],
                            [0]*mask.sum(),
                            color = color, alpha = 0.8, label = target_name)

            plt.legend(loc="best", shadow = False, scatterpoints = 1)
            plt.title("PCA")
        
        case 2:
            escalado_PCA = PCA(n_components=2)
            X_escalado = escalado_PCA.fit_transform(X)
            
            plt.figure()
            
            for color, i, target_name in zip(colores_usados, range(n_atributos), target_names):
                plt.scatter(X_escalado[Y == i, 0],
                            X_escalado[Y == i, 1],
                            color = color, alpha = 0.8, label = target_name)

            plt.legend(loc="best", shadow = False, scatterpoints = 1)
            plt.title("PCA")
            
        case 3:
            escalado_PCA = PCA(n_components=3)
            X_escalado = escalado_PCA.fit_transform(X)
            
            figure = plt.figure()
            ax = figure.add_subplot(111, projection='3d')
            
            for color, i, target_name in zip(colores_usados, range(n_atributos), target_names):
                ax.scatter(X_escalado[Y == i, 0],
                           X_escalado[Y == i, 1],
                           X_escalado[Y == i, 2],
                           color = color, alpha = 0.8, label = target_name)

            plt.legend(loc="best", shadow = False, scatterpoints = 1)
            plt.title("PCA")
        
    
def grafica_epocas(resultados, titulo):
    
    historial = resultados.history
    
    nombre_metricas = list(historial.keys())
    valores_metricas = list(historial.values())
    
    colores = ["blue", "orange", "green", "red"]
    
    plt.figure()
    
    for i, color, metrica in zip(range(4), colores, nombre_metricas):
        plt.plot(valores_metricas[i], color=color, label=metrica)
        
    plt.grid()
    plt.legend(loc="best", shadow=False)
    plt.title(titulo)
    plt.xlabel("Número de Épocas")
    plt.ylabel("Valor de las Métricas")
    
    plt.show()
    
def matriz_confusion(Y_test,Y_pred):
    
    matriz = confusion_matrix(Y_test, Y_pred)
    
    plt.figure()
    sns.heatmap(matriz, annot=True, fmt="d", cmap='Blues')
    plt.xlabel("Predicción")
    plt.ylabel("Valor real")
    plt.title("Matriz de Confusión")
    plt.show()
    
def histograma(dataset):
    X = dataset.data
    atributos = dataset.feature_names

    num_atributos = len(atributos)
    filas = 2
    columnas = (num_atributos + 1) // 2  # Calcula columnas automáticamente

    plt.figure(figsize=(18, 7))

    for i, nombre in zip(range(num_atributos),atributos):
        plt.subplot(filas, columnas, i + 1)
        plt.hist(X[:, i], bins=30)
        plt.xlabel(nombre)
        plt.ylabel("Frecuencia")

    plt.show()
    
def imagen2linea(imagen):
    
    dim = imagen.ndim
    
    match dim:
        case 2:
            vector = imagen.reshape(-1)
            respuesta = vector
        case 3:
            array = imagen.reshape(imagen.shape[0], -1)
            respuesta = array
            
    return respuesta
    


def linea2imagen(respuesta, tamX, tamY):
    dim = respuesta.ndim
    
    match dim:
        case 1:
            vector = respuesta 
            imagen = vector.reshape(tamX, tamY)
            
        case 2:
            array = respuesta
            imagen = array.reshape(respuesta.shape[0], tamX, tamY)
    
    return imagen
    
