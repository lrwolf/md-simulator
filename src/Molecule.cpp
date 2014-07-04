//
//  Molecule.cpp
//  sim
//
//  Created by Lucas Wolf on 3/29/14.
//
//

#include <cmath>
#include <iostream>
#include <string>
#include "Molecule.h"

const double forceCutoff = 3.0;
const double forceCutoff2 = forceCutoff * forceCutoff;


/*
 //Normalization of Velocities
 int nv = 2*N;
 int ng = nv-4;
 // Scale the velocity to satisfy the partition theorem
 double ek = 0;
 for (i=0; i<N; ++i)
 ek += (Math.pow(m[i].vx,2)+Math.pow(m[i].vy,2));
 double vs = Math.sqrt(1.0*ek*nv/(ng*T));
 for (i=0; i<N; ++i)
 {
 m[i].vx /= vs;
 m[i].vy /= vs;
 
 }
 */

Molecule::Molecule() {
    Molecule(0.0, 0.0, 0.0);
}

Molecule::Molecule(double x, double y, double z) : fTitle(false) {
    position[0] = x;
    position[1] = y;
    position[2] = z;

    velocityHalf[0] = rand() / (double)RAND_MAX - 0.5f;
    velocityHalf[1] = rand() / (double)RAND_MAX - 0.5f;
    velocityHalf[2] = rand() / (double)RAND_MAX - 0.5f;
    
    acceleration[0] = 0.0;
    acceleration[1] = 0.0;
    acceleration[2] = 0.0;
}

void Molecule::setPosition(double x, double y, double z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void Molecule::updatePosition(double dt) {
    updateVelocityHalf(dt);
    position[0] += velocityHalf[0] * dt;
    position[1] += velocityHalf[1] * dt;
    position[2] += velocityHalf[2] * dt;
}

void Molecule::updateVelocityHalf(double dt) {
    double dtOver2 = dt / 2.0;
    velocityHalf[0] += acceleration[0] * dtOver2;
    velocityHalf[1] += acceleration[1] * dtOver2;
    velocityHalf[2] += acceleration[2] * dtOver2;
}

void Molecule::updateAcceleration(double fx, double fy, double fz) {
    acceleration[0] += fx;
    acceleration[1] += fy;
    acceleration[2] += fz;
}

void Molecule::positionDifference(Molecule* m2) {
    double dx = position[0] - m2->position[0];
    double dy = position[1] - m2->position[1];
    double dz = position[2] - m2->position[2];
    
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double dz2 = dz * dz;
    
    if (dx2 >= forceCutoff2 || dy2 >= forceCutoff2 || dz2 >= forceCutoff2) {
        //printTitle("Too far to bother.");
        return;
    }
    
    double rSquared = dx2 + dy2 + dz2;
    //  sigma / rSquared
    double rSquaredInverse = 1.0 / rSquared;
    double attract = rSquaredInverse * rSquaredInverse * rSquaredInverse;
    double repel = attract * attract;
    
    double fOverR = 24.0 * ((2.0 * repel) - attract) * rSquaredInverse; // * epsilon
    double fx = fOverR * dx;
    double fy = fOverR * dy;
    double fz = fOverR * dz;
    
    updateAcceleration(fx, fy, fz);
    m2->updateAcceleration(0.0 - fx, 0.0 - fy, 0.0 - fz);
}

void Molecule::addSpringForce(double wallStiffness, double* dimensions) {
    for (int i = 0; i < 3; i++) {
        double dimensionMinusHalf = dimensions[i] - 0.5;
        if (position[i] < 0.5) {
            acceleration[i] = wallStiffness * (0.5 - position[i]);
        } else if (position[i] > dimensionMinusHalf) {
            acceleration[i] = wallStiffness * (dimensionMinusHalf - position[i]);
        } else {
            acceleration[i] = 0.0;
        }
    }
}

void Molecule::printTitle(std::string const& title) {
    if (fTitle) {
        std::cout << title << std::endl;
    }
}

void Molecule::printPosition() {
    printTitle("Position");
    std::cout << "" << position[0] << "\t" << position[1] << "\t" << position[2] << std::endl;
}

void Molecule::printVelocity() {
    printTitle("Velocity");
    std::cout << "" << velocityHalf[0] << "\t" << velocityHalf[1] << "\t" << velocityHalf[2] << std::endl;
}

void Molecule::printAcceleration() {
    printTitle("Acceleration");
    std::cout << "" << acceleration[0] << "\t" << acceleration[1] << "\t" << acceleration[2] << std::endl;
}

void Molecule::distanceFromOrigin() {
    std::cout << sqrt(position[0]*position[0] + position[1]*position[1] + position[2]*position[2])<< std::endl;
}