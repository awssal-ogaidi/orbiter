// schreier_vector_handler.C
//
// Anton Betten
// started: Nov 5, 2018

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

schreier_vector_handler::schreier_vector_handler()
{
	null();
}

schreier_vector_handler::~schreier_vector_handler()
{
	freeself();
}

void schreier_vector_handler::null()
{
	A = NULL;
	cosetrep = NULL;
	Elt1 = NULL;
	Elt2 = NULL;
	Elt3 = NULL;
	f_check_image = FALSE;
}

void schreier_vector_handler::freeself()
{
	if (cosetrep) {
		FREE_int(cosetrep);
	}
	if (Elt1) {
		FREE_int(Elt1);
	}
	if (Elt2) {
		FREE_int(Elt2);
	}
	if (Elt3) {
		FREE_int(Elt3);
	}
	null();
}

void schreier_vector_handler::init(action *A, action *A2,
		int f_allow_failure,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_vector_handler::init" << endl;
	}
	schreier_vector_handler::A = A;
	schreier_vector_handler::A2 = A2;
	schreier_vector_handler::f_allow_failure = f_allow_failure;
	nb_calls_to_coset_rep_inv = 0;
	nb_calls_to_coset_rep_inv_recursion = 0;
	cosetrep = NEW_int(A->elt_size_in_int);
	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);
	if (f_v) {
		cout << "schreier_vector_handler::init done" << endl;
	}
}

int schreier_vector_handler::coset_rep_inv(
		schreier_vector *S,
		int pt, int &pt0,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int ret;

	if (f_v) {
		cout << "schreier_vector_handler::coset_rep_inv "
				"tracing point pt" << endl;
		}
	nb_calls_to_coset_rep_inv++;
	A->element_one(cosetrep, 0);
	if (f_vv) {
		cout << "schreier_vector_handler::coset_rep_inv "
				"cosetrep:" << endl;
		A->element_print_quick(cosetrep, cout);
	}
	if (f_v) {
		cout << "action A:" << endl;
		A->print_info();
		cout << "action A2:" << endl;
		A2->print_info();
		if (S->f_has_local_generators) {
			cout << "action S->local_gens->A:" << endl;
			S->local_gens->A->print_info();
			cout << "schreier_vector_handler::coset_rep_inv "
					"we have " << S->local_gens->len
			<< " local generators" << endl;
			if (f_vv) {
				int i;
				cout << "the local generators are:" << endl;
				for (i = 0; i < S->local_gens->len; i++) {
					cout << "generator " << i << " / "
							<< S->local_gens->len << ":" << endl;
					S->local_gens->A->element_print_quick(
							S->local_gens->ith(i), cout);
				}
			}
		}
		else {
			cout << "there are no local generators" << endl;
		}
	}
	if (f_v) {
		cout << "schreier_vector_handler::coset_rep_inv "
				"before coset_rep_inv_recursion" << endl;
		}
	ret = coset_rep_inv_recursion(
		S, pt, pt0,
		verbose_level - 2);
	if (f_vv) {
		cout << "schreier_vector_handler::coset_rep_inv "
				"after coset_rep_inv_recursion cosetrep:" << endl;
		A->element_print_quick(cosetrep, cout);
		}
	if (f_v) {
		if (ret) {
			cout << "schreier_vector_handler::coset_rep_inv "
					"done " << pt << "->" << pt0 << endl;
			}
		else {
			cout << "schreier_vector_handler::coset_rep_inv "
					"failure to find point" << endl;
			}
		}
	return ret;
}

int schreier_vector_handler::coset_rep_inv_recursion(
	schreier_vector *S,
	int pt, int &pt0,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int hdl, pt_loc, pr, la, n;

	if (f_v) {
		cout << "schreier_vector_handler::coset_rep_inv_recursion "
				"tracing point " << pt << endl;
		}
	nb_calls_to_coset_rep_inv_recursion++;

	//cout << "schreier_vector_coset_rep_inv_compact_general "
	//"pt = " << pt << endl;
	n = S->sv[0];
	if (!int_vec_search(S->sv + 1, S->sv[0], pt, pt_loc)) {
		if (f_allow_failure) {
			if (f_v) {
				cout << "schreier_vector_handler::coset_rep_inv_recursion "
						"did not find point. "
						"f_allow_failure is TRUE, "
						"so we return FALSE" << endl;
			}
			return FALSE;
			}
		else {
			cout << "schreier_vector_handler::coset_rep_inv_recursion "
					"did not find pt" << endl;
			cout << "pt = " << pt << endl;
			cout << "vector of length " << n << endl;
			int_vec_print(cout, S->sv + 1, n);
			cout << endl;
			exit(1);
			}
		}

	// test if the group is trivial:
	if (S->nb_gen == 0) {
		pt0 = pt;
		return TRUE;
		}
	pr = S->sv[1 + n + pt_loc];
	la = S->sv[1 + 2 * n + pt_loc];
	if (pr != -1) {

		if (f_v) {
			cout << "schreier_vector_handler::coset_rep_inv_recursion "
					"prev = " << pr << " label = " << la << endl;
			}
		//hdl = hdl_gen[la];
		if (S->f_has_local_generators) {
			if (f_v) {
				cout << "schreier_vector_handler::coset_rep_inv_recursion "
						"using local_generator" << endl;
				cout << "generator " << la << ":" << endl;
			}
			if (f_vv) {
				A->element_print_quick(S->local_gens->ith(la), cout);
			}
			A->element_move(S->local_gens->ith(la), Elt1, 0);
		} else {
			if (f_v) {
				cout << "schreier_vector_handler::coset_rep_inv_recursion "
						"using global generator" << endl;
			}
			hdl = S->gen_hdl_first + la;
			A->element_retrieve(hdl, Elt1, 0);
			//cout << "retrieving generator " << gen_idx << endl;
		}
		//A->element_print_verbose(Elt1, cout);
		A->element_invert(Elt1, Elt2, 0);

		if (f_check_image) {
			int prev;

			if (f_v) {
				cout << "schreier_vector_handler::coset_rep_inv_recursion "
						"check_image is TRUE" << endl;
			}
			prev = A2->element_image_of(pt, Elt2, 0);

			//cout << "prev = " << prev << endl;
			if (pr != prev) {
				cout << "schreier_vector_handler::coset_rep_inv_recursion: "
						"pr != prev" << endl;
				cout << "pr = " << pr << endl;
				cout << "prev = " << prev << endl;
				cout << "Elt1:" << endl;
				A->element_print_quick(Elt1, cout);
				cout << "Elt2:" << endl;
				A->element_print_quick(Elt2, cout);
				exit(1);
				}
			}

		A->element_mult(cosetrep, Elt2, Elt3, 0);
		A->element_move(Elt3, cosetrep, 0);
		if (f_v) {
			cout << "schreier_vector_handler::coset_rep_inv_recursion "
					"cosetrep:" << endl;
			A->element_print_quick(cosetrep, cout);
			}

		if (f_v) {
			cout << "schreier_vector_handler::coset_rep_inv_recursion "
					"before coset_rep_inv_recursion" << endl;
			}
		if (!coset_rep_inv_recursion(
			S,
			pr, pt0,
			verbose_level)) {
			return FALSE;
			}
		if (f_v) {
			cout << "schreier_vector_handler::coset_rep_inv_recursion "
					"after coset_rep_inv_recursion cosetrep" << endl;
			A->element_print_quick(cosetrep, cout);
			}

		}
	else {
		if (f_v) {
			cout << "prev = -1" << endl;
			}
		pt0 = pt;
		}
	return TRUE;
}



schreier_vector *schreier_vector_handler::sv_read_file(
		int gen_hdl_first, int nb_gen,
		FILE *fp, int verbose_level)
{
	int i, len;
	int4 I, n;
	int f_v = (verbose_level >= 1);
	int f_trivial_group;

	if (f_v) {
		cout << "schreier_vector_handler::sv_read_file" << endl;
		}
	I = fread_int4(fp);
	if (I == 0) {
		cout << "schreier_vector_handler::sv_read_file, "
				"no schreier vector" << endl;
		return NULL;
		}
	f_trivial_group = fread_int4(fp);
	n = fread_int4(fp);

	schreier_vector *Sv;

	int *osv;
	if (f_trivial_group) {
		osv = NEW_int(n + 1);
		len = n;
		}
	else {
		osv = NEW_int(3 * n + 1);
		len = 3 * n;
		}
	osv[0] = n;
	for (i = 0; i < len; i++) {
		osv[1 + i] = fread_int4(fp);
		}
	//sv = osv;
	Sv = NEW_OBJECT(schreier_vector);
	Sv->init(gen_hdl_first, nb_gen, osv, verbose_level);
	if (f_v) {
		cout << "schreier_vector_handler::sv_read_file "
				"read sv with " << n << " live points" << endl;
	}
	if (f_v) {
		cout << "schreier_vector_handler::sv_read_file finished" << endl;
		}
	return Sv;
}

void schreier_vector_handler::sv_write_file(schreier_vector *Sv,
		FILE *fp, int verbose_level)
{
	int i, len;
	int f_v = (verbose_level >= 1);
	int f_trivial_group;

	if (f_v) {
		cout << "schreier_vector_handler::sv_write_file" << endl;
		}
	if (Sv == NULL) {
		fwrite_int4(fp, 0);
		}
	else {
		fwrite_int4(fp, 1);
		if (Sv->nb_gen == 0) {
			f_trivial_group = TRUE;
			}
		else {
			f_trivial_group = FALSE;
			}
		fwrite_int4(fp, f_trivial_group);
		if (Sv->sv == NULL) {
			cout << "schreier_vector_handler::sv_write_file "
					"Sv->sv == NULL" << endl;
			exit(1);
		}
		int *osv = Sv->sv;
		int n = osv[0];
		fwrite_int4(fp, n);
		if (f_trivial_group) {
			len = n;
			}
		else {
			len = 3 * n;
			}
		for (i = 0; i < len; i++) {
			fwrite_int4(fp, osv[1 + i]);
			}
		}

	if (f_v) {
		cout << "schreier_vector_handler::sv_write_file "
				"finished" << endl;
		}
}

set_of_sets *schreier_vector_handler::get_orbits_as_set_of_sets(
		schreier_vector *Sv, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *orbit_reps;
	int nb_orbits;
	set_of_sets *SoS;
	int i, t;

	if (f_v) {
		cout << "schreier_vector_handler::get_orbits_as_set_of_sets" << endl;
	}
	if (Sv->nb_gen == 0) {
		cout << "schreier_vector_handler::get_orbits_as_set_of_sets "
				"Sv->nb_gen == 0" << endl;
		exit(1);
	}
	int n;
	int *pts;
	int *depth;
	int *ancestor;

	n = Sv->sv[0];
	pts = Sv->sv + 1;

	Sv->count_number_of_orbits_and_get_orbit_reps(
		orbit_reps, nb_orbits);
	SoS = NEW_OBJECT(set_of_sets);
	int *prev;

	prev = pts + n;
#if 0
	cout << "i : pts : prev" << endl;
	for (i = 0; i < n; i++) {
		cout << i << " : " << pts[i] << " : " << prev[i] << endl;
	}
#endif

	depth = NEW_int(n);
	ancestor = NEW_int(n);

	for (i = 0; i < n; i++) {
		depth[i] = -1;
		ancestor[i] = -1;
		}
	for (i = 0; i < n; i++) {
		schreier_vector_determine_depth_recursion(n,
				pts, prev, depth, ancestor, i);
		}
#if 0
	cout << "i : pts : depth : ancestor" << endl;
	for (i = 0; i < n; i++) {
		cout << i << " : " << pts[i] << " : " << depth[i] << " : " << ancestor[i] << endl;
	}
#endif

	classify C;
	int f, a;

	C.init(ancestor, n, FALSE, 0);

	SoS->init_basic(A2->degree /* underlying_set_size*/,
			C.nb_types, C.type_len, verbose_level);

	FREE_int(depth);
	FREE_int(ancestor);

	for (t = 0; t < C.nb_types; t++) {
		f = C.type_first[t];
		for (i = 0; i < C.type_len[t]; i++) {
			a = C.sorting_perm_inv[f + i];
			SoS->Sets[t][i] = pts[a];
		}
	}

	if (f_v) {
		cout << "schreier_vector_handler::get_orbits_as_set_of_sets "
				"done" << endl;
	}
	return SoS;
}
