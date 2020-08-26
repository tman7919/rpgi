#ifndef RANDOM_CYCLE_ARRAY_H
#define RANDOM_CYCLE_ARRAY_H
namespace tmn{
	const static int random_cycle[256] = {
		3, 147, 2, 148, 46, 52, 72, 28, 61, 45, 10, 135, 67, 40, 170, 230, 39,
		181, 96, 253, 220, 44, 222, 35, 24, 106, 126, 150, 9, 7, 88, 144, 55,
		33, 226, 189, 14, 247, 11, 102, 83, 232, 178, 221, 218, 31, 190, 114, 86,
		194, 154, 255, 38, 123, 231, 81, 85, 210, 110, 59, 235, 30, 99, 251, 109,
		103, 201, 206, 174, 70, 142, 71, 228, 129, 74, 89, 43, 203, 121, 250, 80,
		95, 49, 53, 50, 65, 56, 111, 168, 113, 227, 140, 36, 115, 186, 157, 133,
		241, 63, 224, 82, 78, 79, 151, 104, 229, 125, 172, 98, 21, 41, 196, 193,
		197, 94, 92, 116, 162, 118, 225, 215, 243, 77, 165, 108, 149, 17, 130, 25,
		132, 237, 66, 184, 136, 180, 32, 158, 93, 138, 107, 112, 153, 160, 155, 177,
		205, 146, 200, 0, 159, 101, 12, 252, 191, 143, 145, 100, 134, 119, 242, 105,
		161, 15, 198, 42, 216, 171, 219, 176, 240, 137, 169, 91, 173, 48, 23, 167,
		18, 20, 26, 166, 239, 19, 87, 127, 60, 62, 187, 131, 117, 6, 37, 4,
		139, 163, 254, 195, 185, 156, 199, 1, 152, 202, 84, 27, 249, 122, 34, 208,
		233, 223, 211, 179, 16, 183, 124, 13, 234, 245, 192, 51, 22, 141, 29, 47,
		97, 5, 217, 175, 58, 90, 209, 188, 182, 69, 75, 64, 73, 238, 212, 57,
		207, 128, 204, 244, 236, 246, 164, 213, 76, 248, 120, 68, 54, 214, 8
	};
	
	struct rand{
		static int count;
		int operator()(){
			int ret = random_cycle[count];
			count = (count + 1) % 256;
			return ret;
		}
	};
}


#endif

