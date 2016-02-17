#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

struct TOK {
    int k;
    int p;
};

class ExParser {
    private:
        vector<TOK> toks;
        char * s;
        int pos;

        bool op(char symbol) {
            while(s[pos] == ' ') pos++;
            if (s[pos] == symbol) {
                pos++;
                return true;
            }
            return false;
        }

        bool number(int *value)  {
            bool f = false;
            *value = 0;
            while(s[pos] == ' ') pos++;
            while(s[pos] >= '0' && s[pos] <= '9') {
                *value *= 10;
                *value += s[pos] - '0';
                pos++;
                f = true;
            }
            return f;
        }

        bool parse_x(TOK * t) {
            int value; bool f = false;
            if (op('x')) {
                f = true;
                if (op('^')) {
                    if (number(&value)) {
                       t->p = value;
                    } else {
                       std::cout << "ERROR: need number";
                    }
                } else {
                    t->p = 1;
                }
            } else {
                t->p = 0;
            }
            return f;
        }

        void parse() {
            while(s[pos] != '\0') {
                TOK t;
                int value;

                if (op('-')) {
                    t.k = -1;
                } else if (op('+')) {
                    t.k = 1;
                } else {
                    t.k = 1;
                }

                if (number(&value)) {
                    t.k *= value;
                    if (op('*')) {
                        parse_x(&t);
                    } else {
                        t.p = 0;
                    }
                } else {
                    parse_x(&t);
                }
                toks.push_back(t);
            }
        }
    public:
        ExParser(char * str): s(str), pos(0) {
            parse();
        }
        ~ExParser() {
        }

        vector<TOK> get_toks() {
            return toks;
        }
};


string to_str(TOK &t, string plus) {
    stringstream ss(ios_base::out);
    if (t.p > 1) {
        if (t.k > 1) {
            ss << plus << t.k << "*x^" << t.p;
        } else if (t.k < 0) {
            ss << t.k << "*x^" << t.p;
        } else {
            ss << plus << "x^" << t.p;
        }
    } else if(t.p == 1){
        if (t.k > 1) {
            ss << plus << t.k << "*x";
        } else if (t.k < 1) {
            ss << t.k << "*x";
        } else {
            ss << plus << "x";
        }
    } else {
        if (t.k >= 1) {
            ss << plus << t.k;
        } else if (t.k < 0) {
            ss << t.k;
        }
    }
    return ss.str();
}


int main(int argc, char ** argv) {

/*
    for (int i = 0; i < argc; i++) {
        std::cout << i << ": " << argv[i] << endl;
    }
*/

    char * str = argv[1];

    ExParser p(str);
    vector<TOK> toks = p.get_toks();

    std::sort(toks.begin(), toks.end(), [](TOK &a, TOK &b){ return b.p < a.p; });

/*
    for (vector<TOK>::iterator it = toks.begin(); it != toks.end(); it++) {
        std::cout << "TOK[" << (*it).k << ", " << (*it).p << "]" << std::endl;
    }
    std::cout << std::endl;
*/

    for (vector<TOK>::iterator it = toks.begin(); it != toks.end(); it++) {
        it->k *= it->p;
        if (it->p != 0) it->p--;
    }


    for (vector<TOK>::iterator it = toks.begin(); it != toks.end(); it++) {
        std::cout << "TOK[" << (*it).k << ", " << (*it).p << "]" << std::endl;
    }
    std::cout << std::endl;


    vector<TOK>::iterator it, it_prev;
    it = it_prev = toks.begin();
    while (++it != toks.end()) {
        if (it->p == it_prev->p) it_prev->k += it->k;
        else {
            it_prev++;
            it_prev->k = it->k;
            it_prev->p = it->p;
        }
    }
    toks.resize(++it_prev - toks.begin());

    stringstream ss(ios_base::out);
    string plus = "";
    for (vector<TOK>::iterator it = toks.begin(); it != toks.end(); it++) {
        ss << to_str(*it, plus);
        plus = "+";
    }
    std::cout << ss.str() << std::endl;

    return 0;
}
