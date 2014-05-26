#include <ostream>

template<class T>
class Number {
protected:
	T val;
public:
	Number() {
		val = 0;
	}

	Number(T value) {
		val = value;
	}

	T value() const {
		return val;
	}

	Number& operator=(Number i) {
		val = i.value();
		return *this;
	}

	Number& operator=(T i) {
		val = i;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Number& obj) {
		return os << obj.val;
	}

	Number& operator++() {
		val++;
		return *this;
	}

	Number operator++(int) {
		Number tmp(*this);
		operator++();
		return tmp;
	}

	Number& operator+=(const Number& rhs) {
		val += rhs.value();
		return *this;
	}

	Number& operator--() {
		val--;
		return *this;
	}

	Number operator--(int) {
		Number tmp(*this);
		operator--();
		return tmp;
	}

	Number& operator-=(const Number& rhs) {
		val -= rhs.value();
		return *this;
	}

	Number& operator*=(const Number& rhs) {
		val *= rhs.value();
		return *this;
	}

	Number& operator/=(const Number& rhs) {
		val /= rhs.value();
		return *this;
	}

	operator T() {
		return val;
	}
};

class Integer : public Number<int> {
public:
	Integer() {
		val = 0;
	}

	Integer(int value) {
		val = value;
	}
};

class Long : public Number<long> {
public:
	Long() {
		val = 0;
	}

	Long(long value) {
		val = value;
	}
};

class LongLong : public Number<long long> {
public:
	LongLong() {
		val = 0;
	}

	LongLong(long long value) {
		val = value;
	}
};

class Short : public Number<short> {
public:
	Short() {
		val = 0;
	}

	Short(short value) {
		val = value;
	}
};

class Char : public Number<char> {
public:
	Char() {
		val = 0;
	}

	Char(char value) {
		val = value;
	}
};

class Float : public Number<float> {
public:
	Float() {
		val = 0;
	}

	Float(float value) {
		val = value;
	}
};

class Double : public Number<double> {
public:
	Double() {
		val = 0;
	}

	Double(double value) {
		val = value;
	}
};

class UInteger : public Number<unsigned int> {
public:
	UInteger() {
		val = 0;
	}

	UInteger(unsigned int value) {
		val = value;
	}
};

class ULong : public Number<unsigned long> {
public:
	ULong() {
		val = 0;
	}

	ULong(unsigned long value) {
		val = value;
	}
};

class ULongLong : public Number<unsigned long long> {
public:
	ULongLong() {
		val = 0;
	}

	ULongLong(unsigned long long value) {
		val = value;
	}
};

class UShort : public Number<unsigned short> {
public:
	UShort() {
		val = 0;
	}

	UShort(unsigned short value) {
		val = value;
	}
};

class UChar : public Number<unsigned char> {
public:
	UChar() {
		val = 0;
	}

	UChar(unsigned char value) {
		val = value;
	}
};

class Boolean {
private:
	bool val;
public:
	Boolean() {
		val = false;
	}

	Boolean(bool value) {
		val = value;
	}

	bool value() const {
		return val;
	}

	Boolean& operator=(Boolean i) {
		val = i.value();
		return *this;
	}

	Boolean& operator=(bool i) {
		val = i;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Boolean& obj) {
		return os << (obj.val == 1 ? "true" : "false");
	}

	operator bool() {
		return val;
	}
};