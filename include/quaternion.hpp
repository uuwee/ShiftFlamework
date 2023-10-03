#pragma once

#include "Vector3.hpp"

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

	inline Quaternionf getinverse(const Quaternionf& p)
	{
		float dir_len = length(Vector3f({p.x, p.y, p.z}));
		return Quaternionf({-p.x / dir_len, -p.y / dir_len, -p.z / dir_len, p.w / dir_len});
	}

	inline Quaternionf get_conjugate(const Quaternionf& p) {
          return Quaternionf({-p.x, -p.y, -p.z, p.w});
    }

	
	inline Vector3f get_dirVector(const Quaternionf& p)
	{
		//ZYX
		//algorithm (https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf#%E5%9B%9E%E8%BB%A2%E8%A1%8C%E5%88%97%E3%81%8B%E3%82%89%E3%82%AF%E3%82%A9%E3%83%BC%E3%82%BF%E3%83%8B%E3%82%AA%E3%83%B3)

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

		auto vec = normalized(Vector3f({0,-1,0}));
		auto m = y_rot * vec;
		m = x_rot * m;
		m = z_rot * m;
		
		return m;
	}
}
