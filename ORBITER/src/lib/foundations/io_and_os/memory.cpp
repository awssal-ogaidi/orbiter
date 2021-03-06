// memory.C
//
// Anton Betten
//
// started:  June 25, 2009




#include "foundations.h"

#define REGISTRY_SIZE 1000
#define POintER_TYPE_INVALID 0
#define POintER_TYPE_SMALLint 1
#define POintER_TYPE_SMALLpint 2
#define POintER_TYPE_int 3
#define POintER_TYPE_pint 4
#define POintER_TYPE_ppint 5
#define POintER_TYPE_char 6
#define POintER_TYPE_uchar 7
#define POintER_TYPE_pchar 8
#define POintER_TYPE_puchar 9
#define POintER_TYPE_PVOID 10
#define POintER_TYPE_OBJECT 11
#define POintER_TYPE_OBJECTS 12



int f_memory_debug = FALSE;
int memory_debug_verbose_level = 0;
mem_object_registry global_mem_object_registry;

static int registry_key_pair_compare_by_size(void *K1v, void *K2v);
static int registry_key_pair_compare_by_type(void *K1v, void *K2v);
static int registry_key_pair_compare_by_location(void *K1v, void *K2v);

mem_object_registry_entry::mem_object_registry_entry()
{
	null();
}

mem_object_registry_entry::~mem_object_registry_entry()
{

}

void mem_object_registry_entry::null()
{
	time_stamp = 0;
	pointer = NULL;
	object_type = POintER_TYPE_INVALID;
	object_n = 0;
	object_size_of = 0;
	extra_type_info = NULL;
	source_file = NULL;
	source_line = 0;
}

void mem_object_registry_entry::set_type_from_string(char *str)
{
	if (strcmp(str, "int") == 0) {
		object_type = POintER_TYPE_SMALLint;
	} else if (strcmp(str, "pint") == 0) {
		object_type = POintER_TYPE_SMALLpint;
	} else if (strcmp(str, "int") == 0) {
		object_type = POintER_TYPE_int;
	} else if (strcmp(str, "pint") == 0) {
		object_type = POintER_TYPE_pint;
	} else if (strcmp(str, "ppint") == 0) {
		object_type = POintER_TYPE_ppint;
	} else if (strcmp(str, "char") == 0) {
		object_type = POintER_TYPE_char;
	} else if (strcmp(str, "uchar") == 0) {
		object_type = POintER_TYPE_uchar;
	} else if (strcmp(str, "pchar") == 0) {
		object_type = POintER_TYPE_pchar;
	} else if (strcmp(str, "puchar") == 0) {
		object_type = POintER_TYPE_puchar;
	} else if (strcmp(str, "pvoid") == 0) {
		object_type = POintER_TYPE_PVOID;
	} else if (strcmp(str, "OBJECT") == 0) {
		object_type = POintER_TYPE_OBJECT;
	} else if (strcmp(str, "OBJECTS") == 0) {
		object_type = POintER_TYPE_OBJECTS;
	} else {
		object_type = POintER_TYPE_INVALID;
	}
}

void mem_object_registry_entry::print_type(ostream &ost)
{
	if (object_type == POintER_TYPE_INVALID) {
		ost << "invalid entry";
		}
	if (object_type == POintER_TYPE_SMALLint) {
		ost << "int";
		}
	else if (object_type == POintER_TYPE_SMALLpint) {
		ost << "pint";
		}
	else if (object_type == POintER_TYPE_int) {
		ost << "int";
		}
	else if (object_type == POintER_TYPE_pint) {
		ost << "pint";
		}
	else if (object_type == POintER_TYPE_ppint) {
		ost << "ppint";
		}
	else if (object_type == POintER_TYPE_char) {
		ost << "char";
		}
	else if (object_type == POintER_TYPE_uchar) {
		ost << "uchar";
		}
	else if (object_type == POintER_TYPE_pchar) {
		ost << "pchar";
		}
	else if (object_type == POintER_TYPE_puchar) {
		ost << "puchar";
		}
	else if (object_type == POintER_TYPE_PVOID) {
		ost << "pvoid";
		}
	else if (object_type == POintER_TYPE_OBJECT) {
		ost << "OBJECT";
		}
	else if (object_type == POintER_TYPE_OBJECTS) {
		ost << "OBJECTS";
		}
	else {
		ost << "unknown" << endl;
		}
}


int mem_object_registry_entry::size_of()
{
	if (object_type == POintER_TYPE_INVALID) {
		cout << "mem_object_registry_entry::size_of invalid entry" << endl;
		exit(1);
		}
	if (object_type == POintER_TYPE_SMALLint) {
		return sizeof(int) * object_n;
		}
	else if (object_type == POintER_TYPE_SMALLpint) {
		return sizeof(int *) * object_n;
		}
	else if (object_type == POintER_TYPE_int) {
		return sizeof(int) * object_n;
		}
	else if (object_type == POintER_TYPE_pint) {
		return sizeof(int *) * object_n;
		}
	else if (object_type == POintER_TYPE_ppint) {
		return sizeof(int **) * object_n;
		}
	else if (object_type == POintER_TYPE_char) {
		return sizeof(char) * object_n;
		}
	else if (object_type == POintER_TYPE_uchar) {
		return sizeof(uchar) * object_n;
		}
	else if (object_type == POintER_TYPE_pchar) {
		return sizeof(char *) * object_n;
		}
	else if (object_type == POintER_TYPE_puchar) {
		return sizeof(uchar *) * object_n;
		}
	else if (object_type == POintER_TYPE_PVOID) {
		return sizeof(pvoid) * object_n;
		}
	else if (object_type == POintER_TYPE_OBJECT) {
		return object_size_of;
		}
	else if (object_type == POintER_TYPE_OBJECTS) {
		return object_n * object_size_of;
		}
	else {
		cout << "mem_object_registry_entry::size_of "
				"unknown object type " << object_type << endl;
		exit(1);
		}
}

void mem_object_registry_entry::print(int line)
{
	cout << line << " : ";
	print_pointer_hex(cout, pointer);
	cout << " : " << time_stamp << " : ";

	print_type(cout);

	cout << " : "
		<< object_n << " : "
		<< object_size_of << " : "
		<< extra_type_info << " : "
		<< source_file << " : "
		<< source_line << endl;

}


void mem_object_registry_entry::print_csv(ostream &ost, int line)
{
	ost << line << ",";
	print_pointer_hex(ost, pointer);
	ost << "," << time_stamp << ",";

	print_type(ost);

	ost << ","
		<< object_n << ","
		<< object_size_of << ","
		<< extra_type_info << ","
		<< source_file << ","
		<< source_line << endl;

}








mem_object_registry::mem_object_registry()
{
	int verbose_level = 1;

	f_automatic_dump = FALSE;
	automatic_dump_interval = 0;
	automatic_dump_fname_mask[0] = 0;

	entries = NULL;
	nb_allocate_total = 0;
	nb_delete_total = 0;
	cur_time = 0;

	f_ignore_duplicates = FALSE;
	f_accumulate = FALSE;

	init(verbose_level);
}

mem_object_registry::~mem_object_registry()
{
	if (entries) {
		delete [] entries;
		entries = NULL;
	}
}

void mem_object_registry::init(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::init" << endl;
	}

	nb_entries_allocated = REGISTRY_SIZE;
	nb_entries_used = 0;

	nb_allocate_total = 0;
	nb_delete_total = 0;
	cur_time = 0;

	f_ignore_duplicates = FALSE;
	f_accumulate = FALSE;

	if (f_v) {
		cout << "mem_object_registry::init trying to allocate "
				<< nb_entries_allocated << " entries" << endl;
	}

	entries = new mem_object_registry_entry[nb_entries_allocated];

	if (f_v) {
		cout << "mem_object_registry::init allocation successful" << endl;
	}


	if (f_v) {
		cout << "mem_object_registry::init done" << endl;
	}
}

void mem_object_registry::accumulate_and_ignore_duplicates(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::accumulate_and_ignore_duplicates" << endl;
	}
	f_accumulate = TRUE;
	f_ignore_duplicates = TRUE;
}

void mem_object_registry::allocate(int N, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate" << endl;
	}

	nb_entries_allocated = N;
	nb_entries_used = 0;

	if (f_v) {
		cout << "mem_object_registry::allocate trying to allocate "
				<< nb_entries_allocated << " entries" << endl;
	}

	entries = new mem_object_registry_entry[nb_entries_allocated];

	if (f_v) {
		cout << "mem_object_registry::allocate allocation successful" << endl;
	}


	if (f_v) {
		cout << "mem_object_registry::allocate done" << endl;
	}
}



void mem_object_registry::set_automatic_dump(
		int automatic_dump_interval, const char *fname_mask,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::set_automatic_dump" << endl;
	}
	f_automatic_dump = TRUE;
	mem_object_registry::automatic_dump_interval = automatic_dump_interval;
	strcpy(automatic_dump_fname_mask, fname_mask);
}

void mem_object_registry::automatic_dump()
{
	if (!f_automatic_dump) {
		return;
	}
	if ((cur_time % automatic_dump_interval) != 0) {
		return;
	}
	char fname[1000];
	int a;

	a = cur_time / automatic_dump_interval;

	cout << "automatic memory dump " << a << endl;
	sprintf(fname, automatic_dump_fname_mask, a);

	dump_to_csv_file(fname);
}

void mem_object_registry::manual_dump()
{
	if (!f_automatic_dump) {
		return;
	}
	char fname[1000];
	int a;

	a = cur_time / automatic_dump_interval + 1;

	sprintf(fname, automatic_dump_fname_mask, a);

	dump_to_csv_file(fname);
}

void mem_object_registry::manual_dump_with_file_name(const char *fname)
{
	dump_to_csv_file(fname);
}

void mem_object_registry::dump()
{
	int i, s, sz;

	cout << "memory registry:" << endl;

	sz = 0;
	for (i = 0; i < nb_entries_used; i++) {
		s = entries[i].size_of();
		sz += s;
	}

	cout << "nb_entries_used=" << nb_entries_used << endl;
	cout << "nb_allocate_total=" << nb_allocate_total << endl;
	cout << "nb_delete_total=" << nb_delete_total << endl;
	cout << "cur_time=" << cur_time << endl;
	cout << "total allocation size in char=" << sz << endl;
	cout << "table of all currently active memory allocations in increasing "
			"order of the value of the pointer" << endl;
	for (i = 0; i < nb_entries_used; i++) {
		entries[i].print(i);
	}
}

void mem_object_registry::dump_to_csv_file(const char *fname)
{
	int i, s, sz;


	{
		ofstream fp(fname);


		//cout << "memory registry:" << endl;

		fp << "Line,Pointer,Timestamp,Type,N,Sizeof,ExtraTypeInfo,File,LineInFile" << endl;
		sz = 0;
		for (i = 0; i < nb_entries_used; i++) {
			s = entries[i].size_of();
			sz += s;
		}

		for (i = 0; i < nb_entries_used; i++) {
			entries[i].print_csv(fp, i);
		}
		fp << "END" << endl;
		fp << "nb_entries_used=" << nb_entries_used << endl;
		fp << "nb_allocate_total=" << nb_allocate_total << endl;
		fp << "nb_delete_total=" << nb_delete_total << endl;
		fp << "cur_time=" << cur_time << endl;
		fp << "total allocation size in char=" << sz << endl;
	}
}


int *mem_object_registry::allocate_int(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_int int[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	int *p;
	p = new int[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_SMALLint, (int) n, sizeof(int),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_int(int *p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_int int[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_int "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

int **mem_object_registry::allocate_pint(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_pint pint[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	int **p;
	p = new pint[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_SMALLpint, (int) n, sizeof(int *),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_pint(int **p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_pint pint[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_pint "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

int ***mem_object_registry::allocate_ppint(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_ppint ppint[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	int ***p;
	p = new ppint[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_ppint, (int) n, sizeof(int **),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_ppint(int ***p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_ppint ppint[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_ppint "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

char *mem_object_registry::allocate_char(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_char char[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	char *p;
	p = new char[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_char, (int) n, sizeof(char),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_char(char *p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_char char[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_char "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

uchar *mem_object_registry::allocate_uchar(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_uchar uchar[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	uchar *p;
	p = new uchar[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_uchar, (int) n, sizeof(uchar),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_uchar(uchar *p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_uchar uchar[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_uchar "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

char **mem_object_registry::allocate_pchar(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_pchar pchar[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	char **p;
	p = new pchar[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_pchar, (int) n, sizeof(char *),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_pchar(char **p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_pchar pchar[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_pchar "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

uchar **mem_object_registry::allocate_puchar(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_puchar puchar[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	uchar **p;
	p = new puchar[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_puchar, (int) n, sizeof(char *),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_puchar(uchar **p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_puchar puchar[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_puchar "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

void **mem_object_registry::allocate_pvoid(int n, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_pvoid pvoid[n], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	void **p;
	p = new pvoid[n];
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_PVOID, (int) n, sizeof(void *),
				"", file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_pvoid(void **p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_pvoid pvoid[n], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_pvoid "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	delete [] p;
}

void *mem_object_registry::allocate_OBJECTS(void *p, int n, int size_of,
		const char *extra_type_info, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_OBJECTS char[n * size_of], "
				"n=" << n << " file=" << file << " line=" << line << endl;
	}
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_OBJECTS, (int) n, size_of,
				extra_type_info, file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_OBJECTS(void *p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_OBJECTS char[n * size_of], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_OBJECTS "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	//delete [] p;
}

void *mem_object_registry::allocate_OBJECT(void *p, int size_of,
		const char *extra_type_info, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::allocate_OBJECT char[size_of], "
				" file=" << file << " line=" << line << endl;
	}
	if (f_memory_debug) {
		add_to_registry(p /* pointer */,
				POintER_TYPE_OBJECT, (int) 1, size_of,
				extra_type_info, file, line,
				memory_debug_verbose_level - 1);
		}
	return p;
}

void mem_object_registry::free_OBJECT(void *p, const char *file, int line)
{
	int f_v = (memory_debug_verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::free_OBJECT char[size_of], "
				" file=" << file << " line=" << line << endl;
	}
	if (p == NULL) {
		cout << "mem_object_registry::free_OBJECTS "
				"NULL pointer, ignoring" << endl;
		cout << "p=" << p << " file=" << file
				<< " line=" << line << endl;
		return;
		}
	if (f_memory_debug) {
		delete_from_registry(p, memory_debug_verbose_level - 1);
	}
	//delete [] p;
}





int mem_object_registry::search(void *p, int &idx)
{
	int l, r, m;
	int f_found = FALSE;

	if (nb_entries_used == 0) {
		idx = 0;
		return FALSE;
		}
	l = 0;
	r = nb_entries_used;
	// invariant:
	// v[i] <= a for i < l;
	// v[i] >  a for i >= r;
	// r - l is the length of the area to search in.
	while (l < r) {
		m = (l + r) >> 1;
		// if the length of the search area is even
		// we examine the element above the middle
		//res = registry_pointer[m] - p;
		//cout << "search l=" << l << " m=" << m << " r="
		//	<< r << "a=" << a << " v[m]=" << v[m] << " res=" << res << endl;
		if (p >= entries[m].pointer) {
			l = m + 1;
			if (p == entries[m].pointer)
				f_found = TRUE;
			}
		else
			r = m;
		}
	// now: l == r;
	// and f_found is set accordingly */
	if (f_found) {
		l--;
	}
	idx = l;
	return f_found;
}

void mem_object_registry::insert_at(int idx)
{
	int i;

	if (nb_entries_used == nb_entries_allocated) {
		nb_entries_allocated = 2 * nb_entries_allocated;
		cout << "mem_object_registry::insert_at reallocating table to "
				<< nb_entries_allocated << " elements" << endl;
		mem_object_registry_entry *old_entries;

		old_entries = entries;
		entries = new mem_object_registry_entry[nb_entries_allocated];
		for (i = 0; i < nb_entries_used; i++) {
			entries[i] = old_entries[i];
		}
		delete [] old_entries;
	}
	for (i = nb_entries_used; i > idx; i--) {
		entries[i] = entries[i - 1];
		}
	entries[idx].null();
	nb_entries_used++;
}

void mem_object_registry::add_to_registry(void *pointer,
		int object_type, int object_n, int object_size_of,
		const char *extra_type_info,
		const char *source_file, int source_line,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int idx;

	if (f_v) {
		cout << "mem_object_registry::add_to_registry" << endl;
	}
	nb_allocate_total++;
	if (search(pointer, idx)) {
		if (f_ignore_duplicates) {

		} else {
			cout << "mem_object_registry::add_to_registry pointer p is "
					"already in the registry, something is wrong" << endl;
			cout << "extra_type_info = " << extra_type_info << endl;
			cout << "source_file = " << source_file << endl;
			cout << "source_line = " << source_line << endl;
			cout << "object_type = " << object_type << endl;
			cout << "object_n = " << object_n << endl;
			cout << "object_size_of = " << object_size_of << endl;
			cout << "the previous object is:" << endl;
			entries[idx].print(idx);
			cout << "ignoring the problem" << endl;
			//exit(1);
		}
	}
	insert_at(idx);
	entries[idx].time_stamp = cur_time;
	entries[idx].pointer = pointer;
	entries[idx].object_type = object_type;
	entries[idx].object_n = object_n;
	entries[idx].object_size_of = object_size_of;
	entries[idx].extra_type_info = extra_type_info;
	entries[idx].source_file = source_file;
	entries[idx].source_line = source_line;


	automatic_dump();
	cur_time++;

	if (f_v) {
		cout << "mem_object_registry::add_to_registry done, there are "
				<< nb_entries_used << " entries in the registry" << endl;
	}
}

void mem_object_registry::delete_from_registry(void *pointer, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int idx, i;

	if (f_v) {
		cout << "mem_object_registry::delete_from_registry" << endl;
	}
	nb_delete_total++;

	if (f_accumulate) {
		// do not delete entries so we can see all allocations
	} else {
		if (!search(pointer, idx)) {
			cout << "mem_object_registry::delete_from_registry pointer is "
					"not in registry, something is wrong; "
					"ignoring, pointer = " << pointer << endl;
			//exit(1);
		}
		for (i = idx + 1; i < nb_entries_used; i++) {
			entries[i - 1] = entries[i];
			}
		entries[nb_entries_used - 1].null();
		nb_entries_used--;
	}
	automatic_dump();
	cur_time++;
	if (f_v) {
		cout << "mem_object_registry::delete_from_registry done, there are "
				<< nb_entries_used << " entries in the registry" << endl;
	}
}
void mem_object_registry::sort_by_size(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::sort_by_size" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_size before Heapsort" << endl;
	}
	Heapsort(entries, nb_entries_used,
		sizeof(mem_object_registry_entry),
		registry_key_pair_compare_by_size);
	if (f_v) {
		cout << "mem_object_registry::sort_by_size after Heapsort" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_size done" << endl;
	}

}

void mem_object_registry::sort_by_location_and_get_frequency(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "mem_object_registry::sort_by_location_and_get_frequency" << endl;
	}

	sort_by_location(verbose_level - 1);

	int nb_types;
	int *type_first;
	int *type_len;
	int c, f, l;

	type_first = new int[nb_entries_used]; // use system memory
	type_len = new int[nb_entries_used];


	nb_types = 0;
	type_first[0] = 0;
	type_len[0] = 1;
	for (i = 1; i < nb_entries_used; i++) {
		c = registry_key_pair_compare_by_location(entries + i, entries + (i - 1));
		if (c == 0) {
			type_len[nb_types]++;
			}
		else {
			type_first[nb_types + 1] =
					type_first[nb_types] + type_len[nb_types];
			nb_types++;
			type_len[nb_types] = 1;
			}
		}
	nb_types++;
	cout << "we have " << nb_types
			<< " different allocation locations:" << endl;

	int t, j, sz, s;
	int *perm;
	int *perm_inv;
	int *frequency;

	perm = new int[nb_types]; // use system memory
	perm_inv = new int[nb_types];
	frequency = new int[nb_types];

	int_vec_copy(type_len, frequency, nb_types);

	int_vec_sorting_permutation(frequency, nb_types,
			perm, perm_inv, FALSE /* f_increasingly */);

	for (t = nb_types - 1; t >= 0; t--) {
		i = perm_inv[t];

		f = type_first[i];
		l = type_len[i];

		sz = 0;
		for (j = 0; j < l; j++) {
			s = entries[f + j].size_of();
			sz += s;
		}

		//idx = entries[f].user_data;
		cout << l << " times file "
				<< entries[f].source_file << " line "
				<< entries[f].source_line
				<< " object type ";
		entries[f].print_type(cout);
		if (entries[f].object_type == POintER_TYPE_OBJECT ||
				entries[f].object_type == POintER_TYPE_OBJECTS) {
			cout << " = " << entries[f].extra_type_info;
		}
		cout << " for a total of " << sz << " char" << endl;
		}

	delete [] type_first;
	delete [] type_len;
	delete [] perm;
	delete [] perm_inv;
	delete [] frequency;

	if (f_v) {
		cout << "mem_object_registry::sort_by_location_and_get_frequency" << endl;
	}
}

void mem_object_registry::sort_by_type(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::sort_by_type" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_type before Heapsort" << endl;
	}
	Heapsort(entries, nb_entries_used,
		sizeof(mem_object_registry_entry),
		registry_key_pair_compare_by_type);
	if (f_v) {
		cout << "mem_object_registry::sort_by_type after Heapsort" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_type done" << endl;
	}

}

void mem_object_registry::sort_by_location(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "mem_object_registry::sort_by_location" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_location before Heapsort" << endl;
	}
	Heapsort(entries, nb_entries_used,
		sizeof(mem_object_registry_entry),
		registry_key_pair_compare_by_location);
	if (f_v) {
		cout << "mem_object_registry::sort_by_location after Heapsort" << endl;
	}

	if (f_v) {
		cout << "mem_object_registry::sort_by_location done" << endl;
	}

}

void start_memory_debug()
{
	f_memory_debug = TRUE;
	cout << "memory debugging started" << endl;
}

void stop_memory_debug()
{
	f_memory_debug = FALSE;
	cout << "memory debugging stopped" << endl;
}

static int registry_key_pair_compare_by_size(void *K1v, void *K2v)
{
	int s1, s2, c;
	mem_object_registry_entry *K1, *K2;

	K1 = (mem_object_registry_entry *) K1v;
	K2 = (mem_object_registry_entry *) K2v;
	s1 = K1->size_of();
	s2 = K2->size_of();
	c = s2 - s1;
	return c;
}

static int registry_key_pair_compare_by_type(void *K1v, void *K2v)
{
	int t1, t2, l1, l2, c;
	mem_object_registry_entry *K1, *K2;

	K1 = (mem_object_registry_entry *) K1v;
	K2 = (mem_object_registry_entry *) K2v;
	t1 = K1->object_type;
	t2 = K2->object_type;
	c = t2 - t1;
	if (c) {
		return c;
	}
	//new the two entries have the same type
	if (t1 == POintER_TYPE_OBJECTS || t1 == POintER_TYPE_OBJECT) {
		c = strcmp(K1->extra_type_info, K2->extra_type_info);
		if (c) {
			return c;
		}
	}
	c = strcmp(K1->source_file, K2->source_file);
	if (c) {
		return c;
	}
	l1 = K1->source_line;
	l2 = K2->source_line;
	c = l2 - l1;
	return c;
}

static int registry_key_pair_compare_by_location(void *K1v, void *K2v)
{
	int l1, l2, c;
	mem_object_registry_entry *K1, *K2;

	K1 = (mem_object_registry_entry *) K1v;
	K2 = (mem_object_registry_entry *) K2v;
	c = strcmp(K1->source_file, K2->source_file);
	if (c) {
		return c;
	}
	l1 = K1->source_line;
	l2 = K2->source_line;
	c = l2 - l1;
	return c;
}


