// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2023 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// External project template for building 1st of 2 Chrono co-simulation FMUs.
// =============================================================================

#include "chrono/physics/ChBodyEasy.h"

// #define FMI2_FUNCTION_PREFIX MyModel_
#include "FmuComponentChrono1.h"

using namespace chrono;

FmuComponent::FmuComponent(fmi2String instanceName,
                           fmi2Type fmuType,
                           fmi2String fmuGUID,
                           fmi2String fmuResourceLocation,
                           const fmi2CallbackFunctions* functions,
                           fmi2Boolean visible,
                           fmi2Boolean loggingOn)
    : FmuChronoComponentBase(instanceName, fmuType, fmuGUID, fmuResourceLocation, functions, visible, loggingOn) {
    initializeType(fmuType);

    SetChronoDataPath(CHRONO_DATA_DIR);

    /// FMU_ACTION: declare relevant variables
    AddFmuVariable(&x_tt, "x_tt", FmuVariable::Type::Real, "m/s2", "cart acceleration");
    AddFmuVariable(&x_t, "x_t", FmuVariable::Type::Real, "m/s", "cart velocity");
    AddFmuVariable(&x, "x", FmuVariable::Type::Real, "m", "cart position");
    AddFmuVariable(&theta_tt, "theta_tt", FmuVariable::Type::Real, "rad/s2", "pendulum ang acceleration");
    AddFmuVariable(&theta_t, "theta_t", FmuVariable::Type::Real, "rad/s", "pendulum ang velocity");
    AddFmuVariable(&theta, "theta", FmuVariable::Type::Real, "rad", "pendulum angle");
    AddFmuVariable(&pendulum_length, "pendulum_length", FmuVariable::Type::Real, "m", "pendulum length",
                   FmuVariable::CausalityType::parameter, FmuVariable::VariabilityType::fixed);
    AddFmuVariable(&cart_mass, "cart_mass", FmuVariable::Type::Real, "kg", "pendulum mass",
                   FmuVariable::CausalityType::parameter, FmuVariable::VariabilityType::fixed);
    AddFmuVariable(&pendulum_mass, "pendulum_mass", FmuVariable::Type::Real, "kg", "cart mass",
                   FmuVariable::CausalityType::parameter, FmuVariable::VariabilityType::fixed);

    auto ground = chrono_types::make_shared<ChBody>();
    ground->SetFixed(true);
    sys.Add(ground);

    // Cart is moving along X axis, Pendulum rotates along Z axis

    auto cart = chrono_types::make_shared<ChBodyEasyBox>(0.2, 0.1, 0.1, 750, true, false);
    cart->SetMass(cart_mass);
    sys.Add(cart);

    auto pendulum = chrono_types::make_shared<ChBodyEasyBox>(0.025, pendulum_length, 0.01, 750, true, false);
    pendulum->SetMass(pendulum_mass);
    pendulum->SetInertiaXX(ChVector3d(0.01, 0.01, 0.01));
    sys.Add(pendulum);

    auto cart_prism = chrono_types::make_shared<ChLinkLockPrismatic>();
    cart_prism->Initialize(cart, ground, ChFrame<>(VNULL, Q_ROTATE_Z_TO_X));
    cart_prism->SetName("cart_prism");
    sys.Add(cart_prism);

    auto pendulum_rev = chrono_types::make_shared<ChLinkRevolute>();
    pendulum_rev->Initialize(pendulum, cart, true, ChFrame<>(VNULL, QUNIT),
                             ChFrame<>(ChVector3d(0, +pendulum_length / 2, 0), QUNIT));
    pendulum_rev->SetName("pendulum_rev");
    sys.Add(pendulum_rev);

    sys.DoAssembly(AssemblyLevel::FULL);

#ifdef CHRONO_IRRLICHT
    vis = chrono_types::make_shared<irrlicht::ChVisualSystemIrrlicht>();
    vis->AttachSystem(&sys);
    vis->SetWindowSize(800, 600);
    vis->SetWindowTitle("CartPendulumFMU");
    vis->Initialize();
    vis->AddLogo();
    vis->AddSkyBox();
    vis->AddCamera(ChVector3d(-0.5, -0.5, -1.0));
    vis->AddTypicalLights();
#endif

    m_postStepCallbacks.push_back([this]() { x_tt = this->sys.SearchBodyID(10)->GetLinAcc().x(); });
    m_postStepCallbacks.push_back([this]() { x_t = this->sys.SearchBodyID(10)->GetLinVel().x(); });
    m_postStepCallbacks.push_back([this]() { x = this->sys.SearchBodyID(10)->GetPos().x(); });
};

void FmuComponent::_preModelDescriptionExport() {
    _exitInitializationMode();
    ChOutputFMU archive_fmu(*this);
    archive_fmu << CHNVP(sys);
}

void FmuComponent::_postModelDescriptionExport() {}

void FmuComponent::_exitInitializationMode() {
    sys.DoAssembly(AssemblyLevel::FULL);
};

fmi2Status FmuComponent::_doStep(fmi2Real currentCommunicationPoint,
                                 fmi2Real communicationStepSize,
                                 fmi2Boolean noSetFMUStatePriorToCurrentPoint) {
    while (m_time < currentCommunicationPoint + communicationStepSize) {
        fmi2Real step_size = std::min((currentCommunicationPoint + communicationStepSize - m_time),
                                      std::min(communicationStepSize, m_stepSize));

#ifdef CHRONO_IRRLICHT
        if (vis) {
            vis->Run();
            vis->BeginScene();
            vis->Render();
            vis->EndScene();
        }
#endif

        sys.DoStepDynamics(step_size);
        sendToLog("Step at time: " + std::to_string(m_time) + " with timestep: " + std::to_string(step_size) +
                      "ms succeeded.\n",
                  fmi2Status::fmi2OK, "logAll");

        m_time += step_size;

        realtime_timer.Spin(step_size);
    }

    return fmi2Status::fmi2OK;
}
