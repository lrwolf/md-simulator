//
//  Molecule.h
//  sim
//
//  Created by Lucas Wolf on 3/29/14.
//
//

#ifndef __sim__Molecule__
#define __sim__Molecule__

#include <array>

class Molecule {
public:
    Molecule();
    Molecule(double x, double y, double z);

    void setPosition(double x, double y, double z);
    void updatePosition(double dt);
    void updateVelocityHalf(double dt);
    void updateAcceleration(double fx, double fy, double fz);
    void addSpringForce(double wallStiffness, double* dimensions);
    
    void printPosition();
    void printVelocity();
    void printAcceleration();
    void printTitle(std::string const& title);
    void distanceFromOrigin();
    
    void positionDifference(Molecule* m2);
    
private:
    std::array<double,3> position;
    std::array<double,3> velocity;
    std::array<double,3> velocityHalf;
    std::array<double,3> acceleration;
    
    bool fTitle;
};

#endif /* defined(__sim__Molecule__) */
