#include "openmm/Force.h"
#include "openmm/OpenMMException.h"
#include "openmm/GroupConstraintForce.h"
#include "openmm/internal/AssertionUtilities.h"
#include "openmm/internal/GroupConstraintForceImpl.h"

using namespace OpenMM;
using namespace std;

GroupConstraintForce::GroupConstraintForce() {
}


ForceImpl* GroupConstraintForce::createImpl() const {
    return new GroupConstraintForceImpl(*this);
}


int GroupConstraintForce::addParticle(int particle) {
    particles.push_back(particle);
    return particles.size()-1;
}


void GroupConstraintForce::setConstraintType(int t) { 
    type = t;
}


void GroupConstraintForce::setReferenceVector(Vec3 V) { 
    refVector = V;
}


int GroupConstraintForce::addConstraintConstant(double k) {
    kConstants.push_back(k);
    return kConstants.size()-1;
}


int GroupConstraintForce::addReferenceParticle(int particle) {
    referenceParticles.push_back(particle);
    return referenceParticles.size()-1;
}


void GroupConstraintForce::updateParametersInContext(Context& context) {
    dynamic_cast<GroupConstraintForceImpl&>(getImplInContext(context)).updateParametersInContext(getContextImpl(context));
}


