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
    return MD_SUCCESS;
}

int Simulator::cleanup() {
    fEnergy.close();
    fPositions.close();
    return MD_SUCCESS;
}
