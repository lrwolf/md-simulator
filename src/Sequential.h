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

class Sequential : public Simulator {
public:
    Sequential();
    Sequential(int cubeSide);

    virtual int computeAccelerations();
    
    void addSpringForce(Molecule* m);
    void addPairwiseForce(Molecule* m1, Molecule* m2);
    
    std::array<double, 3> springRadius;
};

#endif /* defined(__sim__Sequential__) */
