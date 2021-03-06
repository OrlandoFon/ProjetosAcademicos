# -*- coding: utf-8 -*-
import numpy as np
import math
import matplotlib.pyplot as plt
#%% Variaveis globais
ang1 = 0
ang2 = 0

coord_x = 0
coord_y = 0

d1 = 7.5
d2 = 11

cur_ang1 = 0
cur_ang2 = 0

#%% Cinematica Direta
def direct_cinematic(t1, t2):
    global coord_x, coord_y
    global d1, d2
    
    t1 = math.radians(t1)
    t2 = math.radians(t2)

    a = np.array([[math.cos(t1+t2), math.sin(t1+t2), d1*math.cos(t1) + d2*math.cos(t1+t2)],
                  [math.sin(t1+t2), math.cos(t1+t2), d1*math.sin(t1) + d2*math.sin(t1+t2)],
                  [0, 0, 1]])

    b = [[0], [0], [1]]

    #Registra coordenadas
    coord_x = np.dot(a, b)[0]
    coord_y = np.dot(a, b)[1]
    
    return np.dot(a, b)

#%%  Util

#Abs Dif
def absdif(a, b):
    return (max(a, b) - min(a,  b))
   
#Aplica limite
def lim(a, b, c):
    if a >= b and a <= c:
        return a
    else:
        return 999

#Exibe Estudo de Movimento       
def show():
    plt.figure()
    plt.title("Estudo de Movimento")
    plt.axis('equal')
    plt.xlim(0, 22)
    plt.ylim(0, 22)
    plt.plot(xs, ys, marker='.', linestyle='none')
    plt.show()
    
def show2():
    plt.figure()
    plt.title("Estudo de Movimento")
    plt.axis('equal')
    plt.xlim(0, 22)
    plt.ylim(0, 22)
    plt.plot(xs, ys, marker='.', linestyle='none', color='r')
    plt.show()
    
def show3():
    plt.plot(xs, ys, marker='.', linestyle='none', color='b')
    
#Interpolação linear nos eixos X e Y
def lerp(x, x0, x1, y0, y1):
    return y0 + (y1 - y0)* ((x - x0)/(x1 - x0))
#%% Cinematica Inversa
def inverse_cinematic(x, y):
    #variaveis Globais
    global ang1, ang2
    global d1, d2
    global cur_ang1, cur_ang2
    
    t2a = 0
    t2b = 0
    
    #Valores possíveis para Theta1
    try:
        t2a = (math.acos(((x**2 + y**2 - d1**2 - d2**2)/(2*d1*d2))))
        t2b = -t2a
    except:
        print "Valor fora do work envelope"
        print "x = " + str(x) + ", " + "y = " + str(y) + "."
        
    #Valores possíveis para Theta2
    t1a = 0
    t1b = 0
    try:
        t1a = math.atan2((y*(d1 + d2*math.cos(t2a)) - x*d2*math.sin(t2a)), (x*(d1+d2*math.cos(t2a)) + y*(d2*math.sin(t2a))));
        t1b = math.atan2((y*(d1 + d2*math.cos(t2b)) - x*d2*math.sin(t2b)), (x*(d1+d2*math.cos(t2b)) + y*(d2*math.sin(t2b))));
    except:
        print "Valor fora do work envelope"
        print "x = " + str(x) + ", " + "y = " + str(y) + "."
    
    #Converte valores para graus e adapta os eixos
    t2a = math.degrees(t2a) + 90
    t2b = math.degrees(t2b) + 90
    t1a = math.degrees(t1a)
    t1b = math.degrees(t1b)

    #Para uma situaçao com leitura da posição atual, 
    #colocar o angulo atual aqui
    dif1 = absdif(90, t1a)
    dif2 = absdif(90, t1b)

    #Aplica limites aos angulos
    t2a = lim(t2a, 0, 180)
    t2b = lim(t2b, 0, 180)
    t1a = lim(t1a, 0, 180)
    t1b = lim(t1b, 0, 180)
    
    #Condições normais
    if   dif1 > dif2 and t1b != 999 and t2b != 999:
        ang1 = t1b
        ang2 = t2b
    elif dif2 > dif1 and t1a != 999 and t2a != 999:
        ang1 = t1a
        ang2 = t2a

    #Condições especiais
    elif (dif1 > dif2) and ((t1b == 999 or t2b == 999) and (t1a != 999 and t2a != 999)):
        ang1 = t1a;
        ang2 = t2a;
    elif (dif2 > dif1) and ((t1a == 999 or t2a == 999) and (t1b != 999 and t2b != 999)):
       ang1 = t1b;
       ang2 = t2b;

    #Nenhuma condição existente
    else:
        ang1 = 999
        ang2 = 999

    #Ajusta Ang2 para plotagem do grafico
    ang2 -= 90
    
    #Registra ultima posição
    cur_ang1 = ang1
    cur_ang2 = ang2
    
#%% Funções de Movimento

#Move dot - move para um ponto, ignorando trajeto
def moveD(x, y):
    inverse_cinematic(x, y)
    if ang1 != 999 and ang2 != 999:
        direct_cinematic(ang1, ang2)
        xs.append(coord_x)
        ys.append(coord_y)
        
        return [coord_x, coord_y]
    else:
        #print "Coordenada fora do work envelope"
        print "Erro para " + str(x) + ",  " + str(y)
        return [999, 999]
        
#Move Linear - Move linearmente do ponto atual até um ponto especifico
def moveL(x, y, dots, amount):
    global xs, ys
    global coord_x, coord_y
    global cur_ang1, cur_ang2
    
    interpol = 0
    
    #Adquire posicao inicial
    direct_cinematic(cur_ang1, cur_ang2)
    start_x = np.round(coord_x)
    start_y = np.round(coord_y)
    
    #Diferenças entre a coordenada atual e o alvo
    x_dif = absdif(start_x, x)
    y_dif = absdif(start_y, y)
    
    
    x_dif_sign = np.sign(x - start_x) 
    value_x = x_dif/dots
    y_dif_sign = np.sign(y - start_y) 
    value_y = y_dif/dots
    
    #Verifica tipo de interpolação a ser utilizada
    if   x_dif != 0 and y_dif != 0:
        #Interpolação comum
        interpol = 1
    elif x_dif == 0 and y_dif != 0:
        #Interpolação no eixo Y
        interpol = 2
    elif y_dif == 0 and x_dif != 0:
        #Interpolação no eixo X
        interpol = 3
    
    #Interpolação em Ambos Eixos
    if interpol == 1:
        i = 0
        while i < dots:
            direct_cinematic(cur_ang1, cur_ang2)
            next_y = lerp(coord_x + (value_x * x_dif_sign), start_x, x, start_y, y)
            moveD(coord_x + (value_x * x_dif_sign), next_y)
            i += 1
        return
        
    #Interpolação no eixo Y
    if interpol == 2:
        i = 0
        while i < dots:
            direct_cinematic(cur_ang1, cur_ang2)
            moveD(coord_x, coord_y + (value_y * y_dif_sign))
            i += 1
        return
        
    #Interpolação no eixo X
    if interpol == 3:
        i = 0
        while i < dots:
            direct_cinematic(cur_ang1, cur_ang2)
            moveD(coord_x + (value_x * x_dif_sign), coord_y )
            i += 1
        return
#%%
xs = []
ys = []

for i in range(0, 30):
    for j in range(30):
        coord = moveD(i, j)
        if coord[0] != 999:
            xs.append(coord[0])
            ys.append(coord[1])
show2()

xs = []
ys = []

#Letra I
moveD(4, 17)
moveL(6, 17, 20, .1)
moveD(4, 13)
moveL(6, 13, 20, .1)
moveD(5, 17)
moveL(5, 13, 40, .1)

#Letra F
moveD(8, 15)
moveL(10, 15, 20, .1)
moveD(8, 11)
moveL(8, 15, 40, .1)
moveD(8, 13)
moveL(10, 13, 20, .1)

#Letra S
moveD(13, 12)
moveL(11, 12, 20, .1)
moveL(11, 10, 20, .1)
moveL(13, 10, 20, .1)
moveL(13, 8, 20, .1)
moveL(11, 8, 20, .1)

#Letra P
moveD(14, 3)
moveL(14, 7, 40, .1)
moveL(16, 7, 20, .1)
moveL(16, 5, 20, .1)
moveL(14, 5, 20, .1)

show3()


#%% Work Envelope
#Intervalos para angulos inteiros
'''
xs = []
ys = []

for i in range(180):
    for j in range(-90, 90):
        val = direct_cinematic(i, j)
        xs.append(val[0])
        ys.append(val[1])

plt.figure()
plt.title("Work Envelope - Scara")
plt.axis('equal')
plt.plot(xs, ys, marker='.') #, linestyle='none'
plt.show()
'''
#%% Todos os valores possiveis para precisão de 1cm 
'''
xs = []
ys = []

for i in range(-30, 30):
    for j in range(30):
        coord = moveD(i, j)
        if coord[0] != 999:
            xs.append(coord[0])
            ys.append(coord[1])
        
plt.figure()
plt.title("Work envelope - precisao de 1cm")
plt.axis('equal')
plt.xlim(-19, 19)
plt.ylim(0, 19)
plt.plot(xs, ys, marker='.', linestyle='none')
plt.show()
'''

