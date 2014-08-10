//
//  Sequential.cpp
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#include <vector>
#include "Sequential.h"

Sequential::Sequential() : Simulator() {
    forceCutoffMinusHalf = forceCutoff - 0.5;
    negForceCutoffMinusHalf = 0.0 - forceCutoffMinusHalf;
}

Sequential::Sequential(int cubeSide) : Simulator(cubeSide) {
    forceCutoffMinusHalf = forceCutoff - 0.5;
    negForceCutoffMinusHalf = 0.0 - forceCutoffMinusHalf;
}

int Sequential::setup() {
    Simulator::setup();
    return MD_SUCCESS;
}

int Sequential::run() {
    long long counter = 0;
    while (true) {
        // Zero out the energy.
        energy = 0.0;
                
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updatePosition(timestep);
        }

        computeAccelerations();
        
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updateVelocityHalf(timestep);
        }

        if (counter%10 == 0) {
            if (CHECK_SPRING_CONSTANT) {
                double dx = molecules[0]->position[0];
                double dy = molecules[0]->position[1];
                double dz = molecules[0]->position[2];
                double distanceFromOrigin = dx * dx + dy * dy + dz * dz;
                if (distanceFromOrigin < 4) {
                    molecules[0]->distanceFromOrigin();
                }
            }
            if (MOLECULE_DEBUG) {
                molecules[0]->distanceFromOrigin();
                molecules[0]->printPosition();
                molecules[0]->printVelocity();
                molecules[0]->printAcceleration();
                std::cout << std::endl << std::endl;
            }
        }
        
        if (fPositions.is_open()) {
            fPositions << molecules[0]->position[0] << "\t" << molecules[0]->position[1] << "\t" << molecules[0]->position[2] << "\n";
        }
        if (fEnergy.is_open()) {
            fEnergy << counter << "\t" << energy << "\n";
        }
        
        counter++;
    }
    
    return MD_SUCCESS;
}


/**
 * Acceleration computations
 */
void Sequential::computeAccelerations() {
    for (int i = 0; i < particleCount; i++) {
        Molecule* m = molecules[i].get();
        // zero out accelerations
        m->setAcceleration(0.0, 0.0, 0.0);
    }
    
    for (int i = 0; i < particleCount; i++) {
        Molecule* m1 = molecules[i].get();
        for (int j = i+1; j < particleCount; j++) {
            Molecule* m2 = molecules[j].get();
            addPairwiseForce(m1, m2);
        }
    }
    
    for (int i = 0; i < particleCount; i++) {
        Molecule* m = molecules[i].get();
        addSpringForce(m);
    }
}

void Sequential::addSpringForce(Molecule* m) {
    for (int i = 0; i < 3; i++) {
        double delta = 0.0;
        if (m->position[i] < negForceCutoffMinusHalf) {
            delta = negForceCutoffMinusHalf - m->position[i];
        } else if (m->position[i] > forceCutoffMinusHalf) {
            delta = forceCutoffMinusHalf - m->position[i];
        }
        
        if (delta != 0.0) {
            m->acceleration[i] += wallStiffness * delta;
        }
        springRadius[i] = delta;
    }
    
    energy += 0.5 * wallStiffness * (springRadius[0] * springRadius[0] + springRadius[1] * springRadius[1] + springRadius[2] * springRadius[2]);
}

void Sequential::addPairwiseForce(Molecule* m1, Molecule* m2) {
    double dx = m1->position[0] - m2->position[0];
    double dy = m1->position[1] - m2->position[1];
    double dz = m1->position[2] - m2->position[2];
    
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double dz2 = dz * dz;
    double rSquared = dx2 + dy2 + dz2;

    double rSquaredInverse = sigma / rSquared;
    double attract = rSquaredInverse * rSquaredInverse * rSquaredInverse;
    double repel = attract * attract;
    
    double fOverR = 24.0 * epsilon * ((2.0 * repel) - attract) * rSquaredInverse;
    double fx = fOverR * dx;
    double fy = fOverR * dy;
    double fz = fOverR * dz;
    
    m1->updateAcceleration(fx, fy, fz);
    m2->updateAcceleration(0.0 - fx, 0.0 - fy, 0.0 - fz);
    
    // Energy calculation
    energy += 4.0 * epsilon * (pow(sigma * rSquaredInverse, 6.0) - pow(sigma * rSquaredInverse, 3.0));
}
