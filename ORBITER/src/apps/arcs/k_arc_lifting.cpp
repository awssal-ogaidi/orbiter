// k_arc_lifting.C
//
// Anton Betten, Awss Al-ogaidi
//
// Sept 15, 2017

#include "orbiter.h"


// global data:

int t0; // the system time when the program started

int main(int argc, char **argv);
void arc_lifting_from_classification_file(
	const char *classification_fname,
	projective_space *P, int k, int sz, 
	int f_Cook, int f_DLX, int f_McKay, 
	int f_split, int split_r, int split_m, 
	int f_save_system, const char *fname_system, 
	int f_solution_prefix, const char *solution_prefix, 
	int &nb_sol_total, 
	int verbose_level);
void do_arc_lifting(
	ostream &ost,
	projective_space *P, int k,
	int *arc, int arc_sz, int target_sz, 
	int f_save_system, const char *fname_system, 
	int f_Cook, int f_DLX, int f_McKay, 
	int &nb_backtrack_nodes,
	int verbose_level);
void user_callback_solution_found(
	int *sol, int len, int nb_sol, void *data);
void search(ostream &ost, int level);


	finite_field *F;
	projective_space *P;
	//action *A_linear;
int *arc;
int arc_sz;
	int *free_points;
	int nb_free_points;


	int *Cook_table2;
	int *Sz2;
	int w;
	int nb_km1_lines;
	int *km1_lines2;
	int *row; // [nb_needed]
	int *col; // [nb_needed]
	int nb_needed;
	int *Arc;
	int *line_type;
	int *Line_type_after; // [nb_needed * P->N_lines]
	int k = 0;
	int nb_sol = 0;
	int cnt = 0;
	//ofstream *fp;

int main(int argc, char **argv)
{
	int verbose_level = 0;
	int f_k = FALSE;
	int f_sz = FALSE;
	int sz = 0;
	int f_q = FALSE;
	int q = 0;
	int f_poly = FALSE;
	const char *poly = NULL;
	int f_arc = FALSE;
	const char *arc_text = NULL;
	int f_fining_labels = FALSE;
	int f_cook_labels = FALSE;
	int f_classification = FALSE;
	const char *classification_fname = NULL;
	int f_split = FALSE;
	int split_r = 0;
	int split_m = 0;
	int f_solution_prefix = FALSE;
	const char *solution_prefix = NULL;
	int f_Cook = FALSE;
	int f_DLX = FALSE;
	int f_McKay = FALSE;
	int f_save_system = FALSE;
	const char *fname_system = NULL;
	int f_loop = FALSE;
	int loop_start = -1;
	int loop_increment = 0;
	int i;

	t0 = os_ticks();
	cout << argv[0] << endl;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-k") == 0) {
			f_k = TRUE;
			k = atoi(argv[++i]);
			cout << "-k " << k << endl;
			}
		else if (strcmp(argv[i], "-sz") == 0) {
			f_sz = TRUE;
			sz = atoi(argv[++i]);
			cout << "-sz " << sz << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		else if (strcmp(argv[i], "-poly") == 0) {
			f_poly = TRUE;
			poly = argv[++i];
			cout << "-poly " << poly << endl;
			}
		else if (strcmp(argv[i], "-arc") == 0) {
			f_arc = TRUE;
			arc_text = argv[++i];
			cout << "-arc " << arc_text << endl;
			}
		else if (strcmp(argv[i], "-classification") == 0) {
			f_classification = TRUE;
			classification_fname = argv[++i];
			cout << "-classification " << classification_fname << endl;
			}
		else if (strcmp(argv[i], "-solution_prefix") == 0) {
			f_solution_prefix = TRUE;
			solution_prefix = argv[++i];
			cout << "-solution_prefix " << solution_prefix << endl;
			}
		else if (strcmp(argv[i], "-split") == 0) {
			f_split = TRUE;
			split_r = atoi(argv[++i]);
			split_m = atoi(argv[++i]);
			split_r = split_r % split_m;
				// added so that 0 can be written as m.
				// The Sussex HPD does not allow zero for a TASK_ID.
			cout << "-split " << split_r << " " << split_m << endl;
			}
		else if (strcmp(argv[i], "-fining_labels") == 0) {
			f_fining_labels = TRUE;
			cout << "-fining_labels " << endl;
			}
		else if (strcmp(argv[i], "-cook_labels") == 0) {
			f_cook_labels = TRUE;
			cout << "-cook_labels " << endl;
			}
		else if (strcmp(argv[i], "-Cook") == 0) {
			f_Cook = TRUE;
			cout << "-Cook " << endl;
			}
		else if (strcmp(argv[i], "-DLX") == 0) {
			f_DLX = TRUE;
			cout << "-DLX " << endl;
			}
		else if (strcmp(argv[i], "-McKay") == 0) {
			f_McKay = TRUE;
			cout << "-McKay " << endl;
			}
		else if (strcmp(argv[i], "-save_system") == 0) {
			f_save_system = TRUE;
			fname_system = argv[++i];
			cout << "-save_system " << fname_system << endl;
			}
		else if (strcmp(argv[i], "-loop") == 0) {
			f_loop = TRUE;
			loop_start = atoi(argv[++i]);
			loop_increment = atoi(argv[++i]);
			cout << "-loop " << loop_start << " "
					<< loop_increment << endl;
			}
		}
	
	if (!f_k) {
		cout << "please use option -k <k>" << endl;
		exit(1);
		}
	if (!f_sz) {
		cout << "please use option -sz <sz>" << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "please use option -q <q>" << endl;
		exit(1);
		}

	int f_v = (verbose_level >= 1);
	
	cout << "k=" << k << endl;
	cout << "q=" << q << endl;
	cout << "poly=";
	if (f_poly) {
		cout << poly;
		}
	else {
		cout << endl;
		}
	

	if (f_v) {
		cout << "creating finite field:" << endl;
		}
	F = NEW_OBJECT(finite_field);
	F->init_override_polynomial(q, poly, verbose_level);


	if (f_v) {
		cout << "creating projective space "
				"PG(" << 2 << ", " << q << ")" << endl;
		}


	P = NEW_OBJECT(projective_space);

	if (f_v) {
		cout << "before P->init" << endl;
		}
	P->init(2, F, 
		FALSE /* f_init_incidence_structure */, 
		0 /*verbose_level - 2*/);

	P->init_incidence_structure(verbose_level);
	

	if (f_arc) {
		cout << "processing a given arc" << endl;
		if (f_fining_labels) {
			int a;
			int v[3];
			
			for (a = 1; a <= P->N_points; a++) {
				P->F->PG_element_unrank_fining(v, 3, a);
				cout << a << " : ";
				int_vec_print(cout, v, 3);
				cout << endl;
				}
			}
		else if (f_cook_labels) {
			int a;
			int v[3];
			int j, qm1o2;
			
			qm1o2 = (q - 1) >> 1;
			for (a = 0; a < P->N_points; a++) {
				P->F->PG_element_unrank_gary_cook(v, 3, a);
				cout << a << " : (";
				for (i = 0; i < 3; i++) {
					j = v[i];
					if (j > qm1o2) {
						cout << j - q;
						}
					else {
						cout << j;
						}
					if (i < 3 - 1) {
						cout << ", ";
						}
					}
				//int_vec_print(cout, v, 3);
				cout << ")" << endl;
				}
			}

		int *the_arc;
		int the_arc_sz;
		int nb_backtrack_nodes;
		
		int_vec_scan(arc_text, the_arc, the_arc_sz);
		cout << "input arc of size " << the_arc_sz << " = ";
		int_vec_print(cout, the_arc, the_arc_sz);
		cout << endl;


		if (f_fining_labels) {
			int a, b;
			int v[3];
			int w[3];
			
			cout << "changing from fining to orbiter:" << endl;
			for (i = 0; i < the_arc_sz; i++) {
				a = the_arc[i];
				P->F->PG_element_unrank_fining(v, 3, a);
				int_vec_copy(v, w, 3);
				P->F->PG_element_rank_modified(w, 1, 3, b);
				cout << a << " : ";
				int_vec_print(cout, v, 3);
				cout << " : " << b << endl;
				the_arc[i] = b;
				}
			cout << "input arc in orbiter labels = ";
			int_vec_print(cout, the_arc, the_arc_sz);
			cout << endl;
			}
		else if (f_cook_labels) {
			int a, b;
			int v[3];
			int w[3];
			
			cout << "changing from Gary Cook to orbiter:" << endl;
			for (i = 0; i < the_arc_sz; i++) {
				a = the_arc[i];
				P->F->PG_element_unrank_gary_cook(v, 3, a);
				int_vec_copy(v, w, 3);
				P->F->PG_element_rank_modified(w, 1, 3, b);
				cout << a << " : ";
				int_vec_print(cout, v, 3);
				cout << " : " << b << endl;
				the_arc[i] = b;
				}
			cout << "input arc in orbiter labels = ";
			int_vec_print(cout, the_arc, the_arc_sz);
			cout << endl;
			}

		int_vec_heapsort(the_arc, the_arc_sz);

		cout << "input arc in orbiter labels sorted= ";
		int_vec_print(cout, the_arc, the_arc_sz);
		cout << endl;

		
		arc = the_arc;
		arc_sz = the_arc_sz;
		
		const char *fname = "solutions.txt";
		{
		ofstream fp(fname);
		do_arc_lifting(
			fp,
			P, k, the_arc, the_arc_sz, sz,
			f_save_system, fname_system, 
			f_Cook, f_DLX, f_McKay, 
			nb_backtrack_nodes,
			verbose_level);
		}
		cout << "written file " << fname << " of size " << file_size(fname) << endl;
		}
	else if (f_classification) {
		cout << "processing classification" << endl;
		if (f_loop) {
			char classification_fname2[10000];
			int loop_end, nb_loop_iterations = 0, cur_loop_iterations;
			int nb_sol_overall = 0;


			for (cnt = loop_start; TRUE; cnt += loop_increment) {
				sprintf(classification_fname2, classification_fname, cnt);
				if (file_size(classification_fname2) <= 0) {
					cout << "The file " << classification_fname2
							<< " does not exist, we break off" << endl;
					break;
					}
				loop_end = cnt;
				nb_loop_iterations++;
				}
			
			cout << "Processing loop from " << loop_start << " to "
					<< loop_end << " in increments of " << loop_increment
					<< " which is " << nb_loop_iterations
					<< " iterations" << endl;
			
			for (cur_loop_iterations = 0;
					cur_loop_iterations < nb_loop_iterations;
					cur_loop_iterations++) {
				cnt = loop_start + cur_loop_iterations * loop_increment;
				cout << "Loop " << cur_loop_iterations << "/ "
						<< nb_loop_iterations << " is case "
						<< cnt << ":" << endl;
				sprintf(classification_fname2, classification_fname, cnt);

				int nb_sol_total = 0;
				arc_lifting_from_classification_file(
					classification_fname2,
					P, k, sz, 
					f_Cook, f_DLX, f_McKay, 
					f_split, split_r, split_m, 
					f_save_system, fname_system, 
					f_solution_prefix, solution_prefix, 
					nb_sol_total, 
					verbose_level - 2);
				nb_sol_overall += nb_sol_total;
				cout << "Finished loop " << cur_loop_iterations << "/ "
						<< nb_loop_iterations << " is case " << cnt
						<< ": nb_sol_total = " << nb_sol_total
						<< " nb_sol_overall=" << nb_sol_overall << endl;
				}
			cout << "nb_sol_overall = " << nb_sol_overall << endl;
			}
		else {
			int nb_sol_total = 0;
			
			arc_lifting_from_classification_file(classification_fname, 
				P, k, sz, 
				f_Cook, f_DLX, f_McKay, 
				f_split, split_r, split_m, 
				f_save_system, fname_system, 
				f_solution_prefix, solution_prefix, 
				nb_sol_total, 
				verbose_level - 2);
			cout << "nb_sol_total = " << nb_sol_total << endl;
			}
		
		}

	FREE_OBJECT(P);
	FREE_OBJECT(F);

	the_end(t0);
}



void arc_lifting_from_classification_file(
	const char *classification_fname,
	projective_space *P, int k, int sz, 
	int f_Cook, int f_DLX, int f_McKay, 
	int f_split, int split_r, int split_m, 
	int f_save_system, const char *fname_system, 
	int f_solution_prefix, const char *solution_prefix, 
	int &nb_sol_total, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	finite_field *F;
	int q;


	if (f_v) {
		cout << "arc_lifting_from_classification_file" << endl;
		}
	if (f_v) {
		cout << "reading from file " << classification_fname << endl;
		}

	nb_sol_total = 0;
	F = P->F;
	q = F->q;
	if (f_split) {
		cout << "split: doing only the cases congruent to " << split_r
				<< " modulo " << split_m << endl;
		}

	action *A;
	int f_basis = TRUE;
	int f_semilinear = FALSE;
	if (f_v) {
		cout << "arc_generator::init calling init_matrix_group" << endl;
		}

	if (is_prime(q)) {
		f_semilinear = FALSE;
		}
	else {
		f_semilinear = TRUE;
		}

	A = NEW_OBJECT(action);
	A->init_projective_group(3, F,
			f_semilinear, f_basis, 0 /*verbose_level*/);

	if (f_v) {
		cout << "arc_generator::init after init_projective_group" << endl;
		}


	if (f_v) {
		cout << "reading all orbit representatives from "
				"file " << classification_fname << endl;
		}
	orbit_transversal *T;

	T = NEW_OBJECT(orbit_transversal);
	T->read_from_file(A, A, classification_fname, verbose_level - 1);

	if (f_v) {
		cout << "We read all orbit representatives. "
				"There are " << T->nb_orbits << " orbits" << endl;
		}

	set_and_stabilizer *rep;
	int *Nb_sol;
	int *Cnt;
	//int nb_sol_total = 0;
	int orbit_idx;

	Nb_sol = NEW_int(T->nb_orbits);
	Cnt = NEW_int(T->nb_orbits);

	int_vec_zero(Nb_sol, T->nb_orbits);
	int_vec_zero(Cnt, T->nb_orbits);


	char solution_fname[1000];
	char success_fname[1000];



	if (!f_solution_prefix) {
		sprintf(solution_fname,
				"arc_%d_%d_from_%s",
				sz, k,
				strip_directory(classification_fname));
		}
	else {
		sprintf(solution_fname,
				"%sarc_%d_%d_from_%s",
				solution_prefix, sz, k,
				strip_directory(classification_fname));
		}

	if (f_split) {
		sprintf(solution_fname + strlen(solution_fname),
				"_case_r%d_m%d", split_r, split_m);
		}
	sprintf(success_fname, "%s.success", solution_fname);



	if (f_v) {
		cout << "We will write the solutions to the "
				"file " << solution_fname << endl;
		cout << "verbose_level = " << verbose_level << endl;
		}

	{
		ofstream Fp(solution_fname);
		int case_cnt = 0;
		//fp = &Fp;


		for (orbit_idx = 0; orbit_idx < T->nb_orbits; orbit_idx++) {
			rep = &T->Reps[orbit_idx];


			if (f_split) {
				if ((orbit_idx % split_m) != split_r) {
					continue;
					}
				}
			case_cnt++;
#if 0
			if (orbit_idx != 264721) {
				continue;
				}
#endif

			if (f_vv) {
				cout << "File " << classification_fname
						<< " We will consider representative "
						<< orbit_idx << " / " << T->nb_orbits << endl;
				}
			if (f_vvv) {
				cout << " which is " << endl;
				rep->print_set_tex(cout);
				cout << endl;
				}

			arc = rep->data;
			arc_sz = rep->sz;
	
			if (f_vv) {
				cout << "File " << classification_fname
						<< " Lifting arc " << orbit_idx << " / "
						<< T->nb_orbits
						<< " nb_sol_total=" << nb_sol_total << endl;
				}
			nb_sol = 0;
			cnt = 0;
			int nb_backtrack_nodes;

			Fp << "# start arc " << orbit_idx << endl;

			int t0 = os_ticks();

			do_arc_lifting(Fp,
				P, k, arc, arc_sz, sz,
				f_save_system, fname_system, 
				f_Cook, f_DLX, f_McKay, 
				nb_backtrack_nodes,
				verbose_level - 2);

			int t1, dt;

			t1 = os_ticks();
			dt = t1 - t0;



			Fp << "# finish arc " << orbit_idx << " with " << nb_sol
					<< " solutions, nb_backtrack_nodes=" << nb_backtrack_nodes;
			if (f_Cook) {
				Fp << " -Cook";
			}
			else if (f_DLX) {
				Fp << " -DLX";
			}
			else if (f_McKay) {
				Fp << " -McKay";
			}

			Fp << " : time ";
			time_check_delta(Fp, dt);

			Fp << endl;


			Nb_sol[orbit_idx] = nb_sol;
			Cnt[orbit_idx] = cnt;
			nb_sol_total += nb_sol;
			if (f_v || ((case_cnt % 1024) == 0)) {
				cout << "File " << classification_fname
						<< " Lifting arc " << orbit_idx << " / "
						<< T->nb_orbits << " done, nb_sol = "
						<< nb_sol << " nb_sol_total="
						<< nb_sol_total << endl;
				}
	
			}

		if (f_v) {
			cout << "i : Cnt[i] : Nb_sol[i]" << endl;
			for (orbit_idx = 0; orbit_idx < T->nb_orbits; orbit_idx++) {
				if (Nb_sol[orbit_idx] == 0) {
					continue;
					}
				cout << orbit_idx << " : " << Cnt[orbit_idx]
					<< " : " << Nb_sol[orbit_idx] << endl;
				}
			}
		Fp << -1 << endl;
	}
	cout << "Written file " << solution_fname << " of size "
			<< file_size(solution_fname) << endl;
	{
	ofstream fp(success_fname);
	fp << "success" << endl;
	}
	if (f_v) {
		cout << "arc_lifting_from_classification_file done" << endl;
		}
}

void do_arc_lifting(
	ostream &ost,
	projective_space *P, int k,
	int *arc, int arc_sz, int target_sz, 
	int f_save_system, const char *fname_system, 
	int f_Cook, int f_DLX, int f_McKay, 
	int &nb_backtrack_nodes,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *Coord;
	int i, j, a, d, pt;

	if (f_v) {
		cout << "do_arc_lifting" << endl;
		}
	d = P->n + 1;
	F = P->F;
	if (f_v) {
		cout << "do_arc_lifting, the arc is ";
		int_vec_print(cout, arc, arc_sz);
		cout << endl;
		}

	nb_backtrack_nodes = 0;

	free_points = NEW_int(P->N_points);

	set_complement(arc, arc_sz,
			free_points, nb_free_points, P->N_points);

	if (f_v) {
		cout << "nb_free_points = " << nb_free_points << endl;
		}


	Coord = NEW_int(arc_sz * d);
	for (i = 0; i < arc_sz; i++) {
		a = arc[i];
		F->projective_point_unrank(P->n, Coord + i * d, a);
		}

	if (f_vv) {
		for (i = 0; i < arc_sz; i++) {
			cout << arc[i] << " : ";
			int_vec_print(cout, Coord + i * d, d);
			cout << endl;
			}
		}


	
	int *type_collected;

	type_collected = NEW_int(arc_sz + 1);
	P->line_intersection_type_collected(arc, arc_sz, 
		type_collected, 0 /* verbose_level */);

	if (f_vv) {
		cout << "line type collected:";
		int_vec_print(cout, type_collected, arc_sz + 1);
		cout << endl;
		for (i = 0; i <= arc_sz; i++) {
			cout << i << " : " << type_collected[i] << endl;
			}
		}
	
	longinteger_object go;
	int f_semilinear;

	if (is_prime(F->q)) {
		f_semilinear = FALSE;
		}
	else {
		f_semilinear = TRUE;
		}
#if 0
	action *A_linear;

	A_linear = NEW_OBJECT(action);
	A_linear->init_projective_group(P->n + 1, F,
			f_semilinear, TRUE /*f_basis */,
			0 /*verbose_level*/);
	

	{
	if (f_vv) {
		cout << "computing stabilizer of the arc:" << endl;
		}
	sims *Stab;
	strong_generators *SG;
	int canonical_pt;
	int *canonical_set;

	canonical_set = NEW_int(arc_sz);

	Stab = set_stabilizer_in_projective_space(
		A_linear, P, 
		arc, arc_sz, canonical_pt, canonical_set, 
		FALSE, NULL, 
		verbose_level - 3);
		// in ACTION/action_global.C
	if (f_vv) {
		cout << "The stabilizer of the arc has been computed" << endl;
		}
	Stab->group_order(go);
	if (f_v) {
		cout << "The stabilizer of the arc is a group of order " << go << endl;
		}
	SG = NEW_OBJECT(strong_generators);
	SG->init_from_sims(Stab, verbose_level);
	cout << "generators:" << endl;
	SG->print_generators_tex(cout);

	FREE_int(canonical_set);
	FREE_OBJECT(Stab);
	FREE_OBJECT(A_linear);
	exit(1);
	}
#endif


	diophant *D;
	//int *line_type;



	line_type = NEW_int(P->N_lines);
	P->line_intersection_type(arc, arc_sz,
			line_type, 0 /* verbose_level */);
	if (f_vv) {
		cout << "line_type: ";
		int_vec_print_fully(cout, line_type, P->N_lines);
		cout << endl;
		}

	if (f_vv) {
		cout << "line type:" << endl;
		for (i = 0; i < P->N_lines; i++) {
			cout << i << " : " << line_type[i] << endl;
			}
		}

	classify C;
	C.init(line_type, P->N_lines, FALSE, 0);
	if (f_v) {
		cout << "line_type:";
		C.print_naked(TRUE);
		cout << endl;
		cout << "nb_free_points=" << nb_free_points << endl;
		}
	

	//exit(1);
	
	int h;
	
	D = NEW_OBJECT(diophant);
	D->open(P->N_lines + 1, nb_free_points);
	D->f_x_max = TRUE;
	for (j = 0; j < nb_free_points; j++) {
		D->x_max[j] = 1;
		}
	D->f_has_sum = TRUE;
	D->sum = target_sz - arc_sz;
	h = 0;
	for (i = 0; i < P->N_lines; i++) {
		if (line_type[i] > k) {
			cout << "line_type[i] > k" << endl;
			exit(1);
			}
#if 0
		if (line_type[i] < k - 1) {
			continue;
			}
#endif
		for (j = 0; j < nb_free_points; j++) {
			pt = free_points[j];
			if (P->is_incident(pt, i /* line */)) {
				D->Aij(h, j) = 1;
				}
			else {
				D->Aij(h, j) = 0;
				}
			}
		D->type[h] = t_LE;
		D->RHSi(h) = k - line_type[i];
		h++;
		}


	// add one extra row:
	for (j = 0; j < nb_free_points; j++) {
		D->Aij(h, j) = 1;
		}
	D->type[h] = t_EQ;
	D->RHSi(h) = target_sz - arc_sz;
	h++;
	
	D->m = h;

	if (f_vv) {
		cout << "do_arc_lifting The system is:" << endl;
		D->print_tight();
		}


	if (f_save_system) {
		cout << "do_arc_lifting saving the system "
				"to file " << fname_system << endl;
		D->save_in_general_format(fname_system, 0 /* verbose_level */);
		cout << "do_arc_lifting saving the system "
				"to file " << fname_system << " done" << endl;
		D->print();
		D->print_tight();
		}


	if (f_Cook) {
		// and now the approach due to Cook:

		cout << "Cook approach:" << endl;
		
		int *km1_lines; // km1 stands for k minus one
	

		nb_km1_lines = 0;

	
		h = 0;
		for (i = 0; i < P->N_lines; i++) {
			if (line_type[i] == k - 1) {
				nb_km1_lines++;
				}
			}
		km1_lines = NEW_int(nb_km1_lines);
			// number of rows in the Cook table

		h = 0;
		for (i = 0; i < P->N_lines; i++) {
			if (line_type[i] == k - 1) {
				km1_lines[h++] = i;
				}
			}
		int *Cook_table;
		int line, idx;

		w = F->q + 1 - (k - 1);
			// number of columns in the Cook table


		Cook_table = NEW_int(nb_km1_lines * w);
		for (i = 0; i < nb_km1_lines; i++) {
			line = km1_lines[i];
			h = 0;
			for (j = 0; j < F->q + 1; j++) {
				pt = P->Lines[line * P->k + j];
					// j-th point on i-th (k-1)-line
			
				// thes it pt is already in the arc,
				// skip if so.
				if (!int_vec_search(arc, arc_sz, pt, idx)) {

					// otherwise, put it in the Cook table:
					Cook_table[i * w + h++] = pt;
					}
				}
			if (h != w) {
				cout << "h != w" << endl;
				exit(1);
				}
			}

		if (f_vv) {
			cout << "The Cook table:" << endl;
			for (i = 0; i < nb_km1_lines; i++) {
				cout << i << " : " << w << " : " << km1_lines[i] << " : ";
				int_vec_print(cout, Cook_table + i * w, w);
				cout << endl;
				}
			cout << endl;
			}

		int *Sz;
		int l, u;

		Sz = NEW_int(nb_km1_lines);
		for (i = 0; i < nb_km1_lines; i++) {
			Sz[i] = w;
			}
		for (i = 0; i < nb_km1_lines; i++) {
			for (j = 0; j < Sz[i]; j++) {
				pt = Cook_table[i * w + j];
				for (h = i + 1; h < nb_km1_lines; h++) {
					for (l = 0; l < Sz[h]; l++) {
						if (Cook_table[h * w + l] == pt) {
							for (u = l + 1; u < Sz[h]; u++) {
								Cook_table[h * w + u - 1] =
									Cook_table[h * w + u];
								}
							Sz[h]--;
							l--;
							}
						}
					}
				}
			}
		if (f_vv) {
			cout << "The Cook table:" << endl;
			for (i = 0; i < nb_km1_lines; i++) {
				cout << i << " : " << Sz[i] << " : " << km1_lines[i] << " : ";
				int_vec_print(cout, Cook_table + i * w, Sz[i]);
				cout << endl;
				}
			cout << endl;
			}
	
		int *perm;
		int *perm_inv;
		int ii;
	
		perm = NEW_int(nb_km1_lines);
		perm_inv = NEW_int(nb_km1_lines);
	



	
		km1_lines2 = NEW_int(nb_km1_lines);
		Sz2 = NEW_int(nb_km1_lines);



		int_vec_copy(Sz, Sz2, nb_km1_lines);
		int_vec_sorting_permutation(Sz2,
				nb_km1_lines, perm, perm_inv,
				FALSE /* f_increasingly */);



		Cook_table2 = NEW_int(nb_km1_lines * w);
		int_vec_zero(Cook_table2, nb_km1_lines * w);
		for (i = 0; i < nb_km1_lines; i++) {
			ii = perm_inv[i];
			for (j = 0; j < Sz[ii]; j++) {
				Cook_table2[i * w + j] = Cook_table[ii * w + j];
				}
			Sz2[i] = Sz[ii];
			km1_lines2[i] = km1_lines[ii];
			}

		if (f_vv) {
			cout << "The sorted Cook table:" << endl;
			for (i = 0; i < nb_km1_lines; i++) {
				cout << i << " : " << Sz2[i] << " : " << km1_lines2[i] << " : ";
				int_vec_print(cout, Cook_table2 + i * w, Sz2[i]);
				cout << endl;
				}
			cout << endl;
			}

		nb_needed = target_sz - arc_sz;

		if (f_vv) {
			cout << "nb_needed = " << nb_needed << endl;
			}

		row = NEW_int(nb_needed);
		col = NEW_int(nb_needed);

		Arc = NEW_int(nb_needed);



		line_type = NEW_int(P->N_lines);
		P->line_intersection_type(arc, arc_sz,
				line_type, 0 /* verbose_level */);
		if (f_vv) {
			cout << "line_type: " << endl;
			int_vec_print_fully(cout, line_type, P->N_lines);
			cout << endl;

			cout << "line type:" << endl;
			for (i = 0; i < P->N_lines; i++) {
				cout << i << " : " << line_type[i] << endl;
				}
			}


		Line_type_after = NEW_int(nb_needed * P->N_lines);
			// [nb_needed * P->N_lines]
	

#if 1
		if (f_v) {
			cout << "starting the search" << endl;
			}
		search(ost, 0);
		if (f_v) {
			cout << "the search is finished" << endl;
			}

		//exit(0);
#endif

		FREE_int(km1_lines);
		FREE_int(Cook_table);
		FREE_int(Sz);
		FREE_int(perm);
		FREE_int(perm_inv);
		FREE_int(km1_lines2);
		FREE_int(Sz2);
		FREE_int(Cook_table2);
		FREE_int(row);
		FREE_int(col);
		FREE_int(Arc);
		FREE_int(Line_type_after);

		}
	else if (f_DLX) {
		if (f_v) {
			cout << "before solve_all_DLX_with_RHS" << endl;
			}
		D->solve_all_DLX_with_RHS(
			FALSE /* f_write_tree */,
			"" /* "search_tree.tree"*/ /* fname_tree */,
			verbose_level - 1);
		nb_sol = D->_resultanz;
		if (f_v) {
			cout << "after solve_all_DLX_with_RHS" << endl;
			}
		}
	else if (f_McKay) {
		if (f_v) {
			cout << "before solve_all_mckay" << endl;
			}

		D->solve_all_mckay(nb_backtrack_nodes,
				verbose_level - 2);
		if (f_v) {
			cout << "after solve_all_mckay" << endl;
			}
		
		//D->solve_once_mckay(verbose_level - 1);
		nb_sol = D->_resultanz;
		if (f_v) {
			cout << "after solve_all_mckay "
					"nb_sol = " << nb_sol << endl;
			}
		if (nb_sol) {
			int *Sol;
			int nb_sol;
			int i, j, a;
			int *big_arc;

			big_arc = NEW_int(target_sz);
			
			int_vec_copy(arc, big_arc, arc_sz);
			if (f_v) {
				cout << "before D->get_solutions" << endl;
				}
			D->get_solutions(Sol, nb_sol, 0 /* verbose_level */);
			if (f_v) {
				cout << "after D->get_solutions" << endl;
				}
			for (i = 0; i < nb_sol; i++) {
				for (j = 0; j < D->sum; j++) {
					a = Sol[i * D->sum + j];
					pt = free_points[a];
					big_arc[arc_sz + j] = pt;
					}
				
				ost << target_sz;
				for (j = 0; j < target_sz; j++) {
					ost << " " << big_arc[j];
					}
				//*fp << " " << go;
				ost << endl;

				}
			FREE_int(big_arc);
			FREE_int(Sol);
			}
		if (f_v) {
			cout << "after solve_once_mckay" << endl;
			}
		}
	else {
		cout << "please specify an option for how "
				"to solve the system" << endl;
		exit(1);
		}






#if 0
	const char *fname_sol = "solutions.txt";

	if (FALSE /*file_size(fname_sol) > 0*/) {
		int *Sol;
		int nb_sol;

		D->read_solutions_from_file(fname_sol, verbose_level);
		D->get_solutions_full_length(Sol, nb_sol, verbose_level);
		cout << "read solutions from file:" << endl;
		for (i = 0; i < MINIMUM(10, nb_sol); i++) {
			cout << "solution " << i << " / " << nb_sol << ":" << endl;
			int_vec_print_fully(cout,
					Sol + i * nb_free_points, nb_free_points);
			cout << endl;
			}
		for (i = 0; i < nb_sol; i++) {
			D->test_solution_full_length(Sol + i * nb_free_points,
					verbose_level);
			}
		cout << "The solutions are OK" << endl;
		}
	else {
		D->save_in_general_format("system.diophant",
				0 /* verbose_level */);
		D->print();
		D->print_tight();
		}


	cout << "before with_RHS_and_callback" << endl;
	D->solve_all_DLX_with_RHS_and_callback(TRUE /* f_write_tree */,
			"search_tree.tree" /* fname_tree */,
		user_callback_solution_found, 
		1 /* verbose_level */);

	exit(1);
#endif
	

	FREE_OBJECT(D);
	//FREE_OBJECT(A_linear);
	FREE_int(type_collected);
	FREE_int(Coord);

	FREE_int(free_points);
	FREE_int(line_type);
}


void user_callback_solution_found(
		int *sol, int len, int nb_sol, void *data)
{
	cout << "user_callback_solution_found" << endl;
	nb_sol++;
	cout << "nb_sol=" << nb_sol << endl;

	int *big_arc;
	int big_arc_size;
	int i, a;


	big_arc_size = arc_sz + len;
	cout << "big_arc_size=" << big_arc_size << endl;
	cout << "sol=";
	int_vec_print(cout, sol, len);
	cout << endl;

	big_arc = NEW_int(big_arc_size);
	int_vec_copy(arc, big_arc, arc_sz);
	for (i = 0; i < len; i++) {
		a = sol[i];
		big_arc[arc_sz + i] = free_points[a];
		}

	cout << "free_points: ";
	int_vec_print(cout, free_points, nb_free_points);
	cout << endl;
	cout << "The big arc is: ";
	int_vec_print(cout, big_arc, big_arc_size);
	cout << endl;
	
	int *line_type;


	line_type = NEW_int(P->N_lines);
	P->line_intersection_type(big_arc,
			big_arc_size, line_type, 0 /* verbose_level */);
	cout << "line_type: " << endl;
	int_vec_print_fully(cout, line_type, P->N_lines);
	cout << endl;

	cout << "line type:" << endl;
	for (i = 0; i < P->N_lines; i++) {
		cout << i << " : " << line_type[i] << endl;
		}



#if 0
	cout << "computing stabilizer of the arc:" << endl;
	sims *Stab;
	int canonical_pt;
	longinteger_object go;
	int *canonical_set2;

	canonical_set2 = NEW_int(big_arc_size);

	Stab = set_stabilizer_in_projective_space(
		A_linear, P, 
		big_arc, big_arc_size, canonical_pt, canonical_set2, 
		FALSE, NULL, 
		0 /*verbose_level*/);
		// in ACTION/action_global.C
	cout << "The stabilizer of the arc has been computed" << endl;
	Stab->group_order(go);
	cout << "It is a group of order " << go << endl;

	FREE_int(canonical_set2);

	int *type_collected;

	type_collected = NEW_int(big_arc_size + 1);
	P->line_intersection_type_collected(big_arc, big_arc_size, 
		type_collected, 0 /* verbose_level */);

	cout << "line type collected:";
	int_vec_print(cout, type_collected, big_arc_size + 1);
	cout << endl;
	for (i = 0; i <= big_arc_size; i++) {
		cout << i << " : " << type_collected[i] << endl;
		}


	//exit(1);

	FREE_int(type_collected);
#endif
	FREE_int(line_type);
	FREE_int(big_arc);
	
}
 


void search(ostream &ost, int level)
{
	int b, i, line, pt, r0;
	int *line_type_before;
	int *line_type_after;
	
	cnt++;

	if ((cnt & ((1 << 20) - 1)) == 0) {
		cout << "cnt=" << cnt << " level = " << level << " :";
		int_vec_print(cout, row, level);
		cout << endl;
		}
	
#if 0
	cout << "cnt = " << cnt << " level = " << level << " : ";
	int_vec_print(cout, Arc, level);
	cout << endl;
#endif
	
	if (level == nb_needed) {
		cout << "found an arc, solution no " << nb_sol << " : ";
		int_vec_print(cout, Arc, nb_needed);
		//cout << endl;

		int *big_arc;
		int big_arc_size;
		
		big_arc_size = arc_sz + nb_needed;
		big_arc = NEW_int(big_arc_size);
		int_vec_copy(arc, big_arc, arc_sz);
		for (i = 0; i < nb_needed; i++) {
			big_arc[arc_sz + i] = Arc[i];
			}
		cout << " : ";
		int_vec_print(cout, big_arc, big_arc_size);
		//cout << endl;


#if 0
		sims *Stab;
		int canonical_pt;
		int *canonical_set;
		longinteger_object go;

		canonical_set = NEW_int(big_arc_size);

		Stab = set_stabilizer_in_projective_space(
			A_linear, P, 
			big_arc, big_arc_size, canonical_pt, canonical_set, 
			FALSE, NULL, 
			0 /*verbose_level - 3 */);
			// in ACTION/action_global.C
		Stab->group_order(go);
			cout << " : go=" << go;

		int_vec_heapsort(canonical_set, big_arc_size);

		cout << " : ";
		int_vec_print(cout, canonical_set, big_arc_size);
		cout << endl;

		FREE_int(canonical_set);
		delete Stab;
#endif

		int *type_collected;

		type_collected = NEW_int(big_arc_size + 1);
		P->line_intersection_type_collected(big_arc, big_arc_size, 
			type_collected, 0 /* verbose_level */);

		cout << "line type:" << endl;
		//int_vec_print(cout, type_collected, big_arc_size + 1);
		//cout << endl;
		for (i = 0; i <= big_arc_size; i++) {
			if (type_collected[i] == 0) {
				continue;
				}
			cout << i << " : " << type_collected[i] << endl;
			}

		ost << big_arc_size;
		for (i = 0; i < big_arc_size; i++) {
			ost << " " << big_arc[i];
			}
		//ost << " " << go;
		ost << endl;

	//exit(1);

	FREE_int(type_collected);


		FREE_int(big_arc);

		nb_sol++;
		return;
		//exit(1);
		}
	b = nb_needed - level - 1;
	if (level == 0) {
		r0 = nb_km1_lines - 1;
		}
	else {
		r0 = row[level - 1] - 1;
		}
	for (row[level] = r0; row[level] >= b; row[level]--) {
		for (col[level] = 0;
				col[level] < Sz2[row[level]];
				col[level]++) {
			
			pt = Cook_table2[row[level] * w + col[level]];

			//cout << "level " << level << " row = " << row[level]
			//<< " col = " << col[level] << " pt = " << pt << endl;
			
			Arc[level] = pt;

			if (level == 0) {
				line_type_before = line_type;
				}
			else {
				line_type_before = Line_type_after + (level - 1) * P->N_lines;
				}
			line_type_after = Line_type_after + level * P->N_lines;
			
			int_vec_copy(line_type_before, line_type_after, P->N_lines);
			for (i = 0; i < F->q + 1; i++) {
				line = P->Lines_on_point[pt * P->k + i];
				line_type_after[line]++;
				if (line_type_after[line] > k) {
					break;
					}
				}
			if (i == F->q + 1) {
				search(ost, level + 1);
				}
			}
		}
}


