// fancy_set.C
//
// Anton Betten
//
// June 29, 2010

#include "foundations.h"

fancy_set::fancy_set()
{
	null();
}

fancy_set::~fancy_set()
{
	freeself();
}

void fancy_set::null()
{
	n = k = 0;
	set = NULL;
	set_inv = NULL;
}

void fancy_set::freeself()
{
	if (set) {
		FREE_int(set);
		}
	if (set_inv) {
		FREE_int(set_inv);
		}
	null();
}

void fancy_set::init(int n, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	
	if (f_v) {
		cout << "fancy_set::init n=" << n << endl;
		}
	fancy_set::n = n;
	fancy_set::k = 0;
	set = NEW_int(n);
	set_inv = NEW_int(n);
	for (i = 0; i < n; i++) {
		set[i] = i;
		set_inv[i] = i;
		}
}

void fancy_set::init_with_set(int n, int k, int *subset, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	
	if (f_v) {
		cout << "fancy_set::init_with_set n=" << n << " k=" << k << endl;
		cout << "set=";
		int_set_print(cout, subset, k);
		cout << endl;
		}
	init(n, verbose_level - 1);
	for (i = 0; i < k; i++) {
		swap(i, subset[i]);
		}
	if (f_v) {
		cout << "fancy_set::init_with_set done: ";
		println();
		}
}

void fancy_set::print()
{
	int i;
	
	cout << "{ ";
	for (i = 0; i < k; i++) {
		cout << set[i];
		if (i < k - 1) {
			cout << ", ";
			}
		}
	cout << " }";
}

void fancy_set::println()
{
	print();
	cout << endl;
}

void fancy_set::swap(int pos, int a)
{
	int b, pos_a;

	pos_a = set_inv[a];
	if (pos_a == pos)
		return;
	b = set[pos];
	set[pos] = a;
	set[pos_a] = b;
	set_inv[a] = pos;
	set_inv[b] = pos_a;
}

int fancy_set::is_contained(int a)
{
	int pos_a;

	pos_a = set_inv[a];
	if (pos_a < k) {
		return TRUE;
		}
	else {
		return FALSE;
		}
}

void fancy_set::copy_to(fancy_set *to)
{
	int i;
	
	if (to->n != n) {
		cout << "to->n != n" << endl;
		exit(1);
		}
	to->k = k;
	for (i = 0; i < n; i++) {
		to->set[i] = set[i];
		}
	for (i = 0; i < n; i++) {
		to->set_inv[i] = set_inv[i];
		}
}

void fancy_set::add_element(int elt)
{
	if (!is_contained(elt)) {
		swap(k, elt);
		k++;
		}
}

void fancy_set::add_elements(int *elts, int nb)
{
	int i;
	
	for (i = 0; i < nb; i++) {
		add_element(elts[i]);
		}
}

void fancy_set::delete_elements(int *elts, int nb)
{
	int i;
	
	for (i = 0; i < nb; i++) {
		if (is_contained(elts[i])) {
			swap(k - 1, elts[i]);
			k--;
			}
		}
}

void fancy_set::delete_element(int elt)
{
	if (is_contained(elt)) {
		swap(k - 1, elt);
		k--;
		}
}

void fancy_set::select_subset(int *elts, int nb)
{
	int i;
	
	for (i = 0; i < nb; i++) {
		if (!is_contained(elts[i])) {
			cout << "fancy_set::select_subset  element is not contained" << endl;
			exit(1);
			}
		swap(i, elts[i]);
		}
	k = nb;
}

void fancy_set::intersect_with(int *elts, int nb)
{
	int i, l;
	
	l = 0;
	for (i = 0; i < nb; i++) {
		if (is_contained(elts[i])) {
			swap(l, elts[i]);
			l++;
			}
		}
	k = l;
}

void fancy_set::subtract_set(fancy_set *set_to_subtract)
{
	int i, a;
	
	if (k < set_to_subtract->k) {
		for (i = 0; i < k; i++) {
			a = set[i];
			if (set_to_subtract->is_contained(a)) {
				swap(k - 1, a);
				k--;
				i--; // do the current position one more time
				}
			}
		}
	else {
		for (i = 0; i < set_to_subtract->k; i++) {
			a = set_to_subtract->set[i];
			if (is_contained(a)) {
				swap(k - 1, a);
				k--;
				}
			}
		}
}

void fancy_set::sort()
{
	int i, a;
	
	int_vec_heapsort(set, k);
	for (i = 0; i < k; i++) {
		a = set[i];
		set_inv[a] = i;
		}
}
	
int fancy_set::compare_lexicographically(fancy_set *second_set)
{
	sort();
	second_set->sort();
	return ::compare_lexicographically(k, set, second_set->k, second_set->set);
	
}

void fancy_set::complement(fancy_set *compl_set)
{
	int i, a;
	
	if (compl_set->n != n) {
		cout << "fancy_set::complement compl_set->n != n" << endl;
		exit(1);
		}
	compl_set->k = n - k;
	for (i = 0; i < n - k; i++) {
		a = set[k + i];
		compl_set->set[i] = a;
		compl_set->set_inv[a] = i;
		}
	for (i = 0; i < k; i++) {
		a = set[i];
		compl_set->set[n - k + i] = a;
		compl_set->set_inv[a] = n - k + i;
		}
}

int fancy_set::is_subset(fancy_set *set2)
{
	int i, a;
	
	if (set2->k < k)
		return FALSE;
	for (i = 0; i < k; i++) {
		a = set[i];
		if (!set2->is_contained(a))
			return FALSE;
		}
	return TRUE;
}

int fancy_set::is_equal(fancy_set *set2)
{
	if (is_subset(set2) && k == set2->k) {
		return TRUE;
		}
	return FALSE;
}





