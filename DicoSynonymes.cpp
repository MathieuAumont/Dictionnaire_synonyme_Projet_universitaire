/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Mathiue Aumont, 537196633
* \version 0.1
* \date juillet 2024
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"

#include <algorithm>
#include <map>
#include <cassert>

namespace TP3 {
    /**
     * \brief Concstructeur DicoSynonymes
     *		initialisation des membres privées de la classe
     */
    DicoSynonymes::DicoSynonymes() : racine(nullptr), nbRadicaux(0), groupesSynonymes(0) {
    }

    /**
     * \brief Constructeur d'un DicoSynonymes à partir d'un fichier
     * \param[in] fichier
     */
    DicoSynonymes::DicoSynonymes(std::ifstream &fichier) {
        if (fichier.is_open()) {
            chargerDicoSynonyme(fichier);
        }
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

    /**
     * \brief Destructeur de la Classe
     *		permet de suprimer tous les noeuds non supprimer pour libérer l'espace
     */
    DicoSynonymes::~DicoSynonymes() {
        _DetruireRadical(racine);
    }

    /**
     * \brief permet d'ajouter un Radical à la classe
     *		appelle la méthode privée _InsererRaducal()
     * \param[in] motRadical std::string
     */
    void DicoSynonymes::ajouterRadical(const std::string &motRadical) {
        _InsererRadical(racine, motRadical);
    }

    /**
     *
     * \brief permet d'ajouter une flexion sur un radical déjè dans le dico.
     * \param[in] motRadical std::string le mot dont on veut ajouter une flexion
     * \param[in] motFlexion std::strin la flexion à ajouter
     * \exception std::logic_error si le dictionnaire est vide.
     * \exception std::logic_error si le radical n'existe pas
     * \exception std::logic_error si le radical existe déjà
     */
    void DicoSynonymes::ajouterFlexion(const std::string &motRadical, const std::string &motFlexion) {
        //gérer exception
        if (nombreRadicaux() == 0) throw std::logic_error("Le dictionnaire est vide.");
        if (rechercheMotRadical(racine, motRadical) == nullptr) throw std::logic_error("Mot radical n'existe pas");

        NoeudDicoSynonymes *motCourant = rechercheMotRadical(racine, motRadical);

        if (motCourant->flexions.empty() || std::find(motCourant->flexions.begin(), motCourant->flexions.end(),
                                                      motFlexion) == motCourant->flexions.end()) {
            motCourant->flexions.push_back(motFlexion);
        } else throw std::logic_error("La flexion existe déjà");
    }

    /**
     *
     * \brief permet d'ajouter un synonyme dans une groupe appartenant à un radica;
     * \param[in] motRadical std::string le radical qu'on veut ajouter un synonyme
     * \param[in] motSynonyme std::string le synonyme à ajouter
     * \param[in] numGroupe	int le numéro du groupe de synonyme dans notre dico
     * \exception[in] std::logic_error le radical n'existe pas
     * \exception[in] std::logic_error le synonyme existe déjà
     */
    void DicoSynonymes::ajouterSynonyme(const std::string &motRadical, const std::string &motSynonyme, int &numGroupe) {

        if (!appartient(racine, motRadical)) throw std::logic_error("Le radical souhaité n'existe pas.");
        if (!appartient(racine, motSynonyme)) ajouterRadical(motSynonyme);

        NoeudDicoSynonymes *radical = rechercheMotRadical(racine, motRadical);
        NoeudDicoSynonymes *synonyme = rechercheMotRadical(racine, motSynonyme);

        if (numGroupe == -1) {
            groupesSynonymes.resize(groupesSynonymes.size() + 1);
            numGroupe = groupesSynonymes.size() - 1;
            radical->appSynonymes.push_back(numGroupe);
        }

        //tout ça pour savoir si le synonyme est déjà dans le groupe
        for (auto groupe: radical->appSynonymes) {
            if (groupe == numGroupe) {
                for (auto const radical: groupesSynonymes[groupe]) {
                    if (radical == synonyme) {
                        throw std::logic_error("le synonyme est déjà dans le groupe");
                    }
                }
                groupesSynonymes[groupe].push_back(synonyme);
            }
        }
    }

    /**
     * \brief méthose public appellant méthode privé _EnleverRadical
     * \param[in] motRadical std::string le radical à enlever
     */
    void DicoSynonymes::supprimerRadical(const std::string &motRadical) {
        _EnleverRadical(racine, motRadical);
    }

    /**
     * \brief méthode public appellant la méthode privée _EnleverFlexion()
     * \param[in] motRadical std::string le radical dont on veut enlever une flexion
     * \param[in] motFlexion std::string la flexion en question
     */
    void DicoSynonymes::supprimerFlexion(const std::string &motRadical, const std::string &motFlexion) {

        NoeudDicoSynonymes *motCourant = rechercheMotRadical(racine, motRadical);

        if (std::find(motCourant->flexions.begin(), motCourant->flexions.end(), motFlexion) == motCourant->flexions.
            end()) {
            throw std::logic_error("La flexion choisie n'existe pas.");
        } else {
            motCourant->flexions.erase(std::find(motCourant->flexions.begin(), motCourant->flexions.end(), motFlexion));
        }
    }

    /**
     * \brief suppression d'un synonyme dans un groupe d'un radical
     * \param[in] motRadical std::string le radical
     * \param[in] motSynonyme std::string le synonyme qu'on veut enlever
     * \param[in] numGroupe  le numero du groupe de synonyme
     * \exception std::logic_error le radical n'existe pas
     * \exception std::logic_error le synonyme n'existe pas
     * \exception std::logic_error le synonyme ne fait pas parti du groupe.
     */
    void DicoSynonymes::supprimerSynonyme(const std::string &motRadical, const std::string &motSynonyme,
                                          int &numGroupe) {
        //gérer les exceptions
        if (!appartient(racine, motRadical)) throw std::logic_error("Le radical souhaité n'existe pas.");
        if (!appartient(racine, motSynonyme)) throw std::logic_error("Le synonyme n'existe pas.");

        NoeudDicoSynonymes *radical = rechercheMotRadical(racine, motRadical);
        NoeudDicoSynonymes *synonyme = rechercheMotRadical(racine, motSynonyme);

        for (auto groupe: radical->appSynonymes) {
            if (groupe == numGroupe) {
                auto radicalSynonyme = std::find(groupesSynonymes[groupe].begin(), groupesSynonymes[groupe].end(),
                                                 synonyme);
                if (radicalSynonyme != groupesSynonymes[groupe].end()) {
                    groupesSynonymes[groupe].erase(radicalSynonyme);
                } else {
                    throw std::logic_error("le synonyme n'existe pas dans ce groupe");
                }
            }
        }
    }

    /**
     * \brief permet de voir si le Dico est vide
     * \return bool true si vide, false si non_vide
     */
    bool DicoSynonymes::estVide() const {
        return nbRadicaux == 0;
    }

    /**
     * \brief renvoie le nombre de radicaux dans le dico
     * \return int le nombre de radicaux du dico
     */
    int DicoSynonymes::nombreRadicaux() const {
        return nbRadicaux;
    }

    /**
     * \brief permet te trouver le radical d'un mot
     * \param[in] mot std::string la flexion qu'on veut associé au radical
     * \return std::string le radical trouvé
     * \exception std::logic_error Dictionnaire vide
     * \exception std::logic_error flexion innexistante dans la liste de flexion du radical trouvé
     */
    std::string DicoSynonymes::rechercherRadical(const std::string &mot) const {

        if (nbRadicaux == 0) throw std::logic_error("Le Dictionnaire est vide");

        std::map<std::string, double> valeurDico = parcoursDico(mot);
        std::string resultat;

        for (auto element: valeurDico) {
            if (resultat.empty()) {
                resultat = element.first;
            } else if (element.second > valeurDico[resultat]) resultat = element.first;
        }
        NoeudDicoSynonymes *radical = rechercheMotRadical(racine, resultat);

        if (std::find(radical->flexions.begin(), radical->flexions.end(), mot) == radical->flexions.end())
            throw std::logic_error("la flexion n'est pas dans la liste de flexion du radical.");

        return resultat;
    }

    /**
     * \brief trouver le rapport de similitude entre deux strings
     *		plus on s'approche de 1, plus le mot est similaire.
     *		plus on est près de 0, moins le mot est semblable
     *
     *		j'ai fait ma propre méthode en m'inspirant de truc trouver sur Leveinsthein et autres tentatives
     *		de similitude sur GitHub. Je trouve ma version plus simpliste. Peut-être trop simple pour un cas
     *		pluis imposant, mais je tenais à réfléchir un peu sur comment faire.
     * \param[in] mot1 std::string un mot à comparer
     * \param[in] mot2 std::string un mot à comparer
     * \return double un nombre réel entre 0 et 1.
     */
    double DicoSynonymes::similitude(const std::string &mot1, const std::string &mot2) {

        double resultat = 0.0;
        int max_size = std::max(mot1.size(), mot2.size());
        int min_size = std::min(mot1.size(), mot2.size());

        for (auto i = 0; i < min_size; i++)
            if (mot1[i] == mot2[i])
                resultat++;

        return resultat / max_size;
    }

    /**
     * \brief méthode permettant de savoir le nombre de sens d'un mot
     * \param[in] radical std::string le radical dont on cherche le nombre de sens
     * \return int un nombre entier.
     */
    int DicoSynonymes::getNombreSens(std::string radical) const {

        NoeudDicoSynonymes *rad = rechercheMotRadical(racine, radical);

        return rad->appSynonymes.size();
    }

    /**
     * \brief méthode permettant d'avoir le premier mot dans la liste des synonymes d'un radical
     * \param[in] radical std::string le radical dont on cherche le premier synonuyme du sens choisi
     * \param[in] position int représente le numéro du groupe des synonymes
     * \return std::string le premier mot du synonyme
     */
    std::string DicoSynonymes::getSens(std::string radical, int position) const {

        NoeudDicoSynonymes *rad = rechercheMotRadical(racine, radical);
        std::string synonyme = groupesSynonymes[rad->appSynonymes[position]].front()->radical;

        return synonyme;
    }

    /**
     * \brief méthode permettant d'avoir tous les synonymes d'un mot
     * \param[in] radical std::string le mot radical
     * \param[in] position int le numéro du groupe de synonyme
     * \return std::vector<std::string> tous les synonymes du sens demandé.
     */
    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const {

        NoeudDicoSynonymes *rad = rechercheMotRadical(racine, radical);
        std::vector<std::string> synonymes;

        for (auto const syn: groupesSynonymes[rad->appSynonymes[position]]) {
            synonymes.push_back(syn->radical);
        }

        return synonymes;
    }

    /**
     * \brief méthode permettant d'avoir toutes les flexions enregistrées d'un mot
     * \param[in] radical std::string le mot en question
     * \return std::vector<std::string> un liste de toutes les flexions
     */
    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const {

        NoeudDicoSynonymes *rad = rechercheMotRadical(racine, radical);
        std::vector<std::string> flexions;

        for (auto const flex: rad->flexions)
            flexions.push_back(flex);
        if (flexions.empty()) flexions.push_back(radical);

        return flexions;
    }

    /**
     * \brief méthode permettant de déterminer si notre dictionnaire (arbre) est équilibré
     *		appelle sa méthode privé _estArbreAvl()
     * \return bool true si équilibré
     */
    bool DicoSynonymes::estArbreAVL() const {

        bool resultat = true;
        _estArbreAvl(racine, resultat);

        return resultat;
    }

    /**
     * \brief méthode public appelant la méthode privée _parcoursDico
     * \param mot std::string un radical
     * \return std::map<std::string,double> donne la rapport de similitude(double) entre tous les mots du tableau.
     */
    std::map<std::string, double> DicoSynonymes::parcoursDico(const std::string &mot) const
    {
        std::map<std::string, double> v;
        _parcoursDico(racine, v, mot);

        return v;
    }

    /**
     * \brief méthode permettant de trouver le noeud comportant le radical entré en paramèetre
     * \param noeud NoeudDicoSynonymes* pointant sur la racine de l'arbre
     * \param[in] motRadical std::string le mot que l'on veut associé au noeud
     * \return NoeudDicoSynonyme * pointe sur le noeud
     */
    DicoSynonymes::NoeudDicoSynonymes *DicoSynonymes::rechercheMotRadical(NoeudDicoSynonymes *noeud,
                                                                          const std::string &motRadical) const {
        if (noeud == nullptr) return nullptr;

        if (motRadical < noeud->radical)
            return rechercheMotRadical(noeud->gauche, motRadical);
        else if (motRadical > noeud->radical)
            return rechercheMotRadical(noeud->droit, motRadical);
        else
            return noeud;

    }

    /**
     * \brief méthode privé confirmant l'équilibre du dico
     * \param[in] racine NoeudDicoSynonme pointant sur la racine
     * \param[in] resultat bool par référence true si équilibré
     */
    void DicoSynonymes::_estArbreAvl(NoeudDicoSynonymes *racine, bool &resultat) const {

        if (racine == nullptr) return;

        if (racine->gauche == nullptr && racine->droit == nullptr) return;

        int hauteurDroit;
        int hauteurGauche;

        if (racine->droit == nullptr) {
            hauteurDroit = -1;
        } else {
            hauteurDroit = racine->droit->hauteur;
        }

        if (racine->gauche == nullptr) {
            hauteurGauche = -1;
        } else {
            hauteurGauche = racine->gauche->hauteur;
        }

        if (hauteurGauche - hauteurDroit > 1 || hauteurGauche - hauteurDroit < -1) {
            resultat = false;
        } else {
            if (racine->gauche != nullptr)
                _estArbreAvl(racine->gauche, resultat);
            if (racine->droit != nullptr)
                _estArbreAvl(racine->droit, resultat);
        }
    }

    /**
     * \brief méthode privé de parcoursDico.
     *		passe tous les noeuds pour trouver le rapport de similitude et l'ajoute à la map
     * \param[in] racine NoeudDicoSynonymes pointant sur la racine
     * \param[in] v std::map<std::string, double> conteneur des similitudes
     * \param[in] mot std::string le mot dont on cherche le rapport de similitude
     */
    void DicoSynonymes::_parcoursDico(NoeudDicoSynonymes *racine, std::map<std::string, double> &v,
                                      const std::string &mot) const {

        if (racine != nullptr) {
            _parcoursDico(racine->gauche, v, mot);
            v[racine->radical] = similitude(racine->radical, mot);
            _parcoursDico(racine->droit, v, mot);
        }
    }

    /**
     * \brief méthode permettant d'insérer un radical dans un dico
     * \param[in] dico NouedDicoSynonyme pointant sur la racine
     * \param[in] motRadical std::string le mot à inseérer
     * \exception std::logic_error Les duplicatats sont interdits
     */
    void DicoSynonymes::_InsererRadical(NoeudDicoSynonymes *&dico, const std::string &motRadical) {

        if (dico == nullptr) {
            dico = new NoeudDicoSynonymes(motRadical);
            nbRadicaux++;
            return;

        }
        if (motRadical < dico->radical)
            _InsererRadical(dico->gauche, motRadical);
        else if (dico->radical < motRadical)
            _InsererRadical(dico->droit, motRadical);
        else
            throw std::logic_error("Les duplicatats sont interdits");

        _balancer(dico);
    }

    /**
     * \brief permet de supprimer un radical
     * \param[in] dico NoeudDicoSynonyme pointant sur la racine
     * \param[in] motRadical std::string le radical à supprimer
     * \exception std::logic_error Tentative d'enlever une donnée absente
     */
    void DicoSynonymes::_EnleverRadical(NoeudDicoSynonymes *&dico, const std::string &motRadical) {

        if (dico == nullptr) throw std::logic_error("Tentative d'enlever une donnée absente");

        if (motRadical < dico->radical)
            _EnleverRadical(dico->gauche, motRadical);
        else if (dico->radical < motRadical)
            _EnleverRadical(dico->droit, motRadical);
        else if (dico->gauche != nullptr && dico->droit != nullptr) {
            _enleverSuccMinDroite(dico);
        } else {
            NoeudDicoSynonymes *vieuxNoeud = dico;
            dico = (dico->gauche != nullptr) ? dico->gauche : dico->droit;
            delete vieuxNoeud;
            --nbRadicaux;
        }
        _balancer(dico);
    }

    /**
     * \brief méthode détruisant tout sur son passage (Remise à zéro du dico)
     * \param[in] dico NoeudDicoSynonyme pointant sur la racine
     */
    void DicoSynonymes::_DetruireRadical(NoeudDicoSynonymes* &dico) {

        if (dico == nullptr) return;

        if (dico->gauche == nullptr && dico->droit == nullptr) return;

        if (dico->gauche != nullptr)
            _DetruireRadical(dico->gauche);
        if (dico->droit != nullptr)
            _DetruireRadical(dico->droit);

        dico->flexions.clear();
        dico->appSynonymes.clear();
        delete dico;
    }

    /**
     * \brief permet d'enlever le noeud successeur minimum vers la droite
     * \param[in] dico la racine
     */
    void DicoSynonymes::_enleverSuccMinDroite(NoeudDicoSynonymes *dico) {

        NoeudDicoSynonymes *temp = dico->droit;
        NoeudDicoSynonymes *parent = dico;

        while (temp->gauche != nullptr) {
            parent = temp;
            temp = temp->gauche;
        }
        dico->radical = temp->radical;

        if (parent->gauche == temp)
            _EnleverRadical(parent->gauche, temp->radical);
        else
            _EnleverRadical(parent->droit, temp->radical);
    }

    /**
     * \brief renvoie la hauteur d'un noeud
     * \param[in] dico NouedDicoSynonyme pointant sur le noeud qu'on cherche la hauteur
     * \return int la hauteur
     */
    int DicoSynonymes::_hauteur(NoeudDicoSynonymes *dico) const {

        if (dico == nullptr)
            return -1;

        return dico->hauteur;
    }

    /**
     * \brief permet d'équilibrer un dico(arbre)
     * \param[in] dico NoeudDicoSynonyme pointant sur la racine de l'arbre
     */
    void DicoSynonymes::_balancer(NoeudDicoSynonymes *&dico) {

        if (_debalancementAGauche(dico)) {
            if (_sousArbrePencheADroite(dico->gauche)) {
                _zigZagGauche(dico);
            } else {
                _zigZigGauche(dico);
            }
        } else if (_debalancementADroite(dico)) {
            if (_sousArbrePencheAGauche(dico->droit)) {
                _zigZagDroit(dico);
            } else {
                _zigZigDroit(dico);
            }
        } else {
            if (dico != nullptr)
                dico->hauteur = 1 +
                                std::max(_hauteur(dico->gauche), _hauteur(dico->droit));
        }
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le balancement par ziziggauche
     * \param[in] dico NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    void DicoSynonymes::_zigZigGauche(NoeudDicoSynonymes *&dico) {

        NoeudDicoSynonymes *K1 = dico->gauche;
        dico->gauche = K1->droit;
        K1->droit = dico;

        dico->hauteur = 1 + std::max(_hauteur(dico->gauche), _hauteur(dico->droit));
        K1->hauteur = 1 + std::max(_hauteur(K1->gauche), dico->hauteur);

        dico = K1;
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le balancement par zizigdroit
     * \param[in] dico NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    void DicoSynonymes::_zigZigDroit(NoeudDicoSynonymes *&dico) {

        NoeudDicoSynonymes *K1 = dico->droit;
        dico->droit = K1->gauche;
        K1->gauche = dico;

        dico->hauteur = 1 + std::max(_hauteur(dico->droit), _hauteur(dico->gauche));
        K1->hauteur = 1 + std::max(_hauteur(K1->droit), dico->hauteur);

        dico = K1;
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le balancement par zizaggauche
     * \param[in] dico NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    void DicoSynonymes::_zigZagGauche(NoeudDicoSynonymes *&dico) {

        _zigZigDroit(dico->gauche);
        _zigZigGauche(dico);
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le balancement par zizagdroit
     * \param[in] dico NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    void DicoSynonymes::_zigZagDroit(NoeudDicoSynonymes *&dico) {

        _zigZigGauche(dico->droit);
        _zigZigDroit(dico);
    }

    /**
     * \brief méthode vue en classe pour déterminer si un mot appartient à l'arbre
     * \param[in] racine noeudDicoSynonyme pointant sur la racine
     * \param[in] motRadical std::string le mot que l'on cherche
     * \return bool oui ou non il existe
     */
    bool DicoSynonymes::appartient(NoeudDicoSynonymes *racine, const std::string &motRadical) const {

        if (racine == nullptr)
            return false;

        if (motRadical < racine->radical)
            return appartient(racine->gauche, motRadical);
        else if (racine->radical < motRadical)
            return appartient(racine->droit, motRadical);
        else
            return true;
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le debalancementAGauche d'un noeud critique
     * \param[in] arbre NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes *arbre) const {

        if (arbre == nullptr)
            return false;
        return 1 < _hauteur(arbre->gauche) - _hauteur(arbre->droit);
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le debalancementADroite d'un noeud critique
     * \param[in] arbre NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes *arbre) const {

        if (arbre == nullptr)
            return false;
        return 1 < _hauteur(arbre->droit) - _hauteur(arbre->gauche);
    }

    /**
     * \brief la méthode quasi exact vu en classe sur le debalancementAGauche d'un sous-arbre
     * \param[in] arbre NoeudDicoSynonuyme pointant sur la sime de l'arbre
     */
    bool DicoSynonymes::_sousArbrePencheAGauche(NoeudDicoSynonymes *arbre) const {

        if (arbre == nullptr)
            return false;
        return _hauteur(arbre->droit) < _hauteur(arbre->gauche);
    }

    /**
    * \brief la méthode quasi exact vu en classe sur le debalancementADroite d'un sous-arbre
    * \param[in] arbre NoeudDicoSynonuyme pointant sur la sime de l'arbre
    */
    bool DicoSynonymes::_sousArbrePencheADroite(NoeudDicoSynonymes *arbre) const {

        if (arbre == nullptr)
            return false;
        return _hauteur(arbre->gauche) < _hauteur(arbre->droit);
    }
} //Fin du namespace
