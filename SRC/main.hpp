#ifndef MAIN_HPP
#define MAIN_HPP

#include <fstream> //ifstream
#include <iostream> //cout cin cerr
#include <stdio.h> //printf
#include <stdlib.h> //fonctions pour tab
#include <regex> //EXpression REGuliere
#include <string> //fonctions pour string
#include <string.h> //fonctions pour string

//######### Variable globale ###########
const int NbMaxNoeuds = 1000;

//######### Fonctions cpp ###########
std::string recupNewick (const char* fileName);
void lectureBeast (const char* fileName);


#endif
