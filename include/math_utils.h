#pragma once

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Math/Rect.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Math/Random.h>
#include <Urho3D/Math/Sphere.h>
#include <Urho3D/Math/Frustum.h>
#include <Urho3D/Math/Matrix2.h>
#include <Urho3D/Math/Matrix3.h>
#include <Urho3D/Math/Matrix4.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Vector4.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Math/Polyhedron.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Math/BoundingBox.h>
#include <Urho3D/Math/AreaAllocator.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Container/HashSet.h>
#include <Urho3D/Container/List.h>
#include <Urho3D/Container/LinkedList.h>

#define PI 3.14159265359f

using Urho3D::AreaAllocator;
using Urho3D::Color;
using Urho3D::Frustum;
using Urho3D::FrustumPlane;
using Urho3D::HashMap;
using Urho3D::HashSet;
using Urho3D::LinkedList;
using Urho3D::List;
using Urho3D::Plane;
using Urho3D::Polyhedron;
using Urho3D::Sphere;
using Urho3D::String;
using Urho3D::StringHash;
using Urho3D::Vector;

using fray = Urho3D::Ray;
using frect = Urho3D::Rect;
using irect = Urho3D::IntRect;
using fmat2 = Urho3D::Matrix2;
using fmat3 = Urho3D::Matrix3;
using fmat4 = Urho3D::Matrix4;
using fmat3x4 = Urho3D::Matrix3x4;
using fvec2 = Urho3D::Vector2;
using fvec3 = Urho3D::Vector3;
using fvec4 = Urho3D::Vector4;
using ivec2 = Urho3D::IntVector2;
using ivec3 = Urho3D::IntVector3;
using fbbox = Urho3D::BoundingBox;
using fquat = Urho3D::Quaternion;

template<class T>
T degrees(const T & val_)
{
    return (180 / PI) * val_;
}

template<class T>
T radians(const T & val_)
{
    return (PI / 180) * val_;
}

fmat4 perspective_from(float fov_, float aspect_ratio_, float z_near_, float z_far_);

fmat4 perspective_from(float near_left,
                       float near_right,
                       float near_top,
                       float near_bottom,
                       float z_near,
                       float z_far);

fmat4 ortho_from(float left_, float right_, float top_, float bottom_, float near_, float far_);