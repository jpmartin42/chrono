

#include <cmath>
#include <algorithm>

#include "chrono/physics/ChBodyEasy.h"
#include "chrono/assets/ChVisualShape.h"
#include "chrono/assets/ChVisualShapeCylinder.h"
#include "chrono/assets/ChVisualShapeSphere.h"
#include "chrono/assets/ChTexture.h"
#include "chrono/assets/ChVisualShapeTriangleMesh.h"

#include "chrono/motion_functions/ChFunction_Setpoint.h"

#include "chrono/physics/ChLinkMotorRotationAngle.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChLinkMotorRotationTorque.h"
#include "chrono/physics/ChShaftsBody.h"

#include "chrono/physics/ChInertiaUtils.h"

#include "chrono_models/robot/wheel_limb/WheelOnLimb.h"

namespace chrono {
namespace wheel_on_limb {

// =============================================================================

// Default contact material for rover parts
std::shared_ptr<ChMaterialSurface> DefaultContactMaterial(ChContactMethod contact_method) {
    float mu = 0.9f;   // coefficient of friction
    float cr = 0.0f;   // coefficient of restitution
    float Y = 1.5e6f;  // Young's modulus
    float nu = 0.3f;   // Poisson ratio
    float kn = 5e6f;   // normal stiffness
    float gn = 40.0f;  // normal viscous damping
    float kt = 5e6f;   // tangential stiffness
    float gt = 20.0f;  // tangential viscous damping

    switch (contact_method) {
        case ChContactMethod::NSC: {
            auto matNSC = chrono_types::make_shared<ChMaterialSurfaceNSC>();
            matNSC->SetFriction(mu);
            matNSC->SetRestitution(cr);
            return matNSC;
        }
        case ChContactMethod::SMC: {
            auto matSMC = chrono_types::make_shared<ChMaterialSurfaceSMC>();
            matSMC->SetFriction(mu);
            matSMC->SetRestitution(cr);
            matSMC->SetYoungModulus(Y);
            matSMC->SetPoissonRatio(nu);
            matSMC->SetKn(kn);
            matSMC->SetGn(gn);
            matSMC->SetKt(kt);
            matSMC->SetGt(gt);
            return matSMC;
        }
        default:
            return std::shared_ptr<ChMaterialSurface>();
    }
}

WheelOnLimb::WheelOnLimb(ChSystem* system) : m_system(system) {
    m_robot = new ChParserURDF(GetChronoDataFile("robot/wheel_on_limb/urdf/wheel_on_limb.urdf"));
}

void WheelOnLimb::Initialize(ChSystem* system) : m_system(system)
{
    // Set root body pose
    m_robot->SetRootInitPose(pos);

    // Enable actuation for all joints


    // Create the chrono model
    m_robot->PopulateSystem(*m_system);

    // Unfix root body
    m_robot->GetRootChBody()->SetBodyFixed(false);

    // Get individual bodies


    // Set collisions for bodies


    // Set collision material


    // Obtain motors
    
    // 

    for (size_t i = 0; i < 4; i++)
    {
      
    }



}

void WheelOnLimb::InitPose(ChSystem* system, double step_size)
{
  // Temporarily set all joint actuation to position control

  // Set joints to desired poses

  // Iterate system dynamics once to set the joints to the desired positions
  sys.DoStepDynamics(step_size_init_);

  // Re-enable torque control
}

// Command types for system
void WheelOnLimb::SetMotorCommandPos(size_t num_limb, size_t num_joint, double pos)
{

}

void WheelOnLimb::SetMotorCommandVel(size_t num_limb, size_t num_joint, double vel)
{

}

void WheelOnLimb::SetMotorCommandTorque(size_t num_limb, size_t num_joint, double tau)
{

}


};  // End namespace wheel_on_limb
};  // End namespace chrono