// surface_classify_wedge.C
// 
// Anton Betten
// September 2, 2016
//
// 
//
//

#include "orbiter.h"


surface_classify_wedge::surface_classify_wedge()
{
	null();
}

surface_classify_wedge::~surface_classify_wedge()
{
	freeself();
}

void surface_classify_wedge::null()
{
	nb_identify = 0;
	F = NULL;
	LG = NULL;
	A = NULL;

	Surf = NULL;
	Surf_A = NULL;
	Classify_double_sixes = NULL;
	
	Elt0 = NULL;
	Elt1 = NULL;
	Elt2 = NULL;
	Elt3 = NULL;

	Flag_orbits = NULL;
	Surfaces = NULL;
	
	Identify_label = NULL;
	Identify_coeff = NULL;
	Identify_monomial = NULL;
	Identify_length = NULL;
	//Isomorphisms = NULL;
	//The_surface = NULL;
	//is_isomorphic_to = NULL;
	//Orb = NULL;
}

void surface_classify_wedge::freeself()
{
	if (Surf) {
		FREE_OBJECT(Surf);
		}
	if (Surf_A) {
		FREE_OBJECT(Surf_A);
		}

	if (Elt0) {
		FREE_INT(Elt0);
		}
	if (Elt1) {
		FREE_INT(Elt1);
		}
	if (Elt2) {
		FREE_INT(Elt2);
		}
	if (Elt3) {
		FREE_INT(Elt3);
		}

	if (Flag_orbits) {
		FREE_OBJECTS(Flag_orbits);
		}
	if (Surfaces) {
		FREE_OBJECTS(Surfaces);
		}

	if (Classify_double_sixes) {
		FREE_OBJECT(Classify_double_sixes);
		}
	null();
}

void surface_classify_wedge::read_arguments(int argc, const char **argv, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j;

	if (f_v) {
		cout << "surface_classify_wedge::read_arguments" << endl;
		}
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-identify") == 0) {
			if (nb_identify == 0) {
				Identify_label = NEW_PBYTE(1000);
				Identify_coeff = NEW_PINT(1000);
				Identify_monomial = NEW_PINT(1000);
				Identify_length = NEW_INT(1000);
				}
			INT coeff[1000];
			INT monomial[1000];
			INT nb_terms = 0;
			cout << "-identify " << endl;
			const BYTE *label = argv[++i];
			cout << "-identify " << label << endl;
			Identify_label[nb_identify] = NEW_BYTE(strlen(label) + 1);
			strcpy(Identify_label[nb_identify], label);
			for (j = 0; ; j++) {
				coeff[j] = atoi(argv[++i]);
				if (coeff[j] == -1) {
					break;
					}
				monomial[j] = atoi(argv[++i]);
				}
			nb_terms = j;
			Identify_coeff[nb_identify] = NEW_INT(nb_terms);
			Identify_monomial[nb_identify] = NEW_INT(nb_terms);
			Identify_length[nb_identify] = nb_terms;
			INT_vec_copy(coeff, Identify_coeff[nb_identify], nb_terms);
			INT_vec_copy(monomial, Identify_monomial[nb_identify], nb_terms);
			cout << "-identify " << Identify_label[nb_identify] << " ";
			for (j = 0; j < Identify_length[nb_identify]; j++) {
				cout << Identify_coeff[nb_identify][j] << " ";
				cout << Identify_monomial[nb_identify][j] << " ";
				}
			cout << "-1" << endl;
			nb_identify++;
			
			}
		}
}

void surface_classify_wedge::init(finite_field *F, linear_group *LG, 
	int argc, const char **argv, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	
	if (f_v) {
		cout << "surface_classify_wedge::init" << endl;
		}
	surface_classify_wedge::F = F;
	surface_classify_wedge::LG = LG;
	q = F->q;
	
	read_arguments(argc, argv, verbose_level);
	
	A = LG->A_linear;
	A2 = LG->A2;


	if (f_v) {
		cout << "surface_classify_wedge::init before Surf->init" << endl;
		}
	Surf = NEW_OBJECT(surface);
	Surf->init(F, verbose_level - 1);
	if (f_v) {
		cout << "surface_classify_wedge::init after Surf->init" << endl;
		}


	Surf_A = NEW_OBJECT(surface_with_action);

	if (is_prime(q)) {
		f_semilinear = FALSE;
		}
	else {
		f_semilinear = TRUE;
		}

	sprintf(fname_base, "surface_%ld", q);

	
	if (f_v) {
		cout << "surface_classify_wedge::init before Surf_A->init" << endl;
		}
	Surf_A->init(Surf, f_semilinear, verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::init after Surf_A->init" << endl;
		}
	
	Elt0 = NEW_INT(A->elt_size_in_INT);
	Elt1 = NEW_INT(A->elt_size_in_INT);
	Elt2 = NEW_INT(A->elt_size_in_INT);
	Elt3 = NEW_INT(A->elt_size_in_INT);

	Classify_double_sixes = NEW_OBJECT(classify_double_sixes);

	if (f_v) {
		cout << "surface_classify_wedge::init before Classify_double_sixes->init" << endl;
		}
	Classify_double_sixes->init(Surf_A, LG, argc, argv, verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::init after Classify_double_sixes->init" << endl;
		}

	if (f_v) {
		cout << "surface_classify_wedge::init done" << endl;
		}
}

void surface_classify_wedge::classify_surfaces_from_double_sixes(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes" << endl;
		}


	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes before downstep" << endl;
		}
	downstep(verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes after downstep" << endl;
		cout << "we found " << Flag_orbits->nb_flag_orbits << " flag orbits out of " << Classify_double_sixes->Double_sixes->nb_orbits << " orbits of double sixes" << endl;
		}

	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes before upstep" << endl;
		}
	upstep(verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes after upstep" << endl;
		cout << "we found " << Surfaces->nb_orbits << " surfaces out from " << Flag_orbits->nb_flag_orbits << " double sixes" << endl;
		}


	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces_from_double_sixes done" << endl;
		}
}

void surface_classify_wedge::downstep(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	INT i, nb_orbits, nb_flag_orbits;

	if (f_v) {
		cout << "surface_classify_wedge::downstep" << endl;
		}
	nb_orbits = Classify_double_sixes->Double_sixes->nb_orbits;
	Flag_orbits = NEW_OBJECT(flag_orbits);
	Flag_orbits->init(A, A2, nb_orbits /* nb_primary_orbits_lower */, 
		27 /* pt_representation_sz */, nb_orbits /* nb_flag_orbits */, 
		verbose_level);

	if (f_v) {
		cout << "surface_classify_wedge::downstep initializing flag orbits" << endl;
		}

	nb_flag_orbits = 0;
	for (i = 0; i < nb_orbits; i++) {

		if (f_v) {
			cout << "surface_classify_wedge::downstep orbit " << i << " / " << nb_orbits << endl;
			}
		set_and_stabilizer *R;
		//longinteger_object ol;
		longinteger_object go;
		INT Lines[27];

		R = Classify_double_sixes->Double_sixes->get_set_and_stabilizer(
				i /* orbit_index */, 0 /* verbose_level */);

		//gen->orbit_length(i /* node */, 3 /* level */, ol);

		R->Strong_gens->group_order(go);

		INT_vec_copy(R->data, Lines, 12);

		if (f_vv) {
			cout << "surface_classify_wedge::downstep "
					"before create_the_fifteen_other_lines" << endl;
			}

		Surf->create_the_fifteen_other_lines(
				Lines /* double_six */,
				Lines + 12 /* fifteen_other_lines */,
				verbose_level - 4);
		if (f_vv) {
			cout << "surface_classify_wedge::downstep "
					"after create_the_fifteen_other_lines" << endl;
			}


		if (f_vv) {
			cout << "surface_classify_wedge::downstep "
					"before Flag_orbit_node[].init" << endl;
			}

		Flag_orbits->Flag_orbit_node[nb_flag_orbits].init(
			Flag_orbits,
			nb_flag_orbits /* flag_orbit_index */,
			i /* downstep_primary_orbit */,
			0 /* downstep_secondary_orbit */,
			1 /* downstep_orbit_len */,
			FALSE /* f_long_orbit */,
			Lines /* INT *pt_representation */,
			R->Strong_gens,
			verbose_level - 2);

		if (f_vv) {
			cout << "surface_classify_wedge::downstep "
					"after Flag_orbit_node[].init" << endl;
			}

		R->Strong_gens = NULL;

		nb_flag_orbits++;


		FREE_OBJECT(R);
		}

	Flag_orbits->nb_flag_orbits = nb_flag_orbits;


	if (f_v) {
		cout << "surface_classify_wedge::downstep initializing flag orbits done" << endl;
		}
}

void surface_classify_wedge::upstep(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f, po, so, i, j;
	INT *f_processed;
	INT nb_processed;

	if (f_v) {
		cout << "surface_classify_wedge::upstep" << endl;
		}


	f_processed = NEW_INT(Flag_orbits->nb_flag_orbits);
	INT_vec_zero(f_processed, Flag_orbits->nb_flag_orbits);
	nb_processed = 0;

	Surfaces = NEW_OBJECT(classification);

	longinteger_object go;
	A->group_order(go);

	Surfaces->init(A, A2, Flag_orbits->nb_flag_orbits, 27, go, verbose_level);


	for (f = 0; f < Flag_orbits->nb_flag_orbits; f++) {

		double progress;
		INT Lines[27];
		
		if (f_processed[f]) {
			continue;
			}

		progress = ((double)nb_processed * 100. ) /
					(double) Flag_orbits->nb_flag_orbits;

		if (f_v) {
			cout << "Defining n e w orbit "
					<< Flag_orbits->nb_primary_orbits_upper
					<< " from flag orbit " << f << " / "
					<< Flag_orbits->nb_flag_orbits
					<< " progress=" << progress << "%" << endl;
			}
		Flag_orbits->Flag_orbit_node[f].upstep_primary_orbit =
				Flag_orbits->nb_primary_orbits_upper;
		

		if (Flag_orbits->pt_representation_sz != 27) {
			cout << "Flag_orbits->pt_representation_sz != 27" << endl;
			exit(1);
			}
		po = Flag_orbits->Flag_orbit_node[f].downstep_primary_orbit;
		so = Flag_orbits->Flag_orbit_node[f].downstep_secondary_orbit;
		if (f_v) {
			cout << "po=" << po << " so=" << so << endl;
			}
		INT_vec_copy(Flag_orbits->Pt + f * 27, Lines, 27);




		vector_ge *coset_reps;
		INT nb_coset_reps;
		
		coset_reps = NEW_OBJECT(vector_ge);
		coset_reps->init(Surf_A->A);
		coset_reps->allocate(36);


		strong_generators *S;
		longinteger_object go;


		if (f_v) {
			cout << "Lines:";
			INT_vec_print(cout, Lines, 27);
			cout << endl;
			}
		S = Flag_orbits->Flag_orbit_node[f].gens->create_copy();
		S->group_order(go);
		if (f_v) {
			cout << "po=" << po << " so=" << so << " go=" << go << endl;
			}

		nb_coset_reps = 0;
		for (i = 0; i < 36; i++) {
			
			if (f_v) {
				cout << "f=" << f << " / " << Flag_orbits->nb_flag_orbits
						<< ", upstep i=" << i << " / 36" << endl;
				}
			INT f2;

			INT double_six[12];


			for (j = 0; j < 12; j++) {
				double_six[j] = Lines[Surf->Double_six[i * 12 + j]];
				}
			if (f_v) {
				cout << "f=" << f << " / " << Flag_orbits->nb_flag_orbits
						<< ", upstep i=" << i << " / 36 double_six=";
				INT_vec_print(cout, double_six, 12);
				cout << endl;
				}
			
			Classify_double_sixes->identify_double_six(double_six, 
				Elt1 /* transporter */, f2, verbose_level - 4);

			if (f_v) {
				cout << "f=" << f << " / " << Flag_orbits->nb_flag_orbits
						<< ", upstep " << i << " / 36, double six is "
								"isomorphic to orbit " << f2 << endl;
				}

			
			if (f2 == f) {
				if (f_v) {
					cout << "We found an automorphism of the surface:" << endl;
					A->element_print_quick(Elt1, cout);
					cout << endl;
					}
				A->element_move(Elt1, coset_reps->ith(nb_coset_reps), 0);
				nb_coset_reps++;
				}
			else {
				if (f_v) {
					cout << "We are identifying flag orbit " << f2
							<< " with flag orbit " << f << endl;
					}
				if (!f_processed[f2]) {
					Flag_orbits->Flag_orbit_node[f2].upstep_primary_orbit
						= Flag_orbits->nb_primary_orbits_upper;
					Flag_orbits->Flag_orbit_node[f2].f_fusion_node = TRUE;
					Flag_orbits->Flag_orbit_node[f2].fusion_with = f;
					Flag_orbits->Flag_orbit_node[f2].fusion_elt
						= NEW_INT(A->elt_size_in_INT);
					A->element_invert(Elt1,
							Flag_orbits->Flag_orbit_node[f2].fusion_elt, 0);
					f_processed[f2] = TRUE;
					nb_processed++;
					}
				else {
					cout << "Flag orbit " << f2 << " has already been "
							"identified with flag orbit " << f << endl;
					if (Flag_orbits->Flag_orbit_node[f2].fusion_with != f) {
						cout << "Flag_orbits->Flag_orbit_node[f2]."
								"fusion_with != f" << endl;
						exit(1);
						}
					}
				}

			} // next i


		coset_reps->reallocate(nb_coset_reps);

		strong_generators *Aut_gens;

		{
		longinteger_object ago;
		
		if (f_v) {
			cout << "surface_classify_wedge::upstep Extending the "
					"group by a factor of " << nb_coset_reps << endl;
			}
		Aut_gens = NEW_OBJECT(strong_generators);
		Aut_gens->init_group_extension(S, coset_reps,
				nb_coset_reps, verbose_level - 2);

		Aut_gens->group_order(ago);


		if (f_v) {
			cout << "the double six has a stabilizer of order "
					<< ago << endl;
			cout << "The double six stabilizer is:" << endl;
			Aut_gens->print_generators_tex(cout);
			}
		}



		Surfaces->Orbit[Flag_orbits->nb_primary_orbits_upper].init(
			Surfaces,
			Flag_orbits->nb_primary_orbits_upper, 
			Aut_gens, Lines, verbose_level);

		FREE_OBJECT(coset_reps);
		FREE_OBJECT(S);
		
		f_processed[f] = TRUE;
		nb_processed++;
		Flag_orbits->nb_primary_orbits_upper++;
		} // next f


	if (nb_processed != Flag_orbits->nb_flag_orbits) {
		cout << "nb_processed != Flag_orbits->nb_flag_orbits" << endl;
		cout << "nb_processed = " << nb_processed << endl;
		cout << "Flag_orbits->nb_flag_orbits = "
				<< Flag_orbits->nb_flag_orbits << endl;
		exit(1);
		}

	Surfaces->nb_orbits = Flag_orbits->nb_primary_orbits_upper;
	
	if (f_v) {
		cout << "We found " << Surfaces->nb_orbits
				<< " orbits of surfaces from "
				<< Flag_orbits->nb_flag_orbits
				<< " double sixes" << endl;
		}
	
	FREE_INT(f_processed);


	if (f_v) {
		cout << "surface_classify_wedge::upstep done" << endl;
		}
}

void surface_classify_wedge::write_file(
		ofstream &fp, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "surface_classify_wedge::write_file" << endl;
		}
	fp.write((char *) &q, sizeof(INT));

	Flag_orbits->write_file(fp, verbose_level);

	Surfaces->write_file(fp, verbose_level);

	if (f_v) {
		cout << "surface_classify_wedge::write_file finished" << endl;
		}
}

void surface_classify_wedge::read_file(
		ifstream &fp, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT q1;
	
	if (f_v) {
		cout << "surface_classify_wedge::read_file" << endl;
		}
	fp.read((char *) &q1, sizeof(INT));
	if (q1 != q) {
		cout << "surface_classify_wedge::read_file q1 != q" << endl;
		exit(1);
		}

	Flag_orbits = NEW_OBJECT(flag_orbits);
	Flag_orbits->A = A;
	Flag_orbits->A2 = A;
	Flag_orbits->read_file(fp, verbose_level);

	Surfaces = NEW_OBJECT(classification);
	Surfaces->A = A;
	Surfaces->A2 = A2;

	A->group_order(Surfaces->go);

	Surfaces->read_file(fp, verbose_level);

	if (f_v) {
		cout << "surface_classify_wedge::read_file finished" << endl;
		}
}


#if 0
void surface_classify_wedge::classify_surfaces(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT iso_type, orb;

	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces" << endl;
		}

	Classify_double_sixes->test_orbits(verbose_level);

	allocate_data();


	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces before do_classify_surfaces" << endl;
		}
	do_classify_surfaces(verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces after do_classify_surfaces" << endl;
		}


	if (f_v) {
		cout << "The classification is complete, nb_iso=" << nb_iso << endl;
		}
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		orb = Orb[iso_type];
		cout << "iso type " << iso_type << " is orbit " << orb << endl;
		}
	cout << "We found " << nb_iso << " isomorphism types of surfaces" << endl;


	if (f_v) {
		cout << "surface_classify_wedge::classify_surfaces done" << endl;
		}

}
#endif

#if 0
void surface_classify_wedge::do_classify_surfaces(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT orb, iso_type;

	if (f_v) {
		cout << "surface_classify_wedge::do_classify_surfaces" << endl;
		}

	for (orb = 0; orb < Classify_double_sixes->nb; orb++) {
		is_isomorphic_to[orb] = -1;
		}

	iso_type = 0;

	for (orb = 0; orb < Classify_double_sixes->nb; orb++) {

		cout << "orb=" << orb << " is_isomorphic_to[orb]=" << is_isomorphic_to[orb] << endl;

		if (is_isomorphic_to[orb] >= 0) {
			if (f_v) {
				cout << "surface_classify_wedge::do_classify_surfaces orbit " << orb << " / " << Classify_double_sixes->nb << " is isomorphic to " << is_isomorphic_to[orb] << ", skipping" << endl;
				}
			continue;
			}

		if (!new_surface(orb, iso_type, verbose_level)) {
			cout << "Failed surface" << endl;
			continue;
			}




		iso_type++;
		}
	nb_iso = iso_type;
	if (f_v) {
		cout << "surface_classify_wedge::do_classify_surfaces nb_iso = " << nb_iso << endl;
		}

	spreadsheet *Sp;

	Classify_double_sixes->make_spreadsheet_of_fiveplusone_configurations(Sp,
#if 0
		TRUE /*f_with_fusion */, is_isomorphic_to /* INT *Fusion */, Isomorphisms,
#endif
		verbose_level);

	FREE_OBJECT(Sp);

#if 0
	{
	classify C;

	C.init(Nb_points, nb, FALSE, 0);
	cout << "The distribution of the number of points on these surfaces is: ";
	C.print_naked(TRUE);
	cout << endl;
	}
#endif


	if (f_v) {
		cout << "surface_classify_wedge::do_classify_surfaces done" << endl;
		}
}

INT surface_classify_wedge::new_surface(INT orb, INT iso_type, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT idx;

	if (f_v) {
		cout << "surface_classify_wedge::new_surface" << endl;
		}


	if (f_v) {
		cout << "surface_classify_wedge::new_surface n e w isomorphism type " << iso_type << " is defined via orbit " << orb << " / " << Classify_double_sixes->nb << ":" << endl;
		}

	idx = Classify_double_sixes->Idx[orb];

	Orb[iso_type] = orb;
	is_isomorphic_to[orb] = iso_type;
	store_identity(orb, 0);

	surface_data *D;

	D = NEW_OBJECT(surface_data);

	if (!D->init(this, orb, iso_type, verbose_level)) {
		cout << "this surface is bad, skipping" << endl;
		is_isomorphic_to[orb] = -2;
		FREE_OBJECT(D);
		return FALSE;
		}

	cout << "surface with iso_type = " << iso_type << " has been defined" << endl;


	The_surface[iso_type] = D;



	if (f_v) {
		cout << "surface_classify_wedge::new_surface done" << endl;
		}
	return TRUE;
}

void surface_classify_wedge::store_isomorphism(INT *Elt, INT orb, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "surface_classify_wedge::store_isomorphism" << endl;
		}
	A->element_invert(Elt, Isomorphisms->ith(orb), 0);
	if (f_v) {
		cout << "surface_classify_wedge::store_isomorphism done" << endl;
		}
}


void surface_classify_wedge::store_identity(INT orb, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "surface_classify_wedge::store_identity" << endl;
		}
	A->element_one(Isomorphisms->ith(orb), 0);
	if (f_v) {
		cout << "surface_classify_wedge::store_identity done" << endl;
		}
}

void surface_classify_wedge::generate_source_code(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	BYTE fname[1000];
	BYTE *prefix;
	INT iso_type;
	INT *rep;
	INT i, j;

	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code" << endl;
		}
	sprintf(fname, "%s.C", fname_base);
	prefix = fname_base;

	{
	ofstream f(fname);

	f << "INT " << prefix << "_nb_reps = " << nb_iso << ";" << endl;
	f << "INT " << prefix << "_size = " << Surf->nb_monomials << ";" << endl;
	f << "INT " << prefix << "_reps[] = {" << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		rep = The_surface[iso_type]->coeff;
		f << "\t";
		for (i = 0; i < Surf->nb_monomials; i++) {
			f << rep[i];
			f << ", ";
			}
		f << endl;
		}
	f << "};" << endl;
	f << "const BYTE *" << prefix << "_stab_order[] = {" << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		//rep = The_surface[iso_type]->coeff;

		f << "\t\"";

		The_surface[iso_type]->ago.print_not_scientific(f);
		f << "\"," << endl;
		}
	f << "};" << endl;

	f << "INT " << prefix << "_nb_E[] = { " << endl << "\t";
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << The_surface[iso_type]->nb_E;
		if (iso_type < nb_iso - 1) {
			f << ", ";
			}
		if (((iso_type + 1) % 10) == 0) {
			f << endl << "\t";
			}
		}
	f << "};" << endl;

	f << "INT " << prefix << "_single_six[] = { " << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << "\t" << The_surface[iso_type]->S2[5];
		for (j = 0; j < 5; j++) {
			f << ", ";
			f << The_surface[iso_type]->S2[j];
			}
		f << ", " << endl;
		}
	f << "};" << endl;


	f << "INT " << prefix << "_Lines[] = { " << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << "\t";
		for (j = 0; j < 27; j++) {
			f << The_surface[iso_type]->Lines[j];
			f << ", ";
			}
		f << endl;
		}
	f << "};" << endl;

	f << "INT " << prefix << "_make_element_size = " << A->make_element_size << ";" << endl;

	{
	INT *stab_gens_first;
	INT *stab_gens_len;
	INT fst;

	stab_gens_first = NEW_INT(nb_iso);
	stab_gens_len = NEW_INT(nb_iso);
	fst = 0;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		stab_gens_first[iso_type] = fst;
		stab_gens_len[iso_type] = The_surface[iso_type]->stab_gens->gens->len;
		fst += stab_gens_len[iso_type];
		}


	f << "INT " << prefix << "_stab_gens_fst[] = { ";
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << stab_gens_first[iso_type];
		if (iso_type < nb_iso - 1) {
			f << ", ";
			}
		if (((iso_type + 1) % 10) == 0) {
			f << endl << "\t";
			}
		}
	f << "};" << endl;

	f << "INT " << prefix << "_stab_gens_len[] = { ";
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << stab_gens_len[iso_type];
		if (iso_type < nb_iso - 1) {
			f << ", ";
			}
		if (((iso_type + 1) % 10) == 0) {
			f << endl << "\t";
			}
		}
	f << "};" << endl;


	f << "INT " << prefix << "_stab_gens[] = {" << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		INT j;

		for (j = 0; j < stab_gens_len[iso_type]; j++) {
			if (f_vv) {
				cout << "surface_classify_wedge::generate_source_code before extract_strong_generators_in_order generator " << j << " / " << stab_gens_len[iso_type] << endl;
				}
			f << "\t";
			A->element_print_for_make_element(The_surface[iso_type]->stab_gens->gens->ith(j), f);
			f << endl;
			}
		}
	f << "};" << endl;


	FREE_INT(stab_gens_first);
	FREE_INT(stab_gens_len);
	}
	}

	cout << "written file " << fname << " of size " << file_size(fname) << endl;
	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code done" << endl;
		}
}

void surface_classify_wedge::print_surfaces()
{
	INT iso_type;

	cout << "There are " << nb_iso << " surfaces in PG(3," << q << ") up to isomorphism" << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		cout << "surface " << iso_type << " / " << nb_iso << ":" << endl;
		The_surface[iso_type]->print(iso_type);
		}
}

void surface_classify_wedge::print_surface(set_and_stabilizer *SaS, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "surface_classify_wedge::print_surface" << endl;
		}


	if (f_v) {
		cout << "surface_classify_wedge::print_surface" << endl;
		}
}

void surface_classify_wedge::starter_configurations_which_are_involved(INT iso_type,
	INT *&Starter_configuration_idx, INT &nb_starter_conf, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT k, i, cnt, iso;
	INT nb_orbits;

	if (f_v) {
		cout << "surface_classify_wedge::starter_configurations_which_are_involved" << endl;
		}

	k = 5;

	nb_orbits = Classify_double_sixes->Five_plus_one->nb_orbits_at_level(k);
	cnt = 0;
	for (i = 0; i < Classify_double_sixes->nb; i++) {
		iso = is_isomorphic_to[i];
		if (iso == iso_type) {
			cnt++;
			}
		}
	nb_starter_conf = cnt;

	Starter_configuration_idx = NEW_INT(nb_starter_conf);

	cnt = 0;
	for (i = 0; i < Classify_double_sixes->nb; i++) {
		iso = is_isomorphic_to[i];
		if (iso == iso_type) {
			Starter_configuration_idx[cnt++] = i;
			}
		}

	if (f_v) {
		cout << "surface_classify_wedge::starter_configurations_which_are_involved" << endl;
		}
}

void surface_classify_wedge::derived_arcs(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT iso_type;
	INT *Starter_configuration_idx;
	INT nb_starter_conf;
	INT c, s, orb, i;
	INT S[5];
	INT S2[7];
	INT K1[7];
	INT w[6];
	INT v[6];
	INT Arc[6];
	INT four_lines[4];
	INT trans12[2];
	INT perp_sz;
	INT b5;

	if (f_v) {
		cout << "surface_classify_wedge::derived_arcs" << endl;
		}
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		cout << "surface " << iso_type << " / " << nb_iso << ":" << endl;

		starter_configurations_which_are_involved(iso_type,
			Starter_configuration_idx, nb_starter_conf, verbose_level);

		cout << "There are " << nb_starter_conf << " starter configurations which are involved: " << endl;
		INT_vec_print(cout, Starter_configuration_idx, nb_starter_conf);
		cout << endl;

		for (c = 0; c < nb_starter_conf; c++) {
			s = Starter_configuration_idx[c];
			orb = Classify_double_sixes->Idx[s];
			cout << "configuration " << c << " / " << nb_starter_conf << " is " << s << ", which is orbit " << orb << endl;
			Classify_double_sixes->Five_plus_one->get_set_by_level(5, orb, S);
			cout << "starter configuration as neighbors: ";
			INT_vec_print(cout, S, 5);
			cout << endl;
			INT_vec_apply(S, Classify_double_sixes->Neighbor_to_line, S2, 5);
			S2[5] = Classify_double_sixes->pt0_line;

			four_lines[0] = S2[0];
			four_lines[1] = S2[1];
			four_lines[2] = S2[2];
			four_lines[3] = S2[3];
			Surf->perp_of_four_lines(four_lines, trans12, perp_sz, 0 /* verbose_level */);
			if (trans12[0] == Classify_double_sixes->pt0_line) {
				b5 = trans12[1];
				}
			else if (trans12[1] == Classify_double_sixes->pt0_line) {
				b5 = trans12[0];
				}
			else {
				cout << "something is wrong with the starter configuration" << endl;
				exit(1);
				}



			INT *lines;
			INT nb_lines;
			INT lines_meet3[3];
			INT lines_skew3[3];

			lines_meet3[0] = S2[1]; // a_2
			lines_meet3[1] = S2[2]; // a_3
			lines_meet3[2] = S2[3]; // a_4
			lines_skew3[0] = S2[0]; // a_1
			lines_skew3[1] = b5;
			lines_skew3[2] = S2[5]; // b_6

			Surf->lines_meet3_and_skew3(lines_meet3, lines_skew3, lines, nb_lines, 0 /* verbose_level */);
			//Surf->perp_of_three_lines(three_lines, perp, perp_sz, 0 /* verbose_level */);

			cout << "The lines which meet { a_2, a_3, a_4 } and are skew to { a_1, b_5, b_6 } are: ";
			INT_vec_print(cout, lines, nb_lines);
			cout << endl;
			cout << "generator matrices:" << endl;
			Surf->Gr->print_set(lines, nb_lines);

			FREE_INT(lines);

			lines_meet3[0] = S2[0]; // a_1
			lines_meet3[1] = S2[2]; // a_3
			lines_meet3[2] = S2[3]; // a_4
			lines_skew3[0] = S2[1]; // a_2
			lines_skew3[1] = b5;
			lines_skew3[2] = S2[5]; // b6

			Surf->lines_meet3_and_skew3(lines_meet3, lines_skew3, lines, nb_lines, 0 /* verbose_level */);
			//Surf->perp_of_three_lines(three_lines, perp, perp_sz, 0 /* verbose_level */);

			cout << "The lines which meet { a_1, a_3, a_4 } and are skew to { a_2, b_5, b_6 } are: ";
			INT_vec_print(cout, lines, nb_lines);
			cout << endl;
			cout << "generator matrices:" << endl;
			Surf->Gr->print_set(lines, nb_lines);

			FREE_INT(lines);


			cout << "starter configuration as line ranks: ";
			INT_vec_print(cout, S2, 6);
			cout << endl;
			cout << "b5=" << b5 << endl;
			cout << "generator matrices:" << endl;
			Surf->Gr->print_set(S2, 6);
			cout << "b5:" << endl;
			Surf->Gr->print_set(&b5, 1);
			S2[6] = b5;

			INT_vec_apply(S2, Surf->Klein->Line_to_point_on_quadric, K1, 7);
			cout << "starter configuration on the klein quadric: ";
			INT_vec_print(cout, K1, 7);
			cout << endl;

			for (i = 0; i < 7; i++) {
				Surf->O->unrank_point(w, 1, K1[i], 0 /* verbose_level*/);
				cout << i << " / " << 6 << " : ";
				INT_vec_print(cout, w, 6);
				cout << endl;
				}
			Arc[0] = 1;
			Arc[1] = 2;
			for (i = 0; i < 4; i++) {
				Surf->O->unrank_point(w, 1, K1[1 + i], 0 /* verbose_level*/);
				INT_vec_copy(w + 3, v, 3);
				PG_element_rank_modified(*F, v, 1, 3, Arc[2 + i]);
				}
			cout << "The associated arc is ";
			INT_vec_print(cout, Arc, 6);
			cout << endl;
			for (i = 0; i < 6; i++) {
				PG_element_unrank_modified(*F, v, 1, 3, Arc[i]);
				cout << i << " & " << Arc[i] << " & ";
				INT_vec_print(cout, v, 3);
				cout << " \\\\" << endl;
				}

			}

		FREE_INT(Starter_configuration_idx);
		}

	if (f_v) {
		cout << "surface_classify_wedge::derived_arcs done" << endl;
		}
}
#endif


void surface_classify_wedge::identify_surfaces(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "surface_classify_wedge::identify_surfaces" << endl;
		}
	//INT **Label;
	//INT *nb_Labels;
	//INT w, i;
	//INT iso_type;
		
	identify(nb_identify, 
		Identify_label, 
		Identify_coeff, 
		Identify_monomial, 
		Identify_length, 
		//Label, 
		//nb_Labels, 
		verbose_level);


#if 0
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		cout << "iso_type " << iso_type << " / " << nb_iso << ":" << endl;
		if (nb_Labels[iso_type] == 0) {
			continue;
			}
		cout << "Iso type " << iso_type << " is isomorphic to ";
		for (i = 0; i < nb_Labels[iso_type]; i++) {
			w = Label[iso_type][i];
			cout << Identify_label[w] << " ";
			}
		cout << endl;
		}
	
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		FREE_INT(Label[iso_type]);
		}
	FREE_PINT(Label);
	FREE_INT(nb_Labels);
#endif

	
	if (f_v) {
		cout << "surface_classify_wedge::identify_surfaces done" << endl;
		}
}

void surface_classify_wedge::identify(INT nb_identify, 
	BYTE **Identify_label, 
	INT **Identify_coeff, 
	INT **Identify_monomial, 
	INT *Identify_length, 
	//INT **&Label, 
	//INT *&nb_Labels, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT iso_type;
	INT **Elt_isomorphism;
	INT *isomorphic_to;
	INT cnt;
	
	if (f_v) {
		cout << "surface_classify_wedge::identify" << endl;
		}


	cout << "Performing " << nb_identify << " identifications:" << endl;

#if 0
	Label = NEW_PINT(nb_iso);
	nb_Labels = NEW_INT(nb_iso);
	INT_vec_zero(nb_Labels, nb_iso);
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		Label[iso_type] = NEW_INT(nb_identify);
		}
#endif

	Elt_isomorphism = NEW_PINT(nb_identify);
	isomorphic_to = NEW_INT(nb_identify);
	for (cnt = 0; cnt < nb_identify; cnt++) {	
		Elt_isomorphism[cnt] = NEW_INT(A->elt_size_in_INT);
		}


	for (cnt = 0; cnt < nb_identify; cnt++) {

		cout << "identifying surface " << cnt << " / " << nb_identify << " which is " << Identify_label[cnt] << endl;
	
		identify_surface_command_line(cnt, /*Label, nb_Labels,*/ isomorphic_to[cnt], Elt_isomorphism[cnt], verbose_level);
		

		} // next cnt

	cout << "after identify_surface_command_line" << endl;
	for (cnt = 0; cnt < nb_identify; cnt++) {

		cout << "surface " << cnt << " / " << nb_identify << " which is " << Identify_label[cnt] << " is isomorphic to " << isomorphic_to[cnt] << endl;
		}

	INT cnt1, cnt2;
	INT *Elt1, *Elt2, *Elt3, *Elt4, *Elt5;

	Elt1 = NEW_INT(A->elt_size_in_INT);
	Elt2 = NEW_INT(A->elt_size_in_INT);
	Elt3 = NEW_INT(A->elt_size_in_INT);
	Elt4 = NEW_INT(A->elt_size_in_INT);
	Elt5 = NEW_INT(A->elt_size_in_INT);
	
	cout << "finding isomorphisms between surfaces: " << endl;
	for (cnt1 = 0; cnt1 < nb_identify; cnt1++) {

		cout << "surface " << cnt1 << " / " << nb_identify << " which is " << Identify_label[cnt1] << " is isomorphic to " << isomorphic_to[cnt1] << endl;
		for (cnt2 = cnt1 + 1; cnt2 < nb_identify; cnt2++) {
			if (isomorphic_to[cnt2] == isomorphic_to[cnt1]) {
				cout << "surface " << cnt2 << " / " << nb_identify << " which is " << Identify_label[cnt2] << " is also isomorphic to " << isomorphic_to[cnt2] << endl;
				A->element_move(Elt_isomorphism[cnt1], Elt1, 0);
				A->element_move(Elt_isomorphism[cnt2], Elt2, 0);
				A->element_invert(Elt1, Elt3, 0 /* verbose_level */);
				A->element_mult(Elt2, Elt3, Elt4, 0 /* verbose_level */);
				cout << "an isomorphism from surface " << cnt2 << " which is " << Identify_label[cnt2] 
					<< " to surface " << cnt1 << " which is " << Identify_label[cnt1] << " is given by:" << endl;
				A->element_print_quick(Elt4, cout);
				A->element_invert(Elt4, Elt5, 0 /* verbose_level */);
				cout << "The inverse transformation is:" << endl;
				A->element_print_quick(Elt5, cout);
				}
			}
		}

	FREE_INT(Elt1);
	FREE_INT(Elt2);
	FREE_INT(Elt3);
	FREE_INT(Elt4);
	FREE_INT(Elt5);
	FREE_INT(isomorphic_to);
	for (cnt = 0; cnt < nb_identify; cnt++) {	
		FREE_INT(Elt_isomorphism[cnt]);
		}
	FREE_PINT(Elt_isomorphism);
	if (f_v) {
		cout << "surface_classify_wedge::identify done" << endl;
		}
}


void surface_classify_wedge::identify_surface_command_line(INT cnt, 
	//INT **&Label, INT *&nb_Labels, 
	INT &isomorphic_to, INT *Elt_isomorphism, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT *coeff_of_given_surface;
	INT i;
	INT c, m;

	if (f_v) {
		cout << "surface_classify_wedge::identify_surface_command_line" << endl;
		}

	coeff_of_given_surface = NEW_INT(Surf->nb_monomials);
	INT_vec_zero(coeff_of_given_surface, Surf->nb_monomials);
	for (i = 0; i < Identify_length[cnt]; i++) {
		c = Identify_coeff[cnt][i];
		m = Identify_monomial[cnt][i];
		coeff_of_given_surface[m] = c;
		}
	
	identify_surface(coeff_of_given_surface, 
		isomorphic_to, Elt_isomorphism, 
		verbose_level);

#if 0
	if (isomorphic_to >= 0) {
		Label[isomorphic_to][nb_Labels[isomorphic_to]] = cnt;
		nb_Labels[isomorphic_to]++;
		}
#endif

	FREE_INT(coeff_of_given_surface);
	if (f_v) {
		cout << "surface_classify_wedge::identify_surface_command_line done" << endl;
		}
}

void surface_classify_wedge::identify_Sa_and_print_table(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i;
	INT m;
	
	if (f_v) {
		cout << "surface_classify_wedge::identify_Sa_and_print_table" << endl;
		}

	INT *Iso_type;
	INT *Nb_E;

	Iso_type = NEW_INT(q);
	Nb_E = NEW_INT(q);
	for (i = 0; i < q; i++) {
		Iso_type[i] = -1;
		Nb_E[i] = -1;
		}
	identify_Sa(Iso_type, Nb_E, verbose_level);


	m = q - 3;
	cout << "\\begin{array}{c|*{" << m << "}{c}}" << endl;
	for (i = 0; i < m; i++) {
		cout << " & " << i + 2;
		}
	cout << "\\\\" << endl;
	cout << "\\hline" << endl;
	cout << "\\# E ";
	for (i = 0; i < m; i++) {
		cout << " & ";
		if (Nb_E[i + 2] == -1) {
			cout << "\\times ";
			}
		else {
			cout << Nb_E[i + 2];
			}
		}
	cout << "\\\\" << endl;
	cout << "\\hline" << endl;
	cout << "\\mbox{Iso} ";
	for (i = 0; i < m; i++) {
		cout << " & ";
		if (Nb_E[i + 2] == -1) {
			cout << "\\times ";
			}
		else {
			cout << Iso_type[i + 2];
			}
		}
	cout << "\\\\" << endl;
	cout << "\\hline" << endl;
	cout << "\\end{array}" << endl;
	if (f_v) {
		cout << "surface_classify_wedge::identify_Sa_and_print_table done" << endl;
		}
}

void surface_classify_wedge::identify_Sa(INT *Iso_type, INT *Nb_E, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT a, b, alpha, beta, nb_E;
	INT *coeff;
	INT iso_type;
	INT *Elt;

	if (f_v) {
		cout << "surface_classify_wedge::identify_Sa" << endl;
		}

	coeff = NEW_INT(Surf->nb_monomials);
	Elt = NEW_INT(A->elt_size_in_INT);
	cout << "surface_classify_wedge::identify_Sa looping over all a:" << endl;
	b = 1;
	for (a = 2; a < q - 1; a++) {
		cout << "surface_classify_wedge::identify_Sa a = " << a << endl;

		Iso_type[a] = -1;
		Nb_E[a] = -1;

		INT Lines27[27];
		
		if (Surf->create_surface_ab(a, b, Lines27, 
			alpha, beta, nb_E, 
			verbose_level)) {

			Surf->create_equation_Sab(a, b, coeff, 0 /* verbose_level*/);
		
			identify_surface(coeff, 
				iso_type, Elt, 
				verbose_level);
			
			cout << "surface_classify_wedge::identify_Sa a = " << a << " is isomorphic to iso_type " << iso_type << ", an isomorphism is:" << endl;
			A->element_print_quick(Elt, cout);
			
			Iso_type[a] = iso_type;
			Nb_E[a] = nb_E;
			}
			
		}

	FREE_INT(coeff);
	FREE_INT(Elt);
	if (f_v) {
		cout << "surface_classify_wedge::identify_Sa done" << endl;
		}
	
}

void surface_classify_wedge::identify_surface(INT *coeff_of_given_surface, 
	INT &isomorphic_to, INT *Elt_isomorphism, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT line_idx, subset_idx;
	INT double_six_orbit, iso_type, /*orb2,*/ idx2;

	if (f_v) {
		cout << "surface_classify_wedge::identify_surface" << endl;
		}

	isomorphic_to = -1;

	INT *Points;
	INT nb_points;
	INT *Lines;
	INT nb_lines;

	if (f_v) {
		cout << "identifying the surface ";
		INT_vec_print(cout, coeff_of_given_surface, Surf->nb_monomials);
		cout << " = ";
		Surf->print_equation(cout, coeff_of_given_surface);
		cout << endl;
		}


	Points = NEW_INT(Surf->P->N_points);
	Lines = NEW_INT(27);

	Surf->enumerate_points(coeff_of_given_surface, Points, nb_points, 0 /* verbose_level */);
	if (f_v) {
		cout << "The surface to be identified has " << nb_points << " points" << endl;
		}

	Surf->P->find_lines_which_are_contained(Points, nb_points, Lines, nb_lines, 27 /* max_lines */, 0 /* verbose_level */);

	if (f_v) {
		cout << "The surface has " << nb_lines << " lines" << endl;
		}
	if (nb_lines != 27 && nb_lines != 21) {
		cout << "the input surface has " << nb_lines << " lines" << endl;
		cout << "something is wrong with the input surface, skipping" << endl;
		FREE_INT(Points);
		FREE_INT(Lines);
		return;
		}

	INT *Adj;


	Surf->compute_adjacency_matrix_of_line_intersection_graph(Adj, Lines, nb_lines, 0 /* verbose_level */);


	set_of_sets *line_intersections;
	INT *Starter_Table;
	INT nb_starter;

	line_intersections = NEW_OBJECT(set_of_sets);

	line_intersections->init_from_adjacency_matrix(nb_lines, Adj, 0 /* verbose_level */);

	Surf->list_starter_configurations(Lines, nb_lines, 
		line_intersections, Starter_Table, nb_starter, verbose_level);

	INT S3[6];
	INT K1[6];
	//INT W1[6];
	//INT W2[6];
	//INT W3[6];
	INT W4[6];
	INT h, l;
	INT f;

	if (nb_starter == 0) {
		cout << "nb_starter == 0" << endl;
		exit(1);
		}
	l = 0;
	line_idx = Starter_Table[l * 2 + 0];
	subset_idx = Starter_Table[l * 2 + 1];

	Surf->create_starter_configuration(line_idx, subset_idx, line_intersections, Lines, S3, 0 /* verbose_level */);


	if (f_v) {
		cout << "surface_classify_wedge::identify_surface The starter configuration is S3=";
		INT_vec_print(cout, S3, 6);
		cout << endl;
		}


	INT_vec_apply(S3, Surf->Klein->Line_to_point_on_quadric, K1, 6);
		// transform the five lines plus transversal 
		// into points on the Klein quadric

	for (h = 0; h < 5; h++) {
		f = Surf->O->evaluate_bilinear_form_by_rank(K1[h], K1[5]);
		if (f) {
			cout << "surface_classify_wedge::identify_surface K1[" << h << "] and K1[5] are not collinear" << endl;
			exit(1);
			}
		}


	//Surf->line_to_wedge_vec(S3, W1, 5);
		// transform the five lines into wedge coordinates

	if (f_v) {
		cout << "surface_classify_wedge::identify_surface before Classify_double_sixes->identify_five_plus_one" << endl;
		}
	Classify_double_sixes->identify_five_plus_one(S3 /* five_lines */, S3[5] /* transversal_line */, 
		W4 /* INT *five_lines_out_as_neighbors */, idx2 /* &orbit_index */, 
		Elt2 /* transporter */, verbose_level);
	
	if (f_v) {
		cout << "surface_classify_wedge::identify_surface The five plus one configuration lies in orbit " << idx2 << endl;
		cout << "An isomorphism is given by:" << endl;
		A->element_print_quick(Elt2, cout);
		}

#if 0


	A->make_element_which_moves_a_line_in_PG3q(Surf->Gr, S3[5], Elt0, 0 /* verbose_level */);


	A2->map_a_set(W1, W2, 5, Elt0, 0 /* verbose_level */);

	INT_vec_search_vec(Classify_double_sixes->Neighbors, Classify_double_sixes->nb_neighbors, W2, 5, W3);

	if (f_v) {
		cout << "down coset " << l << " / " << nb_starter << " tracing the set ";
		INT_vec_print(cout, W3, 5);
		cout << endl;
		}
	idx2 = Classify_double_sixes->gen->trace_set(W3, 5, 5, W4, Elt1, 0 /* verbose_level */);

	
	A->element_mult(Elt0, Elt1, Elt2, 0);
#endif


	if (!INT_vec_search(Classify_double_sixes->Po, Classify_double_sixes->Flag_orbits->nb_flag_orbits, idx2, f)) {
		cout << "cannot find orbit in Po" << endl;
		exit(1);
		}
	if (f_v) {
		cout << "surface_classify_wedge::identify_surface flag orbit = " << f << endl;
		}


	double_six_orbit = Classify_double_sixes->Flag_orbits->Flag_orbit_node[f].upstep_primary_orbit;

	if (f_v) {
		cout << "surface_classify_wedge::identify_surface double_six_orbit = " << double_six_orbit << endl;
		}

	if (double_six_orbit < 0) {
		cout << "surface_classify_wedge::identify_surface double_six_orbit < 0, something is wrong" << endl;
		exit(1);
		}
	if (Classify_double_sixes->Flag_orbits->Flag_orbit_node[f].f_fusion_node) {

		if (f_v) {
			cout << "surface_classify_wedge::identify_surface the flag orbit is a fusion node" << endl;
			}

		A->element_mult(Elt2, Classify_double_sixes->Flag_orbits->Flag_orbit_node[f].fusion_elt, Elt3, 0);
		}
	else {

		if (f_v) {
			cout << "surface_classify_wedge::identify_surface the flag orbit is a definition node" << endl;
			}

		A->element_move(Elt2, Elt3, 0);
		}

	if (f_v) {
		cout << "An isomorphism is given by:" << endl;
		A->element_print_quick(Elt3, cout);
		}

	iso_type = Flag_orbits->Flag_orbit_node[double_six_orbit].upstep_primary_orbit;

	if (f_v) {
		cout << "surface_classify_wedge::identify_surface iso_type = " << iso_type << endl;
		}

	if (Flag_orbits->Flag_orbit_node[double_six_orbit].f_fusion_node) {
		A->element_mult(Elt3, Flag_orbits->Flag_orbit_node[double_six_orbit].fusion_elt, Elt_isomorphism, 0);
		}
	else {
		A->element_move(Elt3, Elt_isomorphism, 0);
		}

	//iso_type = is_isomorphic_to[orb2];
	//A->element_mult(Elt2, Isomorphisms->ith(orb2), Elt_isomorphism, 0);

	if (f_v) {
		cout << "The surface is isomorphic to surface " << iso_type << endl;
		cout << "An isomorphism is given by:" << endl;
		A->element_print_quick(Elt_isomorphism, cout);
		}
	isomorphic_to = iso_type;

	INT *Elt_isomorphism_inv;

	Elt_isomorphism_inv = NEW_INT(A->elt_size_in_INT);
	A->element_invert(Elt_isomorphism, Elt_isomorphism_inv, 0);

	INT *image;

	image = NEW_INT(nb_points);
	A->map_a_set_and_reorder(Points, image, nb_points, Elt_isomorphism, 0 /* verbose_level */);

	if (f_v) {
		cout << "The inverse isomorphism is given by:" << endl;
		A->element_print_quick(Elt_isomorphism_inv, cout);

		cout << "The image of the set of points is: ";
		INT_vec_print(cout, image, nb_points);
		cout << endl;
		}

#if 0
	INT i;
	for (i = 0; i < nb_points; i++) {
		if (image[i] != The_surface[isomorphic_to]->Surface[i]) {
			cout << "points disagree!" << endl;
			exit(1);
			}
		}
	cout << "the image set agrees with the point set of the chosen representative" << endl;
#endif

	FREE_INT(image);

	INT *coeffs_transformed;

	coeffs_transformed = NEW_INT(Surf->nb_monomials);
	



	INT idx;
	INT Lines0[27];
	INT eqn0[20];

	cout << "the surface in the list is = " << endl;
	idx = Surfaces->Orbit[isomorphic_to].orbit_index;
	INT_vec_copy(Surfaces->Rep + idx * Surfaces->representation_sz, Lines0, 27);
	
	Surf->build_cubic_surface_from_lines(27, Lines0, eqn0, 0 /* verbose_level*/);
	PG_element_normalize_from_front(*F, eqn0, 1, Surf->nb_monomials);

	INT_vec_print(cout, eqn0, Surf->nb_monomials);
	//INT_vec_print(cout, The_surface[isomorphic_to]->coeff, Surf->nb_monomials);
	cout << " = ";
	Surf->print_equation(cout, eqn0 /*The_surface[isomorphic_to]->coeff*/);
	cout << endl;


	matrix_group *mtx;

	mtx = A->G.matrix_grp;
	
	if (mtx->f_semilinear) {
		INT n, frob;

		n = mtx->n;
		frob = Elt_isomorphism[n * n];
		Surf->substitute_semilinear(coeff_of_given_surface, coeffs_transformed, TRUE, frob, Elt_isomorphism_inv, 0 /* verbose_level */);
		}
	else {
		Surf->substitute_semilinear(coeff_of_given_surface, coeffs_transformed, FALSE, 0, Elt_isomorphism_inv, 0 /* verbose_level */);
		}

#if 0
	cout << "coeffs_transformed = " << endl;
	INT_vec_print(cout, coeffs_transformed, Surf->nb_monomials);
	cout << " = ";
	Surf->print_equation(cout, coeffs_transformed);
	cout << endl;
#endif

	PG_element_normalize_from_front(*F, coeffs_transformed, 1, Surf->nb_monomials);

	cout << "the surface to be identified was " << endl;
	INT_vec_print(cout, coeff_of_given_surface, Surf->nb_monomials);
	cout << " = ";
	Surf->print_equation(cout, coeff_of_given_surface);
	cout << endl;


	cout << "coeffs_transformed (and normalized) = " << endl;
	INT_vec_print(cout, coeffs_transformed, Surf->nb_monomials);
	cout << " = ";
	Surf->print_equation(cout, coeffs_transformed);
	cout << endl;


	

	FREE_OBJECT(line_intersections);
	FREE_INT(Starter_Table);
	FREE_INT(Adj);
	FREE_INT(Points);
	FREE_INT(Lines);
	FREE_INT(Elt_isomorphism_inv);
	FREE_INT(coeffs_transformed);
	if (f_v) {
		cout << "surface_classify_wedge::identify_surface done" << endl;
		}
}

void surface_classify_wedge::latex_surfaces(ostream &ost, INT f_with_stabilizers)
{
	BYTE title[10000];
	BYTE title_ds[10000];

	sprintf(title, "Cubic Surfaces with 27 Lines in $\\PG(3,%ld)$", q);
	sprintf(title_ds, "Double Sixes in $\\PG(3,%ld)$", q);

	//ost << "\\clearpage" << endl;
	//ost << "\\subsection*{" << title << "}" << endl;


	ost << "\\subsection*{The Group $\\PGGL(4," << q << ")$}" << endl;

	{
	longinteger_object go;
	A->Strong_gens->group_order(go);

	ost << "The order of the group is ";
	go.print_not_scientific(ost);
	ost << "\\\\" << endl;

	ost << "\\bigskip" << endl;
	}

#if 0
	Classify_double_sixes->print_five_plus_ones(ost);


	Classify_double_sixes->Double_sixes->print_latex(ost, title_ds);
#endif

	Surfaces->print_latex(ost, title, f_with_stabilizers);


#if 1
	INT verbose_level = 1;
	INT orbit_index;
	
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		report_surface(ost, orbit_index, verbose_level);
		}
#endif
}

void surface_classify_wedge::report_surface(ostream &ost, INT orbit_index, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	set_and_stabilizer *SaS;
	INT Lines[27];
	INT equation[20];

	if (f_v) {
		cout << "surface_classify_wedge::report_surface orbit_index = " << orbit_index << endl;
		}

	ost << endl << "\\clearpage" << endl << endl;
	ost << "\\section*{Surface $" << q << "\\#" << orbit_index << "$}" << endl;


	SaS = Surfaces->get_set_and_stabilizer(orbit_index, 0 /* verbose_level */);
	INT_vec_copy(SaS->data, Lines, 27);
	
	Surf->build_cubic_surface_from_lines(27, Lines, equation, 0 /* verbose_level */);
	PG_element_normalize_from_front(*F, equation, 1, 20);


	//Surf->print_equation_wrapped(ost, equation);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO" << endl;
		}
	surface_object *SO;

	SO = NEW_OBJECT(surface_object);
	SO->init(Surf, Lines, equation, TRUE /*f_find_double_six_and_rearrange_lines*/, verbose_level);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->enumerate_points" << endl;
		}
	SO->enumerate_points(verbose_level);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->compute_plane_type_by_points" << endl;
		}
	SO->compute_plane_type_by_points(0 /*verbose_level*/);


	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->compute_tritangent_planes" << endl;
		}
	SO->compute_tritangent_planes(0 /*verbose_level*/);


	SO->print_equation(ost);


	surface_object_with_action *SOA;

	SOA = NEW_OBJECT(surface_object_with_action);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SOA->init" << endl;
		}
	SOA->init_surface_object(Surf_A, SO, 
		SaS->Strong_gens, verbose_level);
	if (f_v) {
		cout << "surface_classify_wedge::report_surface after SOA->init" << endl;
		}


	longinteger_object ago;
	SaS->Strong_gens->group_order(ago);
	ost << "The automorphism group of the surface has order " << ago << "\\\\" << endl;
	ost << "The automorphism group is the following group\\\\" << endl;

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SaS->Strong_gens->print_generators_tex" << endl;
		}
	SaS->Strong_gens->print_generators_tex(ost);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->print_general" << endl;
		}
	SO->print_general(ost);


	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->print_lines" << endl;
		}
	SO->print_lines(ost);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->print_points" << endl;
		}
	SO->print_points(ost);


	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->print_tritangent_planes" << endl;
		}
	SO->print_tritangent_planes(ost);


	//New_clebsch->SO->print_planes_in_trihedral_pairs(fp);

	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SO->print_generalized_quadrangle" << endl;
		}
	SO->print_generalized_quadrangle(ost);


	if (f_v) {
		cout << "surface_classify_wedge::report_surface before SOA->quartic" << endl;
		}
	//SOA->quartic(ost,  verbose_level);

	FREE_OBJECT(SOA);
	FREE_OBJECT(SO);
	FREE_OBJECT(SaS);
	
	if (f_v) {
		cout << "surface_classify_wedge::report_surface orbit_index = " << orbit_index << " done" << endl;
		}
}

void surface_classify_wedge::generate_source_code(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);
	BYTE fname[1000];
	BYTE *prefix;
	//INT iso_type;
	INT orbit_index;
	//INT *rep;
	INT i, j;

	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code" << endl;
		}
	sprintf(fname, "%s.C", fname_base);
	prefix = fname_base;
	
	{
	ofstream f(fname);

	f << "INT " << prefix << "_nb_reps = " << Surfaces->nb_orbits << ";" << endl;
	f << "INT " << prefix << "_size = " << Surf->nb_monomials << ";" << endl;


	
	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing reps" << endl;
		}
	f << "// the equations:" << endl;
	f << "INT " << prefix << "_reps[] = {" << endl;
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {


		set_and_stabilizer *SaS;
		INT Lines[27];
		INT equation[20];

		if (f_v) {
			cout << "surface_classify_wedge::generate_source_code orbit_index = " << orbit_index << endl;
			}

		SaS = Surfaces->get_set_and_stabilizer(orbit_index, 0 /* verbose_level */);
		INT_vec_copy(SaS->data, Lines, 27);
	
		Surf->build_cubic_surface_from_lines(27, Lines, equation, 0 /* verbose_level */);
		PG_element_normalize_from_front(*F, equation, 1, 20);

		f << "\t";
		for (i = 0; i < Surf->nb_monomials; i++) {
			f << equation[i];
			f << ", ";
			}
		f << endl;

		FREE_OBJECT(SaS);

		}
	f << "};" << endl;



	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing stab_order" << endl;
		}
	f << "// the stabilizer orders:" << endl;
	f << "const BYTE *" << prefix << "_stab_order[] = {" << endl;
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {

		longinteger_object ago;
		
		Surfaces->Orbit[orbit_index].gens->group_order(ago);

		f << "\t\"";
		
		ago.print_not_scientific(f);
		f << "\"," << endl;

		}
	f << "};" << endl;


	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing nb_E" << endl;
		}
	f << "// the number of Eckardt points:" << endl;
	f << "INT " << prefix << "_nb_E[] = { " << endl << "\t";
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		set_and_stabilizer *SaS;
		INT Lines[27];
		INT equation[27];
		INT *Pts;
		INT nb_pts;
		set_of_sets *pts_on_lines;
		INT nb_E;

		if (f_v) {
			cout << "surface_classify_wedge::generate_source_code orbit_index = " << orbit_index << endl;
			}

		SaS = Surfaces->get_set_and_stabilizer(orbit_index, 0 /* verbose_level */);
		INT_vec_copy(SaS->data, Lines, 27);
		Surf->build_cubic_surface_from_lines(27, Lines, equation, 0 /* verbose_level */);
		PG_element_normalize_from_front(*F, equation, 1, 20);

		Pts = NEW_INT(Surf->nb_pts_on_surface);
		Surf->enumerate_points(equation, Pts, nb_pts, 0 /* verbose_level */);
		if (nb_pts != Surf->nb_pts_on_surface) {
			cout << "surface_classify_wedge::generate_source_code nb_pts != Surf->nb_pts_on_surface" << endl;
			exit(1);
			}
		Surf->compute_points_on_lines(Pts, nb_pts, 
			Lines, 27 /*nb_lines*/, 
			pts_on_lines, 
			verbose_level);
		nb_E = pts_on_lines->number_of_eckardt_points(verbose_level);

		f << nb_E;
		if (orbit_index < Surfaces->nb_orbits - 1) {
			f << ", ";
			}
		if (((orbit_index + 1) % 10) == 0) {
			f << endl << "\t";
			}

	
		FREE_OBJECT(pts_on_lines);
		FREE_INT(Pts);
		FREE_OBJECT(SaS);
		}
	f << "};" << endl;


#if 0
	f << "INT " << prefix << "_single_six[] = { " << endl;
	for (iso_type = 0; iso_type < nb_iso; iso_type++) {
		f << "\t" << The_surface[iso_type]->S2[5];
		for (j = 0; j < 5; j++) {
			f << ", ";
			f << The_surface[iso_type]->S2[j];
			}
		f << ", " << endl;
		}
	f << "};" << endl;
#endif


	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing Lines" << endl;
		}
	f << "// the lines in the order double six a_i, b_i and 15 more lines c_ij:" << endl;
	f << "INT " << prefix << "_Lines[] = { " << endl;


	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {


		set_and_stabilizer *SaS;
		INT Lines[27];

		if (f_v) {
			cout << "surface_classify_wedge::generate_source_code orbit_index = " << orbit_index << endl;
			}

		SaS = Surfaces->get_set_and_stabilizer(orbit_index, 0 /* verbose_level */);
		INT_vec_copy(SaS->data, Lines, 27);

		f << "\t";
		for (j = 0; j < 27; j++) {
			f << Lines[j];
			f << ", ";
			}
		f << endl;

		FREE_OBJECT(SaS);
		}
	f << "};" << endl;

	f << "INT " << prefix << "_make_element_size = " << A->make_element_size << ";" << endl;
	
	{
	INT *stab_gens_first;
	INT *stab_gens_len;
	INT fst;

	stab_gens_first = NEW_INT(Surfaces->nb_orbits);
	stab_gens_len = NEW_INT(Surfaces->nb_orbits);
	fst = 0;
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		stab_gens_first[orbit_index] = fst;
		stab_gens_len[orbit_index] = Surfaces->Orbit[orbit_index].gens->gens->len;
		//stab_gens_len[orbit_index] = The_surface[iso_type]->stab_gens->gens->len;
		fst += stab_gens_len[orbit_index];
		}


	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing stab_gens_fst" << endl;
		}
	f << "INT " << prefix << "_stab_gens_fst[] = { ";
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		f << stab_gens_first[orbit_index];
		if (orbit_index < Surfaces->nb_orbits - 1) {
			f << ", ";
			}
		if (((orbit_index + 1) % 10) == 0) {
			f << endl << "\t";
			}
		}
	f << "};" << endl;
	
	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing stab_gens_len" << endl;
		}
	f << "INT " << prefix << "_stab_gens_len[] = { ";
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		f << stab_gens_len[orbit_index];
		if (orbit_index < Surfaces->nb_orbits - 1) {
			f << ", ";
			}
		if (((orbit_index + 1) % 10) == 0) {
			f << endl << "\t";
			}
		}
	f << "};" << endl;


	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code preparing stab_gens" << endl;
		}
	f << "INT " << prefix << "_stab_gens[] = {" << endl;
	for (orbit_index = 0; orbit_index < Surfaces->nb_orbits; orbit_index++) {
		INT j;
		
		for (j = 0; j < stab_gens_len[orbit_index]; j++) {
			if (f_vv) {
				cout << "surface_classify_wedge::generate_source_code before extract_strong_generators_in_order generator " << j << " / " << stab_gens_len[orbit_index] << endl;
				}
			f << "\t";
			A->element_print_for_make_element(Surfaces->Orbit[orbit_index].gens->gens->ith(j), f);
			//A->element_print_for_make_element(The_surface[iso_type]->stab_gens->gens->ith(j), f);
			f << endl;
			}
		}
	f << "};" << endl;

	
	FREE_INT(stab_gens_first);
	FREE_INT(stab_gens_len);
	}
	}

	cout << "written file " << fname << " of size " << file_size(fname) << endl;
	if (f_v) {
		cout << "surface_classify_wedge::generate_source_code done" << endl;
		}
}




