// dlx.C
//
// Anton Betten
// April 7, 2013
//

#include "orbiter.h"


int t0;



int main(int argc, const char **argv)
{
	int i;
	int verbose_level = 0;
	int f_file = FALSE;
	const char *fname;
	int f_solution_file = FALSE;
	const char *solution_fname = NULL;
	int f_tree_file = FALSE;
	const char *tree_fname = NULL;


	t0 = os_ticks();

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			fname = argv[++i];
			cout << "-file " << fname << endl;
			}
		else if (strcmp(argv[i], "-solution_file") == 0) {
			f_solution_file = TRUE;
			solution_fname = argv[++i];
			cout << "-solution_file " << solution_fname << endl;
			}
		else if (strcmp(argv[i], "-tree_file") == 0) {
			f_tree_file = TRUE;
			tree_fname = argv[++i];
			cout << "-tree_file " << tree_fname << endl;
			}
		}
	
	if (!f_file) {
		cout << "Please use option -file <fname>" << endl;
		exit(1);
		}
	int *M;
	int m, n;
	int j, d, h;
	int nb_sol, nb_backtrack;
	{
		ifstream fp(fname);

		fp >> m >> n;

		cout << "Reading input of size " << m << " x " << n << endl;
		M = NEW_int(m * n);
		for (i = 0; i < m * n; i++) {
			M[i] = 0;
			}
		for (i = 0; i < m; i++) {
			fp >> d;
			for (h = 0; h < d; h++) {
				fp >> j;
				M[i * n + j] = 1;
				}
			}
	}
	cout << "Read input matrix of size " << m << " x " << n << endl;
	cout << "Solving..." << endl;
	DlxTransposeAppendAndSolve(M, m, n, nb_sol, nb_backtrack, 
		f_solution_file, solution_fname, 
		f_tree_file, tree_fname, 
		0);
	cout << "after solve, we found " << nb_sol << " solutions in "
		<< nb_backtrack << " steps" << endl;


	the_end_quietly(t0);
}

	

