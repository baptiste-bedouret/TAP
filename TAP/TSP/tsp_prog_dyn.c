#include "tools.h"
#include "tsp_prog_dyn.h"
#include "tsp_brute_force.h"

//
//  TSP - PROGRAMMATION DYNAMIQUE
//
// -> compléter uniquement tsp_prog_dyn()
//

/* renvoie l'ensemble S\{i} */
int DeleteSet(int S, int i) { return S & ~(1 << i); }

/* une cellule de la table */
typedef struct
{
  double length; // longueur du chemin minimum D[t][S]
  int pred;      // point précédant t dans la solution D[t][S]
} cell1;

int ExtractPath(cell1 **D, int t, int S, int n, int *Q)
{
  /*
    Construit dans Q le chemin extrait de la table D depuis t (la case
    D[t][S]) jusqu'au point V[n-1]. Il faut que Q[] soit assez grand.
    Renvoie la taille du chemin construit.
  */
  if (D[0][1].length < 0)
    return 0; // si D n'a pas été remplie

  Q[0] = t;  // écrit le dernier point
  int k = 1; // k=taille de Q=nombre de points écrits dans Q
  while (Q[k - 1] != n - 1)
  { // on s'arrête si le dernier point est V[n-1]
    Q[k] = D[Q[k - 1]][S].pred;
    S = DeleteSet(S, Q[k - 1]);
    k++;
  }
  return k;
}

double tsp_prog_dyn(point *V, int n, int *Q)
{
  /*
    Version programmation dynamique du TSP. La tournée optimale
    calculée doit être écrite dans la permutation Q, tableau qui doit
    être alloué par l'appelant. La fonction doit renvoyer aussi la
    valeur de la tournée Q ou 0 s'il y a eut un problème, comme la
    pression de 'q' pour sortir de l'affichage.
    
    La table D est un tableau 2D de "cell1" indexé par t ("int"),
    l'indice d'un point V[t], et S ("int") représentant un ensemble
    d'indices de points.

    o D[t][S].length = longueur minimum d'un chemin allant de V[n-1] à
      V[t] qui visite tous les points d'indice dans S

    o D[t][S].pred = l'indice du point précédant V[t] dans le chemin
      ci-dessus de longueur D[t][S].length

    NB1: Ne pas lancer tsp_prog_dyn() avec n>31 car:
         o les entiers (int sur 32 bits) ne seront pas assez grands
           pour représenter tous les sous-ensembles;
         o pour n=32, il faudra environ n*2^n / 10^9 = 137 secondes sur
           un ordinateur à 1 GHz, ce qui est un peu long.
         o l'espace mémoire, le malloc() pour la table D, risque d'être
           problématique: 32*2^32*sizeof(cell1) représente déjà 1536 Go
           de mémoire.
         En pratique on peut monter facilement jusqu'à n=24 pour une
         dizaine de secondes de calcul.
 
    NB2: La variable globale "running" indique si l'affichage
         graphique est actif, la pression de 'q' la faisant passer à
         faux. L'usage de "running" permet à l'utilisateur de sortir
         des boucles de calcul lorsqu'elles sont trop longues. Donc
         dans la phase de test, pensez à ajouter "&& running" dans vos
         conditions de boucle pour quitter en court de route si c'est
         trop long.
  */

  //-------------------------------------------------------------
  // Phase 1: Déclaration de la table.
  //
  // Elle comportent (n-1)*2^(n-1) "cell1". NB: la colonne S=0
  // (l'ensemble vide) n'est pas utilisée.

  int const L = n - 1;  // L = nombre de lignes = indice du dernier point
  int const C = 1 << L; // C = nombre de colonnes

  cell1 **D = malloc(L * sizeof(cell1 *)); // L=n-1 lignes
  for (int t = 0; t < L; t++)
    D[t] = malloc(C * sizeof(cell1)); // C=2^{n-1} colonnes
  D[0][1].length = -1;                // pour savoir si la table a été remplie

  //-------------------------------------------------------------
  // Phase 2: Remplissage de la table.
  //
  // o Pour toutes les colonnes S faire ...
  //   o Pour chaque ligne t de D[][S] faire ...

  // Rappel de la formule pour remplir la table D:
  // si card(S)=1, alors D[t][S] = d(V[n-1], V[t]) avec S={t};
  // si card(S)>1, alors D[t][S] = min_x { D[x][S\{t}] + d(V[x], V[t]) }
  // avec t∈S et x∈S\{t}. NB: pour calculer T = S\{t}, poser
  // T=DeleteSet(S,t). On peut tester si t∈S aussi avec
  // DeleteSet(S,t).
  for (int S = 1; S < C; S++)
  {
    for (int t = 1; t < L; t++)
    {
      int T = DeleteSet(S, t);
      if (T == S)
      {
        continue;
      }
      if (T == 0)
      {
        D[t][S].length = dist(V[n - 1], V[t]);
        D[t][S].pred = n - 1;
      }
      else
      {
        for (int x = 0; x < L; x++)
        {
          if (DeleteSet(T, x) == T)
          {
            continue;
          }
          D[t][S].length = fmin(D[x][T].length + dist(V[x], V[t]), DBL_MAX);
          D[t][S].pred = x;
        }
      }
      int k = ExtractPath(D, t, S, n, Q);
      drawPath(V, n, Q, k);
    }
  }

  // Toujours dans la boucle, mais à la fin, lorsque le calcul de la
  // cellule D[t][S] est fait (c'est-à-dire les valeurs D[t][S].length
  // et D[t][S].pred correctement calcuées), vous pouvez afficher le
  // chemin Q obtenu de V[n-1] à V[t] à l'aide d'ExtractPath() puis de
  // drawPath() comme ci-dessous:
  //
  // int k = ExtractPath(D, t, S, n, Q);
  // drawPath(V, n, Q, k);
  //
  // Ici, c'est la fin des deux boucles. Le calcul de la table est
  // terminé.

  //-------------------------------------------------------------
  // Phase 3: Extraction de la tournée optimale.
  //
  // On notera w la longueur de la tournée optimale qui reste à
  // calculer. NB: si le calcul a été interrompu (pression de 'q'), il
  // faut renvoyer 0.

  double w = 0; // valeur par défaut
  int S = C - 1;
  int tmp = 0;
  double best_length = DBL_MAX;
  int best_point = -1;
  if (running)
  {
    // Calcul de la longueur w et extraction de la tournée Q optimale
    // (si 'q' n'a pas été pressée) à l'aide d'ExtractPath(...,Q).

    for (int t = 1; t < L; t++)
    {
      tmp = D[t][S].length + dist(V[t], V[n - 1]);
      if (tmp < best_length)
      {
        best_length = tmp;
        best_point = t;
      }
    }
    w = ExtractPath(D, best_point, S, n, Q) + dist(V[n - 1], V[best_point]);
  }

  //-------------------------------------------------------------
  // Phase 4: Valeur retour en libérant la table D.
  for (int t = 0; t < L; t++)
  {
    free(D[t]);
    free(D);
  }

  return w;
}
