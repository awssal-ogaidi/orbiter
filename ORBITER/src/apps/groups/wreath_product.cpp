// wreath_product.C
//
// Anton Betten
//
// August 4, 2018
//
//
//

#include "orbiter.h"


// global data:

INT t0; // the system time when the program started

void usage(int argc, const char **argv);
int main(int argc, const char **argv);
INT wreath_rank_point_func(INT *v, void *data);
void wreath_unrank_point_func(INT *v, INT rk, void *data);



typedef class tensor_product tensor_product;

class tensor_product {
public:
	int argc;
	const char **argv;
	INT nb_factors;
	INT n;
	INT q;

	finite_field *F;
	action *A;
	action *A0;

	strong_generators *SG;
	longinteger_object go;
	wreath_product *W;
	generator *Gen;
	INT vector_space_dimension;

	tensor_product();
	~tensor_product();
	void init(int argc, const char **argv,
			INT nb_factors, INT n, INT q, INT depth,
			INT verbose_level);
};





void usage(int argc, const char **argv)
{
	cout << "usage: " << argv[0] << " [options]" << endl;
	cout << "where options can be:" << endl;
	cout << "-v <n>                   : verbose level n" << endl;
	cout << "-nb_factors <nb_factors> : set number of factors" << endl;
	cout << "-d <d>                   : set dimension d" << endl;
	cout << "-q <q>                   : set field size q" << endl;
}



int main(int argc, const char **argv)
{
	INT i;
	INT verbose_level = 0;
	INT f_nb_factors = FALSE;
	INT nb_factors = 0;
	INT f_d = FALSE;
	INT d = 0;
	INT f_q = FALSE;
	INT q = 0;
	INT f_depth = FALSE;
	INT depth = 0;

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
		else if (strcmp(argv[i], "-nb_factors") == 0) {
			f_nb_factors = TRUE;
			nb_factors = atoi(argv[++i]);
			cout << "-nb_factors " << nb_factors << endl;
			}
		else if (strcmp(argv[i], "-d") == 0) {
			f_d = TRUE;
			d = atoi(argv[++i]);
			cout << "-d " << d << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		else if (strcmp(argv[i], "-depth") == 0) {
			f_depth = TRUE;
			depth = atoi(argv[++i]);
			cout << "-depth " << depth << endl;
			}
		}
	if (!f_nb_factors) {
		cout << "please use -nb_factors <nb_factors>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_d) {
		cout << "please use -d <d>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_q) {
		cout << "please use -q <q>" << endl;
		usage(argc, argv);
		exit(1);
		}
	if (!f_depth) {
		cout << "please use -depth <depth>" << endl;
		usage(argc, argv);
		exit(1);
		}


	//do_it(argc, argv, nb_factors, d, q, verbose_level);


	tensor_product *T;

	T = new tensor_product;

	T->init(argc, argv, nb_factors, d, q, depth, verbose_level);


	the_end_quietly(t0);

}

tensor_product::tensor_product()
{
	argc= 0;
	argv = NULL;
	nb_factors = 0;
	vector_space_dimension = 0;
	n = 0;
	q = 0;
	SG = NULL;
	F = NULL;
	A = NULL;
	A0 = NULL;
	W = NULL;
	Gen = NULL;
}

tensor_product::~tensor_product()
{

}

void tensor_product::init(int argc, const char **argv,
		INT nb_factors, INT n, INT q, INT depth,
		INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT *v;
	INT i, j, a;

	if (f_v) {
		cout << "tensor_product::init" << endl;
	}
	tensor_product::argc = argc;
	tensor_product::argv = argv;
	tensor_product::nb_factors = nb_factors;
	tensor_product::n = n;
	tensor_product::q = q;

	A = new action;

	v = NEW_INT(n);


	F = new finite_field;

	F->init(q, 0);

	A->init_wreath_product_group_and_restrict(nb_factors, n,
			F,
			verbose_level);
	cout << "tensor_product::init after "
			"A->init_wreath_product_group_and_restrict" << endl;

	if (!A->f_has_subaction) {
		cout << "tensor_product::init action "
				"A does not have a subaction" << endl;
		exit(1);
	}
	A0 = A->subaction;

	W = A0->G.wreath_product_group;

	vector_space_dimension = W->dimension_of_tensor_action;

	if (!A0->f_has_strong_generators) {
		cout << "tensor_product::init action A0 does not "
				"have strong generators" << endl;
		exit(1);
		}

	SG = A0->Strong_gens;
	SG->group_order(go);

	cout << "The group " << A->label << " has order " << go
			<< " and permutation degree " << A->degree << endl;



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



#if 0

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

	FREE_INT(Elt1);
	FREE_INT(Elt2);
	FREE_INT(Elt3);
	FREE_INT(Elt4);
	FREE_INT(perm1);
	FREE_INT(perm2);
	FREE_INT(perm3);
	FREE_INT(perm4);
	FREE_INT(perm5);
#endif


	Gen = new generator;

	Gen->read_arguments(argc, argv, 0);

	//Gen->prefix[0] = 0;
	sprintf(Gen->fname_base, "wreath_%ld_%ld_%ld", nb_factors, n, q);


	Gen->depth = depth;

	if (f_v) {
		cout << "tensor_product::init before Gen->init" << endl;
		}
	Gen->init(A0, A, SG, Gen->depth /* sz */, verbose_level);
	if (f_v) {
		cout << "tensor_product::init after Gen->init" << endl;
		}


#if 0
	Gen->init_check_func(
		subspace_orbits_test_func,
		this /* candidate_check_data */);
	Gen->init_early_test_func(
		linear_set_early_test_func,
		this /*void *data */,
		verbose_level);
#endif

	//Gen->init_incremental_check_func(
		//check_mindist_incremental,
		//this /* candidate_check_data */);

	Gen->init_vector_space_action(
		vector_space_dimension,
		F,
		wreath_rank_point_func,
		wreath_unrank_point_func,
		this,
		verbose_level);
#if 0
	Gen->f_print_function = TRUE;
	Gen->print_function = print_set;
	Gen->print_function_data = this;
#endif

	INT nb_nodes = 1000;

	if (f_v) {
		cout << "tensor_product::init before Gen->init_poset_orbit_node" << endl;
		}
	Gen->init_poset_orbit_node(nb_nodes, verbose_level - 1);
	if (f_v) {
		cout << "tensor_product::init calling Gen->init_root_node" << endl;
		}
	Gen->root[0].init_root_node(Gen, verbose_level - 1);

	INT schreier_depth;
	INT f_use_invariant_subset_if_available;
	INT f_debug;

	schreier_depth = Gen->depth;
	f_use_invariant_subset_if_available = TRUE;
	f_debug = FALSE;

	INT t0 = os_ticks();

	if (f_v) {
		cout << "tensor_product::init before Gen->main" << endl;
		cout << "A=";
		A->print_info();
		cout << "A0=";
		A0->print_info();
		}


	//Gen->f_allowed_to_show_group_elements = TRUE;

	Gen->main(t0,
		Gen->depth,
		f_use_invariant_subset_if_available,
		f_debug,
		verbose_level);

	if (f_v) {
		cout << "tensor_product::init after Gen->main" << endl;
	}
}


INT wreath_rank_point_func(INT *v, void *data)
{
	tensor_product *T;
	INT rk;

	T = (tensor_product *) data;
	//AG_element_rank(LS->Fq->q, v, 1, LS->vector_space_dimension, rk);
	PG_element_rank_modified(*T->F, v, 1, T->vector_space_dimension, rk);
	return rk;
}

void wreath_unrank_point_func(INT *v, INT rk, void *data)
{
	tensor_product *T;

	T = (tensor_product *) data;
	//AG_element_unrank(LS->Fq->q, v, 1, LS->vector_space_dimension, rk);
	PG_element_unrank_modified(*T->F, v, 1, T->vector_space_dimension, rk);
}


