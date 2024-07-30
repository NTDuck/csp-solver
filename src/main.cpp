#include <cstdint>

#include <algorithm>
#include <iostream>

#include <string_view>

#include <array>
#include <bitset>


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
    template <typename T, std::size_t N>
    class Combination {
        std::array<T, N> mDigits;

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
            for (auto it = cbegin(), otherIt = other.cbegin(); it != cend() && otherIt != other.cend(); ++it, ++otherIt)
                if (*it != *otherIt)
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
    };

    enum class MatchCondition : std::uint8_t {
        kCorrectValCorrectPos,
        kCorrectValWrongPos,
        kWrongValWrongPos,
    };

    template <std::size_t I, MatchCondition Cond, typename T, std::size_t N>
    class Constraint {
        Combination<T, N> mCombination;

    public:
        constexpr Constraint(Combination<T, N> const& combination)
            : mCombination(combination) {
            static_assert(I <= N);
        }

        template <MatchCondition Cond_ = Cond, std::size_t I_ = I>
        constexpr std::enable_if_t<Cond_ == MatchCondition::kCorrectValCorrectPos, bool> Match(Combination<T, N> const& combination) const noexcept {
            std::size_t count = 0;

            for (auto it = mCombination.cbegin(), otherIt = combination.cbegin(); it != mCombination.cend() && otherIt != combination.cend(); ++it, ++otherIt)
                if (*it == *otherIt)
                    ++count;

            return count == I_;
        }
    
        template <MatchCondition Cond_ = Cond>
        constexpr std::enable_if_t<Cond_ == MatchCondition::kCorrectValWrongPos, bool> Match(Combination<T, N> const& combination) const {
            auto make_table = [](Combination<T, N> const& combination) {
                std::array<std::array<bool, N>, '9' - '0' + 1> table{};

                for (auto& arr : table)
                    for (auto& elem : arr)
                        elem = false;
                
                for (auto pos = 0; pos < combination.size(); ++pos)
                    table[combination[pos] - '0'][pos] = true;

                return table;
            };

            auto table = make_table(mCombination);
            auto otherTable = make_table(combination);

            std::size_t count = 0;
            std::size_t internalCount{};

            for (auto idx = 0; idx < table.size(); ++idx) {
                internalCount = 0;

                for (auto pos = 0; pos < N; ++pos)
                    if (table[idx][pos] != otherTable[idx][pos])
                        ++internalCount;

                count += internalCount >> 1;
            }

            return count == I;
        }

        template <MatchCondition Cond_ = Cond>
        constexpr std::enable_if_t<Cond_ == MatchCondition::kWrongValWrongPos, bool> Match(Combination<T, N> const& combination) const noexcept {
            return Match<MatchCondition::kCorrectValCorrectPos, N - I>(combination);
        }
    };

    template <typename T, std::size_t N>
    class Solution {
    public:
        template <typename... Constraints>
        static constexpr auto Generate(Constraints const&... constraints) {
            details::static_vector<Combination<T, N>, details::pow(std::size_t(10), N)> result{};

            for (Combination<T, N> combination { '0' }; combination != Combination<T, N> { '9' }; ++combination)
                if ((constraints.Match(combination) && ...))
                    result.push_back(combination);

            return result;
        }

    private:
        template <typename... Constraints>
        static constexpr bool Match(Combination<T, N> const& combination, Constraints const&... constraints) {
            return (constraints.Match(combination) && ...);
        }
    };
}


int main(void) {
    constexpr auto solution = ext::Solution<char, 3>::Generate(
        ext::Constraint<1, ext::MatchCondition::kCorrectValWrongPos,   char, 3> {{ "147" }},
        ext::Constraint<1, ext::MatchCondition::kCorrectValCorrectPos, char, 3> {{ "189" }},
        ext::Constraint<2, ext::MatchCondition::kCorrectValWrongPos,   char, 3> {{ "964" }},
        ext::Constraint<3, ext::MatchCondition::kWrongValWrongPos,     char, 3> {{ "523" }},
        ext::Constraint<1, ext::MatchCondition::kCorrectValWrongPos,   char, 3> {{ "286" }}
    );

    for (auto combination : solution)
        std::cout << combination << ' ';
}