/* ****************************************************************
 * (c) 2024                                                       *
 * Joshua Martin                                                  *
 *                                                                *
 * Demo for custom wheel-on-limb robot in Project Chrono          *
 ******************************************************************/

#include "chrono_models/robot/wheel_limb/WheelOnLimb.h"


#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChBodyEasy.h"

#include "chrono/assets/ChVisualSystem.h"
#ifdef CHRONO_IRRLICHT
    #include "chrono_irrlicht/ChVisualSystemIrrlicht.h"

using namespace chrono;
using namespace chrono::parsers;
using namespace chrono::wheel_on_limb;


ChVisualSystem::Type vis_type = ChVisualSystem::Type::IRRLICHT;

int main(int argc, char* argv[]) {
    // Create a Chrono system
    ChSystemSMC sys;
    sys.Set_G_acc(ChVector<>(0, 0, -9.81));
    sys.SetCollisionSystemType(ChCollisionSystem::Type::BULLET);
    ChCollisionModel::SetDefaultSuggestedEnvelope(0.0025);
    ChCollisionModel::SetDefaultSuggestedMargin(0.0025);

    /// Create a "floor" body
    auto floor = chrono_types::make_shared<ChBody>();
    floor->SetBodyFixed(true);
    auto floor_box = chrono_types::make_shared<ChVisualShapeBox>(3, 2, 0.1);
    floor_box->SetTexture(GetChronoDataFile("textures/checker2.png"));
    floor->AddVisualShape(floor_box);
    float friction = 0.9f;
    float Y = 1e7f;

    auto ground_mat = chrono_types::make_shared<ChMaterialSurfaceSMC>();
    ground_mat->SetFriction(friction);

    if (sys.GetContactMethod() == ChContactMethod::SMC) {
        std::static_pointer_cast<ChMaterialSurfaceSMC>(ground_mat)->SetYoungModulus(Y);
    }

    floor->SetCollide(true);
    auto ct_shape = chrono_types::make_shared<ChCollisionShapeBox>(ground_mat, 100, 100, 0.1);
    floor->AddCollisionShape(ct_shape);
    sys.AddBody(floor);

    /// End floor body setup

    /// Create robot


    /// End robot setup

    /// Create visualization window
    std::shared_ptr<ChVisualSystem> vis;
    auto camera_lookat = ChVector<>(0, 0, 0);
    auto camera_loc = camera_lookat + ChVector<>(3, 3, 0);

    auto vis_irr = chrono_types::make_shared<ChVisualSystemIrrlicht>();
    vis_irr->AttachSystem(&sys);
    vis_irr->SetCameraVertical(CameraVerticalDir::Z);
    vis_irr->SetWindowSize(1200, 800);
    vis_irr->SetWindowTitle("RoboSimian URDF demo");
    vis_irr->Initialize();
    vis_irr->AddLogo();
    vis_irr->AddSkyBox();
    vis_irr->AddCamera(camera_loc, camera_lookat);
    vis_irr->AddTypicalLights();

    vis = vis_irr;

    // Solver settings
    sys.SetSolverMaxIterations(200);
    sys.SetSolverType(ChSolver::Type::BARZILAIBORWEIN);

    // Simulation loop
    double step_size = 5e-4;
    ChRealtimeStepTimer real_timer;

    while (vis->Run()) {
        double time = sys.GetChTime();

        vis->BeginScene();
        vis->Render();
        vis->EndScene();

        // Advance system dynamics
        sys.DoStepDynamics(step_size);
        real_timer.Spin(step_size);
    }

  return 0;
}
