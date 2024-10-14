#pragma once

#include "chrono/assets/ChColor.h"
#include "chrono/physics/ChLinkMotorRotation.h"
#include "chrono/physics/ChSystem.h"
#include "chrono/physics/ChShaft.h"

#include "chrono_parsers/ChParserURDF.h"
#include "chrono_parsers/ChRobotActuation.h"

#include "chrono_models/ChApiModels.h"

using namespace chrono::parsers;

namespace chrono {
namespace wheel_on_limb {

/// @addtogroup robot_models_wheel_on_limb
/// @{

/// Cobra wheel/suspension identifiers.
enum CommandTypeID {
    POS = 0;  ///< position
    VEL = 1;  ///< velocity
    TOR = 2;  ///< acceleration
};

/// Joint limits

class CH_MODELS_API WheelOnLimb
{
  public:

    WheelOnLimb(ChSystem* system);
    ~WheelOnLimb() {};

    // Get the containing system
    ChSystem* GetSystem() const { return m_system; };

    // Initialize the system at the specified position
    void Initialize(const ChFrame<>& pos);

    // Command types for system
    void SetMotorCommandPos(size_t num_limb, size_t num_joint, double pos);
    void SetMotorCommandVel(size_t num_limb, size_t num_joint, double vel);
    void SetMotorCommandTorque(size_t num_limb, size_t num_joint, double tau);

    // Function to return specific values
    void GetFTAtJoints(size_t limb_num);


    // Pointers to get specific bodies

  private:
    ChSystem* m_system;     // Pointer to the chrono system
    ChParserURDF* m_robot;  // URDF parser

    // Wheel on limb system body parts

    // Wheel on limb system motors
    std::array<std::shared_ptr<ChLinkMotor>, 4> m_j1_motors;
    std::array<std::shared_ptr<ChLinkMotor>, 4> m_j2_motors;
    std::array<std::shared_ptr<ChLinkMotor>, 4> m_j3_motors;
    std::array<std::shared_ptr<ChLinkMotor>, 4> m_j4_motors;
    std::array<std::shared_ptr<ChLinkMotor>, 4> m_j5_motors;

    std::array<std::shared_ptr<ChFunction_Setpoint>, 4> m_j1_motor_funcs;  ///< drive motor functions
    std::array<std::shared_ptr<ChFunction_Setpoint>, 4> m_j2_motor_funcs;  ///< drive motor functions
    std::array<std::shared_ptr<ChFunction_Setpoint>, 4> m_j3_motor_funcs;  ///< drive motor functions
    std::array<std::shared_ptr<ChFunction_Setpoint>, 4> m_j4_motor_funcs;  ///< drive motor functions
    std::array<std::shared_ptr<ChFunction_Setpoint>, 4> m_j5_motor_funcs;  ///< drive motor functions


};

/// @} robot_models_wheel_on_limb

};  // End namespace wheel_on_limb
};  // End namespace chrono