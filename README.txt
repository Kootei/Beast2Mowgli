L'exécutable Beast2Mowgli est un prototype d'outil qui récupère les données nécessaires pour Mowgli dans un fichier Beast.


Pour l'utiliser :

	Mettez l'exécutable dans le même dossier que votre fichier de sortie Beast.
	
	Lancez l'exécutable avec la commande :
	
		./Beast2Mowgli nom_de_votre_fichier_Beast.txt > nom_de_votre_fichier_B2M.txt

Pour compiler :  

	sous mac : clang++ -std=c++11 -stdlib=libc++ main.cpp -o Beast2Mowgli

	sous ubuntu : g++ -std=c++11 main.cpp -o Beast2Mowgli

