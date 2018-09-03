// product_action.C
//
// Anton Betten
// December 19, 2007

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"

product_action::product_action()
{
	null();
}

product_action::~product_action()
{
	free();
}

void product_action::null()
{
	A1 = NULL;
	A2 = NULL;
	Elt1 = NULL;
	Elt2 = NULL;
	Elt3 = NULL;
	elt1 = NULL;
	elt2 = NULL;
	elt3 = NULL;
	offset = 0;
}

void product_action::free()
{
	if (A1) {
		FREE_OBJECT(A1);
		}
	if (A2) {
		FREE_OBJECT(A2);
		}
	if (Elt1)
		FREE_int(Elt1);
	if (Elt2)
		FREE_int(Elt2);
	if (Elt3)
		FREE_int(Elt3);
	if (elt1)
		FREE_uchar(elt1);
	if (elt2)
		FREE_uchar(elt1);
	if (elt3)
		FREE_uchar(elt1);
	null();
}


void product_action::init(action *A1, action *A2, int f_use_projections, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	
	if (f_v) {
		cout << "product_action::init" << endl;
		}
	product_action::f_use_projections = f_use_projections;
	product_action::A1 = A1;
	product_action::A2 = A2;
	if (f_use_projections)
		offset = A1->degree + A2->degree;
	else	
		offset = 0;
	degree = offset + A1->degree * A2->degree;
	elt_size_in_int = A1->elt_size_in_int + A2->elt_size_in_int;
	coded_elt_size_in_char = A1->coded_elt_size_in_char + A2->coded_elt_size_in_char;

	Elt1 = NEW_int(elt_size_in_int);
	Elt2 = NEW_int(elt_size_in_int);
	Elt3 = NEW_int(elt_size_in_int);
	elt1 = NEW_uchar(coded_elt_size_in_char);
	elt2 = NEW_uchar(coded_elt_size_in_char);
	elt3 = NEW_uchar(coded_elt_size_in_char);


	Elts = NEW_OBJECT(page_storage);
	if (f_vv) {
		cout << "matrix_group::init_linear() calling Elts->init()" << endl;
		}
	Elts->init(coded_elt_size_in_char /* entry_size */, PAGE_LENGTH_LOG, f_vv);
	//Elts->add_elt_print_function(elt_print, (void *) this);
}

int product_action::compute_image(action *A, int *Elt, int i, int verbose_level)
{
	//verbose_level = 1;
	int x, y, xx, yy, j;
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);

	if (f_vv) {
		cout << "product_action::compute_image i = " << i << endl;
		}
	if (f_use_projections) {
		if (i < offset) {
			if (i < A1->degree) {
				j = A1->element_image_of(i, Elt, FALSE);
				}
			else {
				i -= A1->degree;
				j = A2->element_image_of(i, Elt + A1->elt_size_in_int, FALSE);
				j += A1->degree;
				}
			}
		else {
			i -= offset;
			x = i / A2->degree;
			y = i % A2->degree;
			xx = A1->element_image_of(x, Elt, FALSE);
			yy = A2->element_image_of(y, Elt + A1->elt_size_in_int, FALSE);
			j = xx * A2->degree + yy;
			j += offset;
			}
		}
	else {
		x = i / A2->degree;
		y = i % A2->degree;
		xx = A1->element_image_of(x, Elt, FALSE);
		yy = A2->element_image_of(y, Elt + A1->elt_size_in_int, FALSE);
		j = xx * A2->degree + yy;
		}
	if (f_v) {
		cout << "product_action::compute_image image of " << i << " is " << j << endl;
		}
	return j;
}

void product_action::element_one(action *A, int *Elt, int verbose_level)
{
	A1->element_one(Elt, verbose_level);
	A2->element_one(Elt + A1->elt_size_in_int, verbose_level);
}

int product_action::element_is_one(action *A, int *Elt, int verbose_level)
{
	if (!A1->element_is_one(Elt, verbose_level)) {
		return FALSE;
		}
	if (!A2->element_is_one(Elt + A1->elt_size_in_int, verbose_level)) {
		return FALSE;
		}
	return TRUE;
}

void product_action::element_unpack(uchar *elt, int *Elt, int verbose_level)
{
	A1->element_unpack(elt, Elt, verbose_level);
	A2->element_unpack(elt + A1->coded_elt_size_in_char, Elt + A1->elt_size_in_int, verbose_level);
}

void product_action::element_pack(int *Elt, uchar *elt, int verbose_level)
{
	A1->element_pack(Elt, elt, verbose_level);
	A2->element_pack(Elt + A1->elt_size_in_int, elt + A1->coded_elt_size_in_char, verbose_level);
}

void product_action::element_retrieve(action *A, int hdl, int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	uchar *p_elt;
	
	if (f_v) {
		cout << "product_action::element_retrieve() hdl = " << hdl << endl;
		}
	p_elt = Elts->s_i(hdl);
	A1->element_unpack(p_elt, Elt, verbose_level);
	A2->element_unpack(p_elt + A1->coded_elt_size_in_char, Elt + A1->elt_size_in_int, verbose_level);
}

int product_action::element_store(action *A, int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int hdl;
	
	A1->element_pack(Elt, elt1, verbose_level);
	A2->element_pack(Elt + A1->elt_size_in_int, elt1 + A1->coded_elt_size_in_char, verbose_level);
	hdl = Elts->store(elt1);
	if (f_v) {
		cout << "product_action::element_store() hdl = " << hdl << endl;
		}
	return hdl;
}

void product_action::element_mult(int *A, int *B, int *AB, int verbose_level)
{
	A1->element_mult(A, B, AB, verbose_level);
	A2->element_mult(A + A1->elt_size_in_int, B + A1->elt_size_in_int, AB + A1->elt_size_in_int, verbose_level);
}

void product_action::element_invert(int *A, int *Av, int verbose_level)
{
	A1->element_invert(A, Av, verbose_level);
	A2->element_invert(A + A1->elt_size_in_int, Av + A1->elt_size_in_int, verbose_level);
}

void product_action::element_transpose(int *A, int *At, int verbose_level)
{
	A1->element_transpose(A, At, verbose_level);
	A2->element_transpose(A + A1->elt_size_in_int, At + A1->elt_size_in_int, verbose_level);
}

void product_action::element_move(int *A, int *B, int verbose_level)
{
	A1->element_move(A, B, verbose_level);
	A2->element_move(A + A1->elt_size_in_int, B + A1->elt_size_in_int, verbose_level);
}

void product_action::element_print(int *A, ostream &ost)
{
	ost << "(" << endl;
	A1->element_print(A, ost);
	ost << ", " << endl;
	A2->element_print(A + A1->elt_size_in_int, ost);
	ost << ")" << endl;
}

void product_action::element_print_latex(int *A, ostream &ost)
{
	ost << "\\left(" << endl;
	A1->element_print_latex(A, ost);
	ost << ", " << endl;
	A2->element_print_latex(A + A1->elt_size_in_int, ost);
	ost << "\\\right)" << endl;
}

void product_action::make_element(int *Elt, int *data, int verbose_level)
{
	A1->make_element(Elt, data, verbose_level);
	A2->make_element(Elt + A1->elt_size_in_int, 
		data + A1->make_element_size, verbose_level);
}



