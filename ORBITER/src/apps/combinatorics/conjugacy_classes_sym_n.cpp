// conjugacy_classes_sym_n.C
//
// Anton Betten
// Dec 17, 2015

#include "orbiter.h"


void make_partitions(int n, int *Part, int cnt);
int count_partitions(int n);
int next_partition(int n, int *part);

int main(int argc, char **argv)
{
	int i;
	int verbose_level = 0;
	int f_n = FALSE;
	int n;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = atoi(argv[++i]);
			cout << "-n " << n << endl;
			}
		}
	
	if (!f_n) {
		cout << "please specify -n <n>" << endl;
		exit(1);
		}

	int cnt;
	longinteger_object class_size, S, F, A;
	longinteger_domain D;
	
	cnt = count_partitions(n);

	int *Parts;

	Parts = NEW_int(cnt * n);
	make_partitions(n, Parts, cnt);
	

	S.create(0);
	
	cout << "The conjugacy classes in Sym_" << n << " are:" << endl;
	for (i = 0; i < cnt; i++) {
		cout << i << " : ";
		int_vec_print(cout, Parts + i * n, n);
		cout << " : ";

		D.size_of_conjugacy_class_in_sym_n(class_size, n, Parts + i * n);
		cout << class_size << " : ";
		cout << endl;

		D.add_in_place(S, class_size);
		}

	D.factorial(F, n);
	D.integral_division_exact(F, S, A);
	if (!A.is_one()) {
		cout << "the class sizes do not add up" << endl;
		exit(1);
		}
	cout << "The sum of the class sizes is n!" << endl;
	
}

void make_partitions(int n, int *Part, int cnt)
{
	int *part;
	int cnt1;

	cnt1 = 0;


	part = NEW_int(n + 1);
	
	int_vec_zero(part, n + 1);
	part[n] = 1;
	int_vec_copy(part + 1, Part + cnt1 * n, n);

	cnt1 = 1;
	while (TRUE) {


	
		if (!next_partition(n, part))
			break;
		int_vec_copy(part + 1, Part + cnt1 * n, n);
		cnt1++;
		}
	if (cnt1 != cnt) {
		cout << "make_partitions cnt1 != cnt" << endl;
		exit(1);
		}
}

int count_partitions(int n)
{
	int cnt;
	int *part;

	cnt = 0;


	part = NEW_int(n + 1);
	
	int_vec_zero(part, n + 1);
	part[n] = 1;


	cnt = 1;
	
	while (TRUE) {

		if (!next_partition(n, part))
			break;
		cnt++;
		}

	return cnt;
}

int next_partition(int n, int *part)
{
	int s, i, j, q, r;

	s = part[1];
	for (i = 2; i <= n; i++) {
		if (part[i]) {
			s += i;
			part[i]--;
			break;
			}
		}
	if (i == n + 1) {
		return FALSE;
		}
	for (j = i - 1; j >= 1; j--) {
		q = s / j;
		r = s - q * j;
		part[j] = q;
		s = r;
		}
	return TRUE;
}



