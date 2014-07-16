//
//  Sequential.cpp
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#include "Sequential.h"

Sequential::Sequential() {}

int Sequential::setup() {
    for (int i = 0; i < cubeSide; i++) {
        for (int j = 0; j < cubeSide; j++) {
            for (int k = 0; k < cubeSide; k++) {
                // Set initial particle position
                molecules.emplace_back(new Molecule(i*latticeOffset, j*latticeOffset, k*latticeOffset));
            }
        }
    }
    return MD_SUCCESS;
}

int Sequential::run() {
    for (int j = 0; j < 40000; j++) {
        // Single-step
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updatePosition(timestep);
        }
        
        computeAccelerations();
        
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updateVelocityHalf(timestep);
        }
        
        if (j%40 == 0) {
            // molecules[0]->printPosition();
            molecules[0]->distanceFromOrigin();
            //  molecules[0]->printVelocity();
            //  molecules[0]->printAcceleration();
            //  std::cout << "-------------------------" << std::endl;
            //  molecules[7]->printPosition();
            //  molecules[7]->printVelocity();
            //  molecules[7]->printAcceleration();
            //  std::cout << "-------------------------" << std::endl;
        }
    }
    
    return MD_SUCCESS;
}


/**
 * Acceleration computations
 */
void Sequential::computeAccelerations() {
    for (int i = 0; i < particleCount; i++) {
        Molecule* m = molecules[i].get();
        addSpringForce(m);
    }
    
    for (int i = 0; i < particleCount; i++) {
        Molecule* m1 = molecules[i].get();
        for (int j = i+1; j < particleCount; j++) {
            Molecule* m2 = molecules[j].get();
            positionDifference(m1, m2);
        }
    }
}

void Sequential::addSpringForce(Molecule* m) {
    for (int i = 0; i < 3; i++) {
        double dimensionMinusHalf = dimensions[i] - 0.5;
        if (m->position[i] < 0.5) {
            m->acceleration[i] = wallStiffness * (0.5 - m->position[i]);
        } else if (m->position[i] > dimensionMinusHalf) {
            m->acceleration[i] = wallStiffness * (dimensionMinusHalf - m->position[i]);
        } else {
            m->acceleration[i] = 0.0;
        }
    }
}

void Sequential::positionDifference(Molecule* m1, Molecule* m2) {
    double dx = m1->position[0] - m2->position[0];
    double dy = m1->position[1] - m2->position[1];
    double dz = m1->position[2] - m2->position[2];
    
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double dz2 = dz * dz;
    
    if (dx2 >= forceCutoff2 || dy2 >= forceCutoff2 || dz2 >= forceCutoff2) {
        return;
    }
    
    double rSquared = dx2 + dy2 + dz2;
    double rSquaredInverse = 1.0 / rSquared;    //  sigma / rSquared
    double attract = rSquaredInverse * rSquaredInverse * rSquaredInverse;
    double repel = attract * attract;
    
    double fOverR = 24.0 * ((2.0 * repel) - attract) * rSquaredInverse; // * epsilon
    double fx = fOverR * dx;
    double fy = fOverR * dy;
    double fz = fOverR * dz;
    
    m1->updateAcceleration(fx, fy, fz);
    m2->updateAcceleration(0.0 - fx, 0.0 - fy, 0.0 - fz);
}
