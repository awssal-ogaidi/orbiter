// exact_cover_arguments.C
//
// Anton Betten
// January 12, 2016

#include "orbiter.h"



exact_cover_arguments::exact_cover_arguments()
{
	null();
}

exact_cover_arguments::~exact_cover_arguments()
{
	freeself();
}

void exact_cover_arguments::null()
{
	f_lift = FALSE;
	f_has_base_fname = FALSE;
	base_fname = "";
	f_has_input_prefix = FALSE;
	input_prefix = "";
	f_has_output_prefix = FALSE;
	output_prefix = "";
	f_has_solution_prefix = FALSE;
	solution_prefix = "";
	f_lift = FALSE;
	f_starter_size = FALSE;
	starter_size = 0;
	f_lex = FALSE;
	f_split = FALSE;
	split_r = 0;
	split_m = 1;
	f_solve = FALSE;
	f_save = FALSE;
	f_read = FALSE;
	f_draw_system = FALSE;
	fname_system = NULL;
	f_write_tree = FALSE;
	fname_tree = NULL;
	f_has_solution_test_function = FALSE;
	f_has_late_cleanup_function = FALSE;
	prepare_function_new = NULL;
	early_test_function = NULL;
	early_test_function_data = NULL;
	solution_test_func = NULL;
	solution_test_func_data = NULL;
	late_cleanup_function = NULL;
	f_randomized = FALSE;
	random_permutation_fname = NULL;
}

void exact_cover_arguments::freeself()
{
	null();
}

void exact_cover_arguments::read_arguments(int argc, const char **argv, 
	int verbose_level)
{
	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			continue;
			}
		else if (strcmp(argv[i], "-starter_size") == 0) {
			f_starter_size = TRUE;
			starter_size = atoi(argv[++i]);
			cout << "-starter_size " << starter_size << endl;
			}
		else if (strcmp(argv[i], "-lift") == 0) {
			f_lift = TRUE;
			//lift_prefix = argv[++i]; 
			cout << "-lift " << endl;
			}
		else if (strcmp(argv[i], "-lex") == 0) {
			f_lex = TRUE;
			cout << "-lex" << endl;
			}
		else if (strcmp(argv[i], "-solve") == 0) {
			f_solve = TRUE;
			cout << "-solve" << endl;
			}
		else if (strcmp(argv[i], "-save") == 0) {
			f_save = TRUE;
			cout << "-save" << endl;
			}
		else if (strcmp(argv[i], "-read") == 0) {
			f_read = TRUE;
			cout << "-read" << endl;
			}
		else if (strcmp(argv[i], "-split") == 0) {
			f_split = TRUE;
			split_r = atoi(argv[++i]);
			split_m = atoi(argv[++i]);
			cout << "-split " << split_r << " " << split_m << endl;
			}
		else if (strcmp(argv[i], "-draw_system") == 0) {
			f_draw_system = TRUE;
			fname_system = argv[++i];
			cout << "-draw_system " << fname_system << endl;
			}
		else if (strcmp(argv[i], "-write_tree") == 0) {
			f_write_tree = TRUE;
			fname_tree = argv[++i];
			cout << "-write_tree " << fname_tree << endl;
			}
		else if (strcmp(argv[i], "-base_fname") == 0) {
			f_has_base_fname = TRUE;
			base_fname = argv[++i];
			cout << "-base_fname " << base_fname << endl;
			}
		else if (strcmp(argv[i], "-input_prefix") == 0) {
			f_has_input_prefix = TRUE;
			input_prefix = argv[++i];
			cout << "-input_prefix " << input_prefix << endl;
			}
		else if (strcmp(argv[i], "-output_prefix") == 0) {
			f_has_output_prefix = TRUE;
			output_prefix = argv[++i];
			cout << "-output_prefix " << output_prefix << endl;
			}
		else if (strcmp(argv[i], "-solution_prefix") == 0) {
			f_has_solution_prefix = TRUE;
			solution_prefix = argv[++i];
			cout << "-solution_prefix " << solution_prefix << endl;
			}
		else if (strcmp(argv[i], "-randomized") == 0) {
			f_randomized = TRUE;
			random_permutation_fname = argv[++i];
			cout << "-randomized " << random_permutation_fname << endl;
			}
		}
}

void exact_cover_arguments::compute_lifts(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "exact_cover_arguments::compute_lifts" << endl;
		cout << "exact_cover_arguments::compute_lifts verbose_level=" << verbose_level << endl;
		cout << "exact_cover_arguments::compute_lifts base_fname=" << base_fname << endl;
		cout << "exact_cover_arguments::compute_lifts input_prefix=" << input_prefix << endl;
		cout << "exact_cover_arguments::compute_lifts output_prefix=" << output_prefix << endl;
		cout << "exact_cover_arguments::compute_lifts solution_prefix=" << solution_prefix << endl;
		}

	if (!f_has_base_fname) {
		cout << "exact_cover_arguments::compute_lifts no base_fname" << endl;
		exit(1);
		}
	if (!f_has_input_prefix) {
		cout << "exact_cover_arguments::compute_lifts no input_prefix" << endl;
		exit(1);
		}
	if (!f_has_output_prefix) {
		cout << "exact_cover_arguments::compute_lifts no output_prefix" << endl;
		exit(1);
		}
	if (!f_has_solution_prefix) {
		cout << "exact_cover_arguments::compute_lifts no solution_prefix" << endl;
		exit(1);
		}
	if (!f_starter_size) {
		cout << "exact_cover_arguments::compute_lifts no starter_size" << endl;
		exit(1);
		}

	if (target_size == 0) {
		cout << "exact_cover_arguments::compute_lifts target_size == 0" << endl;
		exit(1);
		}

	exact_cover *E;

	E = new exact_cover;

 
	E->init_basic(user_data, 
		A, A2, 
		target_size, starter_size, 
		input_prefix, output_prefix, solution_prefix, base_fname, 
		f_lex, 
		verbose_level - 1);

	E->init_early_test_func(
		early_test_function, early_test_function_data,
		verbose_level);

	E->init_prepare_function_new(
		prepare_function_new, 
		verbose_level);

	if (f_split) {
		E->set_split(split_r, split_m, verbose_level - 1);
		}

	if (f_has_solution_test_function) {
		E->add_solution_test_function(
			solution_test_func, 
			(void *) solution_test_func_data,
			verbose_level - 1);
		}

	if (f_has_late_cleanup_function) {
		E->add_late_cleanup_function(late_cleanup_function);
		}

	if (f_randomized) {
		E->randomize(random_permutation_fname, verbose_level);
		}
	
	if (f_v) {
		cout << "exact_cover_arguments::compute_lifts before compute_liftings_new" << endl;
		}

	E->compute_liftings_new(f_solve, f_save, f_read, 
		f_draw_system, fname_system, 
		f_write_tree, fname_tree,
		verbose_level);

	delete E;
	
	if (f_v) {
		cout << "exact_cover_arguments::compute_lifts done" << endl;
		}
	
}


