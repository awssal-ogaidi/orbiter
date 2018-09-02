INT surface_16_nb_reps = 5;
INT surface_16_size = 20;
INT surface_16_reps[] = {
	0, 0, 0, 0, 0, 0, 1, 0, 6, 0, 0, 6, 0, 8, 0, 0, 9, 7, 7, 0, 
	0, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 8, 0, 5, 0, 0, 6, 11, 12, 8, 
	0, 0, 0, 0, 0, 0, 1, 0, 7, 0, 0, 12, 0, 11, 0, 0, 5, 2, 9, 15, 
	0, 0, 0, 0, 0, 0, 1, 0, 6, 0, 0, 5, 0, 1, 0, 0, 4, 0, 0, 7, 
	0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 
};
const char *surface_16_stab_order[] = {
	"384",
	"12",
	"64",
	"216",
	"103680",
};
INT surface_16_nb_E[] = { 
	13, 3, 5, 9, 45};
INT surface_16_single_six[] = { 
	0, 256, 69904, 530, 2452, 5225, 
	0, 256, 69904, 530, 2452, 5263, 
	0, 256, 69904, 530, 2452, 5810, 
	0, 256, 69904, 530, 4363, 5939, 
	0, 256, 69904, 530, 2997, 7532, 
};
INT surface_16_Lines[] = { 
	0, 1, 256, 69904, 70160, 530, 5225, 2452, 19811, 4359, 23324, 13248, 289, 3408, 2376, 1840, 4223, 1122, 13369, 69978, 32215, 44113, 44219, 19848, 19924, 43503, 43669, 
	0, 256, 288, 69904, 70160, 530, 2452, 5263, 32521, 52280, 3813, 65696, 3436, 3986, 942, 65789, 770, 2688, 2023, 32503, 6814, 60593, 70120, 63706, 18006, 53186, 25260, 
	0, 256, 1925, 69904, 70160, 530, 2452, 5810, 60469, 43872, 817, 24761, 289, 43949, 4260, 1071, 3163, 2970, 566, 48859, 37995, 65282, 70001, 30913, 26742, 24810, 63598, 
	0, 256, 1647, 69904, 4368, 70160, 4624, 530, 4363, 4743, 5105, 5939, 2182, 3408, 4249, 3709, 3164, 2858, 2601, 7274, 6545, 4902, 6921, 8721, 8109, 6561, 70116, 
	0, 1, 256, 69904, 4368, 70160, 4624, 530, 4658, 3269, 7532, 2997, 7275, 289, 290, 3163, 3164, 2906, 2907, 69921, 4369, 4657, 4898, 7531, 7637, 7274, 7365, 
};
INT surface_16_make_element_size = 17;
INT surface_16_stab_gens_fst[] = { 0, 6, 9, 12, 16};
INT surface_16_stab_gens_len[] = { 6, 3, 3, 4, 12};
INT surface_16_stab_gens[] = {
	 1,  0,  0,  0,  0,  5,  0,  0,  0,  0,  5,  0,  0,  0,  0, 15, 2, 
	11,  0,  0,  0, 11, 11,  0,  0, 11,  0, 11,  0, 11, 11, 11, 11, 0, 
	11,  0,  0,  0,  4, 11,  0,  0,  4,  0, 11,  0,  8, 15, 15, 11, 0, 
	 2,  0,  0,  0, 12,  2,  0,  0, 12,  0,  2,  0, 11,  2,  2,  2, 0, 
	 6,  0,  0,  0,  0,  0,  7,  0,  0,  7,  0,  0,  0,  0,  0,  9, 2, 
	 0,  1,  0, 13,  0,  2,  0, 13,  1,  2,  3,  3,  3,  7,  3,  3, 2, 
	 6,  9,  0,  0,  0, 10,  0,  0, 12,  9, 10,  0,  0, 15,  0,  7, 2, 
	 2,  4,  2,  3,  7,  1, 11, 10,  2,  2, 14,  9,  7,  7, 11, 12, 2, 
	 7, 15,  8,  1, 11,  9,  8,  1,  1, 11, 13,  0,  6, 13, 13,  0, 2, 
	 3,  0,  0,  0,  5,  0,  9,  0,  4, 11,  0,  0,  8, 15,  6,  3, 0, 
	 0,  7,  9, 14, 15,  8,  2,  5,  0, 10,  5,  2,  1, 11, 10, 13, 0, 
	 8,  2,  8,  8,  0,  4,  0,  8,  0,  5,  8,  8,  0,  9,  0,  8, 1, 
	14,  0,  0,  0,  0,  0,  3,  0,  0,  9,  0,  0,  0,  0,  0, 14, 0, 
	 8,  3,  7,  5,  3, 15,  6,  7, 14, 15,  5,  8,  5,  7,  4, 13, 2, 
	 7, 12,  4, 14,  3,  9,  2,  7,  8,  8,  6,  6, 14,  5,  3,  9, 0, 
	 3,  0,  4,  0,  0,  0,  4,  0,  7,  8,  3,  3,  3,  0,  3,  3, 1, 
	 5,  0,  0,  0,  0,  5,  0,  0,  0,  0,  5,  0,  0,  0,  0,  5, 2, 
	 8,  0,  0,  0,  0,  8,  0,  0,  0,  0,  8,  0,  0,  0,  0,  8, 3, 
	13,  0,  0,  0,  0,  2,  0,  0,  0,  0,  2,  0,  0,  0,  0, 13, 1, 
	12,  0,  0,  0,  0,  9,  0,  0,  0,  0,  9,  0,  0,  0,  0, 12, 0, 
	14,  0,  0,  0,  0,  6,  0,  0,  0,  0,  6,  0, 14,  0,  0, 14, 1, 
	 7,  0,  0,  0,  0,  7,  0,  0,  7,  0,  7,  0,  0,  7,  0,  7, 1, 
	15,  0,  0,  0,  0, 13,  0,  0,  0, 13, 13,  0,  0,  0,  0, 15, 3, 
	12,  0,  0,  0,  9,  9,  0,  0,  0,  0,  9,  0,  0,  0, 12, 12, 2, 
	 8,  0,  0,  0,  8,  6,  0,  0,  6,  0,  6,  0,  6,  8,  6,  8, 1, 
	13,  0,  0,  0, 13,  2,  2,  0, 13,  2,  0,  0,  0,  0,  2, 13, 1, 
	 7,  3,  0,  0,  0,  4,  0,  0,  4,  3,  4,  3,  0,  7,  0,  7, 2, 
	 3,  4,  0,  3,  4,  3,  4,  7,  0,  4,  0,  0,  3,  7,  0,  0, 3, 
};
