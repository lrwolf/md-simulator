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
#include <iostream>
#include <memory>
#include <vector>
#include "Molecule.h"

#define MD_SUCCESS 0
#define MD_FAILURE -1

class Simulator {
public:
    Simulator();
    
    virtual int setup();
    virtual int run();
    virtual int cleanup();

    int cubeSide;
    int particleCount;
    
    double latticeOffset;
    double timestep;
    
    double wallStiffness;
    // distance to wall on x, y, and z axes
    double dimensions[3] = { 50.0, 50.0, 50.0 };
    
    double forceCutoff;
    double forceCutoff2;
    
    std::unique_ptr<float[]> data;
    std::unique_ptr<float[]> results;
    std::vector<std::unique_ptr<Molecule>> molecules;
};

#endif /* defined(__sim__Simulator__) */
