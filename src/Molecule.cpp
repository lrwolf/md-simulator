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
    std::cout << sqrt(position[0]*position[0] + position[1]*position[1] + position[2]*position[2]) << std::endl;
}
