#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum Sign { Plus, Minus };

class large {
private:
    string num;
public:
    Sign sign;
    large() {
        num = "0";
        sign = Sign::Plus;
    }
    large(string value) {
        num = value;
        sign = value[0] == '-' ? Sign::Minus : Sign::Plus;
        if (sign == Sign::Minus)
            num.erase(0, 1);
    }
    large(long long value) {
        num = to_string(value);
        sign = value < 0 ? Sign::Minus : Sign::Plus;
        if (sign == Sign::Minus)
            num.erase(0, 1);
    }
    large operator+=(const large& value) {
        return *this = (*this + value);
    }
    large operator-=(const large& value) {
        return *this = (*this - value);
    }
    large operator*=(const large& value) {
        return *this = (*this * value);
    }
    large operator/=(const large& value) {
        return *this = (*this / value);
    }
    large operator%=(const large& value) {
        return *this = (*this % value);
    }
    large operator++(int d) {
        auto tmp = *this;
        *this += large(1);
        return tmp;
    }
    large operator--(int d) {
        auto tmp = *this;
        *this -= large(1);
        return tmp;
    }
    bool operator== (large val) {
        string num1 = num, num2 = string(val);
        return num1 == num2;
    }
    bool operator!= (large val) {
        string num1 = num, num2 = string(val);
        return num1 != num2;
    }
    bool operator<= (large val) {
        return large((sign == Minus ? "-" : "") + num) < val || large((sign == Minus ? "-" : "") + num) == val;
    }
    bool operator>= (large val) {
        return large((sign == Minus ? "-" : "") + num) > val || large((sign == Minus ? "-" : "") + num) == val;
    }
    bool operator> (large val) {
        if (val.sign != sign)
            return sign == Sign::Plus;
        bool isMinus = val.sign == Sign::Minus;
        string num1 = num, num2 = string(val);
        if (num1.length() != num2.length())
            return isMinus ? num1.length() < num2.length() : num1.length() > num2.length();
        else if (num1 == num2)
            return false;
        for (int i = 0; i < num1.length(); i++) {
            if (num1[i] != num2[i])
                return isMinus ? num1[i] < num2[i] : num1[i] > num2[i];
        }
    }
    bool operator< (large val) {
        if (val.sign != sign)
            return sign == Sign::Minus;
        bool isMinus = val.sign == Sign::Minus;
        string num1 = num, num2 = string(val);
        if (num1.length() != num2.length())
            return isMinus ? num1.length() > num2.length() : num1.length() < num2.length();
        else if (num1 == num2)
            return false;
        for (int i = 0; i < num1.length(); i++) {
            if (num1[i] != num2[i])
                return isMinus ? num1[i] > num2[i] : num1[i] < num2[i];
        }
    }
    large operator+ (large val) {
        if (val.sign != sign)
            return val.sign == Sign::Plus ? val - large(num) : large(num) - large(string(val));
        bool isMinus = val.sign == Sign::Minus;
        string num2 = string(val), num1 = num;;
        reverse(num1.begin(), num1.end());
        reverse(num2.begin(), num2.end());
        int carry = 0, n = max(num1.length(), num2.length()) + 1;
        num1 += string(n - num1.length(), '0');
        num2 += string(n - num2.length(), '0');
        string ans = string(n, '0');
        for (int i = 0; i < n; i++) {
            ans[i] += num1[i] + num2[i] - 2 * '0';
            if (ans[i] > '9') {
                ans[i + 1]++;
                ans[i] -= 10;
            }
        }
        if (ans[n - 1] == '0') ans.erase(n - 1, 1);
        reverse(ans.begin(), ans.end());
        if (isMinus)
            ans = "-" + ans;
        return large(ans);
    }
    large operator- (large val) {
        if (sign == Sign::Minus && val.sign == Sign::Plus)
            return large("-" + string(val)) + *this;
        else if (sign == Sign::Plus && val.sign == Sign::Minus)
            return *this + large(string(val));
        else if (sign == Sign::Minus && val.sign == Sign::Minus)
            return large(string(val)) - large(num);
        string num1 = num, num2 = string(val);
        if (num1 == num2) {
            return large(0);
        }
        bool isMunus = val > * this;
        if (isMunus)
            swap(num1, num2);
        reverse(num1.begin(), num1.end());
        reverse(num2.begin(), num2.end());
        string ans = string(num1.length(), '0');
        num2 += string(num1.length() - num2.length(), '0');
        for (int i = 0; i < num1.length(); i++) {
            ans[i] += num1[i] - num2[i];
            if (ans[i] < '0') {
                ans[i] += 10;
                ans[i + 1]--;
            }
        }
        reverse(ans.begin(), ans.end());
        int i = 0;
        while (ans[i] == '0') i++;
        return large((isMunus ? "-" : "") + ans.substr(i));
    }
    large operator* (large val) {
        bool isMinus = val.sign != sign;
        string num1 = num, num2 = string(val);
        large ans;
        if (num1 == "0" || num2 == "0")
            return large(0);
        if (num2.length() > 1) {
            vector<large> nums;
            for (int i = num2.length() - 1; i >= 0; i--) {
                nums.push_back(*this * large(num2.substr(i, 1)));
                if (nums[num2.length() - 1 - i] != large(0))
                    nums[num2.length() - 1 - i] = large(string(nums[num2.length() - 1 - i]) + string(num2.length() - 1 - i, '0'));
            }
            for (int i = 0; i < nums.size(); i++) ans += nums[i];
            return large((isMinus ? "-" : "") + string(ans));
        }
        else {
            for (int i = 0; i < atoi(num2.c_str()); i++)
                ans = ans + *this;
            return large((isMinus ? "-" : "") + string(ans));
        }
    }
    large operator/ (long long val) {
        bool isMinus = val >= 0 != sign == Sign::Plus;
        string num1 = num, num2 = to_string(val), ans = "";
        val = abs(val);
        long long tmp, i = 1;
        for (; (i < num1.length()) && (tmp = atoi(num1.substr(0, i).c_str()), tmp < val); i++) {}
        tmp = atoi(num1.substr(0, i).c_str());
        bool moved = false;
        if (val > tmp)
            return large(0);
        else {
            ans += to_string(tmp / val);
            tmp = tmp % val;
            num1.erase(0, i);
            while (num1.length() > 0) {
                if (tmp == 0 && num1 == string(num1.length(), '0'))
                    return large(ans + num1);
                if (tmp != 0) {
                    tmp *= 10;
                    tmp += atoi(num1.substr(0, 1).c_str());
                    num1.erase(0, 1);
                    ans += to_string(tmp / val);
                    tmp = tmp % val;
                }
                else {
                    ans += to_string(atoi(num1.substr(0, 1).c_str()) / val);
                    tmp = atoi(num1.substr(0, 1).c_str()) % val;
                    num1.erase(0, 1);
                }
            }
        }
        return large((isMinus ? "-" : "") + ans);
    }
    large operator/ (large val) {
        //if (string(val) == "0")
        //    throw exception("Division by zero");
        bool isMinus = val.sign != sign;
        string num1 = num, num2 = string(val);
        large L = large(0), R = large(num1), m;
        while (L < R - large(1)) {
            m = L + (R - L) / 2;
            if (m * large(num2) >= large(this->num)) R = m;
            else L = m;
        }
        if (R * large(num2) > large(this->num))
            R -= large(1);
        return large((isMinus && R != large(0) ? "-" : "") + string(R));
    }
    large operator% (large val) {
        bool isMinus = val.sign != sign;
        val = large(string(val));
        auto ans = large(this->num) - large(this->num) / val * val;
        return (isMinus && ans != large(0) ? "-" : "") + string(ans);
    }
    operator string() {
        return num;
    }
    string ToString() {
        return (sign == Sign::Minus ? "-" : "") + num;
    }
    large pow(large val) {
        large ans = 1;
        bool isMinus = sign == Sign::Minus && large(string(val)) % large(2) != large(0);
        if (val.sign == Sign::Plus) {
            for (large i; i < val; i = i + large(1))
                ans = ans * large(this->num);
        }
        else {
            for (large i; i < large(string(val)); i++)
                ans = ans / large(this->num);
        }
        if (isMinus && string(ans) != "0")
            ans.sign = Sign::Minus;
        return ans;
    }
};
