// exact_cover.C
// 
// Anton Betten
//
// started:    April 30 2013
// 
//
//

#include "orbiter.h"


exact_cover::exact_cover()
{
	null();
}

exact_cover::~exact_cover()
{
	freeself();
}

void exact_cover::null()
{
	starter = NULL;
	f_has_solution_test_func = FALSE;
	f_has_late_cleanup_function = FALSE;
	late_cleanup_function = NULL;

	prepare_function_new = NULL;
	early_test_func = NULL;
	early_test_func_data = NULL;

	f_randomized = FALSE;
	random_permutation_fname = NULL;
	random_permutation = NULL;
}

void exact_cover::freeself()
{
	if (starter) {
		FREE_int(starter);
		}
	if (random_permutation) {
		FREE_int(random_permutation);
		}
	null();
}

void exact_cover::init_basic(void *user_data, 
	action *A_base, action *A_on_blocks, 
	int target_size, int starter_size, 
	const char *input_prefix, const char *output_prefix, 
	const char *solution_prefix, const char *base_fname, 
	int f_lex, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 3);

	if (f_v) {
		cout << "exact_cover::init_basic" << endl;
		}

	if (f_vv) {
		cout << "exact_cover::init_basic input_prefix=" << input_prefix << endl;
		cout << "exact_cover::init_basic output_prefix=" << output_prefix << endl;
		cout << "exact_cover::init_basic solution_prefix=" << solution_prefix << endl;
		cout << "exact_cover::init_basic base_fname=" << base_fname << endl;
		cout << "exact_cover::init_basic target_size=" << target_size << endl;
		cout << "exact_cover::init_basic starter_size=" << starter_size << endl;
		cout << "exact_cover::init_basic f_lex=" << f_lex << endl;
		}
	exact_cover::user_data = user_data;
	exact_cover::A_base = A_base;
	exact_cover::A_on_blocks = A_on_blocks;
	exact_cover::target_size = target_size;
	exact_cover::starter_size = starter_size;
	exact_cover::f_lex = f_lex;
	f_split = FALSE;
	f_single_case = FALSE;
	strcpy(exact_cover::input_prefix, input_prefix);
	strcpy(exact_cover::output_prefix, output_prefix);
	strcpy(exact_cover::solution_prefix, solution_prefix);
	strcpy(exact_cover::base_fname, base_fname);

	char fname[1000];

	sprintf(fname, "%s%s_lvl_%d", input_prefix, base_fname, starter_size);
	if (f_v) {
		cout << "exact_cover::init_basic counting number of orbits from file " << fname << endl;
		}
	if (file_size(fname) <= 0) {
		cout << "exact_cover::init_basic the file " << fname << " does not exist" << endl;
		exit(1);
		}
	starter_nb_cases = count_number_of_orbits_in_file(fname, verbose_level + 2);
	if (f_v) {
		cout << "exact_cover::init_basic starter_nb_cases = " << starter_nb_cases << endl;
		}

	sprintf(fname_solutions, "%s%s_depth_%d_solutions.txt", solution_prefix, base_fname, starter_size);
	sprintf(fname_statistics, "%s%s_depth_%d_statistics.csv", solution_prefix, base_fname, starter_size);

	if (f_vv) {
		cout << "exact_cover::init_basic fname_solutions = " << fname_solutions << endl;
		cout << "exact_cover::init_basic fname_statistics = " << fname_statistics << endl;
		}
	starter = NEW_int(starter_size + 1);

	if (f_v) {
		cout << "exact_cover::init_basic done" << endl;
		}
}

void exact_cover::init_early_test_func(
	void (*early_test_func)(int *S, int len, 
		int *candidates, int nb_candidates, 
		int *good_candidates, int &nb_good_candidates, 
		void *data, int verbose_level), 
	void *early_test_func_data,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "exact_cover::init_early_test_func" << endl;
		}
	exact_cover::early_test_func = early_test_func;
	exact_cover::early_test_func_data = early_test_func_data;
}

void exact_cover::init_prepare_function_new(
	void (*prepare_function_new)(exact_cover *E, int starter_case, 
		int *candidates, int nb_candidates, strong_generators *Strong_gens, 
		diophant *&Dio, int *&col_label, 
		int &f_ruled_out, 
		int verbose_level),
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "exact_cover::init_prepare_function_new" << endl;
		}
	exact_cover::prepare_function_new = prepare_function_new;
}

void exact_cover::set_split(int split_r, int split_m, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	exact_cover::f_split = TRUE;
	exact_cover::split_r = split_r;
	exact_cover::split_m = split_m;
	sprintf(fname_solutions, "%s%s_depth_%d_split_%d_%d_solutions.txt", solution_prefix, base_fname, starter_size, split_r, split_m);
	sprintf(fname_statistics, "%s%s_depth_%d_split_%d_%d_statistics.csv", solution_prefix, base_fname, starter_size, split_r, split_m);
	if (f_v) {
		cout << "exact_cover::set_split fname_solutions = " << fname_solutions << endl;
		cout << "exact_cover::set_split fname_statistics = " << fname_statistics << endl;
		}
}

void exact_cover::set_single_case(int single_case, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	exact_cover::f_single_case = TRUE;
	exact_cover::single_case = single_case;
	sprintf(fname_solutions, "%s%s_depth_%d_case_%d_solutions.txt", solution_prefix, base_fname, starter_size, single_case);
	sprintf(fname_statistics, "%s%s_depth_%d_case_%d_statistics.csv", solution_prefix, base_fname, starter_size, single_case);
	if (f_v) {
		cout << "exact_cover::set_single_case fname_solutions = " << fname_solutions << endl;
		cout << "exact_cover::set_single_case fname_statistics = " << fname_statistics << endl;
		}
}

void exact_cover::randomize(const char *random_permutation_fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "exact_cover::randomize" << endl;
		}
	int m, n;
	
	f_randomized = TRUE;
	exact_cover::random_permutation_fname = random_permutation_fname;
	int_matrix_read_csv(random_permutation_fname, random_permutation, m, n, verbose_level);
	if (n != 1) {
		cout << "exact_cover::randomize after int_matrix_read_csv n != n" << endl;
		exit(1);
		}
	if (m != starter_nb_cases) {
		cout << "exact_cover::randomize int_matrix_read_csv m != starter_nb_cases" << endl;
		exit(1);
		}
	if (f_v) {
		cout << "exact_cover::randomize read the random permutation of degree " << m << " from file " << random_permutation_fname << endl;
		}
}

void exact_cover::add_solution_test_function(
	int (*solution_test_func)(exact_cover *EC, int *S, int len, void *data, int verbose_level), 
	void *solution_test_func_data,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "exact_cover::add_solution_test_function" << endl;
		}	
	f_has_solution_test_func = TRUE;
	exact_cover::solution_test_func = solution_test_func;
	exact_cover::solution_test_func_data = solution_test_func_data;
}

void exact_cover::add_late_cleanup_function(
	void (*late_cleanup_function)(exact_cover *E, int starter_case, int verbose_level)
	)
{
	f_has_late_cleanup_function = TRUE;
	exact_cover::late_cleanup_function = late_cleanup_function;
}



void exact_cover::compute_liftings_new(int f_solve, int f_save, int f_read_instead, 
	int f_draw_system, const char *fname_system, 
	int f_write_tree, const char *fname_tree, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int f_v3 = (verbose_level >= 3);
	int Nb_sol_total;
	int *Case_nb;
	int *Nb_col;
	int *Nb_sol;
	int *Nb_backtrack;
	int *Dt;
	int *Dt_in_sec;
	int nb_cases;
	int total_solutions;
	int nb_deleted_solutions = 0;
	int starter_case;
	int the_starter_case;



	if (f_v) {
		cout << "exact_cover::compute_liftings_new" << endl;
		cout << "starter_size=" << starter_size << endl;
		cout << "f_lex=" << f_lex << endl;
		cout << "f_solve=" << f_solve << endl;
		cout << "f_save=" << f_save << endl;
		cout << "f_read_instead=" << f_read_instead << endl;
		cout << "starter_nb_cases=" << starter_nb_cases << endl;
		cout << "verbose_level=" << verbose_level << endl;
		}
	

	Nb_sol_total = 0;
	Nb_sol = 0;
	nb_cases = 0;
	Case_nb = NEW_int(starter_nb_cases);
	Nb_col = NEW_int(starter_nb_cases);
	Nb_sol = NEW_int(starter_nb_cases);
	Nb_backtrack = NEW_int(starter_nb_cases);
	Dt = NEW_int(starter_nb_cases);
	Dt_in_sec = NEW_int(starter_nb_cases);
	{
	ofstream fp(fname_solutions);
	int f_do_it;
	int nb_col, nb_sol, nb_sol_deleted, nb_backtrack, dt, sol_length;

	total_solutions = 0;

	for (starter_case = 0; starter_case < starter_nb_cases; starter_case++) {
		f_do_it = FALSE;

		if (f_split) {
			if ((starter_case % split_m) == split_r) {
				f_do_it = TRUE;
				}
			}
		else {
			f_do_it = TRUE;
			}

		if (!f_do_it) {
			continue;
			}


		if (f_randomized) {
			the_starter_case = random_permutation[starter_case];
			}
		else {
			the_starter_case = starter_case;
			}
		if (f_v) {
			cout << "exact_cover::compute_liftings_new starter_case " << starter_case << " / " << starter_nb_cases << " is case " << the_starter_case << endl;
			}
		nb_col = 0;
		nb_sol = 0;

		int *Solutions = NULL;
		char fname1[1000];


		if (f_write_tree) {
			sprintf(fname1, fname_tree, starter_case);
			}
		
		char fname_system2[1000];
		char fname_tree2[1000];

		if (f_draw_system) {
			sprintf(fname_system2, "%s_%d", fname_system, the_starter_case);
			}
		if (f_write_tree) {
			sprintf(fname_tree2, "%s_%d", fname_tree, the_starter_case);
			}
		compute_liftings_single_case_new(the_starter_case, 
			f_solve, f_save, f_read_instead, 
			nb_col, 
			Solutions, sol_length, nb_sol, nb_backtrack, dt, 
			f_draw_system, fname_system2, 
			f_write_tree, fname_tree2, 
			verbose_level /* - 2 */);

			// see below
		
		if (f_v) {
			int tps, ts, tm, th, td;

			tps = os_ticks_per_second();
			os_ticks_to_dhms(dt, tps, td, th, tm, ts);
			cout << "exact_cover::compute_liftings_new starter_case " << starter_case << " / " << starter_nb_cases << " which is case " << the_starter_case << " found " << nb_sol << " solutions with " << nb_backtrack << " backtrack nodes in ";
			print_elapsed_time(cout, td, th, tm, ts);
			cout << endl;
			}

		nb_sol_deleted = 0;

		if (nb_sol) {

			if (!Solutions) {
				cout << "exact_cover::compute_liftings_new nb_sol && !Solutions" << endl;
				exit(1);
				}

			if (f_v3) {
				cout << "exact_cover::compute_liftings_new There are " << nb_sol << " solutions" << endl;
				//int_matrix_print(Solutions, nb_sol, sol_length);
				}


			if (f_v3) {
				cout << "exact_cover::compute_liftings_new final processing of solutions" << endl;
				}
			
			int *the_solution;

			the_solution = NEW_int(starter_size + sol_length);
			int i, j, f_do_it;

			for (i = 0; i < nb_sol; i++) {
				if (FALSE /* f_v3 */) {
					cout << "exact_cover::compute_liftings_new solution " << i << " / " << nb_sol << endl;
					}

				int_vec_copy(starter, the_solution, starter_size);
				for (j = 0; j < sol_length; j++) {
					the_solution[starter_size + j] = Solutions[i * sol_length + j];
					}

				if (f_has_solution_test_func) {
					if (FALSE /* f_v3 */) {
						cout << "exact_cover::compute_liftings_new calling solution_test_func" << endl;
						}
					f_do_it = (*solution_test_func)(this, 
						the_solution, starter_size + sol_length, 
						solution_test_func_data, 0 /* verbose_level */);
					}
				else {
					f_do_it = TRUE;
					}


				if (f_do_it) {
					if (f_has_solution_test_func && f_v3) {
						cout << "solution " << i << " survives the test and has been written to file" << endl;
						}
					fp << the_starter_case;
					for (j = 0; j < starter_size + sol_length; j++) {
						fp << " " << the_solution[j];
						}
					fp << endl;
					}
				else {
					if (f_v3) {
						cout << "solution " << i << " is not a real solution, skip" << endl;
						}
					nb_sol_deleted++;
					nb_deleted_solutions++;
					}
				}
			FREE_int(the_solution);
			FREE_int(Solutions);
			}

		if (f_has_late_cleanup_function) {
			(*late_cleanup_function)(this, the_starter_case, verbose_level);
			}


		nb_sol -= nb_sol_deleted;
		if (f_v) {
			cout << "exact_cover::compute_liftings_new starter_case " << starter_case << " / " << starter_nb_cases << " which is case " << the_starter_case << " with " << nb_sol << " solutions in " << dt / os_ticks_per_second() << " sec (nb_sol_deleted=" << nb_sol_deleted << ")" << endl;
			}
		total_solutions += nb_sol;
		Case_nb[nb_cases] = the_starter_case;
		Nb_col[nb_cases] = nb_col;
		Nb_sol[nb_cases] = nb_sol;
		Nb_backtrack[nb_cases] = nb_backtrack;
		Dt[nb_cases] = dt;
		Dt_in_sec[nb_cases] = dt / os_ticks_per_second();
		nb_cases++;
		Nb_sol_total += nb_sol;
		}
	fp << -1 << " " << Nb_sol_total << endl;
	}
	cout << "written file " << fname_solutions << " of size " << file_size(fname_solutions) << endl;
	cout << "total_solutions = " << Nb_sol_total << endl;
	cout << "nb_deleted_solutions=" << nb_deleted_solutions << endl;
	
	int *Vec[6];
	const char *column_labels[6] = {"Case_nb", "Nb_sol", "Nb_backtrack", "Nb_col", "Dt", "Dt_in_sec" };
	Vec[0] = Case_nb;
	Vec[1] = Nb_sol;
	Vec[2] = Nb_backtrack;
	Vec[3] = Nb_col;
	Vec[4] = Dt;
	Vec[5] = Dt_in_sec;
	
	int_vec_array_write_csv(6, Vec, nb_cases, fname_statistics, column_labels);
	//int_vecs_write_csv(Nb_sol, Nb_col, nb_cases, fname_statistics, "Nb_sol", "Nb_col");
	cout << "written file " << fname_statistics << " of size " << file_size(fname_statistics) << endl;
	

	
	FREE_int(Case_nb);
	FREE_int(Nb_col);
	FREE_int(Nb_sol);
	FREE_int(Nb_backtrack);
	FREE_int(Dt);
	FREE_int(Dt_in_sec);
	if (f_v) {
		cout << "exact_cover::compute_liftings_new done" << endl;
		}
}


void exact_cover::compute_liftings_single_case_new(int starter_case, 
	int f_solve, int f_save, int f_read_instead, 
	int &nb_col, 
	int *&Solutions, int &sol_length, int &nb_sol, int &nb_backtrack, int &dt, 
	int f_draw_system, const char *fname_system, 
	int f_write_tree, const char *fname_tree, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_v4 = (verbose_level >= 4);
	char str[1000];


	if (f_v) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " verbose_level=" << verbose_level << endl;
		}


	if (prepare_function_new == NULL) {
		cout << "exact_cover::compute_liftings_single_case_new prepare_function_new == NULL" << endl;
		exit(1);
		}

	Solutions = NULL;
	nb_sol = 0;
	nb_col = 0;
	nb_backtrack = 0;
	
	if (f_vv) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " before R->init_from_file" << endl;
		}


	sprintf(str, "%s%s", input_prefix, base_fname);

	orbit_rep *R;
	R = new orbit_rep;


	if (f_vv) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " before R->init_from_file str=" << str << endl;
		}

	R->init_from_file(A_base, str, 
		starter_size, starter_case, starter_size - 1 /*level_of_candidates_file*/, 
		early_test_func, 
		early_test_func_data, 
		verbose_level - 3
		);
	if (f_vv) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " after R->init_from_file str=" << str << endl;
		}

		// R has: int *candidates; int nb_candidates;
	
	int_vec_copy(R->rep, starter, starter_size);

	if (f_v) {
		cout << "exact_cover::compute_liftings_single_case case " << starter_case << " / " << starter_nb_cases << " stab_go = " << *R->stab_go << " starter = ";
		int_vec_print(cout, starter, starter_size);
		cout << endl;
		}

	if (f_vv) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " calling prepare function" << endl;
		}

	diophant *Dio = NULL;
	int *col_labels;
	int f_ruled_out = FALSE;

	(*prepare_function_new)(this, starter_case, 
		R->candidates, R->nb_candidates, R->Strong_gens, 
		Dio, col_labels, 
		f_ruled_out, 
		verbose_level);

	if (f_vv) {
		cout << "exact_cover::compute_liftings_single_case_new after prepare function" << endl;
		}

	if (f_ruled_out) {
		if (f_vv) {
			cout << "Case is ruled out" << endl;
			}
		nb_sol = 0;
		nb_col = 0;
		nb_backtrack = 0;
		dt = 0;
		}
	else {
		if (f_vv) {
			cout << "The system is " << Dio->m << " x " << Dio->n << endl;
			}
		if (FALSE && f_v4) {
			Dio->print();
			}
		
		Dio->trivial_row_reductions(f_ruled_out, verbose_level);


		if (f_draw_system) {
			int xmax_in = 1000000;
			int ymax_in = 1000000;
			int xmax_out = 1000000;
			int ymax_out = 1000000;
		
			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new drawing the system" << endl;
				}
			Dio->draw_it(fname_system, xmax_in, ymax_in, xmax_out, ymax_out);
			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new drawing the system done" << endl;
				}
			}

		char fname[1000];
		char fname_Levi[1000];
		char fname_sol[1000];

		sprintf(fname, "%ssystem_%d.txt", output_prefix, starter_case);
		sprintf(fname_Levi, "%ssystem_%d_Levi_graph.bin", output_prefix, starter_case);
		sprintf(fname_sol, "%ssystem_%d.sol", output_prefix, starter_case);


		if (f_save) {
		
			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new before save_as_Levi_graph, fname=" << fname_Levi << endl;
				}

			
			Dio->save_as_Levi_graph(fname_Levi, verbose_level - 1);

			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new before save_in_compact_format, fname=" << fname << endl;
				}
			Dio->save_in_compact_format(fname, verbose_level - 1);
			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new after save_in_compact_format" << endl;
				}
			}
		if (f_solve || f_read_instead) {
			int t0 = 0, t1 = 0;
			int i, j, a, b;

			if (f_v) {
				cout << "exact_cover::compute_liftings_single_case_new before solve_all_DLX_with_RHS" << endl;
				}
			if (f_solve) { 
				t0 = os_ticks();
				Dio->solve_all_DLX_with_RHS(f_write_tree, fname_tree, verbose_level - 5);
				t1 = os_ticks();
				if (f_v) {
					cout << "exact_cover::compute_liftings_single_case_new after solve_all_DLX_with_RHS nb_backtrack = " << Dio->nb_steps_betten << " nb_sol = " << Dio->_resultanz << endl;
					}
				}
			else if (f_read_instead) {
				char fname_sol[1000];
				const char *fname_solutions_mask = "%ssystem_%d.solutions";
				
				sprintf(fname_sol, fname_solutions_mask, solution_prefix, starter_case);

				if (f_v) {
					cout << "exact_cover::compute_liftings_single_case_new trying to read solution file " << fname_sol << " of size " << file_size(fname_sol) << endl;
					}

				Dio->read_solutions_from_file(fname_sol, 0 /*verbose_level - 2*/);
				Dio->nb_steps_betten = 0;

				if (f_v) {
					cout << "exact_cover::compute_liftings_single_case_new read " << Dio->_resultanz << " solutions from file " << fname_sol << endl;
					}
				
				}
			nb_col = Dio->n;
			nb_sol = Dio->_resultanz;
			nb_backtrack = Dio->nb_steps_betten;
			sol_length = Dio->sum;
			if (nb_sol) {
#if 0
				if (f_save) {
					Dio->write_solutions(verbose_level);
					}
#endif
				Dio->get_solutions(Solutions, nb_sol, 0/*verbose_level - 1*/);
				if (f_v4) {
					cout << "exact_cover::compute_liftings_single_case_new nb_sol=" << nb_sol << endl;
					//cout << "exact_cover::compute_liftings_single_case_new Solutions:" << endl;
					//int_matrix_print(Solutions, nb_sol, sol_length);
					}

				if (f_save) {
					int_matrix_write_text(fname_sol, Solutions, nb_sol, sol_length);
					}
				for (i = 0; i < nb_sol; i++) {
					for (j = 0; j < sol_length; j++) {
						a = Solutions[i * sol_length + j];
						b = col_labels[a];
						Solutions[i * sol_length + j] = b;
						}
					}
				if (f_v4) {
					cout << "exact_cover::compute_liftings_single_case_new nb_sol=" << nb_sol << endl;
					//cout << "exact_cover::compute_liftings_single_case_new Solutions in terms of col_labels[]:" << endl;
					//int_matrix_print(Solutions, nb_sol, sol_length);
					}
				}
			else {
				Solutions = NULL;
				}
			dt = t1 - t0;
			}
		else {
			nb_sol = 0;
			nb_col = Dio->n;
			nb_backtrack = 0;
			dt = 0;
			sol_length = 0;
			}
		} // else 


	if (Dio) {
		delete Dio;
		FREE_int(col_labels);
			// we don't use cleanup_function any more
		}


	delete R;

	if (f_v) {
		cout << "exact_cover::compute_liftings_single_case_new case " << starter_case << " / " << starter_nb_cases << " done with " << nb_sol << " solutions" << endl;
		}

}

void exact_cover::lexorder_test(int *live_blocks2, int &nb_live_blocks2, vector_ge *stab_gens, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vvv = (verbose_level >= 3);

	if (f_v) {
		cout << "exact_cover::lexorder_test" << endl;
		}
	int nb_accepted, max_starter;

	if (starter_size) {
		max_starter = starter[starter_size - 1];
	
		if (f_vvv) {
			cout << "exact_cover::lexorder_test Before lexorder_test, nb_live_blocks2=" << nb_live_blocks2 << endl;
			}
		A_on_blocks->lexorder_test(live_blocks2, nb_live_blocks2, nb_accepted, 
			stab_gens /*starter_stabilizer_gens */, max_starter, verbose_level);

		if (f_vvv) {
			cout << "exact_cover::lexorder_test After lexorder_test, nb_live_blocks2=" << nb_accepted << " we reject " << nb_live_blocks2 - nb_accepted << " blocks" << endl;
			}
		nb_live_blocks2 = nb_accepted;
		}
	if (f_v) {
		cout << "exact_cover::lexorder_test done" << endl;
		}
}


