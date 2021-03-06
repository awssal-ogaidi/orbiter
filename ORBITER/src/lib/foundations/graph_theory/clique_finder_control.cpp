// clique_finder_control.C
//
// Anton Betten
//
// started:  October 11, 2018




#include "foundations.h"

clique_finder_control::clique_finder_control()
{
	f_rainbow = FALSE;
	f_weighted = FALSE;
	weights_string = NULL;
	f_file = FALSE;
	fname_graph = NULL;
	f_nonrecursive = FALSE;
	f_output_solution_raw = FALSE;
	f_output_file = FALSE;
	output_file = NULL;
	f_maxdepth = FALSE;
	maxdepth = 0;
	f_restrictions = FALSE;
	nb_restrictions = 0;
	f_tree = FALSE;
	f_decision_nodes_only = FALSE;
	fname_tree = NULL;
	print_interval = 1;
	nb_search_steps = 0;
	nb_decision_steps = 0;
	nb_sol = 0;
	dt = 0;
}

clique_finder_control::~clique_finder_control()
{
}

int clique_finder_control::parse_arguments(
		int argc, const char **argv)
{
	int i;

	cout << "clique_finder_control::parse_arguments" << endl;
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			fname_graph = argv[++i];
			cout << "-file " << fname_graph << endl;
		}
		else if (strcmp(argv[i], "-rainbow") == 0) {
			f_rainbow = TRUE;
			cout << "-rainbow " << endl;
		}
		else if (strcmp(argv[i], "-weighted") == 0) {
			f_weighted = TRUE;
			weights_string = argv[++i];
			cout << "-weighted " << weights_string << endl;
		}
		else if (strcmp(argv[i], "-nonrecursive") == 0) {
			f_nonrecursive = TRUE;
			cout << "-nonrecursive " << endl;
		}
		else if (strcmp(argv[i], "-tree") == 0) {
			f_tree = TRUE;
			f_decision_nodes_only = FALSE;
			fname_tree = argv[++i];
			cout << "-tree " << fname_tree << endl;
		}
		else if (strcmp(argv[i], "-tree_decision_nodes_only") == 0) {
			f_tree = TRUE;
			f_decision_nodes_only = TRUE;
			fname_tree = argv[++i];
			cout << "-tree_decision_nodes_only " << fname_tree << endl;
		}
		else if (strcmp(argv[i], "-output_file") == 0) {
			f_output_file = TRUE;
			output_file = argv[++i];
			cout << "-output_file " << output_file << endl;
		}
		else if (strcmp(argv[i], "-output_solution_raw") == 0) {
			f_output_solution_raw = TRUE;
			cout << "-output_solution_raw " << endl;
		}
		else if (strcmp(argv[i], "-restrictions") == 0) {
			f_restrictions = TRUE;
			int j;
			for (j = 0; TRUE; j++) {
				restrictions[j] = atoi(argv[++i]);
				if (restrictions[j] == -1) {
					nb_restrictions = j / 3;
					break;
					}
				if (nb_restrictions >= CLIQUE_FINDER_CONTROL_MAX_RESTRICTIONS) {
					cout << "clique_finder_control::parse_arguments "
							"restrictions must end in -1" << endl;
					exit(1);
				}
			}
			cout << "-restrictions ";
			int_vec_print(cout, restrictions, 3 * nb_restrictions);
			cout << endl;
			}
		else if (strcmp(argv[i], "-end") == 0) {
			cout << "-end" << endl;
			return i;
		}
		else {
			cout << "clique_finder_control::parse_arguments "
					"unrecognized option " << argv[i] << endl;
		}
	}
	cout << "clique_finder_control::parse_arguments "
			"did not see -end option" << endl;
	exit(1);
}


void clique_finder_control::all_cliques(
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	colored_graph CG;
	char fname_sol[1000];

	if (f_v) {
		cout << "clique_finder_control::all_cliques" << endl;
		}
	CG.load(fname_graph, verbose_level - 1);
	if (f_output_file) {
		sprintf(fname_sol, "%s", output_file);
		}
	else {
		sprintf(fname_sol, "%s_sol.txt", CG.fname_base);
		}

	//CG.print();

	{
	ofstream fp(fname_sol);


	if (f_rainbow) {
		if (f_weighted) {

			if (f_v) {
				cout << "clique_finder_control::all_cliques "
						"weighted cliques" << endl;
			}

			all_cliques_weighted(&CG, fname_sol, verbose_level);




		} else {
			if (f_v) {
				cout << "clique_finder_control::all_cliques "
						"before CG.all_rainbow_cliques" << endl;
				}
			CG.all_rainbow_cliques(&fp,
				f_output_solution_raw,
				f_maxdepth, maxdepth,
				f_restrictions, restrictions,
				f_tree, f_decision_nodes_only, fname_tree,
				print_interval,
				nb_search_steps, nb_decision_steps, nb_sol, dt,
				verbose_level - 1);
			if (f_v) {
				cout << "clique_finder_control::all_cliques "
						"after CG.all_rainbow_cliques" << endl;
			}
		}
	} else {
		cout << "clique_finder_control::all_cliques !f_rainbow" << endl;
		exit(1);
	}
	fp << -1 << " " << nb_sol << " " << nb_search_steps
		<< " " << nb_decision_steps << " " << dt << endl;
	}
	if (f_v) {
		cout << "clique_finder_control::all_cliques done" << endl;
		}
}

void clique_finder_control::all_cliques_weighted(colored_graph *CG,
	const char *fname_sol,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "clique_finder_control::all_cliques_weighted" << endl;
		}

	int *weights;
	int nb_weights;
	int target_value;
	int i;


	int_vec_scan(weights_string, weights, nb_weights);

	if (CG->nb_colors + 1 != nb_weights) {
		cout << "CG.nb_colors + 1 != nb_weights" << endl;
		exit(1);
	}
	target_value = weights[0];

	for (i = 1; i < nb_weights; i++) {
		weights[i - 1] = weights[i];
	}
	nb_weights--;

	cout << "target_value = " << target_value << endl;
	cout << "the weights are ";
	int_vec_print(cout, weights, nb_weights);
	cout << endl;

	if (nb_weights != 2) {
		cout << "clique_finder_control::all_cliques_weighted "
				"nb_weights != 2" << endl;
		exit(1);
	}

	diophant D;
	int nb_backtrack_nodes;
	int nb_sol;
	int *Sol_weights;
	int j;
	vector<int> res;

	D.init_partition_problem(
			weights, nb_weights, target_value,
			verbose_level);
	D.solve_mckay("weights", INT_MAX /* maxresults */,
			nb_backtrack_nodes, nb_sol, verbose_level);
	cout << "we found " << nb_sol << " solutions for the "
			"weight distribution" << endl;
	Sol_weights = NEW_int(nb_sol * nb_weights);
	for (i = 0; i < D._resultanz; i++) {
		res = D._results.front();
		for (j = 0; j < nb_weights; j++) {
			Sol_weights[i * nb_weights + j] = res[j];
			}
		D._results.pop_front();
		}
	cout << "The solutions are:" << endl;
	for (i = 0; i < nb_sol; i++) {
		cout << i << " : ";
		int_vec_print(cout, Sol_weights + i * nb_weights, nb_weights);
		cout << endl;
	}

	int c1 = 0;
	int c2 = 1;

	cout << "creating subgraph of color " << c1 << ":" << endl;
	colored_graph *subgraph;
	subgraph = CG->subgraph_by_color_classes(
			c1, verbose_level);

	cout << "The subgraph has size " << subgraph->nb_points << endl;

	int target_depth1;
	int target_depth2;
	int nb_cliques_in_subgraph;
	int decision_step_counter;
	int nb_solutions_total;
	int *Sol;

	nb_solutions_total = 0;
	for (i = 0; i < nb_sol; i++) {
		target_depth1 = Sol_weights[i * nb_weights + c1];
		target_depth2 = Sol_weights[i * nb_weights + c2];
		subgraph->all_cliques_of_size_k_ignore_colors(target_depth1,
				Sol, nb_cliques_in_subgraph, decision_step_counter, verbose_level);
		cout << "solution " << i << " with target_depth = " << target_depth1
				<< " nb_cliques_in_subgraph=" << nb_cliques_in_subgraph << endl;

		for (j = 0; j < nb_cliques_in_subgraph; j++) {
			colored_graph *subgraph2;
			int nb_cliques_in_subgraph2;
			int *Sol2;
			cout << "clique1 " << j << " / " << nb_cliques_in_subgraph << ":" << endl;
			subgraph2 = CG->subgraph_by_color_classes_with_condition(
						Sol + j * target_depth1, target_depth1,
						c2, verbose_level);
			cout << "subgraph2 has " << subgraph2->nb_points << " vertices" << endl;
			subgraph2->all_cliques_of_size_k_ignore_colors(target_depth2,
					Sol2, nb_cliques_in_subgraph2, decision_step_counter, verbose_level);
			nb_solutions_total += nb_cliques_in_subgraph2;
			cout << "nb_cliques_in_subgraph2=" << nb_cliques_in_subgraph2
					<< " nb_solutions_total=" << nb_solutions_total << endl;
			FREE_int(Sol2);
			delete subgraph2;
		}
		FREE_int(Sol);
	}
	cout << "nb_solutions_total=" << nb_solutions_total << endl;
	FREE_int(Sol_weights);

	if (f_v) {
		cout << "clique_finder_control::all_cliques_weighted done" << endl;
		}
}
