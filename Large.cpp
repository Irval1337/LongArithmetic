#include "Large.h"

#include <complex>
#include <cmath>
#include <algorithm>

namespace {
    inline void TrimVec(std::vector<uint64_t> &a) {
        while (a.size() > 1 && a.back() == 0) a.pop_back();
    }

    inline uint64_t DivSmallVec(std::vector<uint64_t> &a, uint64_t d, uint64_t base) {
        __uint128_t rem = 0;
        for (int64_t i = (int64_t) a.size() - 1; i >= 0; --i) {
            __uint128_t cur = rem * base + a[(size_t) i];
            a[(size_t) i] = (uint64_t) (cur / d);
            rem = cur % d;
        }
        TrimVec(a);
        return (uint64_t) rem;
    }

    inline void DivMod_Positive_Knuth(const std::vector<uint64_t> &A,
                                      const std::vector<uint64_t> &B,
                                      uint64_t base,
                                      std::vector<uint64_t> &Q_out,
                                      std::vector<uint64_t> &R_out) {
        if (B.size() == 1) {
            uint64_t d = B[0];
            Q_out.assign(A.size(), 0);
            __uint128_t rem = 0;
            for (int64_t i = (int64_t) A.size() - 1; i >= 0; --i) {
                __uint128_t cur = rem * base + A[(size_t) i];
                Q_out[(size_t) i] = (uint64_t) (cur / d);
                rem = cur % d;
            }
            R_out.assign(1, (uint64_t) rem);
            TrimVec(Q_out);
            TrimVec(R_out);
            return;
        }

        std::vector<uint64_t> u = A, v = B;
        TrimVec(u);
        TrimVec(v);
        const size_t n = v.size();
        if (u.size() < n) {
            Q_out.assign(1, 0);
            R_out = A;
            TrimVec(R_out);
            return;
        }
        const size_t m = u.size() - n;

        uint64_t d = (uint64_t) (base / (v.back() + 1));
        if (d > 1) {
            __uint128_t carry = 0;
            for (size_t i = 0; i < u.size(); ++i) {
                __uint128_t cur = (__uint128_t) u[i] * d + carry;
                u[i] = (uint64_t) (cur % base);
                carry = cur / base;
            }
            if (carry) u.push_back((uint64_t) carry);

            carry = 0;
            for (size_t i = 0; i < v.size(); ++i) {
                __uint128_t cur = (__uint128_t) v[i] * d + carry;
                v[i] = (uint64_t) (cur % base);
                carry = cur / base;
            }
        }
        u.push_back(0);

        std::vector<uint64_t> q(m + 1, 0);

        auto subMulAt = [&](size_t i, uint64_t qhat) -> bool {
            __uint128_t carry = 0;
            uint64_t borrow = 0;
            for (size_t j = 0; j < n; ++j) {
                __uint128_t mul = (__uint128_t) v[j] * qhat + carry;
                uint64_t lo = (uint64_t) (mul % base);
                carry = mul / base;

                __int128 diff = (__int128) u[i + j] - lo - borrow;
                if (diff < 0) {
                    u[i + j] = (uint64_t) (diff + base);
                    borrow = 1;
                } else {
                    u[i + j] = (uint64_t) diff;
                    borrow = 0;
                }
            }
            __int128 diff = (__int128) u[i + n] - (uint64_t) carry - borrow;
            if (diff < 0) {
                u[i + n] = (uint64_t) (diff + base);
                return true;
            } else {
                u[i + n] = (uint64_t) diff;
                return false;
            }
        };

        auto addAt = [&](size_t i) {
            __uint128_t carry = 0;
            for (size_t j = 0; j < n; ++j) {
                __uint128_t sum = (__uint128_t) u[i + j] + v[j] + carry;
                if (sum >= base) {
                    u[i + j] = (uint64_t) (sum - base);
                    carry = 1;
                } else {
                    u[i + j] = (uint64_t) sum;
                    carry = 0;
                }
            }
            u[i + n] += (uint64_t) carry;
        };

        for (int64_t i = (int64_t) m; i >= 0; --i) {
            __uint128_t numer = (__uint128_t) u[(size_t) i + n] * base + u[(size_t) i + n - 1];
            uint64_t den = v[n - 1];
            uint64_t qhat = (uint64_t) (numer / den);
            if (qhat >= base) qhat = (uint64_t) base - 1;

            uint64_t v0 = (n >= 2) ? v[n - 2] : 0;
            __uint128_t rhat = numer % den;
            while ((__uint128_t) qhat * v0 >
                   rhat * base + ((n >= 2) ? u[(size_t) i + n - 2] : 0)) {
                --qhat;
                rhat += den;
                if (rhat >= base) break;
            }

            if (subMulAt((size_t) i, qhat)) {
                --qhat;
                addAt((size_t) i);
            }
            q[(size_t) i] = qhat;
        }

        std::vector<uint64_t> r(u.begin(), u.begin() + n);
        if (d > 1) DivSmallVec(r, d, base);

        TrimVec(q);
        TrimVec(r);
        Q_out.swap(q);
        R_out.swap(r);
    }

} // namespace

Large::Large(const std::string &value) noexcept {
    sign_ = Sign::Plus;

    int64_t i;
    for (i = 0; i < value.size(); ++i) {
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
    for (int64_t j = static_cast<int64_t>(value.size()) - 1; j >= i; --j) {
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

bool Large::operator<(const Large &other) const noexcept {
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

    for (int64_t i = static_cast<int64_t>(digits_.size()) - 1; i >= 0; --i) {
        if (digits_[i] != other.digits_[i]) {
            return digits_[i] < other.digits_[i];
        }
    }
    return false;
}

Large Large::operator+(const Large &other) const noexcept {
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

    for (size_t i = 0; i < std::max(digits_.size(), other.digits_.size()); ++i) {
        result.digits_.push_back((carry + DigitAt(i) + other.DigitAt(i)) % base_);
        carry = (carry + DigitAt(i) + other.DigitAt(i)) / base_;
    }
    if (carry != 0) {
        result.digits_.push_back(carry);
    }
    return result;
}

Large Large::operator-(const Large &other) const noexcept {
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
    for (size_t i = 0; i < digits_.size(); ++i) {
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

Large Large::operator*(const Large &other) const noexcept {
    if (IsZero() || other.IsZero()) return Large(0);

    if (sign_ != other.sign_) {
        Large tmp = abs(*this).operator*(abs(other));
        tmp.sign_ = Sign::Minus;
        if (tmp.digits_.size() == 1 && tmp.digits_[0] == 0) tmp.sign_ = Sign::Plus;
        return tmp;
    }
    const size_t n = digits_.size();
    const size_t m = other.digits_.size();

    if (std::min(n, m) <= 32) {
        Large r = MulSchoolbook(other);
        r.sign_ = Sign::Plus;
        return r;
    }
    Large r = MulFFT(other);
    r.sign_ = Sign::Plus;
    return r;
}

Large Large::operator/(const Large &other) const {
    if (other.IsZero()) throw std::logic_error("Division by zero");
    if (IsZero()) return 0;

    Large A = abs(*this);
    Large B = abs(other);
    Large Q, R;
    DivMod_BZ_Positive(A, B, Q, R);

    if (!(Q.digits_.size() == 1 && Q.digits_[0] == 0) && (sign_ != other.sign_)) {
        Q.sign_ = Sign::Minus;
    } else {
        Q.sign_ = Sign::Plus;
    }
    return Q;
}

Large Large::operator%(const Large &other) const {
    if (other.IsZero()) throw std::logic_error("Division by zero");
    if (IsZero()) return 0;

    Large A = abs(*this);
    Large B = abs(other);
    Large Q, R;
    DivMod_BZ_Positive(A, B, Q, R);

    if (!(R.digits_.size() == 1 && R.digits_[0] == 0)) {
        R.sign_ = sign_;
    } else {
        R.sign_ = Sign::Plus;
    }
    return R;
}

Large abs(const Large &large) noexcept {
    Large other = large;
    other.sign_ = Large::Sign::Plus;
    return other;
}

std::string to_string(const Large &num) noexcept {
    std::string result;
    if (num.sign_ == Large::Sign::Minus) {
        result.push_back('-');
    }
    for (int64_t i = static_cast<int64_t>(num.digits_.size()) - 1; i >= 0; --i) {
        if (i + 1 != num.digits_.size()) {
            result += std::string(9 - std::to_string(num.digits_[i]).size(), '0');
        }
        result += std::to_string(num.digits_[i]);
    }
    return result;
}

Large pow(const Large &num, const Large &n) {
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

std::istream &operator>>(std::istream &is, Large &num) noexcept {
    std::string s;
    is >> s;
    num = Large(s);
    return is;
}

std::ostream &operator<<(std::ostream &os, const Large &num) noexcept {
    return os << to_string(num);
}

void Large::Trim() noexcept {
    while (digits_.size() > 1 && digits_.back() == 0) {
        digits_.pop_back();
    }
    if (digits_.size() == 1 && digits_[0] == 0) {
        sign_ = Sign::Plus;
    }
}

Large Large::ShiftLimbs(const Large &x, size_t k) const noexcept {
    if (x.IsZero() || k == 0) return x;
    Large r = x;
    r.digits_.insert(r.digits_.begin(), k, 0);
    return r;
}

Large Large::SliceLow(const Large &x, size_t m) const noexcept {
    Large r;
    r.sign_ = Sign::Plus;
    r.digits_.assign(x.digits_.begin(),
                     x.digits_.begin() + std::min(m, x.digits_.size()));
    if (r.digits_.empty()) r.digits_.push_back(0);
    r.Trim();
    return r;
}

Large Large::SliceHigh(const Large &x, size_t m) const noexcept {
    Large r;
    r.sign_ = Sign::Plus;
    if (x.digits_.size() > m) {
        r.digits_.assign(x.digits_.begin() + m, x.digits_.end());
    } else {
        r.digits_.assign(1, 0);
    }
    r.Trim();
    return r;
}

size_t Large::NextPow2(size_t n) noexcept {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

void Large::FFT(std::vector<std::complex<long double>> &a, bool invert) {
    const long double PI = std::acos((long double) -1);
    size_t n = a.size();

    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }

    for (size_t len = 2; len <= n; len <<= 1) {
        long double ang = 2.0L * PI / len * (invert ? -1.0L : 1.0L);
        std::complex<long double> wlen(std::cos(ang), std::sin(ang));
        for (size_t i = 0; i < n; i += len) {
            std::complex<long double> w(1.0L, 0.0L);
            size_t half = len >> 1;
            for (size_t j = 0; j < half; ++j) {
                auto u = a[i + j];
                auto v = a[i + j + half] * w;
                a[i + j] = u + v;
                a[i + j + half] = u - v;
                w *= wlen;
            }
        }
    }
    if (invert) {
        for (auto &x: a) x /= static_cast<long double>(n);
    }
}

Large Large::MulSchoolbook(const Large &other) const noexcept {
    if (IsZero() || other.IsZero()) return Large(0);

    Large res;
    res.sign_ = (sign_ == other.sign_) ? Sign::Plus : Sign::Minus;
    res.digits_.assign(digits_.size() + other.digits_.size(), 0);

    const uint64_t B = base_;
    for (size_t i = 0; i < digits_.size(); ++i) {
        __uint128_t carry = 0;
        __uint128_t ai = digits_[i];
        for (size_t j = 0; j < other.digits_.size(); ++j) {
            __uint128_t cur = (__uint128_t) res.digits_[i + j] + ai * other.digits_[j] + carry;
            res.digits_[i + j] = (uint64_t) (cur % B);
            carry = cur / B;
        }
        if (carry) {
            res.digits_[i + other.digits_.size()] += (uint64_t) carry;
        }
    }
    res.Trim();
    return res;
}

Large Large::MulFFT(const Large &other) const {
    if (IsZero() || other.IsZero()) return Large(0);

    const uint32_t SPLIT = 1000;
    int chunks = 0;
    uint64_t t = 1;
    while (t < base_) {
        t *= SPLIT;
        ++chunks;
    }
    std::vector<uint64_t> aSmall;
    std::vector<uint64_t> bSmall;
    aSmall.reserve(digits_.size() * chunks);
    bSmall.reserve(other.digits_.size() * chunks);

    auto explode = [&](const std::vector<uint64_t> &in, std::vector<uint64_t> &out) {
        for (uint64_t d: in) {
            for (int k = 0; k < chunks; ++k) {
                out.push_back(d % SPLIT);
                d /= SPLIT;
            }
        }
        while (out.size() > 1 && out.back() == 0) out.pop_back();
    };

    explode(digits_, aSmall);
    explode(other.digits_, bSmall);

    size_t need = aSmall.size() + bSmall.size();
    size_t n = NextPow2(need);

    std::vector<std::complex<long double>> fa(n), fb(n);
    for (size_t i = 0; i < aSmall.size(); ++i) fa[i] = (long double) aSmall[i];
    for (size_t i = 0; i < bSmall.size(); ++i) fb[i] = (long double) bSmall[i];

    FFT(fa, false);
    FFT(fb, false);
    for (size_t i = 0; i < n; ++i) fa[i] *= fb[i];
    FFT(fa, true);

    std::vector<uint64_t> c(n);
    for (size_t i = 0; i < n; ++i) {
        long double val = fa[i].real();
        c[i] = static_cast<uint64_t>(std::llround(val));
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < c.size(); ++i) {
        unsigned long long cur = c[i] + carry;
        c[i] = cur % SPLIT;
        carry = cur / SPLIT;
    }
    while (carry) {
        c.push_back(carry % SPLIT);
        carry /= SPLIT;
    }
    while (c.size() > 1 && c.back() == 0) c.pop_back();

    std::vector<uint64_t> powv(chunks, 1);
    for (int i = 1; i < chunks; ++i) powv[i] = powv[i - 1] * SPLIT;

    Large res;
    res.sign_ = (sign_ == other.sign_) ? Sign::Plus : Sign::Minus;
    res.digits_.clear();
    res.digits_.reserve((c.size() + chunks - 1) / chunks);

    for (size_t i = 0; i < c.size(); i += (size_t) chunks) {
        __uint128_t block = 0;
        for (int j = 0; j < chunks; ++j) {
            if (i + (size_t) j < c.size()) {
                block += (__uint128_t) c[i + (size_t) j] * powv[j];
            }
        }
        res.digits_.push_back((uint64_t) block);
    }

    res.Trim();
    return res;
}

void Large::DivMod_BZ_Positive(const Large &a, const Large &b, Large &q, Large &r) const {
    if (a < b) {
        q = 0;
        r = a;
        return;
    }

    const size_t n = b.digits_.size();

    static const size_t KNUTH_THRESHOLD = 64;
    if (n <= KNUTH_THRESHOLD) {
        std::vector<uint64_t> Qv, Rv;
        DivMod_Positive_Knuth(a.digits_, b.digits_, base_, Qv, Rv);
        q.sign_ = Sign::Plus;
        q.digits_ = std::move(Qv);
        q.Trim();
        r.sign_ = Sign::Plus;
        r.digits_ = std::move(Rv);
        r.Trim();
        return;
    }

    const size_t m = (n + 1) >> 1;

    Large b0 = SliceLow(b, m);
    Large b1 = SliceHigh(b, m);

    Large u0 = SliceLow(a, m);
    Large a_shift_m = SliceHigh(a, m);
    Large u1 = SliceLow(a_shift_m, m);
    Large u2 = SliceHigh(a, 2 * m);

    Large q1, r1;
    DivMod_BZ_Positive(u2, b1, q1, r1);

    Large w = q1 * b0;

    Large x = ShiftLimbs(r1, m) + u1;
    while (x < w) {
        q1 = q1 - 1;
        x = x + b;
    }
    x = x - w;

    Large q0, r0;
    DivMod_BZ_Positive(x, b1, q0, r0);

    Large w0 = q0 * b0;

    Large y = ShiftLimbs(r0, m) + u0;
    while (y < w0) {
        q0 = q0 - 1;
        y = y + b;
    }
    y = y - w0;

    q = ShiftLimbs(q1, m) + q0;
    q.sign_ = Sign::Plus;
    q.Trim();
    r = y;
    r.sign_ = Sign::Plus;
    r.Trim();
}
