// orbits.h
//
// Anton Betten
//
// moved here from top_level.h: July 28, 2018
// top_level started:  September 23 2010
// based on global.h, which was taken from reader.h: 3/22/09

// #############################################################################
// orbit_of_equations.C
// #############################################################################


//! Schreier tree for action on homogeneous equations


class orbit_of_equations {
public:
	action *A;
	action_on_homogeneous_polynomials *AonHPD;
	finite_field *F;
	strong_generators *SG;
	int nb_monomials;
	int sz; // = 1 + nb_monomials
	int sz_for_compare; // = 1 + nb_monomials
	int *data_tmp; // [sz]

	int position_of_original_object;
	int allocation_length;
	int used_length;
	int **Equations;
	int *prev;
	int *label;


	orbit_of_equations();
	~orbit_of_equations();
	void null();
	void freeself();
	void init(action *A, finite_field *F, 
		action_on_homogeneous_polynomials *AonHPD, 
		strong_generators *SG, int *coeff_in, 
		int verbose_level);
	void map_an_equation(int *object_in, int *object_out, 
		int *Elt, int verbose_level);
	void print_orbit();
	void compute_orbit(int *coeff, int verbose_level);
	void get_transporter(int idx, int *transporter, int verbose_level);
		// transporter is an element which maps 
		// the orbit representative to the given subspace.
	void get_random_schreier_generator(int *Elt, int verbose_level);
	void compute_stabilizer(action *default_action, 
		longinteger_object &go, 
		sims *&Stab, int verbose_level);
		// this function allocates a sims structure into Stab.
	strong_generators *stabilizer_orbit_rep(
		longinteger_object &full_group_order, int verbose_level);
	int search_data(int *data, int &idx);
	void save_csv(const char *fname, int verbose_level);
};

int orbit_of_equations_compare_func(void *a, void *b, void *data);

// #############################################################################
// orbit_of_sets.C
// #############################################################################


//! Schreier tree for action on subsets



class orbit_of_sets {
public:
	action *A;
	action *A2;
	vector_ge *gens;
	int *set;
	int sz;

	int position_of_original_set;
	int allocation_length;
	int used_length;
	int **Sets;

	orbit_of_sets();
	~orbit_of_sets();
	void null();
	void freeself();
	void init(action *A, action *A2, int *set, int sz, 
		vector_ge *gens, int verbose_level);
	void compute(int verbose_level);
	void get_table_of_orbits(int *&Table, int &orbit_length, 
		int &set_size, int verbose_level);
};

int orbit_of_sets_compare_func(void *a, void *b, void *data);

// #############################################################################
// orbit_of_subspaces.C
// #############################################################################



//! Schreier tree for action on subspaces



class orbit_of_subspaces {
public:
	action *A;
	action *A2;
	finite_field *F;
	vector_ge *gens;
	int k;
	int n;
	int kn;
	int sz; // = 1 + k + kn
	int sz_for_compare; // = 1 + k + kn
	int f_has_desired_pivots;
	int *desired_pivots; // [k]
	int *subspace_by_rank; // [k]
	int *data_tmp; // [sz]

	int f_has_rank_functions;
	void *rank_unrank_data;
	int (*rank_vector_callback)(int *v, int n, 
		void *data, int verbose_level);
	void (*unrank_vector_callback)(int rk, int *v, 
		int n, void *data, int verbose_level);
	void (*compute_image_of_vector_callback)(int *v, int *w, 
		int *Elt, void *data, int verbose_level);
	void *compute_image_of_vector_callback_data;

	int position_of_original_subspace;
	int allocation_length;
	int used_length;
	int **Subspaces;
	int *prev;
	int *label;


	orbit_of_subspaces();
	~orbit_of_subspaces();
	void null();
	void freeself();
	void init(action *A, action *A2, finite_field *F, 
		int *subspace, int k, int n, 
		int f_has_desired_pivots, int *desired_pivots, 
		int f_has_rank_functions, void *rank_unrank_data, 
		int (*rank_vector_callback)(int *v, int n, 
			void *data, int verbose_level), 
		void (*unrank_vector_callback)(int rk, int *v, 
			int n, void *data, int verbose_level), 
		void (*compute_image_of_vector_callback)(int *v, 
			int *w, int *Elt, void *data, int verbose_level), 
		void *compute_image_of_vector_callback_data, 
		vector_ge *gens, int verbose_level);
	int rank_vector(int *v, int verbose_level);
	void unrank_vector(int rk, int *v, int verbose_level);
	void rref(int *subspace, int verbose_level);
	void rref_and_rank_and_hash(int *subspace, int verbose_level);
	void map_a_subspace(int *subspace, int *image_subspace, 
		int *Elt, int verbose_level);
	void map_a_basis(int *basis, int *image_basis, int *Elt, 
		int verbose_level);
	void print_orbit();
	void compute(int verbose_level);
	void get_transporter(int idx, int *transporter, int verbose_level);
		// transporter is an element which maps the orbit 
		// representative to the given subspace.
	void get_random_schreier_generator(int *Elt, int verbose_level);
	strong_generators *stabilizer_orbit_rep(
		longinteger_object &full_group_order, int verbose_level);
	void compute_stabilizer(action *default_action, longinteger_object &go, 
		sims *&Stab, int verbose_level);
		// this function allocates a sims structure into Stab.
	int search_data(int *data, int &idx);
	int search_data_raw(int *data_raw, int &idx, int verbose_level);
};

int orbit_of_subspaces_compare_func(void *a, void *b, void *data);

// #############################################################################
// orbit_rep.C
// #############################################################################


//! to hold one orbit after reading files from Orbiters poset classification


class orbit_rep {
public:
	char prefix[1000];
	action *A;
	void (*early_test_func_callback)(int *S, int len, 
		int *candidates, int nb_candidates, 
		int *good_candidates, int &nb_good_candidates, 
		void *data, int verbose_level);
	void *early_test_func_callback_data;
	
	int level;
	int orbit_at_level;
	int nb_cases;

	int *rep;

	sims *Stab;
	strong_generators *Strong_gens;

	longinteger_object *stab_go;
	int *candidates;
	int nb_candidates;


	orbit_rep();
	~orbit_rep();
	void null();
	void freeself();
	void init_from_file(action *A, char *prefix, 
		int level, int orbit_at_level, int level_of_candidates_file, 
		void (*early_test_func_callback)(int *S, int len, 
			int *candidates, int nb_candidates, 
			int *good_candidates, int &nb_good_candidates, 
			void *data, int verbose_level), 
		void *early_test_func_callback_data, 
		int verbose_level);
	
};


// #############################################################################
// subspace_orbits.C
// #############################################################################

//! poset classification for orbits on subspaces



class subspace_orbits {
public:

	linear_group *LG;
	int n;
	finite_field *F;
	int q;
	int depth;

	int f_print_generators;
	

	int *tmp_M; // [n * n]
	int *tmp_M2; // [n * n]
	int *tmp_M3; // [n * n]
	int *base_cols; // [n]
	int *v; // [n]
	int *w; // [n]
	int *weights; // [n + 1]

	vector_space *VS;
	poset *Poset;
	poset_classification *Gen;

	int schreier_depth;
	int f_use_invariant_subset_if_available;
	int f_implicit_fusion;
	int f_debug;

	int f_has_strong_generators;
	strong_generators *Strong_gens;

	int f_has_extra_test_func;
	int (*extra_test_func)(subspace_orbits *SubOrb, 
		int len, int *S, void *data, int verbose_level);
	void *extra_test_func_data;

	int test_dim;


	subspace_orbits();
	~subspace_orbits();
	void init(int argc, const char **argv, 
		linear_group *LG, int depth, 
		int verbose_level);
	void init_group(int verbose_level);
	void compute_orbits(int verbose_level);
	void unrank_set_to_M(int len, int *S);
	void unrank_set_to_matrix(int len, int *S, int *M);
	void rank_set_from_matrix(int len, int *S, int *M);
	void Kramer_Mesner_matrix(int t, int k, int f_print_matrix, 
		int f_read_solutions, const char *solution_fname, 
		int verbose_level);
	void print_all_solutions(diophant *D, int k, int *Sol, int nb_sol, 
		int **Subspace_ranks, int &nb_subspaces, int verbose_level);
	void print_one_solution(diophant *D, int k, int *sol, 
		int *&subspace_ranks, int &nb_subspaces, int verbose_level);
	int test_dim_C_cap_Cperp_property(int len, int *S, int d);
	int compute_minimum_distance(int len, int *S);
	void print_set(ostream &ost, int len, int *S);
	int test_set(int len, int *S, int verbose_level);
	int test_minimum_distance(int len, int *S, 
		int mindist, int verbose_level);
	int test_if_self_orthogonal(int len, int *S, 
		int f_doubly_even, int verbose_level);
};


int subspace_orbits_rank_point_func(int *v, void *data);
void subspace_orbits_unrank_point_func(int *v, int rk, void *data);
void subspace_orbits_early_test_func(int *S, int len, 
	int *candidates, int nb_candidates, 
	int *good_candidates, int &nb_good_candidates, 
	void *data, int verbose_level);



