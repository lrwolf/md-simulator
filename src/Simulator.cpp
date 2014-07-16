//
//  Simulator.cpp
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#include "Simulator.h"

const int Default_CubeSide = 2;
const double Default_LatticeOffset = 1.0; //sqrtf(3.f)/2;
const double Default_Timestep = 0.025;
const double Default_WallStiffness = 50.0;
const double Default_ForceCutoff = 3.0;

Simulator::Simulator() : cubeSide(Default_CubeSide)
    , particleCount(cubeSide*cubeSide*cubeSide)
    , latticeOffset(Default_LatticeOffset)
    , timestep(Default_Timestep)
    , wallStiffness(Default_WallStiffness)
    , forceCutoff(Default_ForceCutoff)
    , forceCutoff2(forceCutoff * forceCutoff) {
}

int Simulator::setup() {
    return MD_SUCCESS;
}

int Simulator::run() {
    return MD_SUCCESS;
}

int Simulator::cleanup() {
    return MD_SUCCESS;
}
