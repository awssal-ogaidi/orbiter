// all_cycles.C
// 
// Anton Betten
// February 12, 2015
//
// 
//
//

#include "orbiter.h"


// global data:

int t0; // the system time when the program started

void select_cycles(colored_graph *CG, poset_classification *gen,
		int level, int &nb_selected_orbits, int *&orbits);
void print_orbits_at_level(poset_classification *gen,
		int level, int verbose_level);
void print_selected_orbits_at_level(poset_classification *gen, int level,
	int nb_selected_orbits, int *orbits, int verbose_level);
void early_test_function_paths_and_cycles(int *S, int len, 
	int *candidates, int nb_candidates, 
	int *good_candidates, int &nb_good_candidates, 
	void *data, int verbose_level);

int main(int argc, char **argv)
{
	int i, j;
	t0 = os_ticks();
	int verbose_level = 0;
	int f_file = FALSE;	
	const char *fname = NULL;
	int f_depth = FALSE;
	int depth = 0;
	int f_draw_poset = FALSE;
	int f_embedded = FALSE;
	int f_sideways = FALSE;
	int nb_print_level = 0;
	int print_level[1000];

	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			fname = argv[++i];
			cout << "-file " << fname << endl;
			}
		else if (strcmp(argv[i], "-depth") == 0) {
			f_depth = TRUE;
			depth = atoi(argv[++i]);
			cout << "-depth " << depth << endl;
			}
		else if (strcmp(argv[i], "-draw_poset") == 0) {
			f_draw_poset = TRUE;
			cout << "-draw_poset " << endl;
			}
		else if (strcmp(argv[i], "-embedded") == 0) {
			f_embedded = TRUE;
			cout << "-embedded " << endl;
			}
		else if (strcmp(argv[i], "-sideways") == 0) {
			f_sideways = TRUE;
			cout << "-sideways " << endl;
			}
		else if (strcmp(argv[i], "-print_level") == 0) {
			print_level[nb_print_level++] = atoi(argv[++i]);
			cout << "-embedded " << endl;
			}

		}

	if (!f_file) {
		cout << "Please specify the file name using -file <fname>" << endl;
		exit(1);
		}
	if (!f_depth) {
		cout << "Please specify the depth using -depth <depth>" << endl;
		exit(1);
		}
	colored_graph *CG;

	CG = NEW_OBJECT(colored_graph);

	CG->load(fname, verbose_level);




	int *Adj;
	action *Aut;
	longinteger_object ago;

	cout << "computing automorphism group of the graph:" << endl;
	//Aut = create_automorphism_group_of_colored_graph_object(CG, verbose_level);


	Adj = NEW_int(CG->nb_points * CG->nb_points);
	int_vec_zero(Adj, CG->nb_points * CG->nb_points);
	for (i = 0; i < CG->nb_points; i++) {
		for (j = i + 1; j < CG->nb_points; j++) {
			if (CG->is_adjacent(i, j)) {
				Adj[i * CG->nb_points + j] = 1;
				}
			}
		}


	CG->compute_edges(verbose_level);

	int *M;
	int nb_rows, nb_cols;
	
	create_incidence_matrix_of_graph(Adj,
			CG->nb_points, M, nb_rows, nb_cols, verbose_level);

	if (nb_cols != CG->nb_edges) {
		cout << "nb_cols != CG->nb_edges" << endl;
		exit(1);
		}

	Aut = create_automorphism_group_of_graph(Adj, CG->nb_points, verbose_level);

	Aut->group_order(ago);	
	cout << "ago=" << ago << endl;

	action *Aut_on_edges;
	int *edges;
	
	Aut_on_edges = NEW_OBJECT(action);
	edges = NEW_int(nb_cols);
	for (i = 0; i < nb_cols; i++) {
		edges[i] = nb_rows + i;
		}

	Aut_on_edges->induced_action_by_restriction(*Aut, 
		TRUE /* f_induce_action */, Aut->Sims, 
		nb_cols /* nb_points */, edges, verbose_level);
	
	Aut_on_edges->group_order(ago);	
	cout << "ago on edges = " << ago << endl;

	
	char prefix[1000];
	poset_classification *gen;
	poset *Poset;



	strcpy(prefix, fname);
	replace_extension_with(prefix, "_p_and_c");

	Poset = NEW_OBJECT(poset);
	Poset->init_subset_lattice(Aut_on_edges, Aut_on_edges,
			Aut_on_edges->Strong_gens,
			verbose_level);

	Poset->add_testing_without_group(
			early_test_function_paths_and_cycles,
				CG /* void *data */,
				verbose_level);


	compute_orbits_on_subsets(gen, 
		depth /* target_depth */,
		prefix, 
		FALSE /* f_W */, FALSE /* f_w */,
		Poset,
		verbose_level);


	if (f_draw_poset) {
		gen->draw_poset(gen->fname_base, depth,
				0 /* data1 */, f_embedded, f_sideways,
				verbose_level);
		}

	print_orbits_at_level(gen, depth, verbose_level);
	if (nb_print_level) {
		for (i = 0; i < nb_print_level; i++) {
			print_orbits_at_level(gen, print_level[i],
					verbose_level);
			}
		}

	int nb_selected_orbits;
	int *orbits;

	select_cycles(CG, gen, depth,
			nb_selected_orbits, orbits);
	print_selected_orbits_at_level(gen,
			depth, nb_selected_orbits, orbits,
			verbose_level);

	FREE_int(orbits);
	FREE_int(M);
	FREE_int(Adj);
	FREE_int(edges);
	FREE_OBJECT(Aut_on_edges);
	FREE_OBJECT(Aut);
	FREE_OBJECT(CG);

	the_end(t0);
	//the_end_quietly(t0);

}

void select_cycles(colored_graph *CG,
		poset_classification *gen, int level,
		int &nb_selected_orbits, int *&orbits)
{
	int *set;
	int i, nb_orbits;

	set = NEW_int(level);
	nb_orbits = gen->nb_orbits_at_level(level);

	nb_selected_orbits = 0;
	orbits = NEW_int(nb_orbits);

	for (i = 0; i < nb_orbits; i++) {
		
		gen->get_set_by_level(level, i, set);
		if (CG->is_cycle(level, set, 0 /* verbose_level */)) {
			orbits[nb_selected_orbits++] = i;
			}
		}

	FREE_int(set);
}

void print_orbits_at_level(poset_classification *gen,
		int level, int verbose_level)
{
	int *set;
	longinteger_object go, ol, ago;
	longinteger_domain D;
	int i, nb_orbits;

	set = NEW_int(level);
	nb_orbits = gen->nb_orbits_at_level(level);


	gen->Poset->A->group_order(ago);
	cout << "group order " << ago << endl;
	cout << "The " << nb_orbits << " orbits at level "
			<< level << " are:" << endl;
	cout << "orbit : representative : stabilizer order "
			": orbit length" << endl;
	for (i = 0; i < nb_orbits; i++) {
		
		gen->get_set_by_level(level, i, set);

		strong_generators *gens;
		gen->get_stabilizer_generators(gens,  
			level, i, 0 /*verbose_level*/);
		gens->group_order(go);
		D.integral_division_exact(ago, go, ol);

		
		cout << "Orbit " << i << " is the set ";
		int_vec_print(cout, set, level);
		cout << " : " << go << " : " << ol << endl;
		//cout << endl;

		
		}

	FREE_int(set);
}

void print_selected_orbits_at_level(
	poset_classification *gen, int level,
	int nb_selected_orbits, int *orbits,
	int verbose_level)
{
	int *set;
	longinteger_object go, ol, ago;
	longinteger_domain D;
	int i, j, nb_orbits;

	set = NEW_int(level);
	nb_orbits = gen->nb_orbits_at_level(level);


	gen->Poset->A->group_order(ago);
	cout << "group order " << ago << endl;
	cout << "The " << nb_selected_orbits << " / "
			<< nb_orbits << " orbits at level "
			<< level << " are:" << endl;
	cout << "orbit : representative : stabilizer order "
			": orbit length" << endl;
	for (i = 0; i < nb_selected_orbits; i++) {
		
		j = orbits[i];

		gen->get_set_by_level(level, j, set);

		strong_generators *gens;
		gen->get_stabilizer_generators(gens,  
			level, j, 0 /*verbose_level*/);
		gens->group_order(go);
		D.integral_division_exact(ago, go, ol);

		
		cout << "Orbit " << j << " is the set ";
		int_vec_print(cout, set, level);
		cout << " : " << go << " : " << ol << endl;
		//cout << endl;

		
		}

	FREE_int(set);
}

void early_test_function_paths_and_cycles(int *S, int len, 
	int *candidates, int nb_candidates, 
	int *good_candidates, int &nb_good_candidates, 
	void *data, int verbose_level)
{
	colored_graph *CG = (colored_graph *) data;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "early_test_function for set ";
		print_set(cout, len, S);
		cout << endl;
		}

	CG->early_test_func_for_path_and_cycle_search(S, len, 
		candidates, nb_candidates, 
		good_candidates, nb_good_candidates, 
		verbose_level - 2);


	if (f_v) {
		cout << "early_test_function done" << endl;
		}
}


