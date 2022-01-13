#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <random>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fstream>

using namespace std;

char lectureConfigChar (string paramConfig){
    ifstream ifs ("../SpaceInvaders/config.yaml");
    //si le fichier n'existe pas
    if (!ifs){
        cerr << "fichier n'existe pas" <<endl;
        //on arrete le pgm
        exit (-1);
    }
    for (string str; ifs >> str ; ){
        if(str == paramConfig){
        //on extrait le ':'
        char deuxPoint;
        ifs >> deuxPoint;
        if (deuxPoint != ':') {
            cerr << "mauvais caractère" <<endl;
            //on arrete le pgm
            exit (-1);
        }
        getline (ifs, str);
        str.erase(0, 1);
        //str = str.substr(1, str.size());
        char valeur = str[0];
        return valeur;
        }
    }
    ifs.close();
}

string lectureConfigString (string paramConfig){
    ifstream ifs ("../SpaceInvaders/config.yaml");
    //si le fichier n'existe pas
    if (!ifs){
        cerr << "fichier n'existe pas" <<endl;
        //on arrete le pgm
        exit (-1);
    }
    for (string str; ifs >> str ; ){
        if(str == paramConfig){
        //on extrait le ':'
        char deuxPoint;
        ifs >> deuxPoint;
        if (deuxPoint != ':') {
            cerr << "mauvais caractère" <<endl;
            //on arrete le pgm
            exit (-1);
        }
        getline (ifs, str);
        str.erase(0, 1);
        //str = str.substr(1, str.size());
        string valeur = str;
        return valeur;
        }
    }
    ifs.close();
}

const char KRight               = lectureConfigChar("DROITE");  // Déplacement vers la droite
const char KLeft                = lectureConfigChar("GAUCHE");  // Déplacement vers la gauche
const char KShoot               = lectureConfigChar("TIRER");  // Lancer de torpille

//  Constantes liées à l'envahisseur

const char KTrooper = lectureConfigChar("TROOPER"); //Forme du trooper
const char KRanger = lectureConfigChar("RANGER"); //Forme du ranger
const char KTank = lectureConfigChar("TANK");
const char KTorpedo = lectureConfigChar("TORPILLE");
const char KBalle = lectureConfigChar("BALLE");

// Constantes liées au joueur

const string Difficulty (lectureConfigString("DIFFICULTE"));
const char KJoueur = 'A';
const char KMissile = '|';

// Constantes liées à l'eapace (l'écran)

const unsigned nLignes   = 10+2;   // Nombre de lignes de l'écran (de l'espace)
const unsigned nColonnes  = 15+2;   // Nombre de colonnes de l'écran (de l'espace)


unsigned KRatioMeInvaders = 4; // Nombre de fois où c'est le tour du joueur pour un tour de l'envahisseur
unsigned KTour = 1;


struct termios saved_attributes;

void
reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void) // Fonction TP 10 pour mode non canonique
{
  struct termios tattr;
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
  tattr.c_cc[VTIME] = 4;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
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
};

/*!
 * \brief creerMissile
 * \param posX
 * \param posY
 * \return missile
 */
missile creerMissile(unsigned posX, unsigned posY){
    missile MyMissile;
    MyMissile.carMissile = KMissile;
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
    MyPerso.carVaisseau = KJoueur;
    if (Difficulty == "facile"){
        MyPerso.pv = 6;
    }
    if (Difficulty == "normal"){
        MyPerso.pv = 3;
    }
    if (Difficulty == "difficile"){
        MyPerso.pv = 1;
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
    invader vaisseauEnnemi;
    vaisseauEnnemi.id = id;
    vaisseauEnnemi.classe = classe;
    vaisseauEnnemi.posX = posX;
    vaisseauEnnemi.posY = posY;
    vaisseauEnnemi.sensMove = sensMove;

    if (classe == "ranger")
    {
        vaisseauEnnemi.hp = 1;
        vaisseauEnnemi.carInvader = KRanger;
    }
    else if (classe == "trooper")
    {
        vaisseauEnnemi.hp = 2;
        vaisseauEnnemi.carInvader = KTrooper;
    }
    else if (classe == "tank")
    {
        vaisseauEnnemi.hp = 5;
        vaisseauEnnemi.carInvader = KTank;
    }

    return vaisseauEnnemi;
}

unsigned score = 0; // Initialision du score
vaisseau vaisseauJoueur = creerVaisseau(nColonnes / 2, nLignes - 2); // Création du vaisseau de notre personnage

/*!
 * \brief iterInvader
 * \param n
 * \return listeEnnemi
 */
vector<invader> iterInvader ()
{
    vector<invader> listeEnnemi;
    unsigned ligne = 1;
    unsigned tabEstImpair = 0;
    string sensMove = "droite";
    string invaderClass;
    unsigned seed;
    srand(time(NULL));

    if (nColonnes % 2 != 0)
        tabEstImpair = 1;

    for (unsigned i = 0; i < (nColonnes - 2)*1.5; ++i)
    {
        seed = rand() % 100 + 1;
        if (seed > 1 && seed < 51)
            invaderClass = "trooper";
        else if (seed > 51 && seed < 81)
            invaderClass = "ranger";
        else
            invaderClass = "tank";

        if (i == 0)
            listeEnnemi.push_back(creerEnnemi(0,invaderClass,1,1,"droite"));

        else if (ligne % 2 == 0)
        {
            if (listeEnnemi[i-1].posX - 2 + tabEstImpair < 1 + tabEstImpair)
            {
                sensMove = "droite";
                listeEnnemi.push_back(creerEnnemi(i,invaderClass,1,listeEnnemi[i-1].posY+1,sensMove));
                ++ligne;
            }
            else
            {
                listeEnnemi.push_back(creerEnnemi(i,invaderClass, listeEnnemi[i-1].posX-2, listeEnnemi[i-1].posY, sensMove));
            }
         }

        else
        {
            if (listeEnnemi[i-1].posX + 1 >= nColonnes - 2)
            {
                sensMove = "gauche";
                listeEnnemi.push_back(creerEnnemi(i,invaderClass,nColonnes - 2 - tabEstImpair,listeEnnemi[i-1].posY+1,sensMove));
                ++ligne;
            }
            else
            {
                listeEnnemi.push_back(creerEnnemi(i,invaderClass, listeEnnemi[i-1].posX+2, listeEnnemi[i-1].posY, sensMove));
            }
        }
    }
    return listeEnnemi;
}


vector <invader> listeEnnemi = iterInvader(); // Création des invaders

/*!
 * \brief manageInvaders
 */
void manageInvaders ()
{
    if (KTour == KRatioMeInvaders)
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
        KTour = 0;
    }
    else
        ++KTour;
}

struct torpille
{
    unsigned id;
    char carTorpille = KTorpedo;
    unsigned posX;
    unsigned posY;
    unsigned dmg;
    bool isAlive = false;
};

torpille creerTorpille (unsigned id, unsigned posX, unsigned posY, string classeTorpille)
{
    torpille Torpille;
    Torpille.id = id;
    Torpille.isAlive = true;
    Torpille.posX = posX;
    Torpille.posY = posY;
    if (classeTorpille == "ranger")
    {
        Torpille.dmg = 3;
        Torpille.carTorpille = KBalle;
    }
    else
        Torpille.dmg = 1;

    return Torpille;
}

vector<torpille> listeTorpilles;
bool salveEnCours;

void shootTorpille ()
{
    unsigned maxY = 1;
    if (!salveEnCours)
    {
        for (unsigned i = 0; i < listeEnnemi.size(); ++i)
        {
            if (listeEnnemi[i].posY > maxY && listeEnnemi[i].isAlive)
                maxY = listeEnnemi[i].posY;
        }
        for (unsigned j = 0; j < listeEnnemi.size(); ++j)
        {
            if (listeEnnemi[j].posY == maxY && listeEnnemi[j].isAlive && listeEnnemi[j].classe != "tank")
            {
                listeTorpilles.push_back(creerTorpille(listeEnnemi[j].id, listeEnnemi[j].posX, listeEnnemi[j].posY, listeEnnemi[j].classe));
            }
        }
        salveEnCours = true;
    }
}

void manageTorpille ()
{
    for (unsigned i = 0; i < listeTorpilles.size(); ++i)
    {
        ++listeTorpilles[i].posY;
        if (listeTorpilles[i].posX == vaisseauJoueur.posX && listeTorpilles[i].posY == vaisseauJoueur.posY)
        {
            if (listeTorpilles[i].dmg > vaisseauJoueur.pv)
                vaisseauJoueur.pv = 0;
            else
                vaisseauJoueur.pv = vaisseauJoueur.pv - listeTorpilles[i].dmg;

            if (score < 100)
                score = 0;
            else
                score = score - 100;

            listeTorpilles[i].isAlive = false;
        }
    }

    for (unsigned j = 0; j < listeTorpilles.size(); ++j)
        if (!listeTorpilles[j].isAlive || listeTorpilles[j].posY > nColonnes)
        {
            listeTorpilles.erase(listeTorpilles.begin()+j);
        }
    if (listeTorpilles.size() == 0)
        salveEnCours = false;
}

/*!
 * \brief shootMissile
 * \param listeEnnemi
 */
void shootMissile(){

  if (Missile.isAlive == true){
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
                score = score + 300;
            }
        }
      if(Missile.posY == 0){
          Missile.isAlive = false;
      }
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
        if( Key == KLeft && vaisseauJoueur.posX != 1){
            vaisseauJoueur.posX = vaisseauJoueur.posX - 1;
        }
        else if(Key == KRight && vaisseauJoueur.posX != nColonnes - 2){
            vaisseauJoueur.posX = vaisseauJoueur.posX + 1;
        }
        else if( Key == KShoot && !Missile.isAlive){
        {
            Missile.isAlive = true;
            Missile.posY = vaisseauJoueur.posY;
            Missile.posX = vaisseauJoueur.posX;
            shootTorpille();
        }
        }
    }
}

void ClearScreen ()
{
    cout << "\033[H\033[2J";
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
                        if(listeEnnemi[k].isAlive == true)
                            tableau[j][i] = listeEnnemi[k].carInvader;
                    }
                 }

                for (unsigned l = 0; l < listeTorpilles.size(); ++l)
                {
                    if (j == listeTorpilles[l].posX && i == listeTorpilles[l].posY)
                        tableau[j][i] = listeTorpilles[l].carTorpille;
                }
                 if (j == vaisseauJoueur.posX && i == vaisseauJoueur.posY)
                 {
                     if(vaisseauJoueur.isAlive == true)
                     tableau[j][i] = vaisseauJoueur.carVaisseau;

                 }
                 if((j == Missile.posX && i == Missile.posY))
                 {
                      if(Missile.isAlive == true)
                      tableau[j][i] = Missile.carMissile;

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

struct tabHighScore {
    string nomJoueur;
    unsigned score;
};

bool isHigher (tabHighScore joueur1, tabHighScore joueur2)
{
    return joueur1.score <= joueur2.score;
}


void ecritureDansFichier(string nomJoueur, unsigned score)
{
   ofstream fichier("../SpaceInvaders/score.txt",ios::out | ios::app);
   if(fichier)
   {
       fichier << score << "," << nomJoueur << endl;
   }
   fichier.close();
}

void clearFichier ()
{
    ofstream ofs;
    ofs.open("../SpaceInvaders/score.txt",ofstream::out | ofstream::trunc);
    ofs.close();
}

void triHighScore()
{
   ifstream fichier("../SpaceInvaders/score.txt", ios::in);  // on ouvre en lecture

   if(fichier)  // si l'ouverture a fonctionné
   {
       string ligne;
       unsigned score;
       string nomJoueur;
       vector<tabHighScore> tabScores;
       unsigned i = 0;
       while(getline(fichier, ligne))  // tant que l'on peut mettre la ligne dans "contenu"
       {
           while(ligne[i] != ',')
           {
               score = score + unsigned(ligne[i]);
               ++i;
           }
           for (unsigned j = i+1; j < ligne.size(); ++i)
           {
               nomJoueur = nomJoueur + ligne[j];
           }
           tabScores[i].nomJoueur = nomJoueur;
           tabScores[i].score = score;
           score = 0;
           nomJoueur.clear();
       }
       sort(tabScores.begin(),tabScores.end(),isHigher);
       clearFichier();
       fichier.close();
       for (unsigned i = 0; i < tabScores.size(); ++i)
       {
           ecritureDansFichier(tabScores[i].nomJoueur, tabScores[i].score);
       }
   }
   else
   {
       cerr << "Impossible d'ouvrir le fichier !" << endl;
   }
}

void affichageHighScore()
{
    ifstream fichier("../SpaceInvaders/score.txt", ios::in);

    string ligne;
    string score;
    string nomJoueur;
    unsigned i = 0;
    unsigned classement = 1;
    while (getline(fichier,ligne) && classement <= 10)
    {
        while(ligne[i] != ',')
        {
            score = score + ligne[i];
            ++i;
        }
        for (unsigned j = i+1; j < ligne.size(); ++i)
        {
            nomJoueur = nomJoueur + ligne[j];
        }
        cout << classement << ". " << nomJoueur << " ....... " << score << endl;
        nomJoueur.clear();
        score.clear();
        ++classement;
    }
    fichier.close();
}

int main()
{
    afficherTableau();
    bool gameOver = false;
    while(gameOver == false)
    {
        manageHero();
        manageInvaders();
        shootMissile();
        shootTorpille();
        manageTorpille();
        afficherTableau();
        cout << endl << "HP - " << vaisseauJoueur.pv << endl;
        cout << endl << "SCORE - " << score << endl;
        reset_input_mode();
        if (vaisseauJoueur.pv <= 0)
            gameOver = true;
        for (unsigned i = 0; i < listeEnnemi.size(); ++i)
        {
            if (listeEnnemi[i].posY == vaisseauJoueur.posY && listeEnnemi[i].isAlive == true)
            {
                gameOver = true;
            }
        }
    }
    ClearScreen();
    cout << "GAME OVER" << endl << endl;
    cout << "Entre ton nom: ";
    string nomJoueur;
    getline(cin,nomJoueur);
    cout << nomJoueur << "! Ton score est de " << score <<" point(s). Bravo!" << endl << endl;
    ecritureDansFichier(nomJoueur,score);
    triHighScore();
    cout << "Voici le top 10 des meilleurs scores :" << endl << endl;
    affichageHighScore();
}
