// make_grassmannian.C
// 
// Anton Betten
// 4/18/2011
//
// 
//
//
//

#include "orbiter.h"





// global data:

int t0; // the system time when the program started


void create_grassmannian(int n, int k, finite_field *F, int verbose_level);


int main(int argc, char **argv)
{
	int verbose_level = 0;
	int i;
	int f_n = FALSE;
	int n;
	int f_k = FALSE;
	int k;
	int f_q = FALSE;
	int q;
	int f_poly = FALSE;
	char *poly = NULL;
	
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
		else if (strcmp(argv[i], "-k") == 0) {
			f_k = TRUE;
			k = atoi(argv[++i]);
			cout << "-k " << k << endl;
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
		}
	if (!f_n) {
		cout << "please use -n option" << endl;
		exit(1);
		}
	if (!f_k) {
		cout << "please use -k option" << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "please use -q option" << endl;
		exit(1);
		}
	
	finite_field *F;

	F = NEW_OBJECT(finite_field);
	F->init_override_polynomial(q, poly, 0);
	create_grassmannian(n, k, F, verbose_level);

	the_end(t0);
}

void create_grassmannian(int n, int k, finite_field *F, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	projective_space *P;
	projective_space *P2;
	grassmann *Grass;
	int nk, rk, rk1, det, d;
	int N, i, j, h, col;
	int *v, *B, *A, *L, *subset;
	int q = F->q;

	d = n + 1;
	nk = int_n_choose_k(d, k + 1);
	P = NEW_OBJECT(projective_space);
	P2 = NEW_OBJECT(projective_space);
	B = NEW_int((k + 1) * d);
	v = NEW_int(nk);
	A = NEW_int((k + 1) * (k + 1));
	subset = NEW_int(k + 1);

	if (f_v) {
		cout << "nk=" << nk << endl;
		}

	P->init(n, F, 
		TRUE /* f_init_incidence_structure */, 
		verbose_level);

	Grass = NEW_OBJECT(grassmann);

	if (f_v) {
		cout << "before Grass->init" << endl;
		}

	Grass->init(d, k + 1, F, verbose_level);

	if (f_v) {
		cout << "after Grass->init" << endl;
		}
	
	P2->init(nk - 1, F, 
		TRUE /* f_init_incidence_structure */, 
		verbose_level);

	if (f_v) {
		cout << "after P2->init" << endl;
		cout << "P2->N_points=" << P2->N_points << endl;
		}




	longinteger_domain D;
	longinteger_object aa;

	D.q_binomial(aa, d, k + 1, q, 0);
	N = aa.as_int();

	if (f_v) {
		cout << "N = " << N << endl;
		}
	L = NEW_int(N);
	for (rk = 0; rk < N; rk++) {
		if (f_v) {
			cout << "rk=" << rk << endl;
			}
		Grass->unrank_int(rk, 0);
		for (i = 0; i < (k + 1) * d; i++) {
			B[i] = Grass->M[i];
			}
		if (f_v) {
			print_integer_matrix_width(cout, B, k + 1, d, d, 2);
			}
		for (h = 0; h < nk; h++) {

			if (f_v) {
				cout << "subset " << h << endl;
				}
			unrank_k_subset(h, subset, d, k + 1);
			if (f_v) {
				int_vec_print(cout, subset, k + 1);
				cout << endl;
				}
			for (j = 0; j < k + 1; j++) {
				col = subset[j];
				for (i = 0; i < k + 1; i++) {
					A[i * (k + 1) + j] = B[i * d + col];
					}
				}
			if (f_v) {
				print_integer_matrix_width(cout, A, k + 1, k + 1, k + 1, 2);
				}
			rk1 = F->Gauss_easy(A, k + 1, k + 1);
			if (rk1 < k + 1) {
				det = 0;
				}
			else {
				det = 1;
				for (j = 0; j < k + 1; j++) {
					det = F->mult(det, A[j * (k + 1) + j]);
					}
				}
			v[h] = det;
			} // next h
		j = P2->rank_point(v);
		L[rk] = j;
		if (f_v) {
			cout << setw(4) << rk << " : ";
			int_vec_print(cout, B, (k + 1) * n);
			cout << " : ";
			int_vec_print(cout, v, nk);
			cout << " : ";
			cout << setw(5) << j << endl;
			}
		}
#if 0
	N = q + 1;

	v = NEW_int(n + 1);
	L = NEW_int(P->N_points);

	if (f_v) {
		cout << "i : point : projective rank" << endl;
		}
	for (i = 0; i < N; i++) {
		PG_element_unrank_modified(*F, v2, 1, 2, i);
		s = v2[0];
		t = v2[1];
		v[0] = F->mult(F->power(s, 3), F->power(t, 0));
		v[1] = F->mult(F->power(s, 2), F->power(t, 1));
		v[2] = F->mult(F->power(s, 1), F->power(t, 2));
		v[3] = F->mult(F->power(s, 0), F->power(t, 3));
		j = P->rank_point(v);
		L[i] = j;
		if (f_v) {
			cout << setw(4) << i << " : ";
			int_vec_print(cout, v, d);
			cout << " : " << setw(5) << j << endl;
			}
		}
#endif


	cout << "list of points on G(" << k << "," << n << "):" << endl;
	cout << N << endl;
	for (i = 0; i < N; i++) {
		cout << L[i] << " ";
		}
	cout << endl;

	char fname[1000];
	sprintf(fname, "G_%d_%d_%d.txt", n, k, q);
	write_set_to_file(fname, L, N, verbose_level);


	FREE_OBJECT(P);
	FREE_OBJECT(P2);
	FREE_OBJECT(Grass);
	FREE_int(B);
	FREE_int(L);
	FREE_int(v);
	FREE_int(A);
	FREE_int(subset);
}


