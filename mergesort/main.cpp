#include <omp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include <cstdlib> // rand()
#include <cassert>
#include <ctime>

using namespace std;

#define SIZE 10000000
////////////////////////////////////////////////////
// Сортировать массив сортировкой слиянием глупо. //
// Будем сортировать слиянием список.             //
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// Параллелить сортировку слиянием тоже глупо, т.к.
// http://neerc.ifmo.ru/wiki/index.php?title=%D0%9C%D0%BD%D0%BE%D0%B3%D0%BE%D0%BF%D0%BE%D1%82%D0%BE%D1%87%D0%BD%D0%B0%D1%8F_%D1%81%D0%BE%D1%80%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%BA%D0%B0_%D1%81%D0%BB%D0%B8%D1%8F%D0%BD%D0%B8%D0%B5%D0%BC
///////////////////////////////////////////////////

// Проверка отсортированности списка
void sort_control(list<int> &l, int sz) {
    assert(l.size() == sz);
    list<int>::iterator _it = l.begin();
    int x = *_it;
    while (++_it != l.end()) assert (x < *_it);
    return;
};


// Пример функции слияния, но пользоваться будем stl::list::merge функцией,
// так как она должна эффективнее работать с подкапотным списком контейнера stl::list
void merge (list<int>::iterator it1, list<int>::iterator it1_end,
            list<int>::iterator it2, list<int>::iterator it2_end,
            list<int> &result) {
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



/////////////////////////////////////
// Рекурсивная сортировка слиянием //
/////////////////////////////////////
void merge_sort_recursive(list<int> &l) {
    if (l.size() > 1) {

        list<int> l1, l2;
        list<int>::iterator half_it = l.begin();
        for (size_t cnt = l.size() / 2; cnt > 0; cnt--) half_it++;
        l1.splice(l1.begin(), l, l.begin(), half_it);
        l2.splice(l2.begin(), l, half_it, l.end());

        #pragma omp task shared(l1)
        merge_sort_recursive(l1);

        #pragma omp task shared(l2)
        merge_sort_recursive(l2);

        #pragma omp taskwait
        l.merge(l1);
        l.merge(l2);
    }

    return;
}


/////////////////////////////////////
// Итеративная сортировка слиянием //
/////////////////////////////////////
typedef list<int> * list_ptr;

void split_list(list<int> &l, vector<list_ptr> &tiles) {
    list<int>::iterator it1, it2;
    list_ptr pList;

    while (l.size() > 1) {
        it1 = it2 = l.begin();
        it2++;
        bool order = (*(it2++) - *(it1++) >= 0);
        while(it2 != l.end()) {
            if((*(it2++) - *(it1++) >= 0) != order) break;
        };
        pList = new list<int>;
        pList->splice(pList->begin(), l, l.begin(), it1);
        if (order) tiles.push_back(pList);
        else {
            pList->reverse();
            tiles.push_back(pList);
        }
    }
    if (l.size() > 0) {
        pList = new list<int>;
        pList->splice(pList->begin(), l, l.begin(), l.end());
        tiles.push_back(pList);
    }

    return;
}

void merge_sort_iterative(list<int> &l) {
    vector<list_ptr> tiles;

    split_list(l, tiles);

    omp_set_num_threads(4);

    for(size_t step = 1; step < tiles.size(); step *= 2) {
        #pragma omp parallel for
        for (int i = 0; i < (tiles.size() - step + 1) / 2; i += step) {
            tiles[i * 2]->merge(*(tiles[i * 2 + step]));
            delete tiles[i * 2 + step];
        }
    }

    l.merge(*(tiles[0]));

    delete tiles[0];

    return;
}


//////////////////////////////////////////////////////////
// Попытка распареллелить сортировку слиянием привела к //
// замедлению сортировки, что и было предсказано        //
//////////////////////////////////////////////////////////
int main() {

    list<int> l;
    int x;

    string path_in = "/home/ilvin/_CPP_/stepic-149/mergesort/dataset.txt";
    string path_out = "/home/ilvin/_CPP_/stepic-149/mergesort/dataset_result.txt";
    ifstream ifs;
    ifs.open(path_in.c_str());
    while(!ifs.eof()) {
        ifs >> x;
        if (!ifs.eof()) l.push_back(x);
    }
    ifs.close();

    std::cout << "SZ: " << l.size() << "; CLOCKS: " << 0 << std::endl;
    clock_t t = clock();
    merge_sort_iterative(l);
    t = clock() - t;
    std::cout << "SZ: " << l.size() << "; CLOCKS: " << t << std::endl;

//    merge_sort_recursive(l);
    t = clock() - t;
    std::cout << "SZ: " << l.size() << "; CLOCKS: " << t << std::endl;

    sort_control(l, l.size());

    ofstream ofs;
    ofs.open(path_out.c_str());
    for (list<int>::iterator it = l.begin(); it != l.end(); it++)
        ofs << *it << " ";
    ofs.close();

    return 0;
}
