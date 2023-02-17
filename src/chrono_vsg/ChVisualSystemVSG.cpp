// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2022 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// Screen capture code from https://github.com/vsg-dev/vsgExamples.git
//
// =============================================================================
// Radu Serban, Rainer Gericke
// =============================================================================

#include <algorithm>
#include <cstddef>
#include <cctype>

#include "chrono_vsg/ChVisualSystemVSG.h"
#include "chrono_vsg/utils/ChConversionsVSG.h"
#include "chrono_vsg/utils/ChUtilsVSG.h"

namespace chrono {
namespace vsg3d {

using namespace std;

// -----------------------------------------------------------------------------

class GuiComponentWrapper {
  public:
    GuiComponentWrapper(std::shared_ptr<ChGuiComponentVSG> component, ChVisualSystemVSG* app)
        : m_component(component), m_app(app) {}

    bool operator()() {
        if (m_app->IsGuiVisible() && m_component->IsVisible()) {
            m_component->render();
            return true;
        }
        return false;
    }

  private:
    std::shared_ptr<ChGuiComponentVSG> m_component;
    ChVisualSystemVSG* m_app;
};

class ChBaseGuiComponentVSG : public ChGuiComponentVSG {
  public:
    ChBaseGuiComponentVSG(ChVisualSystemVSG* app) : m_app(app) {}

    // Example here taken from the Dear imgui comments (mostly)
    virtual void render() override {
        char label[64];
        int nstr = sizeof(label) - 1;
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ////ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
        ImGui::Begin("Simulation");

        if (ImGui::BeginTable("SimTable", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit,
                              ImVec2(0.0f, 0.0f))) {
            snprintf(label, nstr, "%8.3f s", m_app->GetSimulationTime());
            ImGui::TableNextColumn();
            ImGui::Text("Model Time:");
            ImGui::TableNextColumn();
            ImGui::Text(label);

            ImGui::TableNextRow();
            double current_time = double(clock()) / double(CLOCKS_PER_SEC);
            snprintf(label, nstr, "%8.3f s", current_time - m_app->m_start_time);
            ImGui::TableNextColumn();
            ImGui::Text("Wall Clock Time:");
            ImGui::TableNextColumn();
            ImGui::Text(label);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Real Time Factor:");
            ImGui::TableNextColumn();
            snprintf(label, nstr, "%8.3f", m_app->GetSimulationRTF());
            ImGui::Text(label);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Rendering FPS:");
            ImGui::TableNextColumn();
            snprintf(label, nstr, "%8.3f", m_app->GetRenderingFPS());
            ImGui::Text(label);

            ImGui::EndTable();
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("Frames", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit,
                              ImVec2(0.0f, 0.0f))) {
            ImGui::Text("COG frames:");
            ImGui::TableNextColumn();
            static bool bES_active = false;
            if (ImGui::Checkbox("COG", &bES_active))
                m_app->ToggleCOGFrameVisibility();
            ImGui::SameLine();
            float foo = m_app->m_cog_scale;
            ImGui::SliderFloat("scale", &foo, 0.1f, 10.0f);
            m_app->m_cog_scale = foo;
            ImGui::EndTable();
        }

        ImGui::Spacing();

        if (ImGui::Button("Quit"))
            m_app->Quit();

        ImGui::End();
    }

    ChVisualSystemVSG* m_app;
};

class ChCameraGuiComponentVSG : public ChGuiComponentVSG {
  public:
    ChCameraGuiComponentVSG(ChVisualSystemVSG* app) : m_app(app) { m_visible = false; }

    virtual void render() override {
        char label[64];
        int nstr = sizeof(label) - 1;

        auto p = m_app->GetCameraPosition();
        auto t = m_app->GetCameraTarget();

        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ////ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
        ImGui::Begin("Camera");

        if (ImGui::BeginTable("Location", 4, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit,
                              ImVec2(0.0f, 0.0f))) {
            ImGui::TableNextColumn();
            snprintf(label, nstr, "Location");
            ImGui::Text(label);
            for (int i = 0; i < 3; i++) {
                ImGui::TableNextColumn();
                snprintf(label, nstr, " %5.1f", p[i]);
                ImGui::Text(label);
            }

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            snprintf(label, nstr, "Look-at");
            ImGui::Text(label);
            for (int i = 0; i < 3; i++) {
                ImGui::TableNextColumn();
                snprintf(label, nstr, " %5.1f", t[i]);
                ImGui::Text(label);
            }

            ImGui::EndTable();

            ImGui::End();
        }
    }

    ChVisualSystemVSG* m_app;
};

class ChColorbarGuiComponentVSG : public ChGuiComponentVSG {
  public:
    ChColorbarGuiComponentVSG(const std::string& title, double min_val, double max_val)
        : m_title(title), m_min_val(min_val), m_max_val(max_val) {}

    //// RADU TODO
    ////   replace with a proper texture.
    ////   see https://github.com/libigl/libigl/issues/1388

    virtual void render() override {
        char label[64];
        int nstr = sizeof(label) - 1;

        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin(m_title.c_str());

        float alpha = 1.0f;
        float cv = 0.9f;
        float cv13 = cv / 3;
        float cv23 = 2 * cv13;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, cv, alpha));
        snprintf(label, nstr, "%.3f", m_min_val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        double stride = m_max_val - m_min_val;
        double val = m_min_val + stride * 1.0 / 6.0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, cv13, cv, alpha));
        snprintf(label, nstr, "%.3f", val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        val = m_min_val + stride * 2.0 / 6.0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, cv23, cv, alpha));
        snprintf(label, nstr, "%.3f", val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        val = m_min_val + 0.5 * stride;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, cv, 0.0, alpha));
        snprintf(label, nstr, "%.3f", val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        val = m_min_val + stride * 4.0 / 6.0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(cv, cv23, 0.0, alpha));
        snprintf(label, nstr - 1, "%.3f", val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        val = m_min_val + stride * 5.0 / 6.0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(cv, cv13, 0.0, alpha));
        snprintf(label, nstr, "%.3f", val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(cv, 0.0, 0.0, alpha));
        snprintf(label, nstr, "%.3f", m_max_val);
        ImGui::Button(label);
        ImGui::PopStyleColor(1);

        ImGui::End();
    }

  private:
    std::string m_title;
    double m_min_val;
    double m_max_val;
};

// -----------------------------------------------------------------------------

class EventHandlerWrapper : public vsg::Inherit<vsg::Visitor, EventHandlerWrapper> {
  public:
    EventHandlerWrapper(std::shared_ptr<ChEventHandlerVSG> component, ChVisualSystemVSG* app)
        : m_component(component), m_app(app) {}

    void apply(vsg::KeyPressEvent& keyPress) override { m_component->process(keyPress); }

  private:
    std::shared_ptr<ChEventHandlerVSG> m_component;
    ChVisualSystemVSG* m_app;
};

class ChBaseEventHandlerVSG : public ChEventHandlerVSG {
  public:
    ChBaseEventHandlerVSG(ChVisualSystemVSG* app) : m_app(app) {}

    virtual void process(vsg::KeyPressEvent& keyPress) override {
        if (keyPress.keyBase == 'm' || keyPress.keyModified == 'm') {
            m_app->ToggleGuiVisibility();
        }
        if (keyPress.keyBase == 'n' || keyPress.keyModified == 'n') {
            m_app->GetGuiComponent(m_app->m_camera_gui)->ToggleVisibility();
        }
        if (keyPress.keyBase == vsg::KEY_Escape || keyPress.keyModified == 65307) {
            m_app->Quit();
        }
    }

    ChVisualSystemVSG* m_app;
};

// -----------------------------------------------------------------------------

class FindVertexData : public vsg::Visitor {
  public:
    void apply(vsg::Object& object) { object.traverse(*this); }

    void apply(vsg::BindVertexBuffers& bvd) {
        if (bvd.arrays.empty())
            return;
        bvd.arrays[0]->data->accept(*this);
    }

    void apply(vsg::vec3Array& vertices) {
        if (verticesSet.count(&vertices) == 0) {
            verticesSet.insert(&vertices);
        }
    }

    std::vector<vsg::ref_ptr<vsg::vec3Array>> getVerticesList() {
        std::vector<vsg::ref_ptr<vsg::vec3Array>> verticesList(verticesSet.size());
        auto vertices_itr = verticesList.begin();
        for (auto& vertices : verticesSet) {
            (*vertices_itr++) = const_cast<vsg::vec3Array*>(vertices);
        }

        return verticesList;
    }

    std::set<vsg::vec3Array*> verticesSet;
};

class FindNormalData : public vsg::Visitor {
  public:
    void apply(vsg::Object& object) { object.traverse(*this); }

    void apply(vsg::BindVertexBuffers& bvd) {
        if (bvd.arrays.empty())
            return;
        bvd.arrays[1]->data->accept(*this);
    }

    void apply(vsg::vec3Array& normals) {
        if (normalsSet.count(&normals) == 0) {
            normalsSet.insert(&normals);
        }
    }

    std::vector<vsg::ref_ptr<vsg::vec3Array>> getNormalsList() {
        std::vector<vsg::ref_ptr<vsg::vec3Array>> normalsList(normalsSet.size());
        auto normals_itr = normalsList.begin();
        for (auto& normals : normalsSet) {
            (*normals_itr++) = const_cast<vsg::vec3Array*>(normals);
        }

        return normalsList;
    }

    std::set<vsg::vec3Array*> normalsSet;
};

class FindColorData : public vsg::Visitor {
  public:
    void apply(vsg::Object& object) { object.traverse(*this); }

    void apply(vsg::BindVertexBuffers& bvd) {
        if (bvd.arrays.empty())
            return;
        bvd.arrays[3]->data->accept(*this);
    }

    void apply(vsg::vec4Array& colors) {
        if (colorsSet.count(&colors) == 0) {
            colorsSet.insert(&colors);
        }
    }

    std::vector<vsg::ref_ptr<vsg::vec4Array>> getColorsList() {
        std::vector<vsg::ref_ptr<vsg::vec4Array>> colorsList(colorsSet.size());
        auto colors_itr = colorsList.begin();
        for (auto& colors : colorsSet) {
            (*colors_itr++) = const_cast<vsg::vec4Array*>(colors);
        }

        return colorsList;
    }

    std::set<vsg::vec4Array*> colorsSet;
};

// -----------------------------------------------------------------------------

struct Merge : public vsg::Inherit<vsg::Operation, Merge> {
    Merge(const vsg::Path& in_path,
          vsg::observer_ptr<vsg::Viewer> in_viewer,
          vsg::ref_ptr<vsg::Group> in_attachmentPoint,
          vsg::ref_ptr<vsg::Node> in_node,
          const vsg::CompileResult& in_compileResult)
        : path(in_path),
          viewer(in_viewer),
          attachmentPoint(in_attachmentPoint),
          node(in_node),
          compileResult(in_compileResult) {}

    vsg::Path path;
    vsg::observer_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Group> attachmentPoint;
    vsg::ref_ptr<vsg::Node> node;
    vsg::CompileResult compileResult;

    void run() override {
        // std::cout << "Merge::run() path = " << path << ", " << attachmentPoint << ", " << node << std::endl;

        vsg::ref_ptr<vsg::Viewer> ref_viewer = viewer;
        if (ref_viewer) {
            updateViewer(*ref_viewer, compileResult);
        }

        attachmentPoint->addChild(node);
    }
};

struct LoadOperation : public vsg::Inherit<vsg::Operation, LoadOperation> {
    LoadOperation(vsg::ref_ptr<vsg::Viewer> in_viewer,
                  vsg::ref_ptr<vsg::Group> in_attachmentPoint,
                  const vsg::Path& in_filename,
                  vsg::ref_ptr<vsg::Options> in_options)
        : viewer(in_viewer), attachmentPoint(in_attachmentPoint), filename(in_filename), options(in_options) {}

    vsg::observer_ptr<vsg::Viewer> viewer;
    vsg::ref_ptr<vsg::Group> attachmentPoint;
    vsg::Path filename;
    vsg::ref_ptr<vsg::Options> options;

    void run() override {
        vsg::ref_ptr<vsg::Viewer> ref_viewer = viewer;
        if (auto node = vsg::read_cast<vsg::Node>(filename, options)) {
            auto result = ref_viewer->compileManager->compile(node);
            if (result)
                ref_viewer->addUpdateOperation(Merge::create(filename, viewer, attachmentPoint, node, result));
        }
    }
};

// -----------------------------------------------------------------------------

ChVisualSystemVSG::ChVisualSystemVSG()
    : m_yup(false),
      m_useSkybox(false),
      m_capture_image(false),
      m_wireframe(false),
      m_show_gui(true),
      m_show_base_gui(true),
      m_camera_trackball(true),
      m_cog_scale(1),
      m_show_cog(false),
      m_frame_number(0),
      m_start_time(0),
      m_time_total(0),
      m_old_time(0),
      m_current_time(0),
      m_fps(0) {
    m_windowTitle = string("Window Title");
    m_clearColor = ChColor(0, 0, 0);
    m_skyboxPath = string("vsg/textures/chrono_skybox.ktx2");
    m_cameraUpVector = vsg::dvec3(0, 0, 1);

    // creation here allows to set entries before initialize
    m_bodyScene = vsg::Group::create();
    m_cogScene = vsg::Switch::create();
    m_linkScene = vsg::Group::create();
    m_particleScene = vsg::Group::create();
    m_decoScene = vsg::Group::create();
    m_deformableScene = vsg::Group::create();

    // set up defaults and read command line arguments to override them
    m_options = vsg::Options::create();
    m_options->paths = vsg::getEnvPaths("VSG_FILE_PATH");
    m_options->paths.push_back(GetChronoDataPath());

    // add vsgXchange's support for reading and writing 3rd party file formats, mandatory for chrono_vsg!
    m_options->add(vsgXchange::all::create());
    m_options->sharedObjects = vsg::SharedObjects::create();
    m_shapeBuilder = ShapeBuilder::create();
    m_shapeBuilder->m_options = m_options;
    m_shapeBuilder->m_sharedObjects = m_options->sharedObjects;
    m_vsgBuilder = vsg::Builder::create();
    m_vsgBuilder->options = m_options;

    // make some default settings
    SetWindowTitle("VSG: Vehicle Demo");
    SetWindowSize(ChVector2<int>(800, 600));
    SetWindowPosition(ChVector2<int>(50, 50));
    SetUseSkyBox(true);
    SetCameraAngleDeg(40);
    SetLightIntensity(1.0);
    SetLightDirection(1.5 * CH_C_PI_2, CH_C_PI_4);
#ifdef __APPLE__
    SetGuiFontSize(20.0);
#else
    SetGuiFontSize(10.0);
#endif
}

ChVisualSystemVSG::~ChVisualSystemVSG() {}

void ChVisualSystemVSG::SetOutputScreen(int screenNum) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    int maxNum = vsg::Device::maxNumDevices();
    GetLog() << "Screens found: " << maxNum << "\n";
    if (screenNum >= 0 && screenNum < maxNum) {
        m_screen_num = screenNum;
    } else {
        GetLog() << "Screen #" << screenNum << " cannot be used on this computer!\n";
        exit(1);
    }
}

void ChVisualSystemVSG::SetFullscreen(bool yesno) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_use_fullscreen = yesno;
}

size_t ChVisualSystemVSG::AddGuiComponent(std::shared_ptr<ChGuiComponentVSG> gc) {
    m_gui.push_back(gc);
    return m_gui.size() - 1;
}

size_t ChVisualSystemVSG::AddGuiColorbar(const std::string& title, double min_val, double max_val) {
    m_gui.push_back(chrono_types::make_shared<ChColorbarGuiComponentVSG>(title, min_val, max_val));
    return m_gui.size() - 1;
}

std::shared_ptr<ChGuiComponentVSG> ChVisualSystemVSG::GetGuiComponent(size_t id) {
    return m_gui.at(id);
}

void ChVisualSystemVSG::SetBaseGuiVisibility(bool show_gui) {
    m_show_base_gui = show_gui;
    if (m_initialized)
        m_base_gui->SetVisibility(m_show_base_gui);
}

void ChVisualSystemVSG::ToggleBaseGuiVisibility() {
    m_show_base_gui = !m_show_base_gui;
    if (m_initialized)
        m_base_gui->SetVisibility(m_show_base_gui);
}

void ChVisualSystemVSG::AddEventHandler(std::shared_ptr<ChEventHandlerVSG> eh) {
    m_evhandler.push_back(eh);
}

void ChVisualSystemVSG::Quit() {
    m_viewer->close();
}

void ChVisualSystemVSG::SetGuiFontSize(float theSize) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_guiFontSize = theSize;
}

void ChVisualSystemVSG::SetWindowSize(ChVector2<int> size) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_windowWidth = size[0];
    m_windowHeight = size[1];
}

void ChVisualSystemVSG::SetWindowSize(int width, int height) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_windowWidth = width;
    m_windowHeight = height;
}

void ChVisualSystemVSG::SetWindowPosition(ChVector2<int> pos) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_windowX = pos[0];
    m_windowY = pos[1];
}

void ChVisualSystemVSG::SetWindowPosition(int from_left, int from_top) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_windowX = from_left;
    m_windowY = from_top;
}

void ChVisualSystemVSG::SetWindowTitle(std::string title) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_windowTitle = title;
}

void ChVisualSystemVSG::SetClearColor(ChColor color) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_clearColor = color;
}

void ChVisualSystemVSG::SetUseSkyBox(bool yesno) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_useSkybox = yesno;
}

int ChVisualSystemVSG::AddCamera(const ChVector<>& pos, ChVector<> targ) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return -1;
    }

    ChVector<> test = pos - targ;
    if (test.Length() == 0.0) {
        GetLog() << "Function '" << __func__ << "' Camera Pos and Target cannot be identical!\n";
        GetLog() << "  pos    = { " << pos.x() << " ; " << pos.y() << " ; " << pos.z() << " }\n";
        GetLog() << "  target = { " << targ.x() << " ; " << targ.y() << " ; " << targ.z() << " }\n";
        exit(42);
    }
    if (m_yup) {
        if (pos.x() == 0.0 && pos.z() == 0.0) {
            GetLog() << "Function '" << __func__ << "' Line of sight is parallel to upvector! -> Corrected!!\n";
            m_vsg_cameraEye = vsg::dvec3(pos.x() + 1.0, pos.y(), pos.z() + 1.0);
        } else {
            m_vsg_cameraEye = vsg::dvec3(pos.x(), pos.y(), pos.z());
        }
    } else {
        if (pos.x() == 0.0 && pos.y() == 0.0) {
            GetLog() << "Function '" << __func__ << "' Line of sight is parallel to upvector! -> Corrected!!\n";
            m_vsg_cameraEye = vsg::dvec3(pos.x() + 1.0, pos.y() + 1.0, pos.z());
        } else {
            m_vsg_cameraEye = vsg::dvec3(pos.x(), pos.y(), pos.z());
        }
    }
    m_vsg_cameraTarget = vsg::dvec3(targ.x(), targ.y(), targ.z());

    return 0;
}

void ChVisualSystemVSG::SetCameraPosition(int id, const ChVector<>& pos) {
    m_lookAt->eye = vsg::dvec3(pos.x(), pos.y(), pos.z());
}

void ChVisualSystemVSG::SetCameraTarget(int id, const ChVector<>& target) {
    m_lookAt->center = vsg::dvec3(target.x(), target.y(), target.z());
}

void ChVisualSystemVSG::SetCameraPosition(const ChVector<>& pos) {
    m_lookAt->eye = vsg::dvec3(pos.x(), pos.y(), pos.z());
}

void ChVisualSystemVSG::SetCameraTarget(const ChVector<>& target) {
    m_lookAt->center = vsg::dvec3(target.x(), target.y(), target.z());
}

ChVector<> ChVisualSystemVSG::GetCameraPosition() const {
    auto p = m_lookAt->eye;
    return ChVector<>(p.x, p.y, p.z);
}

ChVector<> ChVisualSystemVSG::GetCameraTarget() const {
    auto p = m_lookAt->center;
    return ChVector<>(p.x, p.y, p.z);
}

void ChVisualSystemVSG::SetCameraVertical(CameraVerticalDir upDir) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    switch (upDir) {
        case CameraVerticalDir::Y:
            m_cameraUpVector = vsg::dvec3(0, 1, 0);
            m_yup = true;
            break;
        case CameraVerticalDir::Z:
            m_cameraUpVector = vsg::dvec3(0, 0, 1);
            m_yup = false;
            break;
    }
}

void ChVisualSystemVSG::SetLightIntensity(float intensity) {
    m_lightIntensity = ChClamp(intensity, 0.0f, 1.0f);
}

void ChVisualSystemVSG::SetLightDirection(double azimuth, double elevation) {
    if (m_initialized) {
        GetLog() << "Function '" << __func__ << "' must be used before initialization!\n";
        return;
    }
    m_azimuth = ChClamp(azimuth, -CH_C_PI, CH_C_PI);
    m_elevation = ChClamp(elevation, 0.0, CH_C_PI_2);
}

void ChVisualSystemVSG::Initialize() {
    auto builder = vsg::Builder::create();
    builder->options = m_options;

    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = m_windowTitle;
    windowTraits->width = m_windowWidth;
    windowTraits->height = m_windowHeight;
    windowTraits->x = m_windowX;
    windowTraits->y = m_windowY;
    windowTraits->debugLayer = false;
    windowTraits->deviceExtensionNames = {VK_KHR_MULTIVIEW_EXTENSION_NAME, VK_KHR_MAINTENANCE2_EXTENSION_NAME,
                                          VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                          VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME};
    windowTraits->swapchainPreferences.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    windowTraits->depthImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    windowTraits->fullscreen = m_use_fullscreen;
    windowTraits->screenNum = m_screen_num;

    m_scene = vsg::Group::create();

    double radius = 50.0;
    vsg::dbox bound;

    if (m_useSkybox) {
        vsg::Path fileName(m_skyboxPath);
        auto skyPtr = createSkybox(fileName, m_options, m_yup);
        if (skyPtr)
            m_scene->addChild(skyPtr);
        else
            m_useSkybox = false;
    }

    auto ambientLight = vsg::AmbientLight::create();
    ambientLight->name = "ambient";
    ambientLight->color.set(1.0f, 1.0f, 1.0f);
    ambientLight->intensity = 0.2f;

    auto directionalLight = vsg::DirectionalLight::create();
    directionalLight->name = "head light";
    directionalLight->color.set(1.0f, 1.0f, 1.0f);
    directionalLight->intensity = m_lightIntensity;
    double se = std::sin(m_elevation);
    double ce = std::cos(m_elevation);
    double sa = std::sin(m_azimuth);
    double ca = std::cos(m_azimuth);
    if (m_yup)
        directionalLight->direction.set(-ce * ca, -se, -ce * sa);
    else
        directionalLight->direction.set(-ce * ca, -ce * sa, -se);

    auto absoluteTransform = vsg::AbsoluteTransform::create();
    absoluteTransform->addChild(ambientLight);
    absoluteTransform->addChild(directionalLight);

    m_scene->addChild(absoluteTransform);
    m_scene->addChild(m_bodyScene);
    m_scene->addChild(m_cogScene);
    m_scene->addChild(m_linkScene);
    m_scene->addChild(m_particleScene);
    m_scene->addChild(m_decoScene);
    m_scene->addChild(m_deformableScene);

    BindAll();

    // create the viewer and assign window(s) to it
    m_viewer = vsg::Viewer::create();

    m_window = vsg::Window::create(windowTraits);
    if (!m_window) {
        std::cout << "Could not create window." << std::endl;
        return;
    }
    auto& limits = m_window->getOrCreatePhysicalDevice()->getProperties().limits;  // VkPhysicalDeviceLimits
    auto prop = m_window->getPhysicalDevice()->getProperties();
    GetLog() << "****************************************************\n";
    GetLog() << "* Chrono::VSG Vulkan Scene Graph 3D-Visualization\n";
    GetLog() << "* GPU Name: " << prop.deviceName << "\n";
    switch (prop.deviceType) {
        default:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            GetLog() << "* GPU Type: VK_PHYSICAL_DEVICE_TYPE_OTHER"
                     << "\n";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            GetLog() << "* GPU Type: VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU"
                     << "\n";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            GetLog() << "* GPU Type: VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU"
                     << "\n";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            GetLog() << "* GPU Type: VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU"
                     << "\n";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            GetLog() << "* GPU Type: VK_PHYSICAL_DEVICE_TYPE_CPU"
                     << "\n";
            break;
    }
    GetLog() << "* Vulkan Version: " << VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE) << "."
             << VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE) << "." << VK_API_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE)
             << "\n";
    GetLog() << "* Vulkan Scene Graph Version: " << VSG_VERSION_STRING << "\n";
    GetLog() << "* Graphic Output Possible on: " << vsg::Device::maxNumDevices() << " Screens.\n";
    GetLog() << "****************************************************\n";
    m_shapeBuilder->m_maxAnisotropy = limits.maxSamplerAnisotropy;
    m_window->clearColor() = VkClearColorValue{{m_clearColor.R, m_clearColor.G, m_clearColor.B, 1}};
    m_viewer->addWindow(m_window);

    // set up the camera
    m_lookAt = vsg::LookAt::create(m_vsg_cameraEye, m_vsg_cameraTarget, m_cameraUpVector);

    double nearFarRatio = 0.001;
    auto perspective = vsg::Perspective::create(
        m_cameraAngleDeg,
        static_cast<double>(m_window->extent2D().width) / static_cast<double>(m_window->extent2D().height),
        nearFarRatio * radius, radius * 10.0);

    m_vsg_camera = vsg::Camera::create(perspective, m_lookAt, vsg::ViewportState::create(m_window->extent2D()));

    // Add the base keyboard event handler
    auto base_kbhandler = chrono_types::make_shared<ChBaseEventHandlerVSG>(this);
    auto base_kbhandler_wrapper = EventHandlerWrapper::create(base_kbhandler, this);
    m_viewer->addEventHandler(base_kbhandler_wrapper);

    // Add all user-specified event handlers
    for (const auto& eh : m_evhandler) {
        auto evhandler_wrapper = EventHandlerWrapper::create(eh, this);
        m_viewer->addEventHandler(evhandler_wrapper);
    }

    // Add event handler for window close events
    m_viewer->addEventHandler(vsg::CloseHandler::create(m_viewer));

    // Add event handler for mouse camera view manipulation
    if (m_camera_trackball)
        m_viewer->addEventHandler(vsg::Trackball::create(m_vsg_camera));

    // default sets automatic directional light
    // auto renderGraph = vsg::RenderGraph::create(m_window, m_view);
    // switches off automatic directional light setting

    auto renderGraph =
        vsg::createRenderGraphForView(m_window, m_vsg_camera, m_scene, VK_SUBPASS_CONTENTS_INLINE, false);
    auto commandGraph = vsg::CommandGraph::create(m_window, renderGraph);

    // initialize ImGui
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = "../data/vsg/imgui.ini";

#ifdef __APPLE__
    // application runs on retina display by default (window 800*600 generates a viewport 1600*1200)
    // there can be reasons to switch to standard resolution
    // 1) there is no retina display
    // 2) standard resolution is demanded by MacOS (in our Irrlicht/VSG examples), set in the app bundle
    // in this case the desired font size is too big. We take the standard font instead.
    if (m_window->traits()->width != m_window->extent2D().width) {
#endif

        auto foundFontFile = vsg::findFile("vsg/fonts/Ubuntu_Mono/UbuntuMono-Regular.ttf", m_options);
        if (foundFontFile) {
            // convert native filename to UTF8 string that is compatible with ImuGUi.
            std::string c_fontFile = foundFontFile.string();

            // read the font via ImGui, which will then be current when vsgImGui::RenderImGui initializes the rest of
            // ImGui/Vulkan below
            ImGuiIO& io = ImGui::GetIO();
            auto imguiFont = io.Fonts->AddFontFromFileTTF(c_fontFile.c_str(), m_guiFontSize);
            if (!imguiFont) {
                std::cout << "Failed to load font: " << c_fontFile << std::endl;
                return;
            }
        }
#ifdef __APPLE__
    } else {
        // ignore loadable ttf font
        GetLog() << "App runs with standard resolution on the Mac. Font size setting ignored.\n";
    }
#endif

    // Include the base GUI component
    m_base_gui = chrono_types::make_shared<ChBaseGuiComponentVSG>(this);
    m_base_gui->SetVisibility(m_show_base_gui);
    GuiComponentWrapper base_gui_wrapper(m_base_gui, this);
    auto rg = vsgImGui::RenderImGui::create(m_window, base_gui_wrapper);

    // Add the camera info GUI component (initially invisible)
    m_camera_gui = AddGuiComponent(chrono_types::make_shared<ChCameraGuiComponentVSG>(this));

    // Loop through all specified GUI components, wrap them and add them to the renderGraph
    for (const auto& gui : m_gui) {
        GuiComponentWrapper gui_wrapper(gui, this);
        rg->add(gui_wrapper);
    }

    renderGraph->addChild(rg);

    // Add the ImGui event handler first to handle events early
    m_viewer->addEventHandler(vsgImGui::SendEventsToImGui::create());

    m_viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    // assign a CompileTraversal to the Builder that will compile for all the views assigned to the viewer,
    // must be done after Viewer.assignRecordAndSubmitTasksAndPresentations();
    auto compileTraversal = vsg::CompileTraversal::create(*m_viewer);
    m_shapeBuilder->assignCompileTraversal(compileTraversal);
    m_vsgBuilder->assignCompileTraversal(compileTraversal);
    vsg::ref_ptr<vsg::ResourceHints> resourceHints;
    if (!resourceHints) {
        // To help reduce the number of vsg::DescriptorPool that need to be allocated we'll provide a minimum
        // requirement via ResourceHints.
        resourceHints = vsg::ResourceHints::create();
        resourceHints->numDescriptorSets = 256;
        resourceHints->descriptorPoolSizes.push_back(
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256});
    }

    m_viewer->compile(resourceHints);

    // prepare reading 3d files
    m_loadThreads = vsg::OperationThreads::create(m_numThreads, m_viewer->status);

    m_initialized = true;
}

bool ChVisualSystemVSG::Run() {
    return m_viewer->active();
}

void ChVisualSystemVSG::Render() {
    if (m_frame_number == 0)
        m_start_time = double(clock()) / double(CLOCKS_PER_SEC);

    m_timer_render.reset();
    m_timer_render.start();

    UpdateFromMBS();

    if (!m_viewer->advanceToNextFrame()) {
        return;
    }

    // pass any events into EventHandlers assigned to the Viewer
    m_viewer->handleEvents();

    m_viewer->update();

    // Dynamic data transfer CPU -> GPU

    if (m_allowPositionTransfer) {
        // Point clouds
        for (const auto& pc : m_clouds) {
            if (!pc.dyn_pos)
                continue;
            auto& positions = m_cloud_positions[pc.start_pos];
            unsigned int k = 0;
            for (auto& p : *positions) {
                p = vsg::vec3CH(pc.pcloud->GetParticle(k).GetPos());
                k++;
            }
            positions->dirty();
        }
    }

    if (m_allowVertexTransfer) {
        for (auto& vertices : m_vsgVerticesList) {
            size_t k = 0;
            for (auto& v : *vertices) {
                v = vsg::vec3CH(m_mbsMesh->GetMesh()->getCoordsVertices()[k]);
                k++;
            }
            vertices->dirty();
        }
    }

    if (m_allowNormalsTransfer) {
        for (auto& normals : m_vsgNormalsList) {
            size_t k = 0;
            for (auto& n : *normals) {
                n = vsg::vec3CH(m_mbsMesh->GetMesh()->getCoordsNormals()[k]);
                k++;
            }
            normals->dirty();
        }
    }

    if (m_allowColorsTransfer) {
        // Point clouds
        for (const auto& pc : m_clouds) {
            if (!pc.dyn_col)
                continue;
            auto& colors = m_cloud_colors[pc.start_col];
            unsigned int k = 0;
            for (auto& c : *colors) {
                ChColor color = pc.pcloud->GetVisualColor(k);
                c.set(color.R, color.G, color.B, 1);
                k++;
            }
            colors->dirty();
        }

        // Color meshes
        for (auto& colors : m_vsgColorsList) {
            size_t k = 0;
            for (auto& c : *colors) {
                float r = m_mbsMesh->GetMesh()->getCoordsColors()[k].R;
                float g = m_mbsMesh->GetMesh()->getCoordsColors()[k].G;
                float b = m_mbsMesh->GetMesh()->getCoordsColors()[k].B;
                float a = 1.0f;
                c.set(r, g, b, a);
                k++;
            }
            colors->dirty();
        }
    }

    m_viewer->recordAndSubmit();

    if (m_capture_image) {
        exportScreenshot(m_window, m_options, m_imageFilename);
        m_capture_image = false;
    }

    m_viewer->present();
    m_frame_number++;

    m_timer_render.stop();
    m_time_total = .5 * m_timer_render() + .5 * m_time_total;
    m_current_time = m_time_total;
    m_current_time = m_current_time * 0.5 + m_old_time * 0.5;
    m_old_time = m_current_time;
    m_fps = 1.0 / m_current_time;
}

void ChVisualSystemVSG::RenderCOGFrames(double axis_length) {
    m_cog_scale = axis_length;
    m_show_cog = true;

    if (m_initialized) {
        for (auto& child : m_cogScene->children)
            child.mask = m_show_cog;
    }
}

void ChVisualSystemVSG::SetCOGFrameScale(double axis_length) {
    m_cog_scale = axis_length;
}

void ChVisualSystemVSG::ToggleCOGFrameVisibility() {
    m_show_cog = !m_show_cog;

    if (m_initialized) {
        for (auto& child : m_cogScene->children)
            child.mask = m_show_cog;
    }
}

void ChVisualSystemVSG::WriteImageToFile(const string& filename) {
    m_imageFilename = filename;
    m_capture_image = true;
}

// -----------------------------------------------------------------------------

// Utility function for creating a frame with its X axis defined by 2 points.
ChFrame<> PointPointFrame(const ChVector<>& P1, const ChVector<>& P2, double& dist) {
    ChVector<> dir = P2 - P1;
    dist = dir.Length();
    dir.Normalize();
    ChVector<> mx, my, mz;
    dir.DirToDxDyDz(my, mz, mx);
    ChMatrix33<> R_CS;
    R_CS.Set_A_axis(mx, my, mz);

    return ChFrame<>(0.5 * (P2 + P1), R_CS);
}

// Utility function to populate a VSG group with shape groups (from the given visual model).
// The visual model may or may not be associated with a Chrono physics item.
void ChVisualSystemVSG::PopulateGroup(vsg::ref_ptr<vsg::Group> group,
                                      std::shared_ptr<ChVisualModel> model,
                                      std::shared_ptr<ChPhysicsItem> phitem) {
    for (const auto& shape_instance : model->GetShapes()) {
        const auto& shape = shape_instance.first;
        const auto& X_SM = shape_instance.second;

        if (!shape->IsVisible())
            continue;

        // Material for primitive shapes (assumed at most one defined)
        std::shared_ptr<ChVisualMaterial> material =
            shape->GetMaterials().empty() ? ChVisualMaterial::Default() : shape->GetMaterial(0);

        if (auto box = std::dynamic_pointer_cast<ChBoxShape>(shape)) {
            ChVector<> scale = box->GetBoxGeometry().Size;
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, box->GetBoxGeometry().Size);

            // We have boxes and dice. Dice take cubetextures, boxes take 6 identical textures.
            // Use a die if a kd map exists and its name contains "cubetexture". Otherwise, use a box.
            auto grp = !material->GetKdTexture().empty() && material->GetKdTexture().find("cubetexture") != string::npos
                           ? m_shapeBuilder->createPbrShape(ShapeBuilder::DIE_SHAPE, material, transform, m_wireframe)
                           : m_shapeBuilder->createPbrShape(ShapeBuilder::BOX_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        } else if (auto sphere = std::dynamic_pointer_cast<ChSphereShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, sphere->GetSphereGeometry().rad);
            auto grp = m_shapeBuilder->createPbrShape(ShapeBuilder::SPHERE_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        } else if (auto ellipsoid = std::dynamic_pointer_cast<ChEllipsoidShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, ellipsoid->GetEllipsoidGeometry().rad);
            auto grp = m_shapeBuilder->createPbrShape(ShapeBuilder::SPHERE_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        } else if (auto capsule = std::dynamic_pointer_cast<ChCapsuleShape>(shape)) {
            double rad = capsule->GetCapsuleGeometry().rad;
            double height = capsule->GetCapsuleGeometry().hlen;
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, ChVector<>(rad, height, rad));
            auto grp = m_shapeBuilder->createPbrShape(ShapeBuilder::CAPSULE_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        } else if (auto barrel = std::dynamic_pointer_cast<ChBarrelShape>(shape)) {
            //// TODO
        } else if (auto cone = std::dynamic_pointer_cast<ChConeShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, cone->GetConeGeometry().rad);
            auto grp = m_shapeBuilder->createPbrShape(ShapeBuilder::CONE_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        } else if (auto trimesh = std::dynamic_pointer_cast<ChTriangleMeshShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, trimesh->GetScale());
            /*
            auto grp = trimesh->GetNumMaterials() > 0
                           ? m_shapeBuilder->createTrimeshPhongMatShape(transform, m_wireframe, trimesh)
                           : m_shapeBuilder->createTrimeshColShape(transform, m_wireframe, trimesh);
             */
            auto grp = trimesh->GetNumMaterials() > 0
                           ? m_shapeBuilder->createTrimeshPbrMatShape(transform, m_wireframe, trimesh)
                           : m_shapeBuilder->createTrimeshColShape(transform, m_wireframe, trimesh);
            group->addChild(grp);
        } else if (auto surface = std::dynamic_pointer_cast<ChSurfaceShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, 1.0);
            auto grp =
                m_shapeBuilder->createPbrShape(ShapeBuilder::SURFACE_SHAPE, material, transform, m_wireframe, surface);
            group->addChild(grp);
        } else if (auto obj = std::dynamic_pointer_cast<ChModelFileShape>(shape)) {
            string objFilename = obj->GetFilename();
            size_t objHashValue = m_stringHash(objFilename);
            auto grp = vsg::Group::create();
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(ChFrame<>(X_SM.GetPos(), X_SM.GetRot() * Q_from_AngX(-CH_C_PI_2)), 1.0);
            grp->addChild(transform);
            // needed, when BindAll() is called after Initialization
            // vsg::observer_ptr<vsg::Viewer> observer_viewer(m_viewer);
            // m_loadThreads->add(LoadOperation::create(observer_viewer, transform, objFilename, m_options));
            map<size_t, vsg::ref_ptr<vsg::Node>>::iterator objIt;
            objIt = m_objCache.find(objHashValue);
            if (objIt == m_objCache.end()) {
                auto node = vsg::read_cast<vsg::Node>(objFilename, m_options);
                if (node) {
                    transform->addChild(node);
                    group->addChild(grp);
                    m_objCache[objHashValue] = node;
                }
            } else {
                transform->addChild(m_objCache[objHashValue]);
                group->addChild(grp);
            }
        } else if (auto line = std::dynamic_pointer_cast<ChLineShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, 1.0);
            group->addChild(m_shapeBuilder->createLineShape(shape_instance, material, transform, line));
        } else if (auto path = std::dynamic_pointer_cast<ChPathShape>(shape)) {
            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_SM, 1.0);
            group->addChild(m_shapeBuilder->createPathShape(shape_instance, material, transform, path));
        } else if (auto cylinder = std::dynamic_pointer_cast<ChCylinderShape>(shape)) {
            double rad = cylinder->GetCylinderGeometry().rad;
            const auto& P1 = cylinder->GetCylinderGeometry().p1;
            const auto& P2 = cylinder->GetCylinderGeometry().p2;

            ChVector<> dir = P2 - P1;
            double height = dir.Length();
            dir.Normalize();
            ChVector<> mx, my, mz;
            dir.DirToDxDyDz(my, mz, mx);  // y is axis, in cylinder.obj frame
            ChMatrix33<> R_CS;
            R_CS.Set_A_axis(mx, my, mz);
            ChFrame<> X_CS(0.5 * (P2 + P1), R_CS);
            ChFrame<> X_CM = X_SM * X_CS;

            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::dmat4CH(X_CM, ChVector<>(rad, height, rad));
            auto grp = m_shapeBuilder->createPbrShape(ShapeBuilder::CYLINDER_SHAPE, material, transform, m_wireframe);
            group->addChild(grp);
        }

    }  // end loop over visual shapes
}

void ChVisualSystemVSG::BindAll() {
    for (auto sys : m_systems) {
        // Bind visual models associated with bodies in the system.
        // Traverse all bodies in the system and:
        //   - create a VSG node for the body COG frame
        //   - create the VSG node corresponding to the visual model
        for (const auto& body : sys->GetAssembly().Get_bodylist()) {
            // Create the COG node
            auto cog_transform = vsg::MatrixTransform::create();
            cog_transform->matrix = vsg::dmat4CH(body->GetFrame_COG_to_abs(), m_cog_scale);
            vsg::Mask mask = m_show_cog;
            auto cog_node = m_shapeBuilder->createCoGSymbol(cog_transform);
            cog_node->setValue("Body", body);
            cog_node->setValue("Transform", cog_transform);
            m_cogScene->addChild(mask, cog_node);

            const auto& vis_model = body->GetVisualModel();
            if (!vis_model)
                continue;

            // Important for update: keep the correct scenegraph hierarchy
            //     modelGroup->model_transform->shapes_group

            // Create a group to hold this visual model
            auto modelGroup = vsg::Group::create();

            // Create a group to hold the shapes with their subtransforms
            auto shapes_group = vsg::Group::create();

            // Populate the group with shapes in the visual model
            PopulateGroup(shapes_group, vis_model, body);

            // Attach a transform to the group and initialize it with the body current position
            auto model_transform = vsg::MatrixTransform::create();
            model_transform->matrix = vsg::dmat4CH(body->GetVisualModelFrame(), 1.0);
            model_transform->subgraphRequiresLocalFrustum = false;
            if (m_options->sharedObjects) {
                m_options->sharedObjects->share(modelGroup);
                m_options->sharedObjects->share(model_transform);
            }
            model_transform->addChild(shapes_group);
            modelGroup->addChild(model_transform);

            // Set group properties
            modelGroup->setValue("Body", body);
            modelGroup->setValue("Transform", model_transform);

            // Add the group to the global holder
            m_bodyScene->addChild(modelGroup);
        }  // end bodies

        // Bind visual models associated with other physics items in the system
        for (auto& item : sys->Get_otherphysicslist()) {
            if (auto pcloud = std::dynamic_pointer_cast<ChParticleCloud>(item)) {
                if (pcloud->GetVisualShapeType() == ChParticleCloud::ShapeType::NONE)
                    continue;

                auto shape = pcloud->GetVisualShapeType();
                const auto& size = pcloud->GetVisualSize();

                // Create an new entry in the set of Chrono::VSG particle clouds
                int start_pos = -1;
                int start_col = -1;
                if (!m_clouds.empty()) {
                    if (pcloud->IsActive())
                        start_pos = m_clouds.back().start_pos + 1;
                    else
                        start_pos = m_clouds.back().start_pos;
                    if (pcloud->UseDynamicColors())
                        start_col = m_clouds.back().start_col + 1;
                    else
                        start_col = m_clouds.back().start_col;
                } else {
                    if (pcloud->IsActive())
                        start_pos = 0;
                    if (pcloud->UseDynamicColors())
                        start_col = 0;
                }

                ParticleCloud cloud;
                cloud.pcloud = pcloud;
                cloud.num_particles = pcloud->GetNparticles();
                cloud.dyn_pos = pcloud->IsActive();
                cloud.dyn_col = pcloud->UseDynamicColors();
                cloud.start_pos = start_pos;
                cloud.start_col = start_col;

                // Set up geometry and state info for vsgBuilder
                vsg::GeometryInfo geomInfo;
                geomInfo.dx.set((float)size.x(), 0, 0);
                geomInfo.dy.set(0, (float)size.y(), 0);
                geomInfo.dz.set(0, 0, (float)size.z());

                if (cloud.dyn_col) {
                    auto colors = vsg::vec4Array::create(cloud.num_particles);
                    geomInfo.colors = colors;
                    for (size_t k = 0; k < cloud.num_particles; k++)
                        colors->set(k, vsg::vec4(0, 0, 0, 1));
                    colors->properties.dataVariance = vsg::DYNAMIC_DATA;
                    m_cloud_colors.push_back(colors);
                    m_allowColorsTransfer = true;
                } else {
                    const auto& color = pcloud->GetVisualColor(0);
                    geomInfo.color.set(color.R, color.G, color.B, 1.0);
                }

                auto positions = vsg::vec3Array::create(cloud.num_particles);
                geomInfo.positions = positions;
                for (size_t k = 0; k < cloud.num_particles; k++)
                    positions->set(k, vsg::vec3CH(pcloud->GetParticle(k).GetPos()));
                if (cloud.dyn_pos) {
                    positions->properties.dataVariance = vsg::DYNAMIC_DATA;
                    m_cloud_positions.push_back(positions);
                    m_allowPositionTransfer = true;
                }

                vsg::StateInfo stateInfo;
                stateInfo.wireframe = m_wireframe;
                stateInfo.instance_positions_vec3 = true;

                // Add child node for this cloud
                switch (shape) {
                    case ChParticleCloud::ShapeType::SPHERE:
                    case ChParticleCloud::ShapeType::ELLIPSOID:
                        m_particleScene->addChild(m_vsgBuilder->createSphere(geomInfo, stateInfo));
                        break;
                    case ChParticleCloud::ShapeType::BOX:
                        m_particleScene->addChild(m_vsgBuilder->createBox(geomInfo, stateInfo));
                        break;
                    case ChParticleCloud::ShapeType::CAPSULE:
                        m_particleScene->addChild(m_vsgBuilder->createCapsule(geomInfo, stateInfo));
                        break;
                    case ChParticleCloud::ShapeType::CYLINDER:
                        m_particleScene->addChild(m_vsgBuilder->createCylinder(geomInfo, stateInfo));
                        break;
                    case ChParticleCloud::ShapeType::CONE:
                        m_particleScene->addChild(m_vsgBuilder->createCone(geomInfo, stateInfo));
                        break;
                }

                m_clouds.push_back(cloud);

            } else if (auto loadcont = std::dynamic_pointer_cast<ChLoadContainer>(item)) {
                auto visModel = loadcont->GetVisualModel();
                if (!visModel)
                    continue;
                const auto& shape_instance = visModel->GetShapes().at(0);
                auto& shape = shape_instance.first;
                auto trimesh = std::dynamic_pointer_cast<ChTriangleMeshShape>(shape);
                if (!trimesh)
                    continue;
                auto transform = vsg::MatrixTransform::create();
                if (trimesh->GetNumMaterials() > 0) {
                    /*
                    m_deformableScene->addChild(
                        m_shapeBuilder->createTrimeshPhongMatShape(transform, trimesh->IsWireframe(), trimesh));
                     */
                    m_deformableScene->addChild(
                        m_shapeBuilder->createTrimeshPbrMatShape(transform, trimesh->IsWireframe(), trimesh));
                } else {
                    m_deformableScene->addChild(
                        m_shapeBuilder->createTrimeshColShapeSCM(transform, trimesh->IsWireframe(), trimesh));
                }
                m_vsgVerticesList = vsg::visit<FindVertexData>(m_deformableScene->children.at(0)).getVerticesList();
                for (auto& vertices : m_vsgVerticesList) {
                    vertices->properties.dataVariance = vsg::DYNAMIC_DATA;
                    m_num_vsgVertexList += vertices->size();
                }
                m_mbsMesh = trimesh;
                if (m_num_vsgVertexList == trimesh->GetMesh()->getCoordsVertices().size()) {
                    m_allowVertexTransfer = true;
                }
                if (m_allowVertexTransfer && !trimesh->IsWireframe()) {
                    m_vsgNormalsList = vsg::visit<FindNormalData>(m_deformableScene->children.at(0)).getNormalsList();
                    size_t num_vsgNormalsList = 0;
                    for (auto& normals : m_vsgNormalsList) {
                        normals->properties.dataVariance = vsg::DYNAMIC_DATA;
                        num_vsgNormalsList += normals->size();
                    }
                    if (num_vsgNormalsList == m_num_vsgVertexList) {
                        m_allowNormalsTransfer = true;
                    }
                }
                if (m_allowVertexTransfer) {
                    m_vsgColorsList = vsg::visit<FindColorData>(m_deformableScene->children.at(0)).getColorsList();
                    size_t num_vsgColorsList = 0;
                    for (auto& colors : m_vsgColorsList) {
                        colors->properties.dataVariance = vsg::DYNAMIC_DATA;
                        num_vsgColorsList += colors->size();
                    }
                    if (num_vsgColorsList == m_num_vsgVertexList) {
                        m_allowColorsTransfer = true;
                    }
                }
            }
        }  // end other_physics

        // Bind visual models associated with links in the system
        for (auto link : sys->Get_linklist()) {
            auto vis_model = link->GetVisualModel();
            if (!vis_model)
                continue;
            if (vis_model->GetNumShapes() == 0)
                continue;

            if (auto tsda = std::dynamic_pointer_cast<ChLinkTSDA>(link)) {
                for (auto& shape_instance : tsda->GetVisualModel()->GetShapes()) {
                    auto& shape = shape_instance.first;
                    if (auto segshape = std::dynamic_pointer_cast<ChSegmentShape>(shape)) {
                        double length;
                        auto X = PointPointFrame(tsda->GetPoint1Abs(), tsda->GetPoint2Abs(), length);
                        std::shared_ptr<ChVisualMaterial> material =
                            shape->GetMaterials().empty() ? ChVisualMaterial::Default() : shape->GetMaterial(0);

                        auto transform = vsg::MatrixTransform::create();
                        transform->matrix = vsg::dmat4CH(X, ChVector<>(0, length, 0));
                        m_linkScene->addChild(
                            m_shapeBuilder->createUnitSegment(link, shape_instance, material, transform));
                    } else if (auto sprshape = std::dynamic_pointer_cast<ChSpringShape>(shape)) {
                        double rad = sprshape->GetRadius();
                        double length;
                        auto X = PointPointFrame(tsda->GetPoint1Abs(), tsda->GetPoint2Abs(), length);
                        std::shared_ptr<ChVisualMaterial> material =
                            shape->GetMaterials().empty() ? ChVisualMaterial::Default() : shape->GetMaterial(0);

                        auto transform = vsg::MatrixTransform::create();
                        transform->matrix = vsg::dmat4CH(X, ChVector<>(rad, length, rad));
                        m_linkScene->addChild(
                            m_shapeBuilder->createSpringShape(link, shape_instance, material, transform, sprshape));
                    }
                }
            } else if (auto dist = std::dynamic_pointer_cast<ChLinkDistance>(link)) {
                for (auto& shape_instance : dist->GetVisualModel()->GetShapes()) {
                    auto& shape = shape_instance.first;
                    if (auto segshape = std::dynamic_pointer_cast<ChSegmentShape>(shape)) {
                        double length;
                        auto X = PointPointFrame(dist->GetEndPoint1Abs(), dist->GetEndPoint2Abs(), length);
                        std::shared_ptr<ChVisualMaterial> material =
                            shape->GetMaterials().empty() ? ChVisualMaterial::Default() : shape->GetMaterial(0);

                        auto transform = vsg::MatrixTransform::create();
                        transform->matrix = vsg::dmat4CH(X, ChVector<>(0, length, 0));
                        m_linkScene->addChild(
                            m_shapeBuilder->createUnitSegment(link, shape_instance, material, transform));
                    }
                }
            }
        }  // end loop over links

    }  // end loop over systems
}

void ChVisualSystemVSG::UpdateFromMBS() {
    // Update VSG nodes for body COG visualization
    if (m_show_cog) {
        for (auto& child : m_cogScene->children) {
            std::shared_ptr<ChBody> body;
            vsg::ref_ptr<vsg::MatrixTransform> transform;
            if (!child.node->getValue("Body", body))
                continue;
            if (!child.node->getValue("Transform", transform))
                continue;
            transform->matrix = vsg::dmat4CH(body->GetFrame_COG_to_abs(), m_cog_scale);
        }
    }

    // Update VSG nodes for body visualization
    for (const auto& child : m_bodyScene->children) {
        std::shared_ptr<ChBody> body;
        vsg::ref_ptr<vsg::MatrixTransform> transform;
        if (!child->getValue("Body", body))
            continue;
        if (!child->getValue("Transform", transform))
            continue;
        transform->matrix = vsg::dmat4CH(body->GetVisualModelFrame(), 1.0);
    }

    /*
    for (auto& item : m_systems[0]->Get_otherphysicslist()) {
        if (auto pcloud = std::dynamic_pointer_cast<ChParticleCloud>(item)) {
            // Update VSG node for particle visualization, now moved to Render()
        }
    }
    */

    // Update VSG nodes for link visualization
    for (const auto& child : m_linkScene->children) {
        std::shared_ptr<ChLinkBase> link;
        ChVisualModel::ShapeInstance shapeInstance;
        vsg::ref_ptr<vsg::MatrixTransform> transform;
        if (!child->getValue("Link", link))
            continue;
        if (!child->getValue("ShapeInstance", shapeInstance))
            continue;
        if (!child->getValue("Transform", transform))
            continue;

        if (!link->GetVisualModel())
            continue;

        auto& shape = shapeInstance.first;

        if (auto tsda = std::dynamic_pointer_cast<ChLinkTSDA>(link)) {
            if (auto segshape = std::dynamic_pointer_cast<ChSegmentShape>(shape)) {
                double length;
                auto X = PointPointFrame(tsda->GetPoint1Abs(), tsda->GetPoint2Abs(), length);
                transform->matrix = vsg::dmat4CH(X, ChVector<>(0, length, 0));
            } else if (auto sprshape = std::dynamic_pointer_cast<ChSpringShape>(shape)) {
                double rad = sprshape->GetRadius();
                double length;
                auto X = PointPointFrame(tsda->GetPoint1Abs(), tsda->GetPoint2Abs(), length);
                transform->matrix = vsg::dmat4CH(X, ChVector<>(rad, length, rad));
            }
        } else if (auto dist = std::dynamic_pointer_cast<ChLinkDistance>(link)) {
            if (auto segshape = std::dynamic_pointer_cast<ChSegmentShape>(shape)) {
                double length;
                auto X = PointPointFrame(dist->GetEndPoint1Abs(), dist->GetEndPoint2Abs(), length);
                transform->matrix = vsg::dmat4CH(X, ChVector<>(0, length, 0));
            }
        }
    }
}

void ChVisualSystemVSG::OnSetup(ChSystem* sys) {
    //// RADU TODO
    ////    delete VSG elements associated with physics items no longer present in the system
}

int ChVisualSystemVSG::AddVisualModel(std::shared_ptr<ChVisualModel> model, const ChFrame<>& frame) {
    // Important for update: keep the correct scenegraph hierarchy
    //     model_group->model_transform->shapes_group

    // Create a group to hold this visual model
    auto model_group = vsg::Group::create();

    // Create a group to hold the shapes with their subtransforms
    auto shapes_group = vsg::Group::create();

    // Populate the group with shapes in the visual model
    PopulateGroup(shapes_group, model, nullptr);

    // Attach a transform to the group and initialize it with the provided frame
    auto model_transform = vsg::MatrixTransform::create();
    model_transform->matrix = vsg::dmat4CH(frame, 1.0);
    model_transform->subgraphRequiresLocalFrustum = false;
    if (m_options->sharedObjects) {
        m_options->sharedObjects->share(model_group);
        m_options->sharedObjects->share(model_transform);
    }
    model_transform->addChild(shapes_group);
    model_group->addChild(model_transform);

    // Set group properties
    model_group->setValue("Transform", model_transform);

    // Add the group to the global holder
    m_decoScene->addChild(model_group);

    return m_decoScene->children.size() - 1;
}

int ChVisualSystemVSG::AddVisualModel(std::shared_ptr<ChVisualShape> shape, const ChFrame<>& frame) {
    auto model = chrono_types::make_shared<ChVisualModel>();
    model->AddShape(shape);
    return AddVisualModel(model, frame);
}

void ChVisualSystemVSG::UpdateVisualModel(int id, const ChFrame<>& frame) {
    if (id == -1 || id >= m_decoScene->children.size()) {
        return;
    }

    auto model_group = m_decoScene->children[id];
    vsg::ref_ptr<vsg::MatrixTransform> transform;
    if (!model_group->getValue("Transform", transform))
        return;

    transform->matrix = vsg::dmat4CH(frame, 1.0);
}

// -----------------------------------------------------------------------------

void ChVisualSystemVSG::AddGrid(double ustep, double vstep, int nu, int nv, ChCoordsys<> pos, ChColor col) {
    m_decoScene->addChild(m_shapeBuilder->createDecoGrid(ustep, vstep, nu, nv, pos, col));
}

}  // namespace vsg3d
}  // namespace chrono
