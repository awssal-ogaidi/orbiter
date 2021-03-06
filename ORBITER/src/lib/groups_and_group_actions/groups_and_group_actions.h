// action.h
//
// Anton Betten
//
// started:  August 13, 2005


typedef class action action;
typedef class matrix_group matrix_group;
typedef class perm_group perm_group;
typedef class vector_ge vector_ge;
typedef class vector_ge *p_vector_ge;
typedef class schreier schreier;
typedef class sims sims;
typedef class sims *p_sims;
typedef class group group;
typedef class page_storage page_storage;
typedef class action_on_sets action_on_sets;
typedef class action_on_k_subsets action_on_k_subsets;
typedef class action_by_right_multiplication 
	action_by_right_multiplication;
typedef class action_by_restriction action_by_restriction;
typedef class action_by_conjugation action_by_conjugation;
typedef class action_on_orbits action_on_orbits;
typedef class action_on_flags action_on_flags;
typedef class action_by_representation action_by_representation;
typedef class action_by_subfield_structure action_by_subfield_structure;
typedef class action_on_grassmannian action_on_grassmannian;
typedef class action_on_spread_set action_on_spread_set;
typedef class action_on_orthogonal action_on_orthogonal;
typedef class action_on_wedge_product action_on_wedge_product;
typedef class action_on_cosets action_on_cosets;
typedef class action_on_factor_space action_on_factor_space;
typedef class action_on_determinant action_on_determinant;
typedef class action_on_sign action_on_sign;
typedef class action_on_homogeneous_polynomials 
	action_on_homogeneous_polynomials;
typedef class product_action product_action;
typedef class union_find union_find;
typedef class union_find_on_k_subsets union_find_on_k_subsets;
typedef class schreier_sims schreier_sims;
typedef sims *psims;
typedef class action_on_bricks action_on_bricks;
typedef class action_on_andre action_on_andre;
typedef class strong_generators strong_generators;
typedef strong_generators *pstrong_generators;
typedef class linear_group_description linear_group_description;
typedef class linear_group linear_group;
typedef class set_and_stabilizer set_and_stabilizer;
typedef class subgroup subgroup;
typedef class subgroup *psubgroup;
typedef class action_on_subgroups action_on_subgroups;
typedef class orbit_transversal orbit_transversal;
typedef class wreath_product wreath_product;
typedef class direct_product direct_product;
typedef class schreier_vector_handler schreier_vector_handler;
typedef class schreier_vector schreier_vector;
typedef class poset_description poset_description;
typedef class poset poset;


enum symmetry_group_type { 
	unknown_symmetry_group_t, 
	matrix_group_t, 
	perm_group_t, 
	wreath_product_t,
	direct_product_t,
	action_on_sets_t,
	action_on_subgroups_t,
	action_on_k_subsets_t,
	action_on_pairs_t,
	action_on_ordered_pairs_t,
	base_change_t,
	product_action_t,
	action_by_right_multiplication_t,
	action_by_restriction_t,
	action_by_conjugation_t,
	action_on_determinant_t, 
	action_on_sign_t, 
	action_on_grassmannian_t, 
	action_on_spread_set_t, 
	action_on_orthogonal_t, 
	action_on_cosets_t, 
	action_on_factor_space_t, 
	action_on_wedge_product_t, 
	action_by_representation_t,
	action_by_subfield_structure_t,
	action_on_bricks_t,
	action_on_andre_t,
	action_on_orbits_t,
	action_on_flags_t,
	action_on_homogeneous_polynomials_t
};

enum representation_type {
	representation_type_nothing, 
	representation_type_PSL2_on_conic
}; 


//! internal class related to action


union symmetry_group {
	matrix_group *matrix_grp;
	perm_group *perm_grp;
	wreath_product *wreath_product_group;
	direct_product *direct_product_group;
	action_on_sets *on_sets;
	action_on_subgroups *on_subgroups;
	action_on_k_subsets *on_k_subsets;
	product_action *product_action_data;
	action_by_right_multiplication *ABRM;
	action_by_restriction *ABR;
	action_by_conjugation *ABC;
	action_on_determinant *AD;
	action_on_sign *OnSign;
	action_on_grassmannian *AG;
	action_on_spread_set *AS;
	action_on_orthogonal *AO;
	action_on_cosets *OnCosets;
	action_on_factor_space *AF;
	action_on_wedge_product *AW;
	action_by_representation *Rep;
	action_by_subfield_structure *SubfieldStructure;
	action_on_bricks *OnBricks;
	action_on_andre *OnAndre;
	action_on_orbits *OnOrbits;
	action_on_flags *OnFlags;
	action_on_homogeneous_polynomials *OnHP;
};


// #############################################################################
// vector_ge.C:
// #############################################################################

//! vector of group elements


class vector_ge {

public:
	action *A;
	int *data;
	int len;
	
	vector_ge();
	vector_ge(action *A);
	~vector_ge();
	void null();
	void freeself();
	void init(action *A);
	void init_by_hdl(action *A, int *gen_hdl, int nb_gen);
	void init_single(action *A, int *Elt);
	void init_double(action *A, int *Elt1, int *Elt2);
	void init_from_permutation_representation(action *A, int *data, 
		int nb_elements, int verbose_level);
		// data[nb_elements * degree]
	void init_from_data(action *A, int *data, 
		int nb_elements, int elt_size, int verbose_level);
	void init_conjugate_svas_of(vector_ge *v, int *Elt, 
		int verbose_level);
	void init_conjugate_sasv_of(vector_ge *v, int *Elt, 
		int verbose_level);
	int *ith(int i);
	void print(ostream &ost);
	//ostream& print(ostream& ost);
	ostream& print_quick(ostream& ost);
	ostream& print_tex(ostream& ost);
	void print_generators_tex(
			longinteger_object &go, ostream &ost);
	ostream& print_as_permutation(ostream& ost);
	void allocate(int length);
	void reallocate(int new_length);
	void reallocate_and_insert_at(int position, int *elt);
	void insert_at(int length_before, int position, int *elt);
		// does not reallocate, but shifts elements up to make space.
		// the last element might be lost if there is no space.
	void append(int *elt);
	void copy_in(int i, int *elt);
	void copy_out(int i, int *elt);
	void conjugate_svas(int *Elt);
	void conjugate_sasv(int *Elt);
	void print_with_given_action(ostream &ost, action *A2);
	void print(ostream &ost, int f_print_as_permutation, 
		int f_offset, int offset, 
		int f_do_it_anyway_even_for_big_degree, 
		int f_print_cycles_of_length_one);
	void write_to_memory_object(memory_object *m, 
		int verbose_level);
	void read_from_memory_object(memory_object *m, 
		int verbose_level);
	void write_to_file_binary(ofstream &fp, 
		int verbose_level);
	void read_from_file_binary(ifstream &fp, 
		int verbose_level);
	void extract_subset_of_elements_by_rank_text_vector(
		const char *rank_vector_text, sims *S, 
		int verbose_level);
	void extract_subset_of_elements_by_rank(int *rank_vector, 
		int len, sims *S, int verbose_level);
	int test_if_all_elements_stabilize_a_point(action *A2, int pt);
	int test_if_all_elements_stabilize_a_set(action *A2, 
		int *set, int sz, int verbose_level);
};

#include "./data_structures/data_structures.h"
#include "./group_actions/group_actions.h"
#include "./group_theory/group_theory.h"
#include "./induced_actions/induced_actions.h"


