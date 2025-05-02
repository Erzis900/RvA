#include <array>
#include <stack>
#include <vector>
#include <cassert>

template<typename T, size_t Capacity>
class FixedItemPool {
public:
    FixedItemPool() {
        for (int i = static_cast<int>(Capacity) - 1; i >= 0; --i) {
            freeIndices.push(i);
            used[i] = false;
            indexInActive[i] = -1;
        }
    }

    int createItem(T item) {
        assert(!freeIndices.empty());

        int index = freeIndices.top();
        freeIndices.pop();

        items[index] = std::move(item);
        used[index] = true;

        indexInActive[index] = static_cast<int>(activeIndices.size());
        activeIndices.push_back(index);

        return index;
    }

    void releaseItem(int index) {
        if (index < 0 || index >= static_cast<int>(Capacity) || !used[index]) return;

        used[index] = false;
        freeIndices.push(index);

        int pos = indexInActive[index];
        int last = activeIndices.back();
        activeIndices[pos] = last;
        indexInActive[last] = pos;

        activeIndices.pop_back();
        indexInActive[index] = -1;
    }

    T* getItem(int index) {
        assert(index >= 0 && index < static_cast<int>(Capacity) && used[index]);
        return &items[index];
    }

    template<typename Func>
    void forEachActive(Func&& fn, int startFrom = 0) {
        for (auto i = startFrom; i < activeIndices.size(); ++i) {
            auto index = activeIndices[i];
            fn(index, items[activeIndices[i]]);
        }
    }

private:
    std::array<T, Capacity> items;
    std::array<bool, Capacity> used;
    std::array<int, Capacity> indexInActive;

    std::stack<int> freeIndices;
    std::vector<int> activeIndices;
};
