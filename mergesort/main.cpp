#include <iostream>
#include <string>
#include <list>
#include <vector>

#include <cstdlib> // rand()
#include <cassert>

using namespace std;


void sort_control(list<double> &l) {
    list<double>::iterator _it = l.begin();
    double x = *_it;
    while (++_it != l.end()) assert (x < *_it);
    return;
};


// Пример функции слияния, но пользоваться будем stl::list::merge функцией,
// так как она эффективнее работает с подкапотным списком контейнера stl::list
void merge (list<double>::iterator it1, list<double>::iterator it1_end,
            list<double>::iterator it2, list<double>::iterator it2_end,
            list<double> &result) {
    do {
        if (it1 == it1_end) {
            if (it2 == it2_end) break;
            else result.push_back(*(it2++));
        } else {
            if (it2 == it2_end) result.push_back(*(it1++));
            else {
                if (*it1 < *it2) result.push_back(*(it1++));
                else result.push_back(*(it2++));
            }
        }
    } while(true);

    return;
}

// Пример однопоточной рекурсивной сортировки слиянием
// Параллелить её бесполезно, т.к. эффективность получается ниже, чем у однопоточной
// Доказательство написано тут: http://neerc.ifmo.ru/wiki/index.php?title=%D0%9C%D0%BD%D0%BE%D0%B3%D0%BE%D0%BF%D0%BE%D1%82%D0%BE%D1%87%D0%BD%D0%B0%D1%8F_%D1%81%D0%BE%D1%80%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%BA%D0%B0_%D1%81%D0%BB%D0%B8%D1%8F%D0%BD%D0%B8%D0%B5%D0%BC
void merge_sort_recursive(list<double> &l) {
    if (l.size() > 1) {

        list<double> l1, l2;
        list<double>::iterator half_it = l.begin();
        for (size_t cnt = l.size() / 2; cnt > 0; cnt--) half_it++;
        l1.splice(l1.begin(), l, l.begin(), half_it);
        l2.splice(l2.begin(), l, half_it, l.end());

        merge_sort_recursive(l1);
        merge_sort_recursive(l2);

        assert (l.size() == 0);

        merge(l1.begin(), l1.end(), l2.begin(), l2.end(), l);

    }

    return;
}

// Мне остается сделать только:
// Разбить список на части, отслеживая естественную сортировку
// И параллельно мержить отсортированные части методом восходящей сортировки слиянием
typedef list<double> * list_ptr;
void split_list(list<double> &l, vector<list_ptr> &asc, vector<list_ptr> &desc) {
    list<double>::iterator it1, it2;
    list_ptr tmp;

    while (l.size() > 1) {
        it1 = it2 = l.begin();
        it2++;
        bool order = (*(it2++) - *(it1++) > 0);
        while(it2 != l.end() && ((*(it2++) - *(it1++) > 0) == order)) { };
        tmp = new(list_ptr);
        tmp->splice(tmp->begin(), l, l.begin(), it1);
        if (order) asc.push_back(tmp);
        else desc.push_back(tmp);
    }
    if (l.size() > 0) asc.push_back(l);
    return;
}

#define CHUNK_SIZE 100000
typedef list<double> * list_ptr;

void merge_sort_for(list<double> &l) {
    if (l.size() > 1) {
        vector<list_ptr> sum;

        int cnt = (l.size() + CHUNK_SIZE) / CHUNK_SIZE;
        int chunk_size = (l.size() + cnt) / cnt;
        for( int i = 0; i < cnt; i++ ) {
            list<double> chunk;
            list<double>::iterator it = l.begin();
            for (size_t cnt = 0; cnt < chunk_size && it != l.end(); cnt++, it++);

            chunk.splice(l.begin(), l, l.begin(), it);

            merge_sort_recursive(l);
            sum.push_back(&l);
        }

        list<double> result;
        for (int i = 0; i < sum.size(); i++) {
            list<double
            merge(result.begin(), result.end(), sum[i]->begin(), sum[i]->end(), result);
        }

    }

    return;
}


#define SIZE 1000000

int main() {

    list<double> l;

    for (int i = 0; i < SIZE; i++) {
        l.push_back(rand());
    }

    merge_sort_for(l);

    sort_control(l);

    list<double>::iterator _it = l.begin();
    for (int i = 0 ; i< 15; i++ ) std::cout << *(_it++) << std::endl;

    return 0;
}
