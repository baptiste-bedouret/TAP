#include "tsp_brute_force.h"
#include <GL/glut.h>

//
//  TSP - BRUTE-FORCE
//
// -> la structure "point" est définie dans tools.h

double dist(point A, point B)
{
  double dx = A.x - A.y;
  double dy = A.y - B.y;
  return sqrt(dx * dx + dy * dy);
}

double value(point *V, int n, int *P)
{
  double w = 0;
  for (int i = 0; i < n; i++)
  {
    w += dist(V[P[i]], V[P[(i + 1) % n]]);
  }
  return w;
}

double tsp_brute_force(point *V, int n, int *Q)
{
  int P[n];
  for (int i = 0; i < n; i++)
  {
    P[i] = i;
  }
  double score_opt = value(V, n, Q);
  double score;
  memcpy(P, Q, n * sizeof(int));
  while (NextPermutation(Q, n))
  {
    score = value(V, n, Q);

    if (score < score_opt)
    {
      score_opt = score;
      memcpy(P, Q, n * sizeof(int));
    }
  }
  return score_opt;
}

void MaxPermutation(int *P, int n, int k)
{
  int tmp;
  for (int i = k; i < k + (n - k) / 2; i++)
  {
    tmp = P[i];
    P[i] = P[n - 1 - (i - k)];
    P[n - 1 - (i - k)] = tmp;
  }
  return;
}

/*double value_opt(point *V, int n, int *P, double w)
{
  double k = 0;
  double m = 0;
  for (int i = 0; i < n; i++)
  {
    m += dist(V[P[i]], V[P[(i + 1) % n]]);
    i = i + 1;
    while (m > w && i < n)
    {
      k = k + 1;
      m += dist(V[P[i]], V[P[(i + 1) % n]]);
    }
  }
  if (m < w)
  {
    return m;
  }
  else
  {
    return -k;
  }
}

double value_opt2(point *V, int n, int *P, double w)
{
  int res = 0;
  for (int i = 0; i < n - 1; i++)
  {
    res += dist(V[P[i]], V[P[(i + 1)]]);
    if (res + dist(V[P[0]], V[P[i]]) >= w)
    {
      return -i;
    }
  }
  res += dist(V[P[n - 1]], V[P[0]]);
  return res;
}*/

double value_opt(point *V, int n, int *P, double w)
{
  double value_totale = 0;
  for (int i = 0; i < n - 1; i++)
  {
    if (value_totale + dist(V[P[0]], V[P[i]]) >= w)
    {
      return (double)-(i + 1); // l'indice -k
    }
    value_totale += dist(V[P[i]], V[P[i + 1]]);
  }
  value_totale += dist(V[P[0]], V[P[n - 1]]);
  return value_totale;
}

double tsp_brute_force_opt(point *V, int n, int *Q)
{
  int P[n];
  for (int i = 0; i < n; i++)
  {
    P[i] = i;
  }
  double score_opt = value(V, n, P);
  double score_tmp;
  memcpy(Q, P, n * sizeof(int));
  while (NextPermutation(P, n))
  {
    score_tmp = value_opt(V, n, P, score_opt);

    if (score_tmp < 0)
    {
      int indice = (int)(score_tmp);
      MaxPermutation(P, n, -indice);
      score_tmp = score_opt;
    }
    if (score_tmp <= score_opt)
    {
      score_opt = score_tmp;
      memcpy(Q, P, n * sizeof(int));
    }
  }
  return score_opt;
}
