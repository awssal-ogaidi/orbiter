// surface_classify.C
// 
// Anton Betten
// September 1, 2016
//
//
// 
//
//

#include "orbiter.h"


// global data:

INT t0; // the system time when the program started

int main(int argc, const char **argv);
INT callback_check_surface(INT len, INT *S, void *data, INT verbose_level);

int main(int argc, const char **argv)
{
	t0 = os_ticks();
	

	//start_memory_debug();
	
	INT f_memory_dump_at_end = FALSE;
	const BYTE *memory_dump_at_end_fname = NULL;


	{
	finite_field *F;
	linear_group_description *Descr;
	linear_group *LG;


	INT verbose_level = 0;
	INT f_linear = FALSE;
	INT f_report = FALSE;
	INT f_read_double_sixes = FALSE;
	INT f_double_sixes_only = FALSE;
	INT f_read_surfaces = FALSE;
	INT q;
	INT f_draw_poset = FALSE;
	INT f_draw_poset_full = FALSE;
	INT f_automatic_memory_dump = FALSE;
	INT automatic_dump_interval = 0;
	const BYTE *automatic_dump_mask = NULL;
	INT f_memory_dump_at_peak = FALSE;
	const BYTE *memory_dump_at_peak_fname = NULL;


	INT i;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-linear") == 0) {
			f_linear = TRUE;
			Descr = NEW_OBJECT(linear_group_description);
			i += Descr->read_arguments(argc - (i - 1), argv + i, verbose_level);

			cout << "-linear" << endl;
			}
		else if (strcmp(argv[i], "-report") == 0) {
			f_report = TRUE;
			cout << "-report" << endl;
			}
		else if (strcmp(argv[i], "-read_double_sixes") == 0) {
			f_read_double_sixes = TRUE;
			cout << "-read_double_sixes" << endl;
			}
		else if (strcmp(argv[i], "-double_sixes_only") == 0) {
			f_double_sixes_only = TRUE;
			cout << "-double_sixes_only" << endl;
			}
		else if (strcmp(argv[i], "-read_surfaces") == 0) {
			f_read_surfaces = TRUE;
			cout << "-read_surfaces" << endl;
			}
		else if (strcmp(argv[i], "-draw_poset") == 0) {
			f_draw_poset = TRUE;
			cout << "-draw_poset" << endl;
			}
		else if (strcmp(argv[i], "-draw_poset_full") == 0) {
			f_draw_poset_full = TRUE;
			cout << "-draw_poset_full" << endl;
			}
		else if (strcmp(argv[i], "-memory_debug") == 0) {
			f_memory_debug = TRUE;
			cout << "-memory_debug" << endl;
			}
		else if (strcmp(argv[i], "-memory_debug_verbose_level") == 0) {
			memory_debug_verbose_level = atoi(argv[++i]);
			cout << "-memory_debug_verbose_level " << memory_debug_verbose_level << endl;
			}
		else if (strcmp(argv[i], "-automatic_memory_dump") == 0) {
			f_automatic_memory_dump = TRUE;
			automatic_dump_interval = atoi(argv[++i]);
			automatic_dump_mask = argv[++i];
			cout << "-automatic_memory_dump " << automatic_dump_interval << " " << automatic_dump_mask << endl;
			}
		else if (strcmp(argv[i], "-memory_dump_at_peak") == 0) {
			f_memory_dump_at_peak = TRUE;
			memory_dump_at_peak_fname = argv[++i];
			cout << "-memory_dump_at_peak " << memory_dump_at_peak_fname << endl;
			}
		else if (strcmp(argv[i], "-memory_dump_at_end") == 0) {
			f_memory_dump_at_end = TRUE;
			memory_dump_at_end_fname = argv[++i];
			cout << "-memory_dump_at_end " << memory_dump_at_end_fname << endl;
			}
		}



	//f_memory_debug = TRUE;
	//f_memory_debug_verbose = TRUE;

	if (!f_linear) {
		cout << "please use option -linear ..." << endl;
		exit(1);
		}

	if (f_automatic_memory_dump) {
		global_mem_object_registry.set_automatic_dump(
				automatic_dump_interval, automatic_dump_mask,
				verbose_level);
	}

	INT f_v = (verbose_level >= 1);
	

	F = NEW_OBJECT(finite_field);
	F->init(Descr->input_q, 0);

	Descr->F = F;
	q = Descr->input_q;
	


	LG = NEW_OBJECT(linear_group);
	if (f_v) {
		cout << "surface_classify before LG->init, creating the group" << endl;
		}

	LG->init(Descr, verbose_level - 1);
	
	if (f_v) {
		cout << "surface_classify after LG->init" << endl;
		}

	surface_classify_wedge *SCW;

	SCW = NEW_OBJECT(surface_classify_wedge);

	if (f_v) {
		cout << "surface_classify before SCW->init" << endl;
		}
	
	SCW->init(F, LG, argc, argv, 0 /*verbose_level - 1*/);

	if (f_v) {
		cout << "surface_classify after SCW->init" << endl;
		}

	if (f_read_double_sixes) {


		{
		BYTE fname[1000];
	
		sprintf(fname, "Double_sixes_q%ld.data", q);
		cout << "Reading file " << fname << " of size " << file_size(fname) << endl;
		{

		ifstream fp(fname);

		if (f_v) {
			cout << "surface_classify before SCW->Classify_double_sixes->read_file" << endl;
			}
		SCW->Classify_double_sixes->read_file(fp, verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->Classify_double_sixes->read_file" << endl;
			}
		}
		}



		}

	else {
	
		if (f_v) {
			cout << "surface_classify before SCW->Classify_double_sixes->classify_partial_ovoids" << endl;
			}
		SCW->Classify_double_sixes->classify_partial_ovoids(f_draw_poset, 
			f_draw_poset_full, 
			verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->Classify_double_sixes->classify_partial_ovoids" << endl;
			}

		if (f_v) {
			cout << "surface_classify before SCW->Classify_double_sixes->classify" << endl;
			}
		SCW->Classify_double_sixes->classify(verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->Classify_double_sixes->classify" << endl;
			}



		{
		BYTE fname[1000];
	
		sprintf(fname, "Double_sixes_q%ld.data", q);
		{

		ofstream fp(fname);

		if (f_v) {
			cout << "surface_classify before SCW->Classify_double_sixes->write_file" << endl;
			}
		SCW->Classify_double_sixes->write_file(fp, verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->Classify_double_sixes->write_file" << endl;
			}
		}
		cout << "Written file " << fname << " of size " << file_size(fname) << endl;
		}
		
		}


	if (f_v) {
		cout << "surface_classify writing cheat sheet on double sixes" << endl;
		}
	{
	BYTE fname[1000];
	BYTE title[1000];
	BYTE author[1000];

	sprintf(title, "Cheat Sheet on Double Sixes over GF(%ld) ", q);
	sprintf(author, "");
	sprintf(fname, "Double_sixes_q%ld.tex", q);

		{
		ofstream fp(fname);
		
		//latex_head_easy(fp);
		latex_head(fp, FALSE /* f_book */, TRUE /* f_title */, 
			title, author, 
			FALSE /*f_toc */, FALSE /* f_landscape */, FALSE /* f_12pt */, 
			TRUE /*f_enlarged_page */, TRUE /* f_pagenumbers*/, 
			NULL /* extra_praeamble */);


		SCW->Classify_double_sixes->print_five_plus_ones(fp);

		SCW->Classify_double_sixes->Double_sixes->print_latex(fp, 
			"Double Sixes", FALSE /* f_with_stabilizers*/);

		latex_foot(fp);
		}
	cout << "Written file " << fname << " of size " << file_size(fname) << endl;
	}
	if (f_v) {
		cout << "surface_classify writing cheat sheet on double sixes done" << endl;
		}


	if (f_double_sixes_only) {
		cout << "f_double_sixes_only is true so we terminate now."
		exit(0);
		}


	if (f_read_surfaces) {


		{
		BYTE fname[1000];
	
		sprintf(fname, "Surfaces_q%ld.data", q);
		cout << "Reading file " << fname << " of size " << file_size(fname) << endl;
		{

		ifstream fp(fname);

		if (f_v) {
			cout << "surface_classify before SCW->read_file" << endl;
			}
		SCW->read_file(fp, verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->read_file" << endl;
			}
		}
		}


		}
	else {

		cout << "surface_classify classifying surfaces" << endl;

		if (f_v) {
			cout << "surface_classify before SCW->classify_surfaces_from_double_sixes" << endl;
			}
		SCW->classify_surfaces_from_double_sixes(verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->classify_surfaces_from_double_sixes" << endl;
			}

		{
		BYTE fname[1000];
	
		sprintf(fname, "Surfaces_q%ld.data", q);
		{

		ofstream fp(fname);

		if (f_v) {
			cout << "surface_classify before SCW->write_file" << endl;
			}
		SCW->write_file(fp, verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->write_file" << endl;
			}
		}
		cout << "Written file " << fname << " of size " << file_size(fname) << endl;
		}


		}



	if (f_report) {
		{
		BYTE fname[1000];
		BYTE title[1000];
		BYTE author[1000];
		INT f_with_stabilizers = TRUE;

		sprintf(title, "Cubic Surfaces with 27 Lines over GF(%ld) ", q);
		sprintf(author, "");
		sprintf(fname, "Surfaces_q%ld.tex", q);

			{
			ofstream fp(fname);
		
			//latex_head_easy(fp);
			latex_head(fp, FALSE /* f_book */, TRUE /* f_title */, 
				title, author, 
				FALSE /*f_toc */, FALSE /* f_landscape */, FALSE /* f_12pt */, 
				TRUE /*f_enlarged_page */, TRUE /* f_pagenumbers*/, 
				NULL /* extra_praeamble */);


			SCW->latex_surfaces(fp, f_with_stabilizers);

			latex_foot(fp);
			}
		cout << "Written file " << fname << " of size " << file_size(fname) << endl;
		}
		}

#if 1
	if (SCW->nb_identify) {
		if (f_v) {
			cout << "surface_classify before SCW->identify_surfaces" << endl;
			}
		SCW->identify_surfaces(verbose_level - 1);
		if (f_v) {
			cout << "surface_classify after SCW->identify_surfaces" << endl;
			}
		}
#endif

#if 1
	SCW->generate_source_code(verbose_level);
#endif


#if 0
	cout << "classify_surfaces, we are done but all data is still in memory, before doing a dump" << endl;
	registry_dump();
	cout << "classify_surfaces, we are done but all data is still in memory, after doing a dump" << endl;
#endif
	//SCW->identify_Sa_and_print_table(verbose_level);




#if 0
	if (f_v) {
		cout << "surface_classify before SCW->print_surfaces" << endl;
		}
	SCW->print_surfaces();
	if (f_v) {
		cout << "surface_classify after SCW->print_surfaces" << endl;
		}


	if (f_v) {
		cout << "surface_classify before SCW->derived_arcs" << endl;
		}
	SCW->derived_arcs(verbose_level - 1);
	if (f_v) {
		cout << "surface_classify after SCW->derived_arcs" << endl;
		}
#endif



	//registry_dump_sorted_by_size();
	//global_mem_object_registry.dump();

	if (f_memory_dump_at_peak) {
		global_mem_object_registry.manual_dump_with_file_name(memory_dump_at_peak_fname);
	}



	}

	cout << "classify_surfaces, before the_end" << endl;
	the_end(t0);
	//the_end_quietly(t0);
	cout << "classify_surfaces, after the_end" << endl;
	if (f_memory_dump_at_end) {
		global_mem_object_registry.manual_dump_with_file_name(memory_dump_at_end_fname);
	}
}




