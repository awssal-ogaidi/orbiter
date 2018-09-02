// create_BLT_set_main.C
// 
// Anton Betten
//
// March 17, 2018
//
//
// 
//
//

#include "orbiter.h"

// global data:

INT t0; // the system time when the program started

int main(int argc, const char **argv);

int main(int argc, const char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_description = FALSE;
	BLT_set_create_description *Descr;
	INT nb_transform = 0;
	const char *transform_coeffs[1000];
	INT f_inverse_transform[1000];

	t0 = os_ticks();


	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-description") == 0) {
			f_description = TRUE;
			Descr = NEW_OBJECT(BLT_set_create_description);
			i += Descr->read_arguments(argc - (i - 1),
					argv + i, verbose_level) - 1;

			cout << "-description" << endl;
			}
		else if (strcmp(argv[i], "-transform") == 0) {
			transform_coeffs[nb_transform] = argv[++i];
			f_inverse_transform[nb_transform] = FALSE;
			cout << "-transform " << transform_coeffs[nb_transform] << endl;
			nb_transform++;
			}
		else if (strcmp(argv[i], "-transform_inverse") == 0) {
			transform_coeffs[nb_transform] = argv[++i];
			f_inverse_transform[nb_transform] = TRUE;
			cout << "-transform_inverse "
					<< transform_coeffs[nb_transform] << endl;
			nb_transform++;
			}
		}
	if (!f_description) {
		cout << "please use option -description ... "
				"to enter a description of the surface" << endl;
		exit(1);
		}

	BLT_set_create *BC;
	//INT j;

	BC = NEW_OBJECT(BLT_set_create);

	cout << "before BC->init" << endl;
	BC->init(Descr, verbose_level);
	cout << "after BC->init" << endl;
	

	if (nb_transform) {
		cout << "before BC->apply_transformations" << endl;
		BC->apply_transformations(transform_coeffs,
				f_inverse_transform, nb_transform, verbose_level);
		cout << "after BC->apply_transformations" << endl;
		}

	action *A;
	//INT *Elt1;
	INT *Elt2;
	
	A = BC->A;

	Elt2 = NEW_INT(A->elt_size_in_INT);
	


#if 0
	if (BC->f_has_group) {
		for (i = 0; i < BC->Sg->gens->len; i++) {
			cout << "Testing generator " << i << " / "
					<< BC->Sg->gens->len << endl;
			A->element_invert(BC->Sg->gens->ith(i),
					Elt2, 0 /*verbose_level*/);
	
			BC->Surf->substitute_semilinear(SC->coeffs,
					coeffs_out, FALSE /* f_semilinear */, 0,
					Elt2, 0 /*verbose_level*/);

			PG_element_normalize(*SC->F, coeffs_out, 1, 20);


			if (INT_vec_compare(SC->coeffs, coeffs_out, 20)) {
				cout << "error, the transformation does not "
						"preserve the equation of the surface" << endl;
				exit(1);
				}
			cout << "Generator " << i << " / " << SC->Sg->gens->len
					<< " is good" << endl;
			}
		}
	else {
		cout << "We do not have information about the "
				"automorphism group" << endl;
		}
#endif


	cout << "We have created the following BLT-set:" << endl;
	cout << "$$" << endl;
	INT_set_print_tex(cout, BC->set, BC->q + 1);
	cout << endl;
	cout << "$$" << endl;

	if (BC->f_has_group) {
		cout << "The stabilizer is generated by:" << endl;
		BC->Sg->print_generators_tex(cout);
		}


	


	FREE_INT(Elt2);

	FREE_OBJECT(BC);

	the_end(t0);
	//the_end_quietly(t0);
}

