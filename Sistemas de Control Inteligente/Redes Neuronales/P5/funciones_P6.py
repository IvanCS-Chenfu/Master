# -*- coding: utf-8 -*-
"""
Created on Sat Jan  3 12:23:59 2026

@author: Chenfu
"""

import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def info(data):
    
    bucle = True
    
    while bucle:
        print()
        print("=" * 60)
        data.info()
        print("=" * 60)
    
        # Pedir número de columna
        print()
        eleccion = input("Elige una columna por número (0 a " + str(len(data.columns)-1) + ") o cualquier otra cosa para salir: ")
        print()
        
        try:
            eleccion = int(eleccion)
            
            if eleccion < 0 or eleccion >= len(data.columns):
                bucle = False
            else:
                col = data.columns[eleccion]
            
                print(data.value_counts(col))                       # Max y Min y Cantidad de cada
                print()
                print(data[col].nunique(), "valores distintos")     # N valores únicos
                print()
                print(data[col].describe())                         # Datos Estadísticos
                
                print()
                tecla = input("Pulsa ENTER para continuar (cualquier otra cosa para salir): ")
                print()
                
                if tecla != (""):
                    bucle = False
                    
        except ValueError:
            bucle = False
            
        
def info_col_relac(data):
    print()
    print("=" * 60)
    data.info()
    print("=" * 60)
    
    numeric = data.select_dtypes(include="number")

    corr = numeric.corr()
    
    annot = corr.copy()
    annot = annot.applymap(lambda x: f"{x:.2f}" if abs(x) > 0.5 else "")
    
    plt.figure(figsize=(8,6))
    sns.heatmap(corr, annot=annot, fmt="", cmap="coolwarm", linewidths=0.5)
    plt.title("Matriz de correlacion (de los atributos numéricos)")
    plt.show()
    

def tratar_nan_null_none(data):
    
    bucle = True
    
    while bucle:
        
        print()
        print("=" * 60)
        data.info()
        print("=" * 60)
        
        input()
        
        print()
        print("Suma de valores NaN por columna:")
        print(data.isna().sum())
        
        input()
        
        print()
        print("Porcentaje de valores NaN por columna (%):")
        print(data.isna().mean() * 100)
        
        input()
        
        plt.figure(figsize=(10,4))
        sns.heatmap(data.isnull(), cbar=False)
        plt.title('Mapa de valores perdidos')
        plt.show()
        
        borrar_colum = input("Introduce el número de la columna a borrar (0 a " + str(len(data.columns)-1) + ") si quieres borrar: ") 
        try:
            borrar_colum = int(borrar_colum)
            
            if not (borrar_colum < 0 or borrar_colum >= len(data.columns)):
                
                col = data.columns[borrar_colum]
                data.drop(columns=[col], inplace = True)
                print()
                print("Columna borrada:", col )
                print()
                
                plt.figure(figsize=(10,4))
                sns.heatmap(data.isnull(), cbar=False)
                plt.title('Mapa de valores perdidos')
                plt.show()
                    
        except ValueError:
            pass
        
        data = rellenar_columna(data)
        
        borrar_fila = input("¿Quieres borrar las filas con valores NaN? (s/S/y/Y): ")
        if borrar_fila in {"s", "S", "y", "Y"}:
            antes = len(data)
            data.dropna(inplace=True)
            despues = len(data)
            print()
            print("Filas eliminadas: " , antes - despues , "de" , antes , "a" , despues)
            print()
            
            
        print()
        print("=" * 60)
        data.info()
        print("=" * 60)
        
        plt.figure(figsize=(10,4))
        sns.heatmap(data.isnull(), cbar=False)
        plt.title('Mapa de valores perdidos')
        plt.show()
        
        print()
        tecla = input("Pulsa ENTER para continuar (cualquier otra cosa para salir): ")
        print()
        
        if tecla != (""):
            bucle = False
    
    return data
    

def tratar_duplicados(data):
    
    print()
    print(f'Registros duplicados: {data.duplicated().sum()}') 
    print(f'Tamaño del set antes de eliminar las filas repetidas: {data.shape}')
    
    borrar_fila = input("¿Quieres borrar filas duplicadas? (s/S/y/Y): ")
    
    if borrar_fila in {"s", "S", "y", "Y"}:
        data.drop_duplicates(inplace=True)
        print(f'Tamaño del set después de eliminar las filas repetidas: {data.shape}')
    
    print()
    return data
    
def tratar_outliers(data):
    
    bucle = True
    
    while bucle:
        print()
        print("=" * 60)
        data.info()
        print("=" * 60)
    
        # Pedir número de columna
        print()
        eleccion = input("Elige una columna por número (0 a " + str(len(data.columns)-1) + ") o cualquier otra cosa para salir: ")
        print()
        
        try:
            eleccion = int(eleccion)
            
            if eleccion < 0 or eleccion >= len(data.columns):
                bucle = False
            else:
                col = data.columns[eleccion]
                
                if np.issubdtype(data[col].dtype, np.number):
                    sns.boxplot(x=col, data=data)             
                    plt.show()
                    
                    borrar_outliers = input("¿Quieres borrar Outliers? (s/S/y/Y): ")
                    
                    if borrar_outliers in {"s", "S", "y", "Y"}:
                        
                        a = int(input("Introduce el número MÍNIMO a mantener: "))
                        b = int(input("Introduce el número MÁXIMO a mantener: "))
                        
                        print(f'Tamaño del set antes de eliminar registros de edad: {data.shape}')
                        
                        data = data[data[col]<=b]
                        data = data[data[col]>a]
                        
                        print(f'Tamaño del set después de eliminar registros de edad: {data.shape}')
                        
                        sns.boxplot(x=col, data=data)             
                        plt.show()
                        
                else:
                    print("La columna no es numérica")    
                    
                print()
                tecla = input("Pulsa ENTER para continuar (cualquier otra cosa para salir): ")
                print()
                
                if tecla != (""):
                    bucle = False
                    
        except ValueError:
            bucle = False
            
    return data
    

def tratar_textos(data):
    bucle = True
    
    while bucle:
        print()
        print("=" * 60)
        data.info()
        print("=" * 60)
    
        # Pedir número de columna
        print()
        eleccion = input("Elige una columna por número (0 a " + str(len(data.columns)-1) + ") o cualquier otra cosa para salir: ")
        print()
        
        try:
            eleccion = int(eleccion)
            
            if eleccion < 0 or eleccion >= len(data.columns):
                bucle = False
            else:
                col = data.columns[eleccion]
                
                if np.issubdtype(data[col].dtype, np.number):
                    
                    print("La columna es numérica") 
                    
                else:
                    
                    print()
                    sns.countplot(x=col, data=data)    
                    plt.xticks(rotation=270)
                    plt.show()
                    print(data[col].unique())
                    print()
                    
                    minuscula = input("¿Quieres poner los nombres en minúscula? (s/S/y/Y): ")
                    
                    if minuscula in {"s", "S", "y", "Y"}:

                       data[col] = data[col].str.lower()
                       
                       print()
                       sns.countplot(x=col, data=data)    
                       plt.xticks(rotation=270)
                       plt.show()
                       print(data[col].unique())
                       print()
                       
                      
                    unificar = input("¿Quieres unificar columnas? (s/S/y/Y): ")
                    
                    while unificar in {"s", "S", "y", "Y"}:

                        a = input("¿Qué columna quieres mantener? (escribe el texto): ")
                        b = input("¿Qué columna quieres escribir en "+ a +"? (escribe el texto): ")
                        
                        if b in a:
                            data.loc[data[col]==b,col] = a
                        else:
                            data[col] = data[col].str.replace(b,a, regex=False)
                        
                        print()
                        sns.countplot(x=col, data=data)    
                        plt.xticks(rotation=270)
                        plt.show()
                        print(data[col].unique())
                        print()
                        
                        unificar = input("¿Quieres unificar columnas? (s/S/y/Y): ")
                        
                        
                    dummy = input("¿Convertir Columna en Dummy? (s/S/y/Y): ")
                   
                    if dummy in {"s", "S", "y", "Y"}:
                       data[col] = data[col].astype(str).str.strip().str.capitalize()
                       data = pd.get_dummies(data, columns=[col], drop_first=True, dtype = int)
                        
                        
                print()
                tecla = input("Pulsa ENTER para continuar (cualquier otra cosa para salir): ")
                print()
                
                if tecla != (""):
                    bucle = False
                    
        except ValueError:
            bucle = False
            
    return data



def rellenar_columna(data):
    
    print()
    rellenar_columna = input("¿Quieres rellenar valores NaN con otros? (s/S/y/Y): ")
    print()
    
    if rellenar_columna in {"s", "S", "y", "Y"}:
        
        bucle = True
        
        while bucle:
            print()
            print("=" * 60)
            data.info()
            print("=" * 60)
            
            print()
            eleccion = input("Elige una columna por número (0 a " + str(len(data.columns)-1) + ") o cualquier otra cosa para salir: ")
            print()
            
            try:
                eleccion = int(eleccion)
                
                if not (eleccion < 0 or eleccion >= len(data.columns)):
                    
                    col = data.columns[eleccion]
                    
                    med_median_mod = input("Sustituir NaN por 'media' (med), 'mediana' (median) o 'moda' (mod): ")
                                           
                    if med_median_mod == "med":
                        data[col].fillna(data[col].mean(), inplace=True)
                    elif med_median_mod == "median":
                        data[col].fillna(data[col].median(), inplace=True)
                    elif med_median_mod == "mod":
                        data[col].fillna(data[col].mode()[0], inplace=True)
                else: 
                    bucle = False
                    
                    print()
                    tecla = input("Pulsa ENTER para continuar (cualquier otra cosa para salir): ")
                    print()
                    
                    if tecla != (""):
                        bucle = False
                        
            except ValueError:
                bucle = False
        
        plt.figure(figsize=(10,4))
        sns.heatmap(data.isnull(), cbar=False)
        plt.title('Mapa de valores perdidos')
        plt.show()
        
    return data



def guardar_data(data,nombre):
    
    print()
    guardar = input("¿Quieres guardar los datos en '" + nombre + "'? (s/S/y/Y): ")
    print()
    
    if guardar in {"s", "S", "y", "Y"}:
        data.to_csv(nombre + ".csv", index=False)
        print()
        print("Guardado")
        print()
