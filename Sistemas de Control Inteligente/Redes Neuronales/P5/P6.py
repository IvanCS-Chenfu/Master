# -*- coding: utf-8 -*-
"""
Editor de Spyder

Este es un archivo temporal.
"""

import pandas as pd
import funciones_P6 as fp6
import seaborn as sns

"""
data = pd.read_csv("sueldos.csv")
#data = sns.load_dataset('titanic')

#fp6.info(data)
# education y education-num son iguales
"""


####################################################


"""
data = fp6.tratar_nan_null_none(data)
# borrar education-num por repetición con education (4)
# borrar occupation por repetición con workclass y education (6) 
# borrar relationship por repetición con marital-status (5)
# borrar fnlwgt porque no describe a la persona (sino a las personas que representa) (1)

data = fp6.tratar_duplicados(data)
fp6.guardar_data(data,"NaNTratados&Duplicados")
"""


####################################################


"""
data = pd.read_csv("NaNTratados&Duplicados.csv")
data = fp6.tratar_textos(data)
# convertir textos a dummy

data = fp6.tratar_duplicados(data)

fp6.guardar_data(data,"TextosTratados")
"""


####################################################


"""
data = pd.read_csv("TextosTratados.csv")
data = fp6.tratar_outliers(data)

fp6.guardar_data(data,"OutliersTratados")
"""


####################################################
from perceptron_multicapa import Clasificacion_Multi
from perceptron_multicapa import Info_Clas_Mult

from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.model_selection import train_test_split


data = pd.read_csv("OutliersTratados.csv")

fp6.info(data)

Y = data["income_>50k"] 
X = data.drop(columns = ["income_>50k"])


X_test_val, X_train, Y_test_val, Y_train = train_test_split(
    X, Y, test_size=0.7, shuffle=True)

X_val, X_test, Y_val, Y_test = train_test_split(
    X_test_val, Y_test_val, test_size=0.5, shuffle=True)


# No sé con cual está mejor, si con "one_hot" o sin.
modelo, resultados, X_train, Y_train, X_val, Y_val, X_test, Y_test, scaler = Clasificacion_Multi(X_train = X_train, Y_train = Y_train, X_val = X_val, Y_val = Y_val, X_test = X_test, Y_test = Y_test,
                                                                                                 epocas = 200, one_hot_encoding = False)

Y_pred = modelo.predict(X_test)

Info_Clas_Mult( X_train = X_train, Y_train = Y_train, X_val = X_val, Y_val = Y_val, X_test = X_test, Y_test = Y_test,
                scaler = scaler,
                resultados = resultados, matriz_conf = True, datos_terminal = True,
                Y_pred = Y_pred, one_hot_encoding = False)
