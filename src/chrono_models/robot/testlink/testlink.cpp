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

#include "chrono_models/robot/testlink/testlink.h"

#include <iostream>

namespace chrono {
namespace testlink {

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

Testlink::Testlink(ChSystem* system) : m_system(system) {
  m_robot = new ChParserURDF(GetChronoDataFile("robot/testlink/urdf/testlink.urdf"));
}

void Testlink::Initialize(const ChFrame<>& pos) {
  // Set root body pose
  m_robot->SetRootInitPose(pos);

  // Create the Chrono model
  m_robot->PopulateSystem(*m_system);

  m_links[0] = m_robot->GetChBody("link1"); m_links[0]->SetCollide(true);
  m_links[1] = m_robot->GetChBody("link2"); m_links[1]->SetCollide(true);

  auto cmat = DefaultContactMaterial(m_system->GetContactMethod());

  m_links[0]->GetCollisionModel()->SetAllShapesMaterial(cmat);

  m_sensor_link[0] = m_robot->GetChLink("sensor_joint");
}

void Testlink::PrintWrenchAtJoint()
{
  // Get wrench at foot
  ChVector force  = GetSensorLink()->Get_react_force();
  ChVector torque = GetSensorLink()->Get_react_torque();
  
  // Decompose 
  double fx = force.x();
  double fy = force.y();
  double fz = force.z();
  double tx = torque.x();
  double ty = torque.y();
  double tz = torque.z();

  std::cout << "sensor_joint" << "   :   [ " << fx << "  ,  " << fy << "  ,  " << fz << "  ,  " 
                                             << tx << "  ,  " << ty << "  ,  " << tz <<   " ]\n";

  std::cout << std::endl;
}

};
};