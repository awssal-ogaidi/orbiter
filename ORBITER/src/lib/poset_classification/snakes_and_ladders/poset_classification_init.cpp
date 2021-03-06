// poset_classification_init.C
//
// Anton Betten
// December 29, 2003
//
// moved here from poset_classification.C: July 29, 2014


#include "foundations/foundations.h"
#include "groups_and_group_actions/groups_and_group_actions.h"
#include "poset_classification/poset_classification.h"

poset_classification::poset_classification()
{
	null();
}

poset_classification::~poset_classification()
{
	freeself();
}

void poset_classification::null()
{
	problem_label[0] = 0;
	
	//f_candidate_check_func = FALSE;
	//f_candidate_incremental_check_func = FALSE;
	f_print_function = FALSE;
	Elt_memory = NULL;

	//A = NULL;
	//Strong_gens = NULL;
	Poset = NULL;

	Schreier_vector_handler = NULL;
	//SG0 = NULL;
	//transversal_length = NULL;
	S = NULL;

	tmp_set_apply_fusion = NULL;
	tmp_find_node_for_subspace_by_rank1 = NULL;
	tmp_find_node_for_subspace_by_rank2 = NULL;
	//tmp_find_node_for_subspace_by_rank3 = NULL;


	nb_times_trace = 0;
	nb_times_trace_was_saved = 0;
	
	sz = 0;
	transporter = NULL;
	set = NULL;
	
	
	nb_poset_orbit_nodes_used = 0;
	root = NULL;
	first_poset_orbit_node_at_level = NULL;
	set0 = NULL;
	set1 = NULL;
	set3 = NULL;
	nb_extension_nodes_at_level_total = NULL;
	nb_extension_nodes_at_level = NULL;
	nb_fusion_nodes_at_level = NULL;
	nb_unprocessed_nodes_at_level = NULL;


	//f_prefix = FALSE;
	//prefix[0] = 0;
	
	f_lex = FALSE;
	f_max_depth = FALSE;
	
	f_extend = FALSE;
	f_recover = FALSE;
		
	f_w = FALSE;
	f_W = FALSE;
	f_t = FALSE;
	f_T = FALSE;
	f_log = FALSE;
	f_Log = FALSE;
	f_print_only = FALSE;
	f_find_group_order = FALSE;

	f_has_tools_path = FALSE;
	tools_path = NULL;

	f_has_invariant_subset_for_root_node = FALSE;
	invariant_subset_for_root_node = NULL;
	
	verbose_level = 0;
	verbose_level_group_theory = 0;
	
	fname_base[0] = 0;
	
	xmax = 1000000;
	ymax = 1000000;
	radius = 300;
	
	f_starter = FALSE;
#if 0
	f_downstep_split = FALSE;
	f_upstep_split = FALSE;
	f_downstep_collate = FALSE;
	f_upstep_collate = FALSE;
	split_mod = 0;
	split_case = 0;
#endif
	
	f_do_group_extension_in_upstep = TRUE;

	f_allowed_to_show_group_elements = FALSE;	
	downstep_orbits_print_max_orbits = 25;
	downstep_orbits_print_max_points_per_orbit = 50;

#if 0
	f_on_subspaces = FALSE;
	rank_point_func = NULL;
	unrank_point_func = NULL;
	tmp_v1 = NULL;
#endif

#if 0
	f_early_test_func = FALSE;
	early_test_func = NULL;
	f_its_OK_to_not_have_an_early_test_func = FALSE;
#endif

	depth = 0;

	f_export_schreier_trees = FALSE;
	f_draw_schreier_trees = FALSE;
	schreier_tree_prefix[0] = 0;
	schreier_tree_xmax = 1000000;
	schreier_tree_ymax =  500000;
	schreier_tree_f_circletext = TRUE;
	schreier_tree_rad = 25000;
	schreier_tree_f_embedded = TRUE;
	schreier_tree_f_sideways = FALSE;
	schreier_tree_scale = 0.3;
	schreier_tree_line_width = 1.;

	t0 = os_ticks();
}

void poset_classification::freeself()
{
	int i;
	int f_v = FALSE;
	
	if (f_v) {
		cout << "poset_classification::freeself" << endl;
		}
	if (Elt_memory) {
		FREE_int(Elt_memory);
		}

	// do not free Strong_gens
	

	if (f_v) {
		cout << "poset_classification::freeself deleting S" << endl;
		}
	if (S) {
		FREE_int(S);
		}
	if (Schreier_vector_handler) {
		FREE_OBJECT(Schreier_vector_handler);
	}
	if (tmp_set_apply_fusion) {
		FREE_int(tmp_set_apply_fusion);
		}
	if (tmp_find_node_for_subspace_by_rank1) {
		FREE_int(tmp_find_node_for_subspace_by_rank1);
		}
	if (tmp_find_node_for_subspace_by_rank2) {
		FREE_int(tmp_find_node_for_subspace_by_rank2);
		}
	//if (tmp_find_node_for_subspace_by_rank3) {
	//	FREE_int(tmp_find_node_for_subspace_by_rank3);
	//	}

	if (f_v) {
		cout << "poset_classification::freeself "
				"deleting transporter and set[]" << endl;
		}
	if (transporter) {
		FREE_OBJECT(transporter);
		for (i = 0; i <= sz; i++) {
			FREE_int(set[i]);			
			}
		FREE_pint(set);
		}
	if (f_v) {
		cout << "poset_classification::freeself "
				"before exit_poset_orbit_node" << endl;
		}
	exit_poset_orbit_node();
	if (f_v) {
		cout << "poset_classification::freeself "
				"after exit_poset_orbit_node" << endl;
		}
#if 0
	if (tmp_v1) {
		FREE_int(tmp_v1);
	}
#endif


	if (f_v) {
		cout << "poset_classification::freeself done" << endl;
		}
	null();
}

void poset_classification::usage()
{
	cout << "poset_classification options:" << endl;
	cout << "-v <n>" << endl;
	cout << "  verbose level n" << endl;
	cout << "-lex" << endl;
	cout << "  use lex reduction (cannot be used in vector "
			"space setting)" << endl;
	cout << "-gv <n>" << endl;
	cout << "  verbose level n for all group theory related things" << endl;
	cout << "-w" << endl;
	cout << "  write output in level files (only last level)" << endl;
	cout << "-W" << endl;
	cout << "  write output in level files (all levels)" << endl;
	cout << "-depth <n>" << endl;
	cout << "  compute up to depth n" << endl;
	cout << "-prefix <s>" << endl;
	cout << "  use s as prefix for all output files" << endl;
	cout << "-extend <f> <t> <r> <m> <s>" << endl;
	cout << "  extend all partial solutions congruent" << endl;
	cout << "  to <r> mod <m> from file <s> from level <f> "
			"to level <t>" << endl;
	cout << "-x xmax" << endl;
	cout << "   specifies horizontal size (default 1000)" << endl;
	cout << "-y ymax" << endl;
	cout << "   specifies vertical size (default 1000)" << endl;
	cout << "-rad r" << endl;
	cout << "   specifies radius (default 300)" << endl;
	cout << "-t" << endl;
	cout << "   draw tree at last level only" << endl;
	cout << "-T" << endl;
	cout << "   draw tree each level" << endl;
	cout << "-log" << endl;
	cout << "   log nodes at the end only" << endl;
	cout << "-Log" << endl;
	cout << "   log nodes at every step" << endl;
	cout << "-r <fname>" << endl;
	cout << "   recover from data file <fname>" << endl;
	cout << "-printonly <fname>" << endl;
	cout << "   print only (no computation), to be used with -r" << endl;
	cout << "-findgroup <order>" << endl;
	cout << "   find group of order <order>" << endl;
}

void poset_classification::read_arguments(int argc,
		const char **argv, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			i++;
			poset_classification::verbose_level = atoi(argv[i]);
			if (f_v) {
				cout << "-v " << poset_classification::verbose_level << endl;
				}
			}
		else if (strcmp(argv[i], "-gv") == 0) {
			i++;
			verbose_level_group_theory = atoi(argv[i]);
			if (f_v) {
				cout << "-gv " << verbose_level_group_theory << endl;
				}
			}
		else if (strcmp(argv[i], "-lex") == 0) {
			f_lex = TRUE;
			if (f_v) {
				cout << "-lex" << endl;
				}
			}
#if 0
		else if (strcmp(argv[i], "-prefix") == 0) {
			i++;
			f_prefix = TRUE;
			strcpy(prefix, argv[i]);
			if (f_v) {
				cout << "-prefix " << prefix << endl;
				}
			}
#endif
		else if (strcmp(argv[i], "-w") == 0) {
			f_w = TRUE;
			if (f_v) {
				cout << "-w" << endl;
				}
			}
		else if (strcmp(argv[i], "-W") == 0) {
			f_W = TRUE;
			if (f_v) {
				cout << "-W" << endl;
				}
			}
		else if (strcmp(argv[i], "-t") == 0) {
			f_t = TRUE;
			if (f_v) {
				cout << "-t" << endl;
				}
			}
		else if (strcmp(argv[i], "-T") == 0) {
			f_T = TRUE;
			if (f_v) {
				cout << "-T" << endl;
				}
			}
		else if (strcmp(argv[i], "-log") == 0) {
			f_log = TRUE;
			if (f_v) {
				cout << "-log" << endl;
				}
			}
		else if (strcmp(argv[i], "-Log") == 0) {
			f_Log = TRUE;
			if (f_v) {
				cout << "-Log" << endl;
				}
			}
		else if (strcmp(argv[i], "-x") == 0) {
			xmax = atoi(argv[i + 1]);
			i++;
			if (f_v) {
				cout << "-x " << xmax << endl;
				}
			}
		else if (strcmp(argv[i], "-y") == 0) {
			ymax = atoi(argv[i + 1]);
			i++;
			if (f_v) {
				cout << "-y " << ymax << endl;
				}
			}
		else if (strcmp(argv[i], "-rad") == 0) {
			radius = atoi(argv[i + 1]);
			i++;
			if (f_v) {
				cout << "-rad " << radius << endl;
				}
			}
		else if (strcmp(argv[i], "-depth") == 0) {
			f_max_depth = TRUE;
			max_depth = atoi(argv[++i]);
			if (f_v) {
				cout << "-depth " << max_depth << endl;
				}
			}
		else if (strcmp(argv[i], "-extend") == 0) {
			f_extend = TRUE;
			extend_from = atoi(argv[++i]);
			extend_to = atoi(argv[++i]);
			extend_r = atoi(argv[++i]);
			extend_m = atoi(argv[++i]);
			strcpy(extend_fname, argv[++i]);
			if (f_v) {
				cout << "-extend from level " << extend_from 
					<< " to level " << extend_to 
					<< " cases congruent " << extend_r
					<< " mod " << extend_m
					<< " from file " << extend_fname << endl;
				}
			}
		else if (strcmp(argv[i], "-recover") == 0) {
			f_recover = TRUE;
			recover_fname = argv[++i];
			if (f_v) {
				cout << "-recover " << recover_fname << endl; 
				}
			}
		else if (strcmp(argv[i], "-printonly") == 0) {
			f_print_only = TRUE;
			if (f_v) {
				cout << "-printonly" << endl; 
				}
			}
		else if (strcmp(argv[i], "-findgroup") == 0) {
			f_find_group_order = TRUE;
			find_group_order = atoi(argv[++i]);
			if (f_v) {
				cout << "-findgroup " << find_group_order << endl;
				}
			}
		else if (strcmp(argv[i], "-export_schreier_trees") == 0) {
			f_export_schreier_trees = TRUE;
			cout << "-export_schreier_trees" << endl;
		}
		else if (strcmp(argv[i], "-draw_schreier_trees") == 0) {
			f_draw_schreier_trees = TRUE;
			strcpy(schreier_tree_prefix, argv[++i]);
			schreier_tree_xmax = atoi(argv[++i]);
			schreier_tree_ymax = atoi(argv[++i]);
			schreier_tree_f_circletext = atoi(argv[++i]);
			schreier_tree_rad = atoi(argv[++i]);
			schreier_tree_f_embedded = atoi(argv[++i]);
			schreier_tree_f_sideways = atoi(argv[++i]);
			schreier_tree_scale = atoi(argv[++i]) * 0.01;
			schreier_tree_line_width = atoi(argv[++i]) * 0.01;
			cout << "-draw_schreier_trees " << schreier_tree_prefix 
				<< " " << schreier_tree_xmax 
				<< " " << schreier_tree_ymax 
				<< " " << schreier_tree_f_circletext 
				<< " " << schreier_tree_f_embedded 
				<< " " << schreier_tree_f_sideways 
				<< " " << schreier_tree_scale 
				<< " " << schreier_tree_line_width 
				<< endl;
			}
		else if (strcmp(argv[i], "-tools_path") == 0) {
			f_has_tools_path = TRUE;
			tools_path = argv[++i];
			if (f_v) {
				cout << "-tools_path " << tools_path << endl;
				}
			}


		}
}

void poset_classification::init(poset *Poset,
	int sz, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	//int f_v6 = (verbose_level >= 6);
	int i;
	
	if (f_v) {
		cout << "poset_classification::init" << endl;
		}
	
	poset_classification::Poset = Poset;
	//poset_classification::A = A;
	//poset_classification::A2 = A2;
	poset_classification::sz = sz;

	if (Poset == NULL) {
		cout << "poset_classification::init "
				"Poset == NULL" << endl;
		exit(1);
		}
	if (Poset->A == NULL) {
		cout << "poset_classification::init "
				"Poset->A == NULL" << endl;
		exit(1);
		}
	if (Poset->A2 == NULL) {
		cout << "poset_classification::init "
				"Poset->A2 == NULL" << endl;
		exit(1);
		}
	if (f_v) {
		cout << "poset_classification::init "
				"sz = " << sz << endl;
		cout << "poset_classification::init "
				"A->degree=" << Poset->A->degree << endl;
		cout << "poset_classification::init "
				"A2->degree=" << Poset->A2->degree << endl;
		}

	if (Poset->Orbit_based_testing) {
		Poset->Orbit_based_testing->PC = this;
	}

	t0 = os_ticks();

	progress_epsilon = 0.005;

	
	if (f_vv) {
		cout << "poset_classification::init action A:" << endl;
		Poset->A->print_info();
		cout << "poset_classification::init action A2:" << endl;
		Poset->A2->print_info();
		}


#if 0
	if (f_v) {
		cout << "poset_classification::init computing group order" << endl;
		}

	//gens->group_order(go);
#endif

	if (f_v) {
		cout << "poset_classification::init group order is ";
		cout << Poset->go << endl;
		}
	
	Schreier_vector_handler = NEW_OBJECT(schreier_vector_handler);
	Schreier_vector_handler->init(Poset->A, Poset->A2,
			TRUE /* f_allow_failure */,
			verbose_level);
	
	if (f_v) {
		cout << "poset_classification::init sz = " << sz << endl;
		}
	
	//Strong_gens = gens;



	if (f_vv) {
		cout << "poset_classification::init "
				"allocating S of size " << sz << endl;
		}
	S = NEW_int(sz);
	for (i = 0; i < sz; i++) {
		S[i] = i;
		}

	tmp_set_apply_fusion = NEW_int(sz + 1);

	if (f_vv) {
		cout << "poset_classification::init "
				"allocating Elt_memory" << endl;
		}


	Elt_memory = NEW_int(6 * Poset->A->elt_size_in_int);
	Elt1 = Elt_memory + 0 * Poset->A->elt_size_in_int;
	Elt2 = Elt_memory + 1 * Poset->A->elt_size_in_int;
	Elt3 = Elt_memory + 2 * Poset->A->elt_size_in_int;
	Elt4 = Elt_memory + 3 * Poset->A->elt_size_in_int;
	Elt5 = Elt_memory + 4 * Poset->A->elt_size_in_int;
	Elt6 = Elt_memory + 5 * Poset->A->elt_size_in_int;
	
	transporter = NEW_OBJECT(vector_ge);
	transporter->init(Poset->A);
	transporter->allocate(sz + 1);
	Poset->A->element_one(transporter->ith(0), FALSE);
	
	set = NEW_pint(sz + 1);
	for (i = 0; i <= sz; i++) {
		set[i] = NEW_int(sz);
		}

		
	nb_poset_orbit_nodes_used = 0;
	nb_poset_orbit_nodes_allocated = 0;

	nb_times_image_of_called0 = Poset->A->nb_times_image_of_called;
	nb_times_mult_called0 = Poset->A->nb_times_mult_called;
	nb_times_invert_called0 = Poset->A->nb_times_invert_called;
	nb_times_retrieve_called0 = Poset->A->nb_times_retrieve_called;
	nb_times_store_called0 = Poset->A->nb_times_store_called;


	if (Poset->f_subspace_lattice) {
		tmp_find_node_for_subspace_by_rank1 =
				NEW_int(Poset->VS->dimension);
		tmp_find_node_for_subspace_by_rank2 =
				NEW_int(sz * Poset->VS->dimension);
		//tmp_find_node_for_subspace_by_rank3 =
		//		NEW_int(Poset->VS->dimension);
	}

	if (f_v) {
		cout << "poset_classification::init done" << endl;
		}
}


void poset_classification::initialize(
	poset *Poset,
	int depth, 
	const char *path, const char *prefix, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "poset_classification::initialize" << endl;
		cout << "poset_classification::initialize "
				"depth = " << depth << endl;
		}

	strcpy(poset_classification::path, path);
	strcpy(poset_classification::prefix, prefix);
	sprintf(fname_base, "%s%s", path, prefix);

	poset_classification::depth = depth;
	downstep_orbits_print_max_orbits = 50;
	downstep_orbits_print_max_points_per_orbit = INT_MAX;
	

	// !!!
	//f_allowed_to_show_group_elements = TRUE;

	if (f_vv) {
		cout << "poset_classification::initialize "
				"calling gen->init" << endl;
		}
	init(Poset,
		depth, verbose_level - 2);
	
	int nb_poset_orbit_nodes = 1000;
	
	if (f_vv) {
		cout << "poset_classification::initialize "
				"calling gen->init_poset_orbit_node" << endl;
		}
	init_poset_orbit_node(nb_poset_orbit_nodes, verbose_level - 1);
	if (f_vv) {
		cout << "poset_classification::initialize "
				"calling gen->init_root_node" << endl;
		}
	init_root_node(verbose_level - 1);

	if (f_v) {
		cout << "poset_classification::initialize done" << endl;
		}
}


void poset_classification::initialize_with_starter(
	poset *Poset,
	int depth, 
	char *path, 
	char *prefix, 
	int starter_size, 
	int *starter, 
	strong_generators *Starter_Strong_gens, 
	int *starter_live_points, 
	int starter_nb_live_points, 
	void *starter_canonize_data, 
	int (*starter_canonize)(int *Set, int len,
			int *Elt, void *data, int verbose_level),
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "poset_classification::initialize_with_starter" << endl;
		}

	strcpy(poset_classification::path, path);
	strcpy(poset_classification::prefix, prefix);
	sprintf(fname_base, "%s%s", path, prefix);


	poset_classification::depth = depth;
	downstep_orbits_print_max_orbits = 50;
	downstep_orbits_print_max_points_per_orbit = INT_MAX;
	

	// !!!
	//f_allowed_to_show_group_elements = TRUE;

	if (f_vv) {
		cout << "poset_classification::initialize_with_starter "
				"calling gen->init" << endl;
		}
	init(Poset,
		depth, verbose_level - 2);
	

	if (f_vv) {
		cout << "poset_classification::initialize_with_starter "
				"calling init_starter" << endl;
		}
	init_starter(starter_size, 
		starter, 
		Starter_Strong_gens, 
		starter_live_points, 
		starter_nb_live_points, 
		starter_canonize_data, 
		starter_canonize, 
		verbose_level - 2);

	int nb_poset_orbit_nodes = 1000;
	
	if (f_vv) {
		cout << "poset_classification::initialize_with_starter "
				"calling gen->init_poset_orbit_node" << endl;
		}
	init_poset_orbit_node(nb_poset_orbit_nodes, verbose_level - 1);
	if (f_vv) {
		cout << "poset_classification::initialize_with_starter "
				"calling gen->init_root_node" << endl;
		}
	init_root_node(verbose_level);

	if (f_v) {
		cout << "poset_classification::initialize_with_starter done" << endl;
		}
}

void poset_classification::init_root_node_invariant_subset(
	int *invariant_subset, int invariant_subset_size,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "poset_classification::init_root_node_"
				"invariant_subset" << endl;
		}
	f_has_invariant_subset_for_root_node = TRUE;
	invariant_subset_for_root_node = invariant_subset;
	invariant_subset_for_root_node_size = invariant_subset_size;
	if (f_v) {
		cout << "poset_classification::init_root_node_"
				"invariant_subset "
				"installed invariant subset of size "
				<< invariant_subset_size << endl;
		}
}


void poset_classification::init_root_node(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	
	if (f_v) {
		cout << "poset_classification::init_root_node" << endl;
		}
	if (f_starter) {
		int i;
		
		first_poset_orbit_node_at_level[0] = 0;
		root[0].freeself();
		root[0].node = 0;
		root[0].prev = -1;
		root[0].nb_strong_generators = 0;
		root[0].Schreier_vector = NULL;
		for (i = 0; i < starter_size; i++) {
			
			nb_extension_nodes_at_level_total[i] = 0;
			nb_extension_nodes_at_level[i] = 0;
			nb_fusion_nodes_at_level[i] = 0;
			nb_unprocessed_nodes_at_level[i] = 0;
			
			if (f_vv) {
				cout << "poset_classification::init_root_node "
						"initializing "
						"node at level " << i << endl;
				}
			first_poset_orbit_node_at_level[i + 1] =
					first_poset_orbit_node_at_level[i] + 1;
			root[i].E = NEW_OBJECTS(extension, 1);
			root[i].nb_extensions = 1;
			root[i].E[0].type = EXTENSION_TYPE_EXTENSION;
			root[i].E[0].data = i + 1;
			root[i + 1].freeself();
			root[i + 1].node = i + 1;
			root[i + 1].prev = i;
			root[i + 1].pt = starter[i];
			root[i + 1].nb_strong_generators = 0;
			root[i + 1].Schreier_vector = NULL;
			}
		if (f_vv) {
			cout << "poset_classification::init_root_node "
					"storing strong poset_classifications" << endl;
			}
		root[starter_size].store_strong_generators(
				this, starter_strong_gens);
		first_poset_orbit_node_at_level[starter_size + 1] =
				starter_size + 1;
		if (f_vv) {
			cout << "i : first_poset_orbit_node_at_level[i]" << endl;
			for (i = 0; i <= starter_size + 1; i++) {
				cout << i << " : "
						<< first_poset_orbit_node_at_level[i] << endl;
				}
			}
		}
	else {
		root[0].init_root_node(this, verbose_level - 1);
		}
	if (f_v) {
		cout << "poset_classification::init_root_node done" << endl;
		}
}

void poset_classification::init_poset_orbit_node(
		int nb_poset_orbit_nodes, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "poset_classification::init_poset_orbit_node" << endl;
		}
	root = NEW_OBJECTS(poset_orbit_node, nb_poset_orbit_nodes);
	for (i = 0; i < nb_poset_orbit_nodes; i++) {
		root[i].node = i;
		}
	nb_poset_orbit_nodes_allocated = nb_poset_orbit_nodes;
	nb_poset_orbit_nodes_used = 0;
	poset_orbit_nodes_increment = nb_poset_orbit_nodes;
	poset_orbit_nodes_increment_last = nb_poset_orbit_nodes;
	first_poset_orbit_node_at_level = NEW_int(sz + 2);
	first_poset_orbit_node_at_level[0] = 0;
	first_poset_orbit_node_at_level[1] = 1;
	set0 = NEW_int(sz + 1);
	set1 = NEW_int(sz + 1);
	set3 = NEW_int(sz + 1);
	nb_extension_nodes_at_level_total = NEW_int(sz + 1);
	nb_extension_nodes_at_level = NEW_int(sz + 1);
	nb_fusion_nodes_at_level = NEW_int(sz + 1);
	nb_unprocessed_nodes_at_level = NEW_int(sz + 1);
	for (i = 0; i < sz + 1; i++) {
		nb_extension_nodes_at_level_total[i] = 0;
		nb_extension_nodes_at_level[i] = 0;
		nb_fusion_nodes_at_level[i] = 0;
		nb_unprocessed_nodes_at_level[i] = 0;
		}
	if (f_v) {
		cout << "poset_classification::init_poset_orbit_node done" << endl;
		}
}


void poset_classification::exit_poset_orbit_node()
{
	if (root) {
		FREE_OBJECTS(root);
		root = NULL;
		}
	if (set0) {
		FREE_int(set0);
		set0 = NULL;
		}
	if (set1) {
		FREE_int(set1);
		set1 = NULL;
		}
	if (set3) {
		FREE_int(set3);
		set3 = NULL;
		}
	if (first_poset_orbit_node_at_level) {
		FREE_int(first_poset_orbit_node_at_level);
		first_poset_orbit_node_at_level = NULL;
		}

	if (nb_extension_nodes_at_level_total) {
		FREE_int(nb_extension_nodes_at_level_total);
		nb_extension_nodes_at_level_total = NULL;
		}
	if (nb_extension_nodes_at_level) {
		FREE_int(nb_extension_nodes_at_level);
		nb_extension_nodes_at_level = NULL;
		}
	if (nb_fusion_nodes_at_level) {
		FREE_int(nb_fusion_nodes_at_level);
		nb_fusion_nodes_at_level = NULL;
		}
	if (nb_unprocessed_nodes_at_level) {
		FREE_int(nb_unprocessed_nodes_at_level);
		nb_unprocessed_nodes_at_level = NULL;
		}
}

void poset_classification::reallocate()
{
	int increment_new;
	int verbose_level = 0;
	
	increment_new = poset_orbit_nodes_increment +
			poset_orbit_nodes_increment_last;
	reallocate_to(nb_poset_orbit_nodes_allocated +
			poset_orbit_nodes_increment, verbose_level - 1);
	poset_orbit_nodes_increment_last = poset_orbit_nodes_increment;
	poset_orbit_nodes_increment = increment_new;
	
}

void poset_classification::reallocate_to(int new_number_of_nodes,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	poset_orbit_node *new_root;
	
	if (f_v) {
		cout << "poset_classification::reallocate_to" << endl;
		}
	if (new_number_of_nodes < nb_poset_orbit_nodes_allocated) {
		cout << "poset_classification::reallocate_to "
				"new_number_of_nodes < "
				"nb_poset_orbit_nodes_allocated" << endl;
		exit(1);
		}
	if (f_v) {
		cout << "poset_classification::reallocate_to from "
				<< nb_poset_orbit_nodes_allocated
				<< " to " << new_number_of_nodes << endl;
		}
	new_root = NEW_OBJECTS(poset_orbit_node, new_number_of_nodes);
	for (i = 0; i < nb_poset_orbit_nodes_allocated; i++) {
		new_root[i] = root[i];
		root[i].null();
		}
	FREE_OBJECTS(root);
	root = new_root;
	nb_poset_orbit_nodes_allocated = new_number_of_nodes;
	if (f_v) {
		cout << "poset_classification::reallocate_to done" << endl;
		}
}

#if 0
void poset_classification::init_check_func(
	int (*candidate_check_func)(int len, int *S,
			void *data, int verbose_level),
	void *candidate_check_data)
{
	f_candidate_check_func = TRUE;
	poset_classification::candidate_check_func = candidate_check_func;
	poset_classification::candidate_check_data = candidate_check_data;
}

void poset_classification::init_incremental_check_func(
	int (*candidate_incremental_check_func)(int len,
			int *S, void *data, int verbose_level),
	void *candidate_incremental_check_data)
{
	f_candidate_incremental_check_func = TRUE;
	poset_classification::candidate_incremental_check_func =
			candidate_incremental_check_func;
	poset_classification::candidate_incremental_check_data =
			candidate_incremental_check_data;
}
#endif

void poset_classification::init_starter(int starter_size, 
	int *starter, 
	strong_generators *starter_strong_gens, 
	int *starter_live_points, 
	int starter_nb_live_points, 
	void *starter_canonize_data, 
	int (*starter_canonize)(int *Set, int len,
			int *Elt, void *data, int verbose_level),
	int verbose_level)
// Does not initialize the first starter nodes.
// This is done in init_root_node
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "poset_classification::init_starter starter: ";
		int_vec_print(cout, starter, starter_size);
		cout << " with " << starter_strong_gens->gens->len
			<< " strong poset_classifications and "
			<< starter_nb_live_points << " live points" << endl;
		}
	f_starter = TRUE;
	poset_classification::starter_size = starter_size;
	poset_classification::starter = starter;
	poset_classification::starter_strong_gens = starter_strong_gens; 
	poset_classification::starter_live_points = starter_live_points;
	poset_classification::starter_nb_live_points = starter_nb_live_points;
	poset_classification::starter_canonize_data = starter_canonize_data;
	poset_classification::starter_canonize = starter_canonize;
	starter_canonize_Elt = NEW_int(Poset->A->elt_size_in_int);
}

#if 0
void poset_classification::init_vector_space_action(
	int vector_space_dimension,
	finite_field *F, 
	int (*rank_point_func)(int *v, void *data), 
	void (*unrank_point_func)(int *v, int rk, void *data),
	void *data,  
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "poset_classification::init_vector_space_action "
				"vector_space_dimension="
				<< vector_space_dimension << endl;
		}
	f_on_subspaces = TRUE;
	poset_classification::vector_space_dimension = vector_space_dimension;
	poset_classification::F = F;
	poset_classification::rank_point_func = rank_point_func;
	poset_classification::unrank_point_func = unrank_point_func;
	poset_classification::rank_point_data = data;

	tmp_find_node_for_subspace_by_rank1 =
			NEW_int(vector_space_dimension);
	tmp_find_node_for_subspace_by_rank2 =
			NEW_int(sz * vector_space_dimension);
	tmp_find_node_for_subspace_by_rank3 =
			NEW_int(vector_space_dimension);
	tmp_v1 =
			NEW_int(vector_space_dimension);
}
#endif


#if 0
void poset_classification::init_early_test_func(
	void (*early_test_func)(int *S, int len, 
		int *candidates, int nb_candidates, 
		int *good_candidates, int &nb_good_candidates, 
		void *data, int verbose_level), 
	void *data,  
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "poset_classification::init_early_test_func" << endl;
		}
	f_early_test_func = TRUE;
	poset_classification::early_test_func = early_test_func;
	early_test_func_data = data;
}
#endif



