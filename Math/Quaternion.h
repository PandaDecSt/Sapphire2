#pragma once

#include "Predefined.h"
#include "Matrix3.h"

#ifdef SAPPHIRE_SSE
#include <emmintrin.h>
#endif


namespace Sapphire
{

	/// Rotation represented as a four-dimensional normalized vector.
	class SAPPHIRE_API Quaternion
	{
	public:
		/// Construct an identity quaternion.
		Quaternion()
#ifndef SAPPHIRE_SSE
			:w_(1.0f),
			x_(0.0f),
			y_(0.0f),
			z_(0.0f)
#endif
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_set_ps(0.f, 0.f, 0.f, 1.f));
#endif
		}

		/// Copy-construct from another quaternion.
		Quaternion(const Quaternion& quat)
#ifndef SAPPHIRE_SSE
			:w_(quat.w_),
			x_(quat.x_),
			y_(quat.y_),
			z_(quat.z_)
#endif
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_loadu_ps(&quat.w_));
#endif
		}

		/// Construct from values.
		Quaternion(float w, float x, float y, float z)
#ifndef SAPPHIRE_SSE
			:w_(w),
			x_(x),
			y_(y),
			z_(z)
#endif
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_set_ps(z, y, x, w));
#endif
		}

		/// Construct from a float array.
		explicit Quaternion(const float* data)
#ifndef SAPPHIRE_SSE
			:w_(data[0]),
			x_(data[1]),
			y_(data[2]),
			z_(data[3])
#endif
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_loadu_ps(data));
#endif
		}

		/// Construct from an angle (in degrees) and axis.
		Quaternion(float angle, const Vector3& axis)
		{
			FromAngleAxis(angle, axis);
		}

		/// Construct from an angle (in degrees, for Urho2D).
		explicit Quaternion(float angle)
		{
			FromAngleAxis(angle, Vector3::FORWARD);
		}

		/// Construct from Euler angles (in degrees.)
		Quaternion(float x, float y, float z)
		{
			FromEulerAngles(x, y, z);
		}

		/// Construct from the rotation difference between two direction vectors.
		Quaternion(const Vector3& start, const Vector3& end)
		{
			FromRotationTo(start, end);
		}

		/// Construct from orthonormal axes.
		Quaternion(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
		{
			FromAxes(xAxis, yAxis, zAxis);
		}

		/// Construct from a rotation matrix.
		explicit Quaternion(const Matrix3& matrix)
		{
			FromRotationMatrix(matrix);
		}

#ifdef SAPPHIRE_SSE
		explicit Quaternion(__m128 wxyz)
		{
			_mm_storeu_ps(&w_, wxyz);
		}
#endif

		/// Assign from another quaternion.
		Quaternion& operator =(const Quaternion& rhs)
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_loadu_ps(&rhs.w_));
#else
			w_ = rhs.w_;
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
#endif
			return *this;
		}

		/// Add-assign a quaternion.
		Quaternion& operator +=(const Quaternion& rhs)
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_add_ps(_mm_loadu_ps(&w_), _mm_loadu_ps(&rhs.w_)));
#else
			w_ += rhs.w_;
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
#endif
			return *this;
		}

		/// Multiply-assign a scalar.
		Quaternion& operator *=(float rhs)
		{
#ifdef SAPPHIRE_SSE
			_mm_storeu_ps(&w_, _mm_mul_ps(_mm_loadu_ps(&w_), _mm_set1_ps(rhs)));
#else
			w_ *= rhs;
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
#endif
			return *this;
		}

		/// Test for equality with another quaternion without epsilon.
		bool operator ==(const Quaternion& rhs) const
		{
#ifdef SAPPHIRE_SSE
			__m128 c = _mm_cmpeq_ps(_mm_loadu_ps(&w_), _mm_loadu_ps(&rhs.w_));
			c = _mm_and_ps(c, _mm_movehl_ps(c, c));
			c = _mm_and_ps(c, _mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 1, 1, 1)));
			return !_mm_ucomige_ss(c, c);
#else
			return w_ == rhs.w_ && x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
#endif
		}

		/// Test for inequality with another quaternion without epsilon.
		bool operator !=(const Quaternion& rhs) const { return !(*this == rhs); }

		/// Multiply with a scalar.
		Quaternion operator *(float rhs) const
		{
#ifdef SAPPHIRE_SSE
			return Quaternion(_mm_mul_ps(_mm_loadu_ps(&w_), _mm_set1_ps(rhs)));
#else
			return Quaternion(w_ * rhs, x_ * rhs, y_ * rhs, z_ * rhs);
#endif
		}

		/// Return negation.
		Quaternion operator -() const
		{
#ifdef SAPPHIRE_SSE
			return Quaternion(_mm_xor_ps(_mm_loadu_ps(&w_), _mm_castsi128_ps(_mm_set1_epi32((int)0x80000000UL))));
#else
			return Quaternion(-w_, -x_, -y_, -z_);
#endif
		}

		/// Add a quaternion.
		Quaternion operator +(const Quaternion& rhs) const
		{
#ifdef SAPPHIRE_SSE
			return Quaternion(_mm_add_ps(_mm_loadu_ps(&w_), _mm_loadu_ps(&rhs.w_)));
#else
			return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
#endif
		}

		/// Subtract a quaternion.
		Quaternion operator -(const Quaternion& rhs) const
		{
#ifdef SAPPHIRE_SSE
			return Quaternion(_mm_sub_ps(_mm_loadu_ps(&w_), _mm_loadu_ps(&rhs.w_)));
#else
			return Quaternion(w_ - rhs.w_, x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
#endif
		}

		/// Multiply a quaternion.
		Quaternion operator *(const Quaternion& rhs) const
		{
#ifdef SAPPHIRE_SSE
			__m128 q1 = _mm_loadu_ps(&w_);
			__m128 q2 = _mm_loadu_ps(&rhs.w_);
			q2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 3, 2, 1));
			const __m128 signy = _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, 0, 0));
			const __m128 signx = _mm_shuffle_ps(signy, signy, _MM_SHUFFLE(2, 0, 2, 0));
			const __m128 signz = _mm_shuffle_ps(signy, signy, _MM_SHUFFLE(3, 0, 0, 3));
			__m128 out = _mm_mul_ps(_mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 1, 1, 1)), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 2, 3)));
			out = _mm_add_ps(_mm_mul_ps(_mm_xor_ps(signy, _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 2, 2, 2))), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 0, 3, 2))), _mm_xor_ps(signx, out));
			out = _mm_add_ps(_mm_mul_ps(_mm_xor_ps(signz, _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 3, 3, 3))), _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 3, 0, 1))), out);
			out = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 0, 0, 0)), q2), out);
			return Quaternion(_mm_shuffle_ps(out, out, _MM_SHUFFLE(2, 1, 0, 3)));
#else
			return Quaternion(
				w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_,
				w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
				w_ * rhs.y_ + y_ * rhs.w_ + z_ * rhs.x_ - x_ * rhs.z_,
				w_ * rhs.z_ + z_ * rhs.w_ + x_ * rhs.y_ - y_ * rhs.x_
				);
#endif
		}

		/// Multiply a Vector3.
		Vector3 operator *(const Vector3& rhs) const
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 3, 2, 1));
			__m128 v = _mm_set_ps(0.f, rhs.z_, rhs.y_, rhs.x_);
			const __m128 W = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
			const __m128 a_yzx = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 0, 2, 1));
			__m128 x = _mm_mul_ps(q, _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1)));
			__m128 qxv = _mm_sub_ps(x, _mm_mul_ps(a_yzx, v));
			__m128 Wv = _mm_mul_ps(W, v);
			__m128 s = _mm_add_ps(qxv, _mm_shuffle_ps(Wv, Wv, _MM_SHUFFLE(3, 1, 0, 2)));
			__m128 qs = _mm_mul_ps(q, s);
			__m128 y = _mm_shuffle_ps(qs, qs, _MM_SHUFFLE(3, 1, 0, 2));
			s = _mm_sub_ps(_mm_mul_ps(a_yzx, s), y);
			s = _mm_add_ps(s, s);
			s = _mm_add_ps(s, v);

			return Vector3(
				_mm_cvtss_f32(s),
				_mm_cvtss_f32(_mm_shuffle_ps(s, s, _MM_SHUFFLE(1, 1, 1, 1))),
				_mm_cvtss_f32(_mm_movehl_ps(s, s)));
#else
			Vector3 qVec(x_, y_, z_);
			Vector3 cross1(qVec.CrossProduct(rhs));
			Vector3 cross2(qVec.CrossProduct(cross1));

			return rhs + 2.0f * (cross1 * w_ + cross2);
#endif
		}

		/// Define from an angle (in degrees) and axis.
		void FromAngleAxis(float angle, const Vector3& axis);
		/// Define from Euler angles (in degrees.)
		void FromEulerAngles(float x, float y, float z);
		/// Define from the rotation difference between two direction vectors.
		void FromRotationTo(const Vector3& start, const Vector3& end);
		/// Define from orthonormal axes.
		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
		/// Define from a rotation matrix.
		void FromRotationMatrix(const Matrix3& matrix);
		/// Define from a direction to look in and an up direction. Return true if successful, or false if would result in a NaN, in which case the current value remains.
		bool FromLookRotation(const Vector3& direction, const Vector3& up = Vector3::UP);

		/// Normalize to unit length.
		void Normalize()
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			__m128 n = _mm_mul_ps(q, q);
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
			__m128 e = _mm_rsqrt_ps(n);
			__m128 e3 = _mm_mul_ps(_mm_mul_ps(e, e), e);
			__m128 half = _mm_set1_ps(0.5f);
			n = _mm_add_ps(e, _mm_mul_ps(half, _mm_sub_ps(e, _mm_mul_ps(n, e3))));
			_mm_storeu_ps(&w_, _mm_mul_ps(q, n));
#else
			float lenSquared = LengthSquared();
			if (!Sapphire::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				w_ *= invLen;
				x_ *= invLen;
				y_ *= invLen;
				z_ *= invLen;
			}
#endif
		}

		/// Return normalized to unit length.
		Quaternion Normalized() const
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			__m128 n = _mm_mul_ps(q, q);
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
			__m128 e = _mm_rsqrt_ps(n);
			__m128 e3 = _mm_mul_ps(_mm_mul_ps(e, e), e);
			__m128 half = _mm_set1_ps(0.5f);
			n = _mm_add_ps(e, _mm_mul_ps(half, _mm_sub_ps(e, _mm_mul_ps(n, e3))));
			return Quaternion(_mm_mul_ps(q, n));
#else
			float lenSquared = LengthSquared();
			if (!Sapphire::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				return *this * invLen;
			}
			else
				return *this;
#endif
		}

		/// Return inverse.
		Quaternion Inverse() const
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			__m128 n = _mm_mul_ps(q, q);
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
			return Quaternion(_mm_div_ps(_mm_xor_ps(q, _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, (int)0x80000000UL, 0))), n));
#else
			float lenSquared = LengthSquared();
			if (lenSquared == 1.0f)
				return Conjugate();
			else if (lenSquared >= M_EPSILON)
				return Conjugate() * (1.0f / lenSquared);
			else
				return IDENTITY;
#endif
		}

		/// Return squared length.
		float LengthSquared() const
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			__m128 n = _mm_mul_ps(q, q);
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
			return _mm_cvtss_f32(n);
#else
			return w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
#endif
		}

		/// Calculate dot product.
		float DotProduct(const Quaternion& rhs) const
		{
#ifdef SAPPHIRE_SSE
			__m128 q1 = _mm_loadu_ps(&w_);
			__m128 q2 = _mm_loadu_ps(&rhs.w_);
			__m128 n = _mm_mul_ps(q1, q2);
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(2, 3, 0, 1)));
			n = _mm_add_ps(n, _mm_shuffle_ps(n, n, _MM_SHUFFLE(0, 1, 2, 3)));
			return _mm_cvtss_f32(n);
#else
			return w_ * rhs.w_ + x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
#endif
		}

		/// Test for equality with another quaternion with epsilon.
		bool Equals(const Quaternion& rhs) const
		{
			return Sapphire::Equals(w_, rhs.w_) && Sapphire::Equals(x_, rhs.x_) && Sapphire::Equals(y_, rhs.y_) && Sapphire::Equals(z_, rhs.z_);
		}

		/// Return whether is NaN.
		bool IsNaN() const { return Sapphire::IsNaN(w_) || Sapphire::IsNaN(x_) || Sapphire::IsNaN(y_) || Sapphire::IsNaN(z_); }

		/// Return conjugate.
		Quaternion Conjugate() const
		{
#ifdef SAPPHIRE_SSE
			__m128 q = _mm_loadu_ps(&w_);
			return Quaternion(_mm_xor_ps(q, _mm_castsi128_ps(_mm_set_epi32((int)0x80000000UL, (int)0x80000000UL, (int)0x80000000UL, 0))));
#else
			return Quaternion(w_, -x_, -y_, -z_);
#endif
		}

		/// Return Euler angles in degrees.
		Vector3 EulerAngles() const;
		/// Return yaw angle in degrees.
		float YawAngle() const;
		/// Return pitch angle in degrees.
		float PitchAngle() const;
		/// Return roll angle in degrees.
		float RollAngle() const;
		/// Return the rotation matrix that corresponds to this quaternion.
		Matrix3 RotationMatrix() const;
		/// Spherical interpolation with another quaternion.
		Quaternion Slerp(Quaternion rhs, float t) const;
		/// Normalized linear interpolation with another quaternion.
		Quaternion Nlerp(Quaternion rhs, float t, bool shortestPath = false) const;

		//将这个四元数转换为一个旋转轴和旋转角度
		void ToAngleAxis(float& rfAngle, Vector3& rkAxis) const;

		/// Return float data.
		const float* Data() const { return &w_; }

		/// Return as string.
		String ToString() const;

		/// W coordinate.
		float w_;
		/// X coordinate.
		float x_;
		/// Y coordinate.
		float y_;
		/// Z coordinate.
		float z_;

		/// Identity quaternion.
		static const Quaternion IDENTITY;
	};
}