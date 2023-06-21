extern "C"
{
#include "cec17.h"
}
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

pair<int, int> genNuevaCelda(pair<int, int> pos, int movement);

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
    this->espVida = f;
  }

  double getLifeSpan()
  {
    return this->espVida;
  }

  void cambiarEspVida(double f)
  {
    espVida += f;
  }

  void mutacion(float p, float q)
  {
    float r = this->bumblebee[p];
    float s = this->bumblebee[q];

    float aux = r;
    r = s;
    s = aux;

    bumblebee[p] = s;
    bumblebee[q] = r;
  }

  void setPosicion(int p, int q)
  {
    this->posi = make_pair(p, q);
  }

  pair<int, int> getPosicion()
  {
    return this->posi;
  }
};

// ESTRUCTURA CELDA QUE GUARDA SI TIENE COMIDA Y SI ESTÁ OCUPADA
struct Celda
{
  int comida;
  bool ocupada;

  Celda()
  {
    comida = 0;
    ocupada = false;
  }
};

int main()
{
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

    for (int i = 0; i < 80; i++)
    {
      int r = rand() % 20, s = rand() % 20;
      nido[r][s].comida = 7; // Ponemos 5 piezas de fruta en 40 casillas aleatorias
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
      double fitness = cec17_fitness(&abejorro[0]);
      poblacion[i].setCoste(fitness); // Asignamos fitness

      int r, s;
      do
      {
        r = rand() % 20;
        s = rand() % 20;
      } while (nido[r][s].ocupada);

      poblacion[i].setPosicion(r, s); // Asignamos posiciones
      nido[r][s].ocupada = true;
    }

    // BUSCAMOS LAS 5 MEJORES SOLUCIONES Y LAS GUARDAMOS
    float max1 = 0.0, max2 = 0.0, max3 = 0.0, max4 = 0.0, max5 = 0.0;
    vector<BumbleBee> mejores;

    for (int i = 0; i < 5; i++)
    {
      BumbleBee b;
      mejores.push_back(b);
    }

    for (int j = 0; j < poblacion.size(); j++)
    {
      if (poblacion[j].getCoste() > max1)
      {
        max5 = max4;
        max4 = max3;
        max3 = max2;
        max2 = max1;
        max1 = poblacion[j].getCoste();
        mejores[0] = poblacion[j];
      }
      else if (poblacion[j].getCoste() > max2)
      {
        max5 = max4;
        max4 = max3;
        max3 = max2;
        max2 = poblacion[j].getCoste();
        mejores[1] = poblacion[j];
      }
      else if (poblacion[j].getCoste() > max3)
      {
        max5 = max4;
        max4 = max3;
        max3 = poblacion[j].getCoste();
        mejores[2] = poblacion[j];
      }
      else if (poblacion[j].getCoste() > max4)
      {
        max5 = max4;
        max4 = poblacion[j].getCoste();
        mejores[3] = poblacion[j];
      }
      else if (poblacion[j].getCoste() > max5)
      {
        max5 = poblacion[j].getCoste();
        mejores[4] = poblacion[j];
      }
    }

    // AHORA DEBEMOS HACER UN MAPEO DE LOS MEJORES VALORES DE LOS COSTES PARA ASIGNAR
    //  LOS LIFESPANS DE LOS DISTINTOS ABEJORROS

    for (int j = 0; j < poblacion.size(); j++)
    {
      // Mappeo de valores (mejor, mejor/2) -> (100, 0)
      // El abejorro con el mejor coste tendrá 100 generaciones disponibles de lifespan
      // El peor tendrá 0
      //int lifespan = (poblacion[j].getCoste() - (mejores[0].getCoste()-1000)) * 100 / (mejores[0].getCoste() - (mejores[0].getCoste()-1000));
      double lifespan = 1;
      if (lifespan < 0)
        lifespan = 0;

      poblacion[j].setLifeSpan(lifespan);
    }

    // cerr <<"Warning: output by console, if you want to create the output file you have to comment cec17_print_output()" <<endl;
    // cec17_print_output(); // Comment to generate the output file

    int evals = poblacion.size();
    bool extincion = false;
    int contador = 0;
    cout << "funciones " << funcid << endl;

    while (evals < 10000 * dim && !extincion)
    {

      // LO PRIMERO QUE DEBEMOS HACER ES BAJAR EN 1 LA ESPERANZA DE VIDA DE LOS ABEJORROS
      //  Y POR TANTO BORRAR TODOS LOS QUE "MUERAN"
      int k = 0;
      bool continuar = true;
      cout << "a " << poblacion.size() << endl;
      while (continuar)
      {
        poblacion[k].cambiarEspVida(-1);

        if (k = poblacion.size())
          break;

        if (poblacion[k].getLifeSpan() <= 0)
        {
          poblacion.erase(poblacion.begin() + k);
          cout << "ha muerto una abeja " << endl;
          k--;
        }
        k++;
      }
      int eval40 = (10000 * dim - poblacion.size()) / poblacion.size() / 40;
      // CADA 40 GENERACIONES GENERAREMOS UN NUEVO ABEJORRO A PARTIR DE LAS MEJORES SOLUCIONES
      if (contador % eval40 == 1) //
      {
        int r, s;
        int indice_aleatorio = rand() % 5;
        BumbleBee nueva(mejores[indice_aleatorio].getBumblebee()); // Cogemos una de las 5 mejores
        nueva.setCoste(mejores[indice_aleatorio].getCoste());
        nueva.setLifeSpan(mejores[indice_aleatorio].getLifeSpan());
        do
        {
          r = rand() % 20;
          s = rand() % 20;
        } while (nido[r][s].ocupada);

        nido[r][s].ocupada = true;
        nueva.setPosicion(r, s);
        poblacion.push_back(nueva);
        contador++;
      }
      else
      {
        contador++;
      }

      // AHORA LOS ABEJORROS DEBEN MOVERSE TODOS
      for (int i = 0; i < poblacion.size(); i++)
      {
        pair<int, int> celdaAntigua, celdaNueva;
        bool valido = false;

        while(!valido){
          int mov = rand() % 24;
          celdaNueva = genNuevaCelda(poblacion[i].getPosicion(),mov);

          if(!nido[celdaNueva.first][celdaNueva.second].ocupada){
            celdaAntigua = poblacion[i].getPosicion();
            nido[celdaAntigua.first][celdaAntigua.second].ocupada = false;
            poblacion[i].setPosicion(celdaNueva.first, celdaNueva.second);
            nido[celdaNueva.first][celdaNueva.second].ocupada = true;
            valido = true;
          }
        }
      }

      // UNA VEZ QUE TODAS SE HAN MOVIDO, DEBEN MUTAR
      for (int i = 0; i < poblacion.size(); i++)
      {
        if (poblacion[i].getBumblebee().size() != 0)
        {
          float r = rand() % (dim - 1), s; // Mutamos dos genes aleatorios
          do
          {
            s = rand() % (dim - 1);
          } while (r == s);
          poblacion[i].mutacion(r, s);
        }
      }

      // DEBEMOS ACTUALIZAR LOS COSTES DE LOS ABEJORROS HABIENDO MUTADO Y CAMBIADO DE POSICION
      for (int i = 0; i < poblacion.size(); i++)
      {
        poblacion[i].setCoste(cec17_fitness(&poblacion[i].getBumblebee()[0])); // Asignamos fitness
      }
      evals += poblacion.size();

      // POR ÚLTIMO DEBEMOS ACTUALIZAR TODAS LAS SOLUCIONES MEJORES
      for (int j = 0; j < poblacion.size(); j++)
      {
        if (poblacion[j].getCoste() > max1)
        {
          max5 = max4;
          max4 = max3;
          max3 = max2;
          max2 = max1;
          max1 = poblacion[j].getCoste();
          mejores[0] = poblacion[j];
        }
        else if (poblacion[j].getCoste() > max2)
        {
          max5 = max4;
          max4 = max3;
          max3 = max2;
          max2 = poblacion[j].getCoste();
          mejores[1] = poblacion[j];
        }
        else if (poblacion[j].getCoste() > max3)
        {
          max5 = max4;
          max4 = max3;
          max3 = poblacion[j].getCoste();
          mejores[2] = poblacion[j];
        }
        else if (poblacion[j].getCoste() > max4)
        {
          max5 = max4;
          max4 = poblacion[j].getCoste();
          mejores[3] = poblacion[j];
        }
        else if (poblacion[j].getCoste() > max5)
        {
          max5 = poblacion[j].getCoste();
          mejores[4] = poblacion[j];
        }
      }
      
      extincion = (poblacion.size() == 0); // ACABAMOS SI SE MUEREN TODOS LOS ABEJORROS
      if (extincion){
        cout << "SE HAN EXTINGUIDO LOS ABEJORROS EN LA GENERACION " << contador << endl;
      }
    }

    cout << "Best BB[F" << funcid << "]: " << scientific << cec17_error(mejores[0].getCoste()) << endl;
  }
}


//UN ABEJORRO SE PUEDE MOVER A SUS 24 CASILLAS DE ALREDEDOR
pair<int, int> genNuevaCelda(pair<int, int> pos, int movement) {
    pair<int, int> position = pos;
    switch (movement) {
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

	if (position.first < 0) {
		position.first += 20;
	}
	if (position.second < 0) {
		position.second += 20;
	}
	
    position.first = position.first % 20;
    position.second = position.second % 20;
    return position;
}
