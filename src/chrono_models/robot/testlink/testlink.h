#ifndef TESTLINK_H
#define TESTLINK_H

#include <string>
#include <array>

#include "chrono/assets/ChColor.h"
#include "chrono/physics/ChLinkMotorRotation.h"
#include "chrono/physics/ChSystem.h"
#include "chrono/physics/ChShaft.h"

#include "chrono_parsers/ChParserURDF.h"
#include "chrono_parsers/ChRobotActuation.h"

#include "chrono_models/ChApiModels.h"
using namespace chrono::parsers;
namespace chrono{
namespace testlink{

class CH_MODELS_API Testlink
{
  public:
    Testlink(ChSystem* system);

    ~Testlink() {};

    ChSystem* GetSystem() const { return m_system; }

    /// Initialize the rover at the specified position.
    void Initialize(const ChFrame<>& pos);

    std::shared_ptr<ChLinkBase> GetSensorLink() const { return m_sensor_link[0]; }

    void PrintWrenchAtJoint();

  private:
    ChSystem* m_system;
    ChParserURDF* m_robot;

    std::array<std::shared_ptr<ChBody>, 2> m_links;


    // Sensor link
    std::array<std::shared_ptr<ChLinkBase>, 1> m_sensor_link;

};

};
};

#endif