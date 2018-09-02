// poset_of_subsets.C
// 
// Anton Betten
// July 2, 2016

#include "orbiter.h"

void make_subset_lattice(layered_graph *&LG, INT n, INT f_tree,  
	INT f_depth_first, INT f_breadth_first, INT verbose_level);


int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_n = FALSE;
	INT n;
	INT f_tree = FALSE;
	INT f_depth_first = FALSE;
	INT f_breadth_first = FALSE;

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
		else if (strcmp(argv[i], "-depth_first") == 0) {
			f_depth_first = TRUE;
			cout << "-depth_first " << endl;
			}
		else if (strcmp(argv[i], "-breadth_first") == 0) {
			f_breadth_first = TRUE;
			cout << "-breadth_first " << endl;
			}
		else if (strcmp(argv[i], "-tree") == 0) {
			f_tree = TRUE;
			cout << "-tree " << endl;
			}
		}

	if (!f_n) {
		cout << "Please use option -n <n>" << endl;
		exit(1);
		}

	layered_graph *LG;
	char fname[1000];

	sprintf(fname, "poset_of_subsets_%ld", n);
	if (f_tree) {
		sprintf(fname + strlen(fname), "_tree");
		}
	sprintf(fname + strlen(fname), ".layered_graph");
	
	make_subset_lattice(LG, n, f_tree, 
		f_depth_first, f_breadth_first, verbose_level);
	
	LG->write_file(fname, 0 /*verbose_level*/);

	delete LG;
}

void make_subset_lattice(layered_graph *&LG, INT n, INT f_tree,  
	INT f_depth_first, INT f_breadth_first, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT nb_layers = n + 1;
	INT *Nb;
	INT i, k, r, a, b, r0;
	INT *set1;
	INT *set2;

	if (f_v) {
		cout << "make_subset_lattice n=" << n << endl;
		}

	Nb = NEW_INT(nb_layers);
	for (i = 0; i <= n; i++) {
		Nb[i] = INT_n_choose_k(n, i);
		}

	set1 = NEW_INT(n);
	set2 = NEW_INT(n);

	LG = NEW_OBJECT(layered_graph);
	if (f_vv) {
		cout << "generator::make_graph before LG->init" << endl;
		}
	LG->add_data1(0, 0/*verbose_level*/);
	LG->init(nb_layers, Nb, "", verbose_level);
	if (f_vv) {
		cout << "generator::make_graph after LG->init" << endl;
		}
	LG->place(verbose_level);
	if (f_vv) {
		cout << "generator::make_graph after LG->place" << endl;
		}

	// create vertex labels:
	for (k = 0; k <= n; k++) {
		for (r = 0; r < Nb[k]; r++) {
			unrank_k_subset(r, set1, n, k);
			LG->add_node_data1(k, r, set1[k - 1], 0/*verbose_level*/);

			char text[1000];
			INT a, j, j0;
			if (f_depth_first) {
				cout << "k=" << k << " r=" << r << " set=";
				INT_vec_print(cout, set1, k);
				cout << endl;
				a = 0;
				for (i = k - 1; i >= 0; i--) {
					if (i) {
						j0 = set1[i - 1];
						}
					else {
						j0 = -1;
						}
					cout << "i=" << i << " set1[i]=" << set1[i] << endl;
					for (j = j0 + 1; j < set1[i]; j++) {
						cout << "i = " << i << " j=" << j << " adding " << i_power_j(2, n - j - 1) << endl;
						a += i_power_j(2, n - j - 1);
						}
					}
				a += k;
				sprintf(text, "%ld", a);
				}
			else if (f_breadth_first) {
				a = 0;
				for (i = 0; i < k; i++) {
					a += Nb[i];
					}
				a += r;
				sprintf(text, "%ld", a);
				}
			else {
				if (k) {
					sprintf(text, "%ld", set1[k - 1]);
					}
				else {
					text[0] = 0;
					}
				}
			LG->add_text(k, r, text, 0/*verbose_level*/);
			}
		}

	// create edges:
	for (k = 1; k <= n; k++) {
		for (r = 0; r < Nb[k]; r++) {
			unrank_k_subset(r, set1, n, k);

			if (f_tree) {
				for (a = k - 1; a >= k - 1; a--) {
					INT_vec_copy(set1, set2, k);
					for (b = a; b < k - 1; b++) {
						set2[b] = set2[b + 1];
						}
					r0 = rank_k_subset(set2, n, k - 1);
					LG->add_edge(k - 1, r0, k, r, 0 /*verbose_level*/);
					}
				}
			else {
				for (a = k - 1; a >= 0; a--) {
					INT_vec_copy(set1, set2, k);
					for (b = a; b < k - 1; b++) {
						set2[b] = set2[b + 1];
						}
					r0 = rank_k_subset(set2, n, k - 1);
					LG->add_edge(k - 1, r0, k, r, 0 /*verbose_level*/);
					}
				}
			}
		}

	
	FREE_INT(set1);
	FREE_INT(set2);
	if (f_v) {
		cout << "make_subset_lattice done" << endl;
		}
}

