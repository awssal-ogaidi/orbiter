// simeon.C
//
// classifies arcs and uses a method of Simeon Ball 
// to create larger arcs
//
// Anton Betten
// September 25, 2017

#include "orbiter.h"

int test_function_for_arc(int len, int *S, void *data, int verbose_level);
void do_simeon(set_and_stabilizer *SaS);


// global variables:
	int verbose_level = 0;
	int q = 11;
	int d = 2; // largest number of points per line
	int n = 2; // projective dimension
	int k = 9; // size of the arc
	finite_field *F;
	int f_projective = TRUE;
	int f_general = FALSE;
	int f_affine = FALSE;
	int f_semilinear = FALSE;
	int f_special = FALSE;
	sims *S;
	action *A;
	longinteger_object go;
	int *Elt;
	int *v;
	schreier *Sch;
	poset_classification *Gen;
	projective_space *P;

	action *A2; // action on the lines
	action *A3; // action on lines restricted to filtered_lines

// end of global variables


int main()
{
	int i;

		
	v = NEW_int(n + 1);

	F = NEW_OBJECT(finite_field);
	F->init(q, 0);

	create_linear_group(S, A, 
		F, n + 1, 
		f_projective, f_general, f_affine, 
		f_semilinear, f_special, 
		verbose_level);
	
	A->group_order(go);
	cout << "created a group of order " << go << endl;
	
	Elt = NEW_int(A->elt_size_in_int);

#if 0
	for (i = 0; i < go.as_int(); i++) {
		S->element_unrank_int(i, Elt, 0 /* verbose_level */);
		cout << "element " << i << " / " << go << ":" << endl;
		A->element_print_quick(Elt, cout);
		}
#endif

	for (i = 0; i < A->degree; i++) {
		F->PG_element_unrank_modified(v, 1, n + 1, i);
		cout << "point " << i << " / " << A->degree << " is ";
		int_vec_print(cout, v, d);
		cout << endl;
		}

	cout << "generating set: " << endl;
	A->Strong_gens->print_generators();

	Sch = A->Strong_gens->orbits_on_points_schreier(A, verbose_level);
	
	cout << "We have " << Sch->nb_orbits << " orbits on points" << endl;

	Sch->print_and_list_orbits(cout);
	
	P = NEW_OBJECT(projective_space);

	P->init(n /* n */, F /* finite_field *F*/, 
		TRUE /* f_init_incidence_structure */, 
		0 /* verbose_level */);

	P->init_incidence_structure(0 /*verbose_level*/);

	poset *Poset;

	Poset = NEW_OBJECT(poset);
	Poset->init_subset_lattice(A, A,
			A->Strong_gens,
			verbose_level);


	compute_orbits_on_subsets(Gen, 
		k /* target_depth */,
		"" /* const char *prefix */, 
		FALSE /* f_W */, FALSE /* f_w */,
		Poset,
		// ToDo
		//test_function_for_arc /* int (*candidate_incremental_check_func)
		//(int len, int *S, void *data, int verbose_level) */,
		//NULL /* void *candidate_incremental_check_data */,
		verbose_level);


#if 0
	Gen = orbits_on_k_sets_compute(A, A, 
		A->Strong_gens, 
		k, verbose_level);
#endif

	Gen->print_orbit_numbers(k);

	int nb_orbits;

	nb_orbits = Gen->nb_orbits_at_level(k);
	cout << "We found " << nb_orbits
			<< " orbits of subsets of size " << k << endl;

	for (i = 0; i < nb_orbits; i++) {
	
		set_and_stabilizer *SaS;

		SaS = Gen->get_set_and_stabilizer(k /* level */,
				i /* orbit_at_level */, 0 /* verbose_level */);
		cout << "orbit " << i << " / " << nb_orbits << " : ";
		SaS->print_set_tex(cout);
		cout << endl;

		SaS->print_generators_tex(cout);

		if (i == 0) {
			do_simeon(SaS);
			}
		
		}

	
}


int test_function_for_arc(int len, int *S,
		void *data, int verbose_level)
{
	int *type_collected;
	int i;

	//cout << "test_function_for_arc" << endl;
	type_collected = NEW_int(len + 1);
	P->line_intersection_type_collected(
		S /*int *set */, len /* int set_size */,
		type_collected, 2 /*verbose_level */);
	for (i = d + 1; i < len + 1; i++) {
		if (type_collected[i]) {
			FREE_int(type_collected);
			//cout << "test_function_for_arc fail" << endl;
			return FALSE;
			}
		}
	FREE_int(type_collected);
	//cout << "test_function_for_arc OK" << endl;
	return TRUE;
}

void do_simeon(set_and_stabilizer *SaS)
{
	int *type;
	int *original_arc;
	int original_arc_sz;
	int *bisecants;
	int *c2_points;
	int *external_lines;
	int nb_external_lines;
	int h, i, j, pi, pj, nb_bisecants, nb_c2points, bi, bj, a, idx, u, pt;

	original_arc = SaS->data;
	original_arc_sz = SaS->sz;

	nb_bisecants = int_n_choose_k(original_arc_sz, 2);
	nb_c2points = nb_bisecants * nb_bisecants;
	type = NEW_int(P->N_lines);
	external_lines = NEW_int(P->N_lines);
	nb_external_lines = 0;
	P->line_intersection_type(original_arc,
			original_arc_sz, type, 0 /*verbose_level*/);

	for (i = 0; i < P->N_lines; i++) {
		if (type[i] == 0) {
			external_lines[nb_external_lines++] = i;
			}
		}
	cout << "We found " << nb_external_lines
			<< " external lines, they are: ";
	int_vec_print(cout, external_lines, nb_external_lines);
	cout << endl;

	cout << "compute bisecants and c2 points:" << endl;


	bisecants = NEW_int(nb_bisecants);
	
	h = 0;
	for (i = 0; i < original_arc_sz; i++) {
		pi = original_arc[i];
		for (j = i + 1; j < original_arc_sz; j++) {
			pj = original_arc[j];
			bisecants[h++] = P->line_through_two_points(pi, pj);
			}
		}
	if (h != nb_bisecants) {
		cout << "h != nb_bisecants" << endl;
		exit(1);
		}
	cout << "We found " << nb_bisecants << " bisecants : ";
	int_vec_print(cout, bisecants, nb_bisecants);
	cout << endl;
	
	c2_points = NEW_int(nb_c2points);

	h = 0;
	for (i = 0; i < nb_bisecants; i++) {
		bi = bisecants[i];
		for (j = 0; j < nb_bisecants; j++) {
			if (j == i) {
				//continue;
				}
			else {
				bj = bisecants[j];
				a = P->line_intersection(bi, bj);
			
				if (int_vec_search_linear(original_arc,
						original_arc_sz, a, idx)) {
					}
				else {
					if (!int_vec_search(c2_points, h, a, idx)) {
						for (u = h; u > idx; u--) {
							c2_points[u] = c2_points[u - 1];
							}
						c2_points[idx] = a;
						h++;
						}
					}
				}
			}
		}
	cout << "We found " << h << " c2-points: ";
	int_vec_print(cout, c2_points, h);
	cout << endl;

	cout << "filtering the external lines:" << endl;
	int nb_filtered_lines;
	int *filtered_lines;
	int cnt;

	nb_filtered_lines = 0;
	filtered_lines = NEW_int(nb_external_lines);
	for (i = 0; i < nb_external_lines; i++) {
		a = external_lines[i];
		cnt = 0;
		for (j = 0; j < q + 1; j++) {
			pt = P->Lines[a * (q + 1) + j];
			if (int_vec_search(c2_points, h, pt, idx)) {
				cnt++;
				}
			}
		if (cnt > 1) {
			filtered_lines[nb_filtered_lines++] = a;
			}
		}
	cout << "We found " << nb_filtered_lines << " lines which intersect "
			"the set of c2 points in at least 2 points" << endl;


	A2 = A->induced_action_on_grassmannian(2, verbose_level);
	A3 = A2->restricted_action(filtered_lines,
			nb_filtered_lines, verbose_level);


	int target_depth = 5;
	poset *Poset2;
	poset_classification *Gen2;

	Poset2 = NEW_OBJECT(poset);
	Poset2->init_subset_lattice(A, A3,
			SaS->Strong_gens,
			verbose_level);

	compute_orbits_on_subsets(Gen2, 
		target_depth,
		"" /* const char *prefix */, 
		FALSE /* f_W */, FALSE /* f_w */,
		Poset2,
		//NULL /* int (*candidate_incremental_check_func)(
		//int len, int *S, void *data, int verbose_level) */,
		//NULL /* void *candidate_incremental_check_data */,
		5 /* verbose_level */);
	
	
	Gen2->print_orbit_numbers(target_depth);

	int nb_orbits;
	int *covering_number;
	int count;
	int nb_sol = 0;

	nb_orbits = Gen2->nb_orbits_at_level(target_depth);
	cout << "We found " << nb_orbits << " orbits of subsets "
			"of filtered external lines of size " << k << endl;

	covering_number = NEW_int(h);

	for (i = 0; i < nb_orbits; i++) {
	
		set_and_stabilizer *SaS;

		SaS = Gen2->get_set_and_stabilizer(target_depth,
				i /* orbit_at_level */, 0 /* verbose_level */);

		if ((i % 10000) == 0) {
			cout << "testing orbit " << i << endl;
			}
		int_vec_zero(covering_number, h);
		for (j = 0; j < h; j++) {
			for (u = 0; u < target_depth; u++) {
				a = SaS->data[u];
				if (P->is_incident(c2_points[j], a)) {
					covering_number[j]++;
					}
				}
			}
		count = 0;
		for (j = 0; j < h; j++) {
			if (covering_number[j]) {
				count++;
				}
			}
		if (count >= h - 1) {
			cout << "solution" << endl;
			cout << "orbit " << i << " / " << nb_orbits << " : ";
			SaS->print_set_tex(cout);
			cout << endl;
			cout << "covering_number: ";
			int_vec_print(cout, covering_number, h);
			cout << endl;
			nb_sol++;
			}
		
		//SaS->print_generators_tex(cout);

		
		}
	cout << "number of solutions = " << nb_sol << endl;

	
	FREE_int(type);
}


