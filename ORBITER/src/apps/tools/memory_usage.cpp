// memory_usage.cpp
//
// Anton Betten
// August 27, 2018
//
//
//

#include "orbiter.h"


// global data:

INT t0; // the system time when the program started



int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;

	INT f_file_mask = FALSE;
	INT range_first = 0;
	INT range_len = 0;
	const BYTE *fname_mask;

	INT nb_extra_files = 0;
	const BYTE *extra_files[1000];


	t0 = os_ticks();

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-file_mask") == 0) {
			f_file_mask = TRUE;
			i++;
			range_first = atoi(argv[++i]);
			range_len = atoi(argv[++i]);
			fname_mask = argv[i];
			cout << "-file_mask " << range_first << " " << range_len
					<< " " << fname_mask << endl;
			}
		else if (strcmp(argv[i], "-extra_file") == 0) {
			extra_files[nb_extra_files] = argv[++i];
			cout << "-extra_file " << extra_files[nb_extra_files] << endl;
			nb_extra_files++;
			}
		}

	//INT f_v = (verbose_level >= 1);
	//INT f_vv = (verbose_level >= 2);


	INT nb_files = 0;
	INT h;

	if (f_file_mask) {
		nb_files += range_len;
	}
	nb_files += nb_extra_files;


	BYTE fname[1000];
	BYTE **fnames;
	fnames = NEW_PBYTE(nb_files);
	h = 0;
	if (f_file_mask) {
		for (i = 0; i < range_len; i++) {
			sprintf(fname, fname_mask, range_first + i);
			fnames[h] = NEW_BYTE(strlen(fname) + 1);
			strcpy(fnames[h], fname);
			h++;
		}
		for (i = 0; i < nb_extra_files; i++) {
			fnames[h] = NEW_BYTE(strlen(extra_files[i]) + 1);
			strcpy(fnames[h], extra_files[i]);
			h++;
		}
	}

	INT idx;
	mem_object_registry **M;

	M = (mem_object_registry **) NEW_pvoid(nb_files);
	for (idx = 0; idx < nb_files; idx++) {

		spreadsheet *S;
		BYTE *p;
		INT N, a;

		S = NEW_OBJECT(spreadsheet);

		cout << "Reading table " << fnames[idx] << endl;
		if (file_size(fnames[idx]) <= 0) {
			cout << "error: the file " << fnames[idx] << " does not exist" << endl;
			exit(1);
		}
		S->read_spreadsheet(fnames[idx], 0 /*verbose_level*/);
		cout << "Table " << fnames[idx] << " has been read" << endl;

		N = S->nb_rows - 1;


		M[idx] = NEW_OBJECT(mem_object_registry);
		M[idx]->allocate(N, verbose_level);
		for (i = 0; i < N; i++) {

			p = S->get_string(i + 1, 1);
			//sscanf(p, "%ld", &a);
			std::stringstream ss;
			ss << std::hex << p + 2;
			ss >> a;
			M[idx]->entries[i].pointer = (void *) a;

			M[idx]->entries[i].time_stamp = S->get_INT(i + 1, 2);

			M[idx]->entries[i].set_type_from_string(S->get_string(i + 1, 3));
			M[idx]->entries[i].object_n = S->get_INT(i + 1, 4);
			M[idx]->entries[i].object_size_of = S->get_INT(i + 1, 5);

			p = S->get_string(i + 1, 6);
			M[idx]->entries[i].extra_type_info = NEW_BYTE(strlen(p) + 1);
			strcpy((char *) M[idx]->entries[i].extra_type_info, p);

			p = S->get_string(i + 1, 7);
			M[idx]->entries[i].source_file = NEW_BYTE(strlen(p) + 1);
			strcpy((char *) M[idx]->entries[i].source_file, p);

			M[idx]->entries[i].source_line = S->get_INT(i + 1, 8);
			}
		M[idx]->nb_entries_used = N;
		M[idx]->dump();
	}



}


