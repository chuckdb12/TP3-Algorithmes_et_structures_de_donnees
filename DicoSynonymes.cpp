/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author CHarles Dion-Buteau
* \version 0.1
* \date avril 2024
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"
#include <algorithm>
#include <new>

namespace TP3 {

    /*
       *\brief     Constructeur par défaut de la calsse DicoSynonymes
       *
       *\post      Une instance vide de la classe a été initialisée
       *
       */
    DicoSynonymes::DicoSynonymes() : racine(nullptr), nbRadicaux(0) {
    }

    /*
      *\brief  Constructeur de dictionnaire à partir d'un fichier
      *
      *\pre    Il y a suffisament de mémoire
      *\pre    Le fichier est ouvert au préalable
      *
      *\post   Si le fichier est ouvert, l'instance de la classe a été initialisée à partir
      *        du fichier de dictionnaire. Sinon, on génère une classe vide.
      *
      *\exception bad_alloc s'il n'y a pas assez de mémoire
      * Cette méthode appelle chargerDicoSynonyme !
      */
    DicoSynonymes::DicoSynonymes(std::ifstream &fichier) : racine(nullptr), nbRadicaux(0) {
        chargerDicoSynonyme(fichier);
    }

    // Méthode fournie
    void DicoSynonymes::chargerDicoSynonyme(std::ifstream &fichier) {
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

        std::string ligne;
        std::string buffer;
        std::string radical;
        int cat = 1;

        while (!fichier.eof()) // tant qu'on peut lire dans le fichier
        {
            std::getline(fichier, ligne);

            if (ligne == "$") {
                cat = 3;
                std::getline(fichier, ligne);
            }
            if (cat == 1) {
                radical = ligne;
                ajouterRadical(radical);
                cat = 2;
            } else if (cat == 2) {
                std::stringstream ss(ligne);
                while (ss >> buffer)
                    ajouterFlexion(radical, buffer);
                cat = 1;
            } else {
                std::stringstream ss(ligne);
                ss >> radical;
                ss >> buffer;
                int position = -1;
                ajouterSynonyme(radical, buffer, position);
                while (ss >> buffer)
                    ajouterSynonyme(radical, buffer, position);
            }
        }
    }

    /*
      *\brief     Destructeur.
      *
      *\post      Une instance de la classe est détruite.
      *
      */
    DicoSynonymes::~DicoSynonymes() {
        if(!estVide()) _supprimmeArbre(racine);
    }

    /*
      *\brief     Méthode récursive utilisée pas le Destructeur.
      *
      */
    void DicoSynonymes::_supprimmeArbre(NoeudDicoSynonymes *arbre)
    {
        //On supprime de maniere récursive tous les sous arbre de notre arbre
        if (arbre->gauche) _supprimmeArbre(arbre->gauche);
        if (arbre->droit) _supprimmeArbre(arbre->droit);
        delete arbre;
    }

    /*
      *\brief     Ajouter un radical au dictionnaire des synonymes
      *\brief     tout en s’assurant de maintenir l'équilibre de l'arbre.
      *
      *\pre    Il y a suffisament de mémoire.
      *
      *\post   Le mot est ajouté au dictionnaire des synonymes.
      *
      *\exception bad_alloc si il n'y a pas suffisament de mémoire.
      *\exception logic_error si le radical existe déjà.
      */
    void DicoSynonymes::ajouterRadical(const std::string &motRadical) {
        //Vérifier si le radical existe deja!!!!!!!!

        _auxAjouterRadical(racine, motRadical);

        nbRadicaux++;
    }

    /*
       *\brief     Méthode auxiliere pour ajouter un radical au dictionnaire des synonymes
       *\brief     tout en s’assurant de maintenir l'équilibre de l'arbre.
       *
       *\pre    Il y a suffisament de mémoire.
       *
       *\post   Le mot est ajouté au dictionnaire des synonymes.
       *
       *\exception bad_alloc si il n'y a pas suffisament de mémoire.
       *\exception logic_error si le radical existe déjà.
       */
    void DicoSynonymes::_auxAjouterRadical(NoeudDicoSynonymes *&arbre, const std::string &mot) {
        //Si l'arbre est vide, on crée un noeud
        if (arbre == nullptr) {
            arbre = new NoeudDicoSynonymes(mot);
            return;
        }
        //on vérifie si le mot ajouter est plus petit que celui à la racine de l'arbre
        if (mot < arbre->radical) {
            //Si c'Est le cas, on appelle la fonction de maniere récursive sur l'enfant gauche de la racine
            _auxAjouterRadical(arbre->gauche, mot);
        } else if (mot > arbre->radical) {
            //Si c'est le cas, on appelle la fonction de maniere récursive sur l'enfant droit de la racine
            _auxAjouterRadical(arbre->droit, mot);
        } else
            //Sinon, il s'agit du mm mot
            throw std::logic_error("Les duplicatats sont interdits");

        //Une fois que notre nouveau radical
        _balancer(arbre);


    }

    /*
       *\brief     Méthode qui sert à balancer un arbre
       */
    void DicoSynonymes::_balancer(NoeudDicoSynonymes *&arbre) {

        if (_debalancementAGauche(arbre)) {
            //Si debalancement à gauche, zigZag quand le sousArbre penche à gauche ou balancé
            if (_sousArbrePencheADroite(arbre->gauche)) {
                _zigZagGauche(arbre);
            } else {
                _zigZigGauche(arbre);
            }
        } else if (_debalancementADroite(arbre)) {
            //Si debalancement à droite, zigZag quand le sousArbre penche à droite ou balancé
            if (_sousArbrePencheAGauche(arbre->droit)) {
                _zigZagDroit(arbre);
            } else {
                _zigZigDroit(arbre);
            }
        } else {
            //Il n'y a pas de débalancement, on met seulement à jour les hauteurs
            if (arbre != nullptr)
                arbre->hauteur = 1 +
                                 std::max(_hauteur(arbre->gauche), _hauteur(arbre->droit));
        }
    }

    /*
       *\brief     Retourne la hauteur du Noeud en question
       */
    int DicoSynonymes::_hauteur(NoeudDicoSynonymes *arbre) const {
        if (arbre == nullptr)
            return -1;
        return arbre->hauteur;
    }

    /*
       *\brief     Méthode vérifie s'il y a un débalancement à gauche
       */
    bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes *&arbre) const {
        if (arbre == nullptr)
            return false;
        return 1 < _hauteur(arbre->gauche) - _hauteur(arbre->droit);
    }

    /*
       *\brief     Méthode vérifie s'il y a un débalancement à gauche
       */
    bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes *&arbre) const {
        if (arbre == nullptr)
            return false;
        return _hauteur(arbre->droit) - _hauteur(arbre->gauche) > 1;
    }

    /*
       *\brief     Méthode vérifie si le sous arbre penche à gauche
       */
    bool DicoSynonymes::_sousArbrePencheAGauche(DicoSynonymes::NoeudDicoSynonymes *arbre) const {
        if (arbre == nullptr)
            return false;

        return _hauteur(arbre->gauche) > _hauteur(arbre->droit);
    }

    /*
       *\brief     Méthode vérifie si le sous arbre penche à droite
       */
    bool DicoSynonymes::_sousArbrePencheADroite(DicoSynonymes::NoeudDicoSynonymes *arbre) const {
        return _hauteur(arbre->gauche) < _hauteur(arbre->droit);
    }

    /*
        *\brief     Méthode qui effectue un ZigZig a gauche
    */
    void DicoSynonymes::_zigZigGauche(DicoSynonymes::NoeudDicoSynonymes *&K2) {
        NoeudDicoSynonymes *K1 = K2->gauche;
        K2->gauche = K1->droit;
        K1->droit = K2;
        K2->hauteur = 1 + std::max(_hauteur(K2->gauche), _hauteur(K2->droit));
        K1->hauteur = 1 + std::max(_hauteur(K1->gauche), K2->hauteur);
        K2 = K1;
    }

    /*
       *\brief     Méthode qui effectue un ZigZig a droite
   */
    void DicoSynonymes::_zigZigDroit(DicoSynonymes::NoeudDicoSynonymes *&K2) {
        NoeudDicoSynonymes *K1 = K2->droit;
        K2->droit = K1->gauche;
        K1->gauche = K2;
        K2->hauteur = 1 + std::max(_hauteur(K2->droit), _hauteur(K2->gauche));
        K1->hauteur = 1 + std::max(_hauteur(K1->droit), K2->hauteur);
        K2 = K1;
    }

    /*
       *\brief     Méthode qui effectue un ZigZag a gauche
   */
    void DicoSynonymes::_zigZagGauche(DicoSynonymes::NoeudDicoSynonymes *&K3) {
        _zigZigDroit(K3->gauche);
        _zigZigGauche(K3);
    }

    /*
      *\brief     Méthode qui effectue un ZigZag a droite
   */
    void DicoSynonymes::_zigZagDroit(DicoSynonymes::NoeudDicoSynonymes *&K3) {
        _zigZigGauche(K3->droit);
        _zigZigDroit(K3);
    }


    /*
     *\brief  Ajouter une flexion (motFlexion) d'un radical (motRadical) à sa liste de flexions.
     *
     *\pre    Il y a suffisament de mémoire.
     *
     *\post   La flexion est ajoutée au dictionnaire des synonymes.
     *
     *\exception bad_alloc si il n'y a pas suffisament de mémoire.
     *\exception logic_error si motFlexion existe déjà ou motRadical n'existe pas.
     */
    void DicoSynonymes::ajouterFlexion(const std::string &motRadical, const std::string &motFlexion) {

        //On trouve le ptr qui pointe sur le noeud qui nous interesse
        NoeudDicoSynonymes *noeud = _rechercherArbre(motRadical);

        //Il faut lancer une erreur de logique si la flexion est deja dans la liste
        if (std::find(noeud->flexions.begin(), noeud->flexions.end(), motFlexion) != noeud->flexions.end()) {
            throw std::logic_error("Laflexion existe deja");
        }
        //Sinon, on ajoute la flexion à la liste
        noeud->flexions.push_back(motFlexion);

    }

    /*
      *\brief     Méthode qui retourne un ptr sur le noued du radical que l'on cherche
      *\exception logic_error si le dictionnaire est vide.
      * \exception logic_error si le dictionnaire ne contient pas le radical donné
   */
    DicoSynonymes::NoeudDicoSynonymes *DicoSynonymes::_rechercherArbre(const std::string &radical) const{

        //Si le dictionnaire est vide, on lance une erreur
        if (estVide()) throw std::logic_error("_rechercherArbre: Aucun radical pour lequel ajouter la flexion");

        //Initialisation d'un pointeur qui nous servira dans cette fonction
        NoeudDicoSynonymes *ptrActu = racine;

        while (ptrActu->radical != radical) {
            //Si mon radical actuel est plus grand que celui rechercher et qu'il a un fils a gauche
            //mon pointeur actuel pointera maintenant sur le fils gauche
            if (ptrActu->radical > radical && ptrActu->gauche) {
                ptrActu = ptrActu->gauche;
            }
                //Si mon radical actuel est plus grand que celui rechercher et qu'il a un fils a droite
                //mon pointeur actuel pointera maintenant sur le fils droite
            else if (ptrActu->radical < radical && ptrActu->droit) {
                ptrActu = ptrActu->droit;
            } else
                throw std::logic_error("_rechercherArbre: Le dictionnaire ne contient pas le radical donné");
        }

        return ptrActu;
    }

    /*
      *\brief     Méthode qui retourne un ptr sur le noued du radical que l'on cherche
      *\exception logic_error si le dictionnaire est vide.
      * \exception logic_error si le dictionnaire ne contient pas le radical donné
   */
    bool DicoSynonymes::_estPresent(const std::string &radical) {

        //Si le dictionnaire est vide, on lance une erreur
        if (estVide()) throw std::logic_error("_rechercherArbre: Aucun radical pour lequel ajouter la flexion");

        //Initialisation d'un pointeur qui nous servira dans cette fonction
        NoeudDicoSynonymes *ptrActu = racine;

        while (ptrActu->radical != radical) {
            //Si mon radical actuel est plus grand que celui rechercher et qu'il a un fils a gauche
            //mon pointeur actuel pointera maintenant sur le fils gauche
            if (ptrActu->radical > radical && ptrActu->gauche) {
                ptrActu = ptrActu->gauche;
            }
                //Si mon radical actuel est plus grand que celui rechercher et qu'il a un fils a droite
                //mon pointeur actuel pointera maintenant sur le fils droite
            else if (ptrActu->radical < radical && ptrActu->droit) {
                ptrActu = ptrActu->droit;
            } else
                return false;
        }

        return true;
    }

    /*
      *\brief  Ajouter un synonyme (motSynonyme) d'un radical (motRadical)
      *\brief  à un de ses groupes de synonymes.
      *
      *\pre    Le radical se trouve déjà dans le dictionnaire
      *\       Il y a suffisament de mémoire
      *
      *\post   Le synonyme est ajouté au dictionnaire des synonymes
      *\       Si numGroupe vaut –1, le synonyme est ajouté dans un nouveau groupe de synonymes
      *\       et retourne le numéro de ce nouveau groupe dans numgroupe par référence.
      *
      *\exception bad_alloc si il n'y a pas suffisament de mémoire
      *\exception logic_error si motSynonyme est déjà dans la liste des synonymes du motRadical
      *\exception logic_error si numGroupe n'est pas correct ou motRadical n'existe pas.
    */
    void DicoSynonymes::ajouterSynonyme(const std::string &motRadical, const std::string &motSynonyme, int &numGroupe) {
        //On commence par chercher le ptr qui pointe sur le noeud qui nous interesse
        NoeudDicoSynonymes *noeud = _rechercherArbre(motRadical);

        //On traite d'abbord le cas quand le numGroupe = -1
        //Il faut créer un nouveau groupe
        if (numGroupe == -1) {
            //Nouveau groupe
            numGroupe = groupesSynonymes.size();
            //Ajout du nouveau groupe
            groupesSynonymes.emplace_back();
        } else if (numGroupe >= groupesSynonymes.size()) {
            throw std::logic_error("Le numéro du groupe de synonymes n'existe pas");
        }
        if (!(_estPresent(motSynonyme))) {
            ajouterRadical(motSynonyme);
        }
        //On crée ensuite un ptr qui pointe sur le noeud de notre synonyme
        NoeudDicoSynonymes *noeudSynonyme = _rechercherArbre(motSynonyme);
        //On vérifie ensuite si le synonyme est deja present dans la liste de synonyme
        if (std::find(groupesSynonymes[numGroupe].begin(),
                      groupesSynonymes[numGroupe].end(), noeudSynonyme) !=
            groupesSynonymes[numGroupe].end()) {
            throw std::logic_error("Synonyme déjà présent dans la liste de synonymes pour le radical");
        }

        //On ajoute l'indice du grp au tableau des indices de notre radical et du synonyme
        if(std::find(noeud->appSynonymes.begin(),noeud->appSynonymes.end(),numGroupe) == noeud->appSynonymes.end())
        {
            //On La,jout seulement s'il ne s'y trouve pas deja!
            noeud->appSynonymes.push_back(numGroupe);
        }

        noeudSynonyme->appSynonymes.push_back(numGroupe);
        //On ajoute le pointeur du noeud synonyme et du noeud radical (s'il n'y est pas deja) au groupe de synonyme
        if (std::find(groupesSynonymes[numGroupe].begin(), groupesSynonymes[numGroupe].end(), noeud) ==
            groupesSynonymes[numGroupe].end()) {
            groupesSynonymes[numGroupe].push_back(noeud);
        }

        groupesSynonymes[numGroupe].push_back(noeudSynonyme);

    }

    /*
      *\brief     Supprimer un radical du dictionnaire des synonymes
      *\brief     tout en s’assurant de maintenir l'équilibre de l'arbre.
      *
      *\pre    motRadical fait parti de l'arbre.
      *
      *\post   Si le radical appartient au dictionnaire, on l'enlève et on équilibre.
      *\       Il faut libérer la liste des synonymes
      *\       du radical en question ainsi que sa liste des flexions.
      *\       Il faut aussi s’assurer d’éliminer tout pointeur
      *\       pointant sur le nœud contenant le radical.
      *
      *\exception logic_error si l'arbre est vide ou motRadical n'existe pas.
      */
    void DicoSynonymes::supprimerRadical(const std::string &motRadical) {
        //On vérifie d'abbord si l'arbre est vide
        if (estVide()) throw std::logic_error("supprimerRadical : L'arbre est vide");
        _auxSupprimerRadical(racine, motRadical);
    }

    /*
      *\brief     Méthode auxilère (récursive) de supprimerRadical
      */
    void DicoSynonymes::_auxSupprimerRadical(DicoSynonymes::NoeudDicoSynonymes *&arbre, const std::string &mot) {
        //On lance une erreur si le radical n'existe pas
        if (arbre == nullptr) throw std::logic_error("Le radical n'existe pas");

        if (mot < arbre->radical) _auxSupprimerRadical(arbre->gauche, mot);
        else if (arbre->radical < mot) _auxSupprimerRadical(arbre->droit, mot);

        else if (arbre->gauche != nullptr && arbre->droit != nullptr) {
            _supprimerSuccDroit(arbre);
        } else {
            NoeudDicoSynonymes *vieuxNoeud = arbre;
            arbre = (arbre->gauche != nullptr) ? arbre->gauche : arbre->droit;
            delete vieuxNoeud;
            --nbRadicaux;
        }

        _balancer(arbre);

    }

    /*
      *\brief     Méthode utilisée dans supprimerRadical et qui cherche le successeur minimal droit et l'enlève
      */
    void DicoSynonymes::_supprimerSuccDroit(DicoSynonymes::NoeudDicoSynonymes *data) {

        NoeudDicoSynonymes *temp = data->droit;
        NoeudDicoSynonymes *parent = data;

        while (temp->gauche != nullptr) {
            parent = temp;
            temp = temp->gauche;
        }

        data->radical = temp->radical;

        if (temp == parent->gauche)
            _auxSupprimerRadical(parent->gauche, temp->radical);
        else
            _auxSupprimerRadical(parent->droit, temp->radical);

    }

    /*
      *\brief   Supprimer une flexion (motFlexion) d'un radical
      *\brief   (motRadical) de sa liste de flexions.
      *
      *\pre    motRadical et motFlexion font parti de l'arbre.
      *
      *\post   La flexion est enlevée du dictionnaire des synonymes.
      *
      *\exception logic_error si si l'arbre est vide ou
      *\exception motFlexion n'existe pas ou motRadical n'existe pas.
      */
    void DicoSynonymes::supprimerFlexion(const std::string &motRadical, const std::string &motFlexion) {

        if (estVide()) throw std::logic_error("L'arbre est vide");
        //Création d'un pointeur
        NoeudDicoSynonymes *Noeud = _rechercherArbre(motRadical);
        //On se crée un itérateur de notre flexion
        auto flex = std::find((Noeud->flexions).begin(), (Noeud->flexions).end(), motFlexion);

        if (flex == (Noeud->flexions).end()) {
            //On lance une erreur si la flexion spécifiée n'est pas dans la liste de flexion du radical
            throw std::logic_error("supprimerFlexion : La flexion n'existe pas");
        }
        //On efface la flexion si aucune erreur
        Noeud->flexions.erase(flex);

    }

    /*
      *\brief   Retirer motSynonyme faisant partie du numéro de groupe numGroupe du motRadical.
      *
      *\pre    motRadical et motSynonyme existent et motRadical
      *\pre    a une appartenance au groupe numGroupe
      *
      *\post      Le synonyme est enlevé du dictionnaire des synonymes.
      *
      *\exception logic_error si motSynonyme ou motRadical ou numGroupe n'existent pas.
      */
    void
    DicoSynonymes::supprimerSynonyme(const std::string &motRadical, const std::string &motSynonyme, int &numGroupe) {

        //On va chercher le noeud de notre radical
        NoeudDicoSynonymes *NoeudRad = _rechercherArbre(motRadical);

        //On lance une erreur si le groupe n'existe pas
        if (numGroupe >= NoeudRad->appSynonymes.size()) throw std::logic_error("Le groupe donné n'exsiste pas");

        bool trouve = false;
        //On itere sur les synomymes du groupe et on supprimer le synonyme voulu
        for (auto syn = groupesSynonymes[numGroupe].begin(); syn != groupesSynonymes[numGroupe].end(); syn++) {
            if ((*syn)->radical == motSynonyme) {
                //On efface le synonyme qd on le trouve
                groupesSynonymes[numGroupe].erase(syn);
                trouve = true;
                break;
            }
        }
        //On lance une erreur si on a pas trouver le synonyme dans le groupe
        if (!trouve) {
            throw std::logic_error("Le synonyme n'est pas dans la liste des synonymes pour ce radical");
        }
    }

    /*
     *\brief     Vérifier si le dictionnaire est vide
     *
     *\post      Le dictionnaire est inchangée
     *
     */
    bool DicoSynonymes::estVide() const {
        //On retourne true si la racine est vide, false si elle ne l'est pas
        return !(racine);
    }

    /**
      * \brief Retourne le nombre de radicaux dans le dictionnaire
      *
      * \post Le dictionnaire reste inchangé.
      *
      */
    int DicoSynonymes::nombreRadicaux() const {
        return nbRadicaux;
    }

    /**
      * \brief Retourne le radical du mot entré en paramètre
      *
      * \pre mot est dans la liste des flexions d'un radical
      *
      * \post Le dictionnaire reste inchangé.
      * \post Le radical est retournée.
      *
      * \exception logic_error si l'arbre est vide
      * \exception logic_error si la flexion n'est pas dans la liste de flexions du radical
      *
      */
    std::string DicoSynonymes::rechercherRadical(const std::string &mot) const {

        if(estVide()) throw std::logic_error("RechercherRadical: L'arbre est vide");
        //NOeud courrant
        NoeudDicoSynonymes *Noeud = racine;
        //On itere sur les Noeud de notre arbre et on cherche le radical
        for(int i = 0; i<nbRadicaux;++i)
        {
            if(similitude(Noeud->radical,mot)>0.1)
            {
                if(Noeud->radical == mot)
                {
                    return Noeud->radical;
                }
                if(std::find(Noeud->flexions.begin(),Noeud->flexions.end(),mot) != Noeud->flexions.end())
                {
                    return Noeud->radical;
                }
            }
            //On a atteint la fin de l'arbre
            if(!Noeud->gauche) break;
            //Sinon on met a jour le Noeud courrant
            else if(Noeud->radical > mot) Noeud = Noeud->gauche;
            else if(Noeud->radical < mot) Noeud = Noeud->droit;
        }

        //Si la flexion n'a pas été trouvée
        throw std::logic_error("RechercherRadical : La flexion n'existe pas");
    }

    /**
      * \brief Retourne un réel entre 0 et 1 qui représente le degré de similitude entre mot1 et mot2 où
      *        0 représente deux mots complétement différents et 1 deux mots identiques.
      *        Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
      *
      * \post Un réel entre 0 et 1 est retourné
      * \post Le dictionnaire reste inchangé.
      *
      */
    float DicoSynonymes::similitude(const std::string &mot1, const std::string &mot2) const {
        //On calcul la longueur max de nos mots
        int longueurMax = std::max(mot1.size(), mot2.size());
        //On trouve la distance entre le deux avec la méthode de levenshtein
        int distance = _levenshtein(mot1,mot2,mot1.size(),mot2.size());
        //std::cout << distance << std::endl;
        //On calcul et retourne notre indice
        return 1.0 - (float)distance/longueurMax;
    }

    /**
      * \brief Méthode de Levenshtein qui retourne une mesure de similatité (distance) entre deux string.
      * Cette méthode sera utilisée par la méthode similitude
      *
      * Source du code pris sur le web : https://www.geeksforgeeks.org/introduction-to-levenshtein-distance/
      *
      */
    int DicoSynonymes::_levenshtein(const std::string &str1, const std::string &str2,int m, int n) const{
        // str1 is empty
        if (m == 0) {
            return n;
        }
        // str2 is empty
        if (n == 0) {
            return m;
        }

        if (str1[m - 1] == str2[n - 1]) {
            return _levenshtein(str1, str2, m - 1,
                                        n - 1);
        }

        return 1+ std::min(
                // Insert
                _levenshtein(str1, str2, m, n - 1),
                std::min(
                        // Remove
                        _levenshtein(str1, str2, m - 1,n),

                        // Replace
                        _levenshtein(str1, str2, m - 1,n - 1)));
    }

/**
      * \brief Donne le nombre de cellules de appSynonymes.
      *
      * \post Le nombre de cellules de appSynonymes pour le radical entré en paramètre est retourné
      * \post Le dictionnaire reste inchangé.
      *
      */
    int DicoSynonymes::getNombreSens(std::string radical) const {
        //On retourne simplement la taille du vecteur appSynonynes de notre radical
        return _rechercherArbre(radical)->appSynonymes.size();
    }

/**
  * \brief Donne le premier synonyme du groupe de synonyme de l'emplacement entrée en paramètre.
  *
  * \post un string correspondant au premier synonyme est imprimé
  * \post Le dictionnaire reste inchangé.
  *
  */
    std::string DicoSynonymes::getSens(std::string radical, int position) const {
        //On trouve le noeud du radical
        NoeudDicoSynonymes *Noeud = _rechercherArbre(radical);

        int index = Noeud->appSynonymes.at(position);

        //On S'assure qu'il y a des synonymes à cet endroit
        if (groupesSynonymes[index].empty()) {
            throw std::logic_error("getSens: Pas de synonyme à cet index");
        }

        // Iterate through the list to find the first synonym that is different from the radical, if possible
        for (const auto& syn : groupesSynonymes[index]) {
            if (syn->radical != radical) {
                //On retorune le premier synonyme different du radical
                return syn->radical;
            }
        }

        //On retourne le premier element s'il y a juste un synonyme dans la liste et qu'il s'agit du radical
        return  (*groupesSynonymes[Noeud->appSynonymes.at(position)].begin())->radical;
    }

/**
 * \brief Donne tous les synonymes du mot entré en paramètre du groupeSynonyme du parametre position
 *
 * \post un vecteur est retourné avec tous les synonymes
 * \post Le dictionnaire reste inchangé.
 *
 */
    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const {
        std::vector<std::string> synonymes;
        NoeudDicoSynonymes *Noeud = _rechercherArbre(radical);

        for(auto it = groupesSynonymes[Noeud->appSynonymes.at(position)].begin();it != groupesSynonymes[Noeud->appSynonymes.at(position)].end();++it)
        {
            //On ne veut pas retourner le radical lui-mm
            if((*it)->radical != radical)
            {
                synonymes.push_back((*it)->radical);
            }

        }

        return synonymes;
    }

/**
  * \brief Donne toutes les flexions du mot entré en paramètre
  *
  * \post un vecteur est retourné avec toutes les flexions
  * \post Le dictionnaire reste inchangé.
  *
  */
    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const {
        std::vector<std::string> flexions;
        NoeudDicoSynonymes *Noeud = _rechercherArbre(radical);

        for(const auto& it : Noeud->flexions)
        {
            flexions.push_back(it);
        }
        return flexions;
    }

/**
  * \brief Indique si l'arbre est équilibré selon le facteur HB(1)
  *
  * \post L'arbre est inchangé
  * \return bool indiquant si l'arbre est AVL
  *
  */
    bool DicoSynonymes::estArbreAVL() const {

        return _estArbreAVL(racine);
    }

    /**
  * \brief Méthode récursive utilisée par estArbreAVL
  *
  */
    bool DicoSynonymes::_estArbreAVL(NoeudDicoSynonymes *noeud) const{
        //On regarde d'abord si l'Arbre est vide, si oui -> AVL
        if(noeud == nullptr)
        {
            return true;
        }

        //On check la heuteur des deux bords de l'arbre
        int hauteurGauche = _hauteur(noeud->gauche);
        int hauteurDroite = _hauteur(noeud->droit);

        //Si la valeur abs de la diff entre les deux hauteur > 1 -> Pas AVL
        if(std::abs(hauteurGauche-hauteurDroite)>1)
        {
            return false;
        }

        //Appel récursif qui vérifie les sous arbres de l'arbre actuel
        return _estArbreAVL(noeud->gauche) && _estArbreAVL(noeud->droit);
    }

// Mettez l'implantation des autres méthodes (surtout privées) ici.

}//Fin du namespace