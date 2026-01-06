# -*- coding: utf-8 -*-
"""
Created on Fri Dec 12 17:08:41 2025

@author: Chenfu
"""
import mis_cosas as mc
from tensorflow import keras
import numpy as np


def CNN(dataset = None, 
        X_train = None, Y_train = None, X_val = None, Y_val = None, X_test = None, Y_test = None,
        porcentaje_train = 0.8, porcentaje_test = 0.0, 
        scaler = False,
        n_filtros = 64, tam_filtro = 4, tam_pool = 2, dropout = 0.2, n_capas_CNN = 1, batch_dropout = False,
        n_capa_oculta = 50, epocas = 10,
        one_hot_encoding = False, patience = None):
    
    X = None
    n_salida = None
    
    if dataset is not None:
        X = dataset.data
        Y = dataset.target
        
        X_train, Y_train, X_val, Y_val, X_test, Y_test = mc.split_all(dataset,porcentaje_train,porcentaje_test)
        
        n_salida = len(dataset.target_names)
    else:
        X = X_train     # Solo por el X.shape de alante
        n_salida = int(max(Y_train))+1   
 
    
    if X.ndim == 4:
        [_,tamX, tamY, RGB] = X.shape
    else:
        [_,tamX, tamY] = X.shape
        RGB = 1
        
        
    if scaler:
        
        X_train = X_train/255.0
        X_val = X_val/255.0
        X_test = X_test/255.0
        
    
    modelo = None
    if n_capas_CNN == 1:
        if batch_dropout:
            modelo = keras.Sequential([
                
                keras.layers.Conv2D(filters = n_filtros, kernel_size=tam_filtro, activation = "relu", input_shape=[tamX,tamY,RGB]),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.BatchNormalization(),
                keras.layers.Flatten(),
                keras.layers.Dropout(dropout),
                
                keras.layers.Dense(n_capa_oculta, activation="relu"),
                keras.layers.Dense(n_salida, activation="softmax")
            ])
        else:
            modelo = keras.Sequential([
                
                keras.layers.Conv2D(filters = n_filtros, kernel_size=tam_filtro, activation = "relu", input_shape=[tamX,tamY,RGB]),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.Flatten(),
                
                keras.layers.Dense(n_capa_oculta, activation="relu"),
                keras.layers.Dense(n_salida, activation="softmax")
            ])
    else:
        if batch_dropout:
            modelo = keras.Sequential([
                
                keras.layers.Conv2D(filters = n_filtros, kernel_size=tam_filtro, activation = "relu", input_shape=[tamX,tamY,RGB]),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.BatchNormalization(),
                keras.layers.Dropout(dropout),
                keras.layers.Conv2D(filters = int(n_filtros/2), kernel_size=int(tam_filtro/1.5), activation = "relu"),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.BatchNormalization(),
                keras.layers.Flatten(),
                keras.layers.Dropout(dropout),
                
                keras.layers.Dense(n_capa_oculta, activation="relu"),
                keras.layers.Dense(n_salida, activation="softmax")
            ])
        else:
            modelo = keras.Sequential([
                
                keras.layers.Conv2D(filters = n_filtros, kernel_size=tam_filtro, activation = "relu", input_shape=[tamX,tamY,RGB]),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.Conv2D(filters = int(n_filtros/2), kernel_size=int(tam_filtro/1.5), activation = "relu"),
                keras.layers.MaxPool2D(pool_size=tam_pool),
                keras.layers.Flatten(),
                
                keras.layers.Dense(n_capa_oculta, activation="relu"),
                keras.layers.Dense(n_salida, activation="softmax")
            ])
    
    if one_hot_encoding:
        
        if Y_train is not None:
            Y_train = keras.utils.to_categorical(Y_train)
        if Y_val is not None:
            Y_val   = keras.utils.to_categorical(Y_val)
        if Y_test is not None:
            Y_test  = keras.utils.to_categorical(Y_test)
        
        modelo.compile(loss="categorical_crossentropy", optimizer="adam", metrics=["accuracy"])
    else:
        modelo.compile(loss="sparse_categorical_crossentropy", optimizer="adam", metrics=["accuracy"])
        
        
    if patience is not None:
        parada_temprana = keras.callbacks.EarlyStopping(patience=patience)
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val), callbacks=[parada_temprana])
    else:
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val))
        
    return modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test

#####################################################################################

def Info_CNN (dataset = None,
              X_train = None, Y_train = None, X_val = None, Y_val = None, X_test = None, Y_test = None,
              scaler = False,
              resultados = None, matriz_conf = False, datos_terminal = False,
              Y_pred = None, one_hot_encoding = False):
    
    print("\n" + "="*60)
    print("INFORMACIÓN")
    print("="*60)
    
    if scaler:
        X_train = X_train*255
        X_val = X_val*255
        X_test = X_test*255
        

    if X_train is not None:
        mc.informacion_var(X_train)
    if Y_train is not None:
        mc.informacion_var(Y_train)
    if X_val is not None:
        mc.informacion_var(X_val)
    if Y_val is not None:
        mc.informacion_var(Y_val)
    if X_test is not None:
        mc.informacion_var(X_test)
    else:
        X_test = X_val
    if Y_test is not None:
        mc.informacion_var(Y_test)
    else: 
        Y_test = Y_val
        
    
    if resultados is not None:
        mc.grafica_epocas(resultados, "Evolución")
    
    
    Y_test_he = None
    if datos_terminal and ((Y_test is not None) or (Y_pred is not None)):
        
        print("Y_pred redondeada")
        print(np.round(Y_pred,0))
        print()
        
        if one_hot_encoding:
            Y_test_he = Y_test
        else: 
            Y_test_he   = keras.utils.to_categorical(Y_test)
        
        print("Y_test")
        print(Y_test_he)
        print()
        
        print("Error")
        print(np.round(Y_pred,0)-Y_test_he)
        print()
        
        print("Y_pred")
        print(Y_pred)
        print()
        
        print("Y_pred.argmax")
        print(Y_pred.argmax(axis=1))
        print()
        
        
    Y_test_he = None
    if matriz_conf and ((Y_test is not None) or (Y_pred is not None)):
        
        if one_hot_encoding:
            mc.matriz_confusion(Y_test.argmax(axis=1),Y_pred.argmax(axis=1))
            
        else: 
            mc.matriz_confusion(Y_test,Y_pred.argmax(axis=1))
            
        
##################################################################################

fashion_mnist = keras.datasets.fashion_mnist

(X_train, Y_train), (X_test, Y_test) = fashion_mnist.load_data()

modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test = CNN(X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
                                                                                 epocas = 30, scaler = True, one_hot_encoding = True)


Y_pred = modelo.predict(X_test)

Info_CNN( X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
                scaler = True,
                resultados = resultados, matriz_conf = True, datos_terminal = True,
                Y_pred = Y_pred, one_hot_encoding = True)

###################################################################################
"""
(X_train, Y_train), (X_test, Y_test) = keras.datasets.cifar10.load_data()

modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test = CNN(X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
                                                                                 epocas = 30, scaler = True, one_hot_encoding = True, n_capas_CNN = 2, batch_dropout = False)


Y_pred = modelo.predict(X_test)

Info_CNN(X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
         scaler = True,
         resultados = resultados, matriz_conf = True, datos_terminal = True,
         Y_pred = Y_pred, one_hot_encoding = True)
"""