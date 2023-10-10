#pragma once

#include "Vector3.hpp"
#include "matrix.hpp"

namespace ShiftFlamework::Math
{
	using Quaternionf = Vector4f;
	//using Quaterniond = Vector4d;

	inline Quaternionf get_normalized(const Quaternionf& p) {
        float dir_len = length(Vector3f({p.x, p.y, p.z}));
        auto normalized = Quaternionf({p.x / dir_len, p.y / dir_len, p.z / dir_len, 1.0});
        return normalized;
    }
	
	inline Quaternionf operator * (const Quaternionf& q, const Quaternionf& p)
	{
		return Quaternionf({
			q.w * p.x - q.z * p.y + q.y * p.z + q.x * p.w, 
			q.z * p.x + q.w * p.y - q.x * p.z + q.y * p.w,
			-q.y * p.x + q.x * p.y + q.w * p.z + q.z * p.w, 
			-q.x * p.x - q.y * p.y - q.z * p.z + q.w * p.w});
	}

	inline Quaternionf operator * (const Quaternionf& q, const Vector3f& v)
	{
		Quaternionf p = q * Quaternionf({v.x, v.y, v.z, 0});
        auto a = p * Quaternionf({-q.x, -q.y, -q.z, q.w});
		return p * Quaternionf({-q.x, -q.y, -q.z, q.w});
	}

	inline Quaternionf get_inverse(const Quaternionf& p)
	{
		float dir_len = length(Vector3f({p.x, p.y, p.z}));
		return Quaternionf({-p.x / dir_len, -p.y / dir_len, -p.z / dir_len, p.w / dir_len});
	}

	inline Quaternionf get_conjugate(const Quaternionf& p) {
          return Quaternionf({-p.x, -p.y, -p.z, p.w});
    }

	
	inline Vector3f get_dirVector(const Quaternionf& p)
	{
		const auto q = p * Quaternionf({0, -1, 0, 0}) * get_conjugate(p);
		return Vector3f({q.x, q.y, q.z});

	}

	inline Quaternionf angleaxis(float angle, Vector3f axis){
          auto v = normalized(axis);
		return Quaternionf({v.x * sin(angle / 2), v.y * sin(angle / 2), v.z * sin(angle / 2), cos(angle / 2)});
	}

	inline Quaternionf euler_yxz(float y, float x, float z){
		auto x_rot = angleaxis(x, Vector3f({1,0,0}));
		auto y_rot = angleaxis(y, Vector3f({0,1,0}));
		auto z_rot = angleaxis(z, Vector3f({0,0,1}));
		
		return z_rot * x_rot * y_rot;
	}

	inline Matrix<float, 4, 4> transMatrix(Vector3f position_xyz, Quaternionf rotation, Vector3f size_xyz) { 
        auto pos = Matrix<float, 4, 4>({{{0, 0, 0, position_xyz.x},
                                           {0, 0, 0, position_xyz.y},
                                           {0, 0, 0, position_xyz.z},
                                           {0, 0, 0, 1}}});
		float q0 = rotation.x;
        float q1 = rotation.y;
        float q2 = rotation.z;
        float q3 = rotation.w;
        auto rot = Matrix<float, 4, 4>(
            {{{q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3, 2 * (q1 * q2 - q0 * q3),
               2 * (q0 * q2 + q1 * q3), 0},
              {2 * (q0 * q3 + q1 * q2), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3,
               2 * (-q0 * q1 + q2 * q3), 0},
              {2 * (q1 * q3 - q0 * q2), 2 * (q2 * q3 + q0 * q1),
               q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3, 0},
              {0, 0, 0, 1}}});
        auto siz = Matrix<float, 4, 4>({{{size_xyz.x, 0, 0, 0},
                                         {0, size_xyz.y, 0, 0},
                                         {0, 0, size_xyz.z, 0},
                                         {0, 0, 0, 1}}});
        return pos * rot * siz;
	}
}
