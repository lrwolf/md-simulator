
__kernel void square(__global float* input, __global float* output, const unsigned int count) {
    size_t i = get_global_id(0);
    if(i < count)
        output[i] = input[i] * input[i];
}

__kernel void computeAccelerations(__global float* input, __constant float* constants, __global float* output, const unsigned int particleCount) {
    int i = get_global_id(0);
    if (i < particleCount*particleCount) {
        int index = i * 3;
        int m1 = i/particleCount;
        int m2 = i%particleCount;
        if (m1 != m2) {
            int m1_index = m1 * (particleCount - 1);
            int m2_index = m2 * (particleCount - 1);
            
            float dx = input[m1_index] - input[m2_index];
            float dy = input[m1_index+1] - input[m2_index+1];
            float dz = input[m1_index+2] - input[m2_index+2];
            
            float dx2 = dx * dx;
            float dy2 = dy * dy;
            float dz2 = dz * dz;
            float rSquared = dx2 + dy2 + dz2;
  
            float epsilon = constants[0];
            float sigma = constants[1];
            
            float rSquaredInverse = sigma / rSquared;
            float attract = rSquaredInverse * rSquaredInverse * rSquaredInverse;
            float repel = attract * attract;
            
            float fOverR = 24.0 * epsilon * ((2.0 * repel) - attract) * rSquaredInverse;

            output[index] = fOverR * dx;
            output[index+1] = fOverR * dy;
            output[index+2] = fOverR * dz;
        } else {
            output[index] = 0.0;
            output[index+1] = 0.0;
            output[index+2] = 0.0;
        }

        barrier(CLK_GLOBAL_MEM_FENCE);
        if (get_local_id(0) == 0) {
            for (int i = index+3; i < index+(3*particleCount)-1; i +=3) {
                output[index] += output[i];
                output[index+1] += output[i+1];
                output[index+2] += output[i+2];
            }
        }
    }
}
