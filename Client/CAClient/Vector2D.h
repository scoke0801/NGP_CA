#pragma once

template <class T>
class Vector2D
{
public:
	union
	{
		T arr[2];
		struct { T x, y; };
	};
public:
	static Vector2D<T> Clamp(const Vector2D<T> &that, double low, double high);
	static double Length(const Vector2D<T> &a, const Vector2D<T> &b);
	static Vector2D<T> Normalize(Vector2D<T> &that);

public:
	Vector2D() { x = y = T{}; }
	Vector2D(T x, T y) :x(x), y(y) {}
	~Vector2D() {}

	//������ ���� ������ ���� �� ���� ���� ������ ��� �� �� �ִ�.
	float Dot(const Vector2D<T> &other) const { return (x * other.x) + (y * other.y); }

	//2���� ������ ���� ������ ���� ���⼺ �Ǵ��� �� �� �ִ�.
	//ccw or cw
	//http://bowbowbow.tistory.com/14
	float Cross(const Vector2D<T> &other) const { return (x * other.y) - (y * other.x); }

	//�ݻ� ���͸� ����Ѵ�.
	// return value : reflection Vector
	// parameter : collision position
	// this : Incident Vector
	Vector2D<T> Reflect(const Vector2D<T> &collsion) const;

	//���͸� ����ȭ��Ų��.
	void Normalize();
	//������ ���̸� ����Ѵ�.
	float Length() const { return std::sqrt(Dot(*this)); }
	static float Length(const Vector2D<T> &that) { return sqrt(that.Dot()); }

	bool isZero() const { return ::isZero(x) && ::isZero(y); }

	double distance(const Vector2D<T> &ohter, double m) const;
public:
	Vector2D<T> operator+(const Vector2D<T> &other) const { return Vector2D<T>{ x + other.x, y + other.y }; }
	Vector2D<T> operator+(double scalar) const { return Vector2D<T>{ x + scalar, y + scalar }; }
	Vector2D<T> operator-(const Vector2D<T> &other) const { return Vector2D<T>{ x - other.x, y - other.y }; }
	Vector2D<T> operator-(double scalar) const { return Vector2D<T>{ x - scalar, y - scalar }; }
	Vector2D<T> operator*(const Vector2D<T> &other) const { return Vector2D<T>{ x * other.x, y * other.y }; }
	Vector2D<T> operator*(double scalar) const { return Vector2D<T>{ x * scalar, y * scalar }; }
	Vector2D<T> operator/(const Vector2D<T> &other) const { return Vector2D<T>{ x / other.x, y / other.y }; }
	
	Vector2D<T>& operator+=(const Vector2D<T> &other) { return *this = *this + other; }
	Vector2D<T>& operator+=(double sclar) { return *this = *this + sclar; }
	Vector2D<T>& operator-=(const Vector2D<T> &other) { return *this = *this - other; }
	Vector2D<T>& operator-=(double sclar) { return *this = *this - sclar; }
	Vector2D<T>& operator*=(const Vector2D<T> &other) { return *this = *this * other; }
	Vector2D<T>& operator*=(double sclar) { return *this = *this * sclar; }
	Vector2D<T>& operator/=(const Vector2D<T> &other) { return *this = *this / other; }
	Vector2D<T>& operator= (const Vector2D<T> &other) { x = other.x, y = other.y; return *this; }

	bool operator==(const Vector2D<T> &other) const { return (x == other.x && y == other.y); }
	bool operator!=(const Vector2D<T> &other) const { return (x != other.x && y != other.y); }

};

using Vector2i = Vector2D<int>;
using Vector2f = Vector2D<float>;
using Vector2d = Vector2D<double>;

template<class T>
inline Vector2D<T> Vector2D<T>::Clamp(const Vector2D<T>& that, double low, double high)
{
	double x = clamp(that.x, low, high);
	double y = clamp(that.y, low, high);
	return Vector2D<T>{x, y};
}

template<class T>
inline double Vector2D<T>::Length(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return std::sqrt(std::pow((a.x - b.x), 2) + std::pow((a.y - b.y), 2));
}

template<class T>
inline Vector2D<T> Vector2D<T>::Normalize(Vector2D<T>& that)
{
	double length = that.Length();
	that.x /= length;
	that.y /= length;
	return that;
}

template<class T>
inline Vector2D<T> Vector2D<T>::Reflect(const Vector2D<T>& normal) const
{
	Vector2D<T> reflect;
	double _dot = this->Dot(normal);

	reflect = *this - (normal * (_dot * 2));
	return reflect;
}

template<class T>
inline void Vector2D<T>::Normalize()
{
	double length = this->Length();
	x /= length;
	y /= length;
}

template<class T>
inline double Vector2D<T>::distance(const Vector2D<T> &other, double m) const
{
	// ax + by + c = 0
	// y = m(x - x1) + y1
	// mx - mx1 + y1 - y;
	// mx - y + y1 - mx1
	// a = m
	// b = -1
	// c = y1 - mx1
	double a = m;
	double b = -1;
	double c = other.y - m * other.x;

	return std::abs(a * x + b * y + c) / std::sqrt(a * a + b * b);
}

