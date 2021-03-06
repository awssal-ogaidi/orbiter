// vector_ge.C
//
// Anton Betten
// December 9, 2003

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

#undef PRint_WITH_TYPE
#define RANGE_CHECKING

vector_ge::vector_ge()
{
	null();
}

vector_ge::vector_ge(action *A)
{
	null();
	vector_ge::A = A;
}

vector_ge::~vector_ge()
{
	//cout << "in ~vector_ge data = " << data << endl;
	freeself();
}

void vector_ge::null()
{
	vector_ge::A = NULL;
	data = NULL;
	len = 0;
}

void vector_ge::freeself()
{
	if (data) {
		FREE_int(data);
		data = NULL;
		}
}

void vector_ge::init(action *A)
{
	//cout << "vector_ge::init()" << endl;
	freeself();
	vector_ge::A = A;
	data = NULL;
	len = 0;
}

void vector_ge::init_by_hdl(action *A, int *gen_hdl, int nb_gen)
{
	int i;
	
	init(A);
	allocate(nb_gen);
	for (i = 0; i < nb_gen; i++) {
		A->element_retrieve(gen_hdl[i], ith(i), 0);
		}
}

void vector_ge::init_single(action *A, int *Elt)
{
	init(A);
	allocate(1);
	A->element_move(Elt, ith(0), 0);
}

void vector_ge::init_double(action *A, int *Elt1, int *Elt2)
{
	init(A);
	allocate(2);
	A->element_move(Elt1, ith(0), 0);
	A->element_move(Elt2, ith(1), 0);
}

void vector_ge::init_from_permutation_representation(action *A, int *data, 
	int nb_elements, int verbose_level)
// data[nb_elements * A->degree]
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int i;
	int *Elt;

	if (f_v) {
		cout << "vector_ge::init_from_permutation_representation" << endl;
		}
	Elt = NEW_int(A->elt_size_in_int);
	init(A);
	allocate(nb_elements);
	for (i = 0; i < nb_elements; i++) {
		A->make_element_from_permutation_representation(Elt, data + i * A->degree, 0/*verbose_level*/);
		if (f_vv) {
			cout << "vector_ge::init_from_permutation_representation generator " << i << ": " << endl;
			A->element_print_quick(Elt, cout);
			}
		A->element_move(Elt, ith(i), 0);
		}
	
	FREE_int(Elt);
	if (f_v) {
		cout << "vector_ge::init_from_permutation_representation done" << endl;
		}
}

void vector_ge::init_from_data(action *A, int *data, 
	int nb_elements, int elt_size, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int i;
	int *Elt;

	if (f_v) {
		cout << "vector_ge::init_from_data" << endl;
		}
	Elt = NEW_int(A->elt_size_in_int);
	init(A);
	allocate(nb_elements);
	for (i = 0; i < nb_elements; i++) {
		A->make_element(Elt, data + i * elt_size, verbose_level);
		if (f_vv) {
			cout << "vector_ge::init_from_data generator " << i << ": " << endl;
			A->element_print_quick(Elt, cout);
			}
		A->element_move(Elt, ith(i), 0);
		}
	
	FREE_int(Elt);
	if (f_v) {
		cout << "vector_ge::init_from_data done" << endl;
		}
}

void vector_ge::init_conjugate_svas_of(vector_ge *v, int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	int *Elt1, *Elt2, *Elt3;

	if (f_v) {
		cout << "vector_ge::init_conjugate_svas_of" << endl;
		}

	init(v->A);
	allocate(v->len);

	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);
	A->invert(Elt, Elt1);
	for (i = 0; i < len; i++) {
		A->element_mult(Elt1, v->ith(i), Elt2, FALSE);
		A->element_mult(Elt2, Elt, Elt3, FALSE);
		A->element_move(Elt3, ith(i), FALSE);
		}
	
	FREE_int(Elt1);
	FREE_int(Elt2);
	FREE_int(Elt3);
	if (f_v) {
		cout << "vector_ge::init_conjugate_svas_of done" << endl;
		}
}

void vector_ge::init_conjugate_sasv_of(vector_ge *v, int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	int *Elt1, *Elt2, *Elt3;

	if (f_v) {
		cout << "vector_ge::init_conjugate_svas_of" << endl;
		}

	init(v->A);
	allocate(v->len);

	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);
	A->invert(Elt, Elt1);
	for (i = 0; i < len; i++) {
		A->element_mult(Elt, v->ith(i), Elt2, FALSE);
		A->element_mult(Elt2, Elt1, Elt3, FALSE);
		A->element_move(Elt3, ith(i), FALSE);
		}
	
	FREE_int(Elt1);
	FREE_int(Elt2);
	FREE_int(Elt3);
	if (f_v) {
		cout << "vector_ge::init_conjugate_sasv_of done" << endl;
		}
}

int *vector_ge::ith(int i)
{
#ifdef RANGE_CHECKING
	if (i < 0 || i >= len) {
		cout << "vector_ge::ith() access error i = " << i << " len = " << len << endl;
		exit(1);
		}
#endif
	return data + i * A->elt_size_in_int; 
}

void vector_ge::print(ostream &ost)
{
	int i;

	for (i = 0; i < len; i++) {
		ost << "Element " << i << " / " << len << " is:" << endl;
		A->element_print_quick(ith(i), ost);
		ost << endl;
		}
}

#if 0
ostream& vector_ge::print(ostream& ost)
{
	int i;
	
	ost << "(" << endl;
	//ost << "len=" << len << " A->elt_size_in_int=" << A->elt_size_in_int << " data=" << data << endl;
	for (i = 0; i < len; i++) {
		if (data == NULL) {
			cout << "vector_ge::print fatal: data == NULL" << endl;
			exit(1);
			}
		A->element_print(ith(i), ost);
		if (i < len - 1) {
			ost << ", " << endl;
			}
		}
	ost << ")" << endl;
	return ost;
}
#endif

ostream& vector_ge::print_quick(ostream& ost)
{
	int i;
	
	ost << "(" << endl;
	//ost << "len=" << len << " A->elt_size_in_int=" << A->elt_size_in_int << " data=" << data << endl;
	for (i = 0; i < len; i++) {
		if (data == NULL) {
			cout << "vector_ge::print fatal: data == NULL" << endl;
			exit(1);
			}
		A->element_print_quick(ith(i), ost);
		if (i < len - 1) {
			ost << ", " << endl;
			}
		}
	ost << ")" << endl;
	return ost;
}

ostream& vector_ge::print_tex(ostream& ost)
{
	int i;
	
	//ost << "(" << endl;
	//ost << "len=" << len << " A->elt_size_in_int=" << A->elt_size_in_int << " data=" << data << endl;
	for (i = 0; i < len; i++) {
		if (data == NULL) {
			cout << "vector_ge::print fatal: data == NULL" << endl;
			exit(1);
			}
		A->element_print_latex(ith(i), ost);
		if (i < len - 1) {
			ost << ", " << endl;
			}
		}
	//ost << ")" << endl;
	return ost;
}

void vector_ge::print_generators_tex(
		longinteger_object &go, ostream &ost)
{
	int i;

	ost << "Generators for a group of order " << go << ":" << endl;
	ost << "$$" << endl;
	for (i = 0; i < len; i++) {
		//cout << "Generator " << i << " / " << gens->len
		// << " is:" << endl;
		A->element_print_latex(ith(i), ost);
		if (((i + 1) % 4) == 0 && i < len - 1) {
			ost << "$$" << endl;
			ost << "$$" << endl;
			}
		}
	ost << "$$" << endl;
}

ostream& vector_ge::print_as_permutation(ostream& ost)
{
	int i;
	
	ost << "(" << endl;
	for (i = 0; i < len; i++) {
		A->element_print(ith(i), ost);
		cout << "is the permutation" << endl;
		A->element_print_as_permutation(ith(i), ost);
		if (i < len - 1) {
			ost << ", " << endl;
			}
		}
	ost << ")" << endl;
	return ost;
}

void vector_ge::allocate(int length)
{
	if (data) {
		FREE_int(data);
		//cout << "vector_ge::allocate warning, data != NULL, we seem to be having a memory leak here" << endl;
		}
	len = length;
	data = NEW_int(length * A->elt_size_in_int);
}

void vector_ge::reallocate(int new_length)
{
	int *data2 = NEW_int(new_length * A->elt_size_in_int);
	int *elt, *elt2, i, l;
	
	l = MINIMUM(len, new_length);
	for (i = 0; i < l; i++) {
		elt = ith(i);
		elt2 = data2 + i * A->elt_size_in_int;
		A->element_move(elt, elt2, FALSE);
		}
	if (data) {
		FREE_int(data);
		data = NULL;
		}
	data = data2;
	len = new_length;
};

void vector_ge::reallocate_and_insert_at(int position, int *elt)
{
	int *data2 = NEW_int((len + 1) * A->elt_size_in_int);
	int *elt1, *elt2, i;
	
	for (i = 0; i < len; i++) {
		elt1 = ith(i);
		if (i >= position) {
			elt2 = data2 + (i + 1) * A->elt_size_in_int;
			}
		else {
			elt2 = data2 + i * A->elt_size_in_int;
			}
		A->element_move(elt1, elt2, FALSE);
		}
	if (data) {
		FREE_int(data);
		data = NULL;
		}
	data = data2;
	len = len + 1;

	copy_in(position, elt);
}

void vector_ge::insert_at(int length_before, int position, int *elt)
// does not reallocate, but shifts elements up to make space.
// the last element might be lost if there is no space.
{
	int *elt1, *elt2, i;
	
	for (i = length_before; i >= position; i--) {
		if (i + 1 >= len)
			continue;
		
		elt1 = ith(i);
		elt2 = ith(i + 1);
		A->element_move(elt1, elt2, FALSE);
		}
	copy_in(position, elt);
}

void vector_ge::append(int *elt)
{
	reallocate_and_insert_at(len, elt);
}

void vector_ge::copy_in(int i, int *elt)
{
	int *elt2 = ith(i);
	A->element_move(elt, elt2, FALSE);
};

void vector_ge::copy_out(int i, int *elt)
{
	int *elt2 = ith(i);
	A->element_move(elt2, elt, FALSE);
}

void vector_ge::conjugate_svas(int *Elt)
{
	int i;
	int *Elt1, *Elt2, *Elt3;

	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);
	A->invert(Elt, Elt1);
	for (i = 0; i < len; i++) {
		A->element_mult(Elt1, ith(i), Elt2, FALSE);
		A->element_mult(Elt2, Elt, Elt3, FALSE);
		A->element_move(Elt3, ith(i), FALSE);
		}
	FREE_int(Elt1);
	FREE_int(Elt2);
	FREE_int(Elt3);
}

void vector_ge::conjugate_sasv(int *Elt)
{
	int i;
	int *Elt1, *Elt2, *Elt3;

	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);
	A->invert(Elt, Elt1);
	for (i = 0; i < len; i++) {
		A->element_mult(Elt, ith(i), Elt2, FALSE);
		A->element_mult(Elt2, Elt1, Elt3, FALSE);
		A->element_move(Elt3, ith(i), FALSE);
		}
	FREE_int(Elt1);
	FREE_int(Elt2);
	FREE_int(Elt3);
}

void vector_ge::print_with_given_action(ostream &ost, action *A2)
{
	int i, l;

	l = len;
	for (i = 0; i < l; i++) {
		ost << "generator " << i << ":" << endl;
		A->element_print_quick(ith(i), ost);
		ost << endl;
		A2->element_print_as_permutation(ith(i), ost);
		ost << endl;
		}
}

void vector_ge::print(ostream &ost, int f_print_as_permutation, 
	int f_offset, int offset, int f_do_it_anyway_even_for_big_degree, 
	int f_print_cycles_of_length_one)
{
	int i, l;
	
	l = len;
	if (!f_offset)
		offset = 0;
	ost << "Strong generators: (" << l << " of them)" << endl;
	ost << "f_print_as_permutation=" << f_print_as_permutation << endl;
	for (i = 0; i < l; i++) {
		ost << "generator " << i << ":" << endl;
		A->element_print_quick(ith(i), ost);
		ost << endl;
		if (f_print_as_permutation) {
			//A->element_print_as_permutation(ith(i), ost);
			A->element_print_as_permutation_with_offset(ith(i), ost, 
				offset, f_do_it_anyway_even_for_big_degree, 
				f_print_cycles_of_length_one, 0/*verbose_level*/);
			}
		}
}

void vector_ge::write_to_memory_object(memory_object *m, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "vector_ge::write_to_memory_object" << endl;
		}
	m->write_int(len);
	for (i = 0; i < len; i++) {
		A->element_write_to_memory_object(ith(i), m, 0);
		}
}

void vector_ge::read_from_memory_object(memory_object *m, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, l;

	if (f_v) {
		cout << "vector_ge::read_from_memory_object" << endl;
		}
	m->read_int(&l);
	allocate(l);
	for (i = 0; i < len; i++) {
		A->element_read_from_memory_object(ith(i), m, 0);
		}
}

void vector_ge::write_to_file_binary(ofstream &fp, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "vector_ge::write_to_file_binary" << endl;
		}
	fp.write((char *) &len, sizeof(int));
	for (i = 0; i < len; i++) {
		A->element_write_to_file_binary(ith(i), fp, 0);
		}
}

void vector_ge::read_from_file_binary(ifstream &fp, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, l;

	if (f_v) {
		cout << "vector_ge::read_from_file_binary" << endl;
		}
	fp.read((char *) &l, sizeof(int));
	allocate(l);
	for (i = 0; i < len; i++) {
		A->element_read_from_file_binary(ith(i), fp, 0);
		}
}

void vector_ge::extract_subset_of_elements_by_rank_text_vector(const char *rank_vector_text, sims *S, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "vector_ge::extract_subset_of_elements_by_rank_text_vector" << endl;
		}
	int *v;
	int len;

	int_vec_scan(rank_vector_text, v, len);
	if (f_v) {
		cout << "vector_ge::extract_subset_of_elements_by_rank_text_vector after scanning: ";
		int_vec_print(cout, v, len);
		cout << endl;
		}
	extract_subset_of_elements_by_rank(v, len, S, verbose_level);
	FREE_int(v);
	if (f_v) {
		cout << "vector_ge::extract_subset_of_elements_by_rank_text_vector done" << endl;
		}
}

void vector_ge::extract_subset_of_elements_by_rank(int *rank_vector, int len, sims *S, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, r;
	int *Elt;

	if (f_v) {
		cout << "vector_ge::extract_subset_of_elements_by_rank" << endl;
		}
	Elt = NEW_int(A->elt_size_in_int);
	allocate(len);
	for (i = 0; i < len; i++) {
		r = rank_vector[i];
		S->element_unrank_int(r, Elt);

		if (f_v) {
			cout << "vector_ge::extract_subset_of_elements_by_rank element " << i << " = " << r << " / " << len << endl;
			A->element_print_quick(Elt, cout);
			}
		A->element_move(Elt, ith(i), 0);
		}
	FREE_int(Elt);
	if (f_v) {
		cout << "vector_ge::extract_subset_of_elements_by_rank done" << endl;
		}
}

int vector_ge::test_if_all_elements_stabilize_a_point(action *A2, int pt)
{
	int i;
	
	for (i = 0; i < len; i++) {
		if (A2->element_image_of(pt, ith(i), 0) != pt) {
			return FALSE;
			}
		}
	return TRUE;
}

int vector_ge::test_if_all_elements_stabilize_a_set(action *A2, int *set, int sz, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "vector_ge::test_if_all_elements_stabilize_a_set" << endl;
		}
	
	for (i = 0; i < len; i++) {
		if (f_v) {
			cout << "testing element " << i << " / " << len << endl;
			}
		if (!A2->test_if_set_stabilizes(ith(i), sz, set, 0 /* verbose_level*/)) {
			return FALSE;
			}
		}
	if (f_v) {
		cout << "vector_ge::test_if_all_elements_stabilize_a_set done" << endl;
		}
	return TRUE;
}



