// analyze.C
// 
// Anton Betten
// 5/28/2010
//
//
// computes the intersection type of lines with 
// respect to a given set of points
//
// Uses projective_space_set_stabilizer to compute the group
// when needed.

#include "orbiter.h"
#include "discreta.h"

#define MY_MAX_SET_SIZE 2000

// global data:

int t0; // the system time when the program started

void do_switch(projective_space *P, int switch1, int switch2, int *set, int set_size, int verbose_level);
void analyze(int n, finite_field *F, int *set, int set_size, 
	int f_switch, int switch1, int switch2, 
	int f_planes, int f_hf_planes, int f_hyperplanes, 
	int f_show, int f_show_orbits, int f_group, int f_list_group, int f_multiset, 
	int verbose_level);
void do_lines_with_group(projective_space *P, sims *S, 
	int *set, int set_size, int f_show, int f_show_orbits, int verbose_level);
void do_lines(projective_space *P, int *set, int set_size, int f_show, int f_multiset, int verbose_level);
void do_hf_planes(projective_space *P, projective_space *P2, 
	int *set, int set_size, int f_show, int verbose_level);
void do_planes(projective_space *P, projective_space *P2, 
	int *set, int set_size, int f_show, int verbose_level);
void look_at_these_planes_longinteger(
	projective_space *P, projective_space *P2, 
	grassmann *Gr, int *set_of_planes, 
	int *rank_idx, longinteger_object *Rank, 
	int nb_planes, int intersection_size, int *Blocks, 
	int *set, int set_size, int f_show, int verbose_level);
void look_at_these_planes(projective_space *P, projective_space *P2, 
	grassmann *Gr, int *set_of_planes, int nb_planes, int intersection_size, int *Blocks, 
	int *set, int set_size, int f_show, int verbose_level);
void look_at_pairs(projective_space *P, int *Blocks, int nb_blocks, int block_size, 
	int *set, int set_size, int verbose_level);
void do_hyperplanes(projective_space *P, 
	int *set, int set_size, int verbose_level);

int main(int argc, char **argv)
{
	int verbose_level = 0;
	int i, j;
	int f_n = FALSE;
	int n = 0;
	int f_q = FALSE;
	int q;
	int f_set = FALSE;
	int set[MY_MAX_SET_SIZE];
	int set_size = -1;
	int f_switch = FALSE;
	int switch1, switch2;
	int f_file = FALSE;
	char *file_name;
	int f_poly = FALSE;
	char *poly = NULL;
	int f_planes = FALSE;
	int f_hf_planes = FALSE;
	int f_hyperplanes = FALSE;
	int f_show = FALSE;
	int f_show_orbits = FALSE;
	int f_group = FALSE;
	int f_BLT = FALSE;
	int BLT_no = 0;
	int f_list_group = FALSE;
	int f_multiset = FALSE;

	
 	t0 = os_ticks();
	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = atoi(argv[++i]);
			cout << "-n " << n << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		else if (strcmp(argv[i], "-switch") == 0) {
			f_switch = TRUE;
			switch1 = atoi(argv[++i]);
			switch2 = atoi(argv[++i]);
			cout << "-switch " << switch1 << " " << switch2 << endl;
			}
		else if (strcmp(argv[i], "-poly") == 0) {
			f_poly = TRUE;
			poly = argv[++i];
			cout << "-poly " << poly << endl;
			}
		else if (strcmp(argv[i], "-planes") == 0) {
			f_planes = TRUE;
			cout << "-planes " << endl;
			}
		else if (strcmp(argv[i], "-hf_planes") == 0) {
			f_hf_planes = TRUE;
			cout << "-hf_planes " << endl;
			}
		else if (strcmp(argv[i], "-hyperplanes") == 0) {
			f_hyperplanes = TRUE;
			cout << "-hyperplanes " << endl;
			}
		else if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			file_name = argv[++i];
			cout << "-file " << file_name << endl;
			}
		else if (strcmp(argv[i], "-BLT") == 0) {
			f_BLT = TRUE;
			BLT_no = atoi(argv[++i]);
			cout << "-BLT " << BLT_no << endl;
			}
		else if (strcmp(argv[i], "-show") == 0) {
			f_show = TRUE;
			cout << "-show " << endl;
			}
		else if (strcmp(argv[i], "-show_orbits") == 0) {
			f_show_orbits = TRUE;
			cout << "-show_orbits " << endl;
			}
		else if (strcmp(argv[i], "-list_group") == 0) {
			f_list_group = TRUE;
			cout << "-list_group " << endl;
			}
		else if (strcmp(argv[i], "-set") == 0) {
			f_set = TRUE;
			for (j = 0; ; j++) {
				set[j] = atoi(argv[++i]);
				if (set[j] == -1) {
					set_size = j;
					break;
					}
				}
			cout << "-set ";
			int_vec_print(cout, set, set_size);
			cout << endl;
			}
		else if (strcmp(argv[i], "-group") == 0) {
			f_group = TRUE;
			cout << "-group " << endl;
			}
		else if (strcmp(argv[i], "-multiset") == 0) {
			f_multiset = TRUE;
			cout << "-multiset " << endl;
			}
		}
	if (!f_n) {
		cout << "please use -n option" << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "please use -q option" << endl;
		exit(1);
		}

	finite_field *F;

	F = new finite_field;
	F->init_override_polynomial(q, poly, 0);

	if (f_file) {
		int *the_set;
		read_set_from_file(file_name, the_set, set_size, verbose_level);
		if (set_size > MY_MAX_SET_SIZE) {
			cout << "set is too big, please increase MY_MAX_SET_SIZE" << endl;
			exit(1);
			}
		for (i = 0; i < set_size; i++) {
			set[i] = the_set[i];
			}
		FREE_int(the_set);
		}

	if (f_BLT) {
		action *A;
		action_on_orthogonal *AO;
		orthogonal *O;
		int *BLT;
		int v[5];
		int a;
		
		if (EVEN(q)) {
			cout << "BLT-sets need q odd" << endl;
			exit(1);
			}
		BLT = BLT_representative(q, BLT_no);
		set_size = q + 1;
		if (set_size > 1000) {
			cout << "set is too big" << endl;
			exit(1);
			}


		A = new action;
		int f_basis = TRUE;
		int f_init_hash_table = TRUE;
	
		A->init_BLT(F, f_basis, f_init_hash_table, verbose_level);
		//allocate_tmp_data();
		AO = A->G.AO;
		O = AO->O;


		for (i = 0; i < set_size; i++) {

			O->unrank_point(v, 1, BLT[i], 0);

			PG_element_rank_modified(*O->F, v, 1, 5, a);
			
			set[i] = a;
			}

		delete A;

		}
	
	if (set_size < 0) {
		cout << "No set given" << endl;
		exit(1);
		}
	
	//test_if_set(set, set_size);

	analyze(n, F, set, set_size, 
		f_switch, switch1, switch2, 
		f_planes, 
		f_hf_planes, 
		f_hyperplanes, 
		f_show, 
		f_show_orbits, 
		f_group, 
		f_list_group, 
		f_multiset, 
		verbose_level);
	
	delete F;
	the_end(t0);
}


void do_switch(projective_space *P, int switch1, int switch2, 
	int *set, int set_size, int verbose_level)
{
	int *v;
	int i, a;

	v = NEW_int(P->n + 1);
	for (i = 0; i < set_size; i++) {
		a = set[i];
		P->unrank_point(v, a);
		a = v[switch1];
		v[switch1] = v[switch2];
		v[switch2] = a;
		a = P->rank_point(v);
		set[i] = a;
		}
	FREE_int(v);
}


void analyze(int n, finite_field *F, int *set, int set_size, 
	int f_switch, int switch1, int switch2, 
	int f_planes, int f_hf_planes, int f_hyperplanes, 
	int f_show, int f_show_orbits, int f_group, int f_list_group, int f_multiset, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	projective_space *P;
	projective_space *P2;
	int f_with_group = FALSE;
	int f_semilinear = TRUE;
	//const char *override_poly = NULL;
	int f_basis = TRUE;
	int q = F->q;


	if (f_group) {
		f_with_group = TRUE;
		}

	if (is_prime(q)) {
		f_semilinear = FALSE;
		}
	
	P = new projective_space;

	P->init(n, F, f_with_group, 
		FALSE /* f_line_action */, 
		TRUE /* f_init_incidence_structure */, 
		f_semilinear, f_basis, 0 /*verbose_level*/);

	P2 = new projective_space;

	P2->init(2, F, f_with_group, 
		FALSE /* f_line_action */, 
		TRUE /* f_init_incidence_structure */, 
		f_semilinear, f_basis, 0 /*verbose_level*/);
	
	P->F->f_print_as_exponentials = FALSE;
	P2->F->f_print_as_exponentials = FALSE;

	if (f_v) {
		cout << "analyzing set ";
		int_vec_print(cout, set, set_size);
		cout << " of size " << set_size << endl;
		}
	if (f_vv) {
		P->print_set(set, set_size);
		}
	if (f_switch) {
		cout << "doing switch" << endl;
		do_switch(P, switch1, switch2, set, set_size, verbose_level);
		if (f_vv) {
			cout << "after switch" << endl;
			P->print_set(set, set_size);
			}
		}

	cout << "PG(" << n << "," << q << ") has " << P->N_lines << " lines" << endl;

	do_lines(P, set, set_size, f_show, f_multiset, verbose_level);

	if (f_planes) {
		do_planes(P, P2, set, set_size, f_show, verbose_level);
		}
	if (f_hf_planes) {
		do_hf_planes(P, P2, set, set_size, f_show, verbose_level);
		}
	if (f_hyperplanes) {
		do_hyperplanes(P, set, set_size, verbose_level);
		}
	if (f_group) {
		sims *S;
		longinteger_object ago;
		
		S = P->set_stabilizer(set, set_size, verbose_level - 1);

		S->group_order(ago);
		cout << "analyze() found a stabilizer of order " << ago << endl;
		if (f_v) {
			cout << "strong generators are:" << endl;
			S->print_generators();
			cout << "strong generators are (in tex):" << endl;
			S->print_generators_tex(cout);
			cout << "analyze() found a stabilizer of order " << ago << endl;
			}


		if (f_list_group) {
			S->print_all_group_elements();
			}

		
		do_lines_with_group(P, S, 
			set, set_size, f_show, f_show_orbits, verbose_level);
		}
}

void do_lines_with_group(projective_space *P, sims *S, 
	int *set, int set_size, int f_show, int f_show_orbits, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	vector_ge *gens;
	finite_field *F;
	int i, f, l, j, d, b, h, u, sz;
	schreier *Sch_pts;
	schreier *Sch;
	int *Set;
	int *basis;

	if (f_v) {
		cout << "do_lines_with_group" << endl;
		cout << "f_show=" << f_show << endl;
		cout << "f_show_orbits=" << f_show_orbits << endl;
		}
	if (P->Lines_on_point == NULL) {
		cout << "do_lines_with_group: P->Lines_on_point == NULL, so we don't analyze lines" << endl;
		return;
		}
	d = P->n + 1;
	F = P->F;
	
	gens = &S->gens;

	basis = NEW_int(2 * d);
	Set = NEW_int(MAXIMUM(P->N_points, P->N_lines));

	Sch_pts = new schreier;
	Sch_pts->init(P->A);
	Sch_pts->initialize_tables();
	Sch_pts->init_generators(*gens);
	Sch_pts->compute_all_point_orbits(verbose_level - 2);
	
	if (f_v) {
		cout << "do_lines_with_group: found " << Sch_pts->nb_orbits << " orbits on points" << endl;
		}

	Sch = new schreier;
	Sch->init(P->A2);
	Sch->initialize_tables();
	Sch->init_generators(*gens);
	Sch->compute_all_point_orbits(verbose_level - 2);
	
	if (f_v) {
		cout << "do_lines_with_group: found " << Sch->nb_orbits << " orbits on lines" << endl;
		}
	for (i = 0; i < Sch_pts->nb_orbits; i++) {
		f = Sch_pts->orbit_first[i];
		l = Sch_pts->orbit_len[i];
		for (j = 0; j < l; j++) {
			Set[j] = Sch_pts->orbit[f + j];
			}
		if (f_v) {
			cout << "Point-orbit " << i << " of length=" << l << " : ";
			int_vec_print(cout, Set, l);
			cout << endl;
			}
		}



	for (i = 0; i < Sch->nb_orbits; i++) {
		f = Sch->orbit_first[i];
		l = Sch->orbit_len[i];
		for (j = 0; j < l; j++) {
			Set[j] = Sch->orbit[f + j];
			}
		b = Set[0];
		P->unrank_line(basis, b);
		int *L;
		int *I;
		int sz;

		L = P->Lines + b * P->k;
		int_vec_intersect(L, P->k, set, set_size, I, sz);

		if (f_v) {
			cout << "Line-orbit " << i << " of length=" << l << " intersecting in " << sz << " points" << endl;
			}
		FREE_int(I);
		}


	for (i = 0; i < Sch->nb_orbits; i++) {
		f = Sch->orbit_first[i];
		l = Sch->orbit_len[i];
		if (f_v) {
			cout << "orbit " << i << " first=" << f << " length=" << l << endl;
			}
		for (j = 0; j < l; j++) {
			Set[j] = Sch->orbit[f + j];
			}
		if (f_v) {
			cout << "orbit: ";
			int_vec_print(cout, Set, l);
			cout << endl;
			}
		for (j = 0; j < l; j++) {
			if (!f_show_orbits && j == 1) {
				break;
				}
			b = Set[j];
			P->unrank_line(basis, b);
			if (f_show) {
				cout << "line " << b << " has a basis:" << endl;
				print_integer_matrix_width(cout, basis, 2, d, d, P->F->log10_of_q);
				}
			int *L;
			int *I;
			int sz;

			L = P->Lines + b * P->k;
			int_vec_intersect(L, P->k, set, set_size, I, sz);

			if (f_show) {
				cout << "and intersects in " << sz << " points : ";
				int_vec_print(cout, I, sz);
				cout << endl;
				cout << "they are:" << endl;
				P->print_set(I, sz);
				}

			FREE_int(I);
			} // next j
		} // next i
	

	partitionstack Stack;

	Stack.allocate(P->N_points + P->N_lines, 0);
	Stack.subset_continguous(P->N_points, P->N_lines);
	Stack.split_cell(0);


	for (i = 1; i < Sch_pts->nb_orbits; i++) {
		sz = 0;
		for (u = i; u < Sch_pts->nb_orbits; u++) {
			f = Sch_pts->orbit_first[u];
			l = Sch_pts->orbit_len[u];
			for (j = 0; j < l; j++) {
				Set[sz++] = Sch_pts->orbit[f + j];
				}
			}
		Stack.split_cell(Set, sz, 0);
		}
	for (i = 1; i < Sch->nb_orbits; i++) {
		sz = 0;
		for (u = i; u < Sch->nb_orbits; u++) {
			f = Sch->orbit_first[u];
			l = Sch->orbit_len[u];
			for (j = 0; j < l; j++) {
				Set[sz++] = Sch->orbit[f + j] + P->N_points;
				}
			}
		Stack.split_cell(Set, sz, 0);
		}

	
	int *Mtx;
	incidence_structure *Inc;
	
	Inc = new incidence_structure;
	Mtx = NEW_int(P->N_points * P->N_lines);
	if (P->incidence_bitvec == NULL) {
		cout << "P->incidence_bitvec == NULL" << endl;
		exit(1);
		}
	for (i = 0; i < P->N_points; i++) {
		for (j = 0; j < P->N_lines; j++) {
			Mtx[i * P->N_lines + j] = P->is_incident(i, j);
			}
		}
	Inc->init_by_matrix(P->N_points, P->N_lines, Mtx, verbose_level - 2);
	Inc->get_and_print_decomposition_schemes(Stack);
	Inc->get_and_print_decomposition_schemes_tex(Stack);

	//
	// now we will look at planes:
	//

	action_on_grassmannian *A_planes;
	action *A2;
	grassmann *Grass;

	
	Grass = new grassmann;
	A_planes = new action_on_grassmannian;
	A2 = new action;
	
	Grass->init(d, 3, F, verbose_level - 5);
	A_planes->init(*P->A, Grass, verbose_level - 5);
	
	
	if (f_v) {
		cout << "action on grassmannian established" << endl;
		}

	if (f_v) {
		cout << "initializing A2" << endl;
		}
	int f_induce_action = TRUE;
	//sims *S_planes;

	//S_planes = new sims;

#if 0
	longinteger_object go1;
	S_planes->init(P->A);
	S_planes->init_generators(*P->A->strong_generators /*SG*/, 0/*verbose_level*/);
	S_planes->compute_base_orbits_known_length(P->A->transversal_length, 0/*verbose_level - 1*/);
	S_planes->group_order(go1);
	if (f_v) {
		cout << "group order " << go1 << endl;
		}
#endif
	
	A2->induced_action_on_grassmannian(P->A, A_planes, 
		f_induce_action, S, verbose_level - 1);

	schreier *Sch2;
	int *Set2;
	int *basis2;
	int *M_inf;

	Sch2 = new schreier;
	Sch2->init(A2);
	Sch2->initialize_tables();
	Sch2->init_generators(*gens);
	Sch2->compute_all_point_orbits(verbose_level - 2);
	
	int N_planes;

	N_planes = A_planes->degree.as_int();
	if (f_v) {
		cout << "N_planes = " << N_planes << endl;
		}
	

	if (f_v) {
		cout << "do_lines_with_group: found " << Sch2->nb_orbits << " orbits on planes" << endl;
		}

	basis2 = NEW_int(4 * d);
	Set2 = NEW_int(N_planes);
	M_inf = NEW_int(Sch->nb_orbits * Sch2->nb_orbits);
	for (i = 0; i < Sch->nb_orbits * Sch2->nb_orbits; i++) {
		M_inf[i] = 0;
		}

	for (i = 0; i < Sch2->nb_orbits; i++) {
		f = Sch2->orbit_first[i];
		l = Sch2->orbit_len[i];
		b = Sch2->orbit[f + 0];
		A_planes->G->unrank_int(b, 0 /*verbose_level*/);

		int u, jj, rk1, hh;
		int *I;

		I = NEW_int(set_size);
		u = 0;
		for (jj = 0; jj < set_size; jj++) {
			for (hh = 0; hh < 3 * d; hh++) {
				basis2[hh] = A_planes->G->M[hh];
				}
			P->unrank_point(basis2 + 3 * d, set[jj]);
			rk1 = P->F->Gauss_easy(basis2, 4, d);
			if (rk1 <= 3) {
				I[u] = set[jj];
				u++;
				}
			} // next jj

		if (f_v) {
			cout << "Plane orbit " << i << " of length " << l << " intersects in " << u << " points" << endl;
			}
		FREE_int(I);
		}
	
	for (i = 0; i < Sch2->nb_orbits; i++) {
		f = Sch2->orbit_first[i];
		l = Sch2->orbit_len[i];
		if (f_v) {
			cout << "Plane orbit " << i << " first=" << f << " length=" << l << endl;
			}
		for (j = 0; j < l; j++) {
			Set2[j] = Sch2->orbit[f + j];
			}
		if (f_v) {
			cout << "Plane orbit: ";
			int_vec_print(cout, Set2, l);
			cout << endl;
			}
		for (h = 0; h < l; h++) {
			if (!f_show_orbits && h == 1) {
				break;
				}
			b = Set2[h];
			A_planes->G->unrank_int(b, 0 /*verbose_level*/);
			if (f_show) {
				cout << h << "-th plane " << b << " has a basis:" << endl;
				print_integer_matrix_width(cout, A_planes->G->M, 3, d, d, P->F->log10_of_q);
				}

			int u, jj, rk1, hh;
			int *I;

			I = NEW_int(set_size);
			u = 0;
			for (jj = 0; jj < set_size; jj++) {
				for (hh = 0; hh < 3 * d; hh++) {
					basis2[hh] = A_planes->G->M[hh];
					}
				P->unrank_point(basis2 + 3 * d, set[jj]);
				rk1 = P->F->Gauss_easy(basis2, 4, d);
				if (rk1 <= 3) {
					I[u] = set[jj];
					u++;
					}
				} // next jj

			if (f_show) {
				cout << "and intersects in the following " << u << " points:" << endl;
				P->print_set(I, u);
				}

			grassmann *G32;
			grassmann_embedded *Gre;
			int *subspace_basis;
			int *orbit_type;

		
			for (hh = 0; hh < 3 * d; hh++) {
				basis2[hh] = A_planes->G->M[hh];
				}

			orbit_type = NEW_int(Sch->nb_orbits);
			subspace_basis = NEW_int(2 * d);
			G32 = new grassmann;
			Gre = new grassmann_embedded;
			G32->init(3, 2, F, verbose_level - 2);
			Gre->init(d, 3, G32, basis2, verbose_level - 2);
			for (j = 0; j < Sch->nb_orbits; j++) {
				orbit_type[j] = 0;
				}
			for (jj = 0; jj < Gre->degree; jj++) {
				Gre->unrank_int(subspace_basis, jj, 0);
				for (hh = 0; hh < 2 * d; hh++) {
					P->Grass_lines->M[hh] = subspace_basis[hh];
					}
				j = P->Grass_lines->rank_int(0);
				if (f_show) {
					cout << "Subspace " << jj << " has a basis:" << endl;
					print_integer_matrix_width(cout, subspace_basis, 2, d, d, P->F->log10_of_q);
					cout << "and has rank " << j << " and belongs to line orbit " << Sch->orbit_no[j] << endl;
					}
				orbit_type[Sch->orbit_no[j]]++;
				M_inf[Sch->orbit_no[j] * Sch2->nb_orbits + i]++;
				}
			if (f_show) {
				cout << "Orbit type: ";
				int_vec_print(cout, orbit_type, Sch->nb_orbits);
				cout << endl;
				}

			delete G32;
			delete Gre;
			FREE_int(subspace_basis);
			FREE_int(orbit_type);
			FREE_int(I);
			} // next h

		} // next i
	
	cout << "Orbit matrix M_inf (line vs plane - orbits):" << endl;
	print_integer_matrix_width(cout, M_inf, Sch->nb_orbits, 
		Sch2->nb_orbits, Sch2->nb_orbits, 3);

	delete Grass;
	FREE_int(M_inf);
	FREE_int(basis);
	FREE_int(Set);
	delete Sch;
	delete Sch_pts;
	delete A2;
	//delete A_planes;
	FREE_int(basis2);
	FREE_int(Set2);
	delete Sch2;
	FREE_int(Mtx);
	delete Inc;
}


void do_lines(projective_space *P, int *set, int set_size, 
	int f_show, int f_multiset, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *intersection_numbers;
	int i, j, h, a;
	int *v;
	int n = P->n;
	int q = P->q;
	
	if (f_v) {
		cout << "do_lines" << endl;
		cout << "We will now compute the line types" << endl;
		}

	if (P->Lines_on_point == NULL) {
		cout << "P->Lines_on_point == NULL, so we don't analyze lines" << endl;
		return;
		}

	v = NEW_int(n + 1);


	intersection_numbers = NEW_int(P->N_lines);
	if (f_v) {
		cout << "after allocating intersection_numbers" << endl;
		}
	 
	for (i = 0; i < P->N_lines; i++) {
		intersection_numbers[i] = 0;
		}
	for (i = 0; i < set_size; i++) {
		a = set[i];
		for (h = 0; h < P->r; h++) {
			j = P->Lines_on_point[a * P->r + h];
			//if (j == 17) {
			//	cout << "set point " << i << " which is " << a << " lies on line 17" << endl;
			//	}
			intersection_numbers[j]++;
			}
		}

	classify C;
	int f_second = FALSE;

	C.init(intersection_numbers, P->N_lines, f_second, 0);
	if (f_v) {
		cout << "do_lines: line intersection type: ";
		C.print(FALSE /*f_backwards*/);
		}
	
	if (f_vv) {
		int h, f, l, b;
		int *S;
		int *basis;

		basis = NEW_int(2 * (P->n + 1));
		S = NEW_int(P->N_lines);
		for (h = 0; h < C.nb_types; h++) {
			f = C.type_first[h];
			l = C.type_len[h];
			a = C.data_sorted[f];
			if (f_v) {
				cout << a << "-lines: ";
				}
			for (j = 0; j < l; j++) {
				b = C.sorting_perm_inv[f + j];
				S[j] = b;
				}
			int_vec_quicksort_increasingly(S, l);
			int_vec_print(cout, S, l);
			cout << endl;
			for (j = 0; j < l; j++) {
				b = S[j];
				P->unrank_line(basis, b);
				if (f_show) {
					cout << "line " << b << " has a basis:" << endl;
					print_integer_matrix_width(cout, basis, 2, P->n + 1, P->n + 1, P->F->log10_of_q);
					}
				int *L;
				int *I;
				int sz;

				if (P->Lines == NULL) {
					continue;
					}
				L = P->Lines + b * P->k;
				int_vec_intersect(L, P->k, set, set_size, I, sz);

				if (f_show) {
					cout << "intersects in " << sz << " points : ";
					int_vec_print(cout, I, sz);
					cout << endl;
					cout << "they are:" << endl;
					P->print_set(I, sz);
					}

				FREE_int(I);
				}
			}
		FREE_int(S);
		FREE_int(basis);
#if 0
		cout << "i : intersection number of line i" << endl;
		for (i = 0; i < P->N_lines; i++) {
			cout << setw(4) << i << " : " << setw(3) << intersection_numbers[i] << endl;
			}
#endif
		}
	if (EVEN(P->F->e) && !f_multiset) {
		int q0;

		q0 = i_power_j(P->F->p, (P->F->e >> 1));
		if (f_v) {
			cout << "checking for Baer sublines, order q0=" << q0 << endl;
			}

		int h, f, l, b;
		int *S;
		int *basis;

		basis = NEW_int(2 * (P->n + 1));		
		S = NEW_int(P->N_lines);
		for (h = 0; h < C.nb_types; h++) {
			f = C.type_first[h];
			l = C.type_len[h];
			a = C.data_sorted[f];
			if (a < 3) {

				// 3 points determine a Baer subline, so we need at least 3 points
				continue;
				}
			
			if (f_v) {
				cout << "Looking at the " << l << " " << a << "-lines: ";
				}
			for (j = 0; j < l; j++) {
				b = C.sorting_perm_inv[f + j];
				S[j] = b;
				}
			int_vec_quicksort_increasingly(S, l);
			int_vec_print(cout, S, l);
			cout << endl;

			int *f_is_baer;
			int *circle_type;

			f_is_baer = NEW_int(l);
			circle_type = NEW_int(q);
			
			for (j = 0; j < l; j++) {
				b = S[j];
				P->unrank_line(basis, b);
				
				if (f_show ) {
					cout << "line " << b << " has a basis:" << endl;
					print_integer_matrix_width(cout, basis, 2, P->n + 1, P->n + 1, P->F->log10_of_q);
					}

				int *L;
				int *I;
				int sz;

				L = P->Lines + b * P->k;
				int_vec_intersect(L, P->k, set, set_size, I, sz);
				
				if (sz != a) {
					cout << "sz != a" << endl;
					exit(1);
					}
				f_is_baer[j] = P->is_contained_in_Baer_subline(I, sz, verbose_level - 3);
				//cout << "computing circle type:" << endl;				
				//P->circle_type_of_line_subset(I, sz, circle_type, verbose_level);
				FREE_int(I);
				}
			FREE_int(circle_type);

			classify CB;


			CB.init(f_is_baer, l, FALSE /* f_second */, 0);
			if (f_v) {
				cout << "Baer classification of " << a << "-lines: ";
				CB.print(FALSE /*f_backwards*/);
				}

			FREE_int(f_is_baer);
			

			} // next h
		FREE_int(S);
		FREE_int(basis);

		if (FALSE /*P->n == 2 && set_size >= 9*/) {
			int six_coeffs[6];
			
			if (f_v) {
				cout << "trying hermitian form:" << endl;
				}

			// there is a memory problem in the following function
			// detected 7/14/11


			if (P->determine_hermitian_form_in_plane(set /*nine_pts*/, set_size, six_coeffs, verbose_level - 2)) {
				if (f_v) {
					cout << "hermitian form coefficients:" << endl;
					int_vec_print(cout, six_coeffs, 6);
					cout << endl;
					}
				}
			else {
				if (f_v) {
					cout << "cannot find a hermitian form" << endl;
					}
				}
			}
		}

	if (f_v) {
		cout << "do_lines: line-intersection type (again): ";
		C.print(FALSE /*f_backwards*/);
		}
	FREE_int(v);
}

void do_hf_planes(projective_space *P, projective_space *P2, 
	int *set, int set_size, int f_show, int verbose_level)
// high frequency planes
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int i, j, a;
	int n = P->n;
	int q = P->q;
	int N;
	longinteger_object aa, rk;
	longinteger_object *R;
	int *rank_idx;
	longinteger_domain D;
	grassmann *Gr;
	//int *Blocks;
	int subset[3];
	int *Basis;
	int d, len, cnt, idx;
	int *set_of_planes;

	d = n + 1;
	if (f_v) {
		cout << "do_hf_planes" << endl;
		}
	if (f_v) {
		cout << "We will now compute the plane types" << endl;
		}

	Basis = NEW_int(4 * d);
	Gr = new grassmann;

	D.q_binomial(aa, d, 3, q, 0/*verbose_level*/);
	//N = aa.as_int();

	if (f_v) {
		cout << "there are " << aa << " planes" << endl;
		}

	N = int_n_choose_k(set_size, 3);
	if (f_v) {
		cout << "there are " << N << " 3-subsets of points" << endl;
		}
	Gr->init(n + 1, 3, P->F, 0 /*verbose_level*/);

	R = new longinteger_object[N];
	rank_idx = NEW_int(N);
	len = 0;
	for (i = 0; i < N; i++) {
		//cout << "i=" << i << endl;
		unrank_k_subset(i, subset, set_size, 3);
		if (FALSE) {
			cout << i << "-th subset ";
			int_vec_print(cout, subset, 3);
			cout << endl;
			}
		for (j = 0; j < 3; j++) {
			a = set[subset[j]];
			P->unrank_point(Basis + j * d, a);
			}
		for (j = 0; j < 3 * d; j++) {
			Gr->M[j] = Basis[j];
			}
		Gr->rank_longinteger(rk, 0);
		if (FALSE) {
			cout << i << "-th subset ";
			int_vec_print(cout, subset, 3);
			cout << " rank=" << rk << endl;
			}

		if (longinteger_vec_search(R, len, rk, idx)) {
			rank_idx[i] = idx;
			}
		else {
			for (j = len; j > idx; j--) {
				R[j].swap_with(R[j - 1]);
				}
			for (j = 0; j < i; j++) {
				if (rank_idx[j] >= idx) {
					rank_idx[j]++;
					}
				}
			rk.assign_to(R[idx]);
			rank_idx[i] = idx;
			len++;
			}
#if 0
		for (j = 0; j < len; j++) {
			if (D.compare(rk, R[j]) == 0) {
				break;
				}
			}
		if (j == len) {
			rk.assign_to(R[len]);
			rank_idx[i] = len;
			len++;
			}
		else {
			rank_idx[i] = j;
			}
#endif
		if (FALSE) {
			cout << i << "-th subset ";
			int_vec_print(cout, subset, 3);
			cout << " rank=" << rk << " rank_idx=" << rank_idx[i] << endl;
			}
		}
	

	classify C;

	C.init(rank_idx, N, TRUE /* f_second */, FALSE);

	if (f_v) {
		cout << "rank_idx:" << endl;
		int_vec_print(cout, rank_idx, N);
		cout << endl;
		cout << "intersection type of planes: ";
		C.print(FALSE /*f_backwards*/);
		}

	int f, ff, ll, u, nb_types2, nb_planes, nb_times, intersection_size, h;
	
	nb_types2 = C.second_nb_types;
	
	f = C.second_type_first[nb_types2 - 1];
	nb_planes = C.second_type_len[nb_types2 - 1];

	set_of_planes = NEW_int(nb_planes);
	
	for (i = 0; i < nb_planes; i++) {
		j = C.second_sorting_perm_inv[f + i];
		ff = C.type_first[j];
		ll = C.type_len[j];
		cnt = C.sorting_perm_inv[ff + 0];
		set_of_planes[i] = cnt;
		}
	
	if (FALSE) {
		if (nb_planes == 1) {
			cout << "there is a unique plane that appears " << C.second_data_sorted[f] << " times among the 3-sets of points" << endl;
			}
		else {
			cout << "there are " << nb_planes << " planes that each appear " << C.second_data_sorted[f] << " times among the 3-sets of points" << endl;

#if 0
			for (i = 0; i < nb_planes; i++) {
				j = C.second_sorting_perm_inv[f + i];
				ff = C.type_first[j];
				ll = C.type_len[j];
				cnt = C.sorting_perm_inv[ff + 0];
				cout << "The " << i << "-th plane, which is " << R[rank_idx[cnt]] << ", appears " << C.second_data_sorted[f + i] << " times" << endl;
				}
#endif
			}
		}
	nb_times = C.second_data_sorted[f];
	for (intersection_size = 3; ; intersection_size++) {
		if (int_n_choose_k(intersection_size, 3) == nb_times) {
			cout << "intersection_size=" << intersection_size << endl;
			break;
			}
		if (intersection_size == 100) {
			cout << "cannot determine intersection_size" << endl;
			exit(1);
			}
		}

#if 0
	if (f_vv) {
		cout << "these planes are:" << endl;
		for (i = 0; i < nb_planes; i++) {
			cout << "plane " << i << endl;
			j = C.second_sorting_perm_inv[f + i];
			ff = C.type_first[j];
			ll = C.type_len[j];
			for (u = 0; u < 1 /*ll */; u++) {
				cnt = C.sorting_perm_inv[ff + u];
				cout << "subspace " << setw(5) << R[rank_idx[cnt]] << endl;
				}
			}
		}
#endif

	int *Blocks;
	int rk1;

	Blocks = NEW_int(nb_planes * intersection_size);
	for (i = 0; i < nb_planes; i++) {
		j = C.second_sorting_perm_inv[f + i];
		ff = C.type_first[j];
		ll = C.type_len[j];
		cnt = C.sorting_perm_inv[ff + 0];
		Gr->unrank_longinteger(R[rank_idx[cnt]], 0);
		for (u = 0; u < 3 * d; u++) {
			Basis[u] = Gr->M[u];
			}

		a = 0;
		for (h = 0; h < set_size; h++) {
			for (u = 0; u < 3 * d; u++) {
				Basis[u] = Gr->M[u];
				}
			P->unrank_point(Basis + 3 * d, set[h]);
			rk1 = P->F->Gauss_easy(Basis, 4, d);
			if (rk1 <= 3) {
				Blocks[i * intersection_size + a] = h;
				a++;
				}
			}
		if (a != intersection_size) {
			cout << "a != intersection_size" << endl;
			exit(1);
			}
		if (FALSE) {
			cout << "plane " << i << endl;
			cout << "subspace " << setw(5) << R[rank_idx[cnt]] << endl;
			cout << "Basis:" << endl;
			print_integer_matrix_width(cout, Basis, 3, d, d, P->F->log10_of_q);
			cout << "intersects in ";
			int_vec_print(cout, Blocks + i * intersection_size, intersection_size);
			cout << endl;
			cout << "which are the points" << endl;
			for (u = 0; u < intersection_size; u++) {
				cout << set[Blocks[i * intersection_size + u]] << " ";
				}
			cout << endl;
			}
		}

	int g;
	int *Incma;
	int *ItI;
			
	if (f_v) {
		cout << "Computing plane invariant for " << nb_planes << " planes:" << endl;
		}
	Incma = NEW_int(set_size * nb_planes);
	ItI = NEW_int(nb_planes * nb_planes);
	for (i = 0; i < set_size * nb_planes; i++) {
		Incma[i] = 0;
		}
	for (u = 0; u < nb_planes; u++) {
		for (g = 0; g < intersection_size; g++) {
			i = Blocks[u * intersection_size + g];
			Incma[i * nb_planes + u] = 1;
			}
		}

	for (i = 0; i < nb_planes; i++) {
		for (j = 0; j < nb_planes; j++) {
			a = 0;
			for (u = 0; u < set_size; u++) {
				a += Incma[u * nb_planes + i] * Incma[u * nb_planes + j];
				}
			ItI[i * nb_planes + j] = a;
			}
		}
	if (f_v) {
		if (nb_planes == 1) {
			cout << "there is a unique plane that appears " << C.second_data_sorted[f] << " times among the 3-sets of points" << endl;
			}
		else {
			cout << "there are " << nb_planes << " planes that each appear " << C.second_data_sorted[f] << " times among the 3-sets of points" << endl;

#if 0
			for (i = 0; i < nb_planes; i++) {
				j = C.second_sorting_perm_inv[f + i];
				ff = C.type_first[j];
				ll = C.type_len[j];
				cnt = C.sorting_perm_inv[ff + 0];
				cout << "The " << i << "-th plane, which is " << R[rank_idx[cnt]] << ", appears " << C.second_data_sorted[f + i] << " times" << endl;
				}
#endif
			}
		cout << "Plane invariant:" << endl;
		print_integer_matrix_width(cout, ItI, nb_planes, nb_planes, nb_planes, 3);
		}

	if (f_v) {
		cout << "High frequency planes:" << endl;
		print_integer_matrix_width(cout, Blocks, nb_planes, intersection_size, intersection_size, 3);
		}

	classify D1, D2;
	int *f_point_is_present;
	int *missing_points;
	int nb_missing_points;

	f_point_is_present = NEW_int(q + 1);
	missing_points = NEW_int(q + 1);
	for (i = 0; i < q + 1; i++) {
		f_point_is_present[i] = FALSE;
		}
	for (i = 0; i < nb_planes * intersection_size; i++) {
		f_point_is_present[Blocks[i]] = TRUE;
		}
	nb_missing_points = 0;
	for (i = 0; i < q; i++) {
		if (!f_point_is_present[i]) {
			missing_points[nb_missing_points++] = i;
			}
		}

	D1.init(Blocks, nb_planes * intersection_size, FALSE, 0);
	D2.init(Blocks, nb_planes * intersection_size, TRUE, 0);
	if (f_v) {
		cout << "point multiplicities: ";
		D1.print(FALSE /*f_backwards*/);
		cout << "point multiplicities (second order): ";
		D2.print(FALSE /*f_backwards*/);
		cout << "The " << nb_missing_points << " missing points are:" << endl;
		int_vec_print(cout, missing_points, nb_missing_points);
		cout << endl;
		}


	if (f_v) {
		for (i = 0; i < nb_planes; i++) {
			j = C.second_sorting_perm_inv[f + i];
			ff = C.type_first[j];
			ll = C.type_len[j];
			cnt = C.sorting_perm_inv[ff + 0];
			Gr->unrank_longinteger(R[rank_idx[cnt]], 0);
			for (u = 0; u < 3 * d; u++) {
				Basis[u] = Gr->M[u];
				}
			cout << "plane " << i << endl;
			cout << "subspace " << setw(5) << R[rank_idx[cnt]] << endl;
			cout << "Basis:" << endl;
			print_integer_matrix_width(cout, Basis, 3, d, d, P->F->log10_of_q);
			cout << "intersects in ";
			int_vec_print(cout, Blocks + i * intersection_size, intersection_size);
			cout << endl;
			cout << "which are the points" << endl;
			for (u = 0; u < intersection_size; u++) {
				cout << set[Blocks[i * intersection_size + u]] << " ";
				}
			cout << endl;
			}
		}

	FREE_int(Incma);
	FREE_int(ItI);

		ff = C.type_first[j];
		ll = C.type_len[j];
		cnt = C.sorting_perm_inv[ff + 0];

	look_at_these_planes_longinteger(P, P2, 
		Gr, set_of_planes, rank_idx, R, 
		nb_planes, intersection_size, Blocks, 
		set, set_size, TRUE /*f_show*/, verbose_level);
	
	delete [] R;
	delete Gr;
	FREE_int(Basis);
	FREE_int(rank_idx);
	FREE_int(Blocks);
	FREE_int(set_of_planes);
	FREE_int(f_point_is_present);
	FREE_int(missing_points);
}

void do_planes(projective_space *P, projective_space *P2, 
	int *set, int set_size, int f_show, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *plane_type;
	int i, j, h, rk1, a;
	int *v;
	int n = P->n;
	int q = P->q;
	int N;
	longinteger_object aa;
	longinteger_domain D;
	grassmann *Gr;
	int *Blocks;

	if (f_v) {
		cout << "do_planes" << endl;
		}
	if (f_v) {
		cout << "We will now compute the plane types" << endl;
		}

	Gr = new grassmann;
	v = NEW_int(4 * (n + 1));

	D.q_binomial(aa, n + 1, 3, q, 0/*verbose_level*/);
	N = aa.as_int();

	if (f_v) {
		cout << "there are " << N << " planes" << endl;
		}

	Gr->init(n + 1, 3, P->F, 0 /*verbose_level*/);
	
	plane_type = NEW_int(N);
	if (f_v) {
		cout << "after allocating intersection_numbers" << endl;
		}
	 
	for (i = 0; i < N; i++) {
		plane_type[i] = 0;
		}
	for (i = 0; i < N; i++) {
		Gr->unrank_int(i, 0);
		a = 0;
		for (j = 0; j < set_size; j++) {
			for (h = 0; h < 3 * (n + 1); h++) {
				v[h] = Gr->M[h];
				}
			P->unrank_point(v + 3 * (n + 1), set[j]);
			rk1 = P->F->Gauss_easy(v, 4, n + 1);
			if (rk1 <= 3) {
				a++;
				}
			} // next j
		plane_type[i] = a;
		}

	if (f_vv) {
		cout << "plane types:" << endl;
		int_vec_print(cout, plane_type, N);
		cout << endl;
		}



	



	classify C;
	int f_second = FALSE;

	C.init(plane_type, N, f_second, 0);

	if (f_v) {
		cout << "intersection type of planes: ";
		C.print(FALSE /*f_backwards*/);
		}



	if (f_vv) {
		int h, f, b, nb_planes, intersection_size, u, g;
		int *S;
		//int *I;

		S = NEW_int(N);
		for (h = C.nb_types - 1; h >= 0; h--) {
			f = C.type_first[h];
			nb_planes = C.type_len[h];
			intersection_size = C.data_sorted[f];
			if (f_v) {
				cout << intersection_size << "-planes: ";
				}
			for (j = 0; j < nb_planes; j++) {
				b = C.sorting_perm_inv[f + j];
				S[j] = b;
				}
			int_vec_quicksort_increasingly(S, nb_planes);
			int_vec_print(cout, S, nb_planes);
			cout << endl;


			if (nb_planes > 30) {
				continue;
				}
			Blocks = NEW_int(nb_planes * intersection_size);
			
			
			look_at_these_planes(P, P2, 
				Gr, S /*set_of_planes*/, nb_planes, intersection_size, Blocks, 
				set, set_size, f_show, verbose_level);

			int *Incma;
			int *ItI;
			
			Incma = NEW_int(set_size * nb_planes);
			ItI = NEW_int(nb_planes * nb_planes);
			for (i = 0; i < set_size * nb_planes; i++) {
				Incma[i] = 0;
				}
			for (u = 0; u < nb_planes; u++) {
				for (g = 0; g < intersection_size; g++) {
					i = Blocks[u * intersection_size + g];
					Incma[i * nb_planes + u] = 1;
					}
				}

			for (i = 0; i < nb_planes; i++) {
				for (j = 0; j < nb_planes; j++) {
					a = 0;
					for (u = 0; u < set_size; u++) {
						a += Incma[u * nb_planes + i] * Incma[u * nb_planes + j];
						}
					ItI[i * nb_planes + j] = a;
					}
				}
			if (f_v) {
				cout << "I^\\top * I = " << endl;
				print_integer_matrix_width(cout, ItI, nb_planes, nb_planes, nb_planes, 3);
				}

			FREE_int(Incma);
			FREE_int(Blocks);
			FREE_int(ItI);
			}
		FREE_int(S);
		}




	delete Gr;
	FREE_int(v);
	FREE_int(plane_type);
}


void look_at_these_planes_longinteger(
	projective_space *P, projective_space *P2, 
	grassmann *Gr, int *set_of_planes, 
	int *rank_idx, longinteger_object *Rank, 
	int nb_planes, int intersection_size, int *Blocks, 
	int *set, int set_size, int f_show, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int j, jj, hh, a, b, c, rk, i;
	int n = P->n;
	int d = n + 1;
	int *Basis;
	int *base_cols;
	int *local_coords;
	int *line_type;
	int *local_set;
	int *v;
	int six_coeffs[6];
	int *conic_points;
	int conic_nb_points;
	int f_is_subset;
	int *real_points;


	if (f_v) {
		cout << "look_at_these_planes_longinteger" << endl;
		}

	cout << "The " << nb_planes << " " << intersection_size << "-planes are:" << endl;
	print_integer_matrix_width(cout, Blocks, 
		nb_planes, intersection_size, intersection_size, 3);


	Basis = NEW_int(4 * d);
	local_coords = NEW_int(intersection_size * 3);
	base_cols = NEW_int(d);
	line_type = NEW_int(intersection_size + 1);
	local_set = NEW_int(intersection_size);
	v = NEW_int(d);
	conic_points = NEW_int(P2->q + 1);
	real_points = NEW_int(intersection_size);
	
	for (j = 0; j < nb_planes; j++) {
		
		for (i = 0; i < intersection_size; i++) {
			real_points[i] = set[Blocks[j * intersection_size + i]];
			}


		b = set_of_planes[j];
		c = rank_idx[b];
		Gr->unrank_longinteger(Rank[c], 0);

		if (f_show) {
			cout << j << "-th plane " << b << " = " << Rank[c] << " has a basis:" << endl;
			print_integer_matrix_width(cout, Gr->M, 3, d, d, P->F->log10_of_q);
			cout << "and intersects in the following " << intersection_size << " points:" << endl;
			P->print_set(real_points, intersection_size);
			}




		for (hh = 0; hh < 3 * d; hh++) {
			Basis[hh] = Gr->M[hh];
			}
		rk = P->F->Gauss_simple(Basis, 3, d, base_cols, verbose_level - 3);
		if (f_show) {
			cout << "look_at_these_planes" << endl;
			cout << "plane has rank " << rk << endl;
			cout << "base_cols=";
			int_vec_print(cout, base_cols, rk);
			cout << endl;
			cout << "basis:" << endl;
			print_integer_matrix_width(cout, Basis, rk, d, d, P->F->log10_of_q);
			}
		if (rk != 3) {
			cout << "rk != 3" << endl;
			exit(1);
			}

		for (jj = 0; jj < intersection_size; jj++) {
			a = real_points[jj];
			P->unrank_point(v, a);

			if (f_show) {
				cout << "jj=" << jj << "-th point " << a << ":";
				int_vec_print(cout, v, d);
				cout << endl;
				}
			//cout << "basis:" << endl;
			//print_integer_matrix_width(cout, Basis, rk, d, d, P4->F->log10_of_q);
		
			P->F->reduce_mod_subspace_and_get_coefficient_vector(
				rk, d, Basis, base_cols, 
				v, local_coords + jj * rk, verbose_level - 3);
			
			if (f_show) {
				cout << "local coordinates:";
				int_vec_print(cout, local_coords + jj * rk, 3);
				cout << endl;
				}

			local_set[jj] = P2->rank_point(local_coords + jj * rk);
			if (f_show) {
				cout << "local point " << local_set[jj] << endl;
				}
			}
		if (f_show) {
			cout << "look_at_these_planes local coordinates in the subspace are" << endl;
			print_integer_matrix_width(cout, local_coords, intersection_size, 3, 3, P->F->log10_of_q);
			cout << "local_set=";
			int_vec_print(cout, local_set, intersection_size);
			cout << endl;
			}


		P2->line_intersection_type_collected(local_set, 
			intersection_size /* set_size */, line_type, verbose_level - 1);
		if (f_v) {
			cout << "line_type=";
			int_vec_print(cout, line_type, intersection_size + 1);
			cout << endl;
			}
		for (i = 3; i <= intersection_size; i++) {
			if (line_type[i]) {
				break;
				}
			}
		if (i <= intersection_size) {
			continue;
			}

		if (f_v) {
			cout << "It is an arc" << endl;
			}

		if (intersection_size < 5) {
			continue;
			}
		if (f_v) {
			cout << "Finding the conic determined by the first 5 points:" << endl;
			}
		P2->determine_conic_in_plane(local_set, 5, six_coeffs, verbose_level - 3);
		if (f_v) {
			cout << "The conic determined by the first 5 points is:" << endl;
			int_vec_print(cout, six_coeffs, 6);
			cout << endl;
			}
		P2->conic_points(local_set, six_coeffs, 
			conic_points, conic_nb_points, verbose_level - 3);
		if (f_v) {
			cout << "The " << conic_nb_points << " conic points are:" << endl;
			int_vec_print(cout, conic_points, conic_nb_points);
			cout << endl;
			cout << "local_set=";
			int_vec_print(cout, local_set, intersection_size);
			cout << endl;
			}
		f_is_subset = is_subset_of(local_set, intersection_size, conic_points, conic_nb_points);
		if (f_is_subset) {
			cout << "The points lie on a conic" << endl;
			}
		else {
			cout << "The points do not lie on a conic" << endl;
			}
		} // next j


#if 0
	look_at_pairs(P, Blocks, nb_planes, intersection_size, 
		set, set_size, verbose_level);
#endif

	FREE_int(Basis);
	FREE_int(local_coords);
	FREE_int(base_cols);
	FREE_int(line_type);
	FREE_int(local_set);
	FREE_int(v);
	FREE_int(conic_points);
	FREE_int(real_points);
}



void look_at_these_planes(projective_space *P, projective_space *P2, 
	grassmann *Gr, int *set_of_planes, 
	int nb_planes, int intersection_size, int *Blocks, 
	int *set, int set_size, int f_show, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int j, jj, hh, u, a, b, rk, rk1;
	int n = P->n;
	int d = n + 1;
	int *I;
	int *v;
	//int *Blocks;

	v = NEW_int(4 * d);
	I = NEW_int(set_size);
	//Blocks = NEW_int(nb_planes * intersection_size);
	for (j = 0; j < nb_planes; j++) {
		b = set_of_planes[j];
		Gr->unrank_int(b, 0);
		for (hh = 0; hh < 3 * d; hh++) {
			v[hh] = Gr->M[hh];
			}
		//cout << "plane " << b << " has a basis:" << endl;
		//print_integer_matrix_width(cout, v, 3, d, d, P->F->log10_of_q);

		u = 0;
		for (jj = 0; jj < set_size; jj++) {
			for (hh = 0; hh < 3 * d; hh++) {
				v[hh] = Gr->M[hh];
				}
			P->unrank_point(v + 3 * d, set[jj]);
			rk1 = P->F->Gauss_easy(v, 4, d);
			if (rk1 <= 3) {
				I[u] = set[jj];
				Blocks[j * intersection_size + u] = jj;
				u++;
				}
			} // next j

		if (f_show) {
			cout << "plane " << b << " has a basis:" << endl;
			print_integer_matrix_width(cout, Gr->M, 3, d, d, P->F->log10_of_q);
			cout << "and intersects in the following " << u << " points:" << endl;
			P->print_set(I, u);
			}

		int *Basis;
		int *base_cols;
		int *local_coords;
		int *line_type;
		int *local_set;

		Basis = NEW_int(3 * d);
		local_coords = NEW_int(u * 3);
		base_cols = NEW_int(d);
		line_type = NEW_int(u + 1);
		local_set = NEW_int(u);


		for (hh = 0; hh < 3 * d; hh++) {
			Basis[hh] = Gr->M[hh];
			}
		rk = P->F->Gauss_simple(Basis, 3, d, base_cols, verbose_level - 3);
		if (f_show) {
			cout << "look_at_these_planes" << endl;
			cout << "plane has rank " << rk << endl;
			cout << "base_cols=";
			int_vec_print(cout, base_cols, rk);
			cout << endl;
			cout << "basis:" << endl;
			print_integer_matrix_width(cout, Basis, rk, d, d, P->F->log10_of_q);
			}
		if (rk != 3) {
			cout << "rk != 3" << endl;
			exit(1);
			}

		for (jj = 0; jj < u; jj++) {
			a = I[jj];
			P->unrank_point(v, a);

			if (f_show) {
				cout << "jj=" << jj << " point:";
				int_vec_print(cout, v, d);
				cout << endl;
				}
			//cout << "basis:" << endl;
			//print_integer_matrix_width(cout, Basis, rk, d, d, P4->F->log10_of_q);
		
			P->F->reduce_mod_subspace_and_get_coefficient_vector(
				rk, d, Basis, base_cols, 
				v, local_coords + jj * rk, verbose_level - 3);

			local_set[jj] = P2->rank_point(local_coords + jj * rk);
			}
		if (f_show) {
			cout << "look_at_these_planes local coordinates in the subspace are" << endl;
			print_integer_matrix_width(cout, local_coords, u, 3, 3, P->F->log10_of_q);
			cout << "local_set=";
			int_vec_print(cout, local_set, u);
			cout << endl;
			}


		P2->line_intersection_type_collected(local_set, u /* set_size */, line_type, verbose_level - 1);
		if (f_v) {
			cout << "line_type=";
			int_vec_print(cout, line_type, u + 1);
			cout << endl;
			}
		
		FREE_int(Basis);
		FREE_int(local_coords);
		FREE_int(base_cols);
		FREE_int(line_type);
		FREE_int(local_set);
		
		} // next j


	cout << "The " << nb_planes << " " << intersection_size << "-planes are:" << endl;
	print_integer_matrix_width(cout, Blocks, 
		nb_planes, intersection_size, intersection_size, 3);

#if 0
	look_at_pairs(P, Blocks, nb_planes, intersection_size, 
		set, set_size, verbose_level);
#endif

	FREE_int(I);
	FREE_int(v);
	//FREE_int(Blocks);

}

void look_at_pairs(projective_space *P, int *Blocks, int nb_blocks, int block_size, 
	int *set, int set_size, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *Pair_covered;
	int N, i, j1, j2, a, a1, a2, b1, b2, idx;
	int *set_inv;

	set_inv = NEW_int(P->N_points);
	for (i = 0; i < P->N_points; i++) {
		set_inv[i] = -1;
		}
	for (i = 0; i < set_size; i++) {
		a = set[i];
		set_inv[a] = i;
		}
	N = (set_size * (set_size - 1)) >> 1;
	Pair_covered = NEW_int(N);
	for (i = 0; i < N; i++) {
		Pair_covered[i] = 0;
		}
	for (i = 0; i < nb_blocks; i++) {
		for (j1 = 0; j1 < block_size; j1++) {
			a1 = set[Blocks[i * block_size + j1]];
			b1 = set_inv[a1];
			for (j2 = j1 + 1; j2 < block_size; j2++) {
				a2 = set[Blocks[i * block_size + j2]];
				b2 = set_inv[a2];
				idx = ij2k(b1, b2, set_size);
				Pair_covered[idx]++;
				}
			}
		}

	classify C;
	int f_second = FALSE;

	C.init(Pair_covered, N, f_second, 0);
	if (f_v) {
		cout << "look_at_pairs: pairs covered: ";
		C.print(FALSE /*f_backwards*/);
		}

	FREE_int(Pair_covered);
	FREE_int(set_inv);
}


void do_hyperplanes(projective_space *P, 
	int *set, int set_size, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *hyperplane_type;
	int *intersection_numbers;
	int i, j, h, rk1, a;
	int *v;
	int n = P->n;
	int d = n + 1;
	int q = P->q;
	int N;
	longinteger_object aa;
	longinteger_domain D;
	grassmann *Gr;

	if (f_v) {
		cout << "do_hyperplanes" << endl;
		}
	if (f_v) {
		cout << "We will now compute the hyperplane type" << endl;
		}

	Gr = new grassmann;
	v = NEW_int(n * d);

	D.q_binomial(aa, d, n, q, 0/*verbose_level*/);
	N = aa.as_int();

	if (f_v) {
		cout << "there are " << N << " hyperplanes" << endl;
		}

	Gr->init(d, n, P->F, 0 /*verbose_level*/);
	
	hyperplane_type = NEW_int(N);
	intersection_numbers = NEW_int(set_size + 1);
	if (f_v) {
		cout << "after allocating intersection_numbers" << endl;
		}
	 
	for (i = 0; i < set_size + 1; i++) {
		intersection_numbers[i] = 0;
		}
	for (i = 0; i < N; i++) {
		hyperplane_type[i] = 0;
		}
	for (i = 0; i < N; i++) {
		Gr->unrank_int(i, 0);
		a = 0;
		for (j = 0; j < set_size; j++) {
			for (h = 0; h < n * d; h++) {
				v[h] = Gr->M[h];
				}
			P->unrank_point(v + n * d, set[j]);
			rk1 = P->F->Gauss_easy(v, n + 1, d);
			if (rk1 <= n) {
				a++;
				}
			} // next j
		hyperplane_type[i] = a;
		}

	if (f_vv) {
		cout << "hyperplane types:" << endl;
		int_vec_print(cout, hyperplane_type, N);
		cout << endl;
		}



	



	classify C;
	int f_second = FALSE;

	C.init(hyperplane_type, N, f_second, 0);

	if (f_v) {
		cout << "intersection type of hyperplanes: ";
		C.print(FALSE /*f_backwards*/);
		}



	if (f_vv) {
		int h, f, l, b;
		int *S;
		//int *I;

		S = NEW_int(N);
		//I = NEW_int(set_size);
		for (h = 0; h < C.nb_types; h++) {
			f = C.type_first[h];
			l = C.type_len[h];
			a = C.data_sorted[f];
			if (f_v) {
				cout << a << "-hyperplanes: ";
				}
			for (j = 0; j < l; j++) {
				b = C.sorting_perm_inv[f + j];
				S[j] = b;
				}
			int_vec_quicksort_increasingly(S, l);
			int_vec_print(cout, S, l);
			cout << endl;

#if 0
			look_at_these_planes(P, P2, 
				Gr, S /*set_of_planes*/, l /* nb_planes*/, 
				set, set_size, verbose_level);
#endif
			}
		FREE_int(S);
		//FREE_int(I);
#if 0
		cout << "i : intersection number of line i" << endl;
		for (i = 0; i < P->N_lines; i++) {
			cout << setw(4) << i << " : " << setw(3) << intersection_numbers[i] << endl;
			}
#endif
		}




	delete Gr;
	FREE_int(v);
	FREE_int(hyperplane_type);
}


