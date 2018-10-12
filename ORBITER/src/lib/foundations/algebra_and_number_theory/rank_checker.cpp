// rank_checker.C
//
// Anton Betten
//
// moved here from projective:  May 10, 2009




#include "foundations.h"

rank_checker::rank_checker()
{
	M1 = NULL;
	M2 = NULL;
	base_cols = NULL;
	set = NULL;
}

rank_checker::~rank_checker()
{
	//cout << "in ~rank_checker()" << endl;
	if (M1)
		FREE_int(M1);
	if (M2)
		FREE_int(M2);
	if (base_cols)
		FREE_int(base_cols);
	if (set)
		FREE_int(set);
	//cout << "~rank_checker() finished" << endl;
}

void rank_checker::init(finite_field *GFq, int m, int n, int d)
{
	rank_checker::GFq = GFq;
	rank_checker::m = m;
	rank_checker::n = n;
	rank_checker::d = d;
	M1 = NEW_int(m * n);
	M2 = NEW_int(m * n);
	base_cols = NEW_int(n);
	set = NEW_int(n);
}

int rank_checker::check_rank(int len, int *S, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int i, j, aj, rk, f_OK = TRUE;
	
	if (f_v) {
		cout << "rank_checker::check_rank: checking the set ";
		print_set(cout, len, S);
		cout << endl;
		}
	// M1 will be used as a m x len matrix
	for (j = 0; j < len; j++) {
		PG_element_unrank_modified(*GFq,
				M1 + j, len /* stride */, m /* len */, S[j]);
		}
	if (f_vv) {
		cout << "\n";
		//print_integer_matrix(cout, gen.S, 1, len);
		print_integer_matrix(cout, M1, m, len);
		}
	if (len <= 1)
		return TRUE;
	if (d <= 1)
		return TRUE;
	int d1 = MINIMUM(d - 2, len  - 1);
	if (f_vv) {
		cout << "d1=" << d1 << endl;
		}


	// M2 will be used as a m x (d1 + 1) matrix	
	
	first_k_subset(set, len - 1, d1);
	while (TRUE) {
	
		// get the subset of columns:
		if (f_vv) {
			cout << "subset: ";
			print_set(cout, d1, set);
			cout << endl;
			}
		
		for (j = 0; j < d1; j++) {
			aj = set[j];
			for (i = 0; i < m; i++) {
				M2[i * (d1 + 1) + j] = M1[i * len + aj];
				}
			}
		for (i = 0; i < m; i++) {
			M2[i * (d1 + 1) + d1] = M1[i * len + len - 1];
			}
		if (FALSE) {
			print_integer_matrix(cout, M2, m, d1 + 1);
			}
		
		rk = GFq->Gauss_int(M2,
			FALSE /* f_special */,
			FALSE /* f_complete */,
			base_cols,
			FALSE /* f_P */, NULL,
			m /* m */, d1 + 1 /* n */, 0 /* Pn */,
			0 /* verbose_level */);
		if (rk <= d1) {
			f_OK = FALSE;
			if (f_v) {
				cout << "not OK; subset: ";
				print_set(cout, d1, set);
				cout << " leads to a rk " << rk << " submatrix" << endl;
				}
			break;
			}
		if (!next_k_subset(set, len - 1, d1))
			break;
		}
	if (!f_OK)
		return FALSE;
	return TRUE;
}

int rank_checker::check_rank_matrix_input(
		int len, int *S, int dim_S, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int i, j, aj, rk, f_OK = TRUE;
	
	// S is a m x len matrix
	if (len <= 1)
		return TRUE;
	if (d <= 1)
		return TRUE;
	int d1 = MINIMUM(d - 2, len  - 1);
	if (f_vv) {
		cout << "d1=" << d1 << endl;
		}


	// M2 will be used as a m x (d1 + 1) matrix	
	
	first_k_subset(set, len - 1, d1);
	while (TRUE) {
	
		// get the subset of columns:
		if (f_vv) {
			cout << "subset: ";
			print_set(cout, d1, set);
			cout << endl;
			}
		
		for (j = 0; j < d1; j++) {
			aj = set[j];
			for (i = 0; i < m; i++) {
				M2[i * (d1 + 1) + j] = S[i * dim_S + aj];
				}
			}
		for (i = 0; i < m; i++) {
			M2[i * (d1 + 1) + d1] = S[i * dim_S + len - 1];
			}
		
		rk = GFq->Gauss_int(M2,
			FALSE /* f_special */,
			FALSE /* f_complete */,
			base_cols,
			FALSE /* f_P */, NULL,
			m /* m */, d1 + 1 /* n */, 0 /* Pn */,
			0 /* verbose_level */);
		if (rk <= d1) {
			f_OK = FALSE;
			if (f_v) {
				cout << "not OK; subset: ";
				print_set(cout, d1, set);
				cout << " leads to a rk " << rk
						<< " submatrix, but we want rank "
						<< d1 + 1 << endl;
				}
			break;
			}
		if (!next_k_subset(set, len - 1, d1))
			break;
		}
	if (!f_OK)
		return FALSE;
	return TRUE;
}

int rank_checker::check_rank_last_two_are_fixed(
		int len, int *S, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int i, j, aj, rk, f_OK = TRUE;
	
	if (f_v) {
		cout << "rank_checker::check_rank_last_two_are_fixed: "
				"checking the set ";
		print_set(cout, len, S);
		cout << endl;
		}
	// M1 will be used as a m x len matrix
	for (j = 0; j < len; j++) {
		PG_element_unrank_modified(*GFq,
				M1 + j, len /* stride */, m /* len */, S[j]);
		}
	if (f_vv) {
		cout << "\n";
		//print_integer_matrix(cout, gen.S, 1, len);
		print_integer_matrix(cout, M1, m, len);
		}
	if (len <= 1)
		return TRUE;
	if (d <= 2)
		return TRUE;
	int d1 = MINIMUM(d - 3, len  - 2);
	if (f_vv) {
		cout << "d1=" << d1 << endl;
		}


	// M2 will be used as a m x (d1 + 2) matrix	
	
	first_k_subset(set, len - 2, d1);
	while (TRUE) {
	
		// get the subset of columns:
		if (f_vv) {
			cout << "subset: ";
			print_set(cout, d1, set);
			cout << endl;
			}
		
		for (j = 0; j < d1; j++) {
			aj = set[j];
			for (i = 0; i < m; i++) {
				M2[i * (d1 + 2) + j] = M1[i * len + aj];
				}
			}
		for (i = 0; i < m; i++) {
			M2[i * (d1 + 2) + d1] = M1[i * len + len - 2];
			M2[i * (d1 + 2) + d1 + 1] = M1[i * len + len - 1];
			}
		if (FALSE) {
			print_integer_matrix(cout, M2, m, d1 + 2);
			}
		
		rk = GFq->Gauss_int(M2,
			FALSE /* f_special */,
			FALSE /* f_complete */,
			base_cols,
			FALSE /* f_P */, NULL,
			m /* m */, d1 + 2 /* n */, 0 /* Pn */,
			0 /* verbose_level */);
		if (rk <= d1 + 1) {
			f_OK = FALSE;
			if (f_v) {
				cout << "not OK; subset: ";
				print_set(cout, d1, set);
				cout << " leads to a rk " << rk << " submatrix" << endl;
				}
			break;
			}
		if (!next_k_subset(set, len - 2, d1))
			break;
		}
	if (!f_OK)
		return FALSE;
	if (f_v)
		cout << "is OK" << endl;
	return TRUE;
}

int rank_checker::compute_rank_row_vectors(
		int len, int *S, int f_projective, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int j, rk;
	
	if (f_vv) {
		cout << "rank_checker::compute_rank_row_vectors set ";
		print_set(cout, len, S);
		cout << endl;
		}
	// M1 will be used as a len x n matrix
	for (j = 0; j < len; j++) {
		if (f_projective) {
			PG_element_unrank_modified(*GFq,
					M1 + j * n, 1 /* stride */, n /* len */, S[j]);
			}
		else {
			AG_element_unrank(GFq->q, M1 + j * n, 1, n, S[j]);
			}
		}
	if (f_v) {
		cout << "\n";
		//print_integer_matrix(cout, gen.S, 1, len);
		print_integer_matrix(cout, M1, len, n);
		}

		
	rk = GFq->Gauss_int(M1,
		FALSE /* f_special */,
		FALSE /* f_complete */,
		base_cols,
		FALSE /* f_P */, NULL,
		len /* m */, n /* n */, 0 /* Pn */,
		0 /* verbose_level */);

	return rk;
}

