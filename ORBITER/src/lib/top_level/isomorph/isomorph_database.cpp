// isomorph_database.C
// 
// Anton Betten
// Oct 21, 2008
//
// moved here from iso.C 3/22/09
// renamed isomorph_database.C from iso2.C 7/14/11
//
//

#include "orbiter.h"

void isomorph::setup_and_open_solution_database(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::setup_and_open_solution_database" << endl;
		}
	if (DB_sol) {
		freeobject(DB_sol);
		DB_sol = NULL;
		}
	DB_sol = (database *)callocobject(DATABASE);
	DB_sol->change_to_database();
	
	init_DB_sol(0 /*verbose_level - 1*/);
	
	DB_sol->open(0 /*verbose_level - 1*/);
}

void isomorph::setup_and_create_solution_database(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::setup_and_create_solution_database" << endl;
		}
	if (DB_sol) {
		freeobject(DB_sol);
		DB_sol = NULL;
		}
	DB_sol = (database *)callocobject(DATABASE);
	DB_sol->change_to_database();
	
	init_DB_sol(0 /*verbose_level - 1*/);
	
	DB_sol->create(0 /*verbose_level - 1*/);
}

void isomorph::close_solution_database(int verbose_level)
{
	DB_sol->close(0/*verbose_level - 1*/);
}

void isomorph::setup_and_open_level_database(int verbose_level)
// Called from do_iso_test, identify and test_hash 
// (Which are all in isomorph_testing.C)
// Calls init_DB_level for D1 and D2 and D1->open and D2->open.
// Calls fopen for fp_ge1 and fp_ge2.
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::setup_and_open_level_database" << endl;
		}

	if (D1) {
		freeobject(D1);
		D1 = NULL;
		}
	if (D2) {
		freeobject(D2);
		D2 = NULL;
		}
	D1 = (database *)callocobject(DATABASE);
	D1->change_to_database();
	D2 = (database *)callocobject(DATABASE);
	D2->change_to_database();
	
	init_DB_level(*D1, level - 1, verbose_level - 1);
	strcpy(fname_ge1, fname_db_level_ge);
	
	init_DB_level(*D2, level, verbose_level - 1);
	strcpy(fname_ge2, fname_db_level_ge);
	
	D1->open(0/*verbose_level - 1*/);
	D2->open(0/*verbose_level - 1*/);
	fp_ge1 = fopen(fname_ge1, "r");
	fp_ge2 = fopen(fname_ge2, "r");
}

void isomorph::close_level_database(int verbose_level)
// Closes D1, D2, fp_ge1, fp_ge2.
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::close_level_database" << endl;
		}
	D1->close(0/*verbose_level - 1*/);
	D2->close(0/*verbose_level - 1*/);
	freeobject(D1);
	freeobject(D2);
	D1 = NULL;
	D2 = NULL;
	fclose(fp_ge1);
	fclose(fp_ge2);
	fp_ge1 = NULL;
	fp_ge2 = NULL;
}

void isomorph::prepare_database_access(int cur_level, int verbose_level)
// sets DB_level to be D1 or D2, depending on cur_level
// Called from make_set_smaller_database
// and load_strong_generators
// and trace_next_point_database
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::prepare_database_access "
				"cur_level=" << cur_level << endl;
		}
	if (cur_level == level - 1) {
		//first_node = gen->first_poset_orbit_node_at_level[level - 1];
		DB_level = D1;
		fp_ge = fp_ge1;
		}
	else if (cur_level == level) {
		//first_node = gen->first_poset_orbit_node_at_level[level];
		DB_level = D2;
		fp_ge = fp_ge2;
		}
	else {
		cout << "iso_node " << iso_nodes
				<< " isomorph::prepare_database_access "
						"cur_level = " << cur_level << endl;
		exit(1);
		}
}

void isomorph::init_DB_sol(int verbose_level)
// We assume that the starter is of size 'level' and that 
// fields 4,..., 4+level-1 are the starter values
{
	int f_v = (verbose_level >= 1);
	database &D = *DB_sol;
	btree B1, B2, B3, B4;
	int f_compress = TRUE;
	int f_duplicatekeys = TRUE;
	int i;

	if (f_v) {
		cout << "isomorph::init_DB_sol" << endl;
		}
	//cout << "isomorph::init_DB_sol before D.init" << endl;
	D.init(fname_db1, VECTOR, f_compress);


	//cout << "isomorph::init_DB_sol before B1.init" << endl;
	B1.init(fname_db2, f_duplicatekeys, 0 /* btree_idx */);
	B1.add_key_int4(0, 0); 
		// the index of the starter
	B1.add_key_int4(1, 0);
		// the number of this solution within the solutions 
		// of the same starter
	D.btree_access().append(B1);


	//cout << "isomorph::init_DB_sol before B2.init" << endl;
	B2.init(fname_db3, f_duplicatekeys, 1 /* btree_idx */);
		// entries 4, 5, ... 4 + level - 1 are the starter values 
	for (i = 0; i < level; i++) {
		B2.add_key_int4(4 + i, 0);
		}
	//B2.add_key_int4(3, 0);
	//B2.add_key_int4(4, 0);
	//B2.add_key_int4(5, 0);
	//B2.add_key_int4(6, 0);
	//B2.add_key_int4(7, 0);
	//B2.add_key_int4(8, 0);
	D.btree_access().append(B2);


	//cout << "isomorph::init_DB_sol before B3.init" << endl;
	B3.init(fname_db4, f_duplicatekeys, 2 /* btree_idx */);
	B3.add_key_int4(2, 0);
		// the id
	D.btree_access().append(B3);


	B4.init(fname_db5, f_duplicatekeys, 3 /* btree_idx */);
	B4.add_key_int4(0, 0); 
		// the index of the starter
	B4.add_key_int4(3, 0); 
		// the hash value
	D.btree_access().append(B4);


	//cout << "isomorph::init_DB_sol done" << endl;
}

void isomorph::add_solution_to_database(int *data, 
	int nb, int id, int no, int nb_solutions, int h, uint4 &datref, 
	int print_mod, int verbose_level)
{
	int f_vvv = (verbose_level >= 3);
	Vector v;
	int j;
	
	//h = int_vec_hash_after_sorting(data + 1, size);
	v.m_l_n(4 + size);
	v.m_ii(0, data[0]); // starter number
	v.m_ii(1, nb); // solution number within this starter
	v.m_ii(2, id); // global solution number
	v.m_ii(3, h); // the hash number
	for (j = 0; j < size; j++) {
		v.m_ii(4 + j, data[1 + j]);
		}
	if (f_vvv || ((no % print_mod) == 0)) {
		cout << "Solution no " << no << " / " << nb_solutions
				<< " starter case " << data[0] << " nb " << nb
				<< " id=" << id << " : " << v << " : " << endl;
		}
		
	DB_sol->add_object_return_datref(v, datref, 0/*verbose_level - 3*/);
	if (f_vvv) {
		cout << "solution added" << endl;
		}
}

void isomorph::load_solution(int id, int *data)
{
	int i, j, datref;
	Vector v;
	//int verbose_level = 0;
	
	if (f_use_table_of_solutions) {
		for (j = 0; j < size; j++) {
			data[j] = table_of_solutions[id * size + j];
			}
		return;
		}
	//DB_sol->get_object_by_unique_int4(2, id, v, verbose_level);
	datref = id_to_datref[id];
	DB_sol->get_object((uint4) datref, v, 0/*verbose_level*/);
	
	//cout << v << endl;
	for (i = 0; i < size; i++) {
		data[i] = v.s_ii(4 + i);
		}
}

void isomorph::load_solution_by_btree(
		int btree_idx, int idx, int &id, int *data)
{
	//int i;
	Vector v;

	cout << "isomorph::load_solution_by_btree" << endl;
	exit(1);
#if 0
	DB_sol->ith_object(idx, btree_idx, v, 0 /*verbose_level*/);
	for (i = 0; i < size; i++) {
		data[i] = v.s_ii(4 + i);
		}
	id = v.s_ii(2);
#endif
}



int isomorph::find_extension_easy(
		int *set, int case_nb, int &idx, int verbose_level)
// case_nb is the starter that is associated with the given set.
// We wish to find out if the set is a solution that has been stored 
// with that starter. 
// If so, we wish to determine the number of that solution amongst all 
// solutions for that starter (returned in idx). 
// Otherwise, we return FALSE.

// returns TRUE if found, FALSE otherwise
// Called from identify_solution
// Linear search through all solutions at a given starter.
// calls load solution for each of the solutions 
// stored with the case and compares the vectors.
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "isomorph::find_extension_easy "
				"case_nb=" << case_nb << endl;
		}
#if 0
	int ret1, idx1;
	int ret2, idx2;
	ret1 = find_extension_easy_old(D, set, case_nb, idx1, verbose_level);
	if (f_v) {
		cout << "isomorph::find_extension_easy idx1=" << idx1 << endl;
		}
	ret2 = find_extension_easy_new(D, set, case_nb, idx2, verbose_level);
	if (f_v) {
		cout << "isomorph::find_extension_easy idx2=" << idx2 << endl;
		}
	if (ret1 != ret2) {
		cout << "isomorph::find_extension_easy ret1 != ret2" << endl;
		exit(1);
		}
	if (ret1 && (idx1 != idx2)) {
		cout << "isomorph::find_extension_easy "
				"ret1 && (idx1 != idx2)" << endl;
		exit(1);
		}
	idx = idx1;
	return ret1;
#else
	int ret;
	ret = find_extension_easy_new(set, case_nb, idx, verbose_level);
	return ret;
#endif
}

int isomorph::find_extension_search_interval(int *set, 
	int first, int len, int &idx, 
	int f_btree_idx, int btree_idx, 
	int f_through_hash, int verbose_level)
{
	int *data = find_extension_set1;
	int i, id = 0;
	
	for (i = 0; i < len; i++) {
		if (f_btree_idx) {
			load_solution_by_btree(btree_idx, first + i, id, data);
			}
		else {
			if (f_through_hash) {
				id = hash_vs_id_id[first + i];
				}
			else {
				id = first + i;
				}
			load_solution(id, data);
			}
		int_vec_heapsort(data + level, size - level);
		if (int_vec_compare(set + level, data + level, size - level) == 0) {
			break;
			}
		}
	if (i == len) {
		return FALSE;
		//cout << "isomorph::find_extension_search_interval "
		//"did not find extension" << endl;
		//exit(1);
		}
	idx = id;
	return TRUE;
}

int isomorph::find_extension_easy_old(int *set,
		int case_nb, int &idx, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int first, len, ret;
	
	if (f_v) {
		cout << "isomorph::find_extension_easy_old" << endl;
		cout << "case_nb=" << case_nb << endl;
		}
	int_vec_heapsort(set + level, size - level);
	first = solution_first[case_nb];
	len = solution_len[case_nb];
	ret = find_extension_search_interval(set, 
		first, len, idx, FALSE, 0, FALSE, verbose_level);
	if (f_v) {
		if (ret) {
			cout << "isomorph::find_extension_easy_old "
					"solution found at idx=" << idx << endl;
			}
		else {
			cout << "isomorph::find_extension_easy_old "
					"solution not found" << endl;
			}
		}
	return ret;
}

int isomorph::find_extension_easy_new(int *set,
		int case_nb, int &idx, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = FALSE; // (verbose_level >= 2);
	int ret;
	int f_found, first, idx2, len;
	
	if (f_v) {
		cout << "isomorph::find_extension_easy_new" << endl;
		}
	int_vec_heapsort(set + level, size - level);
	
	int h;

	h = int_vec_hash_after_sorting(set, size);
	if (f_v) {
		cout << "isomorph::find_extension_easy_new h=" << h << endl;
		}


	if (f_v) {
		cout << "isomorph::find_extension_easy_new before "
				"int_vec_search_first_occurence(h)" << endl;
		}
	f_found = int_vec_search_first_occurence(hash_vs_id_hash,
			N, h, first, 0 /*verbose_level*/);
	if (f_v) {
		cout << "isomorph::find_extension_easy_new after "
				"int_vec_search_first_occurence(h) f_found=" << f_found << endl;
		}

	if (!f_found) {
		ret = FALSE;
		goto finish;
		}
	if (f_v) {
		cout << "isomorph::find_extension_easy_new before "
				"int_vec_search_first_occurence(h + 1) h+1=" << h + 1 << endl;
		}
	f_found = int_vec_search_first_occurence(hash_vs_id_hash,
			N, h + 1, idx2, 0 /*verbose_level*/);
	if (f_v) {
		cout << "isomorph::find_extension_easy_new after "
				"int_vec_search_first_occurence(h+1) f_found=" << f_found << endl;
		}
	len = idx2 - first;
	if (f_v) {
		cout << "isomorph::find_extension_easy_new len=" << len << endl;
		}
#if 0

	if (f_vv) {
		cout << "case_nb=" << case_nb << " h=" << h << endl;
		}
	btree &B4 = DB_sol->btree_access_i(3);
	//int l0 = case_nb;
	//int u0 = case_nb;
	//int l1 = h;
	//int u1 = h + 1;
	int first, last, len;
	int f_found1, f_found2;
	f_found1 = B4.search_int4_int4(case_nb, h, first,
			0 /*verbose_level */);

#if 0
	B4.search_interval_int4_int4(l0, u0, 
		l1, u1, 
		first, len, 
		3 /*verbose_level*/);
#endif
	if (f_vv) {
		cout << "f_found1=" << f_found1 << " first=" << first << endl;
		}
	f_found2 = B4.search_int4_int4(case_nb, h + 1, last,
			0 /*verbose_level */);
	if (f_vv) {
		cout << "f_found2=" << f_found2 << " last=" << last << endl;
		}
	len = last - first + 1;
#endif

	if (len == 0) {
		ret = FALSE;
		}
	else {
		if (f_v) {
			cout << "isomorph::find_extension_easy_new before "
					"find_extension_search_interval" << endl;
			}
		ret = find_extension_search_interval(set, 
			first, len, idx, FALSE, 3, TRUE, 0 /*verbose_level*/);
		if (f_v) {
			cout << "isomorph::find_extension_easy_new after "
					"find_extension_search_interval ret=" << ret << endl;
			}
		}

finish:


	if (f_v) {
		if (ret) {
			cout << "isomorph::find_extension_easy_new "
					"solution found at idx=" << idx << endl;
			}
		else {
			cout << "isomorph::find_extension_easy_new "
					"solution not found" << endl;
			}
		}
	return ret;
	
}

int isomorph::open_database_and_identify_object(int *set,
	int *transporter,
	int f_implicit_fusion, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int r;
	int f_failure_to_find_point;

	if (f_v) {
		cout << "isomorph::open_database_and_identify_object" << endl;
		}

	setup_and_open_solution_database(0/*verbose_level - 1*/);
	setup_and_open_level_database(0/*verbose_level - 1*/);
	
	r = identify_solution(set, transporter, 
		f_implicit_fusion, f_failure_to_find_point, verbose_level - 2);
	
	if (f_failure_to_find_point) {
		cout << "isomorph::open_database_and_identify_object: "
				"f_failure_to_find_point" << endl;
		r = -1;
 		}

	else {
		if (f_v) {
			cout << "isomorph::open_database_and_identify_object: "
					"object identified as belonging to isomorphism class "
					<< r << endl;
			}
		}

	close_solution_database(0/*verbose_level - 1*/);
	close_level_database(0/*verbose_level - 1*/);
	if (f_v) {
		cout << "isomorph::open_database_and_identify_object done" << endl;
		}
	return r;
}



void isomorph::init_DB_level(database &D, int level, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	btree B1, B2;
	int f_compress = TRUE;
	int f_duplicatekeys = TRUE;
	int i;

	if (f_v) {
		cout << "isomorph::init_DB_level level=" << level << endl;
		}
	sprintf(fname_db_level, "%sstarter_lvl_%d.db", prefix, level);
	sprintf(fname_db_level_idx1, "%sstarter_lvl_%d_a.idx", prefix, level);
	sprintf(fname_db_level_idx2, "%sstarter_lvl_%d_b.idx", prefix, level);
	sprintf(fname_db_level_ge, "%sstarter_lvl_%d_ge.bin", prefix, level);

	D.init(fname_db_level, VECTOR, f_compress);
	
	B1.init(fname_db_level_idx1, f_duplicatekeys, 0 /* btree_idx */);
	B1.add_key_int4(0, 0); 
	D.btree_access().append(B1);

	B2.init(fname_db_level_idx2, f_duplicatekeys, 1 /* btree_idx */);
		// 2 up to 2+level-1 are the values of the starter (of size level)
	for (i = 0; i < level; i++) {
		B2.add_key_int4(2 + i, 0);
		}
	D.btree_access().append(B2);
}

void isomorph::create_level_database(int level, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = FALSE;//(verbose_level >= 2);
	//int f_vvv = (verbose_level >= 3);
	int f, nb_nodes, I, J, i, j, idx, print_mod = 1;
	poset_orbit_node *O;
	int set1[1000];
	int set2[1000];
	//char *elt;
	
	if (f_v) {
		cout << "isomorph::create_level_database "
				"level = " << level << endl;
		cout << "verbose_level=" << verbose_level << endl;
		}
	
	f = gen->first_poset_orbit_node_at_level[level];
	nb_nodes = gen->nb_orbits_at_level(level);

	if (f_vv) {
		cout << "f=" << f << endl;
		cout << "nb_nodes=" << nb_nodes << endl;
		}
	
	database D;
	FILE *fp;
	int cnt = 0;
	
	//elt = NEW_char(gen->A->coded_elt_size_in_char);
	
	init_DB_level(D, level, verbose_level - 1);
	
	D.create(0/*verbose_level - 1*/);
	fp = fopen(fname_db_level_ge, "wb");
	
	//if (nb_nodes > 10000) {
		print_mod = 1000;
		//}
	for (i = 0; i < nb_nodes; i++) {
		I = f + i;
		O = &gen->root[I];
		O->store_set_to(gen, level - 1, set1);
		if (f_v && ((i % print_mod) == 0)) {
			cout << "isomorph::create_level_database level "
					<< level << " i=" << i << " / " << nb_nodes
					<< " set=";
			int_vec_print(cout, set1, level);
			cout << endl;
			}
		
		int len, nb_fusion;
		Vector v;


			// # ints   description
			// 1         global ID
			// 1         ancestor global ID
			// level     the set itself
			// 1         # strong generators
			// A->base_len: tl  (only if # strong generators is != 0)
			// 1         nb_extensions
			// for each extension:
			// 1         pt
			// 1         orbit_len
			// 1         type
			// 1         global ID of descendant node 
			//           (if type == 1 EXTENSION or type == 2 FUSION)
			
			// and finally:
			// 1         ref of first group element
			// (altogether, we are storing 
			// # strong_generators  +
			// # fusion nodes       
			// group elements.
			// they have refs d, d+1,...
			
			
			// A->coded_elt_size_in_char


		nb_fusion = 0;
		for (j = 0; j < O->nb_extensions; j++) {
			if (O->E[j].type == 2) {
				nb_fusion++;
				}
			}
		
		len = 1 + 1 + level + 1;
		if (O->nb_strong_generators) {
			len += gen->Poset->A->base_len;
			}
		len += 1;
		len += 4 * O->nb_extensions;
		len += 1; // for the reference of the first group element
		//len += O->nb_strong_generators;
		//len += nb_fusion;
		
		v.m_l_n(len);
		idx = 0;
		v.m_ii(idx++, I);
		v.m_ii(idx++, O->prev);
		for (j = 0; j < level; j++) {
			v.m_ii(idx++, set1[j]);
			}
		v.m_ii(idx++, O->nb_strong_generators);
		if (O->nb_strong_generators) {
			for (j = 0; j < gen->Poset->A->base_len; j++) {
				v.m_ii(idx++, O->tl[j]);
				}
			}
		v.m_ii(idx++, O->nb_extensions);
		for (j = 0; j < O->nb_extensions; j++) {
			v.m_ii(idx++, O->E[j].pt);
			set1[level] = O->E[j].pt;
			v.m_ii(idx++, O->E[j].orbit_len);
			v.m_ii(idx++, O->E[j].type);
			if (O->E[j].type == 1) {
				v.m_ii(idx++, O->E[j].data);
				}
			else if (O->E[j].type == 2) {
				gen->Poset->A->element_retrieve(O->E[j].data, gen->Elt1, FALSE);
				

				gen->Poset->A2->map_a_set(set1, set2, level + 1, gen->Elt1, 0);
				int_vec_heapsort(set2, level + 1);

				if (f_vv /*f_vv && (i % print_mod) == 0*/) {
					cout << "mapping ";
					int_vec_print(cout, set1, level + 1);
					cout << " to ";
					int_vec_print(cout, set2, level + 1);
					cout << endl;
					}
	
	
				J = gen->find_poset_orbit_node_for_set(level + 1,
						set2, FALSE /* f_tolerant */, 0);
				v.m_ii(idx++, J);
				}
			else {
				cout << "unknown type " << O->E[j].type
						<< " i=" << i << " j=" << j << endl;
				exit(1);
				}
			}
#if 0
		int len_mem, h, idx1;
		char *mem;
		if (idx != len - 1) {
			cout << "idx != len - 1, idx=" << idx << " len=" << len
					<< " i=" << i << " j=" << j << endl;
			exit(1);
			}
		len_mem = (O->nb_strong_generators + nb_fusion) *
				gen->A->coded_elt_size_in_char;
		mem = NEW_char(len_mem);
		idx1 = 0;
		for (j = 0; j < O->nb_strong_generators; j++) {
			gen->A->element_retrieve(O->hdl_strong_generators[j],
					gen->Elt1, FALSE);
			gen->A->element_pack(gen->Elt1, elt, FALSE);
			for (h = 0; h < gen->A->coded_elt_size_in_char; h++) {
				mem[idx1++] = elt[h];
				}
			}
		for (j = 0; j < O->nb_extensions; j++) {
			if (O->E[j].type == 1)
				continue;
			gen->A->element_retrieve(O->E[j].data, gen->Elt1, FALSE);
			gen->A->element_pack(gen->Elt1, elt, FALSE);
			for (h = 0; h < gen->A->coded_elt_size_in_char; h++) {
				mem[idx1++] = elt[h];
				}
			}
		if (idx1 != len_mem) {
			cout << "idx1 != len_mem idx=" << idx << " len_mem=" << len_mem
					<< " i=" << i << " j=" << j << endl;
			exit(1);
			}
		memory M;
		
		M.init(len_mem, mem);
		M.swap(v.s_i(idx));
#else
		v.m_ii(idx++, cnt);
		for (j = 0; j < O->nb_strong_generators; j++) {
			gen->Poset->A->element_retrieve(
					O->hdl_strong_generators[j], gen->Elt1,
					FALSE);
			gen->Poset->A->element_write_file_fp(gen->Elt1, fp,
					0/* verbose_level*/);
			cnt++;
			}
		for (j = 0; j < O->nb_extensions; j++) {
			if (O->E[j].type == 1)
				continue;
			gen->Poset->A->element_retrieve(O->E[j].data, gen->Elt1, FALSE);
			gen->Poset->A->element_write_file_fp(gen->Elt1, fp,
					0/* verbose_level*/);
			cnt++;
			}
		if (idx != len) {
			cout << "idx != len, idx=" << idx << " len=" << len << endl;
			exit(1);
			}
#endif


		D.add_object(v, 0 /*verbose_level - 2*/);
		if (f_v && ((i % print_mod) == 0)) {
			cout << "object " << i << " / " << nb_nodes << " added : ";
			int sz;
			sz = v.csf();
			cout << "size on file = " << sz << ", group element "
					"counter = " << cnt << endl;
			}
		}


	D.close(0/*verbose_level - 1*/);
	fclose(fp);
	if (f_v) {
		cout << "number of group elements in " << fname_db_level_ge
				<< " is " << cnt << endl;
		cout << "file size is " << file_size(fname_db_level_ge) << endl;
		cout << "gen->A->coded_elt_size_in_char="
				<< gen->Poset->A->coded_elt_size_in_char << endl;
		}
	
	//FREE_char(elt);
	
}

void isomorph::load_strong_generators(int cur_level,
		int cur_node_local,
	vector_ge &gens, longinteger_object &go,
	int verbose_level)
// Called from compute_stabilizer and from orbit_representative
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_v5 = (verbose_level >= 5);

	if (f_v) {
		cout << "isomorph::load_strong_generators "
				"cur_level=" << cur_level << " cur_node_local="
				<< cur_node_local << endl;
		}
	if (f_use_database_for_starter) {
		if (f_vv) {
			cout << "isomorph::load_strong_generators "
					"using database" << endl;
			}
		load_strong_generators_database(cur_level, cur_node_local, 
			gens, go, verbose_level);
		if (f_v5) {
			cout << "isomorph::load_strong_generators "
					"found the following strong generators:" << endl;
			gens.print(cout);
			}
		}
	else {
		load_strong_generators_oracle(cur_level, cur_node_local, 
			gens, go, verbose_level);
		}
}

void isomorph::load_strong_generators_oracle(int cur_level,
	int cur_node_local,
	vector_ge &gens, longinteger_object &go, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	poset_orbit_node *O;
	int *tl;
	int i, node;
	longinteger_domain Dom;

	if (f_v) {
		cout << "isomorph::load_strong_generators_oracle "
				"cur_level=" << cur_level << " cur_node_local="
				<< cur_node_local << endl;
		}

	node = gen->first_poset_orbit_node_at_level[cur_level] + cur_node_local;
	O = &gen->root[node];
	if (O->nb_strong_generators == 0) {
		gens.init(gen->Poset->A);
		gens.allocate(0);
		go.create(1);
		goto finish;
		}
	tl = NEW_int(gen->Poset->A->base_len);
	for (i = 0; i < gen->Poset->A->base_len; i++) {
		tl[i] = O->tl[i];
		}
	Dom.multiply_up(go, tl, gen->Poset->A->base_len);
	FREE_int(tl);
	gens.init(gen->Poset->A);
	gens.allocate(O->nb_strong_generators);
	for (i = 0; i < O->nb_strong_generators; i++) {
		gen->Poset->A->element_retrieve(
				O->hdl_strong_generators[i],
				gens.ith(i), FALSE);
		}
finish:
	if (f_v) {
		cout << "isomorph::load_strong_generators_oracle "
				"cur_level=" << cur_level << " cur_node_local="
				<< cur_node_local << " done" << endl;
		}
}

void isomorph::load_strong_generators_database(int cur_level,
		int cur_node_local,
	vector_ge &gens, longinteger_object &go, int verbose_level)
// Reads node cur_node (global index) from database D through btree 0
// Reads generators from file fp_ge
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	int *tmp_ELT;
	Vector v;
	int i;
	int set[1000];
	int *tl;
	longinteger_domain Dom;


	if (f_v) {
		cout << "isomorph::load_strong_generators_database "
				"cur_level=" << cur_level << " cur_node_local="
				<< cur_node_local << endl;
		}

	prepare_database_access(cur_level, verbose_level);

	tmp_ELT = NEW_int(gen->Poset->A->elt_size_in_int);
	
	//cur_node_local = cur_node - first_node;
	if (f_v) {
		cout << "isomorph::load_strong_generators_database "
				"loading object " << cur_node_local << endl;
		}
	DB_level->ith_object(cur_node_local, 0/* btree_idx*/, v,
			0 /*MINIMUM(1, verbose_level - 2)*/);
	
	if (f_vvv) {
		cout << "isomorph::load_strong_generators_database "
				"v=" << v << endl;
		}
	for (i = 0; i < cur_level; i++) {
		set[i] = v.s_ii(2 + i);
		}
	if (f_vv) {
		cout << "isomorph::load_strong_generators_database set: ";
		int_vec_print(cout, set, cur_level);
		cout << endl;
		}
	int nb_strong_generators;
	int pos, ref;
	pos = 2 + cur_level;
	nb_strong_generators = v.s_ii(pos++);
	if (f_vv) {
		cout << "isomorph::load_strong_generators_database "
				"nb_strong_generators="
				<< nb_strong_generators << endl;
		}
	if (nb_strong_generators == 0) {
		gens.init(gen->Poset->A);
		gens.allocate(0);
		go.create(1);
		goto finish;
		}
	tl = NEW_int(gen->Poset->A->base_len);
	for (i = 0; i < gen->Poset->A->base_len; i++) {
		tl[i] = v.s_ii(pos++);
		}
	Dom.multiply_up(go, tl, gen->Poset->A->base_len);
	FREE_int(tl);
	pos = v.s_l() - 1;
	ref = v.s_ii(pos++);
	if (f_vv) {
		cout << "isomorph::load_strong_generators_database "
				"ref = " << ref << endl;
		}

	gens.init(gen->Poset->A);
	gens.allocate(nb_strong_generators);

	fseek(fp_ge, ref * gen->Poset->A->coded_elt_size_in_char, SEEK_SET);
	for (i = 0; i < nb_strong_generators; i++) {
		gen->Poset->A->element_read_file_fp(gens.ith(i), fp_ge,
				0/* verbose_level*/);
		}
finish:
	FREE_int(tmp_ELT);

	if (f_v) {
		cout << "isomorph::load_strong_generators_database "
				"cur_level=" << cur_level << " cur_node_local="
				<< cur_node_local << " done" << endl;
		}
	
}




