// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2021 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Json Zhou
// =============================================================================
//
// NASA RASSOR Mining Experimental Rover Model Class.
// This class contains model for an experimental rover Rassor
// Reference page: https://technology.nasa.gov/patent/KSC-TOPS-7
//
// =============================================================================
#include <cmath>

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

#include "chrono_models/robot/rassor/Rassor.h"

namespace chrono {
namespace rassor {

// =============================================================================

// Default contact material for rover parts
std::shared_ptr<ChMaterialSurface> DefaultContactMaterial(ChContactMethod contact_method) {
    float mu = 0.4f;   // coefficient of friction
    float cr = 0.0f;   // coefficient of restitution
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

// Add a revolute joint between body_1 and body_2
// rel_joint_pos and rel_joint_rot are the position and the rotation of the revolute point
void AddLockJoint(std::shared_ptr<ChBodyAuxRef> body_1,
                  std::shared_ptr<ChBodyAuxRef> body_2,
                  std::shared_ptr<RassorChassis> chassis,
                  const ChVector<>& rel_joint_pos,
                  const ChQuaternion<>& rel_joint_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_joint_pos, rel_joint_rot);

    // auto revo = chrono_types::make_shared<ChLinkLockRevolute>();
    auto revo = chrono_types::make_shared<ChLinkLockLock>();
    revo->Initialize(body_1, body_2, ChCoordsys<>(X_GC.GetCoord().pos, X_GC.GetCoord().rot));
    chassis->GetBody()->GetSystem()->AddLink(revo);
}

// Add a revolute joint between two bodies at the given position and orientation
// (expressed in and relative to the chassis frame).
void AddRevoluteJoint(std::shared_ptr<ChBody> body1,
                      std::shared_ptr<ChBody> body2,
                      std::shared_ptr<RassorChassis> chassis,
                      const ChVector<>& rel_pos,
                      const ChQuaternion<>& rel_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_pos, rel_rot);

    // Create joint (DOF about Z axis of X_GC frame)
    auto joint = chrono_types::make_shared<ChLinkLockRevolute>();
    joint->Initialize(body1, body2, ChCoordsys<>(X_GC.GetPos(), X_GC.GetRot()));
    chassis->GetBody()->GetSystem()->AddLink(joint);
}

// Add a universal joint between two bodies at the given position and orientation
// (expressed in and relative to the chassis frame).
void AddUniversalJoint(std::shared_ptr<ChBody> body1,
                       std::shared_ptr<ChBody> body2,
                       std::shared_ptr<RassorChassis> chassis,
                       const ChVector<>& rel_pos,
                       const ChQuaternion<>& rel_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_pos, rel_rot);

    // Create joint (DOFs about X and Y axes of X_GC frame)
    auto joint = chrono_types::make_shared<ChLinkUniversal>();
    joint->Initialize(body1, body2, X_GC);
    chassis->GetBody()->GetSystem()->AddLink(joint);
}

// Add a rotational speed motor between two bodies at the given position and orientation
// (expressed in and relative to the chassis frame).
std::shared_ptr<ChLinkMotorRotationSpeed> AddMotorSpeed(std::shared_ptr<ChBody> body1,
                                                        std::shared_ptr<ChBody> body2,
                                                        std::shared_ptr<RassorChassis> chassis,
                                                        const ChVector<>& rel_pos,
                                                        const ChQuaternion<>& rel_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_pos, rel_rot);

    // Create motor (actuated DOF about Z axis of X_GC frame)
    auto motor = chrono_types::make_shared<ChLinkMotorRotationSpeed>();
    motor->Initialize(body1, body2, X_GC);
    chassis->GetBody()->GetSystem()->AddLink(motor);

    return motor;
}

// Add a rotational angle motor between two bodies at the given position and orientation
// (expressed in and relative to the chassis frame).
std::shared_ptr<ChLinkMotorRotationAngle> AddMotorAngle(std::shared_ptr<ChBody> body1,
                                                        std::shared_ptr<ChBody> body2,
                                                        std::shared_ptr<RassorChassis> chassis,
                                                        const ChVector<>& rel_pos,
                                                        const ChQuaternion<>& rel_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_pos, rel_rot);

    // Create motor (actuated DOF about Z axis of X_GC frame)
    auto motor = chrono_types::make_shared<ChLinkMotorRotationAngle>();
    motor->Initialize(body1, body2, X_GC);
    chassis->GetBody()->GetSystem()->AddLink(motor);

    return motor;
}

// Add a rotational torque motor between two bodies at the given position and orientation
// (expressed in and relative to the chassis frame).
std::shared_ptr<ChLinkMotorRotationTorque> AddMotorTorque(std::shared_ptr<ChBody> body1,
                                                          std::shared_ptr<ChBody> body2,
                                                          std::shared_ptr<RassorChassis> chassis,
                                                          const ChVector<>& rel_pos,
                                                          const ChQuaternion<>& rel_rot) {
    // Express relative frame in global
    ChFrame<> X_GC = chassis->GetBody()->GetFrame_REF_to_abs() * ChFrame<>(rel_pos, rel_rot);

    // Create motor (actuated DOF about Z axis of X_GC frame)
    auto motor = chrono_types::make_shared<ChLinkMotorRotationTorque>();
    motor->Initialize(body1, body2, X_GC);
    chassis->GetBody()->GetSystem()->AddLink(motor);

    return motor;
}

// =============================================================================

// Base class for all Rassor Part
RassorPart::RassorPart(const std::string& name,
                       const ChFrame<>& rel_pos,
                       std::shared_ptr<ChMaterialSurface> mat,
                       bool collide)
    : m_name(name), m_pos(rel_pos), m_mat(mat), m_collide(collide), m_visualize(true) {}

void RassorPart::Construct(ChSystem* system) {
    m_body = chrono_types::make_shared<ChBodyAuxRef>();
    m_body->SetNameString(m_name + "_body");
    m_body->SetMass(m_mass);
    m_body->SetInertiaXX(m_inertia);
    m_body->SetFrame_COG_to_REF(m_cog);

    // Add visualization shape
    if (m_visualize) {
        auto vis_mesh_file = GetChronoDataFile("robot/rassor/obj/" + m_mesh_name + ".obj");
        auto trimesh_vis = geometry::ChTriangleMeshConnected::CreateFromWavefrontFile(vis_mesh_file, true, true);
        trimesh_vis->Transform(m_mesh_xform.GetPos(), m_mesh_xform.GetA());  // translate/rotate/scale mesh
        trimesh_vis->RepairDuplicateVertexes(1e-9);                          // if meshes are not watertight

        auto trimesh_shape = chrono_types::make_shared<ChVisualShapeTriangleMesh>();
        trimesh_shape->SetMesh(trimesh_vis);
        trimesh_shape->SetName(m_mesh_name);
        trimesh_shape->SetMutable(false);
        trimesh_shape->SetColor(m_color);
        m_body->AddVisualShape(trimesh_shape);
    }

    // Add collision shape
    auto col_mesh_file = GetChronoDataFile("robot/rassor/obj/" + m_mesh_name + ".obj");
    auto trimesh_col = geometry::ChTriangleMeshConnected::CreateFromWavefrontFile(col_mesh_file, false, false);
    trimesh_col->Transform(m_mesh_xform.GetPos(), m_mesh_xform.GetA());  // translate/rotate/scale mesh
    trimesh_col->RepairDuplicateVertexes(1e-9);                          // if meshes are not watertight

    auto shape = chrono_types::make_shared<ChCollisionShapeTriangleMesh>(m_mat, trimesh_col, false, false, 0.005);
    m_body->AddCollisionShape(shape);
    m_body->SetCollide(m_collide);

    system->AddBody(m_body);
}

void RassorPart::CalcMassProperties(double density) {
    auto mesh_filename = GetChronoDataFile("robot/rassor/obj/" + m_mesh_name + ".obj");
    auto trimesh_col = geometry::ChTriangleMeshConnected::CreateFromWavefrontFile(mesh_filename, true, false);
    trimesh_col->Transform(m_mesh_xform.GetPos(), m_mesh_xform.GetA());  // translate/rotate/scale mesh
    trimesh_col->RepairDuplicateVertexes(1e-9);                          // if meshes are not watertight

    double vol;
    ChVector<> cog_pos;
    ChMatrix33<> cog_rot;
    ChMatrix33<> inertia;
    trimesh_col->ComputeMassProperties(true, vol, cog_pos, inertia);
    ChInertiaUtils::PrincipalInertia(inertia, m_inertia, cog_rot);
    m_mass = density * vol;
    m_inertia *= density;
    m_cog = ChFrame<>(cog_pos, cog_rot);
}

void RassorPart::Initialize(std::shared_ptr<ChBodyAuxRef> chassis) {
    Construct(chassis->GetSystem());

    // Set absolute position
    ChFrame<> X_GC = chassis->GetFrame_REF_to_abs() * m_pos;
    m_body->SetFrame_REF_to_abs(X_GC);
}

// =============================================================================

// Rover Chassis
RassorChassis::RassorChassis(const std::string& name, std::shared_ptr<ChMaterialSurface> mat)
    : RassorPart(name, ChFrame<>(VNULL, QUNIT), mat, false) {
    m_mesh_name = "rassor_chassis";
    m_color = ChColor(0.7f, 0.4f, 0.4f);
    CalcMassProperties(1650);
}

void RassorChassis::Initialize(ChSystem* system, const ChFrame<>& pos) {
    Construct(system);

    m_body->SetFrame_REF_to_abs(pos);
}

// =============================================================================

// Rassor Wheel
RassorWheel::RassorWheel(const std::string& name,
                         const ChFrame<>& rel_pos,
                         std::shared_ptr<ChMaterialSurface> mat,
                         RassorWheelType wheel_type)
    : RassorPart(name, rel_pos, mat, true) {
    switch (wheel_type) {
        case RassorWheelType::RealWheel:
            m_mesh_name = "rassor_wheel";
            m_wheel_type = RassorWheelType::RealWheel;
            break;
    }

    m_color = ChColor(0.4f, 0.7f, 0.4f);
    CalcMassProperties(1200);
}

// =============================================================================

// Rassor Razor
RassorRazor::RassorRazor(const std::string& name, const ChFrame<>& rel_pos, std::shared_ptr<ChMaterialSurface> mat)
    : RassorPart(name, rel_pos, mat, true) {
    m_mesh_name = "rassor_razor";
    m_color = ChColor(0.1f, 0.6f, 0.8f);
    CalcMassProperties(1200);
}
// =============================================================================

// Rassor Arm
RassorArm::RassorArm(const std::string& name, const ChFrame<>& rel_pos, std::shared_ptr<ChMaterialSurface> mat)
    : RassorPart(name, rel_pos, mat, false) {
    m_mesh_name = "rassor_arm";
    m_color = ChColor(0.6f, 0.6f, 0.5f);
    CalcMassProperties(1200);
}
// =============================================================================

// Rover model
Rassor::Rassor(ChSystem* system, RassorWheelType wheel_type) : m_system(system), m_chassis_fixed(false) {
    // Set default collision model envelope commensurate with model dimensions.
    // Note that an SMC system automatically sets envelope to 0.
    auto contact_method = m_system->GetContactMethod();

    // Create the contact materials
    m_default_material = DefaultContactMaterial(contact_method);
    m_wheel_material = DefaultContactMaterial(contact_method);

    Create(wheel_type);
}

void Rassor::Create(RassorWheelType wheel_type) {
    // create rover chassis
    m_chassis = chrono_types::make_shared<RassorChassis>("chassis", m_default_material);

    // initialize rover wheels
    double wx = 0.23961;
    double wy = 0.24394;
    double wz = -0.00136;

    m_wheels[RA_LF] = chrono_types::make_shared<RassorWheel>("Wheel_LF", ChFrame<>(ChVector<>(+wx, +wy, wz), QUNIT),
                                                             m_wheel_material, wheel_type);
    m_wheels[RA_RF] = chrono_types::make_shared<RassorWheel>("Wheel_RF", ChFrame<>(ChVector<>(+wx, -wy, wz), QUNIT),
                                                             m_wheel_material, wheel_type);
    m_wheels[RA_LB] = chrono_types::make_shared<RassorWheel>("Wheel_LB", ChFrame<>(ChVector<>(-wx, +wy, wz), QUNIT),
                                                             m_wheel_material, wheel_type);
    m_wheels[RA_RB] = chrono_types::make_shared<RassorWheel>("Wheel_RB", ChFrame<>(ChVector<>(-wx, -wy, wz), QUNIT),
                                                             m_wheel_material, wheel_type);

    m_wheels[RA_RF]->m_mesh_xform = ChFrame<>(VNULL, Q_from_AngZ(CH_C_PI));
    m_wheels[RA_RB]->m_mesh_xform = ChFrame<>(VNULL, Q_from_AngZ(CH_C_PI));

    // initialize rover razors
    double rx = 0.71883;
    double ry = 0.0;
    double rz = -0.00136;

    ChQuaternion<> z2z180;
    z2z180.Q_from_AngAxis(CH_C_PI, ChVector<>(0, 0, 1));

    m_razors[0] =
        chrono_types::make_shared<RassorRazor>("razor_F", ChFrame<>(ChVector<>(+rx, ry, rz), QUNIT), m_wheel_material);

    m_razors[1] =
        chrono_types::make_shared<RassorRazor>("razor_B", ChFrame<>(ChVector<>(-rx, ry, rz), z2z180), m_wheel_material);

    // initialize rover arms

    double ax = 0.25;
    double ay = 0.0;
    double az = 0.0;
    ChQuaternion<> y2y180;
    y2y180.Q_from_AngAxis(CH_C_PI, ChVector<>(0, 1, 0));
    m_arms[0] =
        chrono_types::make_shared<RassorArm>("arm_F", ChFrame<>(ChVector<>(+ax, ay, az), QUNIT), m_wheel_material);

    m_arms[1] =
        chrono_types::make_shared<RassorArm>("arm_B", ChFrame<>(ChVector<>(-ax, ay, az), y2y180), m_wheel_material);
}

void Rassor::Initialize(const ChFrame<>& pos) {
    assert(m_driver);

    m_chassis->Initialize(m_system, pos);
    m_chassis->GetBody()->SetBodyFixed(m_chassis_fixed);

    for (int i = 0; i < 4; i++) {
        m_wheels[i]->Initialize(m_chassis->GetBody());
    }

    for (int i = 0; i < 2; i++) {
        m_razors[i]->Initialize(m_chassis->GetBody());
        m_arms[i]->Initialize(m_chassis->GetBody());
    }

    // initialize drive motors
    double wx = 0.23961;
    double wy = 0.24394;
    double wz = -0.00136;

    std::vector<ChVector<>> drive_motor_rel_pos = {ChVector<>(+wx, +wy, wz), ChVector<>(+wx, -wy, wz),
                                                   ChVector<>(-wx, +wy, wz), ChVector<>(-wx, -wy, wz)};

    ChQuaternion<> z2y;
    z2y.Q_from_AngAxis(CH_C_PI / 2, ChVector<>(1, 0, 0));
    for (int i = 0; i < 4; i++) {
        m_drive_motor_funcs[i] = chrono_types::make_shared<ChFunction_Const>(0.5);
        m_drive_motors[i] =
            AddMotorSpeed(m_chassis->GetBody(), m_wheels[i]->GetBody(), m_chassis, drive_motor_rel_pos[i], z2y);
        m_drive_motors[i]->SetMotorFunction(m_drive_motor_funcs[i]);
    }

    // initialize the bottom motor for rassor arm
    double ax = 0.25;
    double ay = 0.0;
    double az = 0.0;
    std::vector<ChVector<>> arm_motor_rel_pos = {ChVector<>(+ax, ay, az), ChVector<>(-ax, ay, az)};
    for (int i = 0; i < 2; i++) {
        if (i == 0)
            m_arm_1_motor_funcs[i] = chrono_types::make_shared<ChFunction_Const>(-0.25);
        if (i == 1)
            m_arm_1_motor_funcs[i] = chrono_types::make_shared<ChFunction_Const>(0.25);
        m_arm_1_motors[i] =
            AddMotorSpeed(m_chassis->GetBody(), m_arms[i]->GetBody(), m_chassis, arm_motor_rel_pos[i], z2y);
        m_arm_1_motors[i]->SetMotorFunction(m_arm_1_motor_funcs[i]);
    }

    // intialize the top motor for rassor arm and the rassor rasor
    double rx = 0.71883;
    double ry = 0.0;
    double rz = -0.00136;
    std::vector<ChVector<>> arm_motor_rel_pos_2 = {ChVector<>(+rx, ry, rz), ChVector<>(-rx, ry, rz)};

    for (int i = 0; i < 2; i++) {
        m_arm_2_motor_funcs[i] = chrono_types::make_shared<ChFunction_Const>(0.25);
        m_arm_2_motors[i] =
            AddMotorSpeed(m_arms[i]->GetBody(), m_razors[i]->GetBody(), m_chassis, arm_motor_rel_pos_2[i], z2y);
        m_arm_2_motors[i]->SetMotorFunction(m_arm_2_motor_funcs[i]);
    }
}

void Rassor::SetDriver(std::shared_ptr<RassorDriver> driver) {
    m_driver = driver;
    m_driver->rassor = this;
}

void Rassor::SetWheelContactMaterial(std::shared_ptr<ChMaterialSurface> mat) {
    for (auto& wheel : m_wheels)
        wheel->m_mat = mat;
}

void Rassor::SetChassisFixed(bool fixed) {
    m_chassis_fixed = fixed;
}

ChVector<> Rassor::GetWheelContactForce(RassorWheelID id) const {
    return m_wheels[id]->GetBody()->GetContactForce();
}

ChVector<> Rassor::GetWheelContactTorque(RassorWheelID id) const {
    return m_wheels[id]->GetBody()->GetContactTorque();
}

ChVector<> Rassor::GetWheelAppliedForce(RassorWheelID id) const {
    return m_wheels[id]->GetBody()->GetAppliedForce();
}

ChVector<> Rassor::GetWheelAppliedTorque(RassorWheelID id) const {
    return m_wheels[id]->GetBody()->GetAppliedTorque();
}

double Rassor::GetWheelTracTorque(RassorWheelID id) const {
    if (m_driver->GetDriveMotorType() == RassorDriver::DriveMotorType::TORQUE)
        return 0;

    return m_drive_motors[id]->GetMotorTorque();
}

double Rassor::GetRoverMass() const {
    double tot_mass = m_chassis->GetBody()->GetMass();
    for (int i = 0; i < 4; i++) {
        tot_mass += m_wheels[i]->GetBody()->GetMass();
    }
    return tot_mass;
}

double Rassor::GetWheelMass() const {
    return m_wheels[0]->GetBody()->GetMass();
}

void Rassor::Update() {
    double time = m_system->GetChTime();
    m_driver->Update(time);

    for (int i = 0; i < 4; i++) {
        // Extract driver inputs
        double driving = m_driver->drive_speeds[i];

        // Set motor functions
        m_drive_motor_funcs[i]->Set_yconst(driving);
    }

    for (int i = 0; i < 2; i++) {
        double arm_speed = m_driver->arm_speeds[i];
        double razor_speed = m_driver->razor_speeds[i];
        m_arm_1_motor_funcs[i]->Set_yconst(arm_speed);
        m_arm_2_motor_funcs[i]->Set_yconst(razor_speed);
    }
}

// =============================================================================

RassorDriver::RassorDriver() : drive_speeds({0, 0, 0, 0}), arm_speeds({0, 0}), razor_speeds({0, 0}), rassor(nullptr) {}

RassorSpeedDriver::RassorSpeedDriver(double time_ramp) : m_ramp(time_ramp) {}

/// Set current drive motor speed input.
void RassorSpeedDriver::SetDriveMotorSpeed(RassorWheelID wheel_id, double drive_speed) {
    drive_speeds[wheel_id] = drive_speed;
}

/// Set current arm motor speed input.
void RassorSpeedDriver::SetArmMotorSpeed(RassorDirID dir_id, double arm_speed) {
    arm_speeds[dir_id] = arm_speed;
}

/// Set current razor motor speed input.
void RassorSpeedDriver::SetRazorMotorSpeed(RassorDirID dir_id, double razor_speed) {
    razor_speeds[dir_id] = razor_speed;
}

void RassorSpeedDriver::Update(double time) {}

}  // namespace rassor
}  // namespace chrono
