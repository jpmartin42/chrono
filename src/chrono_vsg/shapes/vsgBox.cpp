#include "chrono_vsg/shapes/vsgBox.h"

using namespace chrono::vsg3d;

vsgBox::vsgBox() {}

void vsgBox::compile(vsg::ref_ptr<vsg::Node> subgraph) {
    // std::cout << "Builder::compile(" << subgraph << ") _compile = " << _compile << std::endl;
    if (_compile) {
        subgraph->accept(*_compile);
        _compile->context.record();
        _compile->context.waitForCompletion();
    }
}

vsg::ref_ptr<vsg::Node> vsgBox::createTexturedNode(vsg::vec3 size,
                                                   vsg::vec4 color,
                                                   vsg::ref_ptr<vsg::MatrixTransform> transform) {
    // set up search paths to SPIRV shaders and textures
    vsg::Paths searchPaths = vsg::getEnvPaths("VSG_FILE_PATH");

    vsg::ref_ptr<vsg::ShaderStage> vertexShader = vsg::ShaderStage::read(
        VK_SHADER_STAGE_VERTEX_BIT, "main", vsg::findFile("shaders/vert_PushConstants.spv", searchPaths));
    vsg::ref_ptr<vsg::ShaderStage> fragmentShader = vsg::ShaderStage::read(
        VK_SHADER_STAGE_FRAGMENT_BIT, "main", vsg::findFile("shaders/frag_PushConstants.spv", searchPaths));
    if (!vertexShader || !fragmentShader) {
        std::cout << "Could not create shaders." << std::endl;
        return {};
    }

    // read texture image
    vsg::Path textureFile("vsg/textures/Metal010.vsgb");
    auto textureData = vsg::read_cast<vsg::Data>(vsg::findFile(textureFile, searchPaths));

    if (!textureData) {
        std::cout << "Could not read texture file : " << textureFile << ", we replace it by an default image."
                  << std::endl;
        auto image = vsg::vec4Array2D::create(2, 2, color, vsg::Data::Layout{VK_FORMAT_R32G32B32A32_SFLOAT});
        image->set(0, 0, {0.0f, 0.0f, 1.0f, 1.0f});
        image->set(1, 1, {0.0f, 0.0f, 1.0f, 1.0f});
        textureData = image;
    }

    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
         nullptr}  // { binding, descriptorTpe, descriptorCount, stageFlags, pImmutableSamplers}
    };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create(descriptorBindings);
    vsg::DescriptorSetLayouts descriptorSetLayouts{descriptorSetLayout};

    vsg::PushConstantRanges pushConstantRanges{
        {VK_SHADER_STAGE_VERTEX_BIT, 0, 128}  // projection view, and model matrices, actual push constant calls
                                              // autoaatically provided by the VSG's DispatchTraversal
    };

    auto pipelineLayout = vsg::PipelineLayout::create(descriptorSetLayouts, pushConstantRanges);

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{
        VkVertexInputBindingDescription{0, sizeof(vsg::vec3), VK_VERTEX_INPUT_RATE_VERTEX},  // vertex data
        VkVertexInputBindingDescription{1, sizeof(vsg::vec4), VK_VERTEX_INPUT_RATE_VERTEX},  // colour data
        VkVertexInputBindingDescription{2, sizeof(vsg::vec2), VK_VERTEX_INPUT_RATE_VERTEX}   // tex coord data
    };

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{
        VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},     // vertex data
        VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32B32A32_SFLOAT, 0},  // colour data
        VkVertexInputAttributeDescription{2, 2, VK_FORMAT_R32G32_SFLOAT, 0},        // tex coord data
    };

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create(vertexBindingsDescriptions, vertexAttributeDescriptions),
        vsg::InputAssemblyState::create(),
        vsg::RasterizationState::create(),
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create()};

    auto graphicsPipeline =
        vsg::GraphicsPipeline::create(pipelineLayout, vsg::ShaderStages{vertexShader, fragmentShader}, pipelineStates);
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create(graphicsPipeline);

    // create texture image and associated DescriptorSets and binding
    auto texture = vsg::DescriptorImage::create(vsg::Sampler::create(), textureData, 0, 0,
                                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    auto descriptorSet = vsg::DescriptorSet::create(descriptorSetLayout, vsg::Descriptors{texture});
    auto bindDescriptorSets = vsg::BindDescriptorSets::create(
        VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipelineLayout(), 0, vsg::DescriptorSets{descriptorSet});

    // create StateGroup as the root of the scene/command graph to hold the GraphicsProgram, and binding of Descriptors
    // to decorate the whole graph
    auto scenegraph = vsg::StateGroup::create();
    scenegraph->add(bindGraphicsPipeline);
    scenegraph->add(bindDescriptorSets);

    vsg::vec3 position = {0.0, 0.0, 0.0};
    vsg::vec3 v000(position);
    vsg::vec3 v100(position + vsg::vec3(size.x, 0.0f, 0.0f));
    vsg::vec3 v110(position + vsg::vec3(size.x, size.y, 0.0f));
    vsg::vec3 v010(position + vsg::vec3(0.0f, size.y, 0.0f));
    vsg::vec3 v001(position + vsg::vec3(0.0f, 0.0f, size.z));
    vsg::vec3 v101(position + vsg::vec3(size.x, 0.0f, size.z));
    vsg::vec3 v111(position + vsg::vec3(size.x, size.y, size.z));
    vsg::vec3 v011(position + vsg::vec3(0.0f, size.y, size.z));

    // set up vertex and index arrays
    auto vertices = vsg::vec3Array::create({v000, v100, v101, v001, v100, v110, v111, v101, v110,
                                            v010, v011, v111, v010, v000, v001, v011, v010, v110,
                                            v100, v000, v001, v101, v111, v011});  // VK_FORMAT_R32G32B32_SFLOAT,
                                                                                   // VK_VERTEX_INPUT_RATE_INSTANCE,
                                                                                   // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                   // VK_SHARING_MODE_EXCLUSIVE

    auto colors = vsg::vec3Array::create(vertices->size(), vsg::vec3(1.0f, 1.0f, 1.0f));
    // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    // VK_SHARING_MODE_EXCLUSIVE

#if 0
    vsg::vec3 n0(0.0f, -1.0f, 0.0f);
    vsg::vec3 n1(1.0f, 0.0f, 0.0f);
    vsg::vec3 n2(0.0f, 1.0f, 0.0f);
    vsg::vec3 n3(0.0f, -1.0f, 0.0f);
    vsg::vec3 n4(0.0f, 0.0f, -1.0f);
    vsg::vec3 n5(0.0f, 0.0f, 1.0f);
    auto normals = vsg::vec3Array::create(
    {
        n0, n0, n0, n0,
        n1, n1, n1, n1,
        n2, n2, n2, n2,
        n3, n3, n3, n3,
        n4, n4, n4, n4,
        n5, n5, n5, n5,
    }); // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE
#endif

    vsg::vec2 t00(0.0f, 0.0f);
    vsg::vec2 t01(0.0f, 1.0f);
    vsg::vec2 t10(1.0f, 0.0f);
    vsg::vec2 t11(1.0f, 1.0f);

    auto texcoords = vsg::vec2Array::create(
        {t00, t10, t11, t01, t00, t10, t11, t01, t00, t10, t11, t01,
         t00, t10, t11, t01, t00, t10, t11, t01, t00, t10, t11, t01});  // VK_FORMAT_R32G32_SFLOAT,
                                                                        // VK_VERTEX_INPUT_RATE_VERTEX,
                                                                        // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                        // VK_SHARING_MODE_EXCLUSIVE

    auto indices = vsg::ushortArray::create(
        {0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
         12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23});  // VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                                    // VK_SHARING_MODE_EXCLUSIVE

    // setup geometry
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(
        0, vsg::DataList{vertices, colors, texcoords}));  // shader doesn't support normals yet..
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(indices->size(), 1, 0, 0, 0));

    // add drawCommands to transform
    transform->addChild(drawCommands);
    scenegraph->addChild(transform);

    compile(scenegraph);

    return scenegraph;
}
