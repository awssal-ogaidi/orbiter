// interface_matrix_group.C
//
// Anton Betten
//
// started:  November 13, 2007
// last change:  November 9, 2010
// moved here from interface.C:  January 25, 2014




#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

// #############################################################################
// interface functions: matrix group
// #############################################################################




int matrix_group_element_image_of(action &A,
		int a, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	int b;
	
	if (f_v) {
		cout << "matrix_group_element_image_of "
				"computing image of " << a << endl;
		}
	b = G.image_of_element(Elt, a, verbose_level - 1);

	if (f_v) {
		cout << "matrix_group_element_image_of "
				"image of " << a << " is " << b << endl;
		}
	return b;
}

void matrix_group_element_image_of_low_level(action &A,
		int *input, int *output, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	if (f_v) {
		cout << "matrix_group_element_image_of_low_level "
				"computing image of ";
		int_vec_print(cout, input, A.low_level_point_size);
		cout << " in action " << A.label << endl;
		}
	G.action_from_the_right_all_types(input,
			Elt, output, verbose_level - 1);


	if (f_v) {
		cout << "matrix_group_element_image_of_low_level ";
		int_vec_print(cout, input, A.low_level_point_size);
		cout << " -> ";
		int_vec_print(cout, output, A.low_level_point_size);
		cout << endl;
		}
}

int matrix_group_element_linear_entry_ij(action &A,
		void *elt, int i, int j, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	int w;

	if (f_v) {
		cout << "matrix_group_element_linear_entry_ij "
				"i=" << i << " j=" << j << endl;
		}
	w = G.GL_element_entry_ij(Elt, i, j);
	return w;
}

int matrix_group_element_linear_entry_frobenius(action &A,
		void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	int w;

	if (f_v) {
		cout << "matrix_group_element_linear_entry_frobenius" << endl;
		}
	w = G.GL_element_entry_frobenius(Elt);
	return w;
}

void matrix_group_element_one(action &A, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	
	if (f_v) {
		cout << "matrix_group_element_one calling GL_one" << endl;
		}
	G.GL_one(Elt);
}

int matrix_group_element_is_one(action &A, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	int f_is_one, i, j;
	
	if (f_v) {
		cout << "matrix_group_element_is_one" << endl;
		}
	if (G.f_kernel_is_diagonal_matrices) {
		f_is_one = G.GL_is_one(Elt);
		}
	else if (!G.f_projective) {
		f_is_one = G.GL_is_one(Elt);
		}
	else {
		cout << "matrix_group_element_is_one: warning: "
				"using slow identity element test" << endl;
		f_is_one = TRUE;
		for (i = 0; i < A.degree; i++) {
			j = A.element_image_of(i, elt, FALSE);
			if (j != i) {
				f_is_one = FALSE;
				break;
				}
			}
		}
	if (f_v) {
		if (f_is_one) {
			cout << "matrix_group_element_is_one "
					"returns YES" << endl;
			}
		else {
			cout << "matrix_group_element_is_one "
					"returns NO" << endl;
			}
		}
	return f_is_one;
}

void matrix_group_element_unpack(action &A,
		void *elt, void *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt1 = (int *) Elt;
	uchar *elt1 = (uchar *)elt;
	
	if (f_v) {
		cout << "matrix_group_element_unpack" << endl;
		}
	G.GL_unpack(elt1, Elt1, verbose_level - 1);
}

void matrix_group_element_pack(action &A,
		void *Elt, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt1 = (int *) Elt;
	uchar *elt1 = (uchar *)elt;
	
	if (f_v) {
		cout << "matrix_group_element_pack" << endl;
		}
	G.GL_pack(Elt1, elt1);
}

void matrix_group_element_retrieve(action &A,
		int hdl, void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	uchar *p_elt;
	
	if (f_v) {
		cout << "matrix_group_element_retrieve "
				"hdl = " << hdl << endl;
		}
	p_elt = G.Elts->s_i(hdl);
	//if (f_v) {
	//	element_print_packed(G, p_elt, cout);
	//	}
	G.GL_unpack(p_elt, Elt, verbose_level);
	if (f_v) {
		G.GL_print_easy(Elt, cout);
		}
}

int matrix_group_element_store(action &A,
		void *elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	int hdl;
	
	if (f_v) {
		cout << "matrix_group_element_store" << endl;
		}
	G.GL_pack(Elt, G.elt1);
	hdl = G.Elts->store(G.elt1);
	if (f_v) {
		cout << "matrix_group_element_store "
				"hdl = " << hdl << endl;
		}
	return hdl;
}

void matrix_group_element_mult(action &A,
		void *a, void *b, void *ab, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	matrix_group &G = *A.G.matrix_grp;
	int *AA = (int *) a;
	int *BB = (int *) b;
	int *AB = (int *) ab;

	if (f_v) {
		cout << "matrix_group_element_mult" << endl;
		}
	if (f_vv) {
		cout << "A=" << endl;
		G.GL_print_easy(AA, cout);
		cout << "B=" << endl;
		G.GL_print_easy(BB, cout);
		}
	G.GL_mult(AA, BB, AB, verbose_level - 2);
	if (f_v) {
		cout << "matrix_group_element_mult done" << endl;
		}
	if (f_vv) {
		cout << "AB=" << endl;
		G.GL_print_easy(AB, cout);
		}
}

void matrix_group_element_invert(action &A,
		void *a, void *av, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	matrix_group &G = *A.G.matrix_grp;
	int *AA = (int *) a;
	int *AAv = (int *) av;

	if (f_v) {
		cout << "matrix_group_element_invert" << endl;
		}
	if (f_vv) {
		cout << "A=" << endl;
		G.GL_print_easy(AA, cout);
		}
	G.GL_invert(AA, AAv);
	if (f_v) {
		cout << "matrix_group_element_invert done" << endl;
		}
	if (f_vv) {
		cout << "Av=" << endl;
		G.GL_print_easy(AAv, cout);
		}
}

void matrix_group_element_transpose(action &A,
		void *a, void *at, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	matrix_group &G = *A.G.matrix_grp;
	int *AA = (int *) a;
	int *Atv = (int *) at;

	if (f_v) {
		cout << "matrix_group_element_transpose" << endl;
		}
	if (f_vv) {
		cout << "A=" << endl;
		G.GL_print_easy(AA, cout);
		}
	G.GL_transpose(AA, Atv, verbose_level);
	if (f_v) {
		cout << "matrix_group_element_transpose done" << endl;
		}
	if (f_vv) {
		cout << "At=" << endl;
		G.GL_print_easy(Atv, cout);
		}
}

void matrix_group_element_move(action &A,
		void *a, void *b, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;
	int *AA = (int *) a;
	int *BB = (int *) b;

	if (f_v) {
		cout << "matrix_group_element_move" << endl;
		}
	G.GL_copy(AA, BB);
}

void matrix_group_element_dispose(action &A,
		int hdl, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	matrix_group &G = *A.G.matrix_grp;

	if (f_v) {
		cout << "matrix_group_element_dispose "
				"hdl = " << hdl << endl;
		}
	G.Elts->dispose(hdl);
}

void matrix_group_element_print(action &A,
		void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	

	G.GL_print_easy(Elt, ost);
	ost << endl;
	if (G.GFq->q > 2) {
		ost << "=" << endl;
		G.GL_print_easy_normalized(Elt, ost);
		ost << endl;
		}

#if 0
	int *fp, n;

	fp = NEW_int(A.degree);
	n = A.find_fixed_points(elt, fp, 0);
	cout << "with " << n << " fixed points ";
	A.element_print_base_images(Elt, ost);
	cout << endl;
	FREE_int(fp);
#endif

#if 0
	if (A.degree < 0 /*1000*/) {
		//cout << "matrix_group_element_print:
		//printing element as permutation" << endl;
		matrix_group_element_print_as_permutation(A, elt, ost);
		ost << endl;
		}
#endif
}

void matrix_group_element_code_for_make_element(action &A,
		void *elt, int *data)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	//cout << "matrix_group_element_code_for_make_element
	//calling GL_print_for_make_element" << endl;
	G.GL_code_for_make_element(Elt, data);
	//cout << "matrix_group_element_code_for_make_element
	//after GL_print_for_make_element" << endl;
}

void matrix_group_element_print_for_make_element(action &A,
		void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	//cout << "matrix_group_element_print_for_make_element
	//calling GL_print_for_make_element" << endl;
	G.GL_print_for_make_element(Elt, ost);
	//cout << "matrix_group_element_print_for_make_element
	//after GL_print_for_make_element" << endl;
}

void matrix_group_element_print_for_make_element_no_commas(
		action &A, void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	//cout << "matrix_group_element_print_for_make_element_
	//no_commas calling GL_print_for_make_element_no_commas" << endl;
	G.GL_print_for_make_element_no_commas(Elt, ost);
	//cout << "matrix_group_element_print_for_make_element_
	//no_commas after GL_print_for_make_element_no_commas" << endl;
}

void matrix_group_element_print_quick(action &A,
		void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;
	//int *fp; //, n;
	

	G.GL_print_easy(Elt, ost);


#if 0
	ost << endl;
	ost << "=" << endl;
	G.GL_print_easy_normalized(Elt, ost);
	ost << endl;
#endif

#if 0
	A.element_print_base_images_verbose(Elt, ost, 0);
	ost << endl;
#endif
	//fp = NEW_int(A.degree);
	//n = A.find_fixed_points(elt, fp, 0);
	//cout << "with " << n << " fixed points" << endl;
	//FREE_int(fp);
	if (FALSE /*A.degree < 0*/ /*1000*/) {
		//cout << "matrix_group_element_print:
		//printing element as permutation" << endl;
		matrix_group_element_print_as_permutation(A, elt, ost);
		ost << endl;
		}
}

void matrix_group_element_print_latex(action &A,
		void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	G.GL_print_latex(Elt, ost);
	//G.GL_print_easy_latex(Elt, ost);
}

void matrix_group_element_print_as_permutation(action &A,
		void *elt, ostream &ost)
{
	//matrix_group &G = *A.G.matrix_grp;
	int f_v = FALSE;
	int *Elt = (int *) elt;
	int i, j;
	
	if (f_v) {
		cout << "matrix_group_element_print_as_permutation "
				"degree = " << A.degree << endl;
		}
	int *p = NEW_int(A.degree);
	for (i = 0; i < A.degree; i++) {
		//cout << "matrix_group_element_print_as_permutation
		//computing image of i=" << i << endl;
		//if (i == 3)
			//f_v = TRUE;
		//else
			//f_v = FALSE;
		j = A.element_image_of(i, Elt, 0 /* verbose_level */);
		p[i] = j;
		}
	perm_print(ost, p, A.degree);
	FREE_int(p);
}

void matrix_group_element_print_verbose(action &A,
		void *elt, ostream &ost)
{
	matrix_group &G = *A.G.matrix_grp;
	int *Elt = (int *) elt;

	G.GL_print_easy(Elt, ost);
	ost << "\n";
	int i, j;
	
	if (A.degree < 1000) {
		int *p = NEW_int(A.degree);
		for (i = 0; i < A.degree; i++) {
			j = A.element_image_of(i, Elt, FALSE);
			p[i] = j;
			}
		perm_print(ost, p, A.degree);
		FREE_int(p);
		}
	else {
#if 0
		cout << "i : image" << endl;
		for (i = 0; i < MINIMUM(40, G.degree); i++) {
			j = A.element_image_of(i, Elt, FALSE);
			cout << i << " : " << j << endl;
			}
#endif
		}

}

void matrix_group_print_point(action &A, int a, ostream &ost)
{
	matrix_group *G = A.G.matrix_grp;
	
	if (G->f_projective) {
		G->GFq->PG_element_unrank_modified(G->v1, 1, G->n, a);
		}
	else if (G->f_affine) {
		AG_element_unrank(G->GFq->q, G->v1, 1, G->n, a);
		}
	else if (G->f_general_linear) {
		AG_element_unrank(G->GFq->q, G->v1, 1, G->n, a);
		}
	else {
		cout << "matrix_group_print_point unknown group type" << endl;
		exit(1);
		}
	int_vec_print(ost, G->v1, G->n);
}


