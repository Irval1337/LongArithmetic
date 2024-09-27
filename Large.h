#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <utility>
#include <stdexcept>

class Large {
public:
    Large() : digits_({ 0 }), sign_(Sign::Plus) {}

    Large(const std::string& value) noexcept;

    Large(int64_t value) {
        *this = Large(std::to_string(value));
    }

    Large(const Large& other) : digits_(other.digits_), sign_(other.sign_) {}

    Large(Large&& other) noexcept : digits_(std::exchange(other.digits_, std::vector<uint64_t>())),
                                    sign_(std::exchange(other.sign_, Sign::Plus)) {}

    ~Large() = default;

    bool operator==(const Large& other) const noexcept {
        if (IsZero() && other.IsZero()) {
            return true;
        }
        return digits_ == other.digits_ && sign_ == other.sign_;
    }

    bool operator!=(const Large& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const Large& other) const noexcept;

    bool operator<=(const Large& other) const noexcept {
        return *this == other || *this < other;
    }

    bool operator>(const Large& other) const noexcept {
        return !(*this <= other);
    }

    bool operator>=(const Large& other) const noexcept {
        return *this == other || *this > other;
    }

    Large operator-() const noexcept {
        Large other = *this;
        other.sign_ = other.sign_ == Sign::Plus ? Sign::Minus : Sign::Plus;
        return other;
    }

    Large operator+() const noexcept {
        return *this;
    }

    Large& operator=(const Large& other) noexcept {
        return *this = Large(other);
    }

    Large& operator=(Large&& other) noexcept {
        std::swap(sign_, other.sign_);
        digits_.swap(other.digits_);
        return *this;
    }

    Large operator++(int32_t) noexcept {
        Large tmp = *this;
        *this += 1;
        return tmp;
    }

    Large operator--(int32_t) noexcept {
        Large tmp = *this;
        *this -= 1;
        return tmp;
    }

    Large& operator++() noexcept {
        return *this += 1;
    }

    Large& operator--() noexcept {
        return *this -= 1;
    }

    Large operator+(const Large& other) const noexcept;

    Large operator-(const Large& other) const noexcept;

    Large operator*(const Large& other) const noexcept;

    Large operator/(const Large& other) const;

    Large operator%(const Large& other) const;

    Large& operator+=(const Large& other) noexcept {
        return *this = *this + other;
    }

    Large& operator-=(const Large& other) noexcept {
        return *this = *this - other;
    }

    Large& operator*=(const Large& other) noexcept {
        return *this = *this * other;
    }

    Large& operator/=(const Large& other) {
        return *this = *this / other;
    }

    Large& operator%=(const Large& other) {
        return *this = *this % other;
    }

    friend Large abs(const Large& num) noexcept;

    friend std::string to_string(const Large& num) noexcept;

    friend Large pow(const Large& num, const Large& n);

    friend std::istream& operator>>(std::istream& is, Large& num) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Large& num) noexcept;

private:
    enum Sign { Plus, Minus };

    Sign sign_;
    std::vector<uint64_t> digits_;
    const uint64_t base_ = 1e9;

    bool IsZero() const noexcept {
        return digits_.size() == 1 && digits_[0] == 0;
    }

    uint64_t DigitAt(size_t ind) const noexcept {
        if (ind < digits_.size()) {
            return digits_[ind];
        }
        return 0;
    }

    Large SimpleMult(const Large& lhs, const Large& rhs) const noexcept;

    Large MultByBase(int64_t power) const noexcept;
};
