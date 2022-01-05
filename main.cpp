#include <iostream>
#include <vector>

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
const char KRight               = '6';  // déplacement vers la droite
const char KLeft                = '4';  // Déplacement vers la gauche
const char KShoot               = '5';  // Lancé de torpille

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

const unsigned KSizeLine   = 10;   // Nombre de lignes de l'écran (de l'espace)
const unsigned KSizeSpace  = 10;   // Nombre de colonnes de l'écran (de l'espace)

const unsigned KBegInvader = 0;    // Numéro de colonne où commence l'envahisseur
const unsigned KBegMe      = KSizeLine / 2;  // Numéro de colonne où commence le joueur

typedef vector <string> CVString;    // c'est le type de l'écran (l'espace, la matrice)

const string KEmptyLine (KSizeLine, KEmpty);  // Une ligne vide de la matrice

const unsigned KRatioMeInvaders = 4;    // Nombre de fois où c'est le tour du joueur pour un tour de l'envahisseur

/*!
 * \brief afficherTableau
 */
void afficherTableau()
{
    ClearScreen();

    char tableau[KSizeSpace][KSizeLine];

    //Choix des caractères ASCII pour chaque élément du tableau
    unsigned caraVertical = 45;
    unsigned caraHorizontal = 124;
    unsigned coinCadre = 43;

    // Créations des bordures et du vide à l'intérieur
    for (unsigned i = 0; i < KSizeLine; ++i)
    {
        for (unsigned j = 0; j < KSizeSpace; j++)
        {
            if (j == 0 || j == KSizeSpace - 1)
            {
                tableau[j][i] = caraHorizontal;
            }
            else if (i == 0 || i == KSizeLine - 1)
            {
                tableau[j][i] = caraVertical;
            }
            else
            {
                tableau[j][i] = ' ';
            }
        }
    }

    // Création des coins
    tableau[0][0] = coinCadre;
    tableau[KSizeSpace - 1][0] = coinCadre;
    tableau[0][KSizeLine - 1] = coinCadre;
    tableau[KSizeSpace - 1][KSizeLine - 1] = coinCadre;

    // Affichage du reste du tableau
    for (unsigned i = 0; i < KSizeLine; ++i) {
        for (unsigned j = 0; j < KSizeSpace; ++j) {
            cout << tableau[j][i];
        }
        cout << "\n";
    }
}

/*!
 * \brief coordToTable
 * \param KSizeSpace
 * \param posX
 * \param posY
 */
unsigned coordToTable(const unsigned KSizeSpace, unsigned posX, unsigned posY)
{
    unsigned idCase = posX + (KSizeSpace * posY);
    return idCase;
}

/*!
 * \brief The invader struct
 */
struct invader
{
    unsigned id;
    string classe;
    unsigned posX;
    unsigned posY;
    unsigned caseTab = coordToTable(KSizeSpace, posX, posY);
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
    if (classe != "ranger" || "trooper" || "tank")
    {
        cout << "Paramètre \'classe\' invalide. Arrêt de la fonction." << endl;
        abort();
    }

    invader vaisseauEnnemi;
    vaisseauEnnemi.id = id;
    vaisseauEnnemi.classe = classe;
    vaisseauEnnemi.posX = posX;
    vaisseauEnnemi.posY = posY;

    if (classe == "ranger")
    {
        vaisseauEnnemi.hp = 1;
    }
    else if (classe == "trooper")
    {
        vaisseauEnnemi.hp = 2;
    }
    else if (classe == "tank")
    {
        vaisseauEnnemi.hp = 3;
    }
}
