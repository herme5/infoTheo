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
	
	ajouter_transition(a, 0, 'a', 1);
	ajouter_transition(a, 1, 'a', 2);
	ajouter_transition(a, 1, 'b', 3);
	ajouter_transition(a, 1, 'b', 4);
	ajouter_transition(a, 2, 'b', 5);
	ajouter_transition(a, 4, 'a', 2);
	ajouter_transition(a, 4, 'b', 4);
	ajouter_transition(a, 5, 'a', 2);
	ajouter_transition(a, 5, 'b', 3);
	ajouter_etat_initial(a, 0);
	ajouter_etat_final(a, 3);
	
	Automate * b = miroir(a);
	Automate * c = creer_automate_des_suffixes(a);

	printf("\n========automate(a)==========\n");
	print_automate(a);
	printf("\n========miroir(a)==========\n");
	print_automate(b);
	printf("\n========suffixe(a)==========\n");
	print_automate(c);

	return 1;
}
