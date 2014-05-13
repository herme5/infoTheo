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
#include "table.h"
#include "ensemble.h"
#include "outils.h"

#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> 

#include <math.h>

struct _Automate {
    Ensemble * vide;
    Ensemble * etats;
    Ensemble * alphabet;
    Table * transitions;
    Ensemble * initiaux;
    Ensemble * finaux;
};

typedef struct _Cle {
    int origine;
    int lettre;
} Cle;

int comparer_cle(const Cle * a, const Cle * b){
    if( a->origine < b->origine )
	return -1;
    if( a->origine > b->origine )
	return 1;
    if( a->lettre < b->lettre )
	return -1;
    if( a->lettre > b->lettre )
	return 1;
    return 0;
}

/* Ajout de l'affichage d'epsilon
 */
void print_cle( const Cle * a){
    if (a->lettre == -1)
	printf( "(%d, %s)" , a->origine, "ε");
    else
	printf( "(%d, %c)" , a->origine, (char) (a->lettre) );
}

/* retourne un état non utilisé correspond au plus petit entier
 * disponible l'automate
 */
int get_etat_libre(Automate * automate){
    int i = 0;
    for ( ; est_un_etat_de_l_automate(automate, i); i++);
    return i;
}

void supprimer_cle( Cle * cle ){
    xfree( cle );
}

void initialiser_cle( Cle * cle, int origine, char lettre ){
    cle->origine = origine;
    cle->lettre = (int) lettre;
}

Cle * creer_cle( int origine, char lettre ){
    Cle * result = xmalloc( sizeof(Cle) );
    initialiser_cle( result, origine, lettre );
    return result;
}

Cle * copier_cle( const Cle * cle ){
    return creer_cle( cle->origine, cle->lettre );
}

Automate * creer_automate(){
    Automate * automate = xmalloc( sizeof(Automate) );
    automate->etats = creer_ensemble( NULL, NULL, NULL );
    automate->alphabet = creer_ensemble( NULL, NULL, NULL );
    automate->transitions = 
	creer_table(( int(*)(const intptr_t, const intptr_t) ) comparer_cle,
		    ( intptr_t(*)(const intptr_t) ) copier_cle,
		    ( void(*)(intptr_t) ) supprimer_cle );
    automate->initiaux = creer_ensemble( NULL, NULL, NULL );
    automate->finaux = creer_ensemble( NULL, NULL, NULL );
    automate->vide = creer_ensemble( NULL, NULL, NULL ); 
    return automate;
}

void liberer_automate( Automate * automate ){
    liberer_ensemble( automate->vide );
    liberer_ensemble( automate->finaux );
    liberer_ensemble( automate->initiaux );
    pour_toute_valeur_table( automate->transitions,
			     ( void(*)(intptr_t) ) liberer_ensemble );
    liberer_table( automate->transitions );
    liberer_ensemble( automate->alphabet );
    liberer_ensemble( automate->etats );
    xfree(automate);
}

const Ensemble * get_etats( const Automate * automate ){
    return automate->etats;
}

/* Getteur de l'ensemble des etats initiaux.
 */
const Ensemble * get_initiaux( const Automate * automate ){
    return automate->initiaux;
}

/* Getteur de l'ensemble des etats finaux.
 */
const Ensemble * get_finaux( const Automate * automate ){
    return automate->finaux;
}

/* Getteur de l'alphabet de l'automate.
 */
const Ensemble * get_alphabet( const Automate * automate ){
    return automate->alphabet;
}

void ajouter_etat( Automate * automate, int etat ){
    ajouter_element( automate->etats, etat );
}

/* On ajoute la lettre à l'alphabet de l'automate.
 * Comme pour l'ajout d'un etat il n'est pas necessaire de regarder
 * si la lettre est déjà dans l'ensemble.
 */
void ajouter_lettre( Automate * automate, char lettre ){
    ajouter_element( automate->alphabet, lettre );
}

void ajouter_transition( Automate * automate,
			 int origine,
			 char lettre,
			 int fin
			 ){
    ajouter_etat( automate, origine );
    ajouter_etat( automate, fin );
    ajouter_lettre( automate, lettre );

    Cle cle;
    initialiser_cle( &cle, origine, lettre );
    Table_iterateur it = trouver_table( automate->transitions,
					(intptr_t) &cle );
    Ensemble * ens;
    if( iterateur_est_vide( it ) ){
	ens = creer_ensemble( NULL, NULL, NULL );
	add_table( automate->transitions,
		   (intptr_t) &cle,
		   (intptr_t) ens );
    } else {
	ens = (Ensemble*) get_valeur( it );
    }
    ajouter_element( ens, fin );
}

/* On test si l'etat fait ne fais pas déjà partie de l'automate.
 * Dans ce cas on l'ajoute à l'ensemble des états de l'automate
 * puis on le rend final.
 */
void ajouter_etat_final( Automate * automate, int etat_final ){
    if ( !est_un_etat_de_l_automate( automate, etat_final ))
	ajouter_etat( automate, etat_final );
    ajouter_element( automate->finaux, etat_final );
}

/* On test si l'etat fait ne fais pas déjà partie de l'automate.
 * Dans ce cas on l'ajoute à l'ensemble des états de l'automate
 * puis on le rend initial.
 */
void ajouter_etat_initial( Automate * automate, int etat_initial ){
    if ( !est_un_etat_de_l_automate( automate, etat_initial ))
	ajouter_etat( automate, etat_initial );
    ajouter_element( automate->initiaux, etat_initial );
}

const Ensemble * voisins( const Automate * automate,
			  int origine,
			  char lettre
			  ){
    Cle cle;
    initialiser_cle( &cle, origine, lettre );
    Table_iterateur it = trouver_table( automate->transitions,
					(intptr_t) &cle );
    if( ! iterateur_est_vide( it ) ){
	return (Ensemble*) get_valeur( it );
    } else {
	return automate->vide;
    }
}

Ensemble * delta1( const Automate * automate,
		   int origine,
		   char lettre
		   ){
    Ensemble * res = creer_ensemble( NULL, NULL, NULL );
    ajouter_elements( res, voisins( automate, origine, lettre ) );
    return res; 
}

Ensemble * delta( const Automate * automate,
		  const Ensemble * etats_courants,
		  char lettre
		  ){
    Ensemble * res = creer_ensemble( NULL, NULL, NULL );
    Ensemble_iterateur it;

    for( it = premier_iterateur_ensemble( etats_courants );
	 ! iterateur_ensemble_est_vide( it );
	 it = iterateur_suivant_ensemble( it )
	 ){
	const Ensemble * fins = voisins( automate,
					 get_element( it ),
					 lettre );
	ajouter_elements( res, fins );
    }

    return res;
}

/* On procède par iteration/
 * Etant donné que delta() retourne l'ensemble des états accessibles
 * à partir d'un ensemble d'états et en lisant une lettre, on l'utilise
 * en itérant comme suit : E' = delta(A, E, u_n).
 * E' étant l'ensemble à la prochaine itération, E un ensemble,
 * A un automate, et w = u_0.u_1...u_n pour n allant de 0 à la taille du mot
 * passé en paramètre.
 */
Ensemble * delta_star( const Automate * automate,
		       const Ensemble * etats_courants,
		       const char * mot
		       ){
    Ensemble * res = copier_ensemble( etats_courants );
    int curseur;

    for (curseur = 0; curseur < strlen(mot); curseur++)
	deplacer_ensemble( res, delta( automate, res, mot[curseur] ));

    return res;
}

void pour_toute_transition( const Automate * automate,
			    void (* action )( int origine, 
					      char lettre,
					      int fin,
					      void* data ),
			    void * data
			    ){
    Table_iterateur it1;
    Ensemble_iterateur it2;
    for( it1 = premier_iterateur_table( automate->transitions );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	Cle * cle = (Cle*) get_cle( it1 );
	Ensemble * fins = (Ensemble*) get_valeur( it1 );

	for( it2 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it2 );
	     it2 = iterateur_suivant_ensemble( it2 )
	     ){
	    int fin = get_element( it2 );
	    action( cle->origine, cle->lettre, fin, data );
	}
    }
}

Automate* copier_automate( const Automate * automate ){
    Automate * res = creer_automate();
    Ensemble_iterateur it1;
    // On ajoute les états de l'automate
    for( it1 = premier_iterateur_ensemble( get_etats( automate ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	ajouter_etat( res, get_element( it1 ) );
    }
    // On ajoute les états initiaux
    for( it1 = premier_iterateur_ensemble( get_initiaux( automate ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	ajouter_etat_initial( res, get_element( it1 ) );
    }
    // On ajoute les états finaux
    for( it1 = premier_iterateur_ensemble( get_finaux( automate ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	ajouter_etat_final( res, get_element( it1 ) );
    }
    // On ajoute les lettres
    for( it1 = premier_iterateur_ensemble( get_alphabet( automate ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	ajouter_lettre( res, (char) get_element( it1 ) );
    }
    // On ajoute les transitions
    Table_iterateur it2;
    for( it2 = premier_iterateur_table( automate->transitions );
	 ! iterateur_ensemble_est_vide( it2 );
	 it2 = iterateur_suivant_ensemble( it2 )
	 ){
	Cle * cle = (Cle*) get_cle( it2 );
	Ensemble * fins = (Ensemble*) get_valeur( it2 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    ajouter_transition( res, cle->origine, cle->lettre, fin );
	}
    }
    return res;
}

Automate * translater_etat( const Automate * automate, int n ){
    Automate * res = creer_automate();

    Ensemble_iterateur it;
    for( it = premier_iterateur_ensemble( get_etats( automate ) );
	 ! iterateur_ensemble_est_vide( it );
	 it = iterateur_suivant_ensemble( it )
	 ){
	ajouter_etat( res, get_element( it ) + n );
    }
    
    Table_iterateur it1;
    Ensemble_iterateur it2;
    for( it1 = premier_iterateur_table( automate->transitions );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	Cle * cle = (Cle*) get_cle( it1 );
	Ensemble * fins = (Ensemble*) get_valeur( it1 );

	for( it2 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it2 );
	     it2 = iterateur_suivant_ensemble( it2 )
	     ){
	    int fin = get_valeur( it2 );
	    ajouter_transition( res,
				cle->origine + n,
				cle->lettre,
				fin + n );
	}
    }
    return res;
}


void action_get_max_etat( const intptr_t element, void * data ){
    int * max = (int*) data;
    if( *max < element ) *max = element;
}

int get_max_etat( const Automate * automate ){
    int max = INT_MIN;
    pour_tout_element( automate->etats, action_get_max_etat, &max );
    return max;
}


void action_get_min_etat( const intptr_t element, void * data ){
    int * min = (int*) data;
    if( *min > element ) *min = element;
}

int get_min_etat( const Automate * automate ){
    int min = INT_MAX;
    pour_tout_element( automate->etats, action_get_min_etat, &min );
    return min;
}

/* On crée l'automate (A, Q, d, I, F) tel que pour
 * un mot w = u_1.u_2...u_n, n étant la taille du mot :
 * A = { lettres du mot passé en paramètre }
 * Q = { 0, 1, ... ,n }
 * d = { (0, u_1)->{1}, (1, u_2)->{2}, ... ,(n-1, u_n)->{n} }
 * I = { 0 };
 * F = { n };
 */ 
Automate * mot_to_automate( const char * mot ){
    Automate * nouvel_automate = creer_automate();
    ajouter_etat_initial( nouvel_automate, 0 );
    int cur = 0;
	
    for ( ; cur < strlen(mot); cur++ )
	ajouter_transition( nouvel_automate, cur, mot[cur], cur +1);
	
    ajouter_etat_final( nouvel_automate, cur);
    return nouvel_automate;
}

/* On procède en considérant successivement tous les voisins
 * de l'état 'e' donné en paramètre, puis les voisins de ses voisins et
 * ainsi de suite. On obtiendra à la fin du procédé, un ensemble contenant
 * tous les états accessibles à partir de l'état de depart 'e'.
 */
Ensemble * etats_accessibles( const Automate * automate, int etat ){
    Ensemble * res = creer_ensemble( NULL, NULL, NULL );
    Ensemble_iterateur it;
    ajouter_element( res, etat);

    it = premier_iterateur_ensemble( automate->alphabet );
    while (! iterateur_ensemble_est_vide( it )){
	Ensemble * voisins = delta( automate, res, get_element( it ));

	// Si res inter voisin = voisin on a pas besoin d'ajouter
	// d'éléments.
	if(comparer_ensemble( creer_intersection_ensemble(res, voisins),
			      voisins) == 0 ){
	    it = iterateur_suivant_ensemble( it );
	}
	// Sinon on ajoute le(s) élément(s) et on repart au début
	// de l'alphabet.
	else{
	    ajouter_elements( res, voisins );
	    it = premier_iterateur_ensemble( automate->alphabet );
	}
    }
    return res;
}

/* On crée un nouvel automate, dont on ne recopiera que les états
 * accessibles et les transitions qui n'ont pas de lien avec des états
 * non accessibles.
 */
Automate * automate_accessible( const Automate * automate){
    Automate * res = creer_automate();
    Ensemble * accessibles = creer_ensemble(NULL, NULL, NULL);
    Ensemble * tmp;
    Ensemble_iterateur it1;
    Table_iterateur it2;
	
    // On met à jour la variable 'accessibles' qui contient tous les
    // états accessibles depuis chaque état initial.
    for ( it1 = premier_iterateur_ensemble( get_initiaux( automate ));
	  ! iterateur_ensemble_est_vide( it1 );
	  it1 = iterateur_suivant_ensemble( it1 )
	  ){
	tmp = etats_accessibles(automate, get_element( it1 ));
	ajouter_elements(accessibles, tmp);
    }

    // On met à jour les données de notre nouvel automate :
    // - même états initiaux
    // - l'ensemble des états, et celui des états finaux correspondent
    // aux ensembles de celui de l'ancien automate, intersecté
    // avec l'ensemble des états accessibles.
    // - l'alphabet n'est pas copié car l'ajout de transition, assure
    // l'ajout des lettres concernés. (on peut avoir le d'une réduction
    // de l'alphabet lors de la construction d'un automate plus petit).
	
    res->vide = creer_ensemble( NULL, NULL, NULL);
    //ajouter_elements(res->alphabet, get_alphabet( automate ));
    ajouter_elements(res->initiaux, get_initiaux( automate )); 
    ajouter_elements(res->etats, 
		     creer_intersection_ensemble( get_etats(automate),
						  accessibles));
    ajouter_elements(res->finaux, 
		     creer_intersection_ensemble( get_finaux(automate),
						  accessibles));
    // On met à jour les transitions : il faut que l'état d'origine et
    // l'état d'arrivé soient tous deux accessibles.
    for ( it2 = premier_iterateur_table( automate->transitions );
	  ! iterateur_ensemble_est_vide( it2 );
	  it2 = iterateur_suivant_ensemble( it2 )
	  ){
	Cle * cle = (Cle*) get_cle( it2 );
	Ensemble * fins = (Ensemble*) get_valeur( it2 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    if (est_dans_l_ensemble(accessibles, cle->origine) &&
		est_dans_l_ensemble(accessibles, fin))
		ajouter_transition(res, cle->origine, cle->lettre, fin);
	}
    }
    return res;
}

/* On creer un automate en copiant l'alphabet et les états,
 * en inversant initiaux et finiaux, et en copiant les transitions
 * dans le sens inverse.
 */
Automate * miroir( const Automate * automate){
    Automate * res = creer_automate();
    Ensemble_iterateur it1;
    Table_iterateur it2;
    
    res->vide = creer_ensemble( NULL, NULL, NULL);
    ajouter_elements(res->alphabet, get_alphabet( automate ));
    ajouter_elements(res->etats, get_etats( automate ));
    ajouter_elements(res->initiaux, get_finaux( automate ));
    ajouter_elements(res->finaux, get_initiaux( automate ));
	
    for ( it2 = premier_iterateur_table( automate->transitions );
	  ! iterateur_ensemble_est_vide( it2 );
	  it2 = iterateur_suivant_ensemble( it2 )
	  ){
	Cle * cle = (Cle*) get_cle( it2 );
	Ensemble * fins = (Ensemble*) get_valeur( it2 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    if (est_un_etat_de_l_automate( res, fin ))
		ajouter_transition( res, fin, cle->lettre, cle->origine );
	}
    }
    return res;
}
 
/* On calcule le miroir de l'automate dont tous les etats sont accessibles, et 
 * on le renverse une deuxième fois. 
 */
Automate * automate_co_accessible( const Automate * automate){
    return miroir( automate_accessible( miroir( automate )));
}

/* L'automate des préfixes correspond à l'automate dont tous les états
 * accessibles sont finaux.
 */
Automate * creer_automate_des_prefixes( const Automate* automate ){
    Automate * res = copier_automate(automate);
    ajouter_elements(res->finaux, get_etats(automate_co_accessible( automate )));
    return res;
}

/* L'automate des suffixes correspond à l'automate dont tous les états
 * accessibles sont initiaux.
 */
Automate * creer_automate_des_suffixes( const Automate* automate ){
    Automate * res = copier_automate(automate);
    ajouter_elements(res->initiaux, get_etats(automate_co_accessible( automate )));
    return res;
}

Automate * creer_automate_des_facteurs( const Automate* automate ){
    A_FAIRE_RETURN(NULL);
}

Automate * creer_automate_des_sur_mot( const Automate* automate, Ensemble * alphabet ){
    Automate * res = copier_automate(automate);
    int e1, e2;
    Ensemble_iterateur it1;
	
    ajouter_elements(res->alphabet, alphabet);
	
    e1 = get_etat_libre(res);
    ajouter_etat_initial(res, e1);
    e2 = get_etat_libre(res);
    ajouter_etat_final(res, e2);
	
    for(
	it1 = premier_iterateur_ensemble( alphabet );
	! iterateur_ensemble_est_vide( it1 );
	it1 = iterateur_suivant_ensemble( it1 )
	){
	ajouter_transition(res, e1, get_element( it1 ), e1);
	ajouter_transition(res, e2, get_element( it1 ), e2);
    }
    for(
	it1 = premier_iterateur_ensemble( get_initiaux( res ));
	! iterateur_ensemble_est_vide( it1 );
	it1 = iterateur_suivant_ensemble( it1 )
	){
	if (e1 != get_element( it1 ))
	    ajouter_transition(res, e1, -1 , get_element( it1 ));
    }
    for(
	it1 = premier_iterateur_ensemble( get_finaux( res ));
	! iterateur_ensemble_est_vide( it1 );
	it1 = iterateur_suivant_ensemble( it1 )
	){
	if (e2 != get_element( it1 ))
	    ajouter_transition(res, get_element( it1 ), -1 , e2);
    }
    return res;
}

/* Renvoie l'automate destination, dont le nom de tous les états est
 * différent du nom des états de l'automate source.
 */
Automate * creer_automate_etat_different( const Automate * src,
					  const Automate * dst
					  ){
    Automate * res = creer_automate();
    Automate * copie_src = copier_automate(src);
    Table * table_corresp_etats = creer_table(NULL, NULL, NULL);

    Ensemble_iterateur it1;
    // On ajoute les états de l'automate
    for( it1 = premier_iterateur_ensemble( get_etats( dst ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	int new_etat = get_etat_libre(copie_src);
	add_table( table_corresp_etats, get_element( it1 ), new_etat);
	ajouter_etat( res, new_etat );
	ajouter_etat( copie_src, new_etat );
      
	if( est_un_etat_final_de_l_automate( dst, get_element( it1 )))
	    ajouter_etat_final( res, new_etat );
	if( est_un_etat_initial_de_l_automate( dst, get_element( it1 )))
	    ajouter_etat_initial( res, new_etat );
    }
    // On ajoute les lettres
    for( it1 = premier_iterateur_ensemble( get_alphabet( dst ) );
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	ajouter_lettre( res, (char) get_element( it1 ) );
    }
    // On ajoute les transitions
    Table_iterateur it2;
    for( it2 = premier_iterateur_table( dst->transitions );
	 ! iterateur_ensemble_est_vide( it2 );
	 it2 = iterateur_suivant_ensemble( it2 )
	 ){
	Cle * cle = (Cle*) get_cle( it2 );
	Ensemble * fins = (Ensemble*) get_valeur( it2 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    Table_iterateur t_origine = trouver_table(table_corresp_etats, cle->origine);
	    Table_iterateur t_fin = trouver_table(table_corresp_etats, fin);
	    ajouter_transition( res,
				get_valeur(t_origine),
				cle->lettre,
				get_valeur(t_fin));
	}
    }
    return res;
}

/* On prévoit le cas où l'automate2 utilise les mêmes entiers que les états de
 * l'automate1. La fonction creer_automate_different() assure qu'il n'y aura 
 * pas conflit entre les états des deux automates.
 * Ensuite on branche tous les états finaux de l'automate1 aux états initiaux 
 * de l'automate2 via des epsilon-transition.
 */
Automate * creer_automate_de_concatenation( const Automate * automate1,
					    const Automate * automate2
					    ){
    Automate * res = creer_automate();
    Ensemble_iterateur it1;
    Ensemble_iterateur it2;
    Automate * new_automate2 = creer_automate_etat_different(automate1, automate2);
    
    // On copie l'union des alphabets, et des états.
    // - Les états initiaux de l'automate resultat, correspondent aux états
    // initiaux de l'automate1.
    // - Les états finaux de l'automate resultat, correspondent aux états
    // finaux de l'autoimate2.
    res->vide = creer_ensemble( NULL, NULL, NULL);
    ajouter_elements( res->alphabet,
		      creer_union_ensemble( get_alphabet( automate1 ),
					    get_alphabet( new_automate2 )));
    ajouter_elements( res->etats,
		      creer_union_ensemble( get_etats( automate1 ),
					    get_etats( new_automate2 )));
    ajouter_elements( res->initiaux, get_initiaux( automate1 ));
    ajouter_elements( res->finaux, get_finaux( new_automate2 ));
    
    //on ajoute les transistions de l'automate1. 
    Table_iterateur it3;
    for( it3 = premier_iterateur_table( automate1->transitions );
	 ! iterateur_ensemble_est_vide( it3 );
	 it3 = iterateur_suivant_ensemble( it3 )
	 ){
	Cle * cle = (Cle*) get_cle( it3 );
	Ensemble * fins = (Ensemble*) get_valeur( it3 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    ajouter_transition( res, cle->origine, cle->lettre, fin );
	}
    }
    //on ajoute les transistions de l'automate2. 
    for( it3 = premier_iterateur_table( new_automate2->transitions );
	 ! iterateur_ensemble_est_vide( it3 );
	 it3 = iterateur_suivant_ensemble( it3 )
	 ){
	Cle * cle = (Cle*) get_cle( it3 );
	Ensemble * fins = (Ensemble*) get_valeur( it3 );
	for( it1 = premier_iterateur_ensemble( fins );
	     ! iterateur_ensemble_est_vide( it1 );
	     it1 = iterateur_suivant_ensemble( it1 )
	     ){
	    int fin = get_element( it1 );
	    ajouter_transition( res, cle->origine, cle->lettre, fin );
	}
    }

    //on ajoute les transistions liant l'automate1 à l'automate2.
    for( it1 = premier_iterateur_ensemble( get_finaux( automate1 ));
	 ! iterateur_ensemble_est_vide( it1 );
	 it1 = iterateur_suivant_ensemble( it1 )
	 ){
	for( it2 = premier_iterateur_ensemble( get_initiaux( new_automate2 ));
	     ! iterateur_ensemble_est_vide( it2 );
	     it2 = iterateur_suivant_ensemble( it2 )
	     ){
	    retirer_element(res->finaux, get_element( it1 ));
	    retirer_element(res->initiaux, get_element( it2 ));
	    ajouter_transition(res, get_element( it1), -1, get_element( it2 ));
	}
    }
    return res;
}

/* l'automate des sous-mots, 
 * corresponds à l'automate dont tous les états accessibles sont initiaux
 * et finaux, on creer on donc l'automate des prefixes de l'automate des
 * des suffixes de l'automate accessibles de l'automate passé en paramètre.
 */
Automate * creer_automate_des_sous_mots( const Automate* automate ){
    return creer_automate_des_suffixes
	( creer_automate_des_prefixes
	  ( automate_accessible( automate )));
}

Automate * creer_automate_du_melange( const Automate* automate1, const Automate* automate2 ){
    Automate * res = creer_automate();

    // melange( w, epsilon ) := w
    if (taille_ensemble( automate1->etats ) == 0)
	return automate2 ;
    
    // melange( epsilon, w ) := w
    if (taille_ensemble( automate2->etats ) == 0)
	return automate1 ;

    // melange( a.w1, b.W2 ) := a.melange( w1, b.w2 ) + b.melange( a.w1, w2 )
    Table_iterateur it1, it2;
    Cle * cle_a1 = get_element(premier_iterateur_ensemble( automate1->initiaux ));
    Cle * cle_a2 = get_element(premier_iterateur_ensemble( automate2->initiaux ));
    char a1 = ;
    char a2 = ;
    
    // on branche une transition aux melanges des automates (automate dont
    // états initiaux sont décalé de 1.
    Automate *rec_automate1 = ;
    Automate *rec_automate2 = ;
    

    A_FAIRE_RETURN(NULL);
}

int est_une_transition_de_l_automate( const Automate* automate,
				      int origine, char lettre, int fin
				      ){
    return est_dans_l_ensemble( voisins( automate, origine, lettre ), fin );
}

int est_un_etat_de_l_automate( const Automate * automate, int etat ){
    return est_dans_l_ensemble( get_etats( automate ), etat );
}

int est_un_etat_initial_de_l_automate( const Automate * automate, int etat ){
    return est_dans_l_ensemble( get_initiaux( automate ), etat);
}

int est_un_etat_final_de_l_automate( const Automate * automate, int etat ){
    return est_dans_l_ensemble( get_finaux( automate ), etat);
}

int est_une_lettre_de_l_automate( const Automate * automate, char lettre ){
    return est_dans_l_ensemble( get_alphabet( automate ), lettre);
}

void print_ensemble_2( const intptr_t ens ){
    print_ensemble( (Ensemble*) ens, NULL );
}

void print_lettre( intptr_t c ){
    if (c == -1)
	printf( "%s" , "ε");
    else 
	printf("%c", (char) c );
}

void print_automate( const Automate * automate ){
    printf("- Etats : ");
    print_ensemble( get_etats( automate ), NULL );
    printf("\n- Initiaux : ");
    print_ensemble( get_initiaux( automate ), NULL );
    printf("\n- Finaux : ");
    print_ensemble( get_finaux( automate ), NULL );
    printf("\n- Alphabet : ");
    print_ensemble( get_alphabet( automate ), print_lettre );
    printf("\n- Transitions : ");
    print_table( 
		automate->transitions,
		( void (*)( const intptr_t ) ) print_cle, 
		( void (*)( const intptr_t ) ) print_ensemble_2,
		""
		 );
    printf("\n");
}

int le_mot_est_reconnu( const Automate* automate, const char* mot ){
    Ensemble * accessible = delta_star( automate, get_initiaux( automate ), mot);
    return (taille_ensemble
	    (creer_intersection_ensemble
	     (accessible,
	      get_finaux( automate ))) > 0);
}
