/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Étudiant(e)
* \version 0.1
* \date juillet 2024
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"

#include <algorithm>
#include <map>

namespace TP3
{

    DicoSynonymes::DicoSynonymes() : racine(nullptr), nbRadicaux(0), groupesSynonymes(0){
    }

    DicoSynonymes::DicoSynonymes(std::ifstream &fichier) {
    	if (fichier.is_open())
    	{
    		chargerDicoSynonyme(fichier);
    	}
    }

	// Méthode fournie
	void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
	{
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

		std::string ligne;
		std::string buffer;
		std::string radical;
		int cat = 1;

		while (!fichier.eof()) // tant qu'on peut lire dans le fichier
		{
            std::getline(fichier, ligne);

			if (ligne == "$")
			{
				cat = 3;
				std::getline(fichier, ligne);
			}
			if (cat == 1)
			{
				radical = ligne;
				ajouterRadical(radical);
				cat = 2;
			}
			else if (cat == 2)
			{
				std::stringstream ss(ligne);
				while (ss >> buffer)
					ajouterFlexion(radical, buffer);
				cat = 1;
			}
			else
			{
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

    DicoSynonymes::~DicoSynonymes(){
    }

    void DicoSynonymes::ajouterRadical(const std::string& motRadical){
    	_InsererRadical(racine, motRadical);
    }

    void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion){
    	if(nombreRadicaux() == 0) throw std::logic_error("Le dictionnaire est vide.");
    	if (rechercheMotRadical(racine, motRadical) == nullptr ) throw std::logic_error("Mot radical n'existe pas");
    	NoeudDicoSynonymes* motCourant = rechercheMotRadical(racine, motRadical);

    	if (motCourant->flexions.empty() || std::find(motCourant->flexions.begin(), motCourant->flexions.end(), motFlexion) == motCourant->flexions.end())
    	{
    		motCourant->flexions.push_back(motFlexion);
    	}
    	else throw std::logic_error("La flexion existe déjà");

    }

    void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe) {
	    if (!appartient(racine, motRadical)) throw std::logic_error("Le radical souhaité n'existe pas.");
    	if (!appartient(racine, motSynonyme)) ajouterRadical(motSynonyme);

    	if (numGroupe == -1) {
    		numGroupe = groupesSynonymes.size();
    	}

    	NoeudDicoSynonymes* radical = rechercheMotRadical(racine, motRadical);
    	NoeudDicoSynonymes* synonyme = rechercheMotRadical(racine, motSynonyme);
    	for (auto groupe : radical->appSynonymes)
    	{
    		if (groupe == numGroupe) {
    			for (auto const radical : groupesSynonymes[groupe]) {
    				if(radical == synonyme) {
    					throw std::logic_error("le synonyme est déjà dans le groupe");
    				}
    			}
    			groupesSynonymes[groupe].push_back(synonyme);
    		}
    	}
    }

    void DicoSynonymes::supprimerRadical(const std::string& motRadical){
    	_EnleverRadical(racine, motRadical );
    }

    void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion){

    	NoeudDicoSynonymes* motCourant = rechercheMotRadical(racine, motRadical);
    	if (std::find(motCourant->flexions.begin(), motCourant->flexions.end(), motFlexion) == motCourant->flexions.end())
    	{
    		throw std::logic_error("La flexion choisie n'existe pas.");
    	}
    	else
    	{
    		motCourant->flexions.erase(std::find(motCourant->flexions.begin(), motCourant->flexions.end(), motFlexion));
    	}
    }

    void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
    	if (!appartient(racine, motRadical)) throw std::logic_error("Le radical souhaité n'existe pas.");
    	if (!appartient(racine, motSynonyme)) throw std::logic_error("Le synonyme n'existe pas.");
    	NoeudDicoSynonymes* radical = rechercheMotRadical(racine, motRadical);
    	NoeudDicoSynonymes* synonyme = rechercheMotRadical(racine, motSynonyme);
    	for (auto groupe : radical->appSynonymes) {
    		if (groupe == numGroupe) {
    			auto radicalSynonyme = std::find(groupesSynonymes[groupe].begin(), groupesSynonymes[groupe].end(),synonyme);
    			if (radicalSynonyme != groupesSynonymes[groupe].end()){
    				groupesSynonymes[groupe].erase(radicalSynonyme);
    				}
    			else {
    				throw std::logic_error("le synonyme n'existe pas dans ce groupe");
    			}

    		}
    	}
    }

    bool DicoSynonymes::estVide() const{
        return nbRadicaux == 0;
    }

    int DicoSynonymes::nombreRadicaux() const{
        return nbRadicaux;
    }

    std::string DicoSynonymes::rechercherRadical(const std::string& mot) const
    {
    	if (nbRadicaux == 0) throw std::logic_error("Le Dictionnaire est vide");
		std::map<std::string,float> valeurDico  = parcoursDico(mot);
    	//utilisation d'un lambda (trouver comment faire sur internet https://en.cppreference.com/w/cpp/language/lambda)
    	auto valeurMax  = std::max_element(valeurDico.begin(), valeurDico.end(),
    		[](const std::pair<std::string,float>& p1, const std::pair<std::string, float>& p2)
    		{
    			return p1.second < p2.second;
    		});

        return valeurMax->first;
    }

    float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const
    {
    	//Inspiration de la distance de LEVENSTEIN
    	int longeurMot1 = mot1.size();
    	int longueurMot2 = mot2.size();

    	std::vector<std::vector<int>> distance(longeurMot1 + 1, std::vector<int>(longueurMot2+1,0));

    	for (size_t i = 1; i <= longeurMot1; i++) {
    		distance[i][0] = i;
    	}

    	for (size_t j = 1; j <= longueurMot2; j++) {
    		distance[0][j] = j;
    	}

    	for (size_t i = 1; i <= longeurMot1; i++) {
    		for (size_t j = 1; j <= longueurMot2; j++) {
    			int poids ;
    			(mot1[i - 1] == mot2[j - 1]) ? poids = 0: poids = 1;
    			distance[i][j] = std::min(std::min(distance[i-1][j] + 1, distance[i][j-1] + 1), distance[i-1][j-1] + poids);
    		}
    	}
		int longueurMax = std::max(mot1.size(), mot2.size());

    	return static_cast<double>(distance[longeurMot1][longueurMot2]) / longueurMax;
    }

    int DicoSynonymes::getNombreSens(std::string radical) const{
    	NoeudDicoSynonymes* rad = rechercheMotRadical(racine, radical);

        return rad->appSynonymes.size();
    }

    std::string DicoSynonymes::getSens(std::string radical, int position) const{
    	NoeudDicoSynonymes* rad = rechercheMotRadical(racine, radical);
    	std::string synonyme = groupesSynonymes[rad->appSynonymes[position]].front()->radical;
        return synonyme;
    }

    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const{
        NoeudDicoSynonymes* rad = rechercheMotRadical(racine, radical);
    	std::vector<std::string> synonymes;
    	for (auto const syn : groupesSynonymes[rad->appSynonymes[position]]) {
    		synonymes.push_back(syn->radical);
    	}
        return synonymes;
    }

    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const
    {
    	NoeudDicoSynonymes* rad = rechercheMotRadical(racine, radical);
        std::vector<std::string> flexions;
    	for (auto const flex : rad->flexions)
    		flexions.push_back(flex);
        return flexions;
    }

    bool DicoSynonymes::estArbreAVL() const{
        return true;
    }

     std::map<std::string, float> DicoSynonymes::parcoursDico(const std::string &mot) const
    {
    	std::map<std::string, float> v;
    	_parcoursDico(racine, v, mot);
    	return v;
    }

     DicoSynonymes::NoeudDicoSynonymes* DicoSynonymes::rechercheMotRadical(NoeudDicoSynonymes* noeudm,
	     const std::string& motRadical) const
     {
    	if (noeudm == nullptr) return nullptr;
    	if (motRadical < noeudm->radical)
    	{
    		return rechercheMotRadical(noeudm->gauche, motRadical);
    	}
    	else if (motRadical > noeudm->radical)
    	{
    		return rechercheMotRadical(noeudm->droit, motRadical);
    	}
    	else
    	{
    		return noeudm;
    	}
     }

    void DicoSynonymes::_parcoursDico(NoeudDicoSynonymes* racine, std::map<std::string, float> v, const std::string & mot) const
    {
	    if (racine != 0)
	    {
	    	_parcoursDico(racine->gauche, v,mot);
	    	v[racine->radical] = similitude(racine->radical, mot);
	    	_parcoursDico(racine->droit, v , mot);
	    }
    }

    	void DicoSynonymes::_InsererRadical(NoeudDicoSynonymes*& dico, const std::string& motRadical)
	    {
	    	if (dico == 0)
	    	{
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

    	void DicoSynonymes::_EnleverRadical(NoeudDicoSynonymes*& dico, const std::string& motRadical)
	    {
	    	if( dico==0 ) throw std::logic_error("Tentative dâ€™enlever une donnÃ©e absente");
	    	if( motRadical < dico->radical ) _EnleverRadical( dico->gauche, motRadical);
	    	else if( dico->radical < motRadical ) _EnleverRadical( dico->droit, motRadical);
	    	// valeur == t->data: ici on doit enlever le nÅ“ud pointÃ© par t
	    	else if( dico->gauche != 0 && dico->droit != 0 )
	    	{//cas complexe: chercher le successeur minimal droit et l'enlever
	    		_enleverSuccMinDroite(dico);
	    	}
	    	else
	    	{   //cas simples: le noeud n'a aucun enfant ou un seul enfant, il suffit de retirer
	    		// ce noeud et pointer sur l'Ã©ventuel enfant
	    		NoeudDicoSynonymes * vieuxNoeud = dico;
	    		dico = ( dico->gauche != 0 ) ? dico->gauche : dico->droit;
	    		delete vieuxNoeud;
	    		--nbRadicaux;
	    	}

	    	_balancer(dico);	//rebalancer et mise Ã  jour des hauteurs

	    }

    	void DicoSynonymes::_enleverSuccMinDroite(NoeudDicoSynonymes* dico)
	    {

	    	NoeudDicoSynonymes * temp = dico->droit;
	    	NoeudDicoSynonymes * parent = dico;
	    	while ( temp->gauche != 0)
	    	{
	    		parent = temp;
	    		temp = temp->gauche;
	    	}
	    	dico->radical = temp->radical; //Ã©crasement par le successeur minimal Ã  droite
	    	// enlever nÅ“ud (cas simple)
	    	if (parent->gauche == temp)
	    		_EnleverRadical(parent->gauche, temp->radical);
	    	else
	    		_EnleverRadical(parent->droit, temp->radical);
	    }

    	int DicoSynonymes::_hauteur(NoeudDicoSynonymes* dico) const
	    {
	    	if (dico == 0)
	    		return -1;
	    	return dico->hauteur;

	    }

    	void DicoSynonymes::_balancer(NoeudDicoSynonymes*&dico)
	    {

	    	if ( _debalancementAGauche(dico) )
	    	{

	    		if ( _sousArbrePencheADroite(dico->gauche) ) {
	    			_zigZagGauche(dico);
	    		} else {
	    			_zigZigGauche(dico);
	    		}
	    	}
	    	else if ( _debalancementADroite(dico) )
	    	{
	    		// Lorsque le dÃ©balancement est Ã  droite, on fait un zigZig lorsque le
	    		// sousArbre penche Ã  droite OU est balancÃ©
	    		if ( _sousArbrePencheAGauche(dico->droit) ) {
	    			_zigZagDroit(dico);
	    		} else {
	    			_zigZigDroit(dico);
	    		}
	    	}
	    	else //aucun dÃ©balancement, uniquement m.Ã .j. des hauteurs
	    	{
	    		if (dico != 0) dico->hauteur = 1 +
								 std::max( _hauteur(dico->gauche), _hauteur(dico->droit) );
	    	}

	    }

    	void DicoSynonymes::_zigZigGauche(NoeudDicoSynonymes*& dico)
	    {
	    	NoeudDicoSynonymes *K1 = dico->gauche;
	    	dico->gauche = K1->droit;
	    	K1->droit = dico;
	    	dico->hauteur = 1 + std::max(_hauteur(dico->gauche), _hauteur(dico->droit));
	    	K1->hauteur = 1 + std::max(_hauteur(K1->gauche), dico->hauteur);
	    	dico = K1;
	    }

    	void DicoSynonymes::_zigZigDroit(NoeudDicoSynonymes*& dico)
	    {

	    	NoeudDicoSynonymes *K1 = dico->droit;
	    	dico->droit = K1->gauche;
	    	K1->gauche = dico;
	    	dico->hauteur = 1 + std::max(_hauteur(dico->droit), _hauteur(dico->gauche));
	    	K1->hauteur = 1 + std::max(_hauteur(K1->droit), dico->hauteur);
	    	dico = K1;

	    }

    	void DicoSynonymes::_zigZagGauche(NoeudDicoSynonymes*& dico)
	    {
	    	_zigZigDroit(dico->gauche);
	    	_zigZigGauche(dico);
	    }

    	void DicoSynonymes::_zigZagDroit(NoeudDicoSynonymes*& dico)
	    {
	    	_zigZigGauche(dico->droit);
	    	_zigZigDroit(dico);
	    }

	    bool DicoSynonymes::appartient(NoeudDicoSynonymes *racine, const std::string &motRadical) const {
    	if (racine == nullptr)
    		return false; //si data nâ€™est pas dans lâ€™arbre

    	if ( motRadical < racine->radical )
    		return appartient(racine->gauche, motRadical);
    	else if ( racine->radical < motRadical)
    		return appartient(racine->droit, motRadical);
    	else
    		return true;

	    }

	    bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes* arbre) const
	    {
	    	if (arbre == 0)
	    		return false;
	    	return 1 < _hauteur(arbre->gauche) - _hauteur(arbre->droit);
	    }

    	bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes* arbre) const
	    {
	    	if (arbre == 0)
	    		return false;
	    	return 1 < _hauteur(arbre->droit) - _hauteur(arbre->gauche);
	    }

    	bool DicoSynonymes::_sousArbrePencheAGauche(NoeudDicoSynonymes* arbre) const
	    {
	    	if (arbre == 0)
	    		return false;
	    	return _hauteur(arbre->droit) < _hauteur(arbre->gauche);
	    }

    	bool DicoSynonymes::_sousArbrePencheADroite(NoeudDicoSynonymes* arbre) const
	    {
	    	if (arbre == 0)
	    		return false;
	    	return _hauteur(arbre->gauche) < _hauteur(arbre->droit);

	    }

    	// Mettez l'implantation des autres méthodes (surtout privées) ici.

}//Fin du namespace