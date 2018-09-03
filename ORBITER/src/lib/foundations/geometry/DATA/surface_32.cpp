int surface_32_nb_reps = 11;
int surface_32_size = 20;
int surface_32_reps[] = {
	0, 0, 0, 0, 0, 0, 1, 0, 6, 0, 0, 6, 0, 8, 0, 0, 9, 7, 7, 0, 
	0, 0, 0, 0, 0, 0, 1, 0, 18, 0, 0, 4, 0, 3, 0, 0, 26, 29, 23, 4, 
	0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 5, 0, 20, 0, 0, 1, 16, 10, 10, 
	0, 0, 0, 0, 0, 0, 1, 0, 10, 0, 0, 12, 0, 10, 0, 0, 28, 26, 20, 31, 
	0, 0, 0, 0, 0, 0, 1, 0, 20, 0, 0, 22, 0, 26, 0, 0, 5, 9, 12, 25, 
	0, 0, 0, 0, 0, 0, 1, 0, 14, 0, 0, 20, 0, 17, 0, 0, 22, 19, 6, 9, 
	0, 0, 0, 0, 0, 0, 1, 0, 16, 0, 0, 3, 0, 9, 0, 0, 17, 27, 30, 15, 
	0, 0, 0, 0, 0, 0, 1, 0, 26, 0, 0, 22, 0, 28, 0, 0, 16, 26, 22, 13, 
	0, 0, 0, 0, 0, 0, 1, 0, 12, 0, 0, 2, 0, 29, 0, 0, 18, 13, 15, 2, 
	0, 0, 0, 0, 0, 0, 1, 0, 11, 0, 0, 31, 0, 26, 0, 0, 19, 22, 2, 8, 
	0, 0, 0, 0, 0, 0, 1, 0, 25, 0, 0, 28, 0, 31, 0, 0, 5, 6, 27, 3, 
};
const char *surface_32_stab_order[] = {
	"192",
	"2",
	"6",
	"6",
	"2",
	"2",
	"6",
	"80",
	"5",
	"16",
	"6",
};
int surface_32_nb_E[] = { 
	13, 1, 3, 3, 1, 1, 3, 5, 0, 5, 
	3};
int surface_32_single_six[] = { 
	0, 1024, 1082400, 2082, 30638, 37065, 
	0, 1024, 1082400, 2082, 30638, 37135, 
	0, 1024, 1082400, 2082, 30638, 37197, 
	0, 1024, 1082400, 2082, 30638, 37275, 
	0, 1024, 1082400, 2082, 30638, 37310, 
	0, 1024, 1082400, 2082, 30638, 37399, 
	0, 1024, 1082400, 2082, 30638, 37730, 
	0, 1024, 1082400, 2082, 30638, 39257, 
	0, 1024, 1082400, 2082, 30638, 39701, 
	0, 1024, 1082400, 2082, 30638, 40408, 
	0, 1024, 1082400, 2082, 25353, 39436, 
};
int surface_32_Lines[] = { 
	0, 1, 1024, 1082400, 1083424, 2082, 37065, 30638, 504091, 25363, 453816, 744544, 1089, 19936, 30172, 10116, 25207, 14846, 745165, 1083184, 382635, 915169, 915356, 503708, 504174, 1039319, 1040064, 
	0, 1024, 1079, 1082400, 1083424, 2082, 30638, 37135, 697950, 947488, 107848, 29591, 9758, 25156, 15630, 7623, 26008, 33418, 948107, 97214, 577120, 278439, 1083190, 533464, 165925, 762290, 590798, 
	0, 1024, 2137, 1082400, 1083424, 2082, 314226, 30638, 37197, 1015296, 3166, 834490, 24493, 15526, 8103, 1015766, 28250, 5445, 21920, 1082406, 885669, 1051701, 650695, 704410, 571818, 407978, 1006393, 
	0, 1024, 24330, 1082400, 1083424, 2082, 30638, 37275, 383823, 1077600, 845664, 19021, 24036, 27129, 17680, 7782, 11979, 31368, 846632, 383723, 534489, 915358, 165938, 1033720, 457552, 1083378, 306851, 
	0, 1024, 23271, 1082400, 1083424, 2082, 30638, 37310, 17964, 710528, 1012341, 753743, 15559, 13505, 711340, 14541, 8788, 10890, 20819, 402688, 115661, 1036213, 610919, 356193, 78506, 1082515, 1069011, 
	0, 1024, 22202, 1082400, 1083424, 2082, 30638, 812608, 37399, 628089, 26420, 72250, 16335, 12523, 812820, 32215, 18878, 21716, 29124, 190804, 69751, 364637, 354223, 553675, 1083408, 470339, 1065905, 
	0, 1024, 5304, 1082400, 1083424, 2082, 30638, 727008, 69927, 37730, 11622, 136032, 19238, 30282, 12944, 4420, 11883, 22869, 136323, 214566, 335044, 644749, 1037237, 275204, 857803, 535483, 1083206, 
	0, 1024, 7413, 1082400, 1083424, 2082, 30638, 39257, 7389, 540089, 102464, 489126, 1089, 13257, 32254, 8514, 18944, 102507, 21043, 379443, 949184, 1082563, 740933, 539614, 272001, 768506, 1069011, 
	0, 1024, 1063, 1082400, 1083424, 2082, 30638, 431766, 384582, 643488, 39701, 6332, 19786, 29852, 25047, 12944, 26649, 643705, 28791, 306500, 737113, 1083164, 861041, 233566, 479867, 267228, 1073175, 
	0, 1024, 6356, 1082400, 1083424, 2082, 710784, 30638, 40408, 244827, 12665, 1014648, 1089, 20018, 4298, 3135, 711334, 32831, 28581, 655331, 1082938, 1013727, 571816, 763282, 349965, 643809, 881515, 
	0, 1024, 5299, 1082400, 1083424, 2082, 25353, 39436, 1014180, 348843, 913312, 22187, 1569, 914288, 4996, 16335, 11356, 22714, 3668, 869055, 557583, 1082777, 766309, 847690, 579908, 373441, 566544, 
};
int surface_32_make_element_size = 17;
int surface_32_stab_gens_fst[] = { 0, 5, 6, 8, 10, 11, 12, 14, 17, 18, 
	22};
int surface_32_stab_gens_len[] = { 5, 1, 2, 2, 1, 1, 2, 3, 1, 4, 
	2};
int surface_32_stab_gens[] = {
	 9,  0,  0,  0, 27,  9,  0,  0, 27,  0,  9,  0, 19, 18, 18,  9, 0, 
	 8,  0,  0,  0, 16,  8,  0,  0, 16,  0,  8,  0, 29, 24, 24,  8, 0, 
	19,  0,  0,  0,  5, 19,  0,  0,  5,  0, 19,  0, 31, 19, 19, 19, 0, 
	13,  0,  0,  0, 17,  0, 13,  0, 17, 13,  0,  0, 12, 26, 26, 13, 0, 
	 2,  0, 29, 28,  8,  0,  0,  1,  8,  2,  0,  0, 28,  4,  0,  0, 0, 
	18, 31, 28, 23, 19,  4, 22, 17, 16, 19,  9, 22,  5,  3, 27, 31, 0, 
	 0, 14,  3,  1, 18,  0, 19, 18,  0,  2, 10,  5,  4,  0, 30, 15, 0, 
	20, 28,  0,  0, 28, 20,  0,  0, 14, 14,  8,  0, 29, 29,  0,  8, 0, 
	 0,  0, 15, 15, 11, 11, 15, 15,  0,  0, 11,  0,  1,  0, 11,  0, 0, 
	 5, 31,  9,  7, 15,  4,  6,  2, 21, 16,  2,  1, 15,  5,  6,  3, 0, 
	 3, 21, 31, 22,  5, 26,  7, 30, 10,  6, 23, 25, 20, 12, 28, 14, 0, 
	 1, 24,  1, 28,  3, 13, 17, 15, 31, 22, 24, 12, 27,  9, 13, 20, 0, 
	16, 17,  6,  2, 21, 22, 28, 23, 31, 22, 12,  6, 27,  9, 20, 10, 0, 
	 6, 20, 31, 22, 14,  6, 26, 21, 12,  4, 27, 31, 19, 18,  8,  4, 0, 
	28,  0,  0,  0,  1,  0, 30,  0, 12,  4,  0,  0, 24,  8, 29, 28, 0, 
	 0,  8, 27, 27, 31,  0,  3,  3,  0, 19, 10, 25, 21,  0, 28,  9, 2, 
	29,  8, 29,  8, 23, 24, 23, 24, 25, 30, 22, 17, 18, 25,  4,  7, 4, 
	 7,  7,  0,  0, 17,  8,  0,  0, 21, 21, 20, 20, 12, 25, 14,  0, 4, 
	20,  0,  0,  0, 25,  0, 26,  0, 15,  5,  0,  0, 30, 10, 13, 20, 0, 
	13, 12, 13, 12, 16, 18,  6,  1, 30, 12, 30, 12,  8, 18,  3,  1, 0, 
	13, 16, 20, 15,  6, 31,  8, 27, 30,  2, 15, 16,  3, 28,  4, 29, 0, 
	29,  9, 28, 28, 25, 13, 26, 26, 18, 18,  9, 29, 30, 30, 13, 25, 0, 
	28, 13, 28, 14,  0,  0,  3, 19,  0, 13,  0, 14,  0,  0,  0, 28, 0, 
	 0,  0,  5,  8,  0,  0,  9, 22, 21, 21, 29, 21, 11, 21,  6,  3, 0, 
};
