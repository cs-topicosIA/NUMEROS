# NUMEROS

RESULTADOS

Iter Capas     Train    Test   Wmin Wmax FA NombreArchivo

21	784,25,10	91.20%	86.77%	0	1	1.5	pesos(opcion1)

Los archivos para entrenar y de prueba estan dentro de la carpeta "data" en .zip, se tiene que descomprimir. 

para compilar se necesita c++11, como mínimo.

g++ -o main RedNeuronal.cpp Capa.cpp main.cpp -std=c++11 -lpthread
