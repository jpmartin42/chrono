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

#include "chrono/assets/ChLineShape.h"

namespace chrono {

// Register into the object factory, to enable run-time dynamic creation and persistence
CH_FACTORY_REGISTER(ChLineShape)

ChLineShape::ChLineShape() : npoints(200), thickness(10.0) {
    gline = chrono_types::make_shared<geometry::ChLineSegment>();
}

ChLineShape::ChLineShape(std::shared_ptr<geometry::ChLine>& mline) : npoints(200), thickness(10.0), gline(mline) {}

void ChLineShape::ArchiveOut(ChArchiveOut& marchive) {
    // version number
    marchive.VersionWrite<ChLineShape>();
    // serialize parent class
    ChVisualShape::ArchiveOut(marchive);
    // serialize all member data:
    marchive << CHNVP(gline);
    marchive << CHNVP(npoints);
    marchive << CHNVP(thickness);
}

void ChLineShape::ArchiveIn(ChArchiveIn& marchive) {
    // version number
    /*int version =*/ marchive.VersionRead<ChLineShape>();
    // deserialize parent class
    ChVisualShape::ArchiveIn(marchive);
    // stream in all member data:
    marchive >> CHNVP(gline);
    marchive >> CHNVP(npoints);
    marchive >> CHNVP(thickness);
}

}  // end namespace chrono
