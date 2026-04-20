__kernel void gray_scott(__global const float *u_curr,
                         __global const float *v_curr, __global float *u_next,
                         __global float *v_next, const int W, const int H,
                         const float Du, const float Dv, const float F,
                         const float k, const float dt) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  if (x >= W || y >= H)
    return;

  int i = y * W + x;

  // Toroidal neighbours
  int L = y * W + (x + W - 1) % W, R = y * W + (x + 1) % W;
  int U = ((y + H - 1) % H) * W + x, D = ((y + 1) % H) * W + x;

  // 5-point discrete Laplacian (approximate)
  float lap_u =
      u_curr[L] + u_curr[R] + u_curr[U] + u_curr[D] - 4.0f * u_curr[i];
  float lap_v =
      v_curr[L] + v_curr[R] + v_curr[U] + v_curr[D] - 4.0f * v_curr[i];

  float u = u_curr[i];
  float v = v_curr[i];
  float uvv = u * v * v; // reaction term

  // euler integration step
  u_next[i] = u + dt * (Du * lap_u - uvv + F * (1.0f - u));
  v_next[i] = v + dt * (Dv * lap_v + uvv - (F + k) * v);
}