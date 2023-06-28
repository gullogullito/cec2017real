extern "C"
{
#include "cec17.h"
}
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <limits.h>
#include <float.h>

using namespace std;

pair<int, int> genNuevaCelda(pair<int, int> pos, int movement);

vector<double> generaVecino(vector<double> sol, double fitness)
{
  int tam = sol.size();
  vector<double> sol_cambiada = sol;

  int r, s;

  r = rand() % tam;
  do
  {
    s = rand() % tam;
  } while (r == s);

  swap(sol_cambiada[r], sol_cambiada[s]);

  double coste = cec17_fitness(&sol_cambiada[0]);

  if (coste - fitness < 0)
  {
    return sol_cambiada;
  }

  return sol;
}

vector<double> BL(vector<double> sol, int iteraciones)
{

  for (int i = 0; i < iteraciones; i++)
  {
    double fitness = cec17_fitness(&sol[0]);
    sol = generaVecino(sol, fitness);
  }

  return sol;
}

// CLASE DE UN ABEJORRO, con su solucion asociada, su lifespan, su coste y su posicion
class BumbleBee
{ // Cada instancia será un abejorro de nuestra población

  vector<double> bumblebee; // solution
  double espVida;           // lifespan
  double coste;
  pair<int, int> posi;

public:
  BumbleBee() {}
  BumbleBee(vector<double> solution)
  {
    this->bumblebee = solution;
  }

  vector<double> getBumblebee() { return this->bumblebee; }

  double getCoste()
  {
    return coste;
  }

  void setBumblebee(vector<double> solution)
  {
    for (int i = 0; i < solution.size(); i++)
    {
      bumblebee[i] = solution[i];
    }
  }
  void setCoste(double c)
  {
    this->coste = c;
  }

  void setLifeSpan(double f)
  {
    if (f < 1000)
      this->espVida = f;
    else
      this ->espVida = 1000;
  }

  double getLifeSpan()
  {
    return this->espVida;
  }

  void cambiarEspVida(double f)
  {
    this->espVida += f;
  }

  void mutacion(float p, float q)
  {
    swap(this->bumblebee[p], this->bumblebee[q]);
  }

  void setPosicion(int p, int q)
  {
    this->posi = make_pair(p, q);
  }

  pair<int, int> getPosicion()
  {
    return this->posi;
  }

  BumbleBee &operator=(const BumbleBee &p)
  {
    if (this != &p)
    {
      this->posi = p.posi;
      this->bumblebee = p.bumblebee;
      this->coste = p.coste;
      this->espVida = p.espVida;
    }
  }
};

// ESTRUCTURA CELDA QUE GUARDA SI TIENE COMIDA Y SI ESTÁ OCUPADA
struct Celda
{
  int fruta;
  bool ocupada;

  Celda()
  {
    fruta = 0;
    ocupada = false;
  }
};

int main()
{
  int tipo = 0;
  cout << "INDIQUE LA PROPUESTA QUE QUIERE EVALUAR: (1-BB,  2-BB con hibridación, 3-BB con mejora)" << endl;
  cin >> tipo;
  vector<double> abejorro;
  int dim = 10;
  int seed = 42;
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  for (int funcid = 1; funcid <= 30; funcid++)
  {

    vector<vector<Celda>> nido(20); // Creamos las celdas

    cec17_init("BB", funcid, dim);

    for (int i = 0; i < 20; i++)
    {
      nido[i].resize(20);
    }

    for (int i = 0; i < 20; i++)
    {
      for (int j = 0; j < 20; j++)
      {
        nido[i][j].ocupada = false;
        nido[i][j].fruta = 0;
      }
    }

    for (int i = 0; i < 40; i++)
    {
      int r = rand() % 20, s = rand() % 20;
      nido[r][s].fruta = 5; // Ponemos 5 piezas de fruta en 40 casillas aleatorias
    }

    vector<BumbleBee> poblacion; // Creamos la colonia de abejorros

    std::mt19937 gen(seed); // Inicio semilla

    // CREAMOS POBLACION DE ABEJORROS Y LES ASIGNAMOS SU FITNESS Y SUS POSICIONES
    // FITNESS y POSICIONES
    vector<double> abejorro(dim);
    for (int i = 0; i < 200; i++)
    {
      for (int j = 0; j < dim; j++)
      {
        abejorro[j] = dis(gen);
      }
      poblacion.push_back(abejorro);

      int r, s;
      do
      {
        r = rand() % 20;
        s = rand() % 20;
      } while (nido[r][s].ocupada);

      poblacion[i].setPosicion(r, s); // Asignamos posiciones
      nido[r][s].ocupada = true;
    }

    // CALCULAMOS FITNESS (una evaluacion por cada llamada a cec_fitness) Y LOS GUARDAMOS
    vector<double> costes;
    double fitness = 0;
    for (int i = 0; i < poblacion.size(); i++)
    {
      fitness = cec17_fitness(&poblacion[i].getBumblebee()[0]);
      poblacion[i].setCoste(fitness);
      costes.push_back(fitness);
    }

    // BUSCAMOS LAS 5 MEJORES SOLUCIONES Y LAS GUARDAMOS
    double max1 = DBL_MAX, max2 = DBL_MAX, max3 = DBL_MAX, max4 = DBL_MAX, max5 = DBL_MAX;
    vector<BumbleBee> mejores;

    for (int i = 0; i < 5; i++)
    {
      BumbleBee b;
      mejores.push_back(b);
    }

    for (int j = 0; j < costes.size(); j++)
    {
      if (costes[j] < max1)
      {
        max5 = max4;
        max4 = max3;
        max3 = max2;
        max2 = max1;
        max1 = costes[j];
        mejores[4] = mejores[3];
        mejores[3] = mejores[2];
        mejores[2] = mejores[1];
        mejores[1] = mejores[0];
        mejores[0] = poblacion[j];
      }
      else if (costes[j] < max2)
      {
        max5 = max4;
        max4 = max3;
        max3 = max2;
        max2 = costes[j];
        mejores[4] = mejores[3];
        mejores[3] = mejores[2];
        mejores[2] = mejores[1];
        mejores[1] = poblacion[j];
      }
      else if (costes[j] < max3)
      {
        max5 = max4;
        max4 = max3;
        max3 = costes[j];
        mejores[4] = mejores[3];
        mejores[3] = mejores[2];
        mejores[2] = poblacion[j];
      }
      else if (costes[j] < max4)
      {
        max5 = max4;
        max4 = costes[j];
        mejores[4] = mejores[3];
        mejores[3] = poblacion[j];
      }
      else if (costes[j] < max5)
      {
        max5 = costes[j];
        mejores[4] = poblacion[j];
      }
    }

    // AHORA DEBEMOS HACER UN MAPEO DE LOS MEJORES VALORES DE LOS COSTES PARA ASIGNAR
    //  LOS LIFESPANS DE LOS DISTINTOS ABEJORROS

    for (int j = 0; j < costes.size(); j++)
    {
      int lifespan = (max1 / costes[j]) * 1000;

      if (lifespan < 0)
        lifespan = 0;

      poblacion[j].setLifeSpan(lifespan);
    }

    for (int i = 0; i < 5; i++)
    { // TAMBIEN LAS ASIGNAMOS A LAS 5 MEJORES SOLUCIONES
      int lifespan = (max1 / mejores[i].getCoste()) * 1000;

      if (lifespan < 0)
        lifespan = 0;

      poblacion[i].setLifeSpan(lifespan);
    }

    // cerr <<"Warning: output by console, if you want to create the output file you have to comment cec17_print_output()" <<endl;
    // cec17_print_output(); // Comment to generate the output file

    int evals = poblacion.size();
    bool extincion = false;
    int contador = 0;

    while (evals < 10000 * dim && !extincion)
    {

      // LO PRIMERO QUE DEBEMOS HACER ES BAJAR EN 1 LA ESPERANZA DE VIDA DE LOS ABEJORROS
      //  Y POR TANTO BORRAR TODOS LOS QUE "MUERAN"
      int k = 0;
      bool continuar = true;
      while (continuar)
      {
        poblacion[k].cambiarEspVida(-1);

        if (k == poblacion.size())
        {
          continuar = false;
        }
        if ((poblacion[k].getLifeSpan() <= 0) && continuar)
        {
          pair<int, int> posicion = poblacion[k].getPosicion();
          nido[posicion.first][posicion.second].ocupada = false;

          poblacion.erase(poblacion.begin() + k);
          costes.erase(costes.begin() + k);

          k--;
        }
        k++;
      }

      // CADA 40 GENERACIONES GENERAREMOS UN NUEVO ABEJORRO A PARTIR DE LAS MEJORES SOLUCIONES
      if (contador % 40 == 0) //
      {
        int r, s;
        int indice_aleatorio = rand() % 5;
        if (tipo == 2)
        {
          vector<double> generacionBL = BL(mejores[indice_aleatorio].getBumblebee(), 1000);
          evals += 2000;
          BumbleBee nueva = mejores[indice_aleatorio];
          nueva.setBumblebee(generacionBL);

          do
          {
            r = rand() % 20;
            s = rand() % 20;
          } while (nido[r][s].ocupada);

          nido[r][s].ocupada = true;
          nueva.setPosicion(r, s);
          poblacion.push_back(nueva);
          costes.push_back(mejores[indice_aleatorio].getCoste());
        }
        else
        {
          BumbleBee nueva = mejores[indice_aleatorio];
          do
          {
            r = rand() % 20;
            s = rand() % 20;
          } while (nido[r][s].ocupada);

          nido[r][s].ocupada = true;
          nueva.setPosicion(r, s);
          poblacion.push_back(nueva);
          costes.push_back(mejores[indice_aleatorio].getCoste());
        }
      }

      contador++;

      // AHORA LOS ABEJORROS DEBEN MOVERSE TODOS
      for (int i = 0; i < poblacion.size(); i++)
      {
        pair<int, int> celdaAntigua, celdaNueva;
        bool cont = false;

        while (!cont)
        {
          int mov = rand() % 24;
          celdaNueva = genNuevaCelda(poblacion[i].getPosicion(), mov);

          if (!nido[celdaNueva.first][celdaNueva.second].ocupada)
          {
            celdaAntigua = poblacion[i].getPosicion();
            nido[celdaAntigua.first][celdaAntigua.second].ocupada = false;
            poblacion[i].setPosicion(celdaNueva.first, celdaNueva.second);
            nido[celdaNueva.first][celdaNueva.second].ocupada = true;
            cont = true;
          }
        }

        if (nido[celdaNueva.first][celdaNueva.second].fruta > 0)
        {
          poblacion[i].cambiarEspVida(6);
          nido[celdaNueva.first][celdaNueva.second].fruta--;
        }
      }

      // UNA VEZ QUE TODAS SE HAN MOVIDO, DEBEN MUTAR
      for (int i = 0; i < poblacion.size(); i++)
      {
        if(tipo == 3){
          

        }
        else
        {
          float r = rand() % (dim - 1), s; // Mutamos dos genes aleatorios
          do
          {
            s = rand() % (dim - 1);
          } while (r == s);
          poblacion[i].mutacion(r, s);

          double fitness = cec17_fitness(&poblacion[i].getBumblebee()[0]);
          evals++;
          poblacion[i].setCoste(fitness);

          if (costes[i] > fitness)
          {
            poblacion[i].cambiarEspVida(2);
          }
          else
          {
            poblacion[i].cambiarEspVida(-1);
          }
        }
      }

      // POR ÚLTIMO DEBEMOS ACTUALIZAR TODAS LAS SOLUCIONES MEJORES
      for (int j = 0; j < costes.size(); j++)
      {
        if (costes[j] < max1)
        {
          max5 = max4;
          max4 = max3;
          max3 = max2;
          max2 = max1;
          max1 = costes[j];
          mejores[4] = mejores[3];
          mejores[3] = mejores[2];
          mejores[2] = mejores[1];
          mejores[1] = mejores[0];
          mejores[0] = poblacion[j];
        }
        else if (costes[j] < max2)
        {
          max5 = max4;
          max4 = max3;
          max3 = max2;
          max2 = costes[j];
          mejores[4] = mejores[3];
          mejores[3] = mejores[2];
          mejores[2] = mejores[1];
          mejores[1] = poblacion[j];
        }
        else if (costes[j] < max3)
        {
          max5 = max4;
          max4 = max3;
          max3 = costes[j];
          mejores[4] = mejores[3];
          mejores[3] = mejores[2];
          mejores[2] = poblacion[j];
        }
        else if (costes[j] < max4)
        {
          max5 = max4;
          max4 = costes[j];
          mejores[4] = mejores[3];
          mejores[3] = poblacion[j];
        }
        else if (costes[j] < max5)
        {
          max5 = costes[j];
          mejores[4] = poblacion[j];
        }
      }

      extincion = (poblacion.size() == 0); // ACABAMOS SI SE MUEREN TODOS LOS ABEJORROS
      if (extincion)
      {
        cout << "SE HAN EXTINGUIDO LOS ABEJORROS EN LA GENERACION " << contador << endl;
      }

    }

    contador = 0;

    cout << "Best BB[F" << funcid << "]: " << scientific << cec17_error(mejores[0].getCoste()) << endl;
  }
}

// UN ABEJORRO SE PUEDE MOVER A SUS 24 CASILLAS DE ALREDEDOR
pair<int, int> genNuevaCelda(pair<int, int> pos, int movement)
{
  pair<int, int> position = pos;
  switch (movement)
  {
  case 1:
    position.first += -2;
    position.second += -2;
    break;
  case 2:
    position.first += -2;
    position.second += -1;
    break;
  case 3:
    position.first += -2;
    break;
  case 4:
    position.first += -2;
    position.second += 1;
    break;
  case 5:
    position.first += -2;
    position.second += 2;
    break;
  case 6:
    position.first += -1;
    position.second += -2;
    break;
  case 7:
    position.first += -1;
    position.second += -1;
    break;
  case 8:
    position.first += -1;
    break;
  case 9:
    position.first += -1;
    position.second += 1;
    break;
  case 10:
    position.first += -1;
    position.second += 2;
    break;
  case 11:
    position.second += -2;
    break;
  case 12:
    position.second += -1;
    break;
  case 13:
    position.second += 1;
    break;
  case 14:
    position.second += 2;
    break;
  case 15:
    position.first += 1;
    position.second += -2;
    break;
  case 16:
    position.first += 1;
    position.second += -1;
    break;
  case 17:
    position.first += 1;
    break;
  case 18:
    position.first += 1;
    position.second += 1;
    break;
  case 19:
    position.first += 1;
    position.second += 2;
    break;
  case 20:
    position.first += 2;
    position.second += -2;
    break;
  case 21:
    position.first += 2;
    position.second += -1;
    break;
  case 22:
    position.first += 2;
    break;
  case 23:
    position.first += 2;
    position.second += 1;
    break;
  case 24:
    position.first += 2;
    position.second += 2;
    break;
  }

  if (position.first < 0)
  {
    position.first += 20;
  }
  if (position.second < 0)
  {
    position.second += 20;
  }

  position.first = position.first % 20;
  position.second = position.second % 20;
  return position;
}
