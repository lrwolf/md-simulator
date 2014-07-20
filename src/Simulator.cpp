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
    positions.open("/tmp/positions.txt", std::ios::out | std::ios::app);
    return MD_SUCCESS;
}

int Simulator::run() {
    return MD_SUCCESS;
}

int Simulator::cleanup() {
    positions.close();
    return MD_SUCCESS;
}
