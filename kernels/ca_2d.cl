__kernel void Game_of_Life(__global int *A, __global int *B, int N) {
    int i = get_global_id(0);
    int x = i % N;
    int y = i / N;
    int neighbour_sum = 0;

    // Check all 8 neighbors with boundary protection
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
            continue; // Skip the cell itself
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
            neighbour_sum += A[ny * N + nx];
        }
        }
    }

    if (neighbour_sum < 2 || neighbour_sum > 3) {
        B[i] = 0;
    } else if (neighbour_sum == 3) {
        B[i] = 1;
    } else {
        B[i] = A[i];
    }
}
