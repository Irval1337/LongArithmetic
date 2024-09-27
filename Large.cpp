#include "Large.h"

Large::Large(const std::string& value) noexcept {
    sign_ = Sign::Plus;

    int64_t i;
    for(i = 0; i < value.size(); ++i) {
        if (value[i] == '-') {
            sign_ = sign_ == Sign::Plus ? Sign::Minus : Sign::Plus;
        } else if (value[i] != '+') {
            break;
        }
    }

    uint64_t max_power = 1;
    while (max_power < base_) {
        max_power *= 10;
    }

    uint64_t digit = 0, power = 1;
    for(int64_t j = static_cast<int64_t>(value.size()) - 1; j >= i; --j) {
        digit += power * (value[j] - '0');
        power *= 10;
        if (power == max_power) {
            digits_.push_back(digit);
            power = 1, digit = 0;
        }
    }
    if (digit != 0 || digits_.empty()) {
        digits_.push_back(digit);
    }
}

bool Large::operator<(const Large& other) const noexcept {
    if (*this == other) {
        return false;
    }
    if (other.IsZero()) {
        return sign_ == Sign::Minus;
    }
    if (sign_ != other.sign_) {
        return sign_ == Sign::Minus;
    }
    if (sign_ == Sign::Minus) {
        return !(abs(*this) < abs(other));
    }
    if (digits_.size() != other.digits_.size()) {
        return digits_.size() < other.digits_.size();
    }

    for(int64_t i = static_cast<int64_t>(digits_.size()) - 1; i >= 0; --i) {
        if (digits_[i] != other.digits_[i]) {
            return digits_[i] < other.digits_[i];
        }
    }
    return false;
}

Large Large::operator+(const Large& other) const noexcept {
    if (sign_ != other.sign_) {
        return sign_ == Sign::Plus ? *this - (-other) : other - (-*this);
    }
    if (IsZero()) {
        return other;
    }
    if (other.IsZero()) {
        return *this;
    }
    uint64_t carry = 0;
    Large result;
    result.sign_ = sign_;
    result.digits_.clear();

    for(size_t i = 0; i < std::max(digits_.size(), other.digits_.size()); ++i) {
        result.digits_.push_back((carry + DigitAt(i) + other.DigitAt(i)) % base_);
        carry = (carry + DigitAt(i) + other.DigitAt(i)) / base_;
    }
    if (carry != 0) {
        result.digits_.push_back(carry);
    }
    return result;
}

Large Large::operator-(const Large& other) const noexcept {
    if (sign_ != other.sign_) {
        return sign_ == Sign::Minus ? -(-*this + other) : *this + -other;
    }
    if (digits_ == other.digits_) {
        return 0;
    }
    if (abs(*this) < abs(other)) {
        return -(other - *this);
    }
    Large result;
    result.sign_ = sign_;
    result.digits_.clear();
    bool next_minus = false;
    for(size_t i = 0; i < digits_.size(); ++i) {
        if (DigitAt(i) < other.DigitAt(i) + (next_minus ? 1 : 0)) {
            result.digits_.push_back(DigitAt(i) + base_ - other.DigitAt(i) - (next_minus ? 1 : 0));
            next_minus = true;
        } else {
            result.digits_.push_back(DigitAt(i) - other.DigitAt(i) - (next_minus ? 1 : 0));
            next_minus = false;
        }
    }
    while (result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    return result;
}

Large Large::SimpleMult(const Large& lhs, const Large& rhs) const noexcept {
    Large result;
    result.digits_.clear();
    uint64_t carry = 0;
    for(const uint64_t digit : lhs.digits_) {
        result.digits_.push_back((digit * rhs.digits_[0] + carry) % base_);
        carry = (digit * rhs.digits_[0] + carry) / base_;
    }
    if (carry != 0) {
        result.digits_.push_back(carry);
    }
    return result;
}

Large Large::MultByBase(int64_t power) const noexcept {
    Large other = *this;
    std::reverse(other.digits_.begin(), other.digits_.end());
    for(int i = 0; i < std::abs(power); ++i) {
        if (power > 0) {
            other.digits_.push_back(0);
        } else if (!other.digits_.empty()) {
            other.digits_.pop_back();
        }
    }
    std::reverse(other.digits_.begin(), other.digits_.end());
    while (!other.digits_.empty() && other.digits_.back() == 0) {
        other.digits_.pop_back();
    }
    if (other.digits_.empty()) {
        other.digits_.push_back(0);
    }
    return other;
}

Large Large::operator*(const Large& other) const noexcept {
    if (IsZero() || other.IsZero()) {
        return 0;
    }
    if (sign_ != other.sign_) {
        return -(abs(*this) * abs(other));
    }
    if (sign_ == Sign::Minus) {
        return -*this * -other;
    }
    if (digits_.size() == 1) {
        return SimpleMult(other, *this);
    }
    if (other.digits_.size() == 1) {
        return SimpleMult(*this, other);
    }
    int64_t n = static_cast<int64_t>(std::max(digits_.size(), other.digits_.size()) / 2);
    Large a = this->MultByBase(-n);
    Large b = *this - a.MultByBase(n);
    Large c = other.MultByBase(-n);
    Large d = other - c.MultByBase(n);

    return (a * c).MultByBase(2 * n) + ((a + b) * (c + d) -  a * c - b * d).MultByBase(n) + b * d;
}

Large Large::operator/(const Large& other) const {
    if (other.IsZero()) {
        throw std::logic_error("Division by zero");
    }
    if (sign_ != other.sign_) {
        return -(abs(*this) / abs(other));
    }
    if (sign_ == Sign::Minus) {
        return -*this / -other;
    }
    if (*this < other) {
        return 0;
    }
    Large result;
    result.digits_ = std::vector<uint64_t>(digits_.size(), 0);
    for(int64_t i = static_cast<int64_t>(digits_.size()) - 1; i >= 0; --i) {
        uint64_t L = 0, R = base_, M;
        while (L < R - 1) {
            M = L + (R - L) / 2;
            result.digits_[i] = M;
            if (result * other > *this) {
                R = M;
            } else {
                L = M;
            }
        }
        result.digits_[i] = L;
    }
    while (!result.digits_.empty() && result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    return result;
}

Large Large::operator%(const Large& other) const {
    return *this - *this / other * other;
}

Large abs(const Large& large) noexcept {
    Large other = large;
    other.sign_ = Large::Sign::Plus;
    return other;
}

std::string to_string(const Large& num) noexcept {
    std::string result;
    if (num.sign_ == Large::Sign::Minus) {
        result.push_back('-');
    }
    for(int64_t i = static_cast<int64_t>(num.digits_.size()) - 1; i >= 0; --i) {
        if (i + 1 != num.digits_.size()) {
            result += std::string(9 - std::to_string(num.digits_[i]).size(), '0');
        }
        result += std::to_string(num.digits_[i]);
    }
    return result;
}

Large pow(const Large& num, const Large& n) {
    if (n < 0) {
        throw std::logic_error("Power must be positive");
    }
    if (num.IsZero() || num.digits_.size() == 1 && num.digits_[0] == 1) {
        return num;
    }
    if (n == 0) {
        return 1;
    }
    if (n.digits_[0] % 2 == 1) {
        return num * pow(num, n - 1);
    }
    Large tmp = pow(num, n / 2);
    return tmp * tmp;
}

std::istream& operator>>(std::istream& is, Large& num) noexcept {
    std::string s;
    is >> s;
    num = Large(s);
    return is;
}

std::ostream& operator<<(std::ostream& os, const Large& num) noexcept {
    return os << to_string(num);
}
