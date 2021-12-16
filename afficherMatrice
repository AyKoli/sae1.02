#include <iostream>
#include <thread>
#include <vector>

using namespace std;

#define VERTICALE   45 // caractère ascii en décimal
#define HORIZONTALE 124

#define COIN_HAUT_DROIT     64
#define COIN_HAUT_GAUCHE    64
#define COIN_BAS_DROIT      64
#define COIN_BAS_GAUCHE     64 // pour les contours du tableau

/*!
 * \brief afficherTableau
 * \param p_largeur
 * \param p_hauteur
 */
void afficherTableau(unsigned int p_largeur, unsigned int p_hauteur) {
    char tableau[p_largeur][p_hauteur];

    // Créations des bordures et du vide à l'intérieur
    for (unsigned int y = 0; y < p_hauteur; y++) {
        for (unsigned int x = 0; x < p_largeur; x++) {
            if (x == 0 || x == p_largeur - 1) {
                tableau[x][y] = HORIZONTALE;
            }
            else if (y == 0 || y == p_hauteur - 1) {
                tableau[x][y] = VERTICALE;
            }
            else {
                tableau[x][y] = ' ';
            }
        }
    }

    // Créations des coins
    tableau[0][0] = COIN_HAUT_GAUCHE;
    tableau[p_largeur - 1][0] = COIN_HAUT_DROIT;
    tableau[0][p_hauteur - 1] = COIN_BAS_GAUCHE;
    tableau[p_largeur - 1][p_hauteur - 1] = COIN_BAS_DROIT;

    // Finalement, on affiche tout le contenu du tableau
    for (unsigned int y = 0; y < p_hauteur; y++) {
        for (unsigned int x = 0; x < p_largeur; x++) {
            std::cout << tableau[x][y];
        }
        std::cout << "\n";
    }
}


int main()
{
    afficherTableau(10, 10);
}
