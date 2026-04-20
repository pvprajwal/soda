__kernel void ca_1d(__global const int *curr, __global int *next, const int rule, const int N)              
{
	int id = get_global_id(0);

	int left = (id == 0) ? 0 : curr[id - 1];
	int center = curr[id];
	int right = (id == N - 1) ? 0 : curr[id + 1];

	int pattern = (left << 2) | (center << 1) | right;
	next[id] = (rule >> pattern) & 1;
}
