// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Rainer Gericke
// =============================================================================
// Header for a class defining a box shape for the vulkan scene graph
// =============================================================================

#ifndef CH_VSG_BOX_H
#define CH_VSG_BOX_H

#include <iostream>
#include "chrono/core/ChVector.h"
#include "chrono_vsg/core/ChApiVSG.h"

#include <vsg/all.h>

namespace chrono {
namespace vsg3d {

class CH_VSG_API vsgBox {
  public:
    vsgBox();
    vsg::ref_ptr<vsg::Node> createTexturedNode(vsg::vec3 size,
                                               vsg::vec4 color,
                                               vsg::ref_ptr<vsg::MatrixTransform> transform);
    void compile(vsg::ref_ptr<vsg::Node> subgraph);

  private:
    vsg::ref_ptr<vsg::CompileTraversal> _compile;
};
}  // namespace vsg3d
}  // namespace chrono
#endif
