// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2021 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Jason Zhou
// =============================================================================
//
// Demo to show Rassor Rover operated on Rigid Terrain
// This Demo includes operation to spawn a Rassor rover, control wheel speed, and
// control rover steering
//
// =============================================================================

#include "chrono_models/robot/rassor/Rassor.h"

#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChBodyEasy.h"

#include "chrono_sensor/sensors/ChSegmentationCamera.h"
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/filters/ChFilterGrayscale.h"
#include "chrono_sensor/filters/ChFilterSave.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterCameraNoise.h"
#include "chrono_sensor/filters/ChFilterImageOps.h"

#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/filters/ChFilterPCfromDepth.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterVisualizePointCloud.h"
#include "chrono_sensor/filters/ChFilterLidarReduce.h"
#include "chrono_sensor/filters/ChFilterLidarNoise.h"
#include "chrono_sensor/filters/ChFilterSavePtCloud.h"

#include "chrono/physics/ChInertiaUtils.h"

#ifdef CHRONO_POSTPROCESS
    #include "chrono_postprocess/ChGnuPlot.h"
#endif

#include "chrono/assets/ChVisualSystem.h"
#ifdef CHRONO_IRRLICHT
    #include "chrono_irrlicht/ChVisualSystemIrrlicht.h"
using namespace chrono::irrlicht;
#endif
#ifdef CHRONO_VSG
    #include "chrono_vsg/ChVisualSystemVSG.h"
using namespace chrono::vsg3d;
#endif

using namespace chrono;
using namespace chrono::rassor;
using namespace chrono::sensor;
using namespace chrono::geometry;

// -----------------------------------------------------------------------------

// Run-time visualization system (IRRLICHT or VSG)
ChVisualSystem::Type vis_type = ChVisualSystem::Type::VSG;

// Use custom material for the Rassor Rover Wheel
bool use_custom_mat = false;

// Define Rassor rover wheel type
RassorWheelType wheel_type = RassorWheelType::RealWheel;

// Simulation time step
double time_step = 1e-3;

// -----------------------------------------------------------------------------

std::shared_ptr<ChMaterialSurface> CustomWheelMaterial(ChContactMethod contact_method) {
    float mu = 0.4f;   // coefficient of friction
    float cr = 0.2f;   // coefficient of restitution
    float Y = 2e7f;    // Young's modulus
    float nu = 0.3f;   // Poisson ratio
    float kn = 2e5f;   // normal stiffness
    float gn = 40.0f;  // normal viscous damping
    float kt = 2e5f;   // tangential stiffness
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

int main(int argc, char* argv[]) {
    GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

    // Create the Chrono system with gravity in the negative Z direction
    ChSystemNSC sys;
    sys.Set_G_acc(ChVector<>(0, 0, -9.81));

    sys.SetCollisionSystemType(ChCollisionSystem::Type::BULLET);
    ChCollisionModel::SetDefaultSuggestedEnvelope(0.0025);
    ChCollisionModel::SetDefaultSuggestedMargin(0.0025);

    // Create the ground.
    auto ground_mat = chrono_types::make_shared<ChMaterialSurfaceNSC>();
    auto ground = chrono_types::make_shared<ChBodyEasyBox>(30, 30, 1, 100, true, true, ground_mat);
    ground->SetPos(ChVector<>(0, 0, -0.5));
    ground->SetBodyFixed(true);
    ground->GetVisualShape(0)->SetTexture(GetChronoDataFile("textures/concrete.jpg"), 60, 45);
    sys.Add(ground);

    // Construct a Rassor rover and the asociated driver
    auto driver = chrono_types::make_shared<RassorSpeedDriver>(1.0);

    Rassor rassor(&sys, wheel_type);
    rassor.SetDriver(driver);
    if (use_custom_mat)
        rassor.SetWheelContactMaterial(CustomWheelMaterial(ChContactMethod::NSC));

    rassor.Initialize(ChFrame<>(ChVector<>(0, 0, 0.5), QUNIT));
    // Create the run-time visualization interface
#ifndef CHRONO_IRRLICHT
    if (vis_type == ChVisualSystem::Type::IRRLICHT)
        vis_type = ChVisualSystem::Type::VSG;
#endif
#ifndef CHRONO_VSG
    if (vis_type == ChVisualSystem::Type::VSG)
        vis_type = ChVisualSystem::Type::IRRLICHT;
#endif

    std::shared_ptr<ChVisualSystem> vis;
    switch (vis_type) {
        case ChVisualSystem::Type::IRRLICHT: {
#ifdef CHRONO_IRRLICHT
            auto vis_irr = chrono_types::make_shared<ChVisualSystemIrrlicht>();
            vis_irr->AttachSystem(&sys);
            vis_irr->SetCameraVertical(CameraVerticalDir::Z);
            vis_irr->SetWindowSize(800, 600);
            vis_irr->SetWindowTitle("Rassor Rover on Rigid Terrain");
            vis_irr->Initialize();
            vis_irr->AddLogo();
            vis_irr->AddSkyBox();
            vis_irr->AddCamera(ChVector<>(1.5, 1.5, 0.6));
            vis_irr->AddTypicalLights();
            vis_irr->EnableContactDrawing(ContactsDrawMode::CONTACT_DISTANCES);
            vis_irr->EnableShadows();

            vis = vis_irr;
#endif
            break;
        }
        default:
        case ChVisualSystem::Type::VSG: {
#ifdef CHRONO_VSG
            auto vis_vsg = chrono_types::make_shared<ChVisualSystemVSG>();
            vis_vsg->AttachSystem(&sys);
            vis_vsg->AddCamera(ChVector<>(3, 3, 1));
            vis_vsg->SetWindowTitle("Rassor Rover on Rigid Terrain");
            vis_vsg->Initialize();

            vis = vis_vsg;
#endif
            break;
        }
    }

    // Simulation loop
    while (vis->Run()) {
#if defined(CHRONO_IRRLICHT) || defined(CHRONO_VSG)
        vis->BeginScene();
        vis->Render();
        vis->EndScene();
#endif

        // Set current steering angle
        double time = rassor.GetSystem()->GetChTime();

        for (int i = 0; i < 4; i++) {
            driver->SetDriveMotorSpeed((RassorWheelID)i, 2.0);
        }

        driver->SetRazorMotorSpeed((RassorDirID)0, 3.14);
        driver->SetRazorMotorSpeed((RassorDirID)1, -3.14);

        if (time <= 2.0) {
            driver->SetArmMotorSpeed((RassorDirID)0, -0.5);
            driver->SetArmMotorSpeed((RassorDirID)1, 0.5);
        } else {
            driver->SetArmMotorSpeed((RassorDirID)0, 0.0);
            driver->SetArmMotorSpeed((RassorDirID)1, 0.0);
        }

        std::cout << time << std::endl;

        // Update Rassor controls
        rassor.Update();

        sys.DoStepDynamics(time_step);
    }

    return 0;
}
