#include <math_utils.h>


fmat4 perspective_from(float near_left, float near_right, float near_top, float near_bottom, float z_near, float z_far)
{
    fmat4 ret;
    float x_range = near_right - near_left;
    float y_range = near_top - near_bottom;
    float z_range = z_far - z_near;

    ret.m00_ = 2 * z_near / x_range;
    ret.m11_ = 2 * z_near / y_range;
    ret.m22_ = (z_far + z_near) / z_range;
    ret.m23_ = -2.0f * z_far * z_near / z_range;
    ret.m32_ = 1.0f;
    ret.m33_ = 0.0f;
    ret.m02_ = (near_right + near_left) / x_range;
    ret.m12_ = (near_top + near_bottom) / y_range;
    return ret;
}

fmat4 perspective_from(float fov_, float aspect_ratio_, float z_near_, float z_far_)
{
    float near_y = z_near_ * std::tan(radians(fov_) / 2.0f);
    float near_x = aspect_ratio_ * near_y;
    return perspective_from(-near_x, near_x, near_y, -near_y, z_near_, z_far_);
}


fmat4 ortho_from(float left_, float right_, float top_, float bottom_, float near_, float far_)
{
    fmat4 ret;

    float w = right_ - left_;
    float h = top_ - bottom_;
    float p = far_ - near_;

    float x = (right_ + left_) / w;
    float y = (top_ + bottom_) / h;
    float z = (far_ + near_) / p;

    ret.m00_ = 2.0f / w; ret.m11_ = 2.0f / h; ret.m22_ = -2.0f / p;
    ret.m03_ = -x; ret.m13_ = -y; ret.m23_ = -z; ret.m33_ = 1.0f;
    return ret;
}
