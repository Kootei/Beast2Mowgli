// Compile sous mac : clang++ -std=c++11 -stdlib=libc++ main.cpp
// Compile sous ubuntu : g++ -Wall -std=c++11 -c main.cpp | g++ main.o -o main

#include "main.hpp" 

using namespace std;

//######### Expression Reguliere ###########
regex patternID( "[\\(|,]?([A-Za-z0-9-]+)?\\[height" );
regex patternZone( "location=([\\D ,]+)," );
regex patternProba( "location.prob=([\\d ,\\.E-]+)," );
regex nettoyageChar( "[&%_\"/]" );
regex extractionNewick( "\\[[^\\]]+\\][;|:]\\d?+\\.?\\d?+" );
//regex scoreAire( ",?([\\d\\.E-]+),?" );
//regex nomAire( ",?([A-Za-z0-9-]+),?" );

//######### Fonctions ###########
//------------------------------------------------------------------------
//recup arbre Newick dans fichier Beast
string recupNewick (const char* fileBeast) {
	string newick, arbreBrute, arbreNettoye;
	ifstream fichier( fileBeast, ios::in );
	if ( fichier ) {
		getline( fichier, arbreBrute );
		regex_replace( back_inserter( arbreNettoye ), arbreBrute.begin(), arbreBrute.end(), nettoyageChar, "" );//nettoie fichier
		regex_replace( back_inserter( newick ), arbreNettoye.begin(), arbreNettoye.end(), extractionNewick, "" );//ne garde que l'arbre newick
		newick = newick + ";";
	}
	//cout << "Arbre Newick : \n " << newick << '\n' << endl; //debug
		
	return newick;
}

//------------------------------------------------------------------------
//recup donnees geo dans fichier Beast
void lectureBeast (const char* fileName) {
	string outputArbre, firstLine; //declaration string pour manipuler
	int indiceNoeuds = 1, indiceZones = 1, indiceProbas = 1; //declaration compteurs
	string matriceDonnees[NbMaxNoeuds][4]; //declaration matrice de donne a 4 col : ID, aire, proba de cette aire, noeud pere
	for ( int i = 0; i<NbMaxNoeuds; i++ )
		for ( int j = 0; j<4; j++ )
	   	 matriceDonnees[i][j] = ""; //initialisation matrice à vide
	ifstream fichier( fileName, ios::in );
	if ( fichier ) {
		getline( fichier, firstLine );
		regex_replace( back_inserter( outputArbre ), firstLine.begin(), firstLine.end(), nettoyageChar, "" );//nettoie fichier
		
		// 1 - Recup (feuille) ou cree (noeud interne) une ID pour chaque noeud
		auto debut_s = sregex_iterator( outputArbre.begin(), outputArbre.end(), patternID );
		auto fin_s = sregex_iterator();
		sregex_iterator( outputArbre.begin(), outputArbre.end(), patternID );
		for ( sregex_iterator ite = debut_s; ite!=fin_s; ++ite ) { //itération de la recherche d'ID
			smatch imatch = *ite;			
			string idTemp = imatch.str(1);
			if ( idTemp != "" ) { // feuille avec une ID
				matriceDonnees[indiceNoeuds][0] = idTemp; //stock dans matriceDonne
			}
			else { // cas d'un noeud interne (?) sans ID
				matriceDonnees[indiceNoeuds][0] = "Noeud-" + to_string(indiceNoeuds); //stock dans matriceDonne
			}
			indiceNoeuds++; //incrementation
		}
		// 2 - Recup la zone de chaque noeud
		auto debut_ss = sregex_iterator( outputArbre.begin(), outputArbre.end(), patternZone );
		auto fin_ss = sregex_iterator();	
		sregex_iterator( outputArbre.begin(), outputArbre.end(), patternZone );	
		for ( sregex_iterator ite = debut_ss; ite!=fin_ss; ++ite ) { //itération de la recherche d'aires
			smatch imatch = *ite;
			string zoneTemp = imatch.str(1);
			matriceDonnees[indiceZones][1] = zoneTemp; //stock dans matriceDonne
			indiceZones++; //incrementation
		}
		// 3 - Recup la proba des zones de chaque noeud
		auto debut_sss = sregex_iterator( outputArbre.begin(), outputArbre.end(), patternProba );
		auto fin_sss = sregex_iterator();	
		sregex_iterator( outputArbre.begin(), outputArbre.end(), patternProba );	
		for ( sregex_iterator ite = debut_sss; ite!=fin_sss; ++ite ) { //itération de la recherche de proba
			smatch imatch = *ite;
			string probaTemp = imatch.str(1);
			matriceDonnees[indiceProbas][2] = probaTemp; //stock dans matriceDonne
			indiceProbas++; //incrementation
		}
	}
	if ( (indiceNoeuds==indiceProbas) && (indiceNoeuds==indiceZones) ) { //compare les indices
	}
	else {
		cerr << "Probleme parsing, nombre de noeuds non identique pour les 3 types de données : "
			<< indiceNoeuds << '#' << indiceZones << '#'	<< indiceProbas << endl;
	}	
	// 4 - Recup le pere de chaque noeud
	for ( int l = 1; l<=indiceNoeuds; l++ ) { //parcours matrice
		string idNoeud = "Noeud-" + to_string(l);
		if ( matriceDonnees[l][0] == idNoeud ) { //si un noeud
			int fils = 0;
			while ( fils<2 ) { // tant qu'on a pas 2 fils
				for ( int m = l-1; m>=0; m-- ) { // parcours matrice à l'envers (remonte vers zéro)
					if ( matriceDonnees[m][3] == "" ) { // si noeud orphelin, c-a-d colonne 4 vide
						matriceDonnees[m][3] = idNoeud; //idNoeud devient son pere
						break;
					}
				}
				fils++;
			}
		}
	}
	/*// 5 - Selectionne l'aire, la plus probable, pour chaque noeud
	for ( int y = 1; y<indiceNoeuds; y++ ) {		
		string ssss = matriceDonnees[y][2], aireTemp = "";	
		double meilleureProba = 0.0;
		auto debut_ssss = sregex_iterator( ssss.begin(), ssss.end(), scoreAire );
		auto fin_ssss = sregex_iterator();
		if ( distance(debut_ssss, fin_ssss)>1 ) { // si plusieurs scores 
			sregex_iterator( ssss.begin(), ssss.end(), scoreAire ); // recup les probas de la colonne			
			for ( sregex_iterator ite = debut_ssss; ite!=fin_ssss; ++ite ) {// parcours les matchs de la ligne
				smatch imatch = *ite;                                                
				double probaTemp = stof(imatch.str(1)); // convertion string to double
					if ( probaTemp > meilleureProba ) { //si nouvelle proba rencontree superieur a proba stockee
						meilleureProba = probaTemp; //nouvelle proba devien meilleure proba
						string sssss = matriceDonnees[y][1];
//######## TO DO : stocker zone et proba dans un tab, recuperer l'indice du tableau pour retrouner l'aire avec meilleure proba ########
//######## ou bien, trouve solution pour garder la même position dans liste entre proba.set et location.set ########

					} //fin comparaison proba
			} //fin parcours de proba		
			matriceDonnees[y][2] = to_string(meilleureProba); //score dans la 3e colonne
			matriceDonnees[y][1] = aireTemp; //aire la plus probable dans la 2e colonne
		} //fin cas multiple zones
	}//fin parcours du tableau
	*/
	
	/*debug
	cout << "Matrice de données Beast : \n" << endl;
	for ( int z = 1; z<indiceNoeuds; z++ ) {
		cout << matriceDonnees[z][0] << '\n'
			<< "Aire : " << matriceDonnees[z][1] << '\t'
			<< " Score : " << matriceDonnees[z][2] << '\t'
			<< " Pere : " << matriceDonnees[z][3] << '\n' << endl;
	} //debug */
}

//------------------------------------------------------------------------
//######### Main ###########
int main (int argc, char**argv) {

	if ( argc==1 ){
  		cerr << "Vous devez executer ce programme avec un fichier 'Beast' en paramètre." << endl;
    		return 1;
 	}
	for ( int i = 1 ; i<argc; i++ ){ //plusieurs arguments
		recupNewick( argv[i] );
		lectureBeast( argv[i] );

	}
	return 0;
}
