#include "RedNeuronal.h"

RedNeuronal::RedNeuronal()
{
    nCapas = 0;
    capas = 0;

    neuronasXCapa = 0;

    nEntradas = 0;
    entradas = 0;

    nSalidas = 0;
    salidas = 0;

    fa = 1;

    nIteracions = 10;

    maximoError = 0.1;
}

RedNeuronal::~RedNeuronal()
{

    for(int i = 0; i < nCapas; ++i)
        delete capas[i];
    delete []capas;

    for(int i = 0; i < neuronasXCapa[0]; ++i)
        delete []entradas[i];
    delete []entradas;

    for(int i = 0; i < neuronasXCapa[nCapas - 1]; ++i)
        delete []salidas[i];
    delete []salidas;

    delete []neuronasXCapa;

}

void RedNeuronal::ejecutar()
{
    int errores;
    bool retrocedo;
    int minimo = 70000;
    bool sigoEntrenando = true;

    for(int it = 0; it < nIteracions  && sigoEntrenando; ++it)
    {
        errores = 0;
        sigoEntrenando = false;
        for(int entr = 0; entr < nEntradas; ++entr)
        {
            if(entr % 1000 == 0) cout << "it:" << it << " entrada:" << entr << endl;
            retrocedo = false;
            copiarEntrada(entr);
            for(int cap = 0; cap < nCapas - 1; ++cap)
                avanzar(*capas[cap], *capas[cap + 1]);

            verificarCapaFinal(entr, retrocedo);
            if(retrocedo) ++ errores;//Cuenta las entradas q faltan ajustar

            if(!retrocedo) continue;
            sigoEntrenando = true;

            actualizarErrorCapaFinal(entr);

            for(int cap = nCapas - 2; cap > 0; --cap)
            {
                actualizarPeso(*capas[cap], *capas[cap + 1]);
                actualizarError(*capas[cap], *capas[cap + 1]);
            }

            actualizarPeso(*capas[0], *capas[1]);


        }

        cout << "it:" << it << " errores:" << errores << endl;

        if(errores < minimo) {
            minimo = errores;
            guardarPesos(minimo, it);
        }

        if(minimo < 10000) break;

    }
    cout << "el minimo fue:" << minimo << endl;
}

void RedNeuronal::verificarCapaFinal(int sal, bool &retrocedo)
{
    for(int neur = 0; neur < neuronasXCapa[nCapas - 1]; ++neur)
    {
        if(abs(salidas[sal][neur] - capas[nCapas - 1]->y[neur + 1]) > maximoError)
        {

            retrocedo = true;
            return;
        }
    }

}

RedNeuronal::RedNeuronal(int numeroCapas)
{
    nCapas = numeroCapas;
    neuronasXCapa = new int[nCapas];

    cout << "Numero de neuronas en la capa de entrada: ";
    cin >> neuronasXCapa[0];

    for(int i = 1; i < nCapas - 1; ++i){
        cout << "Numero de neuronas en la capa Oculta " << i << ": ";
        cin >> neuronasXCapa[i];
    }

    cout << "Numero de neuronas en la capa de salida: ";
    cin >> neuronasXCapa[nCapas - 1];

    capas = new Capa*[nCapas];

    //Inicializar Capas,excepto la ultima capa
    for(int i = 0; i < nCapas - 1; ++i){
        capas[i] = new Capa(neuronasXCapa[i] + 1, neuronasXCapa[i + 1] + 1);
    }

    capas[nCapas - 1] = new Capa(neuronasXCapa[nCapas - 1] + 1, 1);

    fa = 1.5;

    nIteracions = 1000;
    maximoError = 0.1;

    //Inicializar datos de prueba y salida
    inicializarIONumeros();

    //Inicializar umbrales
    inicializarUmbrales();

    inicializarPesos();
    //cargarPesosEntrenados();

    nHilos = 5;

}

void RedNeuronal::inicializarIONumeros()
{
    int filas, columnas;
    int nRegistros, neuronasEntradas, neuronasSalidas;

    FILE *leeTrain = fopen("train-images", "rb");
    FILE *leeLabel = fopen("train-labels", "rb");

    fread(&nRegistros, sizeof(nRegistros), 1, leeTrain);
    fread(&filas, sizeof(filas), 1, leeTrain);
    fread(&columnas, sizeof(columnas), 1, leeTrain);

    fread(&nRegistros, sizeof(nRegistros), 1, leeLabel);
    neuronasEntradas = filas * columnas;
    neuronasSalidas = 10;

    unsigned char *img = new unsigned char[filas * columnas];
    unsigned char salida;

    nEntradas = nRegistros;
    nSalidas = nRegistros;

    entradas = new double*[nRegistros];
    for(int i = 0; i < nRegistros; ++i)
        entradas[i] = new double[neuronasEntradas];

    salidas = new double*[nRegistros];
    for(int i = 0; i < nRegistros; ++i)
        salidas[i] = new double[neuronasSalidas];

    for(int i = 0; i < nRegistros; ++i)
    {
        fread(img, sizeof(unsigned char), neuronasEntradas, leeTrain);
        for(int im = 0; im < neuronasEntradas; ++im){
            if((int)img[im] > 110) entradas[i][im] = 1;
            else entradas[i][im] = 0;
        }

        fread(&salida, sizeof(unsigned char), 1, leeLabel);
        salidas[i][salida] = 1;
    }

    fclose(leeLabel);
    fclose(leeTrain);

    cout << "cargo todas la entradas " << endl;
}

void RedNeuronal::inicializarUmbrales()
{
    for(int i = 0; i < nCapas; ++i){
        capas[i]->u[0] = -1;
        capas[i]->y[0] = -1;
    }

}

void RedNeuronal::copiarEntrada(int pos)
{
    copy(entradas[pos], entradas[pos] + neuronasXCapa[0], capas[0]->y + 1);
}

double RedNeuronal::funcionActivacion(double u)
{
    int a;
    double rp = 1.0 /(1 + exp(-u));
    return rp;
    //return tanh(u);
}

double RedNeuronal::derivadaFuncion(double u)
{
    int a;
    double eu = exp(u);
    double rp = eu / pow(eu + 1, 2);
    return rp;
    //return 1.0 / pow(cosh(u), 2);
}

void RedNeuronal::avanzar(Capa &cOrigen, Capa &cDestino)
{
    vector <thread> hilos;
    int ini, fin, m = cDestino.m - 1;
    for(int i = 0; i < nHilos; ++i)
    {
        ini = (i * m / nHilos) + 1;
        fin = ((i + 1) * m / nHilos);
        hilos.push_back(thread(&RedNeuronal::avanzarHilo, this, ref(cOrigen), ref(cDestino), ini, fin));
    }

    for(int i = 0; i < nHilos; ++i)
        hilos[i].join();

    hilos.clear();
}

void RedNeuronal::avanzarHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin)
{
    double suma;
    for(int j = ini; j <= fin; ++j)
    {
        suma = 0;
        for(int i = 0; i < cOrigen.m; ++i)
            suma += cOrigen.y[i] * cOrigen.peso[i][j];

        cDestino.u[j] = suma;
        cDestino.y[j] = funcionActivacion(suma);
    }
}


void RedNeuronal::actualizarErrorCapaFinal(int sal)
{
    double dif;
    for(int neur = 0; neur < neuronasXCapa[nCapas - 1]; ++neur)
    {
        dif = salidas[sal][neur] - capas[nCapas - 1]->y[neur + 1];
        capas[nCapas - 1]->error[neur + 1] = dif * derivadaFuncion(capas[nCapas - 1]->u[neur + 1]);
    }
}

void RedNeuronal::actualizarPeso(Capa &cOrigen, Capa &cDestino)
{
    vector <thread> hilos;
    int ini, fin, m = cOrigen.m;
    for(int i = 0; i < nHilos; ++i)
    {
        ini = (i * m / nHilos);
        fin = ((i + 1) * m / nHilos);
        hilos.push_back(thread(&RedNeuronal::actualizarPesoHilo, this, ref(cOrigen), ref(cDestino), ini, fin));
    }

    for(int i = 0; i < nHilos; ++i)
        hilos[i].join();

}

void RedNeuronal::actualizarPesoHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin)
{
    for(int i = ini; i < fin; ++i)
        for(int j = 1; j < cDestino.m; ++j)
            cOrigen.peso[i][j] += (fa * cOrigen.y[i] * cDestino.error[j]);
}

void RedNeuronal::actualizarError(Capa &cOrigen, Capa &cDestino)
{

    vector <thread> hilos;
    int ini, fin, m = cOrigen.m - 1;
    for(int i = 0; i < nHilos; ++i)
    {
        ini = (i * m / nHilos) + 1;
        fin = ((i + 1) * m / nHilos);
        hilos.push_back(thread(&RedNeuronal::actualizarErrorHilo, this, ref(cOrigen), ref(cDestino), ini, fin));
    }

    for(int i = 0; i < nHilos; ++i)
        hilos[i].join();
}

void RedNeuronal::actualizarErrorHilo(Capa &cOrigen, Capa &cDestino, int ini, int fin)
{
    double suma;
    for(int i = ini; i <= fin; ++i)
    {
        suma = 0;
        for(int j = 1; j < cDestino.m; ++j)
            suma +=  (cOrigen.peso[i][j] * cDestino.error[j]);

        cOrigen.error[i] = derivadaFuncion(cOrigen.u[i]) * suma;

    }
}

void RedNeuronal::inicializarPesos()
{
    srand(time(0));
    for(int cap = 0; cap < nCapas - 1; ++cap)
    {
        for(int m = 0; m < capas[cap]->m; ++m)
            for(int n = 0; n < capas[cap]->n; ++n)
                capas[cap]->peso[m][n] = 1.0 * (rand() % 2001 - 1000) / 1000;
    }
}

void RedNeuronal::cargarPesosEntrenados()
{
    int aux, m, n;

    FILE *lee = fopen("pesos(opcion1)", "rb");

    fread(&aux, sizeof(aux), 1, lee);
    cout << "nerror " << aux<< endl;
    fread(&aux, sizeof(aux), 1, lee);
    cout << "it " << aux<< endl;
    fread(&aux, sizeof(aux), 1, lee);
    cout << "capas " << aux<< endl;

    for(int cap = 0; cap < nCapas - 1; ++cap)
    {
        fread(&m, sizeof(m), 1, lee);
        fread(&n, sizeof(n), 1, lee);
        for(int m = 0; m < capas[cap]->m; ++m)
            fread(capas[cap]->peso[m], sizeof(double), n, lee);

        cout << m << " " << n << endl;
        for(int m = 0; m < capas[cap]->m; ++m)
            for(int n = 0; n < capas[cap]->n; ++n)
                cout << capas[cap]->peso[m][n] << " ";

    }

    cout << "cargo todos los pesos" << endl;

    fclose(lee);
}

void RedNeuronal::guardarPesos(int nError, int iteracion)
{
    FILE *esc = fopen("pesosFinales", "wb");
    fwrite(&nError, sizeof(nError), 1, esc);
    fwrite(&iteracion, sizeof(iteracion), 1, esc);
    fwrite(&nCapas, sizeof(nCapas), 1, esc);
    for(int c = 0; c < nCapas - 1; ++c)
    {
        int m = neuronasXCapa[c]  + 1;
        int n = neuronasXCapa[c + 1] + 1;
        fwrite(&m, sizeof(m), 1, esc);
        fwrite(&n, sizeof(n), 1, esc);

        for(int i = 0; i < m; ++i)
            fwrite(capas[c]->peso[i], sizeof(double), n, esc);
    }

    fclose(esc);
}
