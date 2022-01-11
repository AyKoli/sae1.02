#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
using namespace std;

void ClearScreen ()
{
    cout << "\033[H\033[2J";
}

const string KReset   ("0");
const string KBlack    ("30");
const string KRed   ("31");
const string KGreen    ("32");
const string KYellow   ("33");
const string KBlue    ("34");
const string KMagenta ("35");
const string KCyan    ("36");

void Couleur (const string & coul)
{
    cout << "\033[" << coul <<"m";
}

const char KEmpty               = ' ';  // case vide de l'écran
const char KRight               = 'd';  // déplacement vers la droite
const char KLeft                = 'q';  // Déplacement vers la gauche
const char KShoot               = 's';  // Lancer de torpille

//  Constantes liées à l'envahisseur

const string KInvadersColor (KYellow);  // Couleur de l'envahisseur
const char KInsideInvader       = 'W';  // caractèrere formant l'envahisseur
const char KMissile             = 'T';  // missile
const unsigned KInvadersSize    =  4;   // nombre de caractères qui forment l'envahisseur
const unsigned KInvadersMiddle  = KInvadersSize / 2;
const string KInvadersForm (KInvadersSize, KInsideInvader);  // forme de l'envahisseur

// Constantes liées au joueur

const string KMyColor (KGreen);
const char KInsideMe            = 'A';
const char KTorpedo             = '|';
const string Difficulty ("facile");
const unsigned KMySize          = 1;
const unsigned KMyMiddle        = KMySize / 2;
const string KMyForm (KMySize, KInsideMe);

// Constantes liées à l'eapace (l'écran)

const unsigned nLignes   = 20+2;   // Nombre de lignes de l'écran (de l'espace)
const unsigned nColonnes  = 20+2;   // Nombre de colonnes de l'écran (de l'espace)

const unsigned KBegInvader = 0;    // Numéro de colonne où commence l'envahisseur
const unsigned KBegMe = nLignes / 2;  // Numéro de colonne où commence le joueur

typedef vector <string> CVString;    // c'est le type de l'écran (l'espace, la matrice)

const string KEmptyLine (nLignes, KEmpty);  // Une ligne vide de la matrice

const unsigned KRatioMeInvaders = 4;    // Nombre de fois où c'est le tour du joueur pour un tour de l'envahisseur

chrono::milliseconds framerate(200);

struct termios saved_attributes;

void
reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void) // Fonction TP 10 pour mode non canonique
{
  struct termios tattr;
  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 0;
  tattr.c_cc[VTIME] = 5;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}




/*!
 * \brief coordToTable
 * \param nColonnes
 * \param posX
 * \param posY
 */
unsigned coordToTable(const unsigned nColonnes, unsigned posX, unsigned posY)
{
    unsigned idCase = posX + (nColonnes * posY);
    return idCase;
}

/*!
 * \brief The hero struct
 */
struct missile
{
    char carMissile;
    unsigned posX;
    unsigned posY;
    bool isAlive;
    unsigned caseTab = coordToTable(nColonnes, posX, posY);
};

/*!
 * \brief creerMissile
 * \param posX
 * \param posY
 * \return missile
 */
missile creerMissile(unsigned posX, unsigned posY){
    missile MyMissile;
    MyMissile.carMissile = KTorpedo;
    MyMissile.posX = posX;
    MyMissile.posY = posY;
    MyMissile.isAlive = false;
    return MyMissile;
}

missile Missile = creerMissile(0,0);    // Creation du missile

/*!
 * \brief The hero struct
 */
struct vaisseau
{
    char carVaisseau;
    unsigned posX;
    unsigned posY;
    bool isAlive;
    unsigned caseTab = coordToTable(nColonnes, posX, posY);
    unsigned pv;
};

/*!
 * \brief creerVaisseau
 * \param posX
 * \param posY
 * \return vaisseau
 */
vaisseau creerVaisseau(unsigned posX, unsigned posY){
    vaisseau MyPerso;
    MyPerso.carVaisseau = KInsideMe;
    if (Difficulty == "facile"){
        MyPerso.pv = 6;
    }
    if (Difficulty == "normal"){
        MyPerso.pv = 3;
    }
    if (Difficulty == "difficile"){
        MyPerso.pv = 2;
    }
    MyPerso.posX = posX;
    MyPerso.posY = posY;
    MyPerso.isAlive = true;
    return MyPerso;
}
/*!
 * \brief The invader struct
 */
struct invader
{
    unsigned id;
    string classe;
    char carInvader;
    unsigned posX;
    unsigned posY;
    string sensMove = "droite";
    unsigned caseTab = coordToTable(nColonnes, posX, posY);
    bool isAlive = true;
    unsigned hp;
};

/*!
 * \brief creerEnnemi
 * \param id
 * \param classe
 * \param posX
 * \param posY
 * \param sensMove
 * \return
 */
invader creerEnnemi(unsigned id, string classe, unsigned posX, unsigned posY, string sensMove) //note - afficher tous les invaders d'un coup, puis uniquement gérer les déplacements
{
//    if (classe != "ranger" || "trooper" || "tank")
//    {
//        cout << "Paramètre \'classe\' invalide. Arrêt de la fonction." << endl;
//        abort();
//    }

    invader vaisseauEnnemi;
    vaisseauEnnemi.id = id;
    vaisseauEnnemi.classe = classe;
    vaisseauEnnemi.posX = posX;
    vaisseauEnnemi.posY = posY;
    vaisseauEnnemi.sensMove = sensMove;

    if (classe == "ranger")
    {
        vaisseauEnnemi.hp = 1;
        vaisseauEnnemi.carInvader = 'Y';
    }
    else if (classe == "trooper")
    {
        vaisseauEnnemi.hp = 2;
        vaisseauEnnemi.carInvader = 'V';
    }
    else if (classe == "tank")
    {
        vaisseauEnnemi.hp = 3;
        vaisseauEnnemi.carInvader = 'W';
    }

    return vaisseauEnnemi;
}

int score = 0; // Initialision du score
vaisseau MyHero = creerVaisseau(10, 5); // Création du vaisseau de notre personnage

/*!
 * \brief iterInvader
 * \param n
 * \return listeEnnemi
 */
vector<invader> iterInvader (unsigned n)
{
    vector<invader> listeEnnemi;
    unsigned colonne = 1;
    unsigned cptLigne = 1;
    unsigned ligne = 1;
    string sensMove = "droite";
    for (unsigned i = 0; i < n; ++i)
    {
        listeEnnemi.push_back(creerEnnemi(i,"ranger", colonne, ligne, sensMove));
        ++cptLigne;
        if (ligne % 2 == 0)
        {
            --colonne;
        }
        else
        {
            ++colonne;
        }
        if (cptLigne == nColonnes-1)
        {
            cptLigne = 1;
            ++ligne;
            if (ligne % 2 == 0)
            {
                colonne = nColonnes-2;
                sensMove = "gauche";
            }
            else
            {
                colonne = 1;
                sensMove = "droite";
            }
        }
    }
    return listeEnnemi;
}



vector <invader> listeEnnemi = iterInvader(40); // Création des invaders

/*!
 * \brief manageInvaders
 */
void manageInvaders ()
{

    for (unsigned i = 0; i < listeEnnemi.size(); ++i)
    {
        if(listeEnnemi[i].sensMove == "droite")
        {
            if (listeEnnemi[i].posX+1 == nColonnes-1)
            {
                listeEnnemi[i].posY = listeEnnemi[i].posY + 1;
                listeEnnemi[i].sensMove = "gauche";
            }
            else
            {
                listeEnnemi[i].posX = listeEnnemi[i].posX + 1;
            }
        }
        else
        {
            if (listeEnnemi[i].posX-1 == 0)
            {
                listeEnnemi[i].posY = listeEnnemi[i].posY + 1;
                listeEnnemi[i].sensMove = "droite";
            }
            else
            {
                listeEnnemi[i].posX = listeEnnemi[i].posX - 1;
            }
        }
    }
}
/*!
 * \brief afficherTableau
 * \param listeEnnemi
 */
void afficherTableau(){
    ClearScreen();

    char tableau[nColonnes][nLignes];

    //Choix des caractères ASCII pour chaque élément du tableau
    unsigned caraVertical = 45;
    unsigned caraHorizontal = 124;
    unsigned coinCadre = 43;

    // Créations des bordures et du vide à l'intérieur
    for (unsigned i = 0; i < nLignes; ++i)
    {
        for (unsigned j = 0; j < nColonnes; j++)
        {
            if (j == 0 || j == nColonnes - 1)
            {
                tableau[j][i] = caraHorizontal;
            }
            else if (i == 0 || i == nLignes - 1)
            {
                tableau[j][i] = caraVertical;
            }
            else
            {
                tableau[j][i] = ' ';
                for (unsigned k = 0; k < listeEnnemi.size(); ++k)
                {
                    if (j == listeEnnemi[k].posX && i == listeEnnemi[k].posY)
                    {
                        if(listeEnnemi[k].isAlive == true){
                            tableau[j][i] = listeEnnemi[k].carInvader;
                        }
                        else{
                            tableau[j][i] = ' ';
                        }

                    }
                    else if (j == MyHero.posX && i == MyHero.posY)
                    {
                        if(MyHero.isAlive == true){
                            tableau[j][i] = MyHero.carVaisseau;
                        }
                        else{
                            tableau[j][i] = ' ';
                        }

                    }
                    else if((j == Missile.posX && i == Missile.posY))
                    {
                        if(Missile.isAlive == true){
                            tableau[j][i] = Missile.carMissile;
                        }
                        else{
                            tableau[j][i] = ' ';
                        }
                    }

                }

            }
        }
    }

    // Création des coins
    tableau[0][0] = coinCadre;
    tableau[nColonnes - 1][0] = coinCadre;
    tableau[0][nLignes - 1] = coinCadre;
    tableau[nColonnes - 1][nLignes - 1] = coinCadre;

    // Affichage du reste du tableau
    for (unsigned i = 0; i < nLignes; ++i) {
        for (unsigned j = 0; j < nColonnes; ++j) {
            cout << tableau[j][i];
        }
        cout << "\n";
    }
}


/*!
 * \brief Shoot
 * \param listeEnnemi
 */
void Shoot(){
  Missile.isAlive = true;
  Missile.posY = MyHero.posY -1;
  Missile.posX = MyHero.posX;
  while (Missile.isAlive == true){
     --Missile.posY;

      for (unsigned k = 0; k < listeEnnemi.size(); ++k)
        if(Missile.posX == listeEnnemi[k].posX && Missile.posY == listeEnnemi[k].posY && Missile.isAlive == true && listeEnnemi[k].isAlive == true ){
            cout << k;
            Missile.isAlive = false;
            Missile.posY = 0;
            Missile.posX = 0;
            --listeEnnemi[k].hp;
            if(listeEnnemi[k].hp == 0){
                listeEnnemi[k].isAlive = false;
                listeEnnemi[k].posX = 0;
                listeEnnemi[k].posY = 0;
            }
        }
      if(Missile.posY == 0){
          Missile.isAlive = false;
      }
     afficherTableau();
     this_thread::sleep_for(framerate);
   }
}

/*!
 * \brief deplacementHero
 */
void manageHero(){
    char Key;
    set_input_mode();
    for( unsigned i = 0; i<1; i++){
        read (STDIN_FILENO, &Key, 1);
        if( MyHero.posX < 20 && MyHero.posX > 0 ){
            if( Key == KLeft){

                MyHero.posX = MyHero.posX - 1;
                afficherTableau();
            }
            else if( Key == KRight){
                MyHero.posX = MyHero.posX + 1;
                afficherTableau();
            }
            else if( Key == KShoot){
                Shoot();
                afficherTableau();
            }

        }
    }
}

int main()
{
        afficherTableau();
        bool gameOver = false;
        while(gameOver == false)
        {
            manageHero();
            manageInvaders();
            afficherTableau();
            for (unsigned i = 0; i < listeEnnemi.size(); ++i)
            {
                if (listeEnnemi[i].posY == MyHero.posY && listeEnnemi[i].isAlive == true)
                {
                    gameOver = true;
                    cout << "c fini chacal";
                }
            }
            this_thread::sleep_for(framerate);
        }
}
