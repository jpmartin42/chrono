
// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2019 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Asher Elmquist
// =============================================================================
//
// GPS utils
//
// =============================================================================

#include "chrono/utils/ChConstants.h"

#include "chrono_sensor/utils/ChGPSUtils.h"

namespace chrono {
namespace sensor {

void Cartesian2GPS(ChVector3d& coords, ChVector3d& ref) {
    // convert from cartesian to gps coordinates assuming a sphere
    double lat = (coords.y() / EARTH_RADIUS) * 180.0 / chrono::CH_PI + ref.y();
    double lon = (coords.x() / (EARTH_RADIUS * cos(lat * chrono::CH_PI / 180.0))) * 180.0 / chrono::CH_PI + ref.x();
    double alt = coords.z() + ref.z();

    if (lon < -180.0) {
        lon = lon + 360.0;
    } else if (lon > 180.0) {
        lon = lon - 360.0;
    }
    coords = chrono::ChVector3d({lon, lat, alt});
}

void GPS2Cartesian(ChVector3d& coords, ChVector3d& ref) {
    double lon = coords.x();
    double lat = coords.y();
    double alt = coords.z();

    double x = ((lon - ref.x()) * chrono::CH_PI / 180.0) * (EARTH_RADIUS * cos(lat * chrono::CH_PI / 180.0));
    double y = ((lat - ref.y()) * chrono::CH_PI / 180.0) * EARTH_RADIUS;
    double z = alt - ref.z();

    coords = chrono::ChVector3d({x, y, z});
}

}  // namespace sensor
}  // namespace chrono
