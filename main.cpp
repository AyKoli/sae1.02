#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

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
 * \return
 */
invader creerEnnemi(unsigned id, string classe, unsigned posX, unsigned posY) //note - afficher tous les invaders d'un coup, puis uniquement gérer les déplacements
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
    
vaisseau MyHero = creerVaisseau(10, 20);
vector<invader> iterInvader (unsigned n)
{
    vector<invader> listeEnnemi;
    unsigned colonne = 1;
    unsigned cptLigne = 1;
    unsigned ligne = 1;
    for (unsigned i = 0; i < n; ++i)
    {
        listeEnnemi.push_back(creerEnnemi(i,"ranger", colonne, ligne));
        ++cptLigne;
        ++colonne;
        if (cptLigne == nColonnes-1)
        {
            ++ligne;
            colonne = 1;
            cptLigne = 1;
        }
    }
    return listeEnnemi;
}


/*!
 * \brief afficherTableau
 */
void afficherTableau()
{
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
                if (j == invaderTest.posX && i == invaderTest.posY)
                {
                    if(invaderTest.isAlive == true){
                        tableau[j][i] = invaderTest.carInvader;
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
                else
                    tableau[j][i] = ' ';
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
 */
void Shoot(){
  Missile.isAlive = true;
  Missile.posY = MyHero.posY -1;
  Missile.posX = MyHero.posX;
  while (Missile.posY > 0){
     --Missile.posY;
     if(Missile.posX == invaderTest.posX && Missile.posY == invaderTest.posY && Missile.isAlive == true && invaderTest.isAlive == true ){
         Missile.isAlive = false;
         --invaderTest.hp;
         if(invaderTest.hp == 0){
             invaderTest.isAlive = false;
         }
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
    for( unsigned i = 0; i<10; i++){
        cin >> Key;
        cout << Key;
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
    vector<invader> listeTest = iterInvader(10);
    afficherTableau(listeTest);
    cout << listeTest[9].posX << listeTest[9].posY;
}
