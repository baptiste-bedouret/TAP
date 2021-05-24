#include "tools.h"
#include "tsp_heuristic.h"
#include "tsp_brute_force.h"

//
//  TSP - HEURISTIQUES
//

void reverse(int *T, int p, int q)
{
  // Renverse la partie T[p]...T[q] du tableau T avec p<q si
  // T={0,1,2,3,4,5,6} et p=2 et q=5, alors le nouveau tableau T sera
  // {0,1, 5,4,3,2, 6}.
  /*int tmp;
  int n = (p + q) / 2;
  for (i = 0; i <= n; i++)
  {
    tmp = T[n - i];
    T[n - i] = T[i];
    T[i] = tmp;
  }
}*/

  int i = p, j = q;
  while (i < j)
  {
    int tmp = T[j];
    T[j] = T[i];
    T[i] = tmp;
    i++;
    j--;
  }
}

double first_flip(point *V, int n, int *P)
{
  // Renvoie le gain>0 du premier flip réalisable, tout en réalisant
  // le flip, et 0 s'il n'y en a pas.
  double gain = 0;
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = 0; j < i + 2; j++)
    {
      gain = dist(V[P[i]], V[P[j]]) + dist(V[P[i + 1]], V[P[j + 1]]) - dist(V[P[i]], V[P[i + 1]]) + dist(V[P[j]], V[P[j + 1]]);
      if (gain < 0)
      {
        reverse(P, i + 1, j);
        return -1 * gain;
      }
    }
  }
  return 0.0;
}

double tsp_flip(point *V, int n, int *P)
{
  // La fonction doit renvoyer la valeur de la tournée obtenue. Pensez
  // à initialiser P, par exemple à P[i]=i. Pensez aussi faire
  // drawTour() pour visualiser chaque flip.
  for (int i = 0; i < n; i++)
  {
    P[i] = i;
    while (first_flip(V, n, P) && running)
    {
      drawTour(V, n, P);
    }
    return value(V, n, P);
  }

  return 0.0;
}

double tsp_greedy(point *V, int n, int *P)
{
  // La fonction doit renvoyer la valeur de la tournée obtenue. Pensez
  // à initialiser P, par exemple à P[i]=i.
  for (int i = 0; i < n; i++)
  {
    P[i] = i;
  }

  int index_tmp = 0;
  double val = DBL_MAX;
  double value = 0.0;
  for (int i = 0; i < n - 1; i++)
  {
    val = DBL_MAX;
    for (int j = i + 1; j < n; j++)
    {
      if (dist(V[P[i]], V[P[j]]) < val)
      {
        val = dist(V[P[i]], V[P[j]]);
        //index_tmp=j;
      }
      index_tmp++;
    }
    //switch i+1 et index_tmp
    int tmp = P[i + 1];
    P[i + 1] = P[index_tmp];
    P[index_tmp] = tmp;
    value = val + 1;
  }

  value += dist(V[P[0]], V[P[n - 1]]);
  return value;
}
