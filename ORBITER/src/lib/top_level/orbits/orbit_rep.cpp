// orbit_rep.C
// 
// Anton Betten
// started Nov 6, 2012
//
//
// 
//
//

#include "orbiter.h"



orbit_rep::orbit_rep()
{
	null();
}

orbit_rep::~orbit_rep()
{
	freeself();
}

void orbit_rep::null()
{
	rep = NULL;
	Stab = NULL;
	Strong_gens = NULL;
	candidates = NULL;
	stab_go = NULL;
	nb_cases = 0;
}

void orbit_rep::freeself()
{
	if (rep) {
		FREE_int(rep);
		}
	if (Stab) {
		FREE_OBJECT(Stab);
		}
	if (Strong_gens) {
		FREE_OBJECT(Strong_gens);
		}
	if (candidates) {
		FREE_int(candidates);
		}
	if (stab_go) {
		FREE_OBJECT(stab_go);
		}
	null();
}

void orbit_rep::init_from_file(
	action *A, char *prefix,
	int level, int orbit_at_level, int level_of_candidates_file, 
	void (*early_test_func_callback)(int *S, int len, 
		int *candidates, int nb_candidates, 
		int *good_candidates, int &nb_good_candidates, 
		void *data, int verbose_level), 
	void *early_test_func_callback_data, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int rep_sz;
	
	if (f_v) {
		cout << "orbit_rep::init_from_file orbit_at_level="
				<< orbit_at_level << endl;
		}
	orbit_rep::A = A;
	orbit_rep::level = level;
	orbit_rep::orbit_at_level = orbit_at_level;
	orbit_rep::early_test_func_callback =
			early_test_func_callback;
	orbit_rep::early_test_func_callback_data =
			early_test_func_callback_data;

	read_orbit_rep_and_candidates_from_files_and_process(A, prefix, 
		level, orbit_at_level, level_of_candidates_file, 
		early_test_func_callback, 
		early_test_func_callback_data, 
		rep,
		rep_sz,
		Stab,
		Strong_gens, 
		candidates,
		nb_candidates,
		nb_cases, 
		verbose_level - 1);
		// SNAKES_AND_LADDERS/snakes_and_ladders_global.C
	
	stab_go = NEW_OBJECT(longinteger_object);
	Stab->group_order(*stab_go);

	if (f_v) {
		cout << "orbit_rep::init_from_file orbit_at_level="
				<< orbit_at_level << " done, "
				"stabilizer order = " << *stab_go << endl;
		}

}



