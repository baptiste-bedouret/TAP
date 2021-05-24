#include "heap.h"
#include <stdlib.h>

// macros
#define PERE(x) (x / 2)
#define FILSDROIT(x, n) (2 * (x) + 1 <= (n)) ? 2 * (x) + 1 : 0
#define FILSGAUCHE(x, n) (2 * (x) <= (n)) ? 2 * (x) : 0

heap heap_create(int k, int (*f)(const void *, const void *))
{
  heap h;
  h = malloc(sizeof(*h));
  h->n = 0;
  h->nmax = k;
  h->array = malloc((k + 1) * sizeof(void *));
  h->f = f;
  return h;
}

void heap_destroy(heap h)
{
  free(h->array);
  free(h);
}

bool heap_empty(heap h) { return (h->n == 0); }

void switchElements(heap h, int i1, int i2)
{
  void *tmp = h->array[i1];
  h->array[i1] = h->array[i2];
  h->array[i2] = tmp;
}

bool heap_add(heap h, void *object)
{
  if (h->n + 1 > h->nmax)
  {
    void *ptr;
    ptr = realloc(h->array, (2 * (h->nmax) + 1) * sizeof(void *));
    h->array = ptr;
    if (ptr == NULL)
      exit(EXIT_FAILURE);
    h->nmax = 2 * h->nmax;
    //return true;
  }
  h->n++;
  int index = h->n;
  h->array[h->n] = object;
  while (index > 1 && h->f(h->array[index], h->array[index / 2]) < 0)
  {
    switchElements(h, index, index / 2);
    index = index / 2;
  }
  return false;
}

void *heap_top(heap h)
{
  if (h->n == 0)
    return NULL;
  return h->array[1];
}

void *heap_pop(heap h)
{
  if (h == NULL || h->n == 0)
    return NULL;

  int indexFilsDroit;
  int indexFilsGauche;
  void *objectReturn = heap_top(h);
  switchElements(h, h->n, 1);
  h->n--;
  int n = h->n;

  // the added element is at index 1 (root of the heap)
  int indexElement = 1;
  indexFilsDroit = FILSDROIT(indexElement, n);
  indexFilsGauche = FILSGAUCHE(indexElement, n);
  int maxIndex = 0;

  while (indexFilsDroit != 0 && indexFilsGauche != 0)
  {
    maxIndex = (h->f(h->array[indexFilsGauche], h->array[indexFilsDroit]) < 0)
                   ? indexFilsGauche
                   : indexFilsDroit;
    // if array[indexElement] < array[maxIndex] -> return
    if (h->f(h->array[indexElement], h->array[maxIndex]) <= 0)
    {
      return objectReturn;
    }
    // switch the elements and update the new index
    switchElements(h, maxIndex, indexElement);
    indexElement = maxIndex;
    // update les nouveaux fils
    indexFilsDroit = FILSDROIT(indexElement, n);
    indexFilsGauche = FILSGAUCHE(indexElement, n);
  }
  // at this point h->array[indexElement] has {0 or 1} child .
  // case 0 child
  maxIndex = (indexFilsGauche == 0) ? indexFilsDroit : indexFilsGauche;
  if (maxIndex == 0)
  {
    return objectReturn;
  }
  // if array[indexElement] > array[maxIndex]
  // case 1 child
  if (h->f(h->array[indexElement], h->array[maxIndex]) > 0)
  {
    switchElements(h, maxIndex, indexElement);
  }
  return objectReturn;
}
