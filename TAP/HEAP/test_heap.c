/*
   test_heap.c

   Permet de tester les fonctions de heap.c en remplissant un tas avec
   des éléments aléatoires d'un type donné, puis de les extraire en
   vérifiant qu'ils apparaissent dans l'ordre croissant. Plusieurs
   types et fonctions de comparaisons peuvent être testés.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "heap.h"

#define BAR "-"        // un tiret
#define MAXSTR 7       // taille max d'une string
#define xstr(s) str(s) // permet l'expansion d'une macro
#define str(s) #s      // Ex: scanf("%"xstr(DMAX)"s",buffer);

typedef int (*fcmp)(const void *,
                    const void *); // type fonction de comparaison
typedef char *string;              // type chaîne de caractères
typedef struct
{
  double x, y;
} point; // type point

////////////////////////////////////////////////////

int fcmp_int(const void *x, const void *y) { return *(int *)x - *(int *)y; }

int fcmp_char(const void *x, const void *y) { return *(char *)x - *(char *)y; }

int fcmp_double(const void *x, const void *y)
{
  const double a = *(double *)x;
  const double b = *(double *)y;
  return (a < b) ? -1 : (a > b); // ou encore return (a>b) - (a<b);
}

int fcmp_double2(const void *x, const void *y)
{
  return *(double *)x - *(double *)y; // incorecte !
}

int fcmp_string(const void *x, const void *y)
{
  return strcmp(*(string *)x, *(string *)y);
}

int fcmp_pointx(const void *p, const void *q)
{
  return fcmp_double(&(((point *)p)->x), &(((point *)q)->x));
}

////////////////////////////////////////////////////

enum
{ // pour les switch()
  INT = 0,
  CHAR,
  DOUBLE,
  DOUBLE2,
  STRING,
  POINT,
};

int size[] = {
    // tableau des tailles des types
    sizeof(int),
    sizeof(char),
    sizeof(double),
    sizeof(double),
    sizeof(string),
    sizeof(point),
};

int psize[] = {
    // tableau des tailles d'affichage des types
    2,
    1,
    5,
    5,
    MAXSTR,
    9,
};

fcmp cmp[] = {
    // tableau des fonctions de comparaison
    fcmp_int,
    fcmp_char,
    fcmp_double,
    fcmp_double2,
    fcmp_string,
    fcmp_pointx,
};

char *type[] = {
    // tableau des types pour l'aide
    "int",
    "integers in [0,100[ (default)",
    "char",
    "capital letters in ['A','Z'[",
    "double",
    "real numbers in ]-1.00,+1.00[",
    "double2",
    "same as 'double' but with 'x-y' like fcmp()",
    "string",
    "strings of length at most " xstr(MAXSTR),
    "point",
    "real points of [0,9.9]×[0,9.9] with 'x' like fcmp()",
};

////////////////////////////////////////////////////

// affiche T[i] avec le bon format suivant le type t
void print(int t, void *T, int i)
{
  switch (t)
  {
  case INT:
    printf("%02i ", ((int *)T)[i]);
    break;
  case CHAR:
    printf("%c ", ((char *)T)[i]);
    break;
  case DOUBLE:
  case DOUBLE2:
    printf("%+1.2lf ", ((double *)T)[i]);
    break;
  case STRING:
    printf("%s ", ((string *)T)[i]);
    break;
  case POINT:
    printf("(%.1lf,%.1lf) ", ((point *)T)[i].x, ((point *)T)[i].y);
    break;
  }
}

// initialisation aléatoire de l'élément T[i]
void init(int t, void *T, int i)
{
  switch (t)
  {
  case INT:
  { // entiers aléatoires dans [0,100[
    ((int *)T)[i] = random() % 100;
  }
  break;

  case CHAR:
  { // lettres majuscules aléatoires
    ((char *)T)[i] = 'A' + (random() % ('Z' - 'A' + 1));
  }
  break;

  case DOUBLE:
  case DOUBLE2:
  { // double aléatoires dans [-1,+1] avec 2 chiffres
    ((double *)T)[i] = ((random() % 2000) - 1000) / 1000.0;
  }
  break;

  case STRING:
  {                                         // construit une chaîne aléatoire d'au plus MAXSTR char
    const int k = 1 + random() % MAXSTR;    // k=1..MAXSTR
    string s = calloc(k + 1, sizeof(char)); // remplit de '\0'
    for (int i = 0; i < k; i++)
      s[i] = 'a' + (random() % ('z' - 'a' + 1));
    ((string *)T)[i] = s;
  }
  break;

  case POINT:
  { // point aléatoire de [0,10[ x [0,10[ avec 1 chiffre
    ((point *)T)[i].x = random() % 100 / 10.0;
    ((point *)T)[i].y = random() % 100 / 10.0;
  }
  break;
  }
}

////////////////////////////////////////////////////

// affiche n fois le même la chaîne s
void rule(int n, string s)
{
  for (int i = 0; i < n; i++)
    printf("%s", s);
}

// affiche le contenu du tas h d'éléments de type t
void print_heap(heap h, int t)
{
  rule(psize[t], BAR);
  printf("\n");

  if (h == NULL)
  {
    printf("heap = NULL\n\n");
    return;
  }

  int i, k = 1;
  int n = 0; // n=nombre d'éléments affichés
  int p = 0; // p=hauteur

  while (n < h->n)
  {
    k = (1 << p);
    for (i = 0; (i < k) && (n < h->n); i++, n++)
      print(t, h->array[n + 1], 0);
    printf("\n");
    p++;
  }

  for (i = 0; i < k; i++)
  {
    rule(psize[t], BAR);
    printf(" ");
  }
  printf("\n\n");
}

////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  int i, t, r;
  int ntype = sizeof(cmp) / sizeof(*cmp);      // nombre de types possibles
  int n = (argc >= 2) ? atoi(argv[1]) : -1;    // n = nombre d'éléments
  char *s = (argc >= 3) ? argv[2] : type[INT]; // t = type des éléments

  for (t = 0; t < ntype; t++)
    if (!strcmp(s, type[2 * t]))
      break; // type trouvé !
  if (n < 0 || t == ntype)
  { // erreur
    printf("\n Usage: %s n [t]", argv[0]);
    printf("\n   Ex.: %s 10 int\n\n", argv[0]);
    printf("    n = number of random elements\n");
    printf("    t = type of elements & fcmp():\n\n");
    for (t = 0; t < ntype; t++)
    {
      printf("        '%s' ", type[2 * t]);
      rule(12 - strlen(type[2 * t]), ".");
      printf(" %s\n", type[2 * t + 1]);
    }
    printf("\n");
    exit(1);
  }

  unsigned seed = time(NULL) % 1000;
  srandom(seed);
  printf("\nseed: %u\n", seed); // pour rejouer la même chose au cas où

  void *T = malloc(n * size[t]); // tableau initial
  void *S = malloc(n * size[t]); // tableau final supposé trié

  printf("heap with %i elements of type '%s'\n", n, type[2 * t]);
  printf("(%s)\n", type[2 * t + 1]);

  // affichage et création de T[]
  printf("input array: ");
  for (i = 0; i < 10; i++)
  {
    ((char *)T)[0] = 'M';
    ((char *)T)[1] = 'Q';
    ((char *)T)[2] = 'U';
    ((char *)T)[3] = 'W';
    ((char *)T)[4] = 'N';
    ((char *)T)[5] = 'W';
    ((char *)T)[6] = 'I';
    ((char *)T)[7] = 'U';
    ((char *)T)[8] = 'Y';
    ((char *)T)[9] = 'B';
    print(t, T, 10);
  }
  printf("\n\n");

  // crée le tas avec la bonne fonction
  heap h = heap_create(n, cmp[t]);

  // crée le tas avec la bonne fonction
  heap h = heap_create(n, cmp[t]);

  // déplace les éléments de T[] vers h avec des add()
  for (i = 0; i < n && h; i++)
  {
    printf("insert ");
    print(t, T, i);
    printf("\n");
    if (heap_add(h, T + i * size[t])) // T+i*size[t] = &(T[i])
      break;
    print_heap(h, t);
  }

  // déplace les éléments de h vers S[] avec des pop()
  for (i = 0; i < n && h; i++)
  {
    memcpy(S + i * size[t], heap_pop(h), size[t]); // S[i] = heap_pop(h)
    printf("pop ");
    print(t, S, i);
    printf("\n");
    print_heap(h, t);
  }

  // libère le tas
  heap_destroy(h);

  // affichage du résultat S[] et vérifie qu'il est trié
  printf("output array: ");
  for (i = 0, r = 1; i < n && h; i++)
  {
    print(t, S, i); // affiche S[i]
    if (i > 0)
      r &= (cmp[t](S + (i - 1) * size[t], S + i * size[t]) <=
            0); // S[i-1] <= S[i] ?
  }
  if (h == NULL)
    r = 0;
  printf("\n%s! the above array is %ssorted\n\n", r ? "success" : "fail",
         r ? "" : "not ");

  // libération mémoire
  if (t == STRING) // cas d'un tableau de string
    for (i = 0; i < n; i++)
      free(((string *)T)[i]); // inutile pour S car mêmes éléments que T
  free(T);
  free(S);

  return 0;
}
