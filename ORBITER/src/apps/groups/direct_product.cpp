// direct_product.C
//
// Anton Betten
//
// August 12, 2018
//
//
//

#include "orbiter.h"

#define MAX_MASK_TESTS 1000

// global data:

INT t0; // the system time when the program started

void usage(int argc, const char **argv);
int main(int argc, const char **argv);
INT design_search_check_conditions(INT len, INT *S,
		void *data, INT verbose_level);


typedef class direct_product_action direct_product_action;

//! searching for line transitive point imprimitive designs preserving a grid structure on points


class direct_product_action {
public:
	int argc;
	const char **argv;

	INT d1;
	INT d2;
	INT q1;
	INT q2;

	const BYTE *group_label;

	finite_field *F1;
	finite_field *F2;


	INT DELANDTSHEER_DOYEN_X;
	INT DELANDTSHEER_DOYEN_Y;
	INT K;

	INT Xsize; // = D = q1 = # of rows
	INT Ysize; // = C = q2 = # of cols

	INT V; // = Xsize * Ysize
	INT b;
	INT *line;        // [K];
	INT *row_sum; // [Xsize]
	INT *col_sum; // [Ysize]


	matrix_group *M1;
	matrix_group *M2;
	action *A1;
	action *A2;

	action *A;
	action *A0;

	strong_generators *SG;
	longinteger_object go;
	direct_product *P;
	generator *Pairs;
	generator *Gen;

	// orbits on pairs:
	INT f_subgroup;
	INT *pair_orbit; // [V * V]
	INT nb_orbits;
	INT *transporter;
	INT *tmp_Elt;
	INT *orbit_length; 		// [nb_orbits]
	INT *orbit_covered; 		// [nb_orbits]
	INT *orbit_covered_max; 	// [nb_orbits]
		// orbit_covered_max[i] = orbit_length[i] / b;
	INT *orbits_covered; 		// [K * K]


	// intersection type tests:

	INT inner_pairs_in_rows;
	INT inner_pairs_in_cols;

	// row intersection type
	INT f_R;
	INT nb_row_types;
	INT *row_type;     		// [nb_row_types + 1]
	INT *row_type_cur; 		// [nb_row_types + 1]
	INT *row_type_this_or_bigger; 	// [nb_row_types + 1]

	// col intersection type
	INT f_C;
	INT nb_col_types;
	INT *col_type;     		// [nb_col_types + 1]
	INT *col_type_cur; 		// [nb_col_types + 1]
	INT *col_type_this_or_bigger; 	// [nb_col_types + 1]


	// mask related test:
	INT nb_mask_tests;
	INT mask_test_level[MAX_MASK_TESTS];
	INT mask_test_who[MAX_MASK_TESTS];
		// 1 = x
		// 2 = y
		// 3 = x+y
		// 4 = singletons
	INT mask_test_what[MAX_MASK_TESTS];
		// 1 = eq
		// 2 = ge
		// 3 = le
	INT mask_test_value[MAX_MASK_TESTS];

	// a file where we print the solution, it has the extension bblt
	// for "base block line transitive" design
	//FILE *fp_sol;
	//BYTE fname_solution_file[1000];

	// for testing the mask:
	INT *f_row_used; // [Xsize];
	INT *f_col_used; // [Ysize];
	INT *row_idx; // [Xsize];
	INT *col_idx; // [Ysize];
	INT *singletons; // [K];

	// temporary data
	INT *row_col_idx; // [Xsize];
	INT *col_row_idx; // [Ysize];

	INT *live_points; // [V]
	INT nb_live_points;

	direct_product_action();
	~direct_product_action();
	void init(int argc, const char **argv,
			INT d1, INT q1, INT d2, INT q2,
			INT f_subgroup, const BYTE *subgroup_gens_text,
			const BYTE *subgroup_order_text,
			const BYTE *group_label,
			INT depth,
			INT verbose_level);
	void create_graph(
			INT *line0, INT len, INT verbose_level);
	void testing(strong_generators *SG, INT verbose_level);
	INT find_pair_orbit(INT i, INT j, INT verbose_level);
	INT find_pair_orbit_by_tracing(INT i, INT j, INT verbose_level);
	void compute_pair_orbit_table(INT verbose_level);
	void write_pair_orbit_file(INT verbose_level);
	void print_mask_test_i(ostream &ost, INT i);
	INT check_conditions(INT *S, INT len, INT verbose_level);
	INT check_orbit_covering(INT *line,
			INT len, INT verbose_level);
	INT check_row_sums(INT *line,
			INT len, INT verbose_level);
	INT check_col_sums(INT *line,
			INT len, INT verbose_level);
	INT check_mask(INT *line,
			INT len, INT verbose_level);
	void get_mask_core_and_singletons(
		INT *line, INT len,
		INT &nb_rows_used, INT &nb_cols_used,
		INT &nb_singletons, INT verbose_level);
};





void usage(int argc, const char **argv)
{
	cout << "usage: " << argv[0] << " [options]" << endl;
	cout << "where options can be:" << endl;
	cout << "-v <v>                   : verbose level v" << endl;
	cout << "-d1 <d1>                 : set dimension d1" << endl;
	cout << "-q1 <q1>                 : set field size q1" << endl;
	cout << "-d2 <d2>                 : set dimension d2" << endl;
	cout << "-q2 <q2>                 : set field size q2" << endl;
}



int main(int argc, const char **argv)
{
	INT i;
	INT verbose_level = 0;
	INT f_d1 = FALSE;
	INT d1 = 0;
	INT f_d2 = FALSE;
	INT d2 = 0;
	INT f_q1 = FALSE;
	INT q1 = 0;
	INT f_q2 = FALSE;
	INT q2 = 0;
	INT f_depth = FALSE;
	INT depth = 0;
	INT f_subgroup = FALSE;
	const BYTE *subgroup_gens_text = NULL;
	const BYTE *subgroup_order_text = NULL;
	const BYTE *group_label = NULL;

	t0 = os_ticks();

	//f_memory_debug = TRUE;
	//f_memory_debug_verbose = TRUE;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-h") == 0) {
			usage(argc, argv);
			exit(1);
			}
		else if (strcmp(argv[i], "-help") == 0) {
			usage(argc, argv);
			exit(1);
			}
		else if (strcmp(argv[i], "-d1") == 0) {
			f_d1 = TRUE;
			d1 = atoi(argv[++i]);
			cout << "-d1 " << d1 << endl;
			}
		else if (strcmp(argv[i], "-d2") == 0) {
			f_d2 = TRUE;
			d2 = atoi(argv[++i]);
			cout << "-d2 " << d2 << endl;
			}
		else if (strcmp(argv[i], "-q1") == 0) {
			f_q1 = TRUE;
			q1 = atoi(argv[++i]);
			cout << "-q1 " << q1 << endl;
			}
		else if (strcmp(argv[i], "-q2") == 0) {
			f_q2 = TRUE;
			q2 = atoi(argv[++i]);
			cout << "-q2 " << q2 << endl;
			}
		else if (strcmp(argv[i], "-depth") == 0) {
			f_depth = TRUE;
			depth = atoi(argv[++i]);
			cout << "-depth " << depth << endl;
			}
		else if (strcmp(argv[i], "-subgroup") == 0) {
			f_subgroup = TRUE;
			subgroup_gens_text = argv[++i];
			subgroup_order_text = argv[++i];
			group_label = argv[++i];
			cout << "-subgroup " << subgroup_gens_text
					<< " " << subgroup_order_text
					<< " " << group_label << endl;
			}
		}
	if (!f_d1) {
		cout << "please use -d1 <d1>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_d2) {
		cout << "please use -d2 <d2>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_q1) {
		cout << "please use -q1 <q1>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_q2) {
		cout << "please use -q2 <q2>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_depth) {
		cout << "please use -depth <depth>" << endl;
		usage(argc, argv);
		exit(1);
		}


	//do_it(argc, argv, nb_factors, d, q, verbose_level);


	direct_product_action *T;

	T = NEW_OBJECT(direct_product_action);

	T->init(argc, argv, d1, q1, d2, q2,
			f_subgroup, subgroup_gens_text, subgroup_order_text, group_label,
			depth, verbose_level);


	the_end_quietly(t0);

}

direct_product_action::direct_product_action()
{
	argc= 0;
	argv = NULL;
	d1 = 0;
	d2 = 0;
	q1 = 0;
	q2 = 0;

	group_label = NULL;

	DELANDTSHEER_DOYEN_X = -1;
	DELANDTSHEER_DOYEN_Y = -1;
	K = 0;
	V = 0;
	b = 0;

	Xsize = 0; // = D = q1 = # of rows
	Ysize = 0; // = C = q2 = # of cols

	line = NULL;        // [K];
	row_sum = NULL;
	col_sum = NULL;


	M1 = NULL;
	M2 = NULL;
	A1 = NULL;
	A2 = NULL;
	SG = NULL;
	F1 = NULL;
	F2 = NULL;
	A = NULL;
	A0 = NULL;
	P = NULL;
	Pairs = NULL;
	Gen = NULL;

	f_subgroup = FALSE;
	pair_orbit = NULL;
	nb_orbits = 0;
	transporter = NULL;
	tmp_Elt = NULL;
	orbit_length = NULL;
	orbit_covered = NULL;
	orbit_covered_max = NULL;
		// orbit_covered_max[i] = orbit_length[i] / b;
	orbits_covered = NULL;


	// intersection type tests:

	inner_pairs_in_rows = 0;
	inner_pairs_in_cols = 0;

	// row intersection type
	f_R = FALSE;
	nb_row_types = 0;
	row_type = NULL;     		// [nb_row_types + 1]
	row_type_cur = NULL; 		// [nb_row_types + 1]
	row_type_this_or_bigger = NULL; 	// [nb_row_types + 1]

	// col intersection type
	f_C = FALSE;
	nb_col_types = 0;
	col_type = NULL;     		// [nb_col_types + 1]
	col_type_cur = NULL; 		// [nb_col_types + 1]
	col_type_this_or_bigger = NULL; 	// [nb_col_types + 1]


	// mask related test:
	nb_mask_tests = 0;

	// for testing the mask:
	f_row_used = NULL; // [Xsize];
	f_col_used = NULL; // [Ysize];
	row_idx = NULL; // [Xsize];
	col_idx = NULL; // [Ysize];
	singletons = NULL; // [K];

	// temporary data
	row_col_idx = NULL; // [Xsize];
	col_row_idx = NULL; // [Ysize];

	// a file where we print the solution, it has the extension bblt
	// for "base block line transitive" design
	//fp_sol = NULL;

	live_points = NULL;
	nb_live_points = 0;

}

direct_product_action::~direct_product_action()
{
	if (line) {
		FREE_INT(line);
	}
	if (row_sum) {
		FREE_INT(row_sum);
	}
	if (col_sum) {
		FREE_INT(col_sum);
	}
	if (pair_orbit) {
		FREE_INT(pair_orbit);
	}
	if (transporter) {
		FREE_INT(transporter);
	}
	if (tmp_Elt) {
		FREE_INT(tmp_Elt);
	}
	if (orbit_length) {
		FREE_INT(orbit_length);
	}
	if (orbit_covered) {
		FREE_INT(orbit_covered);
	}
	if (orbit_covered_max) {
		FREE_INT(orbit_covered_max);
	}
	if (orbits_covered) {
		FREE_INT(orbits_covered);
	}
	if (row_type) {
		FREE_INT(row_type);
	}
	if (row_type_cur) {
		FREE_INT(row_type_cur);
	}
	if (row_type_this_or_bigger) {
		FREE_INT(row_type_this_or_bigger);
	}
	if (col_type) {
		FREE_INT(col_type);
	}
	if (col_type_cur) {
		FREE_INT(col_type_cur);
	}
	if (col_type_this_or_bigger) {
		FREE_INT(col_type_this_or_bigger);
	}
	if (f_row_used) {
		FREE_INT(f_row_used);
	}
	if (f_col_used) {
		FREE_INT(f_col_used);
	}
	if (row_idx) {
		FREE_INT(row_idx);
	}
	if (col_idx) {
		FREE_INT(col_idx);
	}
	if (singletons) {
		FREE_INT(singletons);
	}
	if (row_col_idx) {
		FREE_INT(row_col_idx);
	}
	if (col_row_idx) {
		FREE_INT(col_row_idx);
	}
	if (live_points) {
		FREE_INT(live_points);
	}
}

void direct_product_action::init(int argc, const char **argv,
		INT d1, INT q1, INT d2, INT q2,
		INT f_subgroup, const BYTE *subgroup_gens_text,
		const BYTE *subgroup_order_text,
		const BYTE *group_label,
		INT depth,
		INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j, a;
	INT f_K = FALSE;
	INT f_singletons = FALSE;

	if (f_v) {
		cout << "direct_product_action::init" << endl;
	}
	direct_product_action::argc = argc;
	direct_product_action::argv = argv;
	direct_product_action::d1 = d1;
	direct_product_action::d2 = d2;
	direct_product_action::q1 = q1;
	direct_product_action::q2 = q2;
	direct_product_action::group_label = group_label;
	direct_product_action::f_subgroup = f_subgroup;

	A = NEW_OBJECT(action);
	A1 = NEW_OBJECT(action);
	A2 = NEW_OBJECT(action);

	nb_mask_tests = 0;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-DDx") == 0) {
			DELANDTSHEER_DOYEN_X = atoi(argv[++i]);
			cout << "-DDx " << DELANDTSHEER_DOYEN_X << endl;
		}
		else if (strcmp(argv[i], "-DDy") == 0) {
			DELANDTSHEER_DOYEN_Y = atoi(argv[++i]);
			cout << "-DDy " << DELANDTSHEER_DOYEN_Y << endl;
		}
		else if (strcmp(argv[i], "-K") == 0) {
			f_K = TRUE;
			K = atoi(argv[++i]);
			cout << "-K " << K << endl;
		}
		else if (strcmp(argv[i], "-R") == 0) {
			f_R = TRUE;
			nb_row_types = atoi(argv[++i]);
			row_type = NEW_INT(nb_row_types + 1);
			row_type_cur = NEW_INT(nb_row_types + 1);
			row_type_this_or_bigger = NEW_INT(nb_row_types + 1);
			row_type[0] = 0;
			for (j = 1; j <= nb_row_types; j++) {
				row_type[j] = atoi(argv[++i]);
				row_type_cur[j] = 0;
				}
			cout << "-R ";
			INT_vec_print(cout, row_type + 1, nb_row_types);
			cout << endl;
		}
		else if (strcmp(argv[i], "-C") == 0) {
			f_C = TRUE;
			nb_col_types = atoi(argv[++i]);
			col_type = NEW_INT(nb_col_types + 1);
			col_type_cur = NEW_INT(nb_col_types + 1);
			col_type_this_or_bigger = NEW_INT(nb_col_types + 1);
			col_type[0] = 0;
			for (j = 1; j <= nb_col_types; j++) {
				col_type[j] = atoi(argv[++i]);
				col_type_cur[j] = 0;
				}
			cout << "-C ";
			INT_vec_print(cout, col_type + 1, nb_col_types);
			cout << endl;
		}
		else if (strcmp(argv[i], "-masktest") == 0) {
			const char *who;
			const char *what;

			mask_test_level[nb_mask_tests] = atoi(argv[++i]);
			who = argv[++i];
			what = argv[++i];
			mask_test_value[nb_mask_tests] = atoi(argv[++i]);

			if (strcmp(who, "x") == 0)
				mask_test_who[nb_mask_tests] = 1;
			else if (strcmp(who, "y") == 0)
				mask_test_who[nb_mask_tests] = 2;
			else if (strcmp(who, "x+y") == 0)
				mask_test_who[nb_mask_tests] = 3;
			else if (strcmp(who, "s") == 0)
				mask_test_who[nb_mask_tests] = 4;
			else {
				cout << "masktest: unknown 'who' option: " << who << endl;
				cout << "must be one of 'x', 'y', 'x+y' or 's'" << endl;
				exit(1);
				}
			if (strcmp(what, "eq") == 0)
				mask_test_what[nb_mask_tests] = 1;
			else if (strcmp(what, "ge") == 0)
				mask_test_what[nb_mask_tests] = 2;
			else if (strcmp(what, "le") == 0)
				mask_test_what[nb_mask_tests] = 3;
			else {
				cout << "masktest: unknown 'what' option: " << who << endl;
				cout << "must be one of 'eq', 'ge' or 'le'" << endl;
				exit(1);
				}
			cout << "-masktest "
				<< mask_test_level[nb_mask_tests] << " "
				<< mask_test_who[nb_mask_tests] << " "
				<< mask_test_what[nb_mask_tests] << " "
				<< mask_test_value[nb_mask_tests] << endl;
			nb_mask_tests++;
			cout << "nb_mask_tests=" << nb_mask_tests << endl;
		}
		else if (strcmp(argv[i], "-singletons") == 0) {
			f_singletons = TRUE;
			cout << "-singletons" << endl;
		}
	}
	if (!f_K) {
		cout << "please use -K <K> to specify K" << endl;
		exit(1);
	}
	if (q1 == 1) {
		Xsize = d1;
		Ysize = d2;
	} else {
		Xsize = q1; // = D = q1 = # of rows
		Ysize = q2; // = C = q2 = # of cols
	}

	V = Xsize * Ysize;

	cout << "depth=" << depth << endl;
	cout << "V=" << V << endl;
	cout << "K=" << K << endl;
	cout << "Xsize=" << Xsize << endl;
	cout << "Ysize=" << Ysize << endl;
	cout << "V=" << V << endl;

	line = NEW_INT(K);
	row_sum = NEW_INT(Xsize);
	col_sum = NEW_INT(Ysize);
	live_points = NEW_INT(V);


	cout << "DELANDTSHEER_DOYEN_X=" << DELANDTSHEER_DOYEN_X << endl;
	cout << "DELANDTSHEER_DOYEN_Y=" << DELANDTSHEER_DOYEN_Y << endl;

	INT_vec_zero(row_sum, Xsize);
	INT_vec_zero(col_sum, Ysize);


	M1 = NEW_OBJECT(matrix_group);
	M2 = NEW_OBJECT(matrix_group);

	F1 = NEW_OBJECT(finite_field);
	F2 = NEW_OBJECT(finite_field);



	if (q1 == 1) {

		F1->init(2, 0);
		F2->init(2, 0);

		cout << "initializing projective groups:" << endl;

		A1->init_projective_group(d1, F1,
				FALSE /* f_semilinear */, TRUE /* f_basis */,
				verbose_level - 1);
		M1 = A1->G.matrix_grp;

		A2->init_projective_group(d2, F2,
				FALSE /* f_semilinear */, TRUE /* f_basis */,
				verbose_level - 1);
		M2 = A1->G.matrix_grp;

#if 0
		M1->init_projective_group(d1, F1,
				FALSE /* f_semilinear */, A1, verbose_level);

		M2->init_projective_group(d2, F2,
				FALSE /* f_semilinear */, A2, verbose_level);
#endif
		b = 0;

	} else {



		b = (V * (V - 1)) / (K * (K - 1));

		if (b * (K * (K - 1)) != (V * (V - 1))) {
			cout << "integrality conditions violated" << endl;
			exit(1);
		}

		cout << "b=" << b << endl;



		F1->init(q1, 0);
		F2->init(q2, 0);



		cout << "initializing affine groups:" << endl;

		M1->init_affine_group(d1, F1,
				FALSE /* f_semilinear */, A1, verbose_level);

		M2->init_affine_group(d2, F2,
				FALSE /* f_semilinear */, A2, verbose_level);
	}

	cout << "direct_product_action::init before "
			"A->init_direct_product_group_and_restrict" << endl;

	A->init_direct_product_group_and_restrict(M1, M2,
			verbose_level);

	cout << "direct_product_action::init after "
			"A->init_direct_product_group_and_restrict" << endl;

	if (!A->f_has_subaction) {
		cout << "direct_product_action::init action "
				"A does not have a subaction" << endl;
		exit(1);
	}
	A0 = A->subaction;

	P = A0->G.direct_product_group;

	//vector_space_dimension = W->dimension_of_tensor_action;



	if (q1 == 1) {

		strong_generators *SG1;
		strong_generators *SG2;
		strong_generators *SG3;

		SG1 = NEW_OBJECT(strong_generators);
		SG2 = NEW_OBJECT(strong_generators);

		if (f_v) {
			cout << "before generators_for_the_monomial_group "
					"action" << A1->label << endl;
		}
		SG1->generators_for_the_monomial_group(A1,
			M1, verbose_level);
		if (f_v) {
			cout << "after generators_for_the_monomial_group "
					"action" << A1->label << endl;
		}


		if (f_v) {
			cout << "before generators_for_the_monomial_group "
					"action" << A2->label << endl;
		}
		SG2->generators_for_the_monomial_group(A2,
			M2, verbose_level);
		if (f_v) {
			cout << "after generators_for_the_monomial_group "
					"action" << A2->label << endl;
		}

		if (f_v) {
			cout << "direct_product_action::init "
					"before lift_generators" << endl;
		}
		P->lift_generators(
				SG1,
				SG2,
				A0, SG3,
				verbose_level);
		if (f_v) {
			cout << "direct_product_action::init "
					"after lift_generators" << endl;
		}

		SG = SG3;
		SG->group_order(go);

		cout << "The group has order " << go << endl;

		action *Ar;
		INT *points;
		INT nb_points;
		INT h;

		nb_points = d1 * d2;
		points = NEW_INT(nb_points);
		h = 0;
		for (i = 0; i < d1; i++) {
			for (j = 0; j < d2; j++) {
				a = i * A2->degree + j;
				points[h++] = a;
			}
		} // next i


		Ar = A->restricted_action(points, nb_points,
				verbose_level);

		A = Ar;
	} // if (q1 == 1)

	else {
		if (!A0->f_has_strong_generators) {
			cout << "direct_product_action::init action A0 does not "
					"have strong generators" << endl;
			exit(1);
			}

		SG = A0->Strong_gens;
		SG->group_order(go);

		cout << "The group " << A->label << " has order " << go
				<< " and permutation degree " << A->degree << endl;
	}


	cout << "Generators are:" << endl;
	for (i = 0; i < SG->gens->len; i++) {
		cout << "generator " << i << " / "
				<< SG->gens->len << " is: " << endl;
		A->element_print_quick(SG->gens->ith(i), cout);
		cout << "as permutation: " << endl;
		A->element_print_as_permutation_with_offset(
				SG->gens->ith(i), cout,
				0 /* offset*/,
				TRUE /* f_do_it_anyway_even_for_big_degree*/,
				TRUE /* f_print_cycles_of_length_one*/,
				0 /* verbose_level*/);
		//A->element_print_as_permutation(SG->gens->ith(i), cout);
		cout << endl;
		}
	cout << "Generators are:" << endl;
	for (i = 0; i < SG->gens->len; i++) {
		A->element_print_as_permutation(SG->gens->ith(i), cout);
		cout << endl;
		}
	cout << "Generators in GAP format are:" << endl;
	cout << "G := Group([";
	for (i = 0; i < SG->gens->len; i++) {
		A->element_print_as_permutation_with_offset(
				SG->gens->ith(i), cout,
				1 /*offset*/,
				TRUE /* f_do_it_anyway_even_for_big_degree */,
				FALSE /* f_print_cycles_of_length_one */,
				0 /* verbose_level*/);
		if (i < SG->gens->len - 1) {
			cout << ", " << endl;
		}
	}
	cout << "]);" << endl;
	cout << "Generators in compact permutation form are:" << endl;
	cout << SG->gens->len << " " << A->degree << endl;
	for (i = 0; i < SG->gens->len; i++) {
		for (j = 0; j < A->degree; j++) {
			a = A->element_image_of(j,
					SG->gens->ith(i), 0 /* verbose_level */);
			cout << a << " ";
			}
		cout << endl;
		}
	cout << "-1" << endl;





	strong_generators *Strong_gens;

	if (f_subgroup) {
		Strong_gens = NEW_OBJECT(strong_generators);
		INT *data;
		INT sz;
		INT nb_gens;

		INT_vec_scan(subgroup_gens_text, data, sz);
		nb_gens = sz / A->make_element_size;
		if (f_v) {
			cout << "before Strong_gens->init_from_data_"
					"with_target_go_ascii" << endl;
			}
		cout << "nb_gens=" << nb_gens << endl;
		Strong_gens->init_from_data_with_target_go_ascii(A0,
				data,
				nb_gens, A0->make_element_size,
				subgroup_order_text,
				verbose_level + 2);
		if (f_v) {
			cout << "action::init_direct_product_group "
					"after Strong_gens->init_from_data_"
					"with_target_go_ascii" << endl;
			}
		Pairs = NEW_OBJECT(generator);

		Pairs->read_arguments(argc, argv, 0);

		//Pairs->prefix[0] = 0;
		sprintf(Pairs->fname_base, "pairs_%s_%ld_%ld",
				group_label, q1, q2);


		Pairs->depth = 2;

		if (f_v) {
			cout << "direct_product_action::init "
					"before Pairs->init" << endl;
			}
		Pairs->init(A0, A, Strong_gens,
				Pairs->depth /* sz */, verbose_level);
		if (f_v) {
			cout << "direct_product_action::init "
					"after Pairs->init" << endl;
			}


		INT nb_nodes = 1000;

		if (f_v) {
			cout << "direct_product_action::init "
					"before Pairs->init_poset_orbit_node" << endl;
			}
		Pairs->init_poset_orbit_node(nb_nodes, verbose_level - 1);
		if (f_v) {
			cout << "direct_product_action::init "
					"calling Pairs->init_root_node" << endl;
			}
		Pairs->root[0].init_root_node(Pairs, verbose_level - 1);

		INT f_use_invariant_subset_if_available;
		INT f_debug;

		f_use_invariant_subset_if_available = TRUE;
		f_debug = FALSE;

		INT t0 = os_ticks();

		if (f_v) {
			cout << "direct_product_action::init "
					"before Pairs->main" << endl;
			cout << "A=";
			A->print_info();
			cout << "A0=";
			A0->print_info();
			}


		//Pairs->f_allowed_to_show_group_elements = TRUE;

		Pairs->f_max_depth = FALSE;
		Pairs->depth = 2;
		Pairs->main(t0,
			Pairs->depth /* schreier_depth */,
			f_use_invariant_subset_if_available,
			f_debug,
			verbose_level);

		if (f_v) {
			cout << "direct_product_action::init "
					"after Pairs->main" << endl;
		}


		nb_orbits = Pairs->nb_orbits_at_level(2);

		if (f_v) {
			cout << "direct_product_action::init "
					"nb_orbits = "
					<< nb_orbits << endl;
		}

		transporter = NEW_INT(A0->elt_size_in_INT);
		tmp_Elt = NEW_INT(A0->elt_size_in_INT);

		orbit_length = NEW_INT(nb_orbits);
		orbit_covered = NEW_INT(nb_orbits);
		orbit_covered_max = NEW_INT(nb_orbits);
		orbits_covered = NEW_INT(K * K);

		INT_vec_zero(orbit_covered, nb_orbits);

		for (i = 0; i < nb_orbits; i++) {
			orbit_length[i] = Pairs->orbit_length_as_INT(
					i /* orbit_at_level*/, 2 /* level*/);
			orbit_covered_max[i] = orbit_length[i] / b;
			if (orbit_covered_max[i] * b != orbit_length[i]) {
				cout << "integrality conditions violated (2)" << endl;
				exit(1);
			}
		}
		cout << "i : orbit_length[i] : orbit_covered_max[i]" << endl;
		for (i = 0; i < nb_orbits; i++) {
			cout << i << " : " << orbit_length[i]
				<< " : " << orbit_covered_max[i] << endl;
			}

		compute_pair_orbit_table(verbose_level);
		//write_pair_orbit_file(verbose_level);

	} else {
		cout << "We don't have -subgroup, so orbits on pairs "
				"are not computed" << endl;
		//exit(1);
	}



	f_row_used = NEW_INT(Xsize);
	f_col_used = NEW_INT(Ysize);
	row_idx = NEW_INT(Xsize);
	col_idx = NEW_INT(Ysize);
	singletons = NEW_INT(K);

	// temporary data
	row_col_idx = NEW_INT(Xsize);
	col_row_idx = NEW_INT(Ysize);


	if (f_singletons) {
		cout << "searching singletons" << endl;

		INT target_depth;
		target_depth = K - depth;
		cout << "target_depth=" << target_depth << endl;

		orbiter_data_file *ODF;
		BYTE fname[1000];
		//INT i, j;
		INT level = depth;

		sprintf(fname, "design_%s_%ld_%ld_lvl_%ld", group_label, q1, q2, level);

		ODF = NEW_OBJECT(orbiter_data_file);
		ODF->load(fname, verbose_level);
		cout << "found " << ODF->nb_cases
				<< " orbits at level " << level << endl;

		INT *Orbit_idx;
		INT nb_orbits_not_ruled_out;
		INT orbit_idx;
		INT nb_cases = 0;
		INT nb_cases_eliminated = 0;
		INT f_vv;

		Orbit_idx = NEW_INT(ODF->nb_cases);
		nb_orbits_not_ruled_out = 0;

		for (orbit_idx = 0; orbit_idx < ODF->nb_cases; orbit_idx++) {

#if 0
			if (f_split) {
				if ((orbit_idx % split_m) == split_r) {
					continue;
				}
			}
#endif

			if ((orbit_idx % 100)== 0) {
				f_vv = TRUE;
			} else {
				f_vv = FALSE;
			}
			if (f_vv) {
				cout << orbit_idx << " / " << ODF->nb_cases << " : ";
				INT_vec_print(cout, ODF->sets[orbit_idx],
						ODF->set_sizes[orbit_idx]);
				cout << " : " << ODF->Ago_ascii[orbit_idx] << " : "
						<< ODF->Aut_ascii[orbit_idx] << endl;
			}

			INT *line0;

			line0 = ODF->sets[orbit_idx];
			if (ODF->set_sizes[orbit_idx] != level) {
				cout << "ODF->set_sizes[orbit_idx] != level" << endl;
				exit(1);
			}

			create_graph(
					line0, level, 0 /*verbose_level*/);

			if (f_vv) {
				cout << "case " << orbit_idx << " / " << ODF->nb_cases
						<< " we found " << nb_live_points << " live points" << endl;
			}
			if (nb_live_points < target_depth) {
				if (f_vv) {
					cout << "eliminated!" << endl;
				}
				nb_cases_eliminated++;
			} else {
				Orbit_idx[nb_orbits_not_ruled_out++] = orbit_idx;
				nb_cases++;
			}
			if (f_vv) {
				cout << "nb_cases=" << nb_cases << " vs ";
				cout << "nb_cases_eliminated=" << nb_cases_eliminated << endl;
			}
		} // orbit_idx
		cout << "nb_cases=" << nb_cases << endl;
		cout << "nb_cases_eliminated=" << nb_cases_eliminated << endl;

		INT orbit_not_ruled_out;
		INT nb_sol = 0;

		for (orbit_not_ruled_out = 0;
				orbit_not_ruled_out < nb_orbits_not_ruled_out;
				orbit_not_ruled_out++) {
			orbit_idx = Orbit_idx[orbit_not_ruled_out];


			if ((orbit_not_ruled_out % 100)== 0) {
				f_vv = TRUE;
			} else {
				f_vv = FALSE;
			}


			if (f_vv) {
				cout << "orbit_not_ruled_out=" << orbit_not_ruled_out
						<< " / " << nb_orbits_not_ruled_out
						<< " is orbit_idx " << orbit_idx << endl;
			}

			INT *line0;

			line0 = ODF->sets[orbit_idx];
			if (ODF->set_sizes[orbit_idx] != level) {
				cout << "ODF->set_sizes[orbit_idx] != level" << endl;
				exit(1);
			}

			create_graph(
					line0, level, 0 /*verbose_level*/);

			if (f_vv) {
				cout << "orbit_not_ruled_out=" << orbit_not_ruled_out << " / "
						<< nb_orbits_not_ruled_out << " is orbit_idx"
						<< orbit_idx << " / " << ODF->nb_cases
						<< " we found " << nb_live_points
						<< " live points" << endl;
			}
			if (nb_live_points == target_depth) {
				INT_vec_copy(line0, line, level);
				INT_vec_copy(live_points, line + level, target_depth);
				if (check_orbit_covering(line,
					K, 0 /* verbose_level */)) {
					cout << "found a solution in orbit " << orbit_idx << endl;
					nb_sol++;
				}



			} else {
				cout << "orbit_not_ruled_out=" << orbit_not_ruled_out << " / "
						<< nb_orbits_not_ruled_out << " is orbit_idx"
						<< orbit_idx << " / " << ODF->nb_cases
						<< " we found " << nb_live_points
						<< " live points, doing a search" << endl;
				INT *subset;
				INT nCk, l;

				subset = NEW_INT(target_depth);
				nCk = INT_n_choose_k(nb_live_points, target_depth);
				for (l = 0; l < nCk; l++) {
					unrank_k_subset(l, subset, nb_live_points, target_depth);
					INT_vec_copy(line0, line, level);
					INT_vec_apply(subset, live_points, line + level, target_depth);
					if (check_orbit_covering(line,
						K, 0 /* verbose_level */)) {
						cout << "found a solution, subset " << l
								<< " / " << nCk << " in orbit "
								<< orbit_idx << endl;
						nb_sol++;
					}
				} // next l

				FREE_INT(subset);
			} // else
		} // next orbit_not_ruled_out

		cout << "nb_sol=" << nb_sol << endl;
		cout << "searching singletons done" << endl;
	} else {


		Gen = NEW_OBJECT(generator);

		Gen->read_arguments(argc, argv, 0);

		//Gen->prefix[0] = 0;

		if (f_subgroup) {
			sprintf(Gen->fname_base, "design_%s_%ld_%ld",
					group_label, q1, q2);
		}
		else {
			sprintf(Gen->fname_base, "design_no_group_%ld_%ld",
					d1, d2);

		}


		Gen->depth = depth;

		if (f_v) {
			cout << "direct_product_action::init "
					"before Gen->init" << endl;
			}
		Gen->init(A0, A, SG,
				Gen->depth /* sz */, verbose_level);
		if (f_v) {
			cout << "direct_product_action::init "
					"after Gen->init" << endl;
			}
		Gen->init_check_func(::design_search_check_conditions,
			(void *)this /* candidate_check_data */);


		INT nb_nodes = 1000;

		if (f_v) {
			cout << "direct_product_action::init "
					"before Gen->init_poset_orbit_node" << endl;
			}
		Gen->init_poset_orbit_node(nb_nodes, verbose_level - 1);
		if (f_v) {
			cout << "direct_product_action::init "
					"calling Pairs->init_root_node" << endl;
			}
		Gen->root[0].init_root_node(Gen, verbose_level - 1);

		INT f_use_invariant_subset_if_available = TRUE;
		INT f_debug = FALSE;

		//t0 = os_ticks();

		if (f_v) {
			cout << "direct_product_action::init "
					"before Gen->main" << endl;
			cout << "A=";
			A->print_info();
			cout << "A0=";
			A0->print_info();
			}


		//Gen->f_allowed_to_show_group_elements = TRUE;

		Gen->f_max_depth = FALSE;
		Gen->depth = depth;
		Gen->main(t0,
			Gen->depth /* schreier_depth */,
			f_use_invariant_subset_if_available,
			f_debug,
			verbose_level + 10);

		if (f_v) {
			cout << "direct_product_action::init "
					"after Gen->main" << endl;
		}


		if (f_v) {
			cout << "direct_product_action::init "
					"before Gen->draw_poset" << endl;
		}
		Gen->draw_poset(Gen->fname_base, depth,
				0 /* data1 */, TRUE /* f_embedded */, TRUE /* f_sideways */,
				verbose_level);
	} // else


	if (f_v) {
		cout << "direct_product_action::init done" << endl;
	}
}

void direct_product_action::create_graph(
		INT *line0, INT len, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, a, x, y, h, ph, k, pk, o;

	if (f_v) {
		cout << "direct_product_action::create_graph" << endl;
	}

	INT_vec_zero(row_sum, Xsize);
	INT_vec_zero(col_sum, Ysize);

	for (i = 0; i < len; i++) {
		a = line0[i];
		x = a / Ysize;
		y = a % Ysize;
		//cout << "i=" << i << " / " << len << " a=" << a
		//	<< " x=" << x << " y=" << y << endl;
		row_sum[x]++;
		col_sum[y]++;
	}

	if (!check_orbit_covering(line0,
		len, 0 /* verbose_level */)) {
		cout << "line0 is not good" << endl;
		exit(1);
	}

	nb_live_points = 0;
	for (x = 0; x < Xsize; x++) {
		if (row_sum[x]) {
			continue;
		}
		for (y = 0; y < Ysize; y++) {
			if (col_sum[y]) {
				continue;
			}
			a = x * Ysize + y;
			//cout << "testing point a=" << a << endl;
			for (h = 0; h < len; h++) {

				ph = line0[h];
				o = find_pair_orbit(ph, a, 0 /*verbose_level - 1*/);
				orbit_covered[o]++;
				if (orbit_covered[o] > orbit_covered_max[o]) {
					for (k = h; k >= 0; k--) {
						pk = line0[k];
						o = find_pair_orbit(pk, a, 0 /*verbose_level - 1*/);
						orbit_covered[o]--;
					}
					break;
				}
			} // next h
			if (h == len) {
				live_points[nb_live_points++] = a;
			}
		} // next y
	} // next x
	if (f_v) {
		cout << "found " << nb_live_points << " live points" << endl;
	}

	if (f_v) {
		cout << "direct_product_action::create_graph done" << endl;
	}
}

void direct_product_action::testing(
		strong_generators *SG, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;

	if (f_v) {
		cout << "direct_product_action::testing" << endl;
	}


	cout << "testing..." << endl;
	INT r1, r2;
	INT *Elt1;
	INT *Elt2;
	INT *Elt3;
	INT *Elt4;
	INT *perm1;
	INT *perm2;
	INT *perm3;
	INT *perm4;
	INT *perm5;
	INT cnt;

	Elt1 = NEW_INT(A->elt_size_in_INT);
	Elt2 = NEW_INT(A->elt_size_in_INT);
	Elt3 = NEW_INT(A->elt_size_in_INT);
	Elt4 = NEW_INT(A->elt_size_in_INT);
	perm1 = NEW_INT(A->degree);
	perm2 = NEW_INT(A->degree);
	perm3 = NEW_INT(A->degree);
	perm4 = NEW_INT(A->degree);
	perm5 = NEW_INT(A->degree);

	for (cnt = 0; cnt < 10; cnt++) {
		r1 = random_integer(SG->gens->len);
		r2 = random_integer(SG->gens->len);
		cout << "r1=" << r1 << endl;
		cout << "r2=" << r2 << endl;
		A->element_move(SG->gens->ith(r1), Elt1, 0);
		A->element_move(SG->gens->ith(r2), Elt2, 0);
		cout << "Elt1 = " << endl;
		A->element_print_quick(Elt1, cout);
		A->element_as_permutation(Elt1, perm1, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm1, A->degree);
		cout << endl;

		cout << "Elt2 = " << endl;
		A->element_print_quick(Elt2, cout);
		A->element_as_permutation(Elt2, perm2, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm2, A->degree);
		cout << endl;

		A->element_mult(Elt1, Elt2, Elt3, 0);
		cout << "Elt3 = " << endl;
		A->element_print_quick(Elt3, cout);
		A->element_as_permutation(Elt3, perm3, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm3, A->degree);
		cout << endl;

		perm_mult(perm1, perm2, perm4, A->degree);
		cout << "perm1 * perm2= " << endl;
		perm_print(cout, perm4, A->degree);
		cout << endl;

		for (i = 0; i < A->degree; i++) {
			if (perm3[i] != perm4[i]) {
				cout << "test " << cnt
						<< " failed; something is wrong" << endl;
				exit(1);
			}
		}
	}
	cout << "test 1 passed" << endl;


	for (cnt = 0; cnt < 10; cnt++) {
		r1 = random_integer(SG->gens->len);
		cout << "r1=" << r1 << endl;
		A->element_move(SG->gens->ith(r1), Elt1, 0);
		cout << "Elt1 = " << endl;
		A->element_print_quick(Elt1, cout);
		A->element_as_permutation(Elt1, perm1, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm1, A->degree);
		cout << endl;

		A->element_invert(Elt1, Elt2, 0);
		cout << "Elt2 = " << endl;
		A->element_print_quick(Elt2, cout);
		A->element_as_permutation(Elt2, perm2, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm2, A->degree);
		cout << endl;

		A->element_mult(Elt1, Elt2, Elt3, 0);
		cout << "Elt3 = " << endl;
		A->element_print_quick(Elt3, cout);
		A->element_as_permutation(Elt3, perm3, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm3, A->degree);
		cout << endl;

		if (!perm_is_identity(perm3, A->degree)) {
			cout << "fails the inverse test" << endl;
			exit(1);
		}
	}

	cout << "test 2 passed" << endl;



	for (cnt = 0; cnt < 10; cnt++) {
		r1 = random_integer(SG->gens->len);
		r2 = random_integer(SG->gens->len);
		cout << "r1=" << r1 << endl;
		cout << "r2=" << r2 << endl;
		A->element_move(SG->gens->ith(r1), Elt1, 0);
		A->element_move(SG->gens->ith(r2), Elt2, 0);
		cout << "Elt1 = " << endl;
		A->element_print_quick(Elt1, cout);
		A->element_as_permutation(Elt1, perm1, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm1, A->degree);
		cout << endl;

		cout << "Elt2 = " << endl;
		A->element_print_quick(Elt2, cout);
		A->element_as_permutation(Elt2, perm2, 0 /* verbose_level */);
		cout << "as permutation: " << endl;
		perm_print(cout, perm2, A->degree);
		cout << endl;

		A->element_mult(Elt1, Elt2, Elt3, 0);
		cout << "Elt3 = " << endl;
		A->element_print_quick(Elt3, cout);

		A->element_invert(Elt3, Elt4, 0);
		cout << "Elt4 = Elt3^-1 = " << endl;
		A->element_print_quick(Elt4, cout);


		A->element_as_permutation(Elt3, perm3, 0 /* verbose_level */);
		cout << "as Elt3 as permutation: " << endl;
		perm_print(cout, perm3, A->degree);
		cout << endl;

		A->element_as_permutation(Elt4, perm4, 0 /* verbose_level */);
		cout << "as Elt4 as permutation: " << endl;
		perm_print(cout, perm4, A->degree);
		cout << endl;

		perm_mult(perm3, perm4, perm5, A->degree);
		cout << "perm3 * perm4= " << endl;
		perm_print(cout, perm5, A->degree);
		cout << endl;

		for (i = 0; i < A->degree; i++) {
			if (perm5[i] != i) {
				cout << "test " << cnt
						<< " failed; something is wrong" << endl;
				exit(1);
			}
		}
	}
	cout << "test 3 passed" << endl;


#if 0
	cout << "performing test 4:" << endl;

	INT data[] = {2,0,1, 0,1,1,0, 1,0,0,1, 1,0,0,1 };
	A->make_element(Elt1, data, verbose_level);
	A->element_as_permutation(Elt1, perm1, 0 /* verbose_level */);
	cout << "as Elt1 as permutation: " << endl;
	perm_print(cout, perm1, A->degree);
	cout << endl;

	A->element_invert(Elt1, Elt2, 0);
	A->element_as_permutation(Elt2, perm2, 0 /* verbose_level */);
	cout << "as Elt2 as permutation: " << endl;
	perm_print(cout, perm2, A->degree);
	cout << endl;


	A->element_mult(Elt1, Elt2, Elt3, 0);
	cout << "Elt3 = " << endl;
	A->element_print_quick(Elt3, cout);

	perm_mult(perm1, perm2, perm3, A->degree);
	cout << "perm1 * perm2= " << endl;
	perm_print(cout, perm3, A->degree);
	cout << endl;

	for (i = 0; i < A->degree; i++) {
		if (perm3[i] != i) {
			cout << "test 4 failed; something is wrong" << endl;
			exit(1);
		}
	}

	cout << "test 4 passed" << endl;
#endif

	FREE_INT(Elt1);
	FREE_INT(Elt2);
	FREE_INT(Elt3);
	FREE_INT(Elt4);
	FREE_INT(perm1);
	FREE_INT(perm2);
	FREE_INT(perm3);
	FREE_INT(perm4);
	FREE_INT(perm5);

	if (f_v) {
		cout << "direct_product_action::testing done" << endl;
	}
}

INT direct_product_action::find_pair_orbit(INT i, INT j, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT orbit_no;

	if (f_v) {
		cout << "direct_product_action::find_pair_orbit" << endl;
	}
	if (i == j) {
		cout << "direct_product_action::find_pair_orbit i = j = " << j << endl;
		exit(1);
		}
	orbit_no = pair_orbit[i * V + j];
	if (f_v) {
		cout << "direct_product_action::find_pair_orbit done" << endl;
	}
	return orbit_no;
}

INT direct_product_action::find_pair_orbit_by_tracing(INT i, INT j, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT orbit_no;
	INT set[2];
	INT canonical_set[2];

	if (f_v) {
		cout << "direct_product_action::find_pair_orbit_by_tracing" << endl;
	}
	if (i == j) {
		cout << "direct_product_action::find_pair_orbit_by_tracing i = j = " << j << endl;
		exit(1);
		}
	set[0] = i;
	set[1] = j;
	orbit_no = Pairs->trace_set(set, 2, 2,
		canonical_set, transporter,
		verbose_level - 1);
	if (f_v) {
		cout << "direct_product_action::find_pair_orbit_by_tracing done" << endl;
	}
	return orbit_no;
}

void direct_product_action::compute_pair_orbit_table(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j, k;


	if (f_v) {
		cout << "direct_product_action::compute_pair_orbit_table" << endl;
	}
	pair_orbit = NEW_INT(V * V);
	INT_vec_zero(pair_orbit, V * V);
	for (i = 0; i < V; i++) {
		for (j = i + 1; j < V; j++) {
			k = find_pair_orbit_by_tracing(i, j, 0 /*verbose_level - 2*/);
			pair_orbit[i * V + j] = k;
			pair_orbit[j * V + i] = k;
		}
		if ((i % 100) == 0) {
			cout << "i=" << i << endl;
		}
	}
	if (f_v) {
		cout << "direct_product_action::compute_pair_orbit_table done" << endl;
	}
}

void direct_product_action::write_pair_orbit_file(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	BYTE fname[1000];
	INT set[1000];
	INT i, j, k, n, size, l;


	if (f_v) {
		cout << "direct_product_action::write_pair_orbit_file" << endl;
	}
	sprintf(fname, "%s.2orbits", group_label);
	cout << "writing pair-orbit file " << fname << endl;
	{
	ofstream f(fname);
	f << nb_orbits << endl;
	for (i = 0; i < nb_orbits; i++) {
		n = Pairs->first_poset_orbit_node_at_level[2] + i;
		Pairs->get_set(n, set, size);
		if (size != 2) {
			cout << "direct_product_action::write_pair_orbit_file "
					"size != 2" << endl;
			exit(1);
			}
		l = Pairs->orbit_length_as_INT(i, 2);
		f << set[0] << " " << set[1] << " " << l << endl;
		}
	for (i = 0; i < V; i++) {
		for (j = i + 1; j < V; j++) {
			k = find_pair_orbit(i, j, 0 /*verbose_level - 2*/);
			f << k << " ";
			}
		f << endl;
		if ((i % 100) == 0) {
			cout << "i=" << i << endl;
			}
		}
	}
	cout << "written file " << fname << " of size "
			<< file_size(fname) << endl;
	if (f_v) {
		cout << "direct_product_action::write_pair_orbit_file done" << endl;
	}

}

void direct_product_action::print_mask_test_i(ostream &ost, INT i)
{
	INT who, what;

	ost << "mask test at level " << mask_test_level[i] << " : ";
	who = mask_test_who[i];
	what = mask_test_what[i];
	if (who == 1) {
		ost << "x ";
		}
	else if (who == 2) {
		ost << "y ";
		}
	else if (who == 3) {
		ost << "x+y ";
		}
	else if (who == 4) {
		ost << "s ";
		}
	if (what == 1) {
		ost << "= ";
		}
	else if (what == 2) {
		ost << ">= ";
		}
	else if (what == 3) {
		ost << "<= ";
		}
	ost << mask_test_value[i];
	ost << endl;
}

INT direct_product_action::check_conditions(
		INT *S, INT len, INT verbose_level)
{
	//verbose_level = 4;
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT f_OK = TRUE;
	INT f_bad_orbit = FALSE;
	INT f_bad_row = FALSE;
	INT f_bad_col = FALSE;
	INT f_bad_mask = FALSE;
	INT pt, idx;

	if (f_v) {
		cout << "direct_product_action::check_conditions "
				"checking set ";
		print_set(cout, len, S);
		cout << endl;
		//cout << "offset=" << offset << endl;
		}

	pt = S[len - 1];
	if (INT_vec_search_linear(S, len - 1, pt, idx)) {
		if (f_v) {
			cout << "direct_product_action::check_conditions not OK, "
					"repeat entry" << endl;
		}
		return FALSE;
	}
	if (f_subgroup) {
		if (!check_orbit_covering(S, len, verbose_level)) {
			f_bad_orbit = TRUE;
			f_OK = FALSE;
		}
	}

	if (f_OK && !check_row_sums(S, len, verbose_level)) {
		f_bad_row = TRUE;
		f_OK = FALSE;
		}
	if (f_OK && !check_col_sums(S, len, verbose_level)) {
		f_bad_col = TRUE;
		f_OK = FALSE;
		}
	if (f_OK && !check_mask(S, len, verbose_level)) {
		f_bad_mask = TRUE;
		f_OK = FALSE;
		}
	if (f_OK) {
		if (f_v) {
			cout << "OK" << endl;
			}
		return TRUE;
		}
	else {
		if (f_v) {
			cout << "not OK" << endl;
			}
		if (f_vv) {
			cout << "because of ";
			if (f_bad_orbit)
				cout << "orbit covering";
			else if (f_bad_row)
				cout << "row-test";
			else if (f_bad_col)
				cout << "col-test";
			else if (f_bad_mask)
				cout << "mask";
			cout << endl;
			}
		return FALSE;
		}
}

INT direct_product_action::check_orbit_covering(INT *line,
		INT len, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT f_vv = (verbose_level >= 2);
	INT i, pi, j, pj, o, f_OK = TRUE;

	INT_vec_zero(orbit_covered, nb_orbits);

	for (i = 0; i < len; i++) {
		pi = line[i];
		for (j = i + 1; j < len; j++) {
			pj = line[j];
			o = find_pair_orbit(pi, pj, 0 /*verbose_level - 1*/);
			//o = orbits_on_pairs[pi * V + pj];
			if (pi == pj) {
				cout << "direct_product_action::check_orbit_covering "
						"pi = " << pi << " == pj = " << pj << endl;
				exit(1);
				}
			orbit_covered[o]++;
			if (orbit_covered[o] > orbit_covered_max[o]) {
				f_OK = FALSE;
				break;
				}
			}
		if (!f_OK)
			break;
		}
	if (f_v) {
		if (!f_OK) {
			cout << "orbit condition violated" << endl;
#if 0
			if (f_vv) {
				print_orbit_covered(cout);
				print_orbit_covered_max(cout);
				get_orbit_covering_matrix(line, len, verbose_level - 1);
				print_orbit_covering_matrix(len);
				}
#endif
			}
		}
	return f_OK;
}

INT direct_product_action::check_row_sums(INT *line,
		INT len, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, p, x, y, s, f_OK = TRUE;
	INT f_DD_problem = FALSE;

	inner_pairs_in_rows = 0;
	INT_vec_zero(row_sum, Xsize);
	if (f_R) {
		for (i = 1; i <= nb_row_types; i++) {
			row_type_cur[i] = 0;
			}
		}
	for (i = 0; i < len; i++) {
		p = line[i];
		x = p / Ysize;
		y = p % Ysize;
		inner_pairs_in_rows += row_sum[x];
		row_sum[x]++;
		if (DELANDTSHEER_DOYEN_X != -1) {
			if (inner_pairs_in_rows > DELANDTSHEER_DOYEN_X) {
				f_OK = FALSE;
				f_DD_problem = TRUE;
				break;
				}
			}
		if (f_R) {
			s = row_sum[x];
			if (s > nb_row_types) {
				f_OK = FALSE;
				break;
				}
			if (row_type_cur[s] >= row_type_this_or_bigger[s]) {
				f_OK = FALSE;
				break;
				}
			if (s > 1) {
				row_type_cur[s - 1]--;
				}
			row_type_cur[s]++;
			}
		}
	if (f_v) {
		if (!f_OK) {
			cout << "direct_product_action::check_row_sums "
					"row condition violated" << endl;
			if (f_vv) {
				if (f_DD_problem) {
					cout << "direct_product_action::check_row_sums "
							"inner_pairs_in_rows = "
						<< inner_pairs_in_rows
						<< " > DELANDTSHEER_DOYEN_X = "
						<< DELANDTSHEER_DOYEN_X
						<< ", not OK" << endl;
					}
				else {
					cout << "direct_product_action::check_row_sums"
							"problem with row-type:" << endl;
					for (i = 1; i <= nb_row_types; i++) {
						cout << row_type_cur[i] << " ";
						}
					cout << endl;
					for (i = 1; i <= nb_row_types; i++) {
						cout << row_type_this_or_bigger[i] << " ";
						}
					cout << endl;
					}
				}
			}
		}
	return f_OK;
}

INT direct_product_action::check_col_sums(INT *line,
		INT len, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, p, x, y, s, f_OK = TRUE;
	INT f_DD_problem = FALSE;

	inner_pairs_in_cols = 0;
	INT_vec_zero(col_sum, Ysize);
	if (f_C) {
		for (i = 1; i <= nb_col_types; i++) {
			col_type_cur[i] = 0;
			}
		}
	for (i = 0; i < len; i++) {
		p = line[i];
		x = p / Ysize;
		y = p % Ysize;
		inner_pairs_in_cols += col_sum[y];
		col_sum[y]++;
		if (DELANDTSHEER_DOYEN_Y != -1) {
			if (inner_pairs_in_cols > DELANDTSHEER_DOYEN_Y) {
				f_OK = FALSE;
				f_DD_problem = TRUE;
				break;
				}
			}
		if (f_C) {
			s = col_sum[y];
			if (s > nb_col_types) {
				f_OK = FALSE;
				break;
				}
			if (col_type_cur[s] >= col_type_this_or_bigger[s]) {
				f_OK = FALSE;
				break;
				}
			if (s > 1) {
				col_type_cur[s - 1]--;
				}
			col_type_cur[s]++;
			}
		}
	if (f_v) {
		if (!f_OK) {
			cout << "direct_product_action::check_col_sums "
					"col condition violated" << endl;
			if (f_vv) {
				if (f_DD_problem) {
					cout << "direct_product_action::check_col_sums "
							"inner_pairs_in_cols = "
						<< inner_pairs_in_cols
						<< " > DELANDTSHEER_DOYEN_Y = "
						<< DELANDTSHEER_DOYEN_Y
						<< ", not OK" << endl;
					}
				else {
					cout << "direct_product_action::check_col_sums "
							"problem with col-type:" << endl;
					for (i = 1; i <= nb_col_types; i++) {
						cout << col_type_cur[i] << " ";
						}
					cout << endl;
					for (i = 1; i <= nb_col_types; i++) {
						cout << col_type_this_or_bigger[i] << " ";
						}
					cout << endl;
					}
				}
			}
		}
	return f_OK;
}

INT direct_product_action::check_mask(INT *line,
		INT len, INT verbose_level)
{
	//verbose_level = 4;
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT f_OK = TRUE;
	INT k, who;
	INT nb_rows_used, nb_cols_used;
	INT nb_singletons;


	if (f_vv) {
		cout << "direct_product_action::check_mask" << endl;
		}
	get_mask_core_and_singletons(line, len,
			nb_rows_used, nb_cols_used,
			nb_singletons, verbose_level);

	for (k = 0; k < nb_mask_tests; k++) {
		if (mask_test_level[k] != len)
			continue;
		if (mask_test_who[k] == 1) {
			who = inner_pairs_in_rows;
			}
		else if (mask_test_who[k] == 2) {
			who = inner_pairs_in_cols;
			}
		else if (mask_test_who[k] == 3) {
			who = inner_pairs_in_rows + inner_pairs_in_cols;
			}
		else if (mask_test_who[k] == 4) {
			who = nb_singletons;
			}
		else {
			cout << "direct_product_action::check_mask: "
					"unknown mask_test_who value "
					<< mask_test_who[k] << " in test " << k << endl;
			exit(1);
			}
		if (mask_test_what[k] == 1) {
			// eq
			if (who != mask_test_value[k]) {
				f_OK = FALSE;
				break;
				}
			}
		else if (mask_test_what[k] == 2) {
			// ge
			if (who < mask_test_value[k]) {
				f_OK = FALSE;
				break;
				}
			}
		else if (mask_test_what[k] == 3) {
			// le
			if (who > mask_test_value[k]) {
				f_OK = FALSE;
				break;
				}
			}
		else {
			cout << "direct_product_action::check_mask: "
					"unknown mask_test_what value "
					<< mask_test_what[k] << " in test " << k << endl;
			exit(1);
			}
		}
	if (f_v) {
		if (f_OK) {
			cout << "mask" << endl;
			//print_mask(cout, Xsize, Ysize, M);
			cout << "is OK" << endl;
			}
		else {
			if (f_vv) {
				cout << "mask test " << k << " failed:" << endl;
				print_mask_test_i(cout, k);
				//cout << "x=" << inner_pairs_in_rows
					//<< "y=" << inner_pairs_in_cols
					//<< "s=" << nb_singletons << endl;
				}
			}
		}

	return f_OK;
}


void direct_product_action::get_mask_core_and_singletons(
	INT *line, INT len,
	INT &nb_rows_used, INT &nb_cols_used,
	INT &nb_singletons, INT verbose_level)
{
	INT i, j, h, a;
	INT m = Xsize;
	INT n = Ysize;

	INT_vec_zero(f_row_used, m);
	INT_vec_zero(f_col_used, n);
	for (h = 0; h < len; h++) {
		a = line[h];
		i = a / Ysize;
		j = a % Ysize;
		f_row_used[i]++;
		row_col_idx[i] = j;
		f_col_used[j]++;
		col_row_idx[j] = i;
		}
	nb_singletons = 0;
	nb_rows_used = 0;
	for (i = 0; i < m; i++) {
		if (f_row_used[i] > 1) {
			row_idx[nb_rows_used] = i;
			nb_rows_used++;
			}
		else if (f_row_used[i] == 1) {
			j = row_col_idx[i];
			if (f_col_used[j] == 1) {
				singletons[nb_singletons++] = i * n + j;
				}
			else {
				row_idx[nb_rows_used] = i;
				nb_rows_used++;
				}
			}
		}
	nb_cols_used = 0;
	for (j = 0; j < n; j++) {
		if (f_col_used[j] > 1) {
			col_idx[nb_cols_used] = j;
			nb_cols_used++;
			}
		else if (f_col_used[j] == 1) {
			i = col_row_idx[j];
			if (f_row_used[i] > 1) {
				col_idx[nb_cols_used] = j;
				nb_cols_used++;
				}
			}
		}
}

INT design_search_check_conditions(INT len, INT *S,
		void *data, INT verbose_level)
{
	direct_product_action *P = (direct_product_action *) data;
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "design_search_check_conditions" << endl;
	}
	return P->check_conditions(S, len, verbose_level);
}

