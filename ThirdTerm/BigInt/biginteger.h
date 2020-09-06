#include <iostream>
#include <string>
#include <vector>

static const int BASE = 10;

enum Sign {
    PLUS,
    MINUS
};

class BigInteger {
public:
    friend const BigInteger Abs(const BigInteger& number);
    friend bool IsGreaterAbs(const BigInteger& lhs, const BigInteger& rhs);

    BigInteger();
    BigInteger(int number);

    explicit operator int() const;
    explicit operator bool() const;

    std::string toString() const;

    friend BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs);
    friend BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs);
    friend BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs);
    friend BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs);
    friend BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs);

    BigInteger& operator+=(const BigInteger& rhs);
    BigInteger& operator-=(const BigInteger& rhs);
    BigInteger& operator*=(const BigInteger& rhs);
    BigInteger& operator/=(const BigInteger& rhs);
    BigInteger& operator%=(const BigInteger& rhs);

    friend bool operator<(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator>(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator==(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs);

    friend const BigInteger& operator+(const BigInteger& number);
    friend const BigInteger operator-(const BigInteger& number);

    BigInteger& operator++();
    BigInteger operator++(int);

    BigInteger& operator--();
    BigInteger operator--(int);

    friend std::istream& operator>>(std::istream& input_stream, BigInteger& number);
    friend std::ostream& operator<<(std::ostream& output_stream, const BigInteger& number);

private:
    Sign sign;
    std::vector<int> digits;

    static Sign GetIntSign(int number);

    size_t GetSize() const;
    int GetDigit(size_t index) const;
    void AddDigit(int digit);

    bool IsZero() const;

    void DeleteZeros();
};

const BigInteger Abs(const BigInteger& number) {
    BigInteger temp = number;
    temp.sign = Sign::PLUS;

    return temp;
}

bool IsGreaterAbs(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.GetSize() < rhs.GetSize()) {
        return false;

    } else if (lhs.GetSize() == rhs.GetSize()) {
        int curr_pointer = lhs.GetSize() - 1;
        while (curr_pointer >= 0) {
            if (lhs.GetDigit(curr_pointer) < rhs.GetDigit(curr_pointer)) {
                return false;

            } else if (lhs.GetDigit(curr_pointer) > rhs.GetDigit(curr_pointer)) {
                return true;
            }

            --curr_pointer;
        }
    }

    return true;
}

BigInteger::BigInteger() : sign(Sign::PLUS) {

}

BigInteger::BigInteger(int number) {
    sign = GetIntSign(number);
    number = std::abs(number);

    do {
        digits.push_back(number % BASE);
        number /= BASE;

    } while (number != 0);
}

BigInteger::operator int() const {
    int number = 0;

    for (ssize_t i = GetSize() - 1; i >= 0; --i) {
        number *= BASE;
        number += GetDigit(i);
    }

    if (sign == Sign::MINUS) {
        number *= -1;
    }

    return number;
}

BigInteger::operator bool() const {
    return *this != BigInteger(0);
}

std::string BigInteger::toString() const {
    if (BASE == 10) {
        BigInteger number = *this;
        number.DeleteZeros();

        std::string result = "";

        if (number.sign == Sign::MINUS && number != 0) {
            result.push_back('-');
        }

        for (ssize_t i = number.GetSize() - 1; i >= 0; --i) {
            result.push_back('0' + number.GetDigit(i));
        }

        return result;
    }

    std::string reversed_result = "";

    BigInteger temp = *this;

    while (temp != BigInteger(0)) {
        reversed_result.push_back('0' + int(temp % BigInteger(10)));
        temp /= BigInteger(10);
    }

    if (sign == Sign::MINUS) {
        reversed_result.push_back('-');
    }

    std::string result(reversed_result.rbegin(), reversed_result.rend());

    return result;
}

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.sign != rhs.sign) {
        if (rhs.sign == Sign::MINUS) {
            BigInteger rhs_copy = rhs;
            rhs_copy.sign = Sign::PLUS;

            return lhs - rhs_copy;

        } else {
            BigInteger lhs_copy = lhs;
            lhs_copy.sign = Sign::PLUS;

            return rhs - lhs_copy;
        }
    }

    BigInteger result;

    int overflow = 0;
    size_t max_digit_index = std::max(lhs.GetSize(), rhs.GetSize());
    for (size_t index = 0; index < max_digit_index; ++index) {
        int sum = lhs.GetDigit(index) + rhs.GetDigit(index) + overflow;

        if (sum >= BASE) {
            sum -= BASE;

            overflow = 1;

        } else {
            overflow = 0;
        }

        result.AddDigit(sum);
    }

    if (overflow) {
        result.AddDigit(overflow);
    }

    result.sign = (lhs.sign == Sign::PLUS ? Sign::PLUS : Sign::MINUS);

    return result;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.sign != rhs.sign) {
        if (rhs.sign == Sign::MINUS) {
            BigInteger rhs_copy = rhs;
            rhs_copy.sign = Sign::PLUS;

            return lhs + rhs_copy;

        } else {
            BigInteger rhs_copy = rhs;
            rhs_copy.sign = Sign::MINUS;

            return lhs + rhs_copy;
        }
    }

    if (!IsGreaterAbs(lhs, rhs)) {
        return -(rhs - lhs);
    }

    BigInteger result;

    int lack = 0;
    size_t max_digit_index = std::max(lhs.GetSize(), rhs.GetSize());
    for (size_t index = 0; index < max_digit_index; ++index) {
        int difference = lhs.GetDigit(index) - rhs.GetDigit(index) - lack;

        if (difference < 0) {
            difference += BASE;

            lack = 1;

        } else {
            lack = 0;
        }

        result.AddDigit(difference);
    }

    result.sign = (lhs.sign == Sign::PLUS ? Sign::PLUS : Sign::MINUS);

    // If result is zero
    if (result.IsZero()) {
        result.sign = Sign::PLUS;
    }

    // Delete leading zeroes
    result.DeleteZeros();

    return result;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
    // If numbers are small
    if (lhs.GetSize() == 1 && rhs.GetSize() == 1) {
        return BigInteger(lhs.GetDigit(0) * rhs.GetDigit(0));
    }

    BigInteger lhs_copy = lhs;
    BigInteger rhs_copy = rhs;

    size_t n = std::max(lhs_copy.GetSize(), rhs_copy.GetSize());
    n += n % 2; // n must be even

    lhs_copy.digits.resize(n, 0);
    rhs_copy.digits.resize(n, 0);

    BigInteger x1;
    BigInteger x2;
    x2.digits.assign(lhs_copy.digits.begin(), lhs_copy.digits.begin() + n / 2);
    x1.digits.assign(lhs_copy.digits.begin() + n / 2, lhs_copy.digits.end());

    BigInteger y1;
    BigInteger y2;
    y2.digits.assign(rhs_copy.digits.begin(), rhs_copy.digits.begin() + n / 2);
    y1.digits.assign(rhs_copy.digits.begin() + n / 2, rhs_copy.digits.end());

    BigInteger mul1 = x1 * y1;
    BigInteger mul2 = x2 * y2;
    BigInteger mul3 = (x1 + x2) * (y1 + y2) - mul2 - mul1;
    mul3.sign = Sign::PLUS;

    mul3.digits.insert(mul3.digits.begin(), n / 2, 0);
    mul1.digits.insert(mul1.digits.begin(), n, 0);

    BigInteger result = mul1 + mul2 + mul3;

    result.sign = (lhs_copy.sign == rhs_copy.sign ? Sign::PLUS : Sign::MINUS);

    // Delete leading zeroes
    result.DeleteZeros();

    return result;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
    if (!IsGreaterAbs(lhs, rhs)) {
        return BigInteger(0);
    }

    BigInteger dividend = lhs;
    BigInteger divider = rhs;

    dividend.sign = Sign::PLUS;
    divider.sign = Sign::PLUS;

    BigInteger temp;
    std::vector<int> result_digits;
    do {
        int new_digit = 0;

        temp.digits.insert(temp.digits.begin(), dividend.digits.back());
        dividend.digits.pop_back();

        while (temp != 0 && temp >= divider) {
            temp -= divider;
            ++new_digit;
        }

        result_digits.push_back(new_digit);

    } while (dividend.GetSize());

    BigInteger result;
    result.digits.assign(result_digits.rbegin(), result_digits.rend());

    result.sign = (lhs.sign == rhs.sign ? Sign::PLUS : Sign::MINUS);

    // Delete leading zeros
    result.DeleteZeros();

    return result;
}

BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger result = lhs - (lhs / rhs) * rhs;

    return result;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    *this = *this + rhs;

    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
    *this = *this - rhs;

    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    *this = *this * rhs;

    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& rhs) {
    *this = *this / rhs;

    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& rhs) {
    *this = *this % rhs;

    return *this;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger difference = rhs - lhs;

    if (difference.GetSize() > 1 || difference.GetDigit(0) != 0) {
        return difference.sign == Sign::PLUS;
    }

    return false;
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger difference = lhs - rhs;

    if (difference.GetSize() > 1 || difference.GetDigit(0) != 0) {
        return difference.sign == Sign::PLUS;
    }

    return false;
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    return (rhs - lhs).sign == Sign::PLUS;
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    return (lhs - rhs).sign == Sign::PLUS;
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs > rhs) && !(lhs < rhs);
}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs == rhs);
}

const BigInteger& operator+(const BigInteger& number) {
    return number;
}

const BigInteger operator-(const BigInteger& number) {
    BigInteger result = number;
    result.sign = (result.sign == Sign::PLUS ? Sign::MINUS : Sign::PLUS);

    if (result == BigInteger(0)) {
        result.sign = Sign::PLUS;
    }

    return result;
}

BigInteger& BigInteger::operator++() {
    *this += 1;

    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger old_number = *this;

    *this += 1;

    return old_number;
}

BigInteger& BigInteger::operator--() {
    *this -= 1;

    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger old_number = *this;

    *this -= 1;

    return old_number;
}

std::istream& operator>>(std::istream& input_stream, BigInteger& number) {
    std::string decimal_number = "";
    input_stream >> decimal_number;

    if (decimal_number.empty()) {
        number = 0;

        return input_stream;
    }

    number = 0;
    number.digits.pop_back();
    if (BASE == 10) {
        bool has_minus = false;
        for (ssize_t i = decimal_number.size() - 1; i >= 0; --i) {
            if (isdigit(decimal_number[i])) {
                number.AddDigit(decimal_number[i] - '0');

            } else if (decimal_number[i] == '-') {
                has_minus = true;
            }
        }

        if (has_minus && !number.IsZero()) {
            number.sign = Sign::MINUS;

        } else {
            number.sign = Sign::PLUS;
        }

        number.DeleteZeros();

        return input_stream;
    }

    bool is_negative = false;
    if (!decimal_number.empty()) {
        number = 0;
        for (const auto& digit : decimal_number) {
            if (digit == '-') {
                is_negative = true;

            } else {
                number *= BigInteger(10);
                number += BigInteger(digit - '0');
            }
        }
    }

    if (is_negative) {
        number.sign = Sign::MINUS;
    }

    return input_stream;
}

std::ostream& operator<<(std::ostream& output_stream, const BigInteger& number) {
    output_stream << number.toString();

    return output_stream;
}

Sign BigInteger::GetIntSign(int number) {
    return (number < 0 ? Sign::MINUS : Sign::PLUS);
}

size_t BigInteger::GetSize() const {
    return digits.size();
}

int BigInteger::GetDigit(size_t index) const {
    return (index < digits.size() ? digits[index] : 0);
}

void BigInteger::AddDigit(int digit) {
    digits.push_back(digit);
}

bool BigInteger::IsZero() const {
    return GetSize() == 1 && GetDigit(0) == 0;
}

void BigInteger::DeleteZeros() {
    while (GetSize() > 1 && GetDigit(GetSize() - 1) == 0) {
        digits.pop_back();
    }
}
