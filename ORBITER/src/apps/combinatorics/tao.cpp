// tao.C
// 
// Anton Betten
// May 21, 2017
//
//
// 
//
//

#include "orbiter.h"


// global data:

INT t0; // the system time when the program started


void test_heisenberg(INT n, INT q, INT verbose_level);
void difference_set_in_heisenberg_group_early_test_func(INT *S, INT len, 
		INT *candidates, INT nb_candidates, 
		INT *good_candidates, INT &nb_good_candidates, 
		void *data, INT verbose_level);
void zuppo_list(sims *S, INT *Zuppos, INT &nb_zuppos, INT verbose_level);
void dimino(sims *S, 
	INT *subgroup, INT subgroup_sz, INT *gens, INT &nb_gens, 
	INT *cosets, 
	INT new_gen, 
	INT *group, INT &group_sz, 
	INT verbose_level);


int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_heisenberg = FALSE;
	INT n, q;
	
 	t0 = os_ticks();
	
	for (i = 1; i < argc - 1; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-heisenberg") == 0) {
			f_heisenberg = TRUE;
			n = atoi(argv[++i]);
			q = atoi(argv[++i]);
			cout << "-heisenberg " << n << " " << q << endl;
			}
		}
	
	if (f_heisenberg) {
		test_heisenberg(n, q, verbose_level);
		}
	the_end(t0);
}


typedef class difference_set_in_heisenberg_group difference_set_in_heisenberg_group;


//! class related to a problem of Tao


class difference_set_in_heisenberg_group {

public:
	BYTE fname_base[1000];

	INT n;
	INT q;
	finite_field *F;
	heisenberg *H;
	INT *Table;
	INT *Table_abv;
	INT *gens;
	INT nb_gens;

#if 0
	INT *N_gens;
	INT N_nb_gens;
	INT N_go;
#endif
	action *A;
	strong_generators *Aut_gens;
	longinteger_object Aut_order;
	
	INT given_base_length; // = nb_gens
	INT *given_base; // = gens
	INT *base_image;
	INT *base_image_elts;
	INT *E1;
	INT rk_E1;

	BYTE prefix[1000];
	BYTE fname_magma_out[1000];
	sims *Aut;
	sims *U;
	longinteger_object U_go;
	vector_ge *U_gens;
	schreier *Sch;


	// N = normalizer of U in Aut
	INT *N_gens;
	INT N_nb_gens, N_go;
	action *N;
	longinteger_object N_order;

	action *N_on_orbits;
	INT *Paired_with;
	INT nb_paired_orbits;
	INT *Pairs;
	INT *Pair_orbit_length;


	INT *Pairs_of_type1;
	INT nb_pairs_of_type1;
	INT *Pairs_of_type2;
	INT nb_pairs_of_type2;
	INT *Sets1;
	INT *Sets2;


	INT *Short_pairs;
	INT *Long_pairs;

	INT *f_orbit_select;
	INT *Short_orbit_inverse;

	action *A_on_short_orbits;
	INT nb_short_orbits;
	INT nb_long_orbits;

	poset_classification *gen;




	void init(INT n, INT q, INT verbose_level);
	void do_n2q3(INT verbose_level);
	void check_overgroups_of_order_nine(INT verbose_level);
	void create_minimal_overgroups(INT verbose_level);
	void early_test_func(INT *S, INT len, 
		INT *candidates, INT nb_candidates, 
		INT *good_candidates, INT &nb_good_candidates, 
		INT verbose_level);

};

void difference_set_in_heisenberg_group::init(INT n, INT q, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i; //, j, a, b, ord;
	
	if (f_v) {
		cout << "difference_set_in_heisenberg_group::init" << endl;
		}
	difference_set_in_heisenberg_group::n = n;
	difference_set_in_heisenberg_group::q = q;

	F = NEW_OBJECT(finite_field);
	F->init(q, 0);

	H = NEW_OBJECT(heisenberg);
	H->init(F, n, verbose_level);

	cout << "group order = " << H->group_order << endl;
	H->group_table(Table, verbose_level);
	cout << "group_table:" << endl;
	if (H->group_order < 50) {
		print_integer_matrix_with_standard_labels(cout, Table, H->group_order, H->group_order, FALSE /* f_tex */);
		}
	else {
		cout << "Too big to print" << endl;
		}
	H->group_table_abv(Table_abv, verbose_level);
	cout << "group order = " << H->group_order << endl;
	
	
	H->generating_set(gens, nb_gens, verbose_level);
	cout << "Generating set of size " << nb_gens << ":" << endl;
	INT_vec_print(cout, gens, nb_gens);
	cout << endl;

	for (i = 0; i < nb_gens; i++) {
		cout << "generator " << i << " / " << nb_gens << " is " << gens[i] << ":" << endl;
		H->unrank_element(H->Elt1, gens[i]);
		INT_vec_print(cout, H->Elt1, H->len);
		cout << endl;
		}

	sprintf(fname_base, "H_%ld_%ld", n, q);
	//magma_write_permutation_group(fname_base, H->group_order, Table, gens, nb_gens, verbose_level);


	given_base_length = nb_gens;
	given_base = gens;
#if 0
	magma_normalizer_in_Sym_n(fname_base, H->group_order, Table, gens, nb_gens, 
		N_gens, N_nb_gens, N_go, verbose_level);

	cout << "The holomorph has order " << N_go << " and is generated by " << N_nb_gens << " elements" << endl;
	for (i = 0; i < N_nb_gens; i++) {
		cout << "holomorph generator " << i << " / " << N_nb_gens << ":" << endl;

		ord = perm_order(N_gens + i * H->group_order, H->group_order);
		cout << "an element of order " << ord << endl;
		for (j = 0; j < nb_gens; j++) {
			a = gens[j];
			b = N_gens[i * H->group_order + a];
			cout << a << " -> " << b << " : ";
			H->unrank_element(H->Elt1, a);
			H->unrank_element(H->Elt2, b);
			INT_vec_print(cout, H->Elt1, H->len);
			cout << " -> ";
			INT_vec_print(cout, H->Elt2, H->len);
			cout << endl;
			}
		}
	given_base_length = H->len;
	given_base = NEW_INT(given_base_length);
	for (i = 0; i < given_base_length; i++) {
		given_base[i] = i_power_j(q, i);
		}
	cout << "given base: ";
	INT_vec_print(cout, given_base, given_base_length);
	cout << endl;
	
#if 0
	H_gens = NEW_INT(H->group_order * nb_gens);
	for (i = 0; i < nb_gens; i++) {
		INT_vec_copy(Table + gens[i] * H->group_order, H_gens + i * H->group_order, H->group_order);
		}
#endif

	A = NEW_OBJECT(action);


	cout << "creating holomorph" << endl;
	A->init_permutation_group_from_generators(H->group_order /* degree */, 
		N_nb_gens, N_gens, 
		given_base_length, given_base,
		0 /*verbose_level*/);
	{
	longinteger_object go;
	A->group_order(go);
	cout << "The order of the holomorph is " << go << endl;
	}

	cout << "creating automorphism group" << endl;
	Aut_gens = A->Strong_gens->point_stabilizer(0 /* pt */, verbose_level);
	Aut_gens->group_order(Aut_order);
	cout << "The automorphism group has order " << Aut_order << endl;
#endif

	A = create_automorphism_group_from_group_table(fname_base, 
		Table, H->group_order, gens, nb_gens, 
		Aut_gens, 
		verbose_level);

	Aut_gens->group_order(Aut_order);

	

	if (f_v) {
		cout << "difference_set_in_heisenberg_group::init done" << endl;
		}
}

void difference_set_in_heisenberg_group::do_n2q3(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j, a, b, t;
	INT h, k, f, /*l,*/ u, v, len1, len2, /*pos,*/ s;
	
	if (f_v) {
		cout << "difference_set_in_heisenberg_group::do_n2q3" << endl;
		}

	base_image = NEW_INT(given_base_length);
	base_image_elts = NEW_INT(given_base_length * H->len);

	INT_vec_zero(base_image_elts, given_base_length * H->len);

	base_image_elts[0] = 1;
	base_image_elts[2] = 1;
	base_image_elts[4] = 2;

	base_image_elts[5 + 1] = 1;
	base_image_elts[5 + 3] = 1;
	base_image_elts[5 + 4] = 2;

	base_image_elts[10 + 2] = 1;

	base_image_elts[15 + 3] = 1;

	base_image_elts[20 + 4] = 1;

	for (i = 0; i < 5; i++) {
		base_image[i] = H->rank_element(base_image_elts + i * 5);
		}

	E1 = NEW_INT(A->elt_size_in_INT);
	
	cout << "making element" << endl;
	A->make_element_from_base_image(E1, base_image, verbose_level);
	cout << "generator has been created:" << endl;
	A->element_print(E1, cout);
	cout << endl;


	U_gens = NEW_OBJECT(vector_ge);
	U_gens->init_single(A, E1);
	
	Aut = Aut_gens->create_sims(verbose_level);
	U = NEW_OBJECT(sims);

	cout << "The group U" << endl;
	U = create_sims_from_generators_without_target_group_order(A, 
		U_gens, 0 /* verbose_level */);
	U->group_order(U_go);
	cout << "The order of U is " << U_go << endl;

	
	
	Sch = NEW_OBJECT(schreier);
	Sch->init(A);
	Sch->init_generators(*U_gens);
	Sch->compute_all_point_orbits(0 /*verbose_level*/);
	cout << "The orbits of U are:" << endl;
	Sch->print_and_list_orbits_tex(cout);
	Sch->print_orbit_lengths(cout);
	Sch->print_orbit_length_distribution(cout);

	sprintf(prefix, "N_U_2_3");
	cout << "computing normalizer of U in G:" << endl;
	A->normalizer_using_MAGMA(prefix, Aut, U, verbose_level);

	sprintf(fname_magma_out, "%snormalizer.txt", prefix);


	read_magma_permutation_group(fname_magma_out, H->group_order, N_gens, N_nb_gens, N_go, verbose_level);


	
	N = NEW_OBJECT(action);
	longinteger_object n_go;

	n_go.create(N_go);
	N->init_permutation_group_from_generators(H->group_order /* degree */, 
		TRUE, n_go, 
		N_nb_gens, N_gens, 
		given_base_length, given_base,
		0 /*verbose_level*/);
	N->group_order(N_order);

	cout << "created the normalizer N of order " << N_order << endl;

	
	rk_E1 = N->Sims->element_rank_INT(E1);
	cout << "rk_E1 = " << rk_E1 << endl;

	N_on_orbits = NEW_OBJECT(action);
	
	cout << "creating action on orbits:" << endl;
	N_on_orbits->induced_action_on_orbits(N, Sch, TRUE /* f_play_it_safe */, 
		verbose_level);

	cout << "action on orbits has been created, degree = " <<N_on_orbits->degree << endl;


	Paired_with = NEW_INT(Sch->nb_orbits);
	for (i = 0; i < Sch->nb_orbits; i++) {
		f = Sch->orbit_first[i];
		//l = Sch->orbit_len[i];
		a = Sch->orbit[f];
		b = H->element_negate_by_rank(a, 0 /* verbose_level */);
		//pos = Sch->orbit_inv[b];
		j = Sch->orbit_number(b); // Sch->orbit_no[pos];
		Paired_with[i] = j;
		}
	cout << "Paired_with:" << endl;
	for (i = 0; i < Sch->nb_orbits; i++) {
		cout << i << " : " << Paired_with[i] << endl;
		}
	nb_paired_orbits = 0;
	Pairs = NEW_INT(Sch->nb_orbits * 2);
	Pair_orbit_length = NEW_INT(Sch->nb_orbits);
	for (i = 0; i < Sch->nb_orbits; i++) {
		if (Paired_with[i] <= i) { // omit the self paired orbit of the identity element
			continue;
			}
		Pairs[2 * nb_paired_orbits + 0] = i;
		Pairs[2 * nb_paired_orbits + 1] = Paired_with[i];
		Pair_orbit_length[nb_paired_orbits] = Sch->orbit_len[i];
		nb_paired_orbits++;
		}
	cout << "We found " << nb_paired_orbits << " paired orbits" << endl;
	for (h = 0; h < nb_paired_orbits; h++) {
		a = Pairs[2 * h + 0];
		b = Pairs[2 * h + 1];
		u = Sch->orbit_first[a];
		v = Sch->orbit_first[b];
		len1 = Sch->orbit_len[a];
		len2 = Sch->orbit_len[b];
		cout << h << " / " << nb_paired_orbits << " : " << a << "," << b << " = {";
		for (k = 0; k < len1; k++) {
			i = Sch->orbit[u + k];
			cout << i;
			if (k < len1 - 1) {
				cout << ", ";
				}
			}
		cout << "}, ";
		cout << "{";
		for (k = 0; k < len2; k++) {
			j = Sch->orbit[v + k];
			cout << j;
			if (k < len2 - 1) {
				cout << ", ";
				}
			}
		cout << "}, orbits of length " << len1 << endl;
		if (len1 != len2) {
			cout << "len1 != len2" << endl;
			exit(1);
			}
		}


	classify Pair_orbit_type;
	Pair_orbit_type.init(Pair_orbit_length, nb_paired_orbits, FALSE, 0);



	Pair_orbit_type.get_class_by_value(Pairs_of_type1, nb_pairs_of_type1, 1, 0 /* verbose_level */);
	Pair_orbit_type.get_class_by_value(Pairs_of_type2, nb_pairs_of_type2, 3, 0 /* verbose_level */);
	INT_vec_heapsort(Pairs_of_type1, nb_pairs_of_type1);
	INT_vec_heapsort(Pairs_of_type2, nb_pairs_of_type2);
	cout << "We found " << 	nb_pairs_of_type1 << " pairs of short orbits, they are:" << endl;
	INT_vec_print(cout, Pairs_of_type1, nb_pairs_of_type1);
	cout << endl;
	cout << "We found " << 	nb_pairs_of_type2 << " pairs of long orbits, they are:" << endl;
	INT_vec_print(cout, Pairs_of_type2, nb_pairs_of_type2);
	cout << endl;



	Sets1 = NEW_INT(nb_pairs_of_type1 * 2);
	for (s = 0; s < nb_pairs_of_type1; s++) {
		h = Pairs_of_type1[s];
		a = Pairs[2 * h + 0];
		b = Pairs[2 * h + 1];
		u = Sch->orbit_first[a];
		v = Sch->orbit_first[b];
		len1 = Sch->orbit_len[a];
		len2 = Sch->orbit_len[b];
		Sets1[s * 2 + 0] = Sch->orbit[u];
		Sets1[s * 2 + 1] = Sch->orbit[v];
		}
	Sets2 = NEW_INT(nb_pairs_of_type2 * 6);
	for (s = 0; s < nb_pairs_of_type2; s++) {
		h = Pairs_of_type2[s];
		a = Pairs[2 * h + 0];
		b = Pairs[2 * h + 1];
		u = Sch->orbit_first[a];
		v = Sch->orbit_first[b];
		len1 = Sch->orbit_len[a];
		len2 = Sch->orbit_len[b];
		for (k = 0; k < 3; k++) {
			Sets2[s * 6 + k] = Sch->orbit[u + k];
			}
		for (k = 0; k < 3; k++) {
			Sets2[s * 6 + 3 + k] = Sch->orbit[v + k];
			}
		}
	cout << "Sets1:" << endl;
	print_integer_matrix_with_standard_labels(cout, Sets1, nb_pairs_of_type1, 2, FALSE /* f_tex */);
	cout << "Sets2:" << endl;
	print_integer_matrix_with_standard_labels(cout, Sets2, nb_pairs_of_type2, 6, FALSE /* f_tex */);




	nb_short_orbits = 2 * nb_pairs_of_type1;
	nb_long_orbits = 2 * nb_pairs_of_type2;
	

	Short_pairs = NEW_INT(nb_short_orbits);
	Long_pairs = NEW_INT(nb_long_orbits);

	Short_orbit_inverse = NEW_INT(Sch->nb_orbits);
	INT_vec_mone(Short_orbit_inverse, Sch->nb_orbits);

	for (s = 0; s < nb_pairs_of_type1; s++) {
		h = Pairs_of_type1[s];
		INT_vec_copy(Pairs + 2 * h, Short_pairs + 2 * s, 2);
		for (t = 0; t < 2; t++) {
			a = Short_pairs[2 * s + t];
			Short_orbit_inverse[a] = 2 * s + t;
			}
		}


	for (s = 0; s < nb_pairs_of_type2; s++) {
		h = Pairs_of_type2[s];
		INT_vec_copy(Pairs + 2 * h, Long_pairs + 2 * s, 2);
		}
	
	cout << "Short_pairs:" << endl;
	print_integer_matrix_with_standard_labels(cout, Short_pairs, nb_pairs_of_type1, 2, FALSE /* f_tex */);
	cout << "Long_pairs:" << endl;
	print_integer_matrix_with_standard_labels(cout, Long_pairs, nb_pairs_of_type2, 2, FALSE /* f_tex */);


	cout << "creating restricted action on short orbits:" << endl;
	A_on_short_orbits = N_on_orbits->restricted_action(Short_pairs, nb_short_orbits, verbose_level);
	

	//create_minimal_overgroups(verbose_level);

	check_overgroups_of_order_nine(verbose_level);
}


void difference_set_in_heisenberg_group::check_overgroups_of_order_nine(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);



	if (f_v) {
		cout << "difference_set_in_heisenberg_group::check_overgroups_of_order_nine" << endl;
		}
	INT second_gen_idx[] = { 
		731, 353, 381, 379, 46, 357, 700, 359, 698, 721, 
		696, 694, 723, 355, 725, 44, 690, 688, 686, 664, 
		733, 383, 660, 347, 42, 656, 345, 652, 343, 1035, 
		648, 1037, 387, 40, 991, 1295, 1293, 995, 1289, 1287, 
		997, 1283, 1279, 1258, 1254, 38, 1244, 1220, 1218, 1216, 
		1027, 1248, 1210, 1208, 1206, 1007, 1078, 1003, 1072, 1070, 
		426, 1064, 999, 424, 1062, 1043, 1041, 422, 1285, 420, 
		418, 52, 1250, 414, 50, 993, 391, 658, 737, 385, 
		48, 1029 };

	INT *Elt1, *Elt2;
	INT h;
	INT nb_overgroups = sizeof(second_gen_idx) / sizeof(INT);
	
	Elt1 = NEW_INT(A->elt_size_in_INT);
	Elt2 = NEW_INT(A->elt_size_in_INT);
	

	cout << "making element Elt1:" << endl;
	N->Sims->element_unrank_INT(rk_E1, Elt1, 0);
	A->element_print(Elt1, cout);
	cout << endl;


	for (h = 0; h < nb_overgroups; h++) {

		cout << "overgroup " << h << " / " << nb_overgroups << ":" << endl;
		
		sims *O;
		vector_ge *O_gens;
		longinteger_object O_go;
		schreier *Sch1;


		cout << "making element" << endl;
		N->Sims->element_unrank_INT(second_gen_idx[h], Elt2, 0);
		cout << "second generator has been created:" << endl;
		A->element_print(Elt2, cout);
		cout << endl;


		O_gens = NEW_OBJECT(vector_ge);
		O_gens->init_double(A, Elt1, Elt2);
	
		O = NEW_OBJECT(sims);

		cout << "The group O" << endl;
		O = create_sims_from_generators_without_target_group_order(A, 
			O_gens, 0 /* verbose_level */);
		O->group_order(O_go);
		cout << "The order of O is " << O_go << endl;
		if (O_go.as_INT() != 9) {
			cout << "The group O does not have order 9" << endl;
			exit(1);
			}

	
	
		Sch1 = NEW_OBJECT(schreier);
		Sch1->init(N_on_orbits);
		Sch1->init_generators(*O_gens);
		Sch1->compute_all_point_orbits(0 /*verbose_level*/);
		cout << "The orbits of O are:" << endl;
		Sch1->print_and_list_orbits_tex(cout);
		Sch1->print_orbit_lengths(cout);
		Sch1->print_orbit_length_distribution(cout);


		classify Overgroup_orbit_type;
		Overgroup_orbit_type.init(Sch1->orbit_len, Sch1->nb_orbits, FALSE, 0);
		cout << "Overgroup orbit type:" << endl;
		Overgroup_orbit_type.print_naked(TRUE);
		cout << endl;

		INT *Pairing;
		INT s, a, b, f, l, o1, o2; //, pos;


		Pairing = NEW_INT(nb_long_orbits);
		for (a = 0; a < Sch1->nb_orbits; a++) {
			f = Sch1->orbit_first[a];
			l = Sch1->orbit_len[a];
			o1 = Sch1->orbit[f];
			o2 = Paired_with[o1];


			cout << "normalizer orbit " << a << " / " << Sch1->nb_orbits << " is U-orbit " << o1 << endl;
			cout << "U-orbit " << o1 << " is paired with U-orbit " << o2 << endl;
			//pos = Sch1->orbit_inv[o2];
			b = Sch1->orbit_number(o2); // Sch1->orbit_no[pos];
			cout << "Which is normalizer orbit " << b << endl;
			Pairing[a] = b;
			}
		cout << "Pairing:" << endl;
		print_integer_matrix_with_standard_labels(cout, Pairing, Sch1->nb_orbits, 1, FALSE /* f_tex */);
	
	
		INT *long_orbits;
		INT nb_long_orbits;
		INT *Pairs_of_long_orbits;
		INT nb_pairs_of_long_orbits;


		Overgroup_orbit_type.get_class_by_value(long_orbits, nb_long_orbits, 3, 0 /* verbose_level */);
		INT_vec_heapsort(long_orbits, nb_long_orbits);
		cout << "We found " << 	nb_long_orbits << " long orbits, they are:" << endl;
		INT_vec_print(cout, long_orbits, nb_long_orbits);
		cout << endl;


		

		Pairs_of_long_orbits = NEW_INT(Sch1->nb_orbits * 2);
		nb_pairs_of_long_orbits = 0;
		for (s = 0; s < nb_long_orbits; s++) {
			a = long_orbits[s];
			b = Pairing[a];
			if (b <= a) {
				continue;
				}
			Pairs_of_long_orbits[nb_pairs_of_long_orbits * 2 + 0] = a;
			Pairs_of_long_orbits[nb_pairs_of_long_orbits * 2 + 1] = b;
			nb_pairs_of_long_orbits++;
			}
		cout << "Pairs_of_long_orbits:" << endl;
		print_integer_matrix_with_standard_labels(cout, Pairs_of_long_orbits, nb_pairs_of_long_orbits, 2, FALSE /* f_tex */);


		INT N;
		INT *selection;
		INT *count;
		INT *D;
		INT D_sz;
		INT u, t, ff, ll, v, k, di, dj;
		INT i, j;
		INT nb_good, nb_bad;

		N = i_power_j(2, nb_pairs_of_long_orbits);
		cout << "N=" << N << endl;
		selection = NEW_INT(nb_pairs_of_long_orbits);
		D = NEW_INT(H->group_order);
		count = NEW_INT(H->group_order);
		nb_good = 0;
		nb_bad = 0;
		for (u = 0; u < N; u++) {
			AG_element_unrank(2, selection, 1, nb_pairs_of_long_orbits, u);
			D_sz = 0;
			for (t = 0; t < nb_pairs_of_long_orbits; t++) {
				if (selection[t]) {
					a = Pairs_of_long_orbits[t * 2 + 1];
					}
				else {
					a = Pairs_of_long_orbits[t * 2 + 0];
					}
				f = Sch1->orbit_first[a];
				l = Sch1->orbit_len[a];
				if (l != 3) {
					cout << "l != 3" << endl;
					cout << "a=" << a << endl;
					cout << "l=" << l << endl;
					cout << "s=" << s << endl;
					cout << "t=" << t << endl;
					cout << "testing case " << u << " = ";
					INT_vec_print(cout, selection, nb_pairs_of_long_orbits);
					cout << endl;
					exit(1);
					}
				for (v = 0; v < l; v++) {
					o1 = Sch1->orbit[f + v];
					ff = Sch->orbit_first[o1];
					ll = Sch->orbit_len[o1];
					if (ll != 3) {
						cout << "ll != 3" << endl;
						exit(1);
						}
					for (k = 0; k < ll; k++) {
						D[D_sz++] = Sch->orbit[ff + k];
						}
					}
				}
			if (((u + 250) % 1) == 0) {
				cout << "testing case " << u << " = ";
				INT_vec_print(cout, selection, nb_pairs_of_long_orbits);
				cout << endl;
				cout << "We found a set D of size " << D_sz << ":" << endl;
				INT_vec_print(cout, D, D_sz);
				cout << endl;
				}
			INT_vec_zero(count, H->group_order);
			for (i = 0; i < D_sz; i++) {
				di = D[i];
				for (j = 0; j < D_sz; j++) {
					if (j == i) {
						continue;
						}
					dj = D[j];
					k = Table_abv[di * H->group_order + dj];
					count[k]++;
					}
				}

			classify D_type;
			D_type.init(count, H->group_order, FALSE, 0);
			cout << "D type:" << endl;
			D_type.print_naked(TRUE);
			cout << endl;
			for (i = 0; i < H->group_order; i++) {
				if (count[i] > 60) {
					break;
					}
				}
			if (i < H->group_order) {
				cout << "bad" << endl;
				nb_bad++;
				}
			else {
				cout << "good" << endl;
				nb_good++;
				}

			
			}
		cout << "group " << h << ":" << endl;
		cout << "nb_good=" << nb_good << endl;
		cout << "nb_bad=" << nb_bad << endl;

		if (nb_good) {
			break;
			}

		delete Sch1;
		delete O;
		delete O_gens;
		
		//exit(1);
		} // next h
	
	if (f_v) {
		cout << "difference_set_in_heisenberg_group::check_overgroups_of_order_nine done" << endl;
		}
}

void difference_set_in_heisenberg_group::create_minimal_overgroups(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT *Zuppos;
	INT nb_zuppos;
	INT goi;
	longinteger_object go;
	INT i, j, t;

	if (f_v) {
		cout << "difference_set_in_heisenberg_group::create_minimal_overgroups" << endl;
		}
	
	N->Sims->group_order(go);
	cout << "go=" << go << endl;
	goi = go.as_INT();


	Zuppos = NEW_INT(goi);
	zuppo_list(N->Sims, Zuppos, nb_zuppos, verbose_level);

	cout << "We found " << nb_zuppos << " zuppos." << endl;
	INT_vec_print(cout, Zuppos, nb_zuppos);
	cout << endl;

	INT *subgroup_elements;
	INT subgroup_sz;
	INT *gens;
	INT nb_gens;
	INT *cosets;
	//INT new_gen;
	INT *group;
	INT group_sz;

	subgroup_elements = NEW_INT(goi);
	gens = NEW_INT(goi);
	cosets = NEW_INT(goi);
	group = NEW_INT(goi);
	subgroup **Subs;
	INT *Group_order;
	INT z;

	Subs = new psubgroup[nb_zuppos];

	Group_order = NEW_INT(nb_zuppos);

	for (z = 0; z < nb_zuppos; z++) {
		//cout << "zuppo " << z << " / " << nb_zuppos << ":" << endl;
		subgroup_elements[0] = 0;
		subgroup_sz = 1;
		nb_gens = 0;
		dimino(N->Sims, 
			subgroup_elements, subgroup_sz, gens, nb_gens, 
			cosets, 
			Zuppos[z] /* new_gen*/, 
			group, group_sz, 
			0 /* verbose_level */);

#if 0
		cout << "found a group of order " << group_sz << " : ";
		INT_vec_print(cout, group, group_sz);
		cout << endl;
#endif

		Group_order[z] = group_sz;

		Subs[z] = new subgroup;
		Subs[z]->init(group, group_sz, gens, nb_gens);
		
		}
	classify Group_orders;
	INT *Idx_subgroup;
	INT nb_subgroups;
	INT o, idx_E1;

	Group_orders.init(Group_order, nb_zuppos, FALSE, 0);
	cout << "We found the following group orders: ";
	Group_orders.print_naked(TRUE);
	cout << endl;


	subgroup **Subgroups_of_order;

	for (i = 0; i < Group_orders.nb_types; i++) {
		o = Group_orders.data_sorted[Group_orders.type_first[i]];
		Group_orders.get_class_by_value(Idx_subgroup, nb_subgroups, o /* value */, verbose_level);
		cout << "We have " << nb_subgroups << " subgroups of order " << o << endl;
		
		Subgroups_of_order = new psubgroup[nb_subgroups];
		for (j = 0; j < nb_subgroups; j++) {
			Subgroups_of_order[j] = Subs[Idx_subgroup[j]];
			Subs[Idx_subgroup[j]] = NULL;
			}

		cout << "The subgroups of order " << o << " are:" << endl;
		for (j = 0; j < nb_subgroups; j++) {
			Subgroups_of_order[j]->print();
			}

		if (o == 3) {
			// search for the group which contains the element rk_E1:
			for (j = 0; j < nb_subgroups; j++) {
			
				if (Subgroups_of_order[j]->contains_this_element(rk_E1)) {
					break;
					}
				}
			if (j == nb_subgroups) {
				cout << "We could not find the group containing the element rk_E1" << endl;
				exit(1);
				}
			idx_E1 = j;
			cout << "The subgroup containing the element E1=" << rk_E1 << " has index " << idx_E1 << ":" << endl;
			Subgroups_of_order[j]->print();

			action *A_on_subgroups;
			
			cout << "creating action on the subgroups:" << endl;
			A_on_subgroups = N->create_induced_action_on_subgroups(N->Sims, 
				nb_subgroups, o /* group_order */, Subgroups_of_order, verbose_level);
			cout << "action on subgroups created:" << endl;
			A_on_subgroups->print_info();


			schreier *Sch_subgroups;

			cout << "computing orbit of conjugated subgroups:" << endl;
			Sch_subgroups = N->Strong_gens->orbit_of_one_point_schreier(A_on_subgroups, idx_E1, verbose_level);
			
			cout << "found an orbit of conjugated subgroups of length " << Sch_subgroups->orbit_len[0] << endl;

			// compute minimal overgroups:
			subgroup **Overgroups;
			INT *Overgroup_order;

			Overgroups = new psubgroup[nb_zuppos];

			Overgroup_order = NEW_INT(nb_zuppos);

			for (z = 0; z < nb_zuppos; z++) {
				cout << "zuppo " << z << " / " << nb_zuppos << ":" << endl;
				INT_vec_copy(Subgroups_of_order[j]->Elements, subgroup_elements, Subgroups_of_order[j]->group_order);
				subgroup_sz = Subgroups_of_order[j]->group_order;
				INT_vec_copy(Subgroups_of_order[j]->gens, gens, Subgroups_of_order[j]->nb_gens);
				nb_gens = Subgroups_of_order[j]->nb_gens;
				dimino(N->Sims, 
					subgroup_elements, subgroup_sz, gens, nb_gens, 
					cosets, 
					Zuppos[z] /* new_gen*/, 
					group, group_sz, 
					0 /* verbose_level */);

				cout << "found a group of order " << group_sz << " : ";
				INT_vec_print(cout, group, group_sz);
				cout << endl;

				Overgroup_order[z] = group_sz;

				Overgroups[z] = new subgroup;
				Overgroups[z]->init(group, group_sz, gens, nb_gens);
		
				}
			classify Overgroup_orders;

			Overgroup_orders.init(Overgroup_order, nb_zuppos, FALSE, 0);
			cout << "We found the following overgroup orders: ";
			Overgroup_orders.print_naked(TRUE);
			cout << endl;

			INT *Idx_overgroup;
			INT nb_overgroups;
			INT ii, oo, f, a;
			
			subgroup **Overgroups_of_order;

			for (ii = 0; ii < Overgroup_orders.nb_types; ii++) {
				oo = Overgroup_orders.data_sorted[Overgroup_orders.type_first[ii]];
				if (oo == 3) {
					continue;
					}
				Overgroup_orders.get_class_by_value(Idx_overgroup, nb_overgroups, oo /* value */, verbose_level);
				cout << "We have " << nb_overgroups << " overgroups of order " << oo << endl;
		
				Overgroups_of_order = new psubgroup[nb_overgroups];
				for (j = 0; j < nb_overgroups; j++) {
					Overgroups_of_order[j] = Overgroups[Idx_overgroup[j]];
					Overgroups[Idx_overgroup[j]] = NULL;
					}


#if 0
				cout << "The overgroups of order " << oo << " are:" << endl;
				for (j = 0; j < nb_overgroups; j++) {
					cout << j << " / " << nb_overgroups << " : ";
					Overgroups_of_order[j]->print();
					}
#endif
				action *A_on_overgroups;
			
				cout << "creating action on the overgroups of order " << oo << ":" << endl;
				A_on_overgroups = N->create_induced_action_on_subgroups(N->Sims, 
					nb_overgroups, oo /* group_order */, Overgroups_of_order, 0 /*verbose_level*/);
				cout << "action on overgroups created:" << endl;
				A_on_overgroups->print_info();


				schreier *Sch_overgroups;

				cout << "computing the orbits of conjugated overgroups of order " << oo << ":" << endl;
				Sch_overgroups = N->Strong_gens->orbits_on_points_schreier(A_on_overgroups, 0 /*verbose_level*/);
			
				cout << "The conjugacy classes of overgroups of order " << oo << " have the following lengths: ";
				Sch_overgroups->print_orbit_length_distribution(cout);
				cout << "There are " << Sch_overgroups->nb_orbits << " classes" << endl;
				cout << "Representatives of the conjugacy classes are:" << endl;

				INT *second_generator;
				second_generator = NEW_INT(Sch_overgroups->nb_orbits);
				for (j = 0; j < Sch_overgroups->nb_orbits; j++) {
					//cout << "orbit " << j << " / " << Sch_overgroups->nb_orbits << ":" << endl;
					f = Sch_overgroups->orbit_first[j];
					a = Sch_overgroups->orbit[f];
					//Overgroups_of_order[a]->print();
					for (t = 0; t < 2; t++) {
						if (Overgroups_of_order[a]->gens[t] != rk_E1) {
							second_generator[j] = Overgroups_of_order[a]->gens[t];
							break;
							}
						}
					if (t == 2) {
						cout << "t == 2" << endl;
						exit(1);
						}
					}
				cout << "the second generators are:" << endl;
				//print_integer_matrix_with_standard_labels(cout, second_generator, Sch_overgroups->nb_orbits, 1, FALSE /* f_tex */);
				INT_vec_print_Cpp(cout, second_generator, Sch_overgroups->nb_orbits);
				cout << endl;
				} // next ii
			exit(1);
			}
		}
#if 0
	BYTE prefix[000];
	INT f_W = TRUE;
	INT f_w = FALSE;
	INT target_depth = nb_pairs_of_type1;

	sprintf(prefix, "H_%ld_%ld_short", n, q);

	cout << "classifying subsets:" << endl;

	f_orbit_select = NEW_INT(nb_short_orbits);

	compute_orbits_on_subsets(gen, 
		target_depth,
		prefix, 
		f_W, f_w,
		N, A_on_short_orbits, 
		N->Strong_gens, 
		difference_set_in_heisenberg_group_early_test_func,
		this /* void *early_test_func_data */, 
		NULL /* INT (*candidate_incremental_check_func)(INT len, INT *S, void *data, INT verbose_level)*/, 
		this /* void *candidate_incremental_check_data */, 
		verbose_level);

#endif

	if (f_v) {
		cout << "difference_set_in_heisenberg_group::do_n2q3 done" << endl;
		}
}

void difference_set_in_heisenberg_group::early_test_func(INT *S, INT len, 
	INT *candidates, INT nb_candidates, 
	INT *good_candidates, INT &nb_good_candidates, 
	INT verbose_level)
{
	verbose_level = 1;
	INT f_v = (verbose_level >= 1);
	INT i, a, aa, b, bb;
	
	if (f_v) {
		cout << "difference_set_in_heisenberg_group::early_test_func" << endl;
		cout << "S=";
		INT_vec_print(cout, S, len);
		cout << endl;
		}

	nb_good_candidates = 0;
	INT_vec_zero(f_orbit_select, nb_short_orbits);


	for (i = 0; i < len; i++) {
		if (f_v) {
			cout << "set element " << i << " / " << len << ":" << endl;
			}
		a = S[i];
		f_orbit_select[a] = TRUE;
		aa = Short_pairs[a];
		bb = Paired_with[aa];
		b = Short_orbit_inverse[bb];
		if (f_v) {
			cout << "orbit " << a << "=" << aa << " is paired with " << bb << "=" << b << endl;
			}
		if (b == -1) {
			cout << "difference_set_in_heisenberg_group::early_test_func b = -1" << endl;
			cout << "chosen orbit " << a << "=" << aa << " which is paired with " << bb << "=" << b << endl;
			exit(1);
			}
		if (f_orbit_select[b]) {
			cout << "difference_set_in_heisenberg_group::early_test_func the set is not admissible" << endl;
			exit(1);
			}
		}
	for (i = 0; i < nb_candidates; i++) {
		a = candidates[i];
		if (f_orbit_select[a]) {
			continue; // we don't allow an orbit twice.
			}
		aa = Short_pairs[a];
		bb = Paired_with[aa];
		b = Short_orbit_inverse[bb];
		if (f_v) {
			cout << "testing orbit " << a << "=" << aa << " which is paired with " << bb << "=" << b << " : ";
			}
		if (b == -1) {
			cout << "difference_set_in_heisenberg_group::early_test_func b = -1" << endl;
			cout << "testing orbit " << a << "=" << aa << " which is paired with " << bb << "=" << b << " : " << endl;
			exit(1);
			}
		if (!f_orbit_select[b]) {
			if (f_v) {
				cout << " is OK" << endl;
				}
			good_candidates[nb_good_candidates++] = a;
			}
		else {
			if (f_v) {
				cout << " is bad" << endl;
				}
			}
		}
	if (f_v) {
		cout << "we found " << nb_good_candidates << " good candidates" << endl;
		}
	if (f_v) {
		cout << "They are:" << endl;
		INT_vec_print(cout, good_candidates, nb_good_candidates);
		cout << endl;
		}
	if (f_v) {
		cout << "difference_set_in_heisenberg_group::early_test_func done" << endl;
		}
}


void test_heisenberg(INT n, INT q, INT verbose_level)
{
	difference_set_in_heisenberg_group *DS;

	DS = NEW_OBJECT(difference_set_in_heisenberg_group);
	
	DS->init(n, q, verbose_level);



	if (n != 2) {
		cout << "from now on, we are expecting that n=2" << endl;
		exit(1);
		}
	if (q != 3) {
		cout << "from now on, we are expecting that q=3" << endl;
		exit(1);
		}

	DS->do_n2q3(verbose_level);

	
}

void difference_set_in_heisenberg_group_early_test_func(INT *S, INT len, 
		INT *candidates, INT nb_candidates, 
		INT *good_candidates, INT &nb_good_candidates, 
		void *data, INT verbose_level)
{
	difference_set_in_heisenberg_group *DS = (difference_set_in_heisenberg_group *) data;

	DS->early_test_func(S, len, 
		candidates, nb_candidates, 
		good_candidates, nb_good_candidates, 
		verbose_level);
}

void zuppo_list(sims *S, INT *Zuppos, INT &nb_zuppos, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT goi;
	longinteger_object go;
	INT rk, o, i, j;
	INT *Elt1;
	INT *Elt2;
	INT *f_done;
	action *A;

	if (f_v) {
		cout << "zuppo_list" << endl;
		}
	A = S->A;
	S->group_order(go);
	cout << "go=" << go << endl;
	goi = go.as_INT();
	Elt1 = NEW_INT(A->elt_size_in_INT);
	Elt2 = NEW_INT(A->elt_size_in_INT);
	f_done = NEW_INT(goi);
	INT_vec_zero(f_done, goi);
	if (f_v) {
		cout << "zuppo_list group of order " << goi << endl;
		}
	nb_zuppos = 0;
	for (rk = 0; rk < goi; rk++) {
		//cout << "element " << rk << " / " << goi << endl;
		if (f_done[rk]) {
			continue;
			}
		S->element_unrank_INT(rk, Elt1, 0 /*verbose_level*/);
		//cout << "element created" << endl;
		o = A->element_order(Elt1);
		//cout << "element order = " << o << endl;
		if (o == 1) {
			continue;
			}
		if (!is_prime_power(o)) {
			continue;
			}
		cout << "element " << rk << " / " << goi << " has order " << o << " which is a prime power; nb_zuppos = " << nb_zuppos << endl;
		Zuppos[nb_zuppos++] = rk;
		f_done[rk] = TRUE;
		for (i = 1; i < o; i++) {
			if (gcd_INT(i, o) == 1) {
				A->element_move(Elt1, Elt2, 0);
				A->element_power_INT_in_place(Elt2, i, 0 /* verbose_level*/);
				j = S->element_rank_INT(Elt2);
				f_done[j] = TRUE;
				}
			}
		}
	cout << "We found " << nb_zuppos << " zuppo elements" << endl;
	FREE_INT(Elt1);
	FREE_INT(Elt2);
	FREE_INT(f_done);
	if (f_v) {
		cout << "zuppo_list done" << endl;
		}
}

void dimino(sims *S, 
	INT *subgroup, INT subgroup_sz, INT *gens, INT &nb_gens, 
	INT *cosets, 
	INT new_gen, 
	INT *group, INT &group_sz, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, j, k, c, idx, new_coset_rep, nb_cosets;

	if (f_v) {
		cout << "dimino new_gen = " << new_gen << endl;
		}
	INT_vec_copy(subgroup, group, subgroup_sz);
	INT_vec_heapsort(group, subgroup_sz);
	group_sz = subgroup_sz;
	
	cosets[0] = 0;
	nb_cosets = 1;
	gens[nb_gens++] = new_gen;
	for (i = 0; i < nb_cosets; i++) {
		for (j = 0; j < nb_gens; j++) {
			if (f_vv) {
				cout << "coset rep " << i << " = " << cosets[i] << endl;
				cout << "generator " << j << " = " << gens[j] << endl;
				}
			
			c = S->mult_by_rank(cosets[i], gens[j]);
			if (f_vv) {
				cout << "coset rep " << i << " times generator " << j << " is " << c << endl;
				}
			if (INT_vec_search(group, group_sz, c, idx)) {
				if (f_vv) {
					cout << "already there" << endl;
					}
				continue;
				}
			if (f_vv) {
				cout << "n e w coset rep" << endl;
				}
			new_coset_rep = c;
			
			for (k = 0; k < subgroup_sz; k++) {
				c = S->mult_by_rank(subgroup[k], new_coset_rep);
				group[group_sz++] = c;
				}
			INT_vec_heapsort(group, group_sz);
			if (f_vv) {
				cout << "new group size = " << group_sz << endl;
				}
			cosets[nb_cosets++] = new_coset_rep;
			}
		}
	if (f_vv) {
		cout << "dimino, the n e w group has order " << group_sz << endl;
		}
	
	if (f_v) {
		cout << "dimino done" << endl;
		}
}

#if 0
INT mult(sims *S, INT rk_a, INT rk_b, INT *Elt1, INT *Elt2, INT *Elt3)
{
	INT rk;

	S->element_unrank_INT(rk_a, Elt1, 0 /*verbose_level*/);
	S->element_unrank_INT(rk_b, Elt2, 0 /*verbose_level*/);
	S->A->element_mult(Elt1, Elt2, Elt3, 0);
	rk = S->element_rank_INT(Elt3);
	return rk;
}
#endif
#if 0
INT conjugate(sims *S, INT rk_a, INT rk_b, INT *Elt1, INT *Elt2, INT *Elt3, INT *Elt4)
// computes b^{-1} * a * b
{
	INT rk;

	S->element_unrank_INT(rk_a, Elt1, 0 /*verbose_level*/); // Elt1 = a
	S->element_unrank_INT(rk_b, Elt2, 0 /*verbose_level*/); // Elt2 = b
	S->A->element_invert(Elt2, Elt3, 0 /*verbose_level*/); // Elt3 = b^{-1}
	S->A->element_mult(Elt3, Elt1, Elt4, 0);
	S->A->element_mult(Elt4, Elt2, Elt3, 0);
	rk = S->element_rank_INT(Elt3);
	return rk;
}
#endif


