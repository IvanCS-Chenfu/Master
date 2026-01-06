# -*- coding: utf-8 -*-
"""
Created on Thu Dec 11 09:59:53 2025

@author: Chenfu
"""
import mis_cosas as mc
from tensorflow import keras
import numpy as np
from sklearn.preprocessing import StandardScaler, MinMaxScaler


def Clasificacion_Multi(dataset = None, 
                        X_train = None, Y_train = None, X_val = None, Y_val = None, X_test = None, Y_test = None,
                        porcentaje_train = 0.8, porcentaje_test = 0.0, 
                        scaler = None, n_capa_oculta = 8, epocas = 200,
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
        n_salida = 1
        
    if scaler is not None:
        X_escalado, X_train, X_val, X_test = mc.escalado(scaler, X, X_train, X_val, X_test)

    modelo = keras.Sequential([
        
        keras.layers.Dense(n_capa_oculta, activation="sigmoid", input_shape=[X.shape[1]]), 
        
        keras.layers.Dense(n_salida, activation="sigmoid")  
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
        modelo.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])
        
        
    if patience is not None:
        parada_temprana = keras.callbacks.EarlyStopping(patience=patience)
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val), callbacks=[parada_temprana])
    else:
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val))
        
    return modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scaler
        

def Regresion(dataset, porcentaje_train = 0.6, porcentaje_test = 0.2, 
                        scalerX = None, scalerY = None, learning_rate = None,
                        n_capa_oculta = 16, epocas = 200, patience = None):
    
    X = dataset.data
    Y = dataset.target
    
    X_train, Y_train, X_val, Y_val, X_test, Y_test = mc.split_all(dataset,porcentaje_train,porcentaje_test)
    
    
    if scalerX is not None:
        X_escalado, X_train, X_val, X_test = mc.escalado(scalerX, X, X_train, X_val, X_test)
    
    if scalerY is not None:
        Y_escalado, Y_train, Y_val, Y_test = mc.escalado(scalerY, Y, Y_train, Y_val, Y_test)
    
    
    modelo = keras.Sequential([
        
        keras.layers.Dense(n_capa_oculta, activation="relu", input_shape=[X.shape[1]]), 
        
        keras.layers.Dense(len(dataset.target_names), activation="linear")  
    ])
    
    
    # Accuracy solo cuenta el porcentaje de veces que y_pred = y_val
    if learning_rate is not None:
        modelo.compile(loss="mean_squared_error", optimizer=keras.optimizers.Adam(learning_rate=learning_rate))
    else:
        modelo.compile(loss="mean_squared_error", optimizer="adam") # metrics=[keras.metrics.MeanSquaredError()]
    
    
    if patience is not None:
        parada_temprana = keras.callbacks.EarlyStopping(patience=patience)
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val), callbacks=[parada_temprana])
    else:
        resultados = modelo.fit(X_train,Y_train, epochs = epocas, validation_data=(X_val,Y_val))
    
        
    return modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scalerX, scalerY


##################################################################################

def Info_Clas_Mult (    dataset = None,
                        X_train = None, Y_train = None, X_val = None, Y_val = None, X_test = None, Y_test = None,
                        n_dim_PCA = None, scaler_PCA = None, scaler = None,
                        resultados = None, matriz_conf = False, datos_terminal = False,
                        Y_pred = None, one_hot_encoding = False):
    
    print("\n" + "="*60)
    print("INFORMACIÓN")
    print("="*60)
    
    if scaler is not None:
        X_train = scaler.inverse_transform(X_train)
        X_val = scaler.inverse_transform(X_val)
        X_test = scaler.inverse_transform(X_test)

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
        
        
    if n_dim_PCA is not None:
        mc.red_dim(dataset,n_dim_PCA,scaler_PCA)
    
    
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
            y_prob = np.array(Y_pred).ravel()
            Y_pred = (y_prob >= 0.5).astype(int)
            mc.matriz_confusion(Y_test.ravel().astype(int) ,Y_pred)
            
        
def Info_Reg (    dataset = None,
                  X_train = None, Y_train = None, X_val = None, Y_val = None, X_test = None, Y_test = None,
                  resultados = None, histograma = False, datos_terminal = False,
                  scalerX = None, scalerY = None,
                  Y_pred = None):
    
    print("\n" + "="*60)
    print("INFORMACIÓN")
    print("="*60)
    
    if scalerX is not None:
        X_train = scalerX.inverse_transform(X_train)
        X_val = scalerX.inverse_transform(X_val)
        X_test = scalerX.inverse_transform(X_test)
        
    if scalerY is not None:
        Y_train = scalerY.inverse_transform(Y_train)
        Y_val = scalerY.inverse_transform(Y_val)
        Y_test = scalerY.inverse_transform(Y_test)
        Y_pred = scalerY.inverse_transform(Y_pred)
        
    if X_train is not None:
        print("X_train")
        mc.informacion_var(X_train)
    if Y_train is not None:
        print("Y_train")
        mc.informacion_var(Y_train)
    if X_val is not None:
        print("X_val")
        mc.informacion_var(X_val)
    if Y_val is not None:
        print("Y_val")
        mc.informacion_var(Y_val)
    if X_test is not None:
        print("X_test")
        mc.informacion_var(X_test)
    if Y_test is not None:
        print("Y_test")
        mc.informacion_var(Y_test)
    
    if resultados is not None:
        mc.grafica_epocas(resultados, "Evolución")
        
    if histograma:
        mc.histograma(dataset)
    
    if datos_terminal and ((Y_test is not None) or (Y_pred is not None)):
        
        print("Y_pred")
        print(Y_pred)
        print()
        
        print("Y_test")
        print(Y_test)
        print()
        
        print("Y_test.reshape")
        print(Y_test.reshape(-1, 1))
        print()
        
        print("Error")
        print(Y_pred-Y_test.reshape(-1, 1))
        print()
        
        print("Error Medio")
        print(np.mean(np.abs(Y_pred-Y_test.reshape(-1, 1))))
        print()   
        
        
#########################################################################    
"""
from sklearn.datasets import load_iris 

dataset = load_iris()

modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scaler = Clasificacion_Multi(dataset = dataset, one_hot_encoding = True,
                                                                                                 patience = 1, epocas = 300)
Y_pred = modelo.predict(X_val)

Info_Clas_Mult( dataset = dataset,
                X_train = X_train, Y_train = Y_train, X_val = X_val, Y_val = Y_val, X_test = X_test, Y_test = Y_test,
                n_dim_PCA = 2, scaler = scaler,
                resultados = resultados, matriz_conf = True, datos_terminal = True,
                Y_pred = Y_pred, one_hot_encoding = True)

"""
##########################################################################
"""
from sklearn.datasets import fetch_california_housing 
from sklearn.preprocessing import StandardScaler

dataset = fetch_california_housing()

modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scalerX, scalerY = Regresion(dataset = dataset, 
                                                                                                 scalerX = StandardScaler(), scalerY = StandardScaler(),
                                                                                                 patience = None, epocas = 200, learning_rate =0.01)

Y_pred = modelo.predict(X_test)

Info_Reg(dataset = dataset,
         X_train = X_train, Y_train = Y_train, X_val = X_val, Y_val = Y_val, X_test = X_test, Y_test = Y_test,
         resultados = resultados, histograma = True, datos_terminal = True,
         scalerX = scalerX, scalerY = scalerY,
         Y_pred = Y_pred)


"""
#############################################################################
"""
mnist = keras.datasets.mnist

(X_train, Y_train), (X_test, Y_test) = mnist.load_data()
X_train = mc.imagen2linea(X_train)
X_test = mc.imagen2linea(X_test)

mc.informacion_var(X_train)
mc.informacion_var(Y_train)
mc.informacion_var(X_test)
mc.informacion_var(Y_test)

# No sé con cual está mejor, si con "one_hot" o sin.
modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scaler = Clasificacion_Multi(X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
                                                                                                 epocas = 20, one_hot_encoding = True, scaler = MinMaxScaler())

Y_pred = modelo.predict(X_val)
Info_Clas_Mult( X_train = X_train, Y_train = Y_train, X_val = X_test, Y_val = Y_test, X_test = X_test, Y_test = Y_test,
                scaler = scaler,
                resultados = resultados, matriz_conf = True, datos_terminal = True,
                Y_pred = Y_pred, one_hot_encoding = True)
"""