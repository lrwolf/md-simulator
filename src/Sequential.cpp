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
}

Sequential::Sequential(int cubeSide) : Simulator(cubeSide) {
    forceCutoffMinusHalf = forceCutoff - 0.5;
}

int Sequential::setup() {
    Simulator::setup();
    
    for (int i = 0; i < cubeSide; i++) {
        for (int j = 0; j < cubeSide; j++) {
            for (int k = 0; k < cubeSide; k++) {
                // Set initial particle position
                molecules.emplace_back(new Molecule(i*latticeOffset, j*latticeOffset, k*latticeOffset));
            }
        }
    }
    
    // Even parity
    for (int i = (int)molecules.size()-1; i >= 0; i--) {
        if (i%2 != 0) {
            molecules.erase(molecules.begin()+i);
            particleCount--;
        }
    }
    
    return MD_SUCCESS;
}

int Sequential::run() {
    int counter = 0;
    while (true) {
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
        
        if (positions.is_open()) {
            positions << molecules[0]->position[0] << "\t" << molecules[0]->position[1] << "\t" << molecules[0]->position[2] << "\n";
        }
        
        counter = (counter+1)%10;
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
        if (m->position[i] < 0.5) {
            m->acceleration[i] = wallStiffness * (0.5 - m->position[i]);
        } else if (m->position[i] > forceCutoffMinusHalf) {
            m->acceleration[i] = wallStiffness * (forceCutoffMinusHalf - m->position[i]);
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
    double rSquared = dx2 + dy2 + dz2;
    
    if (rSquared >= forceCutoff2) {
        return;
    }
    
    double rSquaredInverse = sigma / rSquared;
    double attract = rSquaredInverse * rSquaredInverse * rSquaredInverse;
    double repel = attract * attract;
    
    double fOverR = 24.0 * epsilon * ((2.0 * repel) - attract) * rSquaredInverse;
    double fx = fOverR * dx;
    double fy = fOverR * dy;
    double fz = fOverR * dz;
    
    m1->updateAcceleration(fx, fy, fz);
    m2->updateAcceleration(0.0 - fx, 0.0 - fy, 0.0 - fz);
}
