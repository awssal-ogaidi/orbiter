// create_surface_main.C
// 
// Anton Betten
//
// December 8, 2017
//
//
// 
//
//

#include "orbiter.h"

// global data:

int t0; // the system time when the program started

int main(int argc, const char **argv);

int main(int argc, const char **argv)
{
	int verbose_level = 0;
	int i;
	int f_description = FALSE;
	surface_create_description *Descr;
	int nb_transform = 0;
	const char *transform_coeffs[1000];
	int f_inverse_transform[1000];
	int f_quartic = FALSE;

	t0 = os_ticks();


	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-description") == 0) {
			f_description = TRUE;
			Descr = NEW_OBJECT(surface_create_description);
			i += Descr->read_arguments(argc - (i - 1), argv + i,
					verbose_level) - 1;

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
		else if (strcmp(argv[i], "-quartic") == 0) {
			f_quartic = TRUE;
			cout << "-quartic " << endl;
			}
		}
	if (!f_description) {
		cout << "please use option -description ... to enter a "
				"description of the surface" << endl;
		exit(1);
		}

	surface_create *SC;
	int j;

	SC = NEW_OBJECT(surface_create);

	cout << "before SC->init" << endl;
	SC->init(Descr, verbose_level);
	cout << "after SC->init" << endl;
	

	if (nb_transform) {
		cout << "before SC->apply_transformations" << endl;
		SC->apply_transformations(transform_coeffs,
				f_inverse_transform, nb_transform, verbose_level);
		cout << "after SC->apply_transformations" << endl;
		}

	int coeffs_out[20];
	action *A;
	//int *Elt1;
	int *Elt2;
	
	A = SC->Surf_A->A;

	Elt2 = NEW_int(A->elt_size_in_int);
	
	SC->F->init_symbol_for_print("\\omega");

	if (SC->F->e == 1) {
		SC->F->f_print_as_exponentials = FALSE;
	}

	PG_element_normalize(*SC->F, SC->coeffs, 1, 20);

	cout << "We have created the following surface:" << endl;
	cout << "$$" << endl;
	SC->Surf->print_equation_tex(cout, SC->coeffs);
	cout << endl;
	cout << "$$" << endl;


	if (SC->f_has_group) {
		for (i = 0; i < SC->Sg->gens->len; i++) {
			cout << "Testing generator " << i << " / "
					<< SC->Sg->gens->len << endl;
			A->element_invert(SC->Sg->gens->ith(i),
					Elt2, 0 /*verbose_level*/);
	

#if 1
			matrix_group *M;

			M = A->G.matrix_grp;
			M->substitute_surface_eqation(Elt2,
					SC->coeffs, coeffs_out, SC->Surf,
					verbose_level - 1);

#else

			SC->Surf->substitute_semilinear(SC->coeffs,
					coeffs_out,
					FALSE /* f_semilinear */,
					0,
					Elt2,
					0 /*verbose_level*/);
			PG_element_normalize(*SC->F, coeffs_out, 1, 20);
#endif



			if (int_vec_compare(SC->coeffs, coeffs_out, 20)) {
				cout << "error, the transformation does not preserve "
						"the equation of the surface" << endl;
				exit(1);
				}
			cout << "Generator " << i << " / " << SC->Sg->gens->len
					<< " is good" << endl;
			}
		}
	else {
		cout << "We do not have information about "
				"the automorphism group" << endl;
		}


	cout << "We have created the following surface:" << endl;
	cout << "$$" << endl;
	SC->Surf->print_equation_tex(cout, SC->coeffs);
	cout << endl;
	cout << "$$" << endl;

	if (SC->f_has_group) {
		cout << "The stabilizer is generated by:" << endl;
		SC->Sg->print_generators_tex(cout);
		}

	if (SC->f_has_lines) {
		cout << "The lines are:" << endl;
		SC->Surf->Gr->print_set_tex(cout, SC->Lines, 27);
		}

	
	if (SC->f_has_group) {

		cout << "creating surface_object_with_action object" << endl;
		
		surface_object_with_action *SoA;

		SoA = NEW_OBJECT(surface_object_with_action);

		if (SC->f_has_lines) {
			cout << "creating surface using the known lines (which are "
					"arranged with respect to a double six):" << endl;
			SoA->init(SC->Surf_A,
				SC->Lines,
				SC->coeffs,
				SC->Sg,
				FALSE /*f_find_double_six_and_rearrange_lines*/,
				verbose_level);
			}
		else {
			cout << "creating surface from equation only "
					"(no lines):" << endl;
			SoA->init_equation(SC->Surf_A,
				SC->coeffs,
				SC->Sg,
				verbose_level);
			}
		cout << "The surface has been created." << endl;




		six_arcs_not_on_a_conic *Six_arcs;
		int *transporter;

		Six_arcs = NEW_OBJECT(six_arcs_not_on_a_conic);
	

		// classify six arcs not on a conic:

		cout << "Classifying six-arcs not on a conic:" << endl;
		
		Six_arcs->init(SC->F,
			SC->Surf->P2,
			argc, argv, 
			verbose_level);
		transporter = NEW_int(Six_arcs->Gen->A->elt_size_in_int);




		char fname[1000];
		char fname_mask[1000];
		char label[1000];
		char label_tex[1000];

		sprintf(fname, "surface_%s.tex", SC->prefix);
		sprintf(label, "surface_%s", SC->label_txt);
		sprintf(label_tex, "surface %s", SC->label_tex);
		sprintf(fname_mask, "surface_%s_orbit_%%d", SC->prefix);
		{
			ofstream fp(fname);

			latex_head_easy(fp);
			SoA->cheat_sheet(fp, 
				label,
				label_tex,
				TRUE /* f_print_orbits */,
				fname_mask /* const char *fname_mask*/,
				verbose_level);



			surface_object *SO;
			int *Arc_iso; // [72]
			int *Clebsch_map; // [nb_pts]
			int *Clebsch_coeff; // [nb_pts * 4]
			int line_a, line_b;
			int transversal_line;
			int tritangent_plane_rk;
			int plane_rk_global;
			int ds, ds_row;
			SO = SoA->SO;

			fp << endl;
			fp << "\\bigskip" << endl;
			fp << endl;
			fp << "\\section{Clebsch maps}" << endl;
			fp << endl;


			Arc_iso = NEW_int(72);
			Clebsch_map = NEW_int(SO->nb_pts);
			Clebsch_coeff = NEW_int(SO->nb_pts * 4);

			for (ds = 0; ds < 36; ds++) {
				for (ds_row = 0; ds_row < 2; ds_row++) {
					SC->Surf->prepare_clebsch_map(
							ds, ds_row,
							line_a, line_b,
							transversal_line,
							0 /*verbose_level */);


					fp << endl;
					fp << "\\bigskip" << endl;
					fp << endl;
					fp << "\\subsection{Clebsch map for double six "
							<< ds << ", row " << ds_row << "}" << endl;
					fp << endl;



					cout << "computing clebsch map:" << endl;
					SO->compute_clebsch_map(line_a, line_b, 
						transversal_line, 
						tritangent_plane_rk, 
						Clebsch_map,
						Clebsch_coeff,
						verbose_level);


					plane_rk_global = SO->Tritangent_planes[
						SO->Eckardt_to_Tritangent_plane[
							tritangent_plane_rk]];

					int Arc[6];
					int Arc2[6];
					int Blown_up_lines[6];
					int perm[6];
					
					SO->clebsch_map_find_arc_and_lines(
							Clebsch_map,
							Arc,
							Blown_up_lines,
							0 /* verbose_level */);

					for (j = 0; j < 6; j++) {
						perm[j] = j;
						}

					int_vec_heapsort_with_log(Blown_up_lines, perm, 6);
					for (j = 0; j < 6; j++) {
						Arc2[j] = Arc[perm[j]];
						}
					
					
					fp << endl;
					fp << "\\bigskip" << endl;
					fp << endl;
					//fp << "\\section{Clebsch map}" << endl;
					//fp << endl;
					fp << "Line 1 = $";
					fp << SC->Surf->Line_label_tex[line_a];
					fp << "$\\\\" << endl;
					fp << "Line 2 = $";
					fp << SC->Surf->Line_label_tex[line_b];
					fp << "$\\\\" << endl;
					fp << "Transversal line $";
					fp << SC->Surf->Line_label_tex[transversal_line];
					fp << "$\\\\" << endl;
					fp << "Image plane $\\pi_{" << tritangent_plane_rk
							<< "}=" << plane_rk_global << "=$\\\\" << endl;
					fp << "$$" << endl;
					
					fp << "\\left[" << endl;
					SC->Surf->Gr3->print_single_generator_matrix_tex(
							fp, plane_rk_global);
					fp << "\\right]," << endl;

					fp << "$$" << endl;
					fp << "Arc $";
					int_set_print_tex(fp, Arc2, 6);
					fp << "$\\\\" << endl;
					fp << "Half double six: $";
					int_set_print_tex(fp, Blown_up_lines, 6);
					fp << "=\\{";
					for (j = 0; j < 6; j++) {
						fp << SC->Surf->Line_label_tex[Blown_up_lines[j]];
						fp << ", ";
						}
					fp << "\\}$\\\\" << endl;

					fp << "The arc consists of the following "
							"points:\\\\" << endl;
					display_table_of_projective_points(fp,
							SC->F, Arc2, 6, 3);

					int orbit_at_level, idx;
					Six_arcs->Gen->gen->identify(Arc2, 6,
							transporter, orbit_at_level,
							0 /*verbose_level */);

			
					if (!int_vec_search(Six_arcs->Not_on_conic_idx, 
						Six_arcs->nb_arcs_not_on_conic,
						orbit_at_level,
						idx)) {
						cout << "could not find orbit" << endl;
						exit(1);
						}

					fp << "The arc is isomorphic to arc " << idx
							<< " in the list.\\\\" << endl;
					Arc_iso[2 * ds + ds_row] = idx;
					


					SO->clebsch_map_latex(fp, Clebsch_map, Clebsch_coeff);

					//SO->clebsch_map_print_fibers(Clebsch_map);
					}
				}
			
			FREE_int(Clebsch_map);
			FREE_int(Clebsch_coeff);

			
			fp << "The isomorphism type of arc associated with "
					"each half-double six is:" << endl;
			fp << "$$" << endl;
			print_integer_matrix_with_standard_labels(fp,
					Arc_iso, 36, 2, TRUE);
			fp << "$$" << endl;

			//fp << "The six-arcs not on a conic are:\\\\" << endl;
			Six_arcs->report_latex(fp);


			if (f_quartic) {
				SoA->quartic(fp, verbose_level);
				}




	
			latex_foot(fp);
		}
		cout << "Written file " << fname << " of size "
				<< file_size(fname) << endl;



		FREE_OBJECT(SoA);
		FREE_OBJECT(Six_arcs);
		FREE_int(transporter);


		}



	FREE_int(Elt2);

	FREE_OBJECT(SC);

	the_end(t0);
	//the_end_quietly(t0);
}

