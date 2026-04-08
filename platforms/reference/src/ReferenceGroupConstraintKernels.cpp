/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2011-2013 Stanford University and the Authors.      *
 * Authors: Luke Czapla                                                       *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "ReferenceGroupConstraintKernels.h"
#include "openmm/OpenMMException.h"
#include "openmm/internal/ContextImpl.h"
#include "SimTKOpenMMUtilities.h"
#include <set>

using namespace OpenMM;
using namespace std;


static vector<RealVec>& extractPositions(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->positions);
}

static vector<RealVec>& extractVelocities(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->velocities);
}

static vector<RealVec>& extractForces(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->forces);
}


void ReferenceCalcGroupConstraintForceKernel::initialize(const System& system, const GroupConstraintForce& force) {

    // Initialize particle constraint parameters.
    
    particles = force.getParticles();
    referenceParticles = force.getReferenceParticles();
    type = force.getConstraintType();
    Kconstants = force.getConstraintConstant();

    for (int i = 0; i < particles.size(); i++) {
        particleMass.push_back(system.getParticleMass(particles[i]));
    }

    for (int i = 0; i < referenceParticles.size(); i++) {
        referenceParticleMass.push_back(system.getParticleMass(referenceParticles[i]));
    }

}

double ReferenceCalcGroupConstraintForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {
    vector<RealVec>& pos = extractPositions(context);
    vector<RealVec>& force = extractForces(context);
    int numParticles = particles.size();
    int numReferenceParticles = referenceParticles.size();
    double energy = 0;
    
    // Compute the center of mass of particles

    RealVec r1(0.0, 0.0, 0.0);
    RealOpenMM M1 = 0.0;
    RealVec r2(0.0, 0.0, 0.0);
    RealOpenMM M2 = 0.0;

    for (int i = 0; i < numParticles; i++) {
        r1[0] += particleMass[i]*pos[i][0];
	r1[1] += particleMass[i]*pos[i][1];
	r1[2] += particleMass[i]*pos[i][2];
        M1 += particleMass[i];
    }
    r1 /= M1;

    if (type > 0) {
        for (int i = 0; i < numReferenceParticles; i++) {
            r2[0] += referenceParticleMass[i]*pos[i][0];
            r2[1] += referenceParticleMass[i]*pos[i][1];
            r2[2] += referenceParticleMass[i]*pos[i][2];
            M2 += referenceParticleMass[i];
        }
        r2 /= M2;
    }

    // Compute the reference vector position constraint for just the particles

    if (type == 0) {
        RealVec diff = referenceVec-r1;
        RealOpenMM distance = diff.dot(diff);
        energy += 0.5*Kconstants[0]*distance*distance;
        for (int i = 0; i < numParticles; i++) {
		force[i][0] += (Kconstants[0]*particleMass[i]/M1)*diff[0];
                force[i][1] += (Kconstants[0]*particleMass[i]/M1)*diff[1];
                force[i][2] += (Kconstants[0]*particleMass[i]/M1)*diff[2];
	}
    }    

    if (type == 1) {
        RealVec diff = r2-r1;
        RealOpenMM distance = diff.dot(diff);
        energy += 0.5*Kconstants[0]*distance*distance;
        for (int i = 0; i < numParticles; i++) {
		force[i][0] += (Kconstants[0]*particleMass[i]/M1)*diff[0];
                force[i][1] += (Kconstants[0]*particleMass[i]/M1)*diff[1];
                force[i][2] += (Kconstants[0]*particleMass[i]/M1)*diff[2];
	}
    } 

    if (type == 2) {
        RealVec diff = r2-r1;
        RealOpenMM distance = diff.dot(referenceVec);
        energy += 0.5*Kconstants[0]*distance*distance;
        for (int i = 0; i < numParticles; i++) {
	force[i][0] += (Kconstants[0]*distance*particleMass[i]/M1)*referenceVec[0];
	force[i][1] += (Kconstants[0]*distance*particleMass[i]/M1)*referenceVec[1];
	force[i][2] += (Kconstants[0]*distance*particleMass[i]/M1)*referenceVec[2];
	}
    }

}

void ReferenceCalcGroupConstraintForceKernel::copyParametersToContext(ContextImpl& context, const GroupConstraintForce& force) {

}


