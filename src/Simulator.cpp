//
//  Simulator.cpp
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#include "Simulator.h"

const int Default_CubeSide = 2;
const double Default_LatticeOffset = 1.0;
const double Default_Timestep = 0.025;
const double Default_ForceCutoff = 10;
const double Default_Sigma = 1.0;
const double Default_Epsilon = 1.0;
const double Default_WallStiffness = 2.0;

Simulator::Simulator() : cubeSide(Default_CubeSide)
    , particleCount(cubeSide*cubeSide*cubeSide)
    , latticeOffset(Default_LatticeOffset)
    , timestep(Default_Timestep)
    , wallStiffness(Default_WallStiffness)
    , forceCutoff(Default_ForceCutoff)
    , forceCutoff2(forceCutoff * forceCutoff)
    , sigma(Default_Sigma)
    , epsilon(Default_Epsilon) {
        forceCutoffMinusHalf = forceCutoff - 0.5;
        negForceCutoffMinusHalf = 0.0 - forceCutoffMinusHalf;
}

Simulator::Simulator(int cubeSide) : Simulator() {
    this->cubeSide = cubeSide;
    this->particleCount = cubeSide*cubeSide*cubeSide;
}

int Simulator::setup() {
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
    
    fPositions.open("/tmp/positions.txt", std::ios::out | std::ios::app);
    fEnergy.open("/tmp/energy.txt", std::ios::out | std::ios::app);
    return MD_SUCCESS;
}

int Simulator::run() {
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

int Simulator::computeAccelerations() {
    return MD_SUCCESS;
}

int Simulator::cleanup() {
    fEnergy.close();
    fPositions.close();
    return MD_SUCCESS;
}