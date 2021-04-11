#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int ALPHABET_SIZE = 26 + 1;

class RegularSearch {
public:
    std::string str;
    int n;
    std::vector<int> powers;
    std::vector<std::vector<int>> suffix_array;
    std::vector<std::vector<int>> r_suffix_array;
    int k;

    explicit RegularSearch(const std::string& input_str) : str(input_str + static_cast<char>('z' + 1)),
                                                           n(str.size()),
                                                           powers(powers_of_2(n)),
                                                           suffix_array(powers.size()),
                                                           k(suffix_array.size()) {
        MakeSuffixArray();
    }

    void MakeSuffixArray() {
        suffix_array[0] = count_sort();
        for (int i = 1; i < k; ++i) {
            std::vector<std::pair<int, int>> pairs(n);
            for (int j = 0; j < n; ++j) {
                pairs[j] = {suffix_array[i - 1][j], suffix_array[i - 1][(j + powers[i - 1]) % n]};
            }
            suffix_array[i] = count_pair_sort(pairs);
        }
        for (std::vector<int>& array : suffix_array) {
            std::vector<int> cnt(n);
            for (int c : array) {
                ++cnt[c];
            }
            for (int i = 1; i < n; ++i) {
                cnt[i] += cnt[i - 1];
            }
            std::vector<int> p(n);
            for (int i = n - 1; i >= 0; --i) {
                p[--cnt[array[i]]] = i;
            }
            array = p;
        }
    }

    std::vector<int> Search(const std::string& pattern) {
        int power = *std::lower_bound(powers.begin(), powers.end(), pattern.size());

        std::cout << power << "\n";
        int left = left_bound(pattern, power);
        int right = right_bound(pattern, power);
        std::vector<int> ans;
        for (int i = left; i < right; ++i) {
            ans.push_back(suffix_array[power][i]);
        }
        return ans;
    }

    // pattern > left, ищем (left, right]
    int left_bound(const std::string& pattern, int power) {
        int left = 0;
        int right = n;
        while (left + 1 < right) {
            int middle = left + (right - left) / 2;
            if (is_lower(pattern, middle, power)) {
                left = middle;
            } else {
                right = middle;
            }
        }
        return right;
    }

    // pattern < left, ищем [left, right)
    int right_bound(const std::string& pattern, int power) {
        int left = 0;
        int right = n;
        while (left + 1 < right) {
            int middle = left + (right - left) / 2;
            if (is_bigger(pattern, middle, power)) {
                right = middle;
            } else {
                left = middle;
            }
        }
        return left;
    }

    bool is_lower(const std::string& pattern, int index, int power) {
        if (pattern < str.substr(suffix_array[power][index], power)) {
            return true;
        }
        return false;
    }

    bool is_bigger(const std::string& pattern, int index, int power) {
        if (pattern > str.substr(suffix_array[power][index], power)) {
            return true;
        }
        return false;
    }

    bool is_equal(const std::string& pattern, int index, int power) {
        if (pattern == str.substr(suffix_array[power][index], power)) {
            return true;
        }
        return false;
    }

    std::vector<int> count_sort() {
        std::vector<int> cnt(ALPHABET_SIZE);
        // подсчет каждого символа
        for (char c : str) {
            ++cnt[c - 'a'];
        }
        // cnt[i] - место, на котором должен стоять символ 'a' + i
        for (int i = 1; i < ALPHABET_SIZE; ++i) {
            cnt[i] += cnt[i - 1];
        }
        std::vector<int> p(n);
        // p[k] = i означает, что на k месте в отсортированном списке стоит s[i]
        for (int i = 0; i < n; ++i) {
            p[--cnt[str[i] - 'a']] = i;
        }
        // раскидываем буквы по классам
        std::vector<int> classes(n);
        int curr_class = 0;
        classes[0] = curr_class;
        for (int i = 1; i < n; ++i) {
            if (str[p[i]] != str[p[i - 1]]) {
                ++curr_class;
            }
            classes[p[i]] = curr_class;
        }

        return classes;
    }

    std::vector<int> count_pair_sort(const std::vector<std::pair<int, int>>& pairs) {
        std::vector<int> cnt(n);
        // сортируем по pair.second
        for (int i = 0; i < n; ++i) {
            ++cnt[pairs[i].second];
        }
        for (int i = 1; i < n; ++i) {
            cnt[i] += cnt[i - 1];
        }

        std::vector<int> p_second(n);
        // p_second[k] = i означает, что на k месте в отсортированном списке по pair.second стоит pairs[i]
        for (int i = n - 1; i >= 0; --i) {
            p_second[--cnt[pairs[i].second]] = i;
        }
        std::vector<std::pair<int, int>> second_cnt_sort(n);
        for (int i = 0; i < n; ++i) {
            second_cnt_sort[i] = pairs[p_second[i]];
        }

        cnt.assign(n, 0);
        // сортируем по pair.first
        for (int i = 0; i < n; ++i) {
            ++cnt[second_cnt_sort[i].first];
        }
        for (int i = 1; i < n; ++i) {
            cnt[i] += cnt[i - 1];
        }

        std::vector<int> p_first(n);
        // p_first[k] = i означает, что на k месте в отсортированном списке стоит second_cnt_sort[i]
        for (int i = n - 1; i >= 0; --i) {
            p_first[--cnt[second_cnt_sort[i].first]] = i;
        }
        // композиция двух перемещений
        std::vector<int> p(n);
        for (int i = 0; i < n; ++i) {
            p[i] = p_second[p_first[i]];
        }
        std::vector<int> classes(n);
        int curr_class = 0;
        classes[p[0]] = 0;
        for (int i = 1; i < n; ++i) {
            if (pairs[p[i]] != pairs[p[i - 1]]) {
                ++curr_class;
            }
            classes[p[i]] = curr_class;
        }
        return classes;
    }

    static std::vector<int> powers_of_2(int max_number) {
        std::vector<int> powers = {1};
        while (powers.back() * 2 < max_number) {
            powers.push_back(powers.back() * 2);
        }
        return powers;
    }
};


int main() {
    std::string s = "aaba";
    RegularSearch search(s);
    std::string pattern = "ab";
    std::vector<int> ans = search.Search(pattern);
    for (int c : ans) {
        std::cout << c << " " << s.substr(c, pattern.size());
    }
    std::cout << "\n";
    return 0;
}