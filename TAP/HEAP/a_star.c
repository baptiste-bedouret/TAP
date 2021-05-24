#include "heap.h" // il faut aussi votre code pour heap.c
#include "tools.h"
#include "math.h"

//#define source_s 1
//#define source_t 0

enum
{
  SOURCE_S = 0,
  SOURCE_T,
};

// Une fonction de type "heuristic" est une fonction h() qui renvoie
// une distance (double) entre une position de départ et une position
// de fin de la grille. La fonction pourrait aussi dépendre de la
// grille (comme par exemple du nombre de murs rencontrés par le
// segment départ-fin), mais on n'utilisera pas forcément ce dernier
// paramètre. Vous pouvez définir votre propre heuristique.
typedef double (*heuristic)(position, position, grid *);

// Heuristique "nulle" pour Dijkstra.
double h0(position s, position t, grid *G) { return 0.0; }

// Heuristique "vol d'oiseau" pour A*.
double hvo(position s, position t, grid *G)
{
  return fmax(abs(t.x - s.x), abs(t.y - s.y));
}

double hec(position s, position t, grid *G)
{
  return sqrt((s.x - t.x) * (s.x - t.x) + (s.y - t.y) * (s.y - t.y));
}

// Heuristique "alpha x vol d'oiseau" pour A*.
static double alpha = 0; // 0 = h0, 1 = hvo, 2 = approximation ...
double halpha(position s, position t, grid *G) { return alpha * hvo(s, t, G); }

// Structure "noeud" pour le tas min Q.
typedef struct node
{
  position pos;        // position (.x,.y) d'un noeud u
  double cost;         // coût[u]
  double score;        // score[u] = coût[u] + h(u,end)
  struct node *parent; // parent[u] = pointeur vers le père, NULL pour start
  double nchild;
  //int SOURCE_S;
  //int SOURCE_T;
  int source;
} * node;

int nbAlloc = 0;
int nbDesalloc = 0;

int f(const void *n1, const void *n2)
{
  node N1 = (node)n1;
  node N2 = (node)n2;
  if (N1->score > N2->score)
  {
    return 1;
  }
  if (N1->score < N2->score)
  {
    return -1;
  }
  return 0;
}

// return true iff Node1 == Node2
bool compareNodePos(node Node1, position pos)
{
  if (Node1 == NULL)
  {
    exit(EXIT_FAILURE);
  }
  return Node1->pos.x == pos.x && Node1->pos.y == pos.y;
}

node createNode(position p, double cost, double score, node parent, int source)
{
  node n = malloc(sizeof(struct node));
  n->pos = p;
  n->cost = cost;
  n->score = score;
  n->parent = parent;
  n->nchild = 0;
  n->source = source;
  return n;
}

bool nodeFree(node n)
{
  if (n == NULL)
  {
    return false;
  }
  free(n);
  return true;
}

// return true if Node is a leaf
bool isLeaf(node Node) { return (Node->nchild == 0); }

// kills all the leafs
void freeNode(node Node)
{
  node tmp;
  while (Node != NULL && isLeaf(Node))
  {
    tmp = Node;
    Node = Node->parent;
    if (Node != NULL)
    {
      Node->nchild--;
    }
    free(tmp);
    nbDesalloc++;
  }
}

// Les arêtes, connectant les 8 cases voisines de la grille, sont
// valuées seulement par certaines valeurs. Le poids de l'arête u->v,
// noté w(u,v) dans le cours, entre deux cases u et v voisines est
// déterminé par la valeur de la case finale v. Plus précisément, si
// la case v de la grille contient la valeur C, le poids de u->v
// vaudra w(u,v) = weight[C] dont les valeurs numériques exactes sont
// définies ci-après. La liste des valeurs possibles d'une case est
// donnée dans "tools.h": V_FREE, V_WALL, V_WATER, ... Remarquer que
// weight[V_WALL]<0 ce qui n'est pas a priori une valuation correcte.
// En effet A* ne marche qu'avec des poids positifs! Mais ce n'est pas
// un problème, puisqu'en position (i,j), si G.value[i][j] = V_WALL,
// alors c'est que le sommet à cette position n'existe pas! Et donc
// aucune arête ne peut donc être incidente à (i,j).

double weight[] = {
    1.0,   // V_FREE
    -99.9, // V_WALL
    3.0,   // V_SAND
    9.0,   // V_WATER
    2.3,   // V_MUD
    1.5,   // V_GRASS
    0.1,   // V_TUNNEL
};

// Que doit renvoyer la fonction A_star(G,h) ?
//---------------------------------------------
//
// Votre fonction A_star(G,h) doit construire un chemin dans la grille
// G, entre la position G.start et G.end, selon l'heuristique h() et
// renvoyer son coût. Le chemin doit être calculé selon l'algorithme
// A* vu en cours (utilisez les notes de cours !). L'heuristique h()
// est une fonction à choisir lors de l'appel parmi celles ci-dessus
// ou que vous pouvez vous-même définir.
//
// Si le chemin n'est pas trouvé (par exemple si la destination est
// enfermée entre 4 murs ou si G.end est sur un mur), il faut renvoyer
// une valeur < 0.
//
// Sinon, il faut renvoyer le coût du chemin trouvé et remplir le
// champs .mark de G pour que le chemin trouvé puisse être visualisé
// par drawGrid(G) (plutard dans le main). Il faut, par convention,
// avoir G.mark[i][j] = M_PATH ssi la case (i,j) appartient au chemin
// trouvé. Utilisez les touches a,z,+,-,p,c pour gérer la vitesse
// d'affichage et de progression de l'algorithme par exemple.
// Repportez-vous à "tools.h" pour avoir la liste des differentes
// touches et leurs actions, ainsi que les différentes valeurs
// possibles pour G.mark[][].
//
//
// Comment gérer les ensembles P et Q ?
//--------------------------------------
//
// Pour gérer l'ensemble P, servez-vous du champs G.mark[i][j] (=
// M_USED ssi (i,j) est dans P). Par défaut, ce champs est initialisé
// partout à M_NULL par toute fonction initGridXXX().
//
// Pour gérer l'ensemble Q, vous devez utiliser un tas min de noeuds
// (type node) avec une fonction de comparaison (à créer) qui dépend
// du champs .score des noeuds. Pour la fonction de comparaison faites
// attention au fait que l'expression "(int)(8.1 - 8.2)" n'est pas
// négative, mais nulle! Vous devez utilisez la gestion paresseuse du
// tas (cf. le paragraphe du cours à ce sujet, dans l'implémentation
// de Dijkstra). Pensez qu'avec cette gestion paresseuse, la taille de
// Q est au plus la somme des degrés des sommets dans la grille. Pour
// visualiser un noeud de coordonnées (i,j) qui passe dans le tas Q
// vous pourrez mettre G.mark[i][j] = M_FRONT au moment où vous
// l'ajoutez. Même si cela est tentant, il ne faut pas utiliser la
// marque M_FRONT pour savoir si un sommet est dans Q. Le champs .mark
// ne doit servir que pour l'ensemble P et l'affichage de la grille.
// Pas pour le tas Q !

// Pensez à dessiner la grille avec drawGrid(G) à chaque fois que
// possible, par exemple, lorsque vous ajoutez un sommet à P mais
// aussi lorsque vous reconstruisez le chemin à la fin de la
// fonction. Lorsqu'un sommet passe dans Q vous pourrez le marquer
// M_FRONT (dans son champs .mark) pour le distinguer à l'affichage
// des sommets de P (couleur différente).

// Après avoir extrait un noeud de Q, il ne faut pas le détruire,
// sous peine de ne plus pouvoir reconstruire le chemin trouvé !
// Une fonction createNode() peut simplifier votre code.

// Les bords de la grille sont toujours constitués de murs (V_WALL)
// ce qui évite d'avoir à tester la validité des indices des
// positions (sentinelle). Dit autrement, un chemin ne peut pas
// s'échapper de la grille.

double A_star(grid G, heuristic h)
{
  int N = 0;
  int x, y;
  double returnValue = 0, cost, score;
  position endPosition = G.end;
  position pos;
  node Node, Voisin, tmp;
  heap Q = heap_create(8, f);
  node startNode = createNode(G.start, 0, h(G.start, G.end, &G), NULL, SOURCE_S);
  heap_add(Q, startNode);
  while (!heap_empty(Q))
  {
    drawGrid(G);
    Node = heap_pop(Q);
    if (compareNodePos(Node, endPosition))
    {
      returnValue = Node->cost;
      tmp = Node;
      while (Node != NULL)
      {
        G.mark[Node->pos.x][Node->pos.y] = M_PATH;
        Node = Node->parent;
      }
      freeNode(tmp);
      while (!heap_empty(Q))
      {
        Node = heap_pop(Q);
        freeNode(Node);
      }
      heap_destroy(Q);
      return returnValue;
    }
    // (c)

    if (G.mark[Node->pos.x][Node->pos.y] == M_USED)
    {
      freeNode(Node);
      continue;
    }
    G.mark[Node->pos.x][Node->pos.y] = M_USED;
    N = Q->n;
    for (int i = -1; i <= 1; i++)
    {
      for (int j = -1; j <= 1; j++)
      {
        x = Node->pos.x + i;
        y = Node->pos.y + j;
        if (G.mark[x][y] != M_USED && G.value[x][y] != V_WALL)
        {
          pos = (position){.x = x, .y = y};
          cost = ((i == 0 || j == 0) ? 1 : M_SQRT2) * weight[G.value[x][y]] +
                 Node->cost;
          score = cost + h(endPosition, pos, &G);
          Voisin = createNode(pos, cost, score, Node, SOURCE_S);
          if (heap_add(Q, Voisin))
          {
            exit(EXIT_FAILURE);
          }
        }
      }
    }
    // aucun voisin n'a été créer
    if (N == Q->n)
      freeNode(Node);
  }
  return -1;
}

double A_star2(grid G, heuristic h)
{
  int N = 0;
  int x, y;
  double returnValue = 0, cost, score;
  position endPosition = G.end;
  position pos;
  node Node, Voisin, tmp;
  heap Q = heap_create(8, f);
  node startNode1 = createNode(G.start, 0, h(G.start, G.end, &G), NULL, SOURCE_S);
  node startNode2 = createNode(G.start, 0, h(G.start, G.end, &G), NULL, SOURCE_T);
  heap_add(Q, startNode1);
  heap_add(Q, startNode2);

  while (!heap_empty(Q))
  {
    drawGrid(G);

    //Étape a:

    Node = heap_pop(Q);

    // Étape b:

    if ((Node->source == SOURCE_S && G.mark[Node->pos.x][Node->pos.y] == M_USED) ||
        (Node->source == SOURCE_T && G.mark[Node->pos.x][Node->pos.y] == M_USED2))
    {
      //compare la racine avec le noeud en dernière position
      if (compareNodePos(Node, endPosition))
      {
        returnValue = Node->cost;
        tmp = Node;
        while (Node != NULL)
        {
          G.mark[Node->pos.x][Node->pos.y] = M_PATH;
          Node = Node->parent;
        }
        freeNode(tmp);
        while (!heap_empty(Q))
        {
          Node = heap_pop(Q);
          freeNode(Node);
        }
        heap_destroy(Q);
        return returnValue;
      }
    }

    //Étape c:

    if ((G.mark[Node->pos.x][Node->pos.y] == M_USED) ||
        (G.mark[Node->pos.x][Node->pos.y] == M_USED2))
    {
      freeNode(Node);
      continue;
    }

    //Étape d:

    G.mark[Node->pos.x][Node->pos.y] = M_USED2;
    N = Q->n;
    for (int i = -1; i <= 1; i++)
    {
      for (int j = -1; j <= 1; j++)
      {
        x = Node->pos.x + i;
        y = Node->pos.y + j;
        if (G.mark[x][y] != M_USED && G.value[x][y] != V_WALL)
        {
          pos = (position){.x = x, .y = y};
          cost = ((i == 0 || j == 0) ? 1 : M_SQRT2) * weight[G.value[x][y]] +
                 Node->cost;
          score = cost + h(endPosition, pos, &G);
          Voisin = createNode(pos, cost, score, Node, SOURCE_T);
          if (heap_add(Q, Voisin))
          {
            exit(EXIT_FAILURE);
          }
        }
      }
    }

    //Étape d:

    G.mark[Node->pos.x][Node->pos.y] = M_USED;

    for (int i = -1; i <= 1; i++)
    {
      for (int j = -1; j <= 1; j++)
      {
        x = Node->pos.x + i;
        y = Node->pos.y + j;
        if (G.mark[x][y] != M_USED && G.value[x][y] != V_WALL)
        {
          pos = (position){.x = x, .y = y};
          cost = ((i == 0 || j == 0) ? 1 : M_SQRT2) * weight[G.value[x][y]] +
                 Node->cost;
          score = cost + h(endPosition, pos, &G);
          Voisin = createNode(pos, cost, score, Node, SOURCE_S);
          if (heap_add(Q, Voisin))
          {
            exit(EXIT_FAILURE);
          }
        }
      }
    }
    // aucun voisin n'a été créer
    if (N == Q->n)
      freeNode(Node);
  }

  return -1;
}

// Améliorations à faire seulement quand vous aurez bien avancé:
//
// (1) Le chemin a tendance à zizaguer, c'est-à-dire à utiliser aussi
//     bien des arêtes horizontales que diagonales (qui peuvent avoir
//     le même coût), même pour des chemins en ligne droite. Essayez
//     de rectifier ce problème d'esthétique en modifiant le calcul de
//     score[v] de sorte qu'à coût[v] égale les arêtes (u,v)
//     horizontales ou verticales soient favorisées (un score plus
//     faible). Bien sûr, votre modification ne doit en rien changer
//     la distance (la somme des coût) entre .start et .end.
//
// (2) Modifier votre implémentation du tas dans heap.c de façon à
//     utiliser un tableau de taille variable, en utilisant realloc()
//     et une stratégie "doublante": lorsqu'il n'y a pas plus assez de
//     place dans le tableau, on double sa taille avec un realloc().
//     On peut imaginer que l'ancien paramètre 'nmax' devienne non pas
//     le nombre maximal d'éléments, mais sa taille maximale initiale
//     (comme par exemple nmax=4).
//
// (3) Gérer plus efficacement la mémoire en libérant les noeuds
//     devenus inutiles. Pour cela on ajoute un champs .nchild à la
//     structure node, permettant de gérer le nombre de fils qu'un
//     node de P ou Q possède. C'est relativement léger et facile à
//     gérer puisqu'on augmente .nchild de u chaque fois qu'on fait
//     parent[v]=p, soit juste après "node v = createNode(p,...)".
//     Pensez à faire .nchild=0 dans createNode(). Notez bien qu'ici
//     on parle de "node", donc de copie de sommet.
//
//     L'observation utile est que tous les nodes de Q sont des
//     feuilles. On va alors pouvoir se débarrasser de tous les nodes
//     ancêtres de ces feuilles simplement en extrayant les nodes de Q
//     dans n'importe quel ordre. (Si on veut être plus efficace que
//     |Q|*log|Q|, on peut vider le tableau .array[] directement sans
//     passer par heap_pop(). Pour être propre, il faudrait peut-être
//     ajouter une fonctions comme "void* heap_get(int i)" qui
//     permettrait d'extraire l'objet numéro i sans modifier le tas,
//     et renvoie NULL s'il est vide). On supprime alors chaque node,
//     en mettant à jour le nombre de fils de son père, puis en
//     supprimant le père s'il devient feuille (son .nchild passe 0)
//     et ainsi de suite. On élimine ainsi l'arbre par branches qui se
//     terminent toutes dans Q.
//
//     Ce processus peut ne pas fonctionner si P contient des nodes
//     qui sont des feuilles. L'observation est que de tels nodes ne
//     peuvent pas être sur le chemin s->t. On peut donc les supprimer
//     au fur et à mesure directement dans la boucle principale quand
//     on les détecte. On voit qu'un tel node apparaît si après avoir
//     parcouru tous les voisins de u aucun node v n'est créé (et
//     ajouté dans Q). Il suffit donc de savoir si on est passé par
//     heap_add() (ou encore de comparer la taille de Q avant et après
//     la boucle sur les voisins). Si u est une feuille, on peut alors
//     supprimer le node u, mettre à jour .nchild de son père et
//     remonter la branche jusqu'à trouver un node qui n'est plus une
//     feuille. C'est donc la même procédure d'élagage que précdemment
//     qu'on pourrait capturer par une fonction freeNode(node p).

int main(int argc, char *argv[])
{
  unsigned seed = time(NULL) % 1000;
  printf("seed: %u\n", seed); // pour rejouer la même grille au cas où
  srandom(seed);

  // testez différentes grilles ...

  grid G = initGridPoints(80, 60, V_FREE, 1); // petite grille vide, sans
  // mur
  // grid G =initGridPoints(width, height, V_FREE, 1);  // grande grille vide,
  // sans mur
  // grid G = initGridPoints(32, 24, V_WALL, 0.2);  // petite grille
  // avec quelques murs
  //grid G = initGridLaby(12, 9, 8); // petit labyrinthe
  // aléatoire
  //grid G = initGridLaby(width / 8, height / 8, 3); // grand
  //labyrinthe aléatoire
  //grid G = initGridFile("mygrid.txt"); // grille à
  // partir d'un fichier

  // ajoutez à G une (ou plus) "région" de texture donnée ...
  // (déconseillé pour initGridLaby() et initGridFile())

  //addRandomBlob(G, V_WALL, (G.X + G.Y) / 20);
  // addRandomBlob(G, V_SAND,   (G.X+G.Y)/15);
  // addRandomBlob(G, V_WATER,  (G.X+G.Y)/6);
  // addRandomBlob(G, V_MUD,    (G.X+G.Y)/3);
  // addRandomBlob(G, V_GRASS,  (G.X+G.Y)/15);
  // addRandomBlob(G, V_TUNNEL, (G.X+G.Y)/4);
  // addRandomArc(G, V_WALL,    (G.X+G.Y)/25);

  // sélectionnez des positions s->t ...
  // (inutile pour initGridLaby() et initGridFile())

  G.start = (position){0.1 * G.X, 0.2 * G.Y},
  G.end = (position){0.8 * G.X, 0.9 * G.Y};
  // G.start=randomPosition(G,V_FREE); G.end=randomPosition(G,V_FREE);

  // constantes à initialiser avant init_SDL_OpenGL()
  scale = fmin((double)width / G.X, (double)height / G.Y); // zoom courant
  hover = false;                                           // interdire les déplacements de points
  init_SDL_OpenGL();                                       // à mettre avant le 1er "draw"
  drawGrid(G);                                             // dessin de la grille avant l'algo
  update = false;                                          // accélère les dessins répétitifs

  alpha = 0;
  double d = A_star2(G, halpha); // heuristique: h0, hvo, alpha*hvo

  // chemin trouvé ou pas ?
  if (d < 0)
    printf("path not found!\n");
  else
    printf("bingo!!! cost of the path: %g\n", d);

  // compte le nombre de sommets explorés pour comparer les
  // heuristiques
  int m = 0;
  for (int i = 0; i < G.X; i++)
    for (int j = 0; j < G.Y; j++)
      m += (G.mark[i][j] != M_NULL);
  printf("#nodes explored: %i\n", m);

  while (running)
  {                    // affiche le résultat et attend
    update = true;     // force l'affichage de chaque dessin
    drawGrid(G);       // dessine la grille (/!\ passe update à false)
    handleEvent(true); // attend un évènement
  }

  freeGrid(G);
  cleaning_SDL_OpenGL();
  return 0;
}
