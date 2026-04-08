#include "openmm/OpenMMException.h"
#include "openmm/internal/ContextImpl.h"
#include "openmm/internal/GroupConstraintForceImpl.h"
#include "openmm/GroupConstraintKernels.h"
#include <cmath>
#include <map>
#include <set>
#include <sstream>

using namespace OpenMM;
using namespace std;

GroupConstraintForceImpl::GroupConstraintForceImpl(const GroupConstraintForce& owner) : owner(owner) {
}

GroupConstraintForceImpl::~GroupConstraintForceImpl() {
}

void GroupConstraintForceImpl::initialize(ContextImpl& context) {
    kernel = context.getPlatform().createKernel(CalcGroupConstraintForceKernel::Name(), context);
    const System& system = context.getSystem();
    vector <int> particles = owner.getParticles();
    for (int i = 0; i < particles.size(); i++) {
        int pid = particles[i];
        if (pid < 0 || pid >= system.getNumParticles()) {
            stringstream msg;
            msg << "GroupConstraintForce: Illegal particle index: ";
            msg << pid;
            throw OpenMMException(msg.str());
        }
    }
    vector <int> referenceParticles = owner.getReferenceParticles();
    for (int i = 0; i < referenceParticles.size(); i++) {
        int pid = referenceParticles[i];
        if (pid < 0 || pid >= system.getNumParticles()) {
            stringstream msg;
            msg << "GroupConstraintForce: Illegal particle index: ";
            msg << pid;
            throw OpenMMException(msg.str());
        }
    }
    kernel.getAs<CalcGroupConstraintForceKernel>().initialize(context.getSystem(), owner);
}

double GroupConstraintForceImpl::calcForcesAndEnergy(ContextImpl& context, bool includeForces, bool includeEnergy, int groups) {
    return kernel.getAs<CalcGroupConstraintForceKernel>().execute(context, includeForces, includeEnergy);
}

std::vector<std::string> GroupConstraintForceImpl::getKernelNames() {
    std::vector<std::string> names;
    names.push_back(CalcGroupConstraintForceKernel::Name());
    return names;
}

void GroupConstraintForceImpl::updateParametersInContext(ContextImpl& context) {
    kernel.getAs<CalcGroupConstraintForceKernel>().copyParametersToContext(context, owner);
}


