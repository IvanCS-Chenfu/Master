# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

def calcular_dia_semana(dia, mes, ano):
    
    if mes <= 2:
        A = mes + 10
        B = (ano - 1) % 100
        C = (ano -1) / 100
    else:
        A = mes - 2
        B = ano % 100
        C = ano / 100
    
    dia_semana = (700 + (26 * A -2) / 10 + dia + B + B/4 + C/4 - 2*C) % 7
    
    return dia_semana

def numero_de_dias(mes, ano):
    if mes == 2:
        if ano % 400 == 0 or (ano % 4 == 0 and not ano % 100 == 0):
            dias = 29
        else:
            dias = 28
    else:
        if mes in [1,3,5,7,8,10,12]:
            dias = 31
        else:
            dias = 30
            
    return dias

def mostrar_calendario(dia_0,n_dias,mes,ano):
    meses = ["Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"]
    print(meses[mes-1], "de:", ano)
    print ("Lu Ma Mi Ju Vi Sa Do") # (1 2 3 4 5 6 0)
    # (1 2 3 4 5 6 0) + 6 = (7 8 9 10 11 12 13)
    # (7 8 9 10 11 12 13) % 7 = (0 1 2 3 4 5 6)
    dia_0 = (dia_0 + 6) % 7
    dias_vacios = 0
    while dia_0 != dias_vacios:
        print(end = "   ")
        dias_vacios = dias_vacios + 1
    
    #(0 1 2 3 4 5 6) - 6 = (-6 -5 -4 -3 -2 -1 0)
    #(0 1 2 3 4 5 6) * (-1) = (6 5 4 3 2 1 0)
        
    dia_falta = (dia_0 - 6)*(-1)
    dia_i = 1
    
    while dia_falta >= 0:
        print("",dia_i,"", end ="")
        dia_falta = dia_falta - 1
        dia_i = dia_i + 1
    
    fin = False
    while not fin:
        print()
        dia, fin = mostrar_semana(dia_i,n_dias)
        dia_i = dia_i + 7

    
def mostrar_semana(dia,n_dias):
    semana = 0
    fin = False
    
    if dia + 7 > n_dias:
        semana = n_dias - dia + 1
        fin = True
    else:
        semana = 7
           
    i = 0
    for i in range(dia, dia + semana):
        if i in range(1, 10):
            print("",i,"",end ="")
        else:
            print(i,"",end ="")
            
    return i, fin

mes = 0
mes_bien = False

while not mes_bien:
    mes = int(input("Introduce mes (1-12): "))
    if mes in range(1, 13):
        mes_bien = True
    else:
        print("Mes no válido")
    
ano = 0
ano_bien = False

while not ano_bien:
    ano = int(input("Introduce año: "))
    if ano > 1752:
        ano_bien = True
    else:
        print("El año debe ser posterior a 1752")
                
dia_0 = int(calcular_dia_semana(1, mes, ano))
print(dia_0)
n_dias = numero_de_dias(mes, ano)
print(n_dias)
mostrar_calendario(dia_0,n_dias,mes,ano)
