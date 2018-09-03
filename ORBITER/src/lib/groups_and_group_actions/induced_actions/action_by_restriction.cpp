// action_by_restriction.C
//
// Anton Betten
// February 20, 2010

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

action_by_restriction::action_by_restriction()
{
	null();
}

action_by_restriction::~action_by_restriction()
{
	free();
}

void action_by_restriction::null()
{
	points = NULL;
	points_sorted = NULL;
	perm_inv = NULL;
}

void action_by_restriction::free()
{
	if (points) {
		FREE_int(points);
		}
	if (points_sorted) {
		FREE_int(points_sorted);
		}
	if (perm_inv) {
		FREE_int(perm_inv);
		}
	null();
}

void action_by_restriction::init_from_sv(int *sv,
		int pt, int verbose_level)
{
	int i, idx;
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	
	if (f_v) {
		cout << "action_by_restriction::init_from_sv "
				"pt=" << pt << endl;
		}
	int n;
	int *pts;
	int *prev;
	int *label;
	int *depth;
	int *ancestor;

	int *orbit_elt_idx;
	int *orbit_elts;
	int orbit_len;

	n = sv[0];
	pts = sv + 1;
	prev = pts + n;
	label = prev + n;
	if (f_v) {
		cout << "action_by_restriction::init_from_sv "
				"schreier vector of length " << n << endl;
		}

	if (!int_vec_search(pts, n, pt, idx)) {
		cout << "action_by_restriction::init_from_sv "
				"fatal: point " << pt << " not found" << endl;
		exit(1);
		}

	depth = NEW_int(n);	
	ancestor = NEW_int(n);	
	orbit_elt_idx = NEW_int(n);	
	
	for (i = 0; i < n; i++) {
		depth[i] = -1;
		ancestor[i] = -1;
		}
	if (f_vv) {
		cout << "action_by_restriction::init_from_sv "
				"determining depth using schreier_vector_determine_"
				"depth_recursion" << endl;
		}
	for (i = 0; i < n; i++) {
		schreier_vector_determine_depth_recursion(n,
				pts, prev, depth, ancestor, i);
		}
	if (f_vv) {
		cout << "action_by_restriction::init_from_sv "
				"determining depth using schreier_vector_"
				"determine_depth_recursion done" << endl;
		}
	if (f_vvv && n < 100) {
		cout << "i : pts[i] : prev[i] : label[i] : "
				"depth[i] : ancestor[i]" << endl;
		for (i = 0; i < n; i++) {
			cout 
				<< setw(5) << i << " : " 
				<< setw(5) << pts[i] << " : " 
				<< setw(5) << prev[i] << " : " 
				<< setw(5) << label[i] << " : " 
				<< setw(5) << depth[i] << " : " 
				<< setw(5) << ancestor[i] 
				<< endl;
			}
		}
	orbit_len = 0;
	for (i = 0; i < n; i++) {
		if (ancestor[i] == pt) {
			orbit_elt_idx[orbit_len++] = i;
			}
		}
	if (f_v) {
		cout << "action_by_restriction::init_from_sv "
				"found orbit of length " << orbit_len << endl;
		}
	orbit_elts = NEW_int(orbit_len);
	for (i = 0; i < orbit_len; i++) {
		orbit_elts[i] = pts[orbit_elt_idx[i]];
		}
	if (f_vv) {
		cout << "the points in the orbit are: ";
		int_vec_print(cout, orbit_elts, orbit_len);
		cout << endl;
		}	
	if (orbit_elts[0] != pt) {
		cout << "action_by_restriction::init_from_sv "
				"fatal: orbit_elts[0] != pt" << endl;
		exit(1);
		}
	for (i = 1; i < orbit_len; i++) {
		if (orbit_elts[i] < orbit_elts[i - 1]) {
			cout << "action_by_restriction::init_from_sv "
					"fatal: orbit_elts[] not increasing" << endl;
			exit(1);
			}
		}
	init(orbit_len, orbit_elts, verbose_level);
	
	FREE_int(depth);
	FREE_int(ancestor);
	FREE_int(orbit_elt_idx);
	FREE_int(orbit_elts);
}

void action_by_restriction::init(int nb_points, int *points,
		int verbose_level)
// the array points must be orderd
{
	int i;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "action_by_restriction::init nb_points="
				<< nb_points << endl;
		}
	action_by_restriction::nb_points = nb_points;
	action_by_restriction::points = NEW_int(nb_points);
	action_by_restriction::points_sorted = NEW_int(nb_points);
	action_by_restriction::perm_inv = NEW_int(nb_points);
	for (i = 0; i < nb_points; i++) {
		action_by_restriction::points[i] = points[i];
		points_sorted[i] = points[i];
		perm_inv[i] = i;
		}
	int_vec_heapsort_with_log(points_sorted, perm_inv, nb_points);
	if (f_v) {
		cout << "action_by_restriction::init finished" << endl;
		}
}

int action_by_restriction::compute_image(
		action *A, int *Elt, int i, int verbose_level)
{
	int idx, b, c, h;
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "action_by_restriction::compute_image i = " << i << endl;
		}
	if (i < 0 || i >= nb_points) {
		cout << "action_by_restriction::compute_image "
				"i = " << i << " out of range" << endl;
		exit(1);
		}
	if (f_vv) {
		cout << "under the group element " << endl;
		A->element_print_quick(Elt, cout);
		cout << endl;
		}
	if (f_vv) {
		cout << "points[i]=" << points[i] << endl;
		}
	b = A->element_image_of(points[i], Elt, verbose_level - 2);
	if (f_vv) {
		cout << "image of " << points[i] << " is " << b << endl;
		}
	if (!int_vec_search(points_sorted, nb_points, b, idx)) {
		cout << "action_by_restriction::compute_image fatal: "
				"image point " << b << " not found" << endl;
		cout << "action: ";
		A->print_info();

		cout << "the element " << endl;
		A->element_print_quick(Elt, cout);
		cout << endl;
		//cout << "as permutation:" << endl;
		//A->print_as_permutation(cout, Elt);
		//cout << endl;

		cout << "i=" << i << endl;
		cout << "image of " << points[i] << " is " << b << endl;
		cout << "nb_points=" << nb_points << endl;
		cout << "points=";
		int_vec_print(cout, points, nb_points);
		cout << endl;
		cout << "points_sorted=" << endl;
		for (h = 0; h < nb_points; h++) {
			cout << h << " : " << points_sorted[h] << endl;
			}
		//int_vec_print(cout, points_sorted, nb_points);
		//cout << endl;

		cout << "We compute the image point again, "
				"this time with more output" << endl;
		b = A->element_image_of(points[i],
				Elt, 10 /* verbose_level - 2*/);
		exit(1);
		}
	if (f_v) {
		cout << "action_on_sets::compute_image idx = " << idx << endl;
		}
	c = perm_inv[idx];
	if (f_v) {
		cout << "action_on_sets::compute_image c = " << c << endl;
		}
	return c;
}



