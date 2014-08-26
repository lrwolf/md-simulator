
__kernel void computeAccelerations(__global float* input, __constant float* constants, __global float* output, const unsigned int particleCount) {
    int i = get_global_id(0);
    if (i < particleCount*particleCount) {
        int index = i * 3;
        int m1 = i/particleCount;
        int m2 = i%particleCount;
        int m1_index = m1 * (particleCount - 1);
        if (m1 != m2) {
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
            
            float fOverR = 24.f * epsilon * ((2.f * repel) - attract) * rSquaredInverse;

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
            // Summation of pairwise forces.
            for (int j = index+3; j < index+(3*particleCount); j +=3) {
                output[index] += output[j];
                output[index+1] += output[j+1];
                output[index+2] += output[j+2];
            }
            
            // Spring-related constants.
            float negForceCutoffMinusHalf = constants[2];
            float forceCutoffMinusHalf = constants[3];
            float wallStiffness = constants[4];
            
            // Add spring force.
            for (int j = 0; j < 3; j++) {
                float delta = 0.f;
                if (input[m1_index+j] < negForceCutoffMinusHalf) {
                    delta = negForceCutoffMinusHalf - input[m1_index+j];
                } else if (input[m1_index+j] > forceCutoffMinusHalf) {
                    delta = forceCutoffMinusHalf - input[m1_index+j];
                }
                
                if (delta != 0.f) {
                    output[index+j] += wallStiffness * delta;
                }
            }
        }
    }
}
