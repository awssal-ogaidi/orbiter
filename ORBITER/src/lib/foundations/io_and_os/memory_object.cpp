// memory_object.C
//
// Anton Betten
// October 6, 2013
//
//
// originally started April 4, 2000
// moved from D2 to ORBI Nov 15, 2007

#include "foundations.h"



//
// 
// alloc_length: allocated length in chars
// used_length: used length in charS
// cur_pointer:
//          0 <= pointer < used length. 
//



memory_object::memory_object()
{
	char_pointer = NULL;
	alloc_length = 0;
	used_length = 0;
	cur_pointer = 0;
}

memory_object::~memory_object()
{
	freeself();
}

void memory_object::null()
{
	char_pointer = NULL;
}

void memory_object::freeself()
{
	if (char_pointer) {
		FREE_char(char_pointer);
		}
	null();
}

void memory_object::init(int length,
		char *d, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;
	
	if (f_v) {
		cout << "memory_object::init" << endl;
		}
	alloc(length, verbose_level - 1);
	for (i = 0; i < length; i++) {
		s_i(i) = d[i];
		}
	if (f_v) {
		cout << "memory_object::init done" << endl;
		}
}

void memory_object::alloc(int length, int verbose_level)
// sets alloc_length to length
// sets used_length to length, 
// sets cur_pointer to 0.
{
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "memory_object::alloc "
				"length=" << length << endl;
		}
	freeself();

	char_pointer = NEW_char(length);
	if (char_pointer == NULL) {
		cout << "memory_object::alloc "
				"out of memory" << endl;
		exit(1);
		}
	alloc_length = length;
	used_length = length;
	cur_pointer = 0;

	if (f_v) {
		cout << "memory_object::alloc done" << endl;
		}
}

void memory_object::append(int length,
		char *d, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, old_length, new_length, new_alloc_length;
	
	if (f_v) {
		cout << "memory_object::append" << endl;
		cout << "used_length=" << endl;
		cout << "alloc_length=" << alloc_length << endl;
		}
	old_length = used_length;
	new_length = old_length + length;
	if (new_length > alloc_length) {
		if (f_v) {
			cout << "memory_object::append before realloc" << endl;
		}
		new_alloc_length = MAXIMUM(new_length, 2 * alloc_length);
		realloc(new_alloc_length, verbose_level);
		if (f_v) {
			cout << "memory_object::append after realloc" << endl;
		}
		used_length = new_length;
		alloc_length = new_alloc_length;
		}
	else {
		used_length = new_length;
		}
	for (i = 0; i < length; i++) {
		char_pointer[old_length + i] = d[i];
		}
	if (f_v) {
		cout << "memory_object::append done" << endl;
		}
}

void memory_object::realloc(int new_length, int verbose_level)
{
	int f_v = TRUE; //(verbose_level >= 1);
	int old_length;
	int old_cur_pointer;
	int i;
	char *old_pc;
	
	if (f_v) {
		cout << "memory_object::realloc "
				"old_length=" << used_length <<
				"new_length=" << new_length << endl;
		}
	old_pc = char_pointer;
	old_length = used_length;
	old_cur_pointer = cur_pointer;
	if (new_length < old_length) {
		cout << "memory_object::realloc warning: "
				"new_length < old_length" << endl;
		}
	char_pointer = NULL;
	alloc(new_length, verbose_level - 1);
	for (i = 0; 
		i < MINIMUM(old_length, new_length); 
		i++) {
		char_pointer[i] = old_pc[i];
		}
	for (i = old_length; i < new_length; i++) {
		char_pointer[i] = 0;
		}
	FREE_char(old_pc);
	cur_pointer = old_cur_pointer;
	if (f_v) {
		cout << "memory_object::realloc done" << endl;
		}
}

void memory_object::write_char(char c)
{	
	append(1, &c, 0);
}

void memory_object::read_char(char *c)
{
	int l1, cur_p, l;
	char *cp;
	
	cur_p = cur_pointer;
	l = used_length;
	l1 = l - cur_p;
	if (l1 < 1) {
		cout << "memory_object::read_char "
				"error: l1 < 1" << endl;
		exit(1);
		}
	cp = char_pointer + cur_p;
	*c = *cp;
	cur_pointer++;
}

void memory_object::write_string(const char *p)
{	
	int l, i;

	l = strlen(p);
	for (i = 0; i < l; i++) {
		write_char(p[i]);
		}
	write_char(0);
}

void memory_object::read_string(char *&p)
{	
	char *q;
	char c;
	int alloc_length;
	int used_length;
	int i;

	alloc_length = 1024;
	used_length = 0;
	q = NEW_char(alloc_length);

	while (TRUE) {
		read_char(&c);
		if (used_length == alloc_length) {
			int new_alloc_length = 2 * alloc_length;
			char *q1;

			q1 = NEW_char(new_alloc_length);
			for (i = 0; i < used_length; i++) {
				q1[i] = q[i];
				}
			FREE_char(q);
			q = q1;
			alloc_length = new_alloc_length;
			}
		q[used_length++] = c;
		if (c == 0) {
			break;
			}
		}
	// now used_length = strlen(q) + 1

	// copy the result over. This gets rid of the overhead:
	p = NEW_char(used_length);
	strcpy(p, q);

	FREE_char(q);
}

void memory_object::write_double(double f)
{
	append(sizeof(double), (char *) &f, 0);
}

void memory_object::read_double(double *f)
{
	double f1;
	int l1, j, cur_p, l;
	char *cp, *cp1;
	
	cur_p = cur_pointer;
	l = used_length;
	l1 = l - cur_p;
	if (l1 < (int)sizeof(double)) {
		cout << "memory_object::read_int "
				"error: l1 < sizeof(double)" << endl;
		exit(1);
		}
	cp = char_pointer + cur_p;
	cp1 = (char *) &f1;
	for (j = 0; j < (int)sizeof(double); j++) {
		*cp1 = *cp;
		cp1++;
		cp++;
		}
	cur_pointer += sizeof(double);
	*f = f1;
}

void memory_object::write_int64(int i)
{
	int8 i1 = (int8) i;
	
	block_swap_chars((char *) &i1, 8, 1);
	append(8, (char *) &i1, 0);
}

void memory_object::read_int64(int *i)
{
	int8 i1;
	int l1, j, cur_p, l;
	char *cp, *cp1;
	
	cur_p = cur_pointer;
	l = used_length;
	l1 = l - cur_p;
	if (l1 < 8) {
		cout << "memory_object::read_int "
				"error: l1 < 8" << endl;
		exit(1);
		}
	cp = char_pointer + cur_p;
	cp1 = (char *) &i1;
	for (j = 0; j < 8; j++) {
		*cp1 = *cp;
		cp1++;
		cp++;
		}
	block_swap_chars((char *) &i1, 8, 1);
	cur_pointer += 8;
	*i = (int) i1;
}

void memory_object::write_int(int i)
{
	int4 i1 = (int4) i;
	
	block_swap_chars((char *) &i1, 4, 1);
	append(4, (char *) &i1, 0);
}

void memory_object::read_int(int *i)
{
	int f_v = FALSE;
	int4 i1;
	int l1, j, cur_p, l;
	char *cp, *cp1;
	
	if (f_v) {
		cout << "memory_object::read_int" << endl;
		}
	cur_p = cur_pointer;
	l = used_length;
	l1 = l - cur_p;
	if (l1 < 4) {
		cout << "memory_object::read_int "
				"error: l1 < 4" << endl;
		exit(1);
		}
	cp = char_pointer + cur_p;
	cp1 = (char *) &i1;
	for (j = 0; j < 4; j++) {
		*cp1 = *cp;
		cp1++;
		cp++;
		}
	if (f_v) {
		cout << "memory_object::read_int before swap: i1 = " << i1 << endl;
		}
	block_swap_chars((char *) &i1, 4, 1);
	if (f_v) {
		cout << "memory_object::read_int after swap: i1 = " << i1 << endl;
		}
	cur_pointer += 4;
	if (f_v) {
		cout << "memory_object::read_int "
				"done read " << i1 << endl;
		}
	*i = (int) i1;
}

#include <cstdio>

void memory_object::read_file(const char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	FILE *fp;
	int fsize;

	if (f_v) {
		cout << "memory_object::read_file" << endl;
		}
	fsize = file_size(fname);
	alloc(fsize, 0);
	fp = fopen(fname, "r");
	if ((int) fread(char_pointer,
			1 /* size */, fsize /* nitems */, fp) != fsize) {
		cout << "memory_object::read_file "
				"error in fread" << endl;
		}
	fclose(fp);
	used_length = fsize;
	cur_pointer = 0;
	if (f_v) {
		cout << "memory_object::read_file read file " 
			<< fname << " of size " << fsize << endl;
		}
}

void memory_object::write_file(const char *fname, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	FILE *fp;
	int size;

	if (f_v) {
		cout << "memory_object::write_file" << endl;
		}
	size = used_length;
	
	fp = fopen(fname, "wb");

	fwrite(char_pointer, 1 /* size */, size /* items */, fp);
	
	fclose(fp);
	if (file_size(fname) != size) {
		cout << "memory_object::write_file error "
				"file_size(fname) != size" << endl;
		exit(1);
		}
	if (f_v) {
		cout << "memory_object::write_file written file " 
			<< fname << " of size " << size << endl;
		}
}

int memory_object::multiplicity_of_character(char c)
{
	int i, l;
	
	l = 0;
	for (i = 0; i < used_length; i++) {
		if (char_pointer[i] == c) {
			l++;
			}
		}
	return l;
}

#if 0
static void code(uchar *pc, int l, uchar *pc2, uchar code_char);
static int decode(uchar *pc2, int l2, uchar *pc, uchar code_char);

static void code(uchar *pc, int l, uchar *pc2, uchar code_char)
/* Wolfgang Boessenecker 940919 */
{
	uchar cc;
	int pos = 0, pos2 = 0, pos2h = 0, i;

	while (pos < l) {
		pos2++;
		cc = 0;
#if 0
		if ((posf % 100000) == 0) {
			cout << posf << endl;
			}
#endif
		for (i = 0; i < 8; i++) {
			cc <<= 1;
			if (pos < l) {
				if (pc[pos] == code_char)
					cc = cc | 0X1U;
				else {
					pc2[pos2] = pc[pos];
					pos2++;
					}
				pos++;
				}
			}
		pc2[pos2h] = cc;
		pos2h = pos2;
		}
}

static int decode(uchar *pc2, int l2, uchar *pc, uchar code_char)
// returns the length of the data after decompression
// pc may be NULL 
{
	uchar cc = 0;
	int pos = 0, pos2 = 0, i = 8;
	
	while (TRUE) {
	/* for (; pos2 < l2; ) { */
		if (pos2 >= l2 && i >= 8)
			break;
		if (i == 8) {
			cc = pc2[pos2];
			pos2++;
			i = 0;
			}
		if (cc & (uchar) 128U) {
			if (pc) {
				pc[pos] = code_char;
				}
			pos++;
			}
		else {
			if (pos2 < l2) {
				if (pc) {
					pc[pos] = pc2[pos2];
					}
				pos2++;
				pos++;
				}
			}
		cc <<= 1;
		i++;
		}
	return pos;
}

void memory_object::compress(int verbose_level)
// Wolfgang Boessenecker 9/94 
{
	int f_v = (verbose_level >= 1);
	memory_object mem2;
	int l, l2, l_c;

	l = used_length;
	if (f_v) {
		cout << "memory_object::compress compressing " << l << " chars";
		}
	l_c = multiplicity_of_character((char) 0);
	l2 = l - l_c + ((l + 7) >> 3);
	mem2.alloc(l2, 0); // sets used_length to l2 
	code((uchar *) char_pointer, l, (uchar *) mem2.char_pointer, (uchar) 0);
#if 0
	if (l3 != l2) {
		cout << "memory_object::compress "
				"warning: l2 = " << l2 << " != l3 = " << l3 << endl;
		}
#endif
	freeself();
	char_pointer = mem2.char_pointer;
	cur_pointer = 0;
	used_length = mem2.used_length;
	alloc_length = mem2.alloc_length;
	mem2.null();
	if (f_v) {
		cout << "memory_object::compress "
				"compressed from " << l << " to " << l2 << " chars." << endl;
		}
}

void memory_object::decompress(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	memory_object mem2;
	int l, l2;
	
	l2 = used_length;
	if (f_v) {
		cout << "memory_object::decompress "
				"decompressing from " << l2 << " chars";
		}
	l = decode((uchar *) char_pointer, l2, NULL, (uchar) 0);
	mem2.alloc(l, 0);
	decode((uchar *) char_pointer, l2, (uchar *) mem2.char_pointer, (uchar) 0);
	freeself();
	char_pointer = mem2.char_pointer;
	cur_pointer = 0;
	used_length = mem2.used_length;
	alloc_length = mem2.alloc_length;
	mem2.null();

	if (f_v) {
		cout << "memory_object::decompress "
				"decompressing from " << l2 << " to ";
		cout << l << " chars." << endl;
		}
}
#endif



