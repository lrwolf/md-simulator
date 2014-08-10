//
//  Sequential.h
//  sim
//
//  Created by Lucas Wolf on 7/12/14.
//
//

#ifndef __sim__Sequential__
#define __sim__Sequential__

#include "Simulator.h"

#define MOLECULE_DEBUG 1
#define CHECK_SPRING_CONSTANT 0

class Sequential : public Simulator {
public:
    Sequential();
    Sequential(int cubeSide);

    virtual int setup();
    virtual int run();

    void computeAccelerations();
    void addSpringForce(Molecule* m);
    void addPairwiseForce(Molecule* m1, Molecule* m2);
    
    double forceCutoffMinusHalf;
    double negForceCutoffMinusHalf;
    
    std::array<double, 3> springRadius;
};

#endif /* defined(__sim__Sequential__) */
