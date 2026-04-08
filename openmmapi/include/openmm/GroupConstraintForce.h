#ifndef OPENMM_GROUPCONSTRAINTFORCE_H_
#define OPENMM_GROUPCONSTRAINTFORCE_H_


#include "openmm/Context.h"
#include "openmm/Force.h"
#include "openmm/Vec3.h"
#include <vector>

// current types: 0 = center of mass constraint around refVector
//                1 = center of mass distances between particle and refParticles
//                2 = center of mass distances between particles and refParticles along vector
// to be implemented:
// 3 = RMSD constraint
// 4 = 
// 

namespace OpenMM {

/**
 *
 * This class implements constraints based on a set of particles and optionally a set of references
 *
*/

class GroupConstraintForce : public Force {
public:

    GroupConstraintForce();

    int getNumParticles() const {
        return particles.size();
    }
    int getNumReferenceParticles() const {
        return referenceParticles.size();
    }
    std::vector<int> getParticles() const { 
        return particles; 
    }
    std::vector<int> getReferenceParticles() const { 
        return referenceParticles;
    }
    int getConstraintType() const { 
        return type;
    }
    Vec3 getReferenceVector() const { 
        return refVector; 
    }
    std::vector<double> getConstraintConstant() const { 
        return kConstants; 
    }

    void setReferenceVector(Vec3 V);
    void setConstraintType(int t);
 
    int addParticle(int particle);
    int addReferenceParticle(int particle);
    int addConstraintConstant(double k);

protected:
    void updateParametersInContext(Context& context);
    ForceImpl* createImpl() const;
private:
    int type;
    std::vector<int> particles;
    std::vector<int> referenceParticles;
    Vec3 refVector;
    std::vector<double> kConstants;
};


} // namespace OpenMM

#endif /*OPENMM_GROUPCONSTRAINTFORCE_H_*/


