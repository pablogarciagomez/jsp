/**
 * @file triangular_fuzzy_number.hpp
 * @author Pablo
 * @brief Triangular Fuzzy Number.
 * @version 0.1
 * @date 14-03-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef TRIANGULARFUZZYNUMBER_HPP_
#define TRIANGULARFUZZYNUMBER_HPP_

/**
 * @brief Triangular fuzzy number.
 * 
 * @tparam T type of the inner numbers.
 */
template <typename T> class TriangularFuzzyNumber
{
  public:
    using Type = T; // type of the inner numbers

  private:
    T smallest; // smallest possible value.
    T most_probable; // most probable value.
    T largest; // largest possible value.

  public:
    TriangularFuzzyNumber() : smallest{0}, most_probable{0}, largest{0} {}

    /**
     * @brief Constructs a new TriangularFuzzyNumber.
     * 
     * @param smallest smallest possible value.
     * @param most_probable most probable value.
     * @param largest largest possible value.
     */
    TriangularFuzzyNumber(T smallest, T most_probable, T largest) : smallest{smallest}, most_probable{most_probable}, largest{largest} {}

    /**
     * @brief Returns the smallest possible value.
     * 
     * @return the smallest possible value.
     */
    T GetSmallest() const
    {
        return smallest;
    }

    /**
     * @brief Returns the most probable value.
     * 
     * @return the most probable value.
     */
    T GetMostProbable() const
    {
        return most_probable;
    }

    /**
     * @brief Returns the largest possible value.
     * 
     * @return the largest possible value.
     */
    T GetLargest() const
    {
        return largest;
    }

    /**
     * @brief Returns the expected value.
     * 
     * @return the expected value.
     */
    T ExpectedValue() const
    {
        return (smallest + 2 * most_probable + largest) / 4;
    }

    operator T() const
    {
        return ExpectedValue();
    }

    TriangularFuzzyNumber& operator+=(const TriangularFuzzyNumber& rhs)
    {
        smallest += rhs.smallest;
        most_probable += rhs.most_probable;
        largest += rhs.largest;

        return *this;
    }

    friend TriangularFuzzyNumber operator+(TriangularFuzzyNumber lhs, const TriangularFuzzyNumber& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    TriangularFuzzyNumber& operator-=(const T& rhs)
    {
        smallest -= rhs;
        most_probable -= rhs;
        largest -= rhs;

        return *this;
    }

    friend TriangularFuzzyNumber operator-(TriangularFuzzyNumber lhs, const T& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    TriangularFuzzyNumber& operator*=(const T& rhs)
    {
        smallest *= rhs;
        most_probable *= rhs;
        largest *= rhs;

        return *this;
    }

    friend TriangularFuzzyNumber operator*(TriangularFuzzyNumber lhs, const T& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend bool operator==(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.smallest == rhs.smallest && lhs.most_probable == rhs.most_probable && lhs.largest == rhs.largest;
    };

    friend bool operator!=(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.smallest != rhs.smallest || lhs.most_probable != rhs.most_probable || lhs.largest != rhs.largest;
    };

    friend bool operator<(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.ExpectedValue() < rhs.ExpectedValue();
    };

    friend bool operator>(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.ExpectedValue() > rhs.ExpectedValue();
    };

    friend bool operator<=(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.ExpectedValue() <= rhs.ExpectedValue();
    };

    friend bool operator>=(const TriangularFuzzyNumber& lhs, const TriangularFuzzyNumber& rhs)
    {
        return lhs.ExpectedValue() >= rhs.ExpectedValue();
    };

    friend std::istream& operator>>(std::istream& is, TriangularFuzzyNumber& n)
    {
        char c;
        if (is >> c && c != '(') {
            is.setstate(std::ios::failbit);
            return is;
        }
        is >> n.smallest;
        if (is >> c && c != ',') {
            is.setstate(std::ios::failbit);
            return is;
        }
        is >> n.most_probable;
        if (is >> c && c != ',') {
            is.setstate(std::ios::failbit);
            return is;
        }
        is >> n.largest;
        if (is >> c && c != ')') {
            is.setstate(std::ios::failbit);
            return is;
        }
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const TriangularFuzzyNumber& n)
    {
        return os << '(' << n.smallest << ',' << n.most_probable << ',' << n.largest << ')';
    }
};

namespace std
{
    template <typename T> const TriangularFuzzyNumber<T> max(const TriangularFuzzyNumber<T>& a, const TriangularFuzzyNumber<T>& b)
    {
        return TriangularFuzzyNumber<T>(
            std::max(a.GetSmallest(), b.GetSmallest()), std::max(a.GetMostProbable(), b.GetMostProbable()), std::max(a.GetLargest(), b.GetLargest()));
    }

    template <class T, class Compare>
    const TriangularFuzzyNumber<T>& max(const TriangularFuzzyNumber<T>& a, const TriangularFuzzyNumber<T>& b, Compare)
    {
        return (comp(a, b)) ? b : a;
    }
}

#endif /* TRIANGULARFUZZYNUMBER_HPP_ */
