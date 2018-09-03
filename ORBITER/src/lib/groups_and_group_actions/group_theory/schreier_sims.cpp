// schreier_sims.C
//
// Anton Betten
// July 27, 2010

#include "foundations/foundations.h"
#include "groups_and_group_actions.h"


schreier_sims::schreier_sims()
{
	null();
}

schreier_sims::~schreier_sims()
{
	freeself();
}

void schreier_sims::null()
{
	f_interested_in_kernel = FALSE;
	GA = NULL;
	G = NULL;
	KA = NULL;
	K = NULL;
	Elt1 = NULL;
	Elt2 = NULL;
	Elt3 = NULL;
	f_has_target_group_order = FALSE;
	f_from_generators = FALSE;
	f_from_random_process = FALSE;
	f_from_old_G = FALSE;
	f_has_base_of_choice = FALSE;
	f_override_choose_next_base_point_method = FALSE;
	gens = NULL;
	callback_choose_random_generator = NULL;
	callback_choose_random_generator_data = NULL;
	old_G = NULL;
	iteration = 0;
}

void schreier_sims::freeself()
{
	if (Elt1) {
		FREE_int(Elt1);
		Elt1 = NULL;
		}
	if (Elt2) {
		FREE_int(Elt2);
		Elt1 = NULL;
		}
	if (Elt3) {
		FREE_int(Elt3);
		Elt1 = NULL;
		}
	if (G) {
		FREE_OBJECT(G);
		G = NULL;
		}
	if (K) {
		FREE_OBJECT(K);
		K = NULL;
		}
}

void schreier_sims::init(action *A, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init action:" << endl;
		A->print_info();
		}
	schreier_sims::GA = A;
	Elt1 = NEW_int(GA->elt_size_in_int);
	Elt2 = NEW_int(GA->elt_size_in_int);
	Elt3 = NEW_int(GA->elt_size_in_int);
	G = NEW_OBJECT(sims);
	//cout << "schreier_sims::init sims object " << G
	// << " with action " << GA << "=" << GA->label << endl;
	G->init(GA);
	G->init_trivial_group(0);
}

void schreier_sims::interested_in_kernel(action *KA,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::interested_in_kernel "
				"kernel action:" << endl;
		KA->print_info();
		}
	schreier_sims::KA = KA;
	K = NEW_OBJECT(sims);
	K->init(KA);
	K->init_trivial_group(0);
	f_interested_in_kernel = TRUE;
}


void schreier_sims::init_target_group_order(
		longinteger_object &tgo, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_target_group_order " << tgo << endl;
		}
	tgo.assign_to(schreier_sims::tgo);
	f_has_target_group_order = TRUE;
}

void schreier_sims::init_generators(
		vector_ge *gens, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_generators " << endl;
		}
	schreier_sims::gens = gens;
	f_from_generators = TRUE;
}

void schreier_sims::init_random_process(
	void (*callback_choose_random_generator)(int iteration,
			int *Elt, void *data, int verbose_level),
	void *callback_choose_random_generator_data, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_random_process" << endl;
		}
	schreier_sims::callback_choose_random_generator =
			callback_choose_random_generator;
	schreier_sims::callback_choose_random_generator_data =
			callback_choose_random_generator_data;
	f_from_random_process = TRUE;
}

void schreier_sims::init_old_G(sims *old_G, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_old_G" << endl;
		}
	schreier_sims::old_G = old_G;
	f_from_old_G = TRUE;
}

void schreier_sims::init_base_of_choice(
	int base_of_choice_len, int *base_of_choice,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_base_of_choice" << endl;
		}
	schreier_sims::base_of_choice_len = base_of_choice_len;
	schreier_sims::base_of_choice = base_of_choice;
	f_has_base_of_choice = TRUE;
}

void schreier_sims::init_choose_next_base_point_method(
	int (*choose_next_base_point_method)(action *A,
			int *Elt, int verbose_level),
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::init_choose_next_base_point_method" << endl;
		}
	schreier_sims::choose_next_base_point_method =
			choose_next_base_point_method;
	f_override_choose_next_base_point_method = TRUE;
}

void schreier_sims::compute_group_orders()
{
	G->group_order(G_order);
	if (f_interested_in_kernel) {
		longinteger_domain D;
		K->group_order(K_order);
		D.mult(G_order, K_order, KG_order);
		}
	else {
		G_order.assign_to(KG_order);
		}
}

void schreier_sims::print_group_orders()
{
	cout << "current group order is " << G_order;
	if (f_has_target_group_order) {
		cout << " target group order is " << tgo;
		}
	cout << endl;
}

void schreier_sims::get_generator_internal(
		int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	
	if (f_v) {
		cout << "schreier_sims::get_generator_internal "
				"choosing random schreier generator" << endl;
		cout << "verbose_level=" << verbose_level << endl;
		}
	G->random_schreier_generator(verbose_level - 3);
	GA->element_move(G->schreier_gen, Elt, 0);
	if (f_vvv) {
		cout << "schreier_sims::get_generator_internal "
				"random element chosen:" << endl;
		GA->element_print_quick(Elt, cout);
		cout << endl;
		}
}

void schreier_sims::get_generator_external(
		int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	//int f_vvv = (verbose_level >= 3);
	
	if (f_v) {
		cout << "schreier_sims::get_generator_external" << endl;
		}
	if (f_from_generators) {
		get_generator_external_from_generators(Elt, verbose_level);
		}
	else if (f_from_random_process) {
		get_generator_external_random_process(Elt, verbose_level);
		}
	else if (f_from_old_G) {
		get_generator_external_old_G(Elt, verbose_level);
		}
	if (FALSE /*f_vvv*/) {
		cout << "schreier_sims::get_generator_external "
				"we have chosen the following generator" << endl;
		GA->element_print_quick(Elt, cout);
		cout << endl;
		}
}

void schreier_sims::get_generator_external_from_generators(
		int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int r;
	
	if (FALSE) {
		cout << "schreier_sims::get_generator_external_"
				"from_generators" << endl;
		}
	if (gens->len) {
		r = random_integer(gens->len);
		if (f_v) {
			cout << "schreier_sims::get_generator_external_"
					"from_generators choosing generator "
					<< r << " / " << gens->len << endl;
			}
		GA->element_move(gens->ith(r), Elt, 0);
		}
	else {
		// no generators, we are creating the identity group:
		GA->element_one(Elt, 0);
		}
}

void schreier_sims::get_generator_external_random_process(
		int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "schreier_sims::get_generator_external_"
				"random_process" << endl;
		}
	(*callback_choose_random_generator)((iteration >> 1),
			Elt, callback_choose_random_generator_data, verbose_level - 1);
}

void schreier_sims::get_generator_external_old_G(
		int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	
	if (FALSE) {
		cout << "schreier_sims::get_generator_external_old_G" << endl;
		}
	old_G->random_element(Elt, verbose_level - 1);
	if (f_v) {
		cout << "schreier_sims::get_generator_external_old_G "
				"random element chosen, path = ";
		int_vec_print(cout, old_G->path, old_G->A->base_len);
		cout << endl;
		}
}

void schreier_sims::get_generator(int *Elt, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "schreier_sims::get_generator" << endl;
		}
	if ((iteration % 2) == 0) {
		get_generator_internal(Elt, verbose_level);
		}
	else if ((iteration % 2) == 1) {
		get_generator_external(Elt, verbose_level);
		}
	if (f_v) {
		cout << "schreier_sims::get_generator done" << endl;
		}
}

void schreier_sims::closure_group(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vvv = (verbose_level >= 3);
	longinteger_domain D;
	longinteger_object quo, rem;
	int cnt = 0;

	if (f_v) {
		cout << "schreier_sims::closure_group" << endl;
		}
	D.integral_division(tgo, KG_order, quo, rem, 0);
	while (!quo.is_zero() && !rem.is_zero()) {
		if (f_vvv) {
			cout << "schreier_sims::closure_group iteration "
					<< iteration << " cnt " << cnt
					<< ": remainder is not zero, "
							"this is not a subgroup" << endl;
			}
		int nb_times = 30;

		if (f_v) {
			cout << "schreier_sims::closure_group "
					"calling G->closure_group" << endl;
			}
		G->closure_group(nb_times, verbose_level - 3);
		compute_group_orders();
		D.integral_division(tgo, KG_order, quo, rem, 0);
		if (f_vvv) {
			cout << "schreier_sims::closure_group iteration "
					<< iteration << " cnt " << cnt
					<< ": after closure_group: remaining factor: "
					<< quo << " remainder " << rem << endl;
			}
		cnt++;
		if (cnt == 100) {
			cout << "schreier_sims::closure_group cnt == 100, "
					"we are breaking off" << endl;
			break;
			}
		}
	if (f_v) {
		cout << "schreier_sims::closure_group done ";
		print_group_orders();
		}
}

void schreier_sims::create_group(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 3);
	int f_vvv = (verbose_level >= 4);
	int f_vvvv = (verbose_level >= 5);
	longinteger_domain D;
	int drop_out_level, image, b, c, f_added, old_base_len;
	
	if (f_v) {
		cout << "schreier_sims::create_group" << endl;
		}
	
	compute_group_orders();
	if (f_v) {
		print_group_orders();
		}
	iteration = 0;
	while (TRUE) {
	
		if (f_vv) {
			cout << "iteration " << iteration << endl;
			}
		if (f_has_target_group_order && iteration > 10000) {
			cout << "schreier_sims::create_group iteration > 1000, "
					"something seems to be wrong" << endl;
			cout << "target group order = " << tgo << endl;
			cout << "KG_order = " << KG_order << endl;		
			//test_if_subgroup(old_G, 2);
			exit(1);
			}

		if (!f_has_target_group_order && iteration == 10000) {
			if (f_v) {
				cout << "schreier_sims::create_group "
						"iteration == 1000, we seem to be done" << endl;
				}
			break;
			}

		get_generator(Elt1, verbose_level - 3);

		if (f_vvvv) {
			cout << "schreier_sims::create_group "
					"calling strip:" << endl;
			}
		if (G->strip(Elt1, Elt2, drop_out_level,
				image, 0 /*verbose_level - 2*/)) {
			if (f_vv) {
				cout << "schreier_sims::create_group: "
						"element strips through" << endl;
				if (f_vvvv) {
					cout << "schreier_sims::create_group: "
							"residue = " << endl;
					GA->element_print_quick(Elt2, cout);
					cout << endl;
					}
				}
			f_added = FALSE;
			if (!GA->element_is_one(Elt2, 0)) {
				if (f_vvv) {
					cout << "schreier_sims::create_group: "
							"the residue is not trivial, "
							"we need to choose another base point" << endl;
					}
				if (f_override_choose_next_base_point_method) {
					b = (*choose_next_base_point_method)(GA,
							Elt2, verbose_level - 5);
					}
				else {
					b = choose_next_base_point_default_method(
							GA, Elt2, verbose_level - 5);
					}

				if (f_vv) {
					cout << "schreier_sims::create_group: "
							"next suggested base point is "
							<< b << endl;
					}
				if (b == -1) {
					if (f_vv) {
						cout << "schreier_sims::create_group: "
								"cannot find next base point" << endl;
						}
					if (K->strip(Elt2, Elt3, drop_out_level,
							image, 0/*verbose_level - 3*/)) {
						if (f_vv) {
							cout << "schreier_sims::create_group: "
									"element strips through kernel" << endl;
							if (f_vvvv) {
								cout << "schreier_sims::create_group: "
										"residue = " << endl;
								KA->element_print_quick(Elt3, cout);
								cout << endl;
								K->print(FALSE);
								K->print_basic_orbits();
								cout << "schreier_sims::create_group: "
										"residue" << endl;
								KA->element_print_image_of_set(
										Elt3, KA->base_len, KA->base);
								cout << "schreier_sims::create_group: "
										"Elt2" << endl;
								KA->element_print_image_of_set(
										Elt2, KA->base_len, KA->base);
								}
							}
						if (!KA->element_is_one(Elt3, 0)) {
							cout << "schreier_sims::create_group: "
									"element strips through kernel, "
									"residue = " << endl;
							cout << "but the element is not the "
									"identity, something is wrong" << endl;
							GA->element_print(Elt3, cout);
							cout << endl;
							compute_group_orders();
							print_group_orders();

							exit(1);
							}
						}
					K->add_generator_at_level(Elt3,
							drop_out_level, 0/*verbose_level - 3*/);
					if (f_vvv) {
						cout << "schreier_sims::create_group: "
								"the residue has been added as "
								"kernel generator at level "
								<< drop_out_level << endl;
						}
					f_added = TRUE;
					}
				else {
					if (f_vvv) {
						cout << "schreier_sims::create_group: "
								"choosing n e w base point " << b << endl;
						}
					old_base_len = GA->base_len;
					GA->reallocate_base(b);
					if (f_vvv) {
						//cout << "after reallocate_base 1" << endl;
						}
					G->reallocate_base(old_base_len, verbose_level - 1);
					if (f_vvv) {
						//cout << "after reallocate_base 2" << endl;
						}
					if (f_vv) {
						cout << "schreier_sims::create_group: "
								"n e w base point " << b
							<< " chosen, n e w base has length "
							<< GA->base_len << endl;
						cout << "schreier_sims::create_group: "
								"calling add_generator_at_level" << endl;
						}
					G->add_generator_at_level(Elt2,
							GA->base_len - 1, 0/*verbose_level - 3*/);
					if (f_vv) {
						cout << "schreier_sims::create_group: "
								"the residue has been added "
								"at level " << GA->base_len - 1 << endl;
						}
					} // if b
				} // if ! element is one
			else {
				if (f_vv) {
					cout << "schreier_sims::create_group: "
							"the residue is trivial" << endl;
					}
				}
			//G->closure_group(10, verbose_level - 2);
			}
		else {
			f_added = TRUE;
			if (f_vv) {
				cout << "schreier_sims::create_group: "
						"element needs to be inserted at level = "
					<< drop_out_level << " with image "
					<< image << endl;
				if (FALSE) {
					GA->element_print(Elt2, cout);
					cout  << endl;
					}
				}
			G->add_generator_at_level(Elt2,
					drop_out_level, 0/*verbose_level - 3*/);
			}
		
		compute_group_orders();


		if ((f_v && f_added) || f_vv) {
			cout << "schreier_sims::create_group: "
					"n e w group order is ";
			print_group_orders();
			}
		iteration++;

		if (f_has_target_group_order) {
			c = D.compare(tgo, KG_order);
			if (c == 0) {
				if (f_v) {
					cout << "schreier_sims::create_group: "
							"reached the full group after "
							<< iteration << " iterations" << endl;
					}
				break;
				}
			if (c < 0) {
				if (TRUE) {
					cout << "schreier_sims::create_group "
							"overshooting the expected group after "
							<< iteration << " iterations" << endl;
					print_group_orders();
					}
				//break;
				exit(1);
				}
			else {
				closure_group(verbose_level - 2);
				}
			}
		else {
			closure_group(verbose_level - 2);
			}
		}
	if (f_v) {
		cout << "schreier_sims::create_group finished:";
		print_group_orders();

		cout << "the n e w action has base ";
		int_vec_print(cout, GA->base, GA->base_len);
		cout << " of length " << GA->base_len  << endl;
		}
}


