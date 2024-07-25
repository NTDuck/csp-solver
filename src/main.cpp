#include <cctype>
#include <cstddef>
#include <cstdint>

#include <tuple>

#include <iostream>

#include <array>


namespace aux::details {
    /**
     * @brief Calculate binomial coefficient.
     * 
     * @see https://stackoverflow.com/questions/44718971/calculate-binomial-coffeficient-very-reliably
     */
    template <typename T = std::uint64_t, typename U>
    constexpr T GetBinomialCoefficient(U n, U k) noexcept {
        if (k > n)
            return 0;

        if (k == 0 || k == n)
            return 1;

        if (k == 1 || k == n - 1)
            return n;

        return k << 1 > n
            ? (GetBinomialCoefficient(n - 1, k) * n / (n - k))
            : (GetBinomialCoefficient(n - 1, k - 1) * n) / k;
    }

    template <typename T = std::uint64_t, typename U>
    constexpr T GetPower(U base, U power) {
        switch (power) {
            case 0:
                return 1;

            case 1:
                return base;

            default:
                break;
        }

        U sq = GetPower(base, power >> 1);
        return power & 1
            ? (sq * sq * base)
            : (sq * sq);
    }

    template <typename T, std::size_t N>
    struct GetFilledArray {
        constexpr std::array<T, N> operator()(T value) {
            return impl(value, std::make_index_sequence<N>{});
        }

    private:
        template <std::size_t... Is>
        static constexpr std::array<T, N> impl(T value, std::index_sequence<Is...>) {
            return { ((void)Is, value)... };
        }
    };

    template <typename T, std::size_t N>
    struct ArrayProxy {
        constexpr void push_back(T const& value) {
            mData[mSize++] = value;
        }

        constexpr void pop_back() {
            --mSize;
        }

        constexpr auto to_array() const noexcept {
            return mData;
        }

        constexpr std::size_t size() const noexcept {
            return mSize;
        }

    private:
        std::array<T, N> mData{};
        std::size_t mSize{};
    };

    template <typename T, std::size_t N, std::size_t K>
    class Permutation {
    public:
        static constexpr std::size_t size() noexcept {
            return GetBinomialCoefficient(N, K);
        }

        static constexpr auto Generate() {
            ArrayProxy<std::array<T, K>, size()> resultProxy;
            ArrayProxy<T, K> currProxy;

            GenerateImpl(resultProxy, currProxy);

            return resultProxy.to_array();
        }

    private:
        static constexpr void GenerateImpl(
            ArrayProxy<std::array<T, K>, size()>& resultProxy,
            ArrayProxy<T, K>& currProxy, std::size_t begin = 0
        ) {
            if (currProxy.size() == K) {
                resultProxy.push_back(currProxy.to_array());
                return;
            }

            for (auto i = begin; i < N; ++i) {
                currProxy.push_back(i);
                GenerateImpl(resultProxy, currProxy, i + 1);
                currProxy.pop_back();
            }
        }
    };
}


namespace aux {
    using Digit = std::uint8_t;
    static constexpr Digit kAnonDigit = '*';

    template <std::size_t N>
    struct Combination {
        constexpr Combination(std::array<Digit, N>&& digits)
            : mDigits{ std::move(digits) } {}

        constexpr Combination(Digit&& digit = '0')
            : mDigits{ details::GetFilledArray<Digit, N>{}(digit) } {}

        friend constexpr bool operator==(Combination const& lhs, Combination const& rhs) {
            for (auto I = 0; I < N; ++I) {
                if (lhs.mDigits[I] == kAnonDigit)
                    continue;

                if (rhs.mDigits[I] == kAnonDigit)
                    continue;

                if (lhs.mDigits[I] != rhs.mDigits[I])
                    return false;
            }

            return true;
        }

        constexpr Digit& operator[](std::size_t index) {
            return mDigits[index];
        }

        constexpr bool Unanonymous() const {
            for (const auto digit : mDigits)
                if (digit == kAnonDigit)
                    return false;

            return true;
        }

        template <std::size_t K>
        constexpr Combination Anonymize(std::array<std::size_t, K> indices) const {
            Combination combination = *this;

            for (const auto index : indices)
                combination[index] = kAnonDigit;

            return combination;
        }

        constexpr Combination Anonymize(std::size_t index) const {
            return Anonymize({ index });
        }
        
        constexpr Combination& operator++() {
            for (int I = N - 1; I >= 0; --I) {
                if (mDigits[I] == kAnonDigit)
                    continue;

                if (mDigits[I] == '9')
                    mDigits[I] = '0';
                else {
                    ++mDigits[I];
                    break;
                }
            }
            
            return *this;
        }

        constexpr Combination operator++(int) {
            Combination prev = *this;
            operator++();
            return prev;
        }

        friend std::ostream& operator<<(std::ostream& os, Combination const& combination) {
            for (auto digit : combination.mDigits)
                os << digit;

            return os;
        }
    
    private:
        // template<std::size_t... Is>
        // friend constexpr bool equal_to_impl(Combination& lhs, Combination const& rhs, std::index_sequence<Is...>) {
        //     return ((lhs[Is] == kAnonDigit || rhs[Is] == kAnonDigit || lhs[Is] == rhs[Is]) && ...);
        // }

        std::array<Digit, N> mDigits;
    };

    template <std::size_t N, std::size_t K, bool V_, bool P_>
    struct Constraint {
        constexpr Constraint(Combination<N> const& combination)
            : mCombination(combination) {}

        /**
         * @see https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
         */
        template <bool V = V_, bool P = P_>
        constexpr std::enable_if_t<V && P, bool> Match(Combination<N> const& combination) const {
            constexpr auto permutations = details::Permutation<std::size_t, N, K>::Generate();
            for (auto const& indices : permutations)
                if (mCombination.Anonymize(indices) == combination)
                    return true;

            return false;
        }

    private:
        const Combination<N> mCombination;
    };

    template <std::size_t N, typename... Constraints>
    class Solution {
    public:
        constexpr Solution(Constraints... constraints)
            : mConstraints{ constraints... } {}

        static constexpr std::size_t max_size() noexcept {
            return details::GetPower(std::size_t(10), N);
        }

        constexpr auto Generate() const {
            details::ArrayProxy<Combination<N>, max_size()> result;
            Combination<N> combination { '0' };

            auto count = max_size();
            while (count--)
                if (MatchAllConstraints(combination++))
                    result.push_back(combination);

            return result;
        }

    private:
        std::tuple<Constraints...> mConstraints;

        constexpr bool MatchAllConstraints(Combination<N> const& combination) const {
            return std::apply([&](const auto&... constraints) {
                return (constraints.Match(combination) && ...);
            }, mConstraints);
        }
    };
}


extern "C" auto main(int argc, char* argv[]) -> std::int32_t {  
    using namespace aux;

    constexpr Constraint<3, 2, true, true> constraint {
        {{ '1', '4', '7' }}
    };

    constexpr auto solution = Solution<3, decltype(constraint)> { constraint };

    constexpr auto resultProxy = solution.Generate();
    constexpr auto result = resultProxy.to_array();
    constexpr auto size = resultProxy.size();

    for (auto i = 0; i < size; ++i)
        std::cout << result[i] << ' ';

    return 0;
}