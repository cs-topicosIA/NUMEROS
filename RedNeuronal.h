#ifndef REDNEURONAL_H
#define REDNEURONAL_H

#include <iostream>
#include <algorithm>
#include <fstream>
#include <thread>
#include "Capa.h"

using namespace std;

class RedNeuronal
{
    public:
        RedNeuronal();
        RedNeuronal(int numeroCapas);
        ~RedNeuronal();
        void ejecutar();

    protected:

    private:
        int nCapas;
        int *neuronasXCapa;
        Capa **capas;
        double **entradas;
        double **salidas;
        int nEntradas, nSalidas;
        double fa;
        int nIteracions;
        double maximoError;
        double nHilos;

    private:
        void inicializarIONumeros();
        void inicializarUmbrales();
        void inicializarPesos();

        void cargarPesosEntrenados();

        void copiarEntrada(int pos);
        void guardarPesos(int nError, int iteracion);

    private:
        double funcionActivacion(double u);
        double derivadaFuncion(double u);

        void avanzar(Capa &cOrigen, Capa &cDestino);
        void avanzarHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin);
        void verificarCapaFinal(int sal, bool &cambio);
        void actualizarErrorCapaFinal(int sal);

        void actualizarPeso(Capa &cOrigen, Capa &cDestino);
        void actualizarPesoHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin);
        void actualizarError(Capa &cOrigen, Capa &cDestino);
        void actualizarErrorHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin);

};

#endif // REDNEURONAL_H
