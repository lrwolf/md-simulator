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

    virtual int setup();
    virtual int run();

    void computeAccelerations();
    void addSpringForce(Molecule* m);
    void positionDifference(Molecule* m1, Molecule* m2);
};

#endif /* defined(__sim__Sequential__) */
