/*#include "heap.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
/*#define PERE(x) (x / 2)
#define FILSDROIT(x, n) (2 * (x) + 1 > (n)) ? 2 * (x) + 1 : 0
#define FILSGAUCHE(x, n) (2 * (x) > (n)) ? 2 * (x) : 0*/

/*#define PERE(x) ((x) / 2)
#define FILSDROIT(x, n) (2 * (x) + 1 > (n)) ? (2 * (x) + 1) : 0
#define FILSGAUCHE(x, n) (2 * (x) > (n)) ? (2 * (x)) : 0*/

/*heap heap_create(int k, int (*f)(const void *, const void *))
{
  heap h;
  h = malloc(sizeof(*h));
  assert(h != NULL);
  h->n = 0;
  h->nmax = k;
  h->f = f;
  h->array = malloc((k + 1) * sizeof(void *));
  return NULL;
}

void heap_destroy(heap h)
{
  if (h != NULL)
  {
    free(h);
    free(h->array);
  }
}

bool heap_empty(heap h)
{
  //assert(h != NULL);
  return (h->n == 0);
}

bool heap_add(heap h, void *object)
{

  if (h->n + 1 >= (h->nmax))
  {
    printf("Not enough space!");
    return true;
  }

  if (h->n == 0)
  {
    h->n = h->n + 1;
    h->array[h->n] = object;
    return false;
  }

  h->n = h->n + 1;
  int i = h->n;
  h->array[h->n] = object;
  void *tmp;
  while (h->f(h->array[i], h->array[PERE(i)]) == -1 && i != 1)
  {
    tmp = h->array[i];
    h->array[i] = h->array[PERE(i)];
    h->array[PERE(i)] = tmp;
    i = PERE(i);
  }
  return false;
}

void *heap_top(heap h)
{
  if (h->n == 0)
  {
    return NULL;
  }
  if (h->n == 1)
  {
    return h->array[1];
  }
  // Tu prends l'élément à  h->n, tu regardes si l'élément est plus petit que son père,
  // si oui, tu échanges les deux, si non, tu diminues de 1 dans le
  // tableau et tu répetes
  void *tmp;
  int i_tmp = h->n;
  int imax = h->n;
  while (h->f(h->array[i_tmp], h->array[PERE(i_tmp)]) == -1 && i_tmp != 1)
  {
    // j'échange imax et le pere de imax
    tmp = h->array[i_tmp];
    h->array[i_tmp] = h->array[PERE(i_tmp)];
    h->array[PERE(i_tmp)] = tmp;
    //i_tmp = PERE(i_tmp);
    imax = imax - 1;
    i_tmp = imax;
  }

  while ((h->f(h->array[i_tmp], h->array[PERE(i_tmp)]) == 1 || h->f(h->array[i_tmp], h->array[PERE(i_tmp)]) == 0) && i_tmp != 1)
  {
    imax = imax - 1;
    i_tmp = imax;
  }
  void *object;
  object = h->array[i_tmp - 1];
  return object;
}

// renvoie NULL si le tas est vide
// 1- on prend le dernier élément du tableau (h->n) et
// on l'ajoute en premiere place du tableau( à la place de la racine)
// 2-si l'objet est plus grand que ses deux fils on le remplace par son fils gauche
// si l'objet > filsgauche et <filsdroit alors on le remplace par son fils gauche
// si l'objet < filsgauche et >filsdroit alors on le remplace par son fils droit
// 3-on appelle heap_top pour renvoyer l'élément minimal à la racine
// pas besoin de retourner la racine  car il y a head_top()
void *heap_pop(heap h)
{

  if (h->n == 0)
  {
    return NULL;
  }
  if (h->n == 1)
  {
    return h->array[1];
  }
  // Étape 1
  int i = 1;
  /*int k = h->n;
  for (int k = h->n; k > 0; k--)
  {
  if (i == k)
  {
  h->array[i] = h->array[h->n];
  }
  }
  h->n--;

  h->array[i] = h->array[h->n];
  h->n--;

  //Étape 2
  void *tmp;
  int n;
  while ((h->f(h->array[i], h->array[FILSGAUCHE(i, n)]) == 1) && (h->f(h->array[i], h->array[FILSDROIT(i, n)]) == 1))
  {
    tmp = h->array[i];
    h->array[i] = h->array[FILSGAUCHE(i, n)];
    h->array[FILSGAUCHE(i, n)] = tmp;
    i = FILSGAUCHE(i, n);
  }
  while ((h->f(h->array[i], h->array[FILSGAUCHE(i, n)]) == 1) && (h->f(h->array[i], h->array[FILSDROIT(i, n)]) == -1))
  {
    tmp = h->array[i];
    h->array[i] = h->array[FILSGAUCHE(i, n)];
    h->array[FILSGAUCHE(i, n)] = tmp;
    i = FILSGAUCHE(i, n);
  }
  while ((h->f(h->array[i], h->array[FILSGAUCHE(i, n)]) == -1) && (h->f(h->array[i], h->array[FILSDROIT(i, n)]) == 1))
  {
    tmp = h->array[i];
    h->array[i] = h->array[FILSDROIT(i, n)];
    h->array[FILSDROIT(i, n)] = tmp;
    i = FILSDROIT(i, n);
  }
  // Étape 3
  void *object;
  object = heap_top(h);
  return object;
}*/

/* BROUILLON */

/*int i = h->n;
  void *tmp;
  while (h->f(h->array[i], h->array[PERE(i)]))
  {
    tmp = h->array[i];
    h->array[i] = h->array[PERE(i)];
    h->array[PERE(i)] = tmp;

  }
  return h->array[i];

  // échanger i et la racine

  int i = h->n;
  if (h->array[i] < h->array[1])
  {
    void *tmp;
    tmp = h->array[i];
    h->array[i] = h->array[1];
    h->array[1] = tmp;
  }

  // on regarde si nouvelle racine i est bien la plus petite
  int filsdroit;
  int filsgauche;
  int n = h->n;
  while (h->f(h->array[i], h->array[FILSGAUCHE(i, n)]) ||
         h->f(h->array[i], h->array[FILSDROIT(i, n)]))
  {
  }*/