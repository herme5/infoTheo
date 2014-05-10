/*
 *   Ce fichier fait parti d'un projet de programmation donné en Licence 3 
 *   à l'Université de Bordeaux 1
 *
 *   Copyright (C) 2014 Adrien Boussicault
 *
 *    This Library is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This Library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this Library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "automate.h"
#include "outils.h"

int main(){

	Automate * a = creer_automate();
	Automate * a2 = creer_automate();
	
	printf("\n==========automate(a)==============\n");
	ajouter_transition(a, 1, 'a', 2);
	ajouter_transition(a, 2, 'b', 3);
	ajouter_transition(a, 3, 'b', 2);
	ajouter_transition(a, 3, 'b', 4);
	ajouter_transition(a, 5, 'c', 3);
	ajouter_transition(a, 2, 'a', 6);
	ajouter_transition(a, 7, 'a', 4);
	ajouter_etat_initial(a, 1);
	ajouter_etat_final(a, 2);
	ajouter_etat_final(a, 3);
	print_automate(a);
	
	printf("\n==========automate(a2)==============\n");
	ajouter_transition(a2, 1, 'a', 3);
	ajouter_transition(a2, 2, 'b', 3);
	ajouter_transition(a2, 3, 'a', 4);
	ajouter_etat_initial(a2, 1);
	ajouter_etat_initial(a2, 2);
	ajouter_etat_final(a2, 4);
	print_automate(a2);
	
	/*
	printf("\n==========delta_star()=============\n");
	printf("delta_star(a, I, \"abb\") : ");
	print_ensemble(delta_star(a, get_initiaux(a), "abb"), NULL);
	printf("\n");
	*/

	printf("\n==========mot_to_automate==========\n");
	Automate * z = mot_to_automate( "automate" );
	print_automate(z);
	printf("\n");
	
	printf("\n==========etats_accessibles()==========\n");
	print_ensemble(etats_accessibles(a, 1), NULL);
	printf("\n");

	printf("\n==========automate accessible==========\n");
	Automate * b = automate_accessible( a );
	print_automate(b);
	printf("\n");

	printf("\n==========suffixe(a)===============\n");
	Automate * c = creer_automate_des_suffixes( a );
	print_automate(c);
	printf("\n");

	printf("\n==========prefixe(a)===============\n");
	Automate * d = creer_automate_des_prefixes( a );
	print_automate(d);
	printf("\n");

	printf("\n==========sousmots(a)===============\n");
	Automate * e = creer_automate_des_sous_mots( a );
	print_automate(e);
	printf("\n");

	printf("\n==========surmots(a)================\n");
	Ensemble * alphabet = creer_ensemble(NULL, NULL, NULL);
	ajouter_element(alphabet, 'm');
	ajouter_elements(alphabet, get_alphabet(a));
	Automate * f = creer_automate_des_sur_mot( a, alphabet );
	print_automate(f);
	printf("\n");

	printf("\n==========concatenation(a,a2)========\n");
	Automate * g = creer_automate_de_concatenation( a, a2);
	print_automate(g);
	printf("\n");

	printf("\n==========miroir(a)================\n");
	Automate * h = miroir( a );
	print_automate(h);
	printf("\n");

	printf("\n==========co-accessible(a)================\n");
	Automate * i = automate_co_accessible( a );
	print_automate(i);
	printf("\n");
	
	return 0;
}
