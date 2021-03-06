//
//  Simulator.h
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#ifndef __sim__Simulator__
#define __sim__Simulator__

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "Molecule.h"

#define MD_SUCCESS 0
#define MD_FAILURE -1
#define MOLECULE_DEBUG 0
#define CHECK_SPRING_CONSTANT 0

class Simulator {
public:
    Simulator();
    Simulator(int cubeSide);
    
    virtual int setup();
    virtual int run();
    virtual int computeAccelerations();
    virtual int cleanup();

    int cubeSide;
    int particleCount;
    
    double latticeOffset;
    double timestep;
    
    double forceCutoffMinusHalf;
    double negForceCutoffMinusHalf;
    double wallStiffness;
    
    double forceCutoff;
    double forceCutoff2;
    double energy;
    
    double sigma;
    double epsilon;
    
    std::vector<std::unique_ptr<Molecule>> molecules;
    
    std::ofstream fPositions, fEnergy;
    
    /**
     * Setters
     */
    void setLatticeOffset(double latticeOffset) {
        this->latticeOffset = latticeOffset;
    }
    
    void setTimestep(double timestep) {
        this->timestep = timestep;
    }
    
    void setWallStiffness(double wallStiffness) {
        this->wallStiffness = wallStiffness;
    }

    void setForceCutoff(double forceCutoff) {
        this->forceCutoff = forceCutoff;
        this->forceCutoff2 = forceCutoff * forceCutoff;
    }
    
    void setSigma(double sigma) {
        this->sigma = sigma;
    }
    
    void setEpsilon(double epsilon) {
        this->epsilon = epsilon;
    }
};

#endif /* defined(__sim__Simulator__) */
