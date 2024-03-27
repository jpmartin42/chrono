// =====================================================================================
//  
// ChModuleCore.i
// Create the Python and C# wrappers for the core Chrono module.
//
// ATTENTION: 
// Must be included from another SWIG interface file which defines the module.
//
// =====================================================================================

// Turn on the documentation of members, for more intuitive IDE typing
%feature("autodoc", "1");
%feature("flatnested", "1");

// Turn on the exception handling to intercept C++ exceptions
%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}


// For optional downcasting of polimorphic objects:
%include "../chrono_downcast.i" 

// For supporting shared pointers:
%include <std_shared_ptr.i>


// Include C++ headers this way...

%{
#include <typeindex>
#include <cstddef>

#include "chrono/ChConfig.h"

#include "chrono/core/ChApiCE.h"
#include "chrono/serialization/ChArchiveJSON.h"
#include "chrono/physics/ChBody.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChLink.h"
#include "chrono/physics/ChLinkMate.h"
#include "chrono/physics/ChLinkMotionImposed.h"
#include "chrono/physics/ChLoad.h"
#include "chrono/physics/ChLoadsBody.h"
#include "chrono/physics/ChNodeBase.h"
#include "chrono/physics/ChNodeXYZ.h"
#include "chrono/physics/ChLoadsNodeXYZ.h"
#include "chrono/physics/ChIndexedNodes.h"

#include "chrono/assets/ChVisualMaterial.h"
#include "chrono/assets/ChGlyphs.h"
#include "chrono/assets/ChVisualSystem.h"

#include "chrono/fea/ChMesh.h"


#include "chrono/collision/ChCollisionShape.h"
#include "chrono/collision/ChCollisionShapes.h"
#include "chrono/collision/ChCollisionModel.h"
#include "chrono/collision/ChCollisionSystem.h"
#include "chrono/collision/bullet/ChCollisionSystemBullet.h"
#ifdef CHRONO_COLLISION
#include "chrono/collision/multicore/ChCollisionSystemMulticore.h"
#endif

#include "chrono/geometry/ChTriangleMesh.h"
#include "chrono/geometry/ChTriangleMeshConnected.h"
#include "chrono/geometry/ChTriangleMeshSoup.h"
#include "chrono/core/ChBezierCurve.h"
#include "Eigen/src/Core/util/Memory.h"
#include "chrono/utils/ChConstants.h"
#include "chrono/utils/ChUtils.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono/utils/ChFilters.h"
#include "chrono/utils/ChUtilsCreators.h"
#include "chrono/utils/ChUtilsGeometry.h"

using namespace chrono;
using namespace chrono::fea;
%}


// Undefine ChApi otherwise SWIG gives a syntax error
#define ChApi 
#define EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#define CH_DEPRECATED(msg)

%ignore CH_ENUM_MAPPER_BEGIN;
%ignore CH_ENUM_VAL;
%ignore CH_ENUM_MAPPER_END;
%ignore CH_CLASS_VERSION;
// Cross-inheritance between Python and c++ for callbacks that must be inherited.
// Put this 'director' feature _before_ class wrapping declaration.

// Include other .i configuration files for SWIG. 
// These are divided in many .i files, each per a
// different c++ class, when possible.

%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "cpointer.i"

// This is to enable references to double,int,etc. types in function parameters
%pointer_class(int,int_ptr);
%pointer_class(double,double_ptr);
%pointer_class(float,float_ptr);


//
// For each class, keep updated the  A, B, C sections: 
// 


//
// A- ENABLE SHARED POINTERS
//
// Note that this must be done for almost all objects (not only those that are
// handled by shered pointers in C++, but all their chidren and parent classes. It
// is enough that a single class in an inheritance tree uses %shared_ptr, and all other in the 
// tree must be promoted to %shared_ptr too).

%shared_ptr(chrono::ChFrame<double>)
%shared_ptr(chrono::ChFrameMoving<double>)

//%shared_ptr(chrono::ChColor)
%shared_ptr(chrono::ChBezierCurve)
%shared_ptr(chrono::ChGlyphs)
%shared_ptr(chrono::ChCamera) 
%shared_ptr(chrono::ChVisualMaterial)
%shared_ptr(chrono::ChVisualSystem)

%shared_ptr(chrono::ChFunction)
%shared_ptr(chrono::ChFunctionBSpline)
%shared_ptr(chrono::ChFunctionConst)
%shared_ptr(chrono::ChFunctionConstAcc)
%shared_ptr(chrono::ChFunctionCycloidal)
%shared_ptr(chrono::ChFunctionDerivative)
%shared_ptr(chrono::ChFunctionFillet3)
%shared_ptr(chrono::ChFunctionIntegral)
%shared_ptr(chrono::ChFunctionMirror)
%shared_ptr(chrono::ChFunctionOperator)
%shared_ptr(chrono::ChFunctionPoly)
%shared_ptr(chrono::ChFunctionPoly345)
%shared_ptr(chrono::ChFunctionRamp)
%shared_ptr(chrono::ChFunctionInterp)
%shared_ptr(chrono::ChFunctionRepeat)
%shared_ptr(chrono::ChFunctionSequence)
%shared_ptr(chrono::ChFunctionPoly23)
%shared_ptr(chrono::ChFunctionSine)
%shared_ptr(chrono::ChFunctionSineStep)
%shared_ptr(chrono::ChFunctionSetpoint)
%shared_ptr(chrono::ChFunctionSetpointCallback)

%shared_ptr(chrono::ChFunctionRotation)
%shared_ptr(chrono::ChFunctionRotationAxis)
%shared_ptr(chrono::ChFunctionRotationABCFunctions)
%shared_ptr(chrono::ChFunctionRotationSetpoint)
%shared_ptr(chrono::ChFunctionRotationBSpline)
%shared_ptr(chrono::ChFunctionRotationSQUAD)
%shared_ptr(chrono::ChFunctionPosition)
%shared_ptr(chrono::ChFunctionPositionLine)
%shared_ptr(chrono::ChFunctionPositionSetpoint)
%shared_ptr(chrono::ChFunctionPositionXYZFunctions)

%shared_ptr(chrono::ChObj)
%shared_ptr(chrono::ChPhysicsItem)
%shared_ptr(chrono::ChContactable)
%shared_ptr(chrono::ChContactable_1vars<3>)
%shared_ptr(chrono::ChContactable_1vars<6>)
%shared_ptr(chrono::ChContactable_3vars<3,3,3>)
%shared_ptr(chrono::ChContactable_3vars<6,6,6>)
%shared_ptr(chrono::ChIndexedNodes)
%shared_ptr(chrono::ChContactMaterialNSC)
%shared_ptr(chrono::ChContactMaterialSMC)
%shared_ptr(chrono::ChContactMaterial)
%shared_ptr(chrono::ChNodeBase)
%shared_ptr(chrono::ChNodeXYZ)
%shared_ptr(chrono::ChMarker)
%shared_ptr(chrono::ChForce)
%shared_ptr(chrono::ChBodyEasySphere)
%shared_ptr(chrono::ChBodyEasyBox)
%shared_ptr(chrono::ChBodyEasyEllipsoid)
%shared_ptr(chrono::ChBodyEasyCylinder)
%shared_ptr(chrono::ChBodyEasyConvexHull)
%shared_ptr(chrono::ChBodyEasyConvexHullAuxRef)
%shared_ptr(chrono::ChBodyEasyMesh)
%shared_ptr(chrono::ChBodyEasyClusterOfSpheres)
%shared_ptr(chrono::ChConveyor)
%shared_ptr(chrono::ChFeeder)
%shared_ptr(chrono::ChParticle)
%shared_ptr(chrono::ChParticleBase)
%shared_ptr(chrono::ChIndexedParticles)
%shared_ptr(chrono::ChParticleCloud)
%shared_ptr(chrono::ChSystemNSC)
%shared_ptr(chrono::ChSystemSMC)
%shared_ptr(chrono::ChContactContainer)
%shared_ptr(chrono::ChProximityContainer)

%shared_ptr(chrono::fea::ChMesh)

%shared_ptr(chrono::ChCollisionModel)

%shared_ptr(chrono::ChCollisionSystem)
%shared_ptr(chrono::ChCollisionSystemBullet)
#ifdef CHRONO_COLLISION
%shared_ptr(chrono::ChCollisionSystemMulticore)
#endif

%shared_ptr(chrono::ChCollisionSystem::BroadphaseCallback)
%shared_ptr(chrono::ChCollisionSystem::NarrowphaseCallback)
%shared_ptr(chrono::ChCollisionSystem::VisualizationCallback)

%shared_ptr(chrono::ChLinkMarkers)
%shared_ptr(chrono::ChLinkLimit)

%shared_ptr(chrono::ChLinkDistance)
%shared_ptr(chrono::ChLinkLockGear)
%shared_ptr(chrono::ChLinkLockLinActuator)
%shared_ptr(chrono::ChLinkMate)
%shared_ptr(chrono::ChLinkMateGeneric)
%shared_ptr(chrono::ChLinkMatePlanar)
%shared_ptr(chrono::ChLinkMateCylindrical)
%shared_ptr(chrono::ChLinkMateSpherical)
%shared_ptr(chrono::ChLinkMateDistanceZ)
%shared_ptr(chrono::ChLinkMateParallel)
%shared_ptr(chrono::ChLinkMateOrthogonal)
%shared_ptr(chrono::ChLinkMateFix)
%shared_ptr(chrono::ChLinkMateRevolute)
%shared_ptr(chrono::ChLinkMatePrismatic)
%shared_ptr(chrono::ChLinkMateRackPinion)
%shared_ptr(chrono::ChLinkLockPulley)
%shared_ptr(chrono::ChLinkRevolute)
%shared_ptr(chrono::ChLinkRevoluteSpherical)
%shared_ptr(chrono::ChLinkLockScrew)
%shared_ptr(chrono::ChLinkTSDA)
%shared_ptr(chrono::ChLinkUniversal)
%shared_ptr(chrono::ChLinkMotor)
%shared_ptr(chrono::ChLinkMotorLinear)
%shared_ptr(chrono::ChLinkMotorLinearDriveline)
%shared_ptr(chrono::ChLinkMotorLinearForce)
%shared_ptr(chrono::ChLinkMotorLinearPosition)
%shared_ptr(chrono::ChLinkMotorLinearSpeed)
%shared_ptr(chrono::ChLinkMotorRotation)
%shared_ptr(chrono::ChLinkMotorRotationAngle)
%shared_ptr(chrono::ChLinkMotorRotationDriveline)
%shared_ptr(chrono::ChLinkMotorRotationSpeed)
%shared_ptr(chrono::ChLinkMotorRotationTorque)
%shared_ptr(chrono::ChLinkLockTrajectory)
%shared_ptr(chrono::ChLinkLockPointSpline)
%shared_ptr(chrono::ChLinkMotionImposed)

%shared_ptr(chrono::ChGeometry)
%shared_ptr(chrono::ChLine)
%shared_ptr(chrono::ChVolume)
%shared_ptr(chrono::ChSurface)
%shared_ptr(chrono::ChBox)
%shared_ptr(chrono::ChSphere)
%shared_ptr(chrono::ChCylinder)
%shared_ptr(chrono::ChCapsule)
%shared_ptr(chrono::ChCone)
%shared_ptr(chrono::ChEllipsoid)
%shared_ptr(chrono::ChLineArc)
%shared_ptr(chrono::ChLineSegment)
%shared_ptr(chrono::ChLineNurbs)
%shared_ptr(chrono::ChLinePath)
%shared_ptr(chrono::ChLinePoly)
%shared_ptr(chrono::ChLineBezier)
%shared_ptr(chrono::ChLineCam)
%shared_ptr(chrono::ChLineBSpline)
%shared_ptr(chrono::ChTriangle)
%shared_ptr(chrono::ChSurfaceNurbs)
%shared_ptr(chrono::ChTriangleMesh)
%shared_ptr(chrono::ChTriangleMeshConnected)
%shared_ptr(chrono::ChTriangleMeshSoup)

// Cross-inheritance for callbacks that must be inherited.
// Put these 'director' features _before_ class wrapping declaration.

%feature("director") chrono::ChCollisionSystem::BroadphaseCallback;
%feature("director") chrono::ChCollisionSystem::NarrowphaseCallback;
%feature("director") chrono::ChCollisionSystem::VisualizationCallback;

//
// B- INCLUDE HEADERS
//
//
// 1) 
//    When including with %include all the .i files, make sure that 
// the .i of a derived class is included AFTER the .i of
// a base class, otherwise SWIG is not able to build the type
// infos. 
//
// 2)
//    Then, this said, if one member function in Foo_B.i returns
// an object of Foo_A.i (or uses it as a parameter) and yet you must %include
// A before B, ex.because of rule 1), a 'forward reference' to A must be done in
// B by. Seems that it is enough to write 
//  mynamespace { class myclass; }
// in the .i file, before the %include of the .h, even if already forwarded in .h

//  core/  classes
%include "ChClassFactory.i"
%include "../../../chrono/core/ChGlobal.h"
//%include "ChArchive.i"
%include "ChMatrix.i"
%include "ChVector3.i"
#define ChVector3d ChVector3d
%include "ChQuaternion.i"
%include "ChTensors.i"
%include "../../../chrono/core/ChBezierCurve.h"
#define Quaternion ChQuaternion<double>
%include "ChMatrix33.i"
%include "ChCoordsys.i"
#define Coordsys ChCoordsys<double>
%include "ChFrame.i"
%include "ChFrameMoving.i"
%include "ChRandom.i"
%include "ChTimer.i"
%include "ChRealtimeStep.i"

// geometry/   classes
%include "ChGeometry.i"




//collision classes
%include "ChContactMaterial.i"
%include "ChCollisionShape.i"
%include "ChCollisionModel.i"
%include "../../../chrono/collision/ChCollisionShape.h"
%include "../../../chrono/collision/ChCollisionShapes.h"
%include "../../../chrono/collision/ChCollisionModel.h"
%include "ChCollisionInfo.i"
%include "../../../chrono/collision/ChCollisionSystem.h"
%include "../../../chrono/collision/bullet/ChCollisionSystemBullet.h"
#ifdef CHRONO_COLLISION
%include "../../../chrono/collision/multicore/ChCollisionSystemMulticore.h"
#endif

// functions/   classes
%include "ChFunction.i"

%include "../../../chrono/fea/ChMesh.h"


// assets
%include "ChColor.i"
%include "ChVisualMaterial.i"
%include "ChVisualShape.i"
%include "ChVisualModel.i"
%include "ChTexture.i"
%include "ChCamera.i"
%include "../../../chrono/assets/ChGlyphs.h"
%include "ChVisualSystem.i"

// physics/  classes
%include "ChControllers.i"
%include "ChLoadable.i"
%include "ChObject.i"
%include "ChPhysicsItem.i"
%include "../../../chrono/physics/ChIndexedNodes.h"
%include "../../../chrono/physics/ChNodeBase.h"
%include "ChNodeXYZ.i"
%include "ChBodyFrame.i"
%include "ChMarker.i"
%include "ChForce.i"
%include "ChBody.i"
%include "ChBodyAuxRef.i"
%include "../../../chrono/physics/ChBodyEasy.h"
%include "ChConveyor.i"
%include "ChFeeder.i"
%include "ChIndexedParticles.i"
%include "ChParticleCloud.i"
%include "ChLinkBase.i"
%include "ChLink.i"
%include "ChLinkMarkers.i"
%include "ChLinkLimit.i"
%include "ChLinkForce.i"
%include "ChLinkLock.i"
%include "ChLinkMate.i"
%include "ChLinkDistance.i"
%include "ChLinkLockLinActuator.i"
%include "ChLinkLockPulley.i"
%include "ChLinkLockScrew.i"
%include "ChLinkTSDA.i"
%include "ChLinkRSDA.i"
%include "ChLinkLockGear.i"
%include "ChLinkRevolute.i"
%include "ChLinkRevoluteSpherical.i"
%include "ChLinkUniversal.i" 
%include "ChLinkLockTrajectory.i" 
%include "ChLinkLockPointSpline.i"
%include "../../../chrono/physics/ChLinkMotionImposed.h"
%include "ChAssembly.i"
%include "ChTimestepper.i"
%include "ChSolver.i"
%include "ChContactContainer.i"
%include "ChSystem.i"
%include "ChSystemNSC.i"
%include "ChSystemSMC.i"
%include "ChProximityContainer.i"
%include "ChLoader.i"
%include "ChLoad.i"
%include "ChLoadContainer.i"
%include "ChShaft.i"
%include "ChShaftMotor.i"
%include "ChLinkMotor.i"


// Utils

%include "../../../chrono/utils/ChConstants.h"
%include "../../../chrono/utils/ChUtils.h"
%include "../../../chrono/utils/ChUtilsInputOutput.h"
%include "../../../chrono/utils/ChFilters.h"
%include "../../../chrono/utils/ChUtilsCreators.h"
%include "../../../chrono/utils/ChUtilsGeometry.h"

%include "ChParticleFactory.i"
//
// C- DOWNCASTING OF SHARED POINTERS
// 
// This is not automatic in Python + SWIG, except if one uses the 
// %downcast_output_sharedptr(...) macro, as above, but this causes
// a lot of code bloat. So in the following we create a set of Python-side
// functions to perform casting by hand, thank to the macro 
// %DefSharedPtrDynamicDowncast(base,derived). 
// Do not specify the "chrono::" namespace before base or derived!
// Later, in python, you can do the following:
//  myvis = chrono.CastToChVisualizationShared(myasset)
//  print ('Could be cast to visualization object?', !myvis.IsNull())

%DefSharedPtrDynamicDowncast(chrono, ChContactable, ChBody)
%DefSharedPtrDynamicDowncast(chrono, ChContactable, ChBodyAuxRef)

%DefSharedPtrDynamicDowncast(chrono, ChLoadable, ChBody)
%DefSharedPtrDynamicDowncast(chrono, ChLoadable, ChNodeBase)

// C- DOWNCASTING OF SHARED POINTERS
// see details in ChModuleCore.i


%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeFEA)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeModelFile)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeTriangleMesh)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeSphere)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeEllipsoid)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeBarrel)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeBox)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeCone)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeCylinder)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeCapsule)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeRoundedCylinder)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeRoundedBox)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapePath)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeLine)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapePointPoint)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeRotSpring)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeSegment)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeSpring)
%DefSharedPtrDynamicDowncast(chrono, ChVisualShape, ChVisualShapeSurface)


%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeArc2D)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeBarrel)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeBox)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeCapsule)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeCone)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeConvexHull)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeCylinder)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeCylindricalShell)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeEllipsoid)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapePath2D)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapePoint)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeRoundedBox)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeRoundedCylinder)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeSegment2D)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeSphere)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeTriangle)
%DefSharedPtrDynamicDowncast(chrono, ChCollisionShape, ChCollisionShapeTriangleMesh)


%DefSharedPtrDynamicDowncast(chrono, ChCollisionSystem, ChCollisionSystemBullet)
#ifdef CHRONO_COLLISION
%DefSharedPtrDynamicDowncast(chrono, ChCollisionSystem, ChCollisionSystemMulticore)
#endif

%DefSharedPtrDynamicDowncast(chrono, ChBodyFrame, ChBody)
%DefSharedPtrDynamicDowncast(chrono, ChBodyFrame, ChBodyAuxRef)
%DefSharedPtrDynamicDowncast(chrono, ChBodyFrame, ChConveyor)
%DefSharedPtrDynamicDowncast(chrono, ChBody, ChBodyFrame)  // <- upcast, for testing & workaround
%DefSharedPtrDynamicDowncast(chrono, ChBodyAuxRef, ChBodyFrame)  // <- upcast, for testing & workaround
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChBody)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChConveyor)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChBodyAuxRef)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChIndexedParticles)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChParticleCloud)
%DefSharedPtrDynamicDowncast(chrono, ChParticleCloud, ChIndexedParticles)


%DefSharedPtrDynamicDowncast(chrono, ChNodeBase, ChNodeXYZ)

%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLink)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMarkers)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLock)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockLock)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockRevolute)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockSpherical)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockCylindrical)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockPrismatic)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockPointPlane)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockPointLine)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockOldham)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockFree)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockAlign)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockParallel)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockPerpend)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMate)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateGeneric)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMatePlanar)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateCylindrical)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateSpherical)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateDistanceZ)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateParallel)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateOrthogonal)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateFix)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMateRevolute)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMatePrismatic)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockGear)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkDistance)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockLinActuator)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockPulley)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkLockScrew)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkTSDA)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkRSDA)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotor)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorLinear)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorLinearDriveline)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorLinearForce)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorLinearPosition)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorLinearSpeed)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorRotation)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorRotationAngle)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorRotationDriveline)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorRotationSpeed)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLinkMotorRotationTorque)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChLoadContainer)

%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMarkers)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLock)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockLock)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockRevolute)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockSpherical)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockCylindrical)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPrismatic)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPointPlane)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPointLine)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockOldham)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockFree)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockAlign)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockParallel)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPerpend)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMate)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateGeneric)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMatePlanar)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateCylindrical)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateSpherical)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateDistanceZ)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateParallel)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateOrthogonal)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateFix)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMateRevolute)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkMatePrismatic)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockGear)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkDistance)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockLinActuator)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPulley)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockScrew)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkTSDA)
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockPointSpline) 
%DefSharedPtrDynamicDowncast(chrono, ChLink, ChLinkLockTrajectory)


%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaft)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftBodyRotation)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftBodyTranslation)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsCouple)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsClutch)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsMotor)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsTorsionSpring)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsAppliedTorque)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsPlanetary)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsTorque)
%DefSharedPtrDynamicDowncast(chrono, ChPhysicsItem, ChShaftsThermalEngine)

%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadCustom)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadCustomMultiple)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyForce)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyTorque)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyInertia)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBody)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBodyTorque)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBodyBushingSpherical)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBodyBushingPlastic)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBodyBushingMate)
%DefSharedPtrDynamicDowncast(chrono, ChLoadBase, ChLoadBodyBodyBushingGeneric)

%DefSharedPtrDynamicDowncast(chrono, ChGeometry, ChTriangleMeshConnected)
%DefSharedPtrDynamicDowncast(chrono, ChGeometry, ChTriangleMeshSoup)

// .. to complete


#ifdef SWIGPYTHON  // --------------------------------------------------------------------- PYTHON

//
// ADDITIONAL C++ FUNCTIONS / CLASSES THAT ARE USED ONLY FOR PYTHON WRAPPER
//

%extend chrono::ChBezierCurveTracker {
public:
  double ClosestPointCurvature(const chrono::ChVector3d& loc, chrono::ChFrame<>& tnb) {
    double curvature;
    int foo = $self->CalcClosestPoint(loc, tnb, curvature);
    return curvature;
  }
};


//
// ADD PYTHON CODE
//


%pythoncode %{

def ImportSolidWorksSystem(mpath):
    import builtins
    import sys
    import os

    mdirname, mmodulename = os.path.split(mpath)

    builtins.exported_system_relpath = mdirname + "/"

    try:
        if sys.version_info[0] == 3 and sys.version_info[1] >= 5:
            import importlib.util
            spec = importlib.util.spec_from_file_location(mmodulename, mpath)
            imported_mod = importlib.util.module_from_spec(spec)
            sys.modules[mmodulename] = imported_mod
            spec.loader.exec_module(imported_mod)
        elif sys.version_info[0] == 3 and sys.version_info[1] < 5:
            import importlib.machinery
            loader = importlib.machinery.SourceFileLoader(mmodulename, mpath)
            imported_mod = loader.load_module()
        else:
            raise Exception("Python version not supported. Please upgrade it.")
    except Exception as e:
        print(f"Error loading module: {e}")
        return None

    return imported_mod.exported_items


%}

#endif             // --------------------------------------------------------------------- PYTHON
