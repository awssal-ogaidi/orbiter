// schreier_io.C
//
// Anton Betten
// moved here from schreier.cpp: November 3, 2018
// originally started as schreier.cpp: December 9, 2003

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

void schreier::latex(const char *fname)
{
	int f_with_cosetrep = TRUE;

	{
	ofstream fp(fname);

	latex_head_easy(fp);

	print_generators_latex(fp);

	print_and_list_orbits_tex(fp);

	print_tables_latex(fp, f_with_cosetrep);

	latex_foot(fp);
	}
}


void schreier::print_orbit_lengths(ostream &ost)
{
	int i, f, l, m;
	int *orbit_len_sorted;
	int *sorting_perm;
	int *sorting_perm_inv;
	int nb_types;
	int *type_first;
	int *type_len;

	int_vec_classify(nb_orbits, orbit_len, orbit_len_sorted,
		sorting_perm, sorting_perm_inv,
		nb_types, type_first, type_len);

	ost << nb_orbits << " orbits: " << endl;
	for (i = 0; i < nb_types; i++) {
		f = type_first[i];
		l = type_len[i];
		m = orbit_len_sorted[f];
		if (l > 1) {
			cout << l << " \\times ";
			}
		cout << m;
		if (i < nb_types - 1)
			cout << ", ";
		}
	ost << endl;
	FREE_int(orbit_len_sorted);
	FREE_int(sorting_perm);
	FREE_int(sorting_perm_inv);
	FREE_int(type_first);
	FREE_int(type_len);

}

void schreier::print_orbit_length_distribution(ostream &ost)
{
	int *val, *mult, len;

	int_vec_distribution(orbit_len, nb_orbits, val, mult, len);
	int_distribution_print(ost, val, mult, len);
	ost << endl;

	FREE_int(val);
	FREE_int(mult);
}

void schreier::print_orbit_reps(ostream &ost)
{
	int i, c, r;

	ost << nb_orbits << " orbits" << endl;
	ost << "orbits of a group with " << gens.len
			<< " generators:" << endl;
	ost << "i : orbit_first[i] : orbit_len[i] : rep" << endl;
	for (i = 0; i < nb_orbits; i++) {
		ost << setw(3) << i << " : " << setw(6)
				<< orbit_first[i] << " : " << setw(6) << orbit_len[i];
		c = orbit_first[i];
		r = orbit[c];
		ost << " : " << setw(6) << r << endl;
		//<< " : ";
		//print_orbit(ost, i);
		//ost << endl;
		}
	ost << endl;
}


void schreier::print(ostream &ost)
{
	int i;

	ost << nb_orbits << " orbits" << endl;
	ost << "orbit group with " << gens.len << " generators:" << endl;
	ost << "i : orbit_first[i] : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {
		ost << i << " : " << orbit_first[i] << " : "
				<< orbit_len[i] << endl;
		//<< " : ";
		//print_orbit(ost, i);
		//ost << endl;
		}
	ost << endl;
}

void schreier::print_and_list_orbits_and_stabilizer(ostream &ost,
	action *default_action, longinteger_object &go,
	void (*print_point)(ostream &ost, int pt, void *data),
	void *data)
{
	int i;

	ost << nb_orbits << " orbits" << endl;
	ost << "orbit group with " << gens.len << " generators:" << endl;
	ost << "i : orbit_first[i] : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {

		sims *Stab;
		strong_generators *SG;

		ost << "Orbit " << i << " / " << nb_orbits
				<< " : of length " << orbit_len[i];
		ost << " is:" << endl;
		print_orbit(ost, i);
		ost << endl;
		ost << "Which is:" << endl;
		print_orbit_using_callback(ost, i, print_point, data);
		//ost << endl;
		ost << "The stabilizer of the element "
				<< orbit[orbit_first[i]] << " is:" << endl;
		point_stabilizer(default_action, go, Stab, i, 0 /* verbose_level */);

		SG = NEW_OBJECT(strong_generators);

		SG->init_from_sims(Stab, 0 /* verbose_level*/);
		SG->print_generators_ost(ost);
		FREE_OBJECT(SG);
		FREE_OBJECT(Stab);
		}
	ost << endl;
}

void schreier::print_and_list_orbits(ostream &ost)
{
	int i;

	ost << nb_orbits << " orbits" << endl;
	ost << "orbit group with " << gens.len << " generators:" << endl;
	ost << "i : orbit_first[i] : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {
		ost << " Orbit " << i << " / " << nb_orbits
				<< " : " << orbit_first[i] << " : " << orbit_len[i];
		ost << " : ";
		print_orbit(ost, i);
		ost << endl;
		}
	ost << endl;
}

void schreier::print_and_list_orbits_tex(ostream &ost)
{
	int i;

	ost << nb_orbits << " orbits:\\\\" << endl;
	ost << "orbits under a group with " << gens.len
			<< " generators acting on a set of size "
			<< A->degree << ":\\\\" << endl;
	//ost << "i : orbit_first[i] : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {
		print_and_list_orbit_tex(i, ost);
		}
	ost << endl;
}

void schreier::print_and_list_orbit_tex(int i, ostream &ost)
{
	ost << " Orbit " << i << " / " << nb_orbits << " : ";
	print_orbit_tex(ost, i);
	ost << " of length " << orbit_len[i] << "\\\\" << endl;
}

void schreier::print_and_list_orbit_and_stabilizer_tex(int i,
	action *default_action,
	longinteger_object &full_group_order, ostream &ost)
{
	ost << " Orbit " << i << " / " << nb_orbits << " : ";
	print_orbit_tex(ost, i);
	ost << " of length " << orbit_len[i] << "\\\\" << endl;

	strong_generators *gens;

	gens = stabilizer_orbit_rep(default_action,
		full_group_order, i, 0 /*verbose_level */);

	gens->print_generators_tex(ost);

	FREE_OBJECT(gens);
}

void schreier::print_and_list_orbit_and_stabilizer_with_list_of_elements_tex(
	int i, action *default_action,
	strong_generators *gens, ostream &ost)
{
	longinteger_object full_group_order;

	gens->group_order(full_group_order);

	ost << " Orbit " << i << " / " << nb_orbits << " : ";
	print_orbit_tex(ost, i);
	ost << " of length " << orbit_len[i] << "\\\\" << endl;

	strong_generators *gens_stab;

	gens_stab = stabilizer_orbit_rep(
		default_action,
		full_group_order, i, 0 /*verbose_level */);

	gens_stab->print_generators_tex(ost);

	int *Subgroup_elements_by_index;
	int sz_subgroup;

	sz_subgroup = gens_stab->group_order_as_int();

	if (sz_subgroup < 20) {
		gens->list_of_elements_of_subgroup(gens_stab,
			Subgroup_elements_by_index, sz_subgroup,
			0 /* verbose_level */);

		int_vec_heapsort(Subgroup_elements_by_index,
				sz_subgroup);

		ost << "The subgroup consists of the following "
				<< sz_subgroup << " elements:" << endl;
		ost << "$$" << endl;
		int_vec_print_as_matrix(ost,
				Subgroup_elements_by_index, sz_subgroup,
				10 /* width */, TRUE /* f_tex */);
		ost << "$$" << endl;

		FREE_int(Subgroup_elements_by_index);

		}

	FREE_OBJECT(gens_stab);
}

void schreier::print_and_list_orbits_sorted_by_length_tex(
		ostream &ost)
{
	print_and_list_orbits_sorted_by_length(ost, TRUE);
}

void schreier::print_and_list_orbits_sorted_by_length(
		ostream &ost)
{
	print_and_list_orbits_sorted_by_length(ost, FALSE);
}

void schreier::print_and_list_orbits_sorted_by_length(
	ostream &ost, int f_tex)
{
	int i, h;
	int *Len;
	int *Perm;
	int *Perm_inv;

	Len = NEW_int(nb_orbits);
	Perm = NEW_int(nb_orbits);
	Perm_inv = NEW_int(nb_orbits);
	int_vec_copy(orbit_len, Len, nb_orbits);
	int_vec_sorting_permutation(Len, nb_orbits,
			Perm, Perm_inv, TRUE /*f_increasingly*/);

	ost << "There are " << nb_orbits
			<< " orbits under a group with "
			<< gens.len << " generators:";
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	ost << "Orbit lengths: ";
	int_vec_print(ost, orbit_len, nb_orbits);
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	if (!f_tex) {
		ost << "i : orbit_len[i]" << endl;
		}
	for (h = 0; h < nb_orbits; h++) {
		i = Perm_inv[h];
		if (f_tex) {
			print_and_list_orbit_tex(i, ost);
			}
		else {
			ost << " Orbit " << h << " / " << nb_orbits
					<< " is " << i << " : " << orbit_len[i];
			ost << " : ";
			print_orbit(ost, i);
			ost << endl;
			}
		}
	ost << endl;

	FREE_int(Len);
	FREE_int(Perm);
	FREE_int(Perm_inv);
}

void schreier::print_and_list_orbits_and_stabilizer_sorted_by_length(
	ostream &ost, int f_tex,
	action *default_action,
	longinteger_object &full_group_order)
{
	int i, h;
	int *Len;
	int *Perm;
	int *Perm_inv;

	Len = NEW_int(nb_orbits);
	Perm = NEW_int(nb_orbits);
	Perm_inv = NEW_int(nb_orbits);
	int_vec_copy(orbit_len, Len, nb_orbits);
	int_vec_sorting_permutation(Len, nb_orbits,
			Perm, Perm_inv, TRUE /*f_increasingly*/);

	ost << "There are " << nb_orbits << " orbits under a group with "
			<< gens.len << " generators:";
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	ost << "Orbit lengths: ";
	int_vec_print(ost, orbit_len, nb_orbits);
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	if (!f_tex) {
		ost << "i : orbit_len[i]" << endl;
		}
	for (h = 0; h < nb_orbits; h++) {
		i = Perm_inv[h];
		if (f_tex) {
			print_and_list_orbit_and_stabilizer_tex(i,
					default_action, full_group_order, ost);
			}
		else {
			ost << " Orbit " << h << " / " << nb_orbits
					<< " is " << i << " : " << orbit_len[i];
			ost << " : ";
			print_orbit(ost, i);
			ost << endl;
			}
		}
	ost << endl;

	FREE_int(Len);
	FREE_int(Perm);
	FREE_int(Perm_inv);
}

void schreier::print_fancy(
	ostream &ost, int f_tex,
	action *default_action,
	strong_generators *gens_full_group)
{
	int i, h;
	int *Len;
	int *Perm;
	int *Perm_inv;
	longinteger_object full_group_order;

	gens_full_group->group_order(full_group_order);
	Len = NEW_int(nb_orbits);
	Perm = NEW_int(nb_orbits);
	Perm_inv = NEW_int(nb_orbits);
	int_vec_copy(orbit_len, Len, nb_orbits);
	int_vec_sorting_permutation(Len, nb_orbits,
			Perm, Perm_inv, TRUE /*f_increasingly*/);

	ost << "There are " << nb_orbits << " orbits under a group with "
			<< gens.len << " generators:";
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	ost << "Orbit lengths: ";
	int_vec_print(ost, orbit_len, nb_orbits);
	if (f_tex) {
		ost << "\\\\" << endl;
		}
	else {
		ost << endl;
		}
	if (!f_tex) {
		ost << "i : orbit_len[i]" << endl;
		}
	for (h = 0; h < nb_orbits; h++) {
		i = Perm_inv[h];
		if (f_tex) {
			//print_and_list_orbit_and_stabilizer_tex(
			// i, default_action, full_group_order, ost);
			print_and_list_orbit_and_stabilizer_with_list_of_elements_tex(
				i, default_action,
				gens_full_group, ost);
			}
		else {
			ost << " Orbit " << h << " / " << nb_orbits
					<< " is " << i << " : " << orbit_len[i];
			ost << " : ";
			print_orbit(ost, i);
			ost << endl;
			}
		}
	ost << endl;

	FREE_int(Len);
	FREE_int(Perm);
	FREE_int(Perm_inv);
}

void schreier::print_and_list_orbits_of_given_length(
	ostream &ost, int len)
{
	int i;


	ost << "Orbits of length " << len << ":" << endl;
	cout << "i : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {
		if (orbit_len[i] != len) {
			continue;
			}
		ost << " Orbit " << i << " / "
				<< nb_orbits << " : " << orbit_len[i];
		ost << " : ";
		print_orbit(ost, i);
		ost << endl;
		}
	ost << endl;
}

void schreier::print_and_list_orbits_using_labels(
		ostream &ost, int *labels)
{
	int i;

	ost << nb_orbits << " orbits" << endl;
	ost << "orbit group with " << gens.len << " generators:" << endl;
	ost << "i : orbit_first[i] : orbit_len[i]" << endl;
	for (i = 0; i < nb_orbits; i++) {
		ost << i << " : " << orbit_first[i]
			<< " : " << orbit_len[i];
		ost << " : ";
		print_orbit_using_labels(ost, i, labels);
		ost << endl;
		}
	ost << endl;
}

void schreier::print_tables(ostream &ost,
	int f_with_cosetrep)
{
    int i;
    int w; //  j, k;

#if 0
	ost << gens.len << " generators:" << endl;
	for (i = 0; i < A->degree; i++) {
		ost << i;
		for (j = 0; j < gens.len; j++) {
			k = A->element_image_of(i, gens.ith(j), FALSE);
			ost << " : " << k;
			}
		ost << endl;
		}
	ost << endl;
#endif
	w = (int) int_log10(A->degree) + 1;
	ost << "i : orbit[i] : orbit_inv[i] : prev[i] : label[i]";
	if (f_with_cosetrep)
		ost << " : coset_rep";
	ost << endl;
	for (i = 0; i < A->degree; i++) {
		coset_rep(i);
		//coset_rep_inv(i);
		ost << setw(w) << i << " : " << " : "
			<< setw(w) << orbit[i] << " : "
			<< setw(w) << orbit_inv[i] << " : "
			<< setw(w) << prev[i] << " : "
			<< setw(w) << label[i];
		if (f_with_cosetrep) {
			ost << " : ";
			//A->element_print(Elt1, cout);
			A->element_print_as_permutation(cosetrep, ost);
			ost << endl;
			A->element_print_quick(cosetrep, ost);
			}
		ost << endl;
		}
	ost << endl;
}

void schreier::print_tables_latex(ostream &ost,
	int f_with_cosetrep)
{
    int i;
    int w; //  j, k;

#if 0
	ost << gens.len << " generators:" << endl;
	for (i = 0; i < A->degree; i++) {
		ost << i;
		for (j = 0; j < gens.len; j++) {
			k = A->element_image_of(i, gens.ith(j), FALSE);
			ost << " : " << k;
			}
		ost << endl;
		}
	ost << endl;
#endif
	w = (int) int_log10(A->degree) + 1;
	ost << "$$" << endl;
	ost << "\\begin{array}{|c|c|c|c|c|" << endl;
	if (f_with_cosetrep) {
		ost << "c|";
		}
	ost << "}" << endl;
	ost << "\\hline" << endl;
	ost << "i & orbit & orbitinv & prev & label";
	if (f_with_cosetrep) {
		ost << "& cosetrep";
		}
	ost << "\\\\" << endl;
	ost << "\\hline" << endl;
	ost << "\\hline" << endl;
	for (i = 0; i < A->degree; i++) {
		coset_rep(i);
		//coset_rep_inv(i);
		ost << i << " & "
			<< setw(w) << orbit[i] << " & "
			<< setw(w) << orbit_inv[i] << " & "
			<< setw(w) << prev[i] << " & "
			<< setw(w) << label[i];
		if (f_with_cosetrep) {
			ost << " & ";
			//A->element_print(Elt1, cout);
			//A->element_print_as_permutation(cosetrep, ost);
			//ost << endl;
			A->element_print_latex(cosetrep, ost);
			}
		ost << "\\\\" << endl;
		ost << "\\hline" << endl;

		if (((i + 1) % 10) == 0) {
			ost << "\\end{array}" << endl;
			ost << "$$" << endl;
			ost << "$$" << endl;
			ost << "\\begin{array}{|c|c|c|c|c|" << endl;
			if (f_with_cosetrep) {
				ost << "c|";
				}
			ost << "}" << endl;
			ost << "\\hline" << endl;
			ost << "i & orbit & orbitinv & prev & label";
			if (f_with_cosetrep) {
				ost << "& cosetrep";
				}
			ost << "\\\\" << endl;
			ost << "\\hline" << endl;
			ost << "\\hline" << endl;
			}
		}
	ost << "\\end{array}" << endl;
	ost << "$$" << endl;
	ost << endl;
}

void schreier::print_generators()
{
	int j;

	cout << gens.len << " generators in action "
			<< A->label << " of degree " << A->degree << ":" << endl;
	for (j = 0; j < gens.len; j++) {
		cout << "generator " << j << ":" << endl;
		//A->element_print(gens.ith(j), cout);
		A->element_print_quick(gens.ith(j), cout);
		//A->element_print_as_permutation(gens.ith(j), cout);
		if (j < gens.len - 1) {
			cout << ", " << endl;
			}
		}
}

void schreier::print_generators_latex(ostream &ost)
{
	int j;

	ost << gens.len << " generators in action $"
			<< A->label_tex << "$ of degree "
			<< A->degree << ":\\\\" << endl;
	for (j = 0; j < gens.len; j++) {
		ost << "generator " << j << ":" << endl;
		//A->element_print(gens.ith(j), cout);
		ost << "$$" << endl;
		A->element_print_latex(gens.ith(j), ost);
		//A->element_print_as_permutation(gens.ith(j), cout);
		if (j < gens.len - 1) {
			ost << ", " << endl;
			}
		ost << "$$" << endl;
		}
}

void schreier::print_generators_with_permutations()
{
	int j;

	cout << gens.len << " generators in action "
			<< A->label << " of degree "
			<< A->degree << ":" << endl;
	for (j = 0; j < gens.len; j++) {
		cout << "generator " << j << ":" << endl;
		//A->element_print(gens.ith(j), cout);
		A->element_print_quick(gens.ith(j), cout);
		A->element_print_as_permutation(gens.ith(j), cout);
		cout << endl;
		if (j < gens.len - 1) {
			cout << ", " << endl;
			}
		}
}

void schreier::print_orbit(int orbit_no)
{
	print_orbit(cout, orbit_no);
}

void schreier::print_orbit_using_labels(
		int orbit_no, int *labels)
{
	print_orbit_using_labels(cout, orbit_no, labels);
}

void schreier::print_orbit(ostream &ost, int orbit_no)
{
	int i, first, len;
	int *v;

	first = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	v = NEW_int(len);
	for (i = 0; i < len; i++) {
		v[i] = orbit[first + i];
		}
	//int_vec_print(ost, v, len);
	//int_vec_heapsort(v, len);
	int_vec_print_fully(ost, v, len);

	FREE_int(v);
}

void schreier::print_orbit_tex(ostream &ost, int orbit_no)
{
	int i, first, len;
	int *v;

	first = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	v = NEW_int(len);
	for (i = 0; i < len; i++) {
		v[i] = orbit[first + i];
		}
	//int_vec_print(ost, v, len);
	//int_vec_heapsort(v, len);
	//int_vec_print_fully(ost, v, len);
	int_set_print_tex(ost, v, len);

	FREE_int(v);
}

void schreier::print_orbit_using_labels(ostream &ost,
	int orbit_no, int *labels)
{
	int i, first, len;
	int *v;

	first = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	v = NEW_int(len);
	for (i = 0; i < len; i++) {
		v[i] = labels[orbit[first + i]];
		}
	//int_vec_print(ost, v, len);
	int_vec_heapsort(v, len);
	int_vec_print_fully(ost, v, len);

	FREE_int(v);
}

void schreier::print_orbit_using_callback(ostream &ost,
	int orbit_no,
	void (*print_point)(ostream &ost, int pt, void *data),
	void *data)
{
	int i, first, len;

	first = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	for (i = 0; i < len; i++) {
		ost << orbit[first + i] << " which is " << endl;
		(*print_point)(ost, orbit[first + i], data);
		}
}

void schreier::print_orbit_type(int f_backwards)
{
	classify C;

	C.init(orbit_len, nb_orbits, FALSE, 0);
	C.print_naked(f_backwards);
}

void schreier::list_all_orbits_tex(ostream &ost)
{
	int i, j, f, l, a;

	ost << "$";
	for (i = 0; i < nb_orbits; i++) {
		f = orbit_first[i];
		l = orbit_len[i];
		for (j = 0; j < l; j++) {
			a = orbit[f + j];
			ost << a;
			if (j < l - 1) {
				ost << ", ";
				}
			}
		if (i < nb_orbits - 1) {
			ost << " \\mid ";
			}
		}
	ost << "$";
}

void schreier::print_orbit_through_labels(ostream &ost,
	int orbit_no, int *point_labels)
{
	int i, first, len;
	int *v;

	first = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	v = NEW_int(len);
	for (i = 0; i < len; i++) {
		v[i] = point_labels[orbit[first + i]];
		}
	int_vec_heapsort(v, len);
	int_vec_print_fully(ost, v, len);
	FREE_int(v);
}

void schreier::print_orbit_sorted(ostream &ost, int orbit_no)
{
	int i, len;
	int *v;

	len = orbit_first[orbit_no + 1] - orbit_first[orbit_no];
	v = NEW_int(len);
	for (i = 0; i < len; i++) {
		v[i] = orbit[orbit_first[orbit_no] + i];
		}
	int_vec_sort(len, v);

	ost << "{ ";
	for (i = 0; i < len; i++) {
		if (f_print_function) {
			ost << v[i] << "=";
			(*print_function)(ost, v[i], print_function_data);
			}
		else {
			ost << v[i];
			}
		if (i < len - 1)
			ost << ", ";
		}
	ost << " }";
	FREE_int(v);
}

void schreier::print_orbit(int cur, int last)
{
	int i;

	for (i = 0; i < A->degree; i++) {
		if (i == cur)
			cout << ">";
		if (i == last)
			cout << ">";
		cout << i << " : " << orbit[i]
			<< " : " << orbit_inv[i] << endl;
		}
	cout << endl;
}

void schreier::print_tree(int orbit_no)
{
	int *path;
	int i, j, l;

	path = NEW_int(A->degree);
	i = orbit_first[orbit_no];
	while (i < orbit_first[orbit_no + 1]) {
		trace_back(path, orbit[i], l);
		// now l is the distance from the root
		cout << l;
		for (j = 0; j < l; j++) {
			cout << " " << path[j];
			}
		cout << " 0 ";
		if (label[i] != -1) {
			cout << " $s_{" << label[i] << "}$";
			}
		cout << endl;
		i++;
		}
	FREE_int(path);
}

void schreier::draw_forest(const char *fname_mask,
	int xmax, int ymax,
	int f_circletext, int rad,
	int f_embedded, int f_sideways,
	double scale, double line_width,
	int f_has_point_labels, int *point_labels,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	char fname[1000];
	int i;

	if (f_v) {
		cout << "schreier::draw_forest" << endl;
		}
	for (i = 0; i < nb_orbits; i++) {
		sprintf(fname, fname_mask, i);

		if (f_v) {
			cout << "schreier::draw_forest drawing orbit "
					<< i << " / " << nb_orbits << endl;
			}
		draw_tree(fname,
			i /* orbit_no */, xmax, ymax,
			f_circletext, rad,
			f_embedded, f_sideways,
			scale, line_width,
			f_has_point_labels, point_labels,
			verbose_level);
		}
	if (f_v) {
		cout << "schreier::draw_forest done" << endl;
		}
}

void schreier::export_tree_as_layered_graph(int orbit_no,
		const char *fname_mask,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	int fst, len;
	int *depth;
	int *horizontal_position;
	int i, j, l, max_depth;

	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph" << endl;
		cout << "schreier::export_tree_as_layered_graph "
				"nb_gen = " << gens.len << endl;
		}

	fst = orbit_first[orbit_no];
	len = orbit_len[orbit_no];
	depth = NEW_int(len);
	horizontal_position = NEW_int(len);
	max_depth = 0;
	for (j = 0; j < len; j++) {
		trace_back(NULL, orbit[fst + j], l);
		l--;
		depth[j] = l;
		max_depth = MAX(max_depth, l);
		}
	int nb_layers;
	nb_layers = max_depth + 1;
	int *Nb;
	int *Nb1;
	int **Node;


	//classify C;
	//C.init(depth, len, FALSE, 0);
	Nb = NEW_int(nb_layers);
	Nb1 = NEW_int(nb_layers);
	int_vec_zero(Nb, nb_layers);
	int_vec_zero(Nb1, nb_layers);
	for (j = 0; j < len; j++) {
		trace_back(NULL, orbit[fst + j], l);
		l--;
		horizontal_position[j] = Nb[l];
		Nb[l]++;
	}
	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph" << endl;
		cout << "number of nodes at depth:" << endl;
		for (i = 0; i <= max_depth; i++) {
			cout << i << " : " << Nb[i] << endl;
		}
	}
	Node = NEW_pint(nb_layers);
	for (i = 0; i <= max_depth; i++) {
		Node[i] = NEW_int(Nb[i]);
	}
	for (j = 0; j < len; j++) {
		trace_back(NULL, orbit[fst + j], l);
		l--;
		Node[l][Nb1[l]] = j;
		Nb1[l]++;
	}

	layered_graph *LG;
	int n1, n2, j2;

	LG = NEW_OBJECT(layered_graph);
	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph "
				"before LG->init" << endl;
		}
	//LG->add_data1(data1, 0/*verbose_level*/);
	LG->init(nb_layers, Nb, "", verbose_level);
	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph "
				"after LG->init" << endl;
		}
	LG->place(verbose_level);
	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph "
				"after LG->place" << endl;
		}
	for (i = 0; i <= max_depth; i++) {
		if (f_vv) {
			cout << "schreier::export_tree_as_layered_graph "
					"adding edges at depth "
					"i=" << i << " / " << max_depth
					<< " Nb[i]=" << Nb[i] << endl;
		}
		for (j = 0; j < Nb[i]; j++) {
			n1 = Node[i][j];
			if (f_vvv) {
				cout << "schreier::export_tree_as_layered_graph "
						"adding edges "
						"i=" << i << " / " << max_depth
						<< " j=" << j << " n1=" << n1 << endl;
			}
			if (prev[fst + n1] != -1) {
				n2 = orbit_inv[prev[fst + n1]] - fst;
				j2 = horizontal_position[n2];
				if (f_vvv) {
					cout << "schreier::export_tree_as_layered_graph "
							"adding edges "
							"i=" << i << " / " << max_depth
							<< " j=" << j << " n1=" << n1
							<< " n2=" << n2 << " j2=" << j2 << endl;
				}
				if (f_vvv) {
					cout << "adding edge ("<< i - 1 << "," << j2 << ") "
							"-> (" << i << "," << j << ")" << endl;
				}
				LG->add_edge(i - 1, j2, i, j,
						0 /*verbose_level*/);
			}
		}
	}
	for (j = 0; j < len; j++) {
		char text[1000];

		sprintf(text, "%d", orbit[fst + j]);
		trace_back(NULL, orbit[fst + j], l);
		l--;
		LG->add_text(l, horizontal_position[j],
				text, 0/*verbose_level*/);
	}
	char fname[1000];

	sprintf(fname, fname_mask, orbit_no);
	LG->write_file(fname, 0 /*verbose_level*/);
	delete LG;

	FREE_int(Nb);
	FREE_int(Nb1);
	FREE_int(depth);
	FREE_int(horizontal_position);

	if (f_v) {
		cout << "schreier::export_tree_as_layered_graph done" << endl;
		}
}

void schreier::draw_tree(const char *fname,
	int orbit_no, int xmax, int ymax,
	int f_circletext, int rad,
	int f_embedded, int f_sideways,
	double scale, double line_width,
	int f_has_point_labels, int *point_labels,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *path;
	int *weight;
	int *placement_x;
	int i, j, last, max_depth = 0;


	if (f_v) {
		cout << "schreier::draw_tree" << endl;
		}
	path = NEW_int(A->degree);
	weight = NEW_int(A->degree);
	placement_x = NEW_int(A->degree);

	i = orbit_first[orbit_no];
	last = orbit_first[orbit_no + 1];

	for (j = 0; j < A->degree; j++) {
		weight[j] = 0;
		placement_x[j] = 0;
		}
	subtree_calc_weight(weight, max_depth, i, last);
	if (f_vv) {
		cout << "the weights: " << endl;
		for (j = i; j < last; j++) {
			cout << j << " : " << weight[j] << " : " << endl;
			}
		cout << endl;
		cout << "max_depth = " << max_depth << endl;
		}
	subtree_place(weight, placement_x, 0, 1000000, i, last);
	if (f_vv) {
		for (j = i; j < last; j++) {
			cout << j << " : " << placement_x[j] << endl;
			}
		cout << endl;
		}
	if (orbit_len[orbit_no] > 100) {
		f_circletext = FALSE;
		}
	draw_tree2(fname, xmax, ymax, f_circletext,
		weight, placement_x, max_depth, i, last, rad,
		f_embedded, f_sideways,
		scale, line_width,
		f_has_point_labels, point_labels,
		verbose_level - 2);

	FREE_int(path);
	FREE_int(weight);
	FREE_int(placement_x);
	if (f_v) {
		cout << "schreier::draw_tree done" << endl;
		}
}

static void calc_y_coordinate(int &y, int l, int max_depth)
{
	int dy;

	dy = (int)((double)1000000 / (double)max_depth);
	y = (int)(dy * ((double)l + 0.5));
	y = 1000000 - y;
}

void schreier::draw_tree2(const char *fname,
	int xmax, int ymax, int f_circletext,
	int *weight, int *placement_x,
	int max_depth, int i, int last, int rad,
	int f_embedded, int f_sideways,
	double scale, double line_width,
	int f_has_point_labels, int *point_labels,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int x_min = 0, x_max = 1000000;
	int y_min = 0, y_max = 1000000;
	int factor_1000 = 1000;
	char fname_full[1000];

	if (f_v) {
		cout << "schreier::draw_tree2" << endl;
		}
	sprintf(fname_full, "%s.mp", fname);
	mp_graphics G(fname_full, x_min, y_min, x_max, y_max,
		f_embedded, f_sideways);
	G.out_xmin() = 0;
	G.out_ymin() = 0;
	G.out_xmax() = xmax;
	G.out_ymax() = ymax;
	G.set_parameters(scale, line_width);

	G.header();
	G.begin_figure(factor_1000);

	int x = 500000, y;
	calc_y_coordinate(y, 0, max_depth);


	subtree_draw_lines(G, f_circletext, x, y, weight,
		placement_x, max_depth, i, last, verbose_level);

	subtree_draw_vertices(G, f_circletext, x, y, weight,
		placement_x, max_depth, i, last, rad,
		f_has_point_labels, point_labels,
		verbose_level);

	int j, L, l, N;
	double avg;

	N = last - i;
	L = 0;
	for (j = i; j < last; j++) {
		trace_back(NULL, orbit[j], l);
		L += l;
		}
	avg = (double) L / (double)N;
	x = 500000;
	calc_y_coordinate(y, max_depth + 1, max_depth);
	char str[1000];
	int nb_gens;
	double H; // entropy

	nb_gens = gens.len;
	if (nb_gens) {
		H = log(N) / log(nb_gens);
		}
	else {
		H = 0.;
		}
	sprintf(str, "N=%d, avg=%lf,  gens=%d, H=%lf", N, avg, nb_gens, H);
	G.aligned_text(x, y, "", str);


#if 0
	if (f_circletext) {
		G.circle_text(x, y, rad, "$\\emptyset$");
		}
	else {
		G.circle_text(x, y, rad, "");
		//G.circle(x, y, rad);
		}
#endif

	G.end_figure();
	//print_and_list_orbits_tex(ostream &ost)
	G.footer();
	if (f_v) {
		cout << "schreier::draw_tree2 done" << endl;
		}
}

void schreier::subtree_draw_lines(mp_graphics &G,
	int f_circletext, int parent_x, int parent_y, int *weight,
	int *placement_x, int max_depth, int i, int last,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int pt = orbit[i];
	int x, y, l, ii;
	int Px[3], Py[3];

	if (f_v) {
		cout << "schreier::subtree_draw_lines" << endl;
		}
	trace_back(NULL, pt, l);
	// l is 1 if pt is the root.
	x = placement_x[pt];
	calc_y_coordinate(y, l, max_depth);

	//G.circle(x, y, 2000);
	Px[0] = parent_x;
	Py[0] = parent_y;
	Px[1] = x;
	Py[1] = y;
	Px[2] = (Px[0] + Px[1]) >> 1;
	Py[2] = (Py[0] + Py[1]) >> 1;
	//cout << "schreier::subtree_draw_lines "
	// << parent_x << "," << parent_y << " - "
	// << x << "," << y << endl;


#if 0
	int y1;
	calc_y_coordinate(y1, 0, max_depth);
	if (parent_x == 500000 && parent_y == y1) {
		}
	else {
		G.polygon2(Px, Py, 0, 1);
		}
#endif
	if (l > 1) {
		char str[1000];
		// if pt is not the root node:
		G.polygon2(Px, Py, 0, 1);
		sprintf(str, "$\\alpha_{%d}$", label[i]);
		G.aligned_text(Px[2], Py[2], "", str);
		}

	for (ii = i + 1; ii < last; ii++) {
		if (prev[ii] == pt) {
			subtree_draw_lines(G, f_circletext,
					x, y, weight, placement_x,
					max_depth, ii, last, verbose_level);
			}
		}

	if (f_v) {
		cout << "schreier::subtree_draw_lines done" << endl;
		}
}

void schreier::subtree_draw_vertices(mp_graphics &G,
	int f_circletext, int parent_x, int parent_y, int *weight,
	int *placement_x, int max_depth, int i, int last, int rad,
	int f_has_point_labels, int *point_labels,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int pt = orbit[i];
	int x, y, l, ii;
	//int Px[2], Py[2];
	char str[1000];

	if (f_v) {
		cout << "schreier::subtree_draw_vertices" << endl;
		}
	trace_back(NULL, pt, l);
	x = placement_x[pt];
	calc_y_coordinate(y, l, max_depth);

#if 0
	Px[0] = parent_x;
	Py[0] = parent_y;
	Px[1] = x;
	Py[1] = y;
	//cout << "schreier::subtree_draw_vertices "
	// << parent_x << "," << parent_y << " - " << x << "," << y << endl;
	//G.polygon2(Px, Py, 0, 1);
#endif

	for (ii = i + 1; ii < last; ii++) {
		if (prev[ii] == pt) {
			subtree_draw_vertices(G, f_circletext,
				x, y, weight, placement_x,
				max_depth, ii, last, rad,
				f_has_point_labels, point_labels,
				verbose_level);
			}
		}
#if 0
	if (pt == 169303 || pt == 91479) {
		G.circle(x, y, 4 * rad);
		}
#endif
	if (f_has_point_labels) {
		sprintf(str, "%d", point_labels[pt]);
		}
	else {
		sprintf(str, "%d", pt);
		}
	if (f_circletext) {
		G.circle_text(x, y, rad, str);
		}
	else {
		G.circle_text(x, y, rad, "");
		//G.circle(x, y, rad);
		//G.aligned_text(Px, Py, 1, "tl", str);
		}
	if (f_v) {
		cout << "schreier::subtree_draw_vertices done" << endl;
		}
}

void schreier::subtree_place(int *weight, int *placement_x,
	int left, int right, int i, int last)
{
	int pt = orbit[i];
	int ii, l, w, w0, w1, lft, rgt, width;
	double dx;

	placement_x[pt] = (left + right) >> 1;
	w = weight[pt];
	width = right - left;
	dx = width / (double) (w - 1);
		// the node itself counts for the weight, so we subtract one
	w0 = 0;

	trace_back(NULL, pt, l);
	for (ii = i + 1; ii < last; ii++) {
		if (prev[ii] == pt) {
			w1 = weight[orbit[ii]];
			lft = left + (int)((double)w0 * dx);
			rgt = left + (int)((double)(w0 + w1) * dx);
			subtree_place(weight, placement_x, lft, rgt, ii, last);
			w0 += w1;
			}
		}
}

int schreier::subtree_calc_weight(int *weight,
	int &max_depth, int i, int last)
{
	int pt = orbit[i];
	int ii, l, w = 1, w1;

	trace_back(NULL, pt, l);
	if (l > max_depth)
		max_depth = l;
	for (ii = i + 1; ii < last; ii++) {
		if (prev[ii] == pt) {
			w1 = subtree_calc_weight(weight, max_depth, ii, last);
			w += w1;
			}
		}
	weight[pt] = w;
	return w;
}

int schreier::subtree_depth_first(ostream &ost,
		int *path, int i, int last)
{
	int pt = orbit[i];
	int ii, l, w = 1, w1;

	for (ii = i + 1; ii < last; ii++) {
		if (prev[ii] == pt) {


			trace_back(path, orbit[ii], l);
			// now l is the distance from the root
			print_path(ost, path, l);

			w1 = subtree_depth_first(ost, path, ii, last);
			w += w1;
			}
		}
	return w;
}

void schreier::print_path(ostream &ost, int *path, int l)
{
	int j;

	ost << l;
	for (j = 0; j < l; j++) {
		ost << " " << path[j];
		}
	ost << endl;
}

void schreier::write_to_memory_object(
		memory_object *m, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "schreier::write_to_memory_object" << endl;
		}
	m->write_int(0); // indicator
	m->write_int(1); // version
	m->write_int(A->degree);
	m->write_int(nb_orbits);
	for (i = 0; i < nb_orbits; i++) {
		m->write_int(orbit_first[i]);
		m->write_int(orbit_len[i]);
		}
	for (i = 0; i < A->degree; i++) {
		m->write_int(orbit[i]);
		m->write_int(prev[i]);
		m->write_int(label[i]);
		//m->write_int(orbit_no[i]);
		}
	gens.write_to_memory_object(m, verbose_level - 1);
	gens_inv.write_to_memory_object(m, verbose_level - 1);
	if (f_v) {
		cout << "schreier::write_to_memory_object done" << endl;
		}
}

void schreier::read_from_memory_object(
		memory_object *m, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, deg, dummy, a, version;

	if (f_v) {
		cout << "schreier::read_from_memory_object" << endl;
		}
	init2();
	m->read_int(&a);
	if (a == 0) {
		m->read_int(&version);
		m->read_int(&deg);
		}
	else {
		version = 0;
		deg = a;
		}
	m->read_int(&nb_orbits);
	if (deg != A->degree) {
		cout << "schreier::read_from_memory_object "
				"deg != A->degree" << endl;
		}
	orbit_first = NEW_int(nb_orbits);
	orbit_len = NEW_int(nb_orbits);
	for (i = 0; i < nb_orbits; i++) {
		m->read_int(&orbit_first[i]);
		m->read_int(&orbit_len[i]);
		}
	orbit = NEW_int(A->degree);
	orbit_inv = NEW_int(A->degree);
	prev = NEW_int(A->degree);
	label = NEW_int(A->degree);
	//orbit_no = NEW_int(A->degree);
	for (i = 0; i < A->degree; i++) {
		m->read_int(&orbit[i]);
		m->read_int(&prev[i]);
		m->read_int(&label[i]);
		m->read_int(&dummy);
		if (version == 0) {
			m->read_int(&dummy);
			}
		//m->read_int(&orbit_no[i]);
		}
	perm_inverse(orbit, orbit_inv, A->degree);
	gens.init(A);
	gens.read_from_memory_object(m, verbose_level - 1);
	gens_inv.init(A);
	gens_inv.read_from_memory_object(m, verbose_level - 1);
	if (f_v) {
		cout << "schreier::read_from_memory_object done" << endl;
		}
}

void schreier::write_file(char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	memory_object M;

	if (f_v) {
		cout << "schreier::write_file" << endl;
		}
	M.alloc(1024 /* length */, verbose_level - 1);
	M.used_length = 0;
	M.cur_pointer = 0;
	write_to_memory_object(&M, verbose_level - 1);
	M.write_file(fname, verbose_level - 1);
	if (f_v) {
		cout << "schreier::write_file done" << endl;
		}
}

void schreier::read_file(const char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	memory_object M;

	if (f_v) {
		cout << "schreier::read_file reading file "
				<< fname << " of size " << file_size(fname) << endl;
		}
	M.read_file(fname, verbose_level - 1);
	if (f_v) {
		cout << "schreier::read_file read file " << fname << endl;
		}
	M.cur_pointer = 0;
	read_from_memory_object(&M, verbose_level - 1);
	if (f_v) {
		cout << "schreier::read_file done" << endl;
		}
}

void schreier::write_to_file_binary(ofstream &fp, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, a = 0, version = 1;

	if (f_v) {
		cout << "schreier::write_to_file_binary" << endl;
		}
	fp.write((char *) &a, sizeof(int));
	fp.write((char *) &version, sizeof(int));
	fp.write((char *) &A->degree, sizeof(int));
	fp.write((char *) &nb_orbits, sizeof(int));
	for (i = 0; i < nb_orbits; i++) {
		fp.write((char *) &orbit_first[i], sizeof(int));
		fp.write((char *) &orbit_len[i], sizeof(int));
		}
	for (i = 0; i < A->degree; i++) {
		fp.write((char *) &orbit[i], sizeof(int));
		fp.write((char *) &prev[i], sizeof(int));
		fp.write((char *) &label[i], sizeof(int));
		//fp.write((char *) &orbit_no[i], sizeof(int));
		}
	gens.write_to_file_binary(fp, verbose_level - 1);
	gens_inv.write_to_file_binary(fp, verbose_level - 1);
	if (f_v) {
		cout << "schreier::write_to_file_binary done" << endl;
		}
}

void schreier::read_from_file_binary(ifstream &fp, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, deg, dummy, a, version;

	if (f_v) {
		cout << "schreier::read_from_file_binary" << endl;
		}
	init2();
	fp.read((char *) &a, sizeof(int));
	if (a == 0) {
		fp.read((char *) &version, sizeof(int));
		fp.read((char *) &deg, sizeof(int));
		}
	else {
		version = 0;
		deg = a;
		}
	//fp.read((char *) &deg, sizeof(int));
	fp.read((char *) &nb_orbits, sizeof(int));
	if (deg != A->degree) {
		cout << "schreier::read_from_file_binary "
				"deg != A->degree" << endl;
		}
	orbit_first = NEW_int(nb_orbits);
	orbit_len = NEW_int(nb_orbits);
	for (i = 0; i < nb_orbits; i++) {
		fp.read((char *) &orbit_first[i], sizeof(int));
		fp.read((char *) &orbit_len[i], sizeof(int));
		}
	orbit = NEW_int(A->degree);
	orbit_inv = NEW_int(A->degree);
	prev = NEW_int(A->degree);
	label = NEW_int(A->degree);
	//orbit_no = NEW_int(A->degree);
	for (i = 0; i < A->degree; i++) {
		fp.read((char *) &orbit[i], sizeof(int));
		fp.read((char *) &prev[i], sizeof(int));
		fp.read((char *) &label[i], sizeof(int));
		if (version == 0) {
			fp.read((char *) &dummy, sizeof(int));
			//fp.read((char *) &orbit_no[i], sizeof(int));
			}
		}
	perm_inverse(orbit, orbit_inv, A->degree);

	gens.init(A);
	gens.read_from_file_binary(fp, verbose_level - 1);
	gens_inv.init(A);
	gens_inv.read_from_file_binary(fp, verbose_level - 1);
	if (f_v) {
		cout << "schreier::read_from_file_binary done" << endl;
		}
}


void schreier::write_file_binary(char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier::write_file_binary" << endl;
		}
	{
		ofstream fp(fname, ios::binary);

		write_to_file_binary(fp, verbose_level - 1);
	}
	cout << "schreier::write_file_binary Written file "
			<< fname << " of size " << file_size(fname) << endl;
	if (f_v) {
		cout << "schreier::write_file_binary done" << endl;
		}
}

void schreier::read_file_binary(const char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier::read_file_binary reading file "
				<< fname << " of size " << file_size(fname) << endl;
		}
	cout << "schreier::read_file_binary Reading file "
			<< fname << " of size " << file_size(fname) << endl;
	{
		ifstream fp(fname, ios::binary);

		read_from_file_binary(fp, verbose_level - 1);
	}
	if (f_v) {
		cout << "schreier::read_file_binary done" << endl;
		}
}
