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
// Authors: Dario Mangoni, Radu Serban
// =============================================================================
//
// Chrono wrappers to fmu_tools FMU export classes for FMI standard 2.0.
//
// =============================================================================

#ifndef CH_FMU_TOOLS_EXPORT_H
#define CH_FMU_TOOLS_EXPORT_H

#include <stack>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "chrono/serialization/ChArchive.h"
#include "chrono/core/ChFrameMoving.h"
#include <memory>

#include "chrono/assets/ChVisualModel.h"
#include "chrono/assets/ChVisualShapes.h"

// fmu_tools
// #include "rapidxml_ext.hpp"
#include "fmi2/FmuToolsExport.h"

namespace chrono {

#define ADD_BVAL_AS_FMU_GETSET(returnType, codeGet, codeSet)                                         \
    _fmucomp->AddFmuVariable(                                                                        \
        std::make_pair(std::function<fmi2##returnType(void)>([bVal]() -> fmi2##returnType codeGet),  \
                       std::function<void(fmi2##returnType)>([bVal](fmi2##returnType val) codeSet)), \
        getCurrentVarName(bVal.name()), FmuVariable::Type::returnType, "", "",                       \
        CausalityType_conv.at(bVal.GetCausality()), VariabilityType_conv.at(bVal.GetVariability()));

#define ADD_BVAL_AS_FMU_POINTER(returnType)                                                                          \
    _fmucomp->AddFmuVariable(&(bVal.value()), getCurrentVarName(bVal.name()), FmuVariable::Type::returnType, "", "", \
                             CausalityType_conv.at(bVal.GetCausality()),                                             \
                             VariabilityType_conv.at(bVal.GetVariability()));

const std::unordered_map<chrono::ChVariabilityType, FmuVariable::VariabilityType> VariabilityType_conv = {
    {chrono::ChVariabilityType::constant, FmuVariable::VariabilityType::constant},
    {chrono::ChVariabilityType::fixed, FmuVariable::VariabilityType::fixed},
    {chrono::ChVariabilityType::tunable, FmuVariable::VariabilityType::tunable},
    {chrono::ChVariabilityType::discrete, FmuVariable::VariabilityType::discrete},
    {chrono::ChVariabilityType::continuous, FmuVariable::VariabilityType::continuous}};

const std::unordered_map<chrono::ChCausalityType, FmuVariable::CausalityType> CausalityType_conv = {
    {chrono::ChCausalityType::parameter, FmuVariable::CausalityType::parameter},
    {chrono::ChCausalityType::calculatedParameter, FmuVariable::CausalityType::calculatedParameter},
    {chrono::ChCausalityType::input, FmuVariable::CausalityType::input},
    {chrono::ChCausalityType::output, FmuVariable::CausalityType::output},
    {chrono::ChCausalityType::local, FmuVariable::CausalityType::local},
    {chrono::ChCausalityType::independent, FmuVariable::CausalityType::independent}};

// TODO expand serialization to have description

/// Class for serializing variables to FmuComponentBase.
class ChOutputFMU : public ChArchiveOut {
  public:
    ChOutputFMU(FmuComponentBase& fmucomp) {
        _fmucomp = &fmucomp;

        tablevel = 0;
        nitems.push(0);
        is_array.push_back(false);
    };

    virtual ~ChOutputFMU() {
        nitems.pop();
        is_array.pop_back();
    };

    virtual void out(ChNameValue<bool> bVal) {
        ADD_BVAL_AS_FMU_GETSET(
            Boolean, { return static_cast<fmi2Boolean>(bVal.value()); }, { bVal.value() = val; })

        ++nitems.top();
    }
    virtual void out(ChNameValue<int> bVal) {
        ADD_BVAL_AS_FMU_POINTER(Integer)

        ++nitems.top();
    }
    virtual void out(ChNameValue<double> bVal) {
        ADD_BVAL_AS_FMU_POINTER(Real)

        ++nitems.top();
    }
    virtual void out(ChNameValue<float> bVal) {
        ADD_BVAL_AS_FMU_GETSET(
            Real, { return static_cast<fmi2Real>(bVal.value()); }, { bVal.value() = static_cast<float>(val); })

        ++nitems.top();
    }
    virtual void out(ChNameValue<char> bVal) {
        ADD_BVAL_AS_FMU_GETSET(
            Integer, { return static_cast<fmi2Integer>(bVal.value()); }, { bVal.value() = val; })

        ++nitems.top();
    }
    virtual void out(ChNameValue<unsigned int> bVal) {
        ADD_BVAL_AS_FMU_GETSET(
            Integer, { return static_cast<fmi2Integer>(bVal.value()); }, { bVal.value() = val; })

        ++nitems.top();
    }
    virtual void out(ChNameValue<const char*> bVal) {
        // TODO

        ++nitems.top();
    }
    virtual void out(ChNameValue<std::string> bVal) {
        // TODO

        ++nitems.top();
    }
    virtual void out(ChNameValue<unsigned long> bVal) {
        // TODO

        ++nitems.top();
    }
    virtual void out(ChNameValue<unsigned long long> bVal) {
        // TODO

        ++nitems.top();
    }
    virtual void out(ChNameValue<ChEnumMapperBase> bVal) { ++nitems.top(); }

    virtual void out_array_pre(ChValue& bVal, size_t msize) {
        pushLevelName(bVal.name());

        ++tablevel;
        nitems.push(0);
        // signaling that, from now on, serialized variables are part of an array
        is_array.push_back(true);
    }
    virtual void out_array_between(ChValue& bVal, size_t msize) {}

    virtual void out_array_end(ChValue& bVal, size_t msize) {
        --tablevel;
        nitems.pop();
        is_array.pop_back();
        ++nitems.top();
        popLevelName();
    }

    // for custom c++ objects:
    virtual void out(ChValue& bVal, bool tracked, size_t obj_ID) {
        pushLevelName(bVal.name());

        ++tablevel;
        nitems.push(0);
        is_array.push_back(false);

        bVal.CallArchiveOut(*this);

        --tablevel;
        nitems.pop();
        is_array.pop_back();

        ++nitems.top();
        popLevelName();
    }

    virtual void out_ref(ChValue& bVal, bool already_inserted, size_t obj_ID, size_t ext_ID) {
        pushLevelName(bVal.name());

        ++tablevel;
        nitems.push(0);
        is_array.push_back(false);

        if (!already_inserted) {
            // New Object, we have to full serialize it
            bVal.CallArchiveOutConstructor(*this);
            bVal.CallArchiveOut(*this);
        }

        --tablevel;
        nitems.pop();

        is_array.pop_back();

        ++nitems.top();

        popLevelName();
    }

  protected:
    void pushLevelName(const std::string& newLevelName) { parent_names.push_back(newLevelName); }

    void popLevelName() { parent_names.pop_back(); }

    std::string getCurrentVarName(std::string bVal_name) {
        std::string current_fullname = "";
        if (parent_names.size() > 0)
            current_fullname = parent_names[0];

        for (size_t i = 1; i < parent_names.size(); ++i) {
            current_fullname += is_array.at(i) ? ("[" + parent_names[i] + "]") : "." + parent_names[i];
        }
        current_fullname += (is_array.back() ? ("[" + bVal_name + "]") : "." + bVal_name);

        return current_fullname;
    }

    int tablevel;
    FmuComponentBase* _fmucomp;
    std::stack<int> nitems;
    std::deque<bool> is_array;
    std::deque<std::string> parent_names;
};

/// Extension of FmuComponentBase class for Chrono FMUs.
class FmuChronoComponentBase : public FmuComponentBase {
  public:
    FmuChronoComponentBase(fmi2String instanceName,
                           fmi2Type fmuType,
                           fmi2String fmuGUID,
                           fmi2String fmuResourceLocation,
                           const fmi2CallbackFunctions* functions,
                           fmi2Boolean visible,
                           fmi2Boolean loggingOn)
        : FmuComponentBase(
              instanceName,
              fmuType,
              fmuGUID,
              fmuResourceLocation,
              functions,
              visible,
              loggingOn,
              {{"logEvents", true},
               {"logSingularLinearSystems", true},
               {"logNonlinearSystems", true},
               {"logStatusWarning", true},
               {"logStatusError", true},
               {"logStatusPending", true},
               {"logDynamicStateSelection", true},
               {"logStatusDiscard", true},
               {"logStatusFatal", true},
               {"logAll", true}},
              {"logStatusWarning", "logStatusDiscard", "logStatusError", "logStatusFatal", "logStatusPending"}),
          variables_serializer(*this) {}

    virtual ~FmuChronoComponentBase() {}

    /// Add FMU variables corresponding to the specified ChVector3d.
    /// This function creates 3 FMU variables, one for each component of the ChVector3d, with names "name.x", "name.y",
    /// and "name.z", all of type FmuVariable::Type::Real.
    void AddFmuVecVariable(ChVector3d& v,
                           const std::string& name,
                           const std::string& unit_name,
                           const std::string& description,
                           FmuVariable::CausalityType causality = FmuVariable::CausalityType::local,
                           FmuVariable::VariabilityType variability = FmuVariable::VariabilityType::continuous) {
        std::string comp[3] = {"x", "y", "z"};
        for (int i = 0; i < 3; i++) {
            AddFmuVariable(&v.data()[i], name + "." + comp[i], FmuVariable::Type::Real, unit_name,
                           description + " (" + comp[i] + ")", causality, variability);
        }
    }

    /// Add FMU variables corresponding to the specified ChQuaternion.
    /// This function creates 4 FMU variables, one for each component of the ChVector3d, with names "name.e0",
    /// "name.e1", "name.e2", and "name.e3", all of type FmuVariable::Type::Real.
    void AddFmuQuatVariable(ChQuaternion<>& q,
                            const std::string& name,
                            const std::string& unit_name,
                            const std::string& description,
                            FmuVariable::CausalityType causality = FmuVariable::CausalityType::local,
                            FmuVariable::VariabilityType variability = FmuVariable::VariabilityType::continuous) {
        std::string comp[4] = {"e0", "e1", "e2", "e3"};
        for (int i = 0; i < 4; i++) {
            AddFmuVariable(&q.data()[i], name + "." + comp[i], FmuVariable::Type::Real, unit_name,
                           description + " (" + comp[i] + ")", causality, variability);
        }
    }

    /// Add FMU variables corresponding to the specified ChCoordsys.
    /// This function creates 7 FMU variables, one for each component of the position ChVector3d and one for each
    /// component of the rotation quaternion, all of type FmuVariable::Type::Real.
    void AddFmuCsysVariable(ChCoordsysd& s,
                            const std::string& name,
                            const std::string& unit_name,
                            const std::string& description,
                            FmuVariable::CausalityType causality = FmuVariable::CausalityType::local,
                            FmuVariable::VariabilityType variability = FmuVariable::VariabilityType::continuous) {
        AddFmuVecVariable(s.pos, name + ".pos", unit_name, description + " position", causality, variability);
        AddFmuQuatVariable(s.rot, name + ".rot", "1", description + " orientation", causality, variability);
    }

    /// Add FMU variables corresponding to the specified ChFrame.
    /// This function creates 7 FMU variables, one for each component of the position ChVector3d and one for each
    /// component of the rotation quaternion, all of type FmuVariable::Type::Real.
    void AddFmuFrameVariable(ChFrame<>& s,
                             const std::string& name,
                             const std::string& unit_name,
                             const std::string& description,
                             FmuVariable::CausalityType causality = FmuVariable::CausalityType::local,
                             FmuVariable::VariabilityType variability = FmuVariable::VariabilityType::continuous) {
        AddFmuCsysVariable(s.m_csys, name, unit_name, description, causality, variability);
    }

    /// Add FMU variables corresponding to the specified ChFrameMoving.
    /// This function creates 7 FMU variables for the pose, one for each component of the position ChVector3d and one
    /// for each component of the rotation quaternion, all of type FmuVariable::Type::Real.  Additionally, 7 FMU
    /// variables are created to encode the position and orientation time derivatives.
    void AddFmuFrameMovingVariable(
        ChFrameMoving<>& s,
        const std::string& name,
        const std::string& unit_name,
        const std::string& unit_name_dt,
        const std::string& description,
        FmuVariable::CausalityType causality = FmuVariable::CausalityType::local,
        FmuVariable::VariabilityType variability = FmuVariable::VariabilityType::continuous) {
        AddFmuCsysVariable(s.m_csys, name, unit_name, description, causality, variability);
        AddFmuVecVariable(s.m_csys_dt.pos, name + ".pos_dt", unit_name_dt, description + " position derivative",
                          causality, variability);
        AddFmuQuatVariable(s.m_csys_dt.rot, name + ".rot_dt", "1", description + " orientation derivative", causality,
                           variability);
    }

    /// Add FMU variables corresponding to the visual shapes attached to the specified ChPhysicsItem.
    /// Variables with the following names are created for each visual shape:
    /// - VISUALIZER[i].frame.pos.[x|y|z]: position from global to shape local frame in global reference frame
    /// - VISUALIZER[i].frame.pos.[x|y|z]: position from global to shape local frame in global reference frame
    /// - VISUALIZER[i].frame.rot.[e0|e1|e2|e3]]: rotation from shape local to global frame
    /// - VISUALIZER[i].shape.type [fmi2String]: one of the supported_shape_types
    /// - VISUALIZER[i].owner [fmi2String]: owner of the shape, either custom_pi_name or from ChPhysicsItem::GetName()
    /// - VISUALIZER[i].shape.<shape-specific variables>
    /// Variables are of fmi2Real type if not otherwise specified.
    void AddFmuVisualShapes(ChPhysicsItem& pi, std::string custom_pi_name = "");

    /// Clear all visual shapes added with AddFmuVisualShapes.
    void ClearFmuVisualShapes() {
        visualizer_frames.clear();
        visualizers_counter = 0;
    }

  protected:
    /// add variables to the FMU component by leveraging the serialization mechanism
    chrono::ChOutputFMU variables_serializer;

    /// list of supported shapes for visualization, required to provide a memory position to getters of shape type
    static const std::unordered_set<std::string> supported_shape_types;

    using VisTuple = std::tuple<ChFrame<>, const ChPhysicsItem*, const ChFrame<>*, bool>;

    /// Map of visualization global frames.
    /// Includes:
    /// - visual shape frames (from shape local frame to global)
    /// - ChPhysicsItem to which they are bounded
    /// - (constant) ChFrame local to the shape
    /// - flag to tell if the frame has been already updated in the current step
    std::unordered_map<int, VisTuple> visualizer_frames;

    /// total number of visualizers (not unsigned to keep compatibility with FMU standard types)
    int visualizers_counter = 0;
};

void FmuChronoComponentBase::AddFmuVisualShapes(ChPhysicsItem& pi, std::string custom_pi_name) {
    for (auto& shape_inst : pi.GetVisualModel()->GetShapeInstances()) {
        // variables referring to visualizers will start with VISUALIZER[<counter>]
        // and will be split in .shape and .frame
        // the frame is NOT the one of the ShapeInstance (which is from shape to body)
        // but is a local frame that transforms from shape to world directly
        std::string shape_name = "VISUALIZER[" + std::to_string(visualizers_counter) + "].shape";
        variables_serializer << CHNVP(shape_inst.first, shape_name.c_str());
        std::string frame_basename = "VISUALIZER[" + std::to_string(visualizers_counter) + "].frame";

        auto update_frame = [](VisTuple& tup) {
            if (!std::get<3>(tup)) {
                std::get<0>(tup) = std::get<1>(tup)->GetVisualModelFrame() * (*std::get<2>(tup));
                std::get<3>(tup) = true;
            }
        };

        VisTuple current_tuple = std::make_tuple(ChFrame<>(), &pi, &shape_inst.second, false);
        update_frame(current_tuple);

        visualizer_frames[visualizers_counter] = current_tuple;

        static std::function<void(fmi2Real)> null_fmi2Real_setter = [](fmi2Real) {};

        // TODO: check if needed: the following functions must be binded to a fixed counter value, not to
        // this->visualizers_counter
        int visualizer_counter_current = visualizers_counter;

        // POS X
        std::function<fmi2Real(void)> pos_x_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetPos().x();
        };

        AddFmuVariable(std::make_pair(pos_x_getter, null_fmi2Real_setter), frame_basename + ".pos.x",
                       FmuVariable::Type::Real, "m", "global x position of shape");

        // POS Y
        std::function<fmi2Real(void)> pos_y_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetPos().y();
        };

        AddFmuVariable(std::make_pair(pos_y_getter, null_fmi2Real_setter), frame_basename + ".pos.y",
                       FmuVariable::Type::Real, "m", "global y position of shape");

        // POS Z
        std::function<fmi2Real(void)> pos_z_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetPos().z();
        };

        AddFmuVariable(std::make_pair(pos_z_getter, null_fmi2Real_setter), frame_basename + ".pos.z",
                       FmuVariable::Type::Real, "m", "global z position of shape");

        // ROT E0
        std::function<fmi2Real(void)> pos_e0_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetRot().e0();
        };

        AddFmuVariable(std::make_pair(pos_e0_getter, null_fmi2Real_setter), frame_basename + ".rot.e0",
                       FmuVariable::Type::Real, "", "shape quaternion component e0 from local to global");

        // ROT E1
        std::function<fmi2Real(void)> pos_e1_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetRot().e1();
        };

        AddFmuVariable(std::make_pair(pos_e1_getter, null_fmi2Real_setter), frame_basename + ".rot.e1",
                       FmuVariable::Type::Real, "", "shape quaternion component e1 from local to global");

        // ROT E2
        std::function<fmi2Real(void)> pos_e2_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetRot().e2();
        };

        AddFmuVariable(std::make_pair(pos_e2_getter, null_fmi2Real_setter), frame_basename + ".rot.e2",
                       FmuVariable::Type::Real, "", "shape quaternion component e2 from local to global");

        // ROT E3
        std::function<fmi2Real(void)> pos_e3_getter = [update_frame, visualizer_counter_current, this]() -> fmi2Real {
            VisTuple& tup = visualizer_frames[visualizer_counter_current];
            update_frame(tup);
            return std::get<0>(tup).GetRot().e3();
        };

        AddFmuVariable(std::make_pair(pos_e3_getter, null_fmi2Real_setter), frame_basename + ".rot.e3",
                       FmuVariable::Type::Real, "", "shape quaternion component e3 from local to global");

        // Set shape type

        std::shared_ptr<ChVisualShape> shape = shape_inst.first;
        auto shape_type = supported_shape_types.find("ChVisualShapeUNKNOWN");
        if (std::dynamic_pointer_cast<ChVisualShapeModelFile>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeModelFile");
        } else if (std::dynamic_pointer_cast<ChVisualShapeTriangleMesh>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeTriangleMesh");
        } else if (std::dynamic_pointer_cast<ChVisualShapeSurface>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeSurface");
        } else if (std::dynamic_pointer_cast<ChVisualShapeSphere>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeSphere");
        } else if (std::dynamic_pointer_cast<ChVisualShapeEllipsoid>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeEllipsoid");
        } else if (std::dynamic_pointer_cast<ChVisualShapeCylinder>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeCylinder");
        } else if (std::dynamic_pointer_cast<ChVisualShapeCapsule>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeCapsule");
        } else if (std::dynamic_pointer_cast<ChVisualShapeBox>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeBox");
        } else if (std::dynamic_pointer_cast<ChVisualShapeBarrel>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeBarrel");
        } else if (std::dynamic_pointer_cast<ChGlyphs>(shape)) {
            shape_type = supported_shape_types.find("ChGlyphs");
        } else if (std::dynamic_pointer_cast<ChVisualShapePath>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapePath");
        } else if (std::dynamic_pointer_cast<ChVisualShapeLine>(shape)) {
            shape_type = supported_shape_types.find("ChVisualShapeLine");
        } else {
            sendToLog("Unsupported shape type in ChPhysicsItem: " + pi.GetName(), fmi2Status::fmi2Warning,
                      "logStatusWarning");
        }

        AddFmuVariable(const_cast<std::string*>(&(*shape_type)), shape_name + ".type", FmuVariable::Type::String, "",
                       "shape type", FmuVariable::CausalityType::output, FmuVariable::VariabilityType::constant);

        static std::list<int> intbuf;
        intbuf.push_back(pi.GetIdentifier());
        AddFmuVariable(&intbuf.back(), shape_name + ".owner_id", FmuVariable::Type::Integer, "", "shape owner id",
                       FmuVariable::CausalityType::output, FmuVariable::VariabilityType::constant);

        static std::list<std::string> stringbuf;

        std::string* string_ptr;
        if (!custom_pi_name.empty() || !pi.GetName().empty()) {
            if (custom_pi_name.empty()) {
                if (pi.GetName().empty()) {
                    stringbuf.push_back("pi_" + std::to_string(pi.GetIdentifier()));
                    string_ptr = &stringbuf.back();
                } else
                    string_ptr = const_cast<std::string*>(&pi.GetName());
            } else {
                stringbuf.push_back(custom_pi_name);
                string_ptr = &stringbuf.back();
            }

            AddFmuVariable(string_ptr, shape_name + ".owner", FmuVariable::Type::String, "", "shape owner tag",
                           FmuVariable::CausalityType::output, FmuVariable::VariabilityType::constant);
        }

        variables_serializer << CHNVP(*shape.get(), shape_name);

        ++visualizers_counter;
    }
}

const std::unordered_set<std::string> FmuChronoComponentBase::supported_shape_types = {
    "ChVisualShapeModelFile", "ChVisualShapeTriangleMesh",
    "ChVisualShapeSurface",   "ChVisualShapeSphere",
    "ChVisualShapeEllipsoid", "ChVisualShapeCylinder",
    "ChVisualShapeCapsule",   "ChVisualShapeBox",
    "ChVisualShapeBarrel",    "ChGlyphs",
    "ChVisualShapePath",      "ChVisualShapeLine",
    "ChVisualShapeUNKNOWN"};

// -----------------------------------------------------------------------------

}  // end namespace chrono

#endif
