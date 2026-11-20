#include <iostream>
#include <string>
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

struct Counter {
    long long cmp;
    long long mov;
    long long passes;
    long long shifts;
};

typedef void (*SortFn)(int[], int, Counter&);

struct SortCase {
    const char* name;
    int n;
    int in[12];
    int out[12];
};

struct PosCase {
    const char* name;
    int n;
    int arr[6];
    int key;
    int expected;
};

void resetCounter(Counter& c) {
    c.cmp = 0;
    c.mov = 0;
    c.passes = 0;
    c.shifts = 0;
}

void copyArray(const int src[], int dst[], int n) {
    for (int i = 0; i < n; ++i) dst[i] = src[i];
}

bool sameArray(const int a[], const int b[], int n) {
    for (int i = 0; i < n; ++i)
        if (a[i] != b[i]) return false;
    return true;
}

bool isSorted(const int a[], int n) {
    for (int i = 1; i < n; ++i)
        if (a[i - 1] > a[i]) return false;
    return true;
}

bool sameContent(const int before[], int nb, const int after[], int na) {
    if (nb != na) return false;
    long long s1 = 0, s2 = 0, q1 = 0, q2 = 0;
    for (int i = 0; i < nb; ++i) {
        s1 += before[i];
        q1 += (long long)before[i] * before[i];
    }
    for (int i = 0; i < na; ++i) {
        s2 += after[i];
        q2 += (long long)after[i] * after[i];
    }
    return s1 == s2 && q1 == q2;
}

bool checkSort(const int before[], int nb, const int after[], int na) {
    return isSorted(after, na) && sameContent(before, nb, after, na);
}

void swapElems(int a[], int i, int j, Counter& c) {
    int tmp = a[i];
    ++c.mov;
    a[i] = a[j];
    ++c.mov;
    a[j] = tmp;
    ++c.mov;
}

int findInsertPos(const int a[], int len, int key, Counter& c) {
    int lo = 0, hi = len;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        ++c.cmp;
        if (key < a[mid]) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}

void binaryInsertionSort(int a[], int n, Counter& c) {
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        ++c.mov;
        int pos = findInsertPos(a, i, key, c);
        for (int j = i; j > pos; --j) {
            a[j] = a[j - 1];
            ++c.mov;
            ++c.shifts;
        }
        a[pos] = key;
        ++c.mov;
    }
}

void insertionSort(int a[], int n, Counter& c) {
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        ++c.mov;
        int j = i - 1;
        while (j >= 0) {
            ++c.cmp;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            ++c.mov;
            ++c.shifts;
            --j;
        }
        a[j + 1] = key;
        ++c.mov;
    }
}

void shakerSort(int a[], int n, Counter& c) {
    int left = 0, right = n - 1;
    while (left < right) {
        bool swapped = false;
        ++c.passes;
        for (int i = left; i < right; ++i) {
            ++c.cmp;
            if (a[i] > a[i + 1]) {
                swapElems(a, i, i + 1, c);
                swapped = true;
            }
        }
        --right;
        if (!swapped) break;
        ++c.passes;
        swapped = false;
        for (int i = right; i > left; --i) {
            ++c.cmp;
            if (a[i - 1] > a[i]) {
                swapElems(a, i - 1, i, c);
                swapped = true;
            }
        }
        ++left;
        if (!swapped) break;
    }
}

void bubbleSort(int a[], int n, Counter& c) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        ++c.passes;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++c.cmp;
            if (a[j] > a[j + 1]) {
                swapElems(a, j, j + 1, c);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

int utf8Length(const string& s) {
    int len = 0;
    for (unsigned char ch : s)
        if ((ch & 0xC0) != 0x80) ++len;
    return len;
}

string padRight(const string& s, int width) {
    string r = s;
    int l = utf8Length(s);
    while (l < width) {
        r += ' ';
        ++l;
    }
    return r;
}

string padLeft(const string& s, int width) {
    string r = s;
    int l = utf8Length(s);
    while (l < width) {
        r = " " + r;
        ++l;
    }
    return r;
}

void printArray(const char* label, const int a[], int n) {
    cout << label;
    for (int i = 0; i < n; ++i) {
        if (i) cout << ", ";
        cout << a[i];
    }
    cout << "\n";
}

const SortCase sortCases[] = {
    {"порожній масив", 0, {}, {}},
    {"один елемент", 1, {5}, {5}},
    {"уже впорядкований", 5, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}},
    {"зворотний порядок", 5, {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}},
    {"змішаний порядок", 5, {4, 2, 5, 1, 3}, {1, 2, 3, 4, 5}},
    {"повторювані значення", 5, {3, 1, 3, 2, 1}, {1, 1, 2, 3, 3}},
    {"від'ємні числа і нуль", 5, {0, -4, 7, -1, 3}, {-4, -1, 0, 3, 7}},
    {"усі значення однакові", 4, {8, 8, 8, 8}, {8, 8, 8, 8}},
    {"мінімальна перестановка", 2, {2, 1}, {1, 2}},
    {"майже впорядкований набір", 6, {1, 2, 3, 5, 4, 6}, {1, 2, 3, 4, 5, 6}},
    {"власний тест: від'ємні дублікати", 6, {-5, -2, -5, 3, -2, -5}, {-5, -5, -5, -2, -2, 3}},
    {"власний тест: великі значення", 5, {1000000, 999999, -1000000, 500000, 0}, {-1000000, 0, 500000, 999999, 1000000}}
};

const PosCase posCases[] = {
    {"ключ менший за всі елементи", 4, {10, 20, 30, 40}, 5, 0},
    {"ключ у середині ділянки", 4, {10, 20, 30, 40}, 25, 2},
    {"ключ більший за всі елементи", 4, {10, 20, 30, 40}, 45, 4},
    {"ключ рівний наявному", 4, {10, 20, 30, 40}, 20, 2},
    {"серед однакових значень", 5, {1, 2, 2, 2, 5}, 2, 4},
    {"порожня ділянка", 0, {}, 7, 0}
};

int runSortTests(const char* title, SortFn fn) {
    const int total = (int)(sizeof(sortCases) / sizeof(sortCases[0]));
    int ok = 0;
    for (int t = 0; t < total; ++t) {
        int work[12];
        int n = sortCases[t].n;
        copyArray(sortCases[t].in, work, n);
        Counter c;
        resetCounter(c);
        fn(work, n, c);
        bool good = checkSort(sortCases[t].in, n, work, n) && sameArray(work, sortCases[t].out, n);
        if (good) ++ok;
        cout << padRight(title, 18) << padRight("тест " + to_string(t + 1), 10)
             << padRight(sortCases[t].name, 40) << (good ? "успішно" : "помилка") << "\n";
    }
    return ok;
}

int runPositionTests() {
    const int total = (int)(sizeof(posCases) / sizeof(posCases[0]));
    int ok = 0;
    for (int t = 0; t < total; ++t) {
        Counter c;
        resetCounter(c);
        int pos = findInsertPos(posCases[t].arr, posCases[t].n, posCases[t].key, c);
        bool good = pos == posCases[t].expected;
        if (good) ++ok;
        cout << padRight("Позиція", 18) << padRight("тест " + to_string(t + 1), 10)
             << padRight(posCases[t].name, 40) << (good ? "успішно" : "помилка") << "\n";
    }
    return ok;
}

void runOne(const char* title, SortFn fn, const int src[], int n, int extra) {
    int work[1000];
    copyArray(src, work, n);
    Counter c;
    resetCounter(c);
    fn(work, n, c);
    bool ok = checkSort(src, n, work, n);
    cout << "=== " << title << " ===\n";
    printArray("Результат: ", work, n);
    cout << "Порівняння: " << c.cmp << "\n";
    cout << "Переміщення: " << c.mov << "\n";
    if (extra == 1) cout << "Зміщення: " << c.shifts << "\n";
    if (extra == 2) cout << "Проходи: " << c.passes << "\n";
    cout << "Перевірка: " << (ok ? "успішно" : "помилка") << "\n";
}

void makeData(int type, int n, int a[]) {
    static const int pattern[9] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    for (int i = 0; i < n; ++i) {
        if (type == 0) a[i] = i + 1;
        else if (type == 1) a[i] = n - i;
        else if (type == 2) a[i] = i + 1;
        else a[i] = pattern[i % 9];
    }
    if (type == 2) {
        for (int k = 2; k + 1 < n; k += 4) {
            int tmp = a[k];
            a[k] = a[k + 1];
            a[k + 1] = tmp;
        }
    }
}

struct ExpResult {
    Counter bin;
    Counter ins;
    Counter shk;
    Counter bub;
    bool okBin;
    bool okIns;
    bool okShk;
    bool okBub;
};

void runExperiment() {
    const int sizes[3] = {10, 100, 1000};
    const char* typeNames[4] = {"Зростання", "Спадання", "Майже впорядкований", "Змішаний"};
    static int base[1000], w1[1000], w2[1000], w3[1000], w4[1000];
    static ExpResult res[3][4];
    int passedChecks = 0;

    for (int si = 0; si < 3; ++si) {
        int n = sizes[si];
        for (int ti = 0; ti < 4; ++ti) {
            ExpResult& r = res[si][ti];
            makeData(ti, n, base);

            copyArray(base, w1, n);
            resetCounter(r.bin);
            binaryInsertionSort(w1, n, r.bin);
            r.okBin = checkSort(base, n, w1, n);

            copyArray(base, w2, n);
            resetCounter(r.ins);
            insertionSort(w2, n, r.ins);
            r.okIns = checkSort(base, n, w2, n);

            copyArray(base, w3, n);
            resetCounter(r.shk);
            shakerSort(w3, n, r.shk);
            r.okShk = checkSort(base, n, w3, n);

            copyArray(base, w4, n);
            resetCounter(r.bub);
            bubbleSort(w4, n, r.bub);
            r.okBub = checkSort(base, n, w4, n);

            passedChecks += r.okBin + r.okIns + r.okShk + r.okBub;
        }
    }

    cout << "=== Експеримент: бінарні вставки та шейкерне сортування ===\n";
    cout << padRight("Набір", 22) << padLeft("n", 6) << padLeft("Бін.пор", 9) << padLeft("Бін.пер", 9)
         << padLeft("Шейк.пор", 10) << padLeft("Шейк.пер", 10) << padLeft("Проходи", 9) << "  Перевірка\n";
    for (int si = 0; si < 3; ++si)
        for (int ti = 0; ti < 4; ++ti) {
            ExpResult& r = res[si][ti];
            bool ok = r.okBin && r.okShk;
            cout << padRight(typeNames[ti], 22) << padLeft(to_string(sizes[si]), 6)
                 << padLeft(to_string(r.bin.cmp), 9) << padLeft(to_string(r.bin.mov), 9)
                 << padLeft(to_string(r.shk.cmp), 10) << padLeft(to_string(r.shk.mov), 10)
                 << padLeft(to_string(r.shk.passes), 9) << "  " << (ok ? "успішно" : "помилка") << "\n";
        }

    cout << "=== Пара 1: звичайні вставки та бінарні вставки ===\n";
    cout << padRight("Набір", 22) << padLeft("n", 6) << padLeft("Звич.пор", 10) << padLeft("Бін.пор", 9)
         << padLeft("Звич.зсув", 11) << padLeft("Бін.зсув", 10) << padLeft("Звич.пер", 10) << padLeft("Бін.пер", 9) << "\n";
    for (int si = 0; si < 3; ++si)
        for (int ti = 0; ti < 4; ++ti) {
            ExpResult& r = res[si][ti];
            cout << padRight(typeNames[ti], 22) << padLeft(to_string(sizes[si]), 6)
                 << padLeft(to_string(r.ins.cmp), 10) << padLeft(to_string(r.bin.cmp), 9)
                 << padLeft(to_string(r.ins.shifts), 11) << padLeft(to_string(r.bin.shifts), 10)
                 << padLeft(to_string(r.ins.mov), 10) << padLeft(to_string(r.bin.mov), 9) << "\n";
        }

    cout << "=== Пара 2: бульбашкове сортування та шейкерне сортування ===\n";
    cout << padRight("Набір", 22) << padLeft("n", 6) << padLeft("Бульб.пор", 10) << padLeft("Шейк.пор", 10)
         << padLeft("Бульб.обм", 10) << padLeft("Шейк.обм", 10) << padLeft("Бульб.пр", 10) << padLeft("Шейк.пр", 10) << "\n";
    for (int si = 0; si < 3; ++si)
        for (int ti = 0; ti < 4; ++ti) {
            ExpResult& r = res[si][ti];
            cout << padRight(typeNames[ti], 22) << padLeft(to_string(sizes[si]), 6)
                 << padLeft(to_string(r.bub.cmp), 10) << padLeft(to_string(r.shk.cmp), 10)
                 << padLeft(to_string(r.bub.mov / 3), 10) << padLeft(to_string(r.shk.mov / 3), 10)
                 << padLeft(to_string(r.bub.passes), 10) << padLeft(to_string(r.shk.passes), 10) << "\n";
        }

    cout << "Успішних перевірок експерименту: " << passedChecks << " з 48\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    cout << "=== Практична робота 3: бінарні вставки та шейкерне сортування ===\n";
    cout << "Студентка: П'ятенко Софія, група 31-ІП, варіант 22\n\n";
    cout << "1 - варіант 22 (за замовчуванням)\n";
    cout << "2 - ввести власний масив\n";
    cout << "Ваш вибір: ";

    int choice = 1;
    cin >> choice;

    static int base[1000];
    int n = 12;
    if (choice == 2) {
        cout << "Кількість елементів (від 1 до 1000): ";
        cin >> n;
        while (n < 1 || n > 1000) {
            cout << "Некоректне значення, повторіть: ";
            cin >> n;
        }
        cout << "Введіть елементи через пробіл: ";
        for (int i = 0; i < n; ++i) cin >> base[i];
    } else {
        const int v22[12] = {45, 22, 33, 17, 39, 25, 20, 43, 15, 31, 27, 18};
        copyArray(v22, base, n);
    }

    cout << "=== Індивідуальний масив ===\n";
    printArray("Початковий масив: ", base, n);
    runOne("Бінарні вставки", binaryInsertionSort, base, n, 1);
    runOne("Шейкерне сортування", shakerSort, base, n, 2);
    runOne("Звичайні вставки (ПР2)", insertionSort, base, n, 1);
    runOne("Бульбашкове сортування (ПР2)", bubbleSort, base, n, 2);

    cout << "=== Автоматизовані тести ===\n";
    int okBinTests = runSortTests("Бінарні вставки", binaryInsertionSort);
    int okShkTests = runSortTests("Шейкерне", shakerSort);
    int totalSort = (int)(sizeof(sortCases) / sizeof(sortCases[0]));
    cout << "Успішних перевірок сортування: " << okBinTests + okShkTests << " з " << 2 * totalSort << "\n";

    cout << "=== Перевірка позиції вставлення ===\n";
    int okPos = runPositionTests();
    cout << "Успішних перевірок позиції: " << okPos << " з " << (int)(sizeof(posCases) / sizeof(posCases[0])) << "\n";

    runExperiment();
    return 0;
}
