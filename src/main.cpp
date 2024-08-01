#ifndef CSP_SOLVER_CPP
#define CSP_SOLVER_CPP


/* Configure input parameters and configurations
by modifying the macros below */
/*  */

// Enables strict checking of exact digit counts
#define CSP_REQUIRES_EXACT_DIGIT_COUNT      false

// The number of digits in a Combination
#define CSP_COMBINATION_LENGTH              3

// The type used for each digit in the Combination
#define CSP_COMBINATION_DIGIT_TYPE          char

// Used to "skip" a clause in a Constraint
#define CSP_NULL                            std::size_t(-1)

// Represent a constraint: for a given Combination `Num`,
// a matching Combination to this Constraint
// should have exactly `ValCount` correct digits
// and `PosCount` correctly placed digits
#define CSP_CONSTRAINT(Num, ValCount, PosCount)                                \
    ext::Constraint<ValCount, PosCount, CSP_COMBINATION_LENGTH, CSP_COMBINATION_DIGIT_TYPE> {{ #Num }}

// Represent a Constraint with given Combination `Num`
// where `Count` digits are correct and correctly placed
#define CSP_CONSTRAINT_CORRECT_AND_CORRECTLY_PLACED(Num, Count)                \
    CSP_CONSTRAINT(Num, Count, Count)

// Represent a Constraint with given Combination `Num`
// where `Count` digits are correct but incorrectly placed
#define CSP_CONSTRAINT_CORRECT_BUT_INCORRECTLY_PLACED(Num, Count)              \
    CSP_CONSTRAINT(Num, Count, 0)

// Represent a Constraint with given Combination `Num`
// where `Count` digits are incorrect
#define CSP_CONSTRAINT_INCORRECT(Num, Count)                                   \
    CSP_CONSTRAINT(Num, CSP_COMBINATION_LENGTH - Count, CSP_NULL)

// Represent a Constraint with given Combination `Num`
// where all digits are incorrect
#define CSP_CONSTRAINT_ALL_INCORRECT(Num)                                      \
    CSP_CONSTRAINT_INCORRECT(Num, CSP_COMBINATION_LENGTH)

// A problem is defined by one or more constraints
#define CSP_CONSTRAINTS                                                        \
    CSP_CONSTRAINT_CORRECT_AND_CORRECTLY_PLACED(682, 1)                       ,\
    CSP_CONSTRAINT_CORRECT_BUT_INCORRECTLY_PLACED(614, 1)                     ,\
    CSP_CONSTRAINT_CORRECT_BUT_INCORRECTLY_PLACED(206, 2)                     ,\
    CSP_CONSTRAINT_ALL_INCORRECT(738)                                         ,\
    CSP_CONSTRAINT_CORRECT_BUT_INCORRECTLY_PLACED(780, 1)


#include <cstdint>

#include <algorithm>
#include <chrono>
#include <iostream>

#include <tuple>
#include <string_view>

#include <array>


namespace ext::details {
    template <typename T, std::size_t N>
    class static_vector {
        std::size_t mSize;
        std::array<T, N> mContainer;

    public:
        using value_type = typename std::array<T, N>::value_type;
        using size_type = typename std::array<T, N>::size_type;
        using difference_type = typename std::array<T, N>::difference_type;

        using reference = typename std::array<T, N>::reference;
        using const_reference = typename std::array<T, N>::const_reference;
        using pointer = typename std::array<T, N>::pointer;
        using const_pointer = typename std::array<T, N>::const_pointer;

        using iterator = typename std::array<T, N>::iterator;
        using const_iterator = typename std::array<T, N>::const_iterator;
        using reverse_iterator = typename std::array<T, N>::reverse_iterator;
        using const_reverse_iterator = typename std::array<T, N>::const_reverse_iterator;

        constexpr reference at(size_type pos) {
            if (pos < 0 || pos > mSize - 1)
                throw(std::out_of_range("Index out of range"));

            return operator[](pos);
        }

        constexpr const_reference at(size_type pos) const {
            if (pos < 0 || pos > mSize - 1)
                throw(std::out_of_range("Index out of range"));

            return operator[](pos);
        }

        constexpr reference operator[](size_type pos) {
            return mContainer.operator[](pos);
        }

        constexpr const_reference operator[](size_type pos) const {
            return mContainer.operator[](pos);
        }

        constexpr reference front() {
            return *begin();
        }

        constexpr const_reference front() const {
            return *begin();
        }

        constexpr reference back() {
            return *std::prev(end());
        }

        constexpr const_reference back() const {
            return *std::prev(end());
        }

        constexpr iterator begin() noexcept {
            return mContainer.begin();
        }

        constexpr const_iterator begin() const noexcept {
            return mContainer.begin();
        }

        constexpr const_iterator cbegin() const noexcept {
            return mContainer.cbegin();
        }

        constexpr iterator end() noexcept {
            auto it = begin();
            std::advance(it, mSize);
            return it;
        }

        constexpr const_iterator end() const noexcept {
            auto it = begin();
            std::advance(it, mSize);
            return it;
        }

        constexpr const_iterator cend() const noexcept {
            auto it = cbegin();
            std::advance(it, mSize);
            return it;
        }

        constexpr reverse_iterator rbegin() noexcept {
            auto it = mContainer.rbegin();
            std::advance(it, N - mSize);
            return it;
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            auto it = mContainer.rbegin();
            std::advance(it, N - mSize);
            return it;
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            auto it = mContainer.crbegin();
            std::advance(it, N - mSize);
            return it;
        }

        constexpr reverse_iterator rend() noexcept {
            return mContainer.rend();
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return mContainer.rend();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return mContainer.crend();
        }

        constexpr bool empty() const noexcept {
            return size() != 0;
        }

        constexpr size_type size() const noexcept {
            return mSize;
        }

        constexpr size_type max_size() const noexcept {
            return mContainer.max_size();
        }

        constexpr size_type capacity() const noexcept {
            return mContainer.size();
        }

        constexpr void clear() {
            mSize = 0;
        }

        constexpr void push_back(const_reference value) {
            mContainer[mSize++] = value;
        }

        constexpr void pop_back() noexcept {
            --mSize;
        }

        constexpr void resize(size_type count) {
            mSize = count;
        }
    };

    /**
     * @note Time complexity: `O(log(power))`
     * @note Auxiliary space: `O(log(power))`
     */
    template <typename T = std::uint64_t, typename U>
    constexpr T pow(U base, U power) {
        switch (power) {
            case 0:
                return 1;

            case 1:
                return base;

            default:
                break;
        }

        U recurse = pow(base, power >> 1);
        return power & 1
            ? (recurse * recurse * base)
            : (recurse * recurse);
    }

    template <typename T, std::size_t N>
    struct make_array {
        constexpr std::array<T, N> operator()(T val) const {
            return fill(val, std::make_index_sequence<N>{});
        }

        constexpr std::array<T, N> operator()(std::basic_string_view<T> sv) const {
            std::array<T, N> arr{};
            auto size = std::min(sv.size(), arr.size());

            for (auto pos = 0; pos < size; ++pos)
                arr[pos] = sv[pos];

            return arr;
        }

    private:
        template <std::size_t... Is>
        static constexpr std::array<T, N> fill(T val, std::index_sequence<Is...>) {
            return { ((void)Is, val)... };
        }
    };
}


namespace ext {
    template <std::size_t N, typename T>
    class Combination {
        std::array<T, N> mDigits;

        static constexpr auto toValTable(Combination const& combination) {
            auto table = details::make_array<bool, 10>{}(false);

            for (auto const digit : combination)
                table[digit - '0'] = true;
            
            return table;
        };

        static constexpr auto toPosTable(Combination const& combination) {
            auto table = details::make_array<std::array<bool, N + 1>, 10>{}(details::make_array<bool, N + 1>{}(false));

            for (std::size_t pos{}; pos < N; ++pos)
                table[combination[pos] - '0'][pos] = table[combination[pos] - '0'][N] = true;

            return table;
        }

        template <typename... Tables>
        static constexpr bool isAllTrue(std::tuple<Tables...> const& tables, std::size_t idx) {
            return std::apply([idx](Tables const&... tables_) {
                return ((tables_[idx] && ...));
            }, tables);
        }

        template <typename... Tables>
        static constexpr bool isAllTrue(std::tuple<Tables...> const& tables, std::size_t idx, std::size_t pos) {
            return std::apply([idx, pos](Tables const&... tables_) {
                return ((tables_[idx][pos] && ...));
            }, tables);
        }

    public:
        using value_type = typename std::array<T, N>::value_type;
        using size_type = typename std::array<T, N>::size_type;
        using difference_type = typename std::array<T, N>::difference_type;

        using reference = typename std::array<T, N>::reference;
        using const_reference = typename std::array<T, N>::const_reference;
        using pointer = typename std::array<T, N>::pointer;
        using const_pointer = typename std::array<T, N>::const_pointer;

        using iterator = typename std::array<T, N>::iterator;
        using const_iterator = typename std::array<T, N>::const_iterator;
        using reverse_iterator = typename std::array<T, N>::reverse_iterator;
        using const_reverse_iterator = typename std::array<T, N>::const_reverse_iterator;

        constexpr Combination() = default;

        constexpr Combination(std::array<T, N> digits)
            : mDigits(digits) {}

        constexpr Combination(T digit)
            : mDigits(details::make_array<T, N>{}(digit)) {}

        constexpr Combination(std::basic_string_view<T> digits)
            : mDigits(details::make_array<T, N>{}(digits)) {}

        constexpr reference at(size_type pos) {
            return mDigits.at(pos);
        }

        constexpr const_reference at(size_type pos) const {
            return mDigits.at(pos);
        }

        constexpr reference operator[](size_type pos) {
            return mDigits.operator[](pos);
        }

        constexpr const_reference operator[](size_type pos) const {
            return mDigits.operator[](pos);
        }

        constexpr reference front() {
            return *begin();
        }

        constexpr const_reference front() const {
            return *begin();
        }

        constexpr reference back() {
            return *std::prev(end());
        }

        constexpr const_reference back() const {
            return *std::prev(end());
        }

        constexpr iterator begin() noexcept {
            return mDigits.begin();
        }

        constexpr const_iterator begin() const noexcept {
            return mDigits.begin();
        }

        constexpr const_iterator cbegin() const noexcept {
            return mDigits.cbegin();
        }

        constexpr iterator end() noexcept {
            return mDigits.end();
        }

        constexpr const_iterator end() const noexcept {
            return mDigits.end();
        }

        constexpr const_iterator cend() const noexcept {
            return mDigits.cend();
        }

        constexpr reverse_iterator rbegin() noexcept {
            return mDigits.rbegin();
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return mDigits.rbegin();
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return mDigits.crbegin();
        }

        constexpr reverse_iterator rend() noexcept {
            return mDigits.rend();
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return mDigits.rend();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return mDigits.crend();
        }

        constexpr bool empty() const noexcept {
            return size() != 0;
        }

        constexpr size_type size() const noexcept {
            return mDigits.size();
        }

        constexpr size_type max_size() const noexcept {
            return mDigits.max_size();
        }

        friend std::ostream& operator<<(std::ostream& os, Combination const& c) {
            for (auto const digit : c.mDigits)
                os << digit;

            return os;
        }

        constexpr bool operator==(Combination const& other) {
            for (std::size_t pos{}; pos < N; ++pos)
                if (operator[](pos) != other[pos])
                    return false;
        
            return true;
        }

        constexpr bool operator!=(Combination const& other) {
            return !operator==(other);
        }

        constexpr Combination& operator++() {
            for (auto it = rbegin(); it != rend(); ++it) {
                if (*it == '9')
                    *it = '0';
                else {
                    ++*it;
                    break;
                }
            }

            return *this;
        }

        constexpr Combination operator++(int) {
            Combination old = *this;
            operator++();
            return old;
        }

        static constexpr bool isValid(T digit) noexcept {
            return '0' <= digit && digit <= '9';
        }

        constexpr bool isValid() const noexcept {
            for (auto const digit : mDigits)
                if (!isValid(digit))
                    return false;

            return true;
        }

        /**
         * @note Time complexity: `O(N * sizeof...(Combinations))`
         * @note Auxiliary space: `O(N * sizeof...(Combinations))`
         */
        template <typename... Combinations>
        static constexpr std::size_t getCorrectValCount(Combinations const&... combinations) {
            static_assert(sizeof...(Combinations) > 0);

            auto tables = std::make_tuple(toValTable(combinations)...);
            std::size_t count{};

            for (std::size_t idx{}; idx < 10; ++idx)
                if (isAllTrue(tables, idx))
                    ++count;

            return count;
        }

        /**
         * @note Time complexity: `O(N * sizeof...(Combinations))`
         * @note Auxiliary space: `O(N * sizeof...(Combinations))`
         */
        template <typename... Combinations>
        static constexpr std::size_t getCorrectPosCount(Combinations const&... combinations) {
            static_assert(sizeof...(Combinations) > 0);

            auto tables = std::make_tuple(toPosTable(combinations)...);

            std::size_t count{};

            for (auto idx = 0; idx < 10; ++idx) {
                if (!isAllTrue(tables, idx, N))
                    continue;

                for (auto pos = 0; pos < N; ++pos)
                    if (isAllTrue(tables, idx, pos))
                        ++count;
            }

            return count;
        }
    };

    template <std::size_t I, std::size_t J, std::size_t N, typename T>
    class Constraint {
        Combination<N, T> mCombination;

    public:
        constexpr Constraint(Combination<N, T> const& combination)
            : mCombination(combination) {
            // static_assert(J <= I);
            // static_assert(I <= N);
        }

        constexpr Match(Combination<N, T> const& combination) const {
            return (I == CSP_NULL || I == Combination<N, T>::getCorrectValCount(mCombination, combination))
                && (J == CSP_NULL || J == Combination<N, T>::getCorrectPosCount(mCombination, combination));
        }
    
        friend std::ostream& operator<<(std::ostream& os, Constraint const& constraint) {
            return os << constraint.mCombination << " has " << I << " correct digit(s), " << J << " of which correctly placed" << '.';
        }
    };

    template <std::size_t N, typename T, typename... Constraints>
    class Solution {
        std::tuple<Constraints...> mConstraints;

        template <typename Clock = std::chrono::high_resolution_clock, typename Duration = std::chrono::microseconds>
        auto getDeltaTime() const {
            static auto begin = Clock::now();
            auto end = Clock::now();
            
            auto dt = end - begin;
            begin = end;

            return std::chrono::duration_cast<Duration>(dt).count();
        }

    public:
        constexpr Solution(Constraints&&... constraints)
            : mConstraints(std::forward<Constraints>(constraints)...) {}

        constexpr auto Generate() const {
            details::static_vector<Combination<N, T>, details::pow(std::size_t(10), N)> result{};

            for (Combination<N, T> combination { '0' }; combination != Combination<N, T> { '9' }; ++combination)
                if (std::apply([combination](Constraints const&... constraints) {
                    return ((constraints.Match(combination) && ...));
                }, mConstraints))
                    result.push_back(combination);

            return result;
        }

        void Print(details::static_vector<Combination<N, T>, details::pow(std::size_t(10), N)> combinations, std::ostream& os = std::cout) const {
            getDeltaTime();

            os << "With " << sizeof...(Constraints) << (sizeof...(Constraints) > 1 ? " constraints" : " constraint") << ":" << std::endl;
            std::apply([&os](Constraints... constraints) {
                std::size_t idx{};
                ((os << '(' << ++idx << ") " << constraints << std::endl), ...);
            }, mConstraints);
            
            os << "Found " << combinations.size() << " matching combinations:" << std::endl;
            for (auto combination : combinations)
                os << combination << ' ';
            os << std::endl;

            auto dt = getDeltaTime();
            os << "(generated within compile-time, printed in " << dt << "ms)" << std::endl;
        }
    };

    template <std::size_t N, typename T, typename... Constraints>
    static constexpr auto CreateSolution(Constraints&&... constraints) {
        return Solution<N, T, Constraints...>(std::forward<Constraints>(constraints)...);
    }

}


int main(void) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    #ifdef CSP_CONSTRAINTS
    constexpr auto solution = ext::CreateSolution<CSP_COMBINATION_LENGTH, CSP_COMBINATION_DIGIT_TYPE> (CSP_CONSTRAINTS);

    // Ignore intellisense's "expression not folded to a constant due to excessive constexpr function call complexity"
    // constexpr can only mean good :3
    constexpr auto generation = solution.Generate();
    solution.Print(generation);

    #else
    std::cerr << "Error: macro CSP_CONSTRAINTS is not defined" << std::endl;
    #endif
}


#undef CSP_REQUIRES_EXACT_DIGIT_COUNT
#undef CSP_COMBINATION_LENGTH
#undef CSP_COMBINATION_DIGIT_TYPE
#undef CSP_NULL
#undef CSP_CONSTRAINT
#undef CSP_CONSTRAINTS

#endif   // CSP_SOLVER_CPP