// wreath_product.C
//
// Anton Betten
//
// started:  August 2, 2018




#include "foundations/foundations.h"
#include "groups_and_group_actions.h"


wreath_product::wreath_product()
{
	null();
}

wreath_product::~wreath_product()
{
	freeself();
}

void wreath_product::null()
{
	M = NULL;
	F = NULL;
	P = NULL;
	perm_offset_i = NULL;
	mtx_size = NULL;
	index_set1 = NULL;
	index_set2 = NULL;
	u = NULL;
	v = NULL;
	w = NULL;
	A1 = NULL;
	A2 = NULL;
	A3 = NULL;
	tmp_Elt1 = NULL;
	tmp_perm1 = NULL;
	tmp_perm2 = NULL;
	induced_perm = NULL;
	elt1 = NULL;
	Elts = NULL;
	base_for_component = NULL;
	tl_for_component = NULL;
	the_base = NULL;
	the_transversal_length = NULL;
}

void wreath_product::freeself()
{
	INT verbose_level = 0;
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "wreath_product::freeself" << endl;
		}
	if (mtx_size) {
		FREE_INT(mtx_size);
	}
	if (perm_offset_i) {
		FREE_INT(perm_offset_i);
	}
	if (index_set1) {
		FREE_INT(index_set1);
	}
	if (index_set2) {
		FREE_INT(index_set2);
	}
	if (u) {
		FREE_INT(u);
	}
	if (v) {
		FREE_INT(v);
	}
	if (w) {
		FREE_INT(w);
	}
	if (A1) {
		FREE_INT(A1);
	}
	if (A2) {
		FREE_INT(A2);
	}
	if (A3) {
		FREE_INT(A3);
	}
	if (tmp_Elt1) {
		FREE_INT(tmp_Elt1);
	}
	if (tmp_perm1) {
		FREE_INT(tmp_perm1);
	}
	if (tmp_perm2) {
		FREE_INT(tmp_perm2);
	}
	if (induced_perm) {
		FREE_INT(induced_perm);
	}
	if (P) {
		FREE_OBJECT(P);
	}
	if (elt1) {
		FREE_UBYTE(elt1);
	}
	if (Elts) {
		FREE_OBJECT(Elts);
	}
	if (base_for_component) {
		FREE_INT(base_for_component);
	}
	if (tl_for_component) {
		FREE_INT(tl_for_component);
	}
	if (the_base) {
		FREE_INT(the_base);
	}
	if (the_transversal_length) {
		FREE_INT(the_transversal_length);
	}
	null();
	if (f_v) {
		cout << "wreath_product::freeself finished" << endl;
		}
}

void wreath_product::init_tensor_wreath_product(matrix_group *M,
		action *A_mtx, INT nb_factors, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;

	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product" << endl;
	}
	if (M->f_projective) {
		cout << "void wreath_product::init_tensor_wreath_product "
				"the input group must be of type general linear "
				"(not projective)" << endl;
		exit(1);
	}
	if (M->f_affine) {
		cout << "void wreath_product::init_tensor_wreath_product "
				"the input group must be of type general linear "
				"(not affine)" << endl;
		exit(1);
	}
	wreath_product::M = M;
	wreath_product::A_mtx = A_mtx;
	wreath_product::nb_factors = nb_factors;
	F = M->GFq;
	q = F->q;

	P = NEW_OBJECT(perm_group);
	P->init(nb_factors, 10 /* page_length_log */, 0 /* verbose_level */);

	sprintf(label, "%s_wreath_Sym%ld", M->label, nb_factors);
	sprintf(label_tex, "%s \\wr {\\rm Sym}(%ld)", M->label_tex, nb_factors);

	degree_of_matrix_group = M->degree;
	dimension_of_matrix_group = M->n;
	dimension_of_tensor_action =
			i_power_j(dimension_of_matrix_group, nb_factors);
	low_level_point_size = dimension_of_tensor_action;
	make_element_size = nb_factors + nb_factors *
			dimension_of_matrix_group * dimension_of_matrix_group;
	degree_of_tensor_action =
			(i_power_j(q, dimension_of_tensor_action) - 1) / (q - 1);
	degree_overall = nb_factors + nb_factors *
			degree_of_matrix_group + degree_of_tensor_action;
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"dimension_of_tensor_action = "
				<< dimension_of_tensor_action << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"degree_of_tensor_action = "
				<< degree_of_tensor_action << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"degree_overall = " << degree_overall << endl;
	}
	perm_offset_i = NEW_INT(nb_factors + 1);
		// one more so it can also be used to indicated
		// the start of the tensor product action.
	perm_offset_i[0] = nb_factors;
	for (i = 1; i <= nb_factors; i++) {
		// note equality here!
		perm_offset_i[i] = perm_offset_i[i - 1] + degree_of_matrix_group;
	}
	mtx_size = NEW_INT(nb_factors);
	for (i = 0; i < nb_factors; i++) {
		mtx_size[i] = i_power_j(dimension_of_matrix_group, i + 1);
	}
	index_set1 = NEW_INT(nb_factors);
	index_set2 = NEW_INT(nb_factors);
	u = NEW_INT(dimension_of_tensor_action);
	v = NEW_INT(dimension_of_tensor_action);
	w = NEW_INT(dimension_of_tensor_action);
	A1 = NEW_INT(dimension_of_tensor_action * dimension_of_tensor_action);
	A2 = NEW_INT(dimension_of_tensor_action * dimension_of_tensor_action);
	A3 = NEW_INT(dimension_of_tensor_action * dimension_of_tensor_action);
	elt_size_INT = M->elt_size_INT * nb_factors + P->elt_size_INT;
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"elt_size_INT = " << elt_size_INT << endl;
	}
	tmp_Elt1 = NEW_INT(elt_size_INT);
	tmp_perm1 = NEW_INT(P->elt_size_INT);
	tmp_perm2 = NEW_INT(P->elt_size_INT);
	induced_perm = NEW_INT(dimension_of_tensor_action);

	bits_per_digit = M->bits_per_digit;
	bits_per_elt = nb_factors * dimension_of_matrix_group *
			dimension_of_matrix_group * bits_per_digit;
	char_per_elt = nb_factors + ((bits_per_elt + 7) >> 3);
	elt1 = NEW_UBYTE(char_per_elt);
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"bits_per_digit = " << bits_per_digit << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"bits_per_elt = " << bits_per_elt << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"char_per_elt = " << char_per_elt << endl;
	}
	base_len_in_component = matrix_group_base_len_general_linear_group(
			dimension_of_matrix_group, q,
			FALSE /*f_semilinear */, verbose_level - 1);
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"base_len_in_component = " << base_len_in_component << endl;
	}
	base_for_component = NEW_INT(base_len_in_component);
	tl_for_component = NEW_INT(base_len_in_component);
	general_linear_matrix_group_base_and_transversal_length(
		dimension_of_matrix_group,
		F, FALSE /* f_semilinear */,
		base_len_in_component, degree_of_matrix_group,
		base_for_component, tl_for_component,
		verbose_level - 1);
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"base_for_component = ";
		INT_vec_print(cout, base_for_component, base_len_in_component);
		cout << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"tl_for_component = ";
		INT_vec_print(cout, tl_for_component, base_len_in_component);
		cout << endl;
	}

	Elts = NEW_OBJECT(page_storage);
	Elts->init(char_per_elt /* entry_size */,
			10 /* page_length_log */, verbose_level);

	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"before compute_base_and_transversals" << endl;
	}
	compute_base_and_transversals(verbose_level);
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"after compute_base_and_transversals" << endl;
	}
	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product "
				"the_base = ";
		INT_vec_print(cout, the_base, base_length);
		cout << endl;
		cout << "wreath_product::init_tensor_wreath_product "
				"the_transversal_length = ";
		INT_vec_print(cout, the_transversal_length, base_length);
		cout << endl;
	}

	if (f_v) {
		cout << "wreath_product::init_tensor_wreath_product done" << endl;
	}
}

INT wreath_product::element_image_of(INT *Elt, INT a, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, a0, b, c;

	if (f_v) {
		cout << "wreath_product::element_image_of" << endl;
	}
	a0 = a;
	b = 0;
	if (a < nb_factors) {
		if (f_v) {
			cout << "wreath_product::element_image_of "
					"we are in the permutation" << endl;
		}
		b = Elt[a];
	} else {
		a -= nb_factors;
		b += nb_factors;
		for (f = 0; f < nb_factors; f++) {
			if (a < M->degree) {
				if (f_v) {
					cout << "wreath_product::element_image_of "
							"we are in component " << f
							<< " reduced input a=" << a << endl;
				}
				AG_element_unrank(q, u, 1, M->n, a);
				F->mult_vector_from_the_left(u, Elt + offset_i(f), v,
						M->n, M->n);
				AG_element_rank(q, v, 1, M->n, c);
				if (f_v) {
					cout << "wreath_product::element_image_of "
							"we are in component " << f
							<< " reduced output c=" << c << endl;
				}
				b += c;
				break;
			} else {
				a -= M->degree;
				b += M->degree;
			}
		} // next f
		if (f == nb_factors) {
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"we are in the tensor product component "
						"reduced input a = " << a << endl;
			}
			PG_element_unrank_modified(*F, u, 1,
					dimension_of_tensor_action, a);
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"u = ";
				INT_vec_print(cout, u, dimension_of_tensor_action);
				cout << endl;
			}
			create_matrix(Elt, A3, 0 /* verbose_level */);
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"A3 = " << endl;
				INT_matrix_print(A3,
						dimension_of_tensor_action,
						dimension_of_tensor_action);
			}
			F->mult_vector_from_the_left(u, A3, v,
					dimension_of_tensor_action,
					dimension_of_tensor_action);
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"v = ";
				INT_vec_print(cout, v, dimension_of_tensor_action);
				cout << endl;
			}
			apply_permutation(Elt, v, w, 0 /* verbose_level*/);
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"w = ";
				INT_vec_print(cout, w, dimension_of_tensor_action);
				cout << endl;
			}
			PG_element_rank_modified(*F, w, 1,
					dimension_of_tensor_action, c);
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"we are in tensor product component " << f
						<< " reduced output c=" << c << endl;
			}
			b += c;
			if (f_v) {
				cout << "wreath_product::element_image_of "
						"we are in tensor product component " << f
						<< " output b=" << b << endl;
			}
		}
	}
	if (f_v) {
		cout << "wreath_product::element_image_of " << a0
				<< " maps to " << b << endl;
	}
	return b;
}

void wreath_product::element_image_of_low_level(INT *Elt,
		INT *input, INT *output, INT verbose_level)
// we assume that we are in the tensor product domain
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "wreath_product::element_image_of_low_level" << endl;
	}
	if (f_v) {
		cout << "wreath_product::element_image_of_low_level "
				"input = ";
		INT_vec_print(cout, input, dimension_of_tensor_action);
		cout << endl;
	}
	create_matrix(Elt, A3, 0 /* verbose_level */);
	if (f_v) {
		cout << "wreath_product::element_image_of_low_level "
				"A3 = " << endl;
		INT_matrix_print(A3,
				dimension_of_tensor_action,
				dimension_of_tensor_action);
	}
	F->mult_vector_from_the_left(input, A3, v,
			dimension_of_tensor_action,
			dimension_of_tensor_action);
	if (f_v) {
		cout << "wreath_product::element_image_of_low_level "
				"v = ";
		INT_vec_print(cout, v, dimension_of_tensor_action);
		cout << endl;
	}
	apply_permutation(Elt, v, output, verbose_level - 1);
	if (f_v) {
		cout << "wreath_product::element_image_of_low_level "
				"output = ";
		INT_vec_print(cout, output, dimension_of_tensor_action);
		cout << endl;
	}
	if (f_v) {
		cout << "wreath_product::element_image_of_low_level done" << endl;
	}
}

void wreath_product::element_one(INT *Elt)
{
	INT f;

	P->one(Elt);
	for (f = 0; f < nb_factors; f++) {
		M->GL_one(Elt + offset_i(f));
	}
}

INT wreath_product::element_is_one(INT *Elt)
{
		INT f, scalar;

		if (!P->is_one(Elt)) {
			return FALSE;
		}
		for (f = 0; f < nb_factors; f++) {
			if (!F->is_scalar_multiple_of_identity_matrix(
					Elt + offset_i(f), dimension_of_matrix_group,
					scalar)) {
				return FALSE;
			}
		}
		return TRUE;
}

void wreath_product::element_mult(INT *A, INT *B, INT *AB,
		INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, g;

	if (f_v) {
		cout << "wreath_product::element_mult" << endl;
	}
	perm_mult(A, B, AB, nb_factors);
	for (f = 0; f < nb_factors; f++) {
		g = A[f];
		M->GL_mult(A + offset_i(f),
				B + offset_i(g),
				AB + offset_i(f),
				0 /* verbose_level */);
	}
	if (f_v) {
		cout << "wreath_product::element_mult done" << endl;
	}
}

void wreath_product::element_move(INT *A, INT *B, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "wreath_product::element_move" << endl;
	}
	INT_vec_copy(A, B, elt_size_INT);

#if 0
	if (f_v) {
		element_print_easy(B, cout);
	}
#endif

	if (f_v) {
		cout << "wreath_product::element_move done" << endl;
	}
}

void wreath_product::element_invert(INT *A, INT *Av, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, g;

	if (f_v) {
		cout << "wreath_product::element_invert" << endl;
	}
	perm_inverse(A, Av, nb_factors);
	for (f = 0; f < nb_factors; f++) {
		g = A[f];
		M->GL_invert(A + offset_i(f), Av + offset_i(g));
	}
	if (f_v) {
		cout << "wreath_product::element_invert done" << endl;
	}
}

void wreath_product::compute_induced_permutation(INT *Elt, INT *perm)
{
	INT i, j, h, k, a;

	for (i = 0; i < dimension_of_tensor_action; i++) {
		AG_element_unrank(q, index_set1, 1, nb_factors, i);
		for (h = 0; h < nb_factors; h++) {
			a = index_set1[h];
			k = Elt[h];
			index_set2[k] = a;
		}
		AG_element_rank(q, index_set2, 1, nb_factors, j);
		perm[i] = j;
	}
}


void wreath_product::apply_permutation(INT *Elt,
		INT *v_in, INT *v_out, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j;

	if (f_v) {
		cout << "wreath_product::apply_permutation" << endl;
	}
	if (f_v) {
		cout << "wreath_product::apply_permutation perm=";
		INT_vec_print(cout, Elt, nb_factors);
		cout << endl;
	}
	if (f_v) {
		cout << "wreath_product::apply_permutation v_in=";
		INT_vec_print(cout, v_in, dimension_of_tensor_action);
		cout << endl;
	}
	INT_vec_zero(v_out, dimension_of_tensor_action);

	//perm_inverse(Elt, tmp_perm1, nb_factors);

	compute_induced_permutation(Elt, induced_perm);
	if (f_v) {
		cout << "wreath_product::apply_permutation induced_perm=";
		INT_vec_print(cout, induced_perm, dimension_of_tensor_action);
		cout << endl;
	}

	for (i = 0; i < dimension_of_tensor_action; i++) {
		j = induced_perm[i];
		v_out[j] = v_in[i];
	}

	if (f_v) {
		cout << "wreath_product::apply_permutation" << endl;

		cout << "i : in[i] : perm[i] : out[i] " << endl;
		for (i = 0; i < dimension_of_tensor_action; i++) {
			cout << setw(3) << i << " & " << setw(3) << v_in[i]
					<< " & " << setw(3) << induced_perm[i]
					<< " & " << setw(3) << v_out[i] << endl;
		}
	}
	if (f_v) {
		cout << "wreath_product::apply_permutation v_out=";
		INT_vec_print(cout, v_out, dimension_of_tensor_action);
		cout << endl;
	}
	if (f_v) {
		cout << "wreath_product::apply_permutation done" << endl;
	}
}

INT wreath_product::offset_i(INT f)
{
	return P->elt_size_INT + f * M->elt_size_INT;
}

void wreath_product::create_matrix(INT *Elt, INT *A, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, N;

	if (f_v) {
		cout << "wreath_product::create_matrix" << endl;
	}
	for (f = 0; f < nb_factors; f++) {
		if (f == 0) {
			INT_vec_copy(Elt + offset_i(f), A1,
					dimension_of_matrix_group * dimension_of_matrix_group);
			N = dimension_of_matrix_group;
		} else {
			F->Kronecker_product_square_but_arbitrary(
					A1, Elt + offset_i(f),
					mtx_size[f - 1], dimension_of_matrix_group,
					A2, N, 0 /* verbose_level */);
			INT_vec_copy(A2, A1, N * N);
		}
		if (f_v) {
			cout << "wreath_product::create_matrix "
					"after step " << f << ":" << endl;
			INT_matrix_print(A1, N, N);
		}
	}
	INT_vec_copy(A1, A,
			dimension_of_tensor_action * dimension_of_tensor_action);
	if (f_v) {
		cout << "wreath_product::create_matrix done" << endl;
	}
}

void wreath_product::element_pack(INT *Elt, UBYTE *elt)
{
	INT i, j, f;

	for (f = 0; f < nb_factors; f++) {
		elt[f] = (UBYTE) Elt[f];
	}
	for (f = 0; f < nb_factors; f++) {
		for (i = 0; i < dimension_of_matrix_group; i++) {
			for (j = 0; j < dimension_of_matrix_group; j++) {
				put_digit(elt, f, i, j,
						(Elt + offset_i(f))[i * dimension_of_matrix_group + j]);
			}
		}
	}
}

void wreath_product::element_unpack(UBYTE *elt, INT *Elt)
{
	INT i, j, f;
	INT *m;

	for (f = 0; f < nb_factors; f++) {
		Elt[f] = elt[f];
	}
	for (f = 0; f < nb_factors; f++) {
		m = Elt + offset_i(f);
		for (i = 0; i < dimension_of_matrix_group; i++) {
			for (j = 0; j < dimension_of_matrix_group; j++) {
				m[i * dimension_of_matrix_group + j] =
						get_digit(elt, f, i, j);
			}
		}
		M->GL_invert_internal(m, m + M->elt_size_INT_half,
				0 /*verbose_level - 2*/);
	}
}

void wreath_product::put_digit(UBYTE *elt, INT f, INT i, INT j, INT d)
{
	INT h0 = (int) (f * dimension_of_matrix_group * dimension_of_matrix_group +
			(i * dimension_of_matrix_group + j)) * bits_per_digit;
	INT h, h1, a;

	for (h = 0; h < bits_per_digit; h++) {
		h1 = h0 + h;

		if (d & 1) {
			a = 1;
		} else {
			a = 0;
		}
		bitvector_m_ii(elt + nb_factors, h1, a);
		d >>= 1;
	}
}

INT wreath_product::get_digit(UBYTE *elt, INT f, INT i, INT j)
{
	INT h0 = (int) (f * dimension_of_matrix_group * dimension_of_matrix_group +
			(i * dimension_of_matrix_group + j)) * bits_per_digit;
	INT h, h1, a, d;

	d = 0;
	for (h = bits_per_digit - 1; h >= 0; h--) {
		h1 = h0 + h;

		a = bitvector_s_i(elt + nb_factors, h1);
		d <<= 1;
		if (a) {
			d |= 1;
		}
	}
	return d;
}

void wreath_product::make_element_from_one_component(INT *Elt,
		INT f, INT *Elt_component)
{
		INT g;

		P->one(Elt);
		for (g = 0; g < nb_factors; g++) {
			if (g == f) {
				M->GL_copy(Elt_component, Elt + offset_i(g));
			} else {
				M->GL_one(Elt + offset_i(g));
			}
		}
}

void wreath_product::make_element_from_permutation(INT *Elt, INT *perm)
{
		INT f;

		for (f = 0; f < nb_factors; f++) {
			Elt[f] = perm[f];
		}
		for (f = 0; f < nb_factors; f++) {
			M->GL_one(Elt + offset_i(f));
		}
}

void wreath_product::make_element(INT *Elt, INT *data, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, offset;

	if (f_v) {
		cout << "wreath_product::make_element" << endl;
		}
	if (f_v) {
		cout << "wreath_product::make_element data:" << endl;
		INT_vec_print(cout, data, make_element_size);
		cout << endl;
	}
	for (f = 0; f < nb_factors; f++) {
		Elt[f] = data[f];
	}
	offset = nb_factors;
	for (f = 0; f < nb_factors; f++) {
		M->make_element(Elt + offset_i(f), data + offset,
				0 /* verbose_level */);
		offset += M->elt_size_INT_half;
	}
	if (f_v) {
		cout << "wreath_product::make_element created this element:" << endl;
		element_print_easy(Elt, cout);
	}
	if (f_v) {
		cout << "wreath_product::make_element done" << endl;
		}
}

void wreath_product::element_print_easy(INT *Elt, ostream &ost)
{
	INT f;

	ost << "begin element of wreath product: " << endl;
	ost << "[";
	for (f = 0; f < nb_factors; f++) {
		ost << Elt[f];
		if (f < nb_factors - 1) {
			ost << ", ";
		}
	}
	ost << "]" << endl;
	for (f = 0; f < nb_factors; f++) {
		ost << "factor " << f << ":" << endl;
		M->GL_print_easy(Elt + offset_i(f), ost);
	}
	ost << "end element of wreath product" << endl;
}

void wreath_product::compute_base_and_transversals(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, f, h;

	if (f_v) {
		cout << "wreath_product::compute_base_and_transversals" << endl;
		}
	base_length = 0;
	base_length += nb_factors;
	base_length += nb_factors * base_len_in_component;
	the_base = NEW_INT(base_length);

	h = 0;
	for (i = 0; i < nb_factors; i++, h++) {
		the_base[h] = i;
	}
	for (f = 0; f < nb_factors; f++) {
		for (i = 0; i < base_len_in_component; i++, h++) {
			the_base[h] = perm_offset_i[f] + base_for_component[i];
		}
	}
	if (h != base_length) {
		cout << "wreath_product::compute_base_and_transversals "
				"h != base_length (1)" << endl;
		exit(1);
	}
	the_transversal_length = NEW_INT(base_length);
	h = 0;
	for (i = 0; i < nb_factors; i++, h++) {
		the_transversal_length[h] = nb_factors - i;
	}
	for (f = 0; f < nb_factors; f++) {
		for (i = 0; i < base_len_in_component; i++, h++) {
			the_transversal_length[h] = tl_for_component[i];
		}
	}
	if (h != base_length) {
		cout << "wreath_product::compute_base_and_transversals "
				"h != base_length (2)" << endl;
		exit(1);
	}
	if (f_v) {
		cout << "wreath_product::compute_base_and_transversals done" << endl;
		}
}

void wreath_product::make_strong_generators_data(INT *&data,
		INT &size, INT &nb_gens, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT *GL_data;
	INT GL_size;
	INT GL_nb_gens;
	INT h, k, f, g;
	INT *dat;

	if (f_v) {
		cout << "wreath_product::make_strong_generators_data" << endl;
	}
	if (f_v) {
		cout << "wreath_product::make_strong_generators_data "
				"before strong_generators_for_general_linear_group" << endl;
	}
	strong_generators_for_general_linear_group(
		dimension_of_matrix_group, F,
		FALSE /*M->f_semilinear*/,
		GL_data, GL_size, GL_nb_gens,
		verbose_level - 1);
		// in GALOIS/projective.C
	if (f_v) {
		cout << "wreath_product::make_strong_generators_data "
				"after strong_generators_for_general_linear_group" << endl;
	}
	nb_gens = nb_factors - 1 + nb_factors * GL_nb_gens;
	size = nb_factors + nb_factors *
			dimension_of_matrix_group * dimension_of_matrix_group;
	data = NEW_INT(nb_gens * size);
	dat = NEW_INT(size);

	h = 0;
	// generators for the components:
	for (f = nb_factors - 1; f >= 0; f--) {
		for (g = 0; g < GL_nb_gens; g++) {
			perm_identity(dat, nb_factors);
			for (k = 0; k < nb_factors; k++) {
				if (k == f) {
					INT_vec_copy(GL_data + g * GL_size,
							dat + nb_factors + k * M->elt_size_INT_half,
							GL_size);
				} else {
					F->identity_matrix(
							dat + nb_factors + k * M->elt_size_INT_half,
							dimension_of_matrix_group);
				}
			}
			INT_vec_copy(dat, data + h * size, size);
			h++;
		}
	}
#if 1
	// create the elementary swap permutations:
	for (k = nb_factors - 2; k >= 0; k--) {
		perm_elementary_transposition(dat, nb_factors, k);
		for (f = 0; f < nb_factors; f++) {
			F->identity_matrix(dat + nb_factors + f * M->elt_size_INT_half,
					dimension_of_matrix_group);
		}
		INT_vec_copy(dat, data + h * size, size);
		h++;
	}
#endif
	if (h != nb_gens) {
		cout << "h != nb_gens" << endl;
		exit(1);
	}
	FREE_INT(dat);
	if (f_v) {
		cout << "wreath_product::make_strong_generators_data done" << endl;
	}
}
