#include <iostream>
#include <string>
#include <vector>


using namespace std;

struct TOK {
    int sign;
    int k;
    int x;
    int p;
};
        ///////////////////////////////////////////////////////
        // <expression> ::= <first term> [ <addop> <term> ]* //
        // <first term> ::= <first factor> <rest>            //
        // <term> ::= <factor> <rest>                        //
        // <rest> ::= [ <mulop> <factor> ]*                  //
        // <first factor> ::= [ <addop> ] <factor>           //
        // <factor> ::= <base> [ ^ <factor> ]                //
        // <base> ::= <var> | <number> | ( <expression> )    //
        ///////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////
        // <expression> ::= <first term> [ <addop> <term> ]*                 //
        // <first term> ::= <first factor> <rest>                            //
        // <term> ::= <factor> <rest>                                        //
        // <rest> ::= [ <mulop> <factor> ]*                                  //
        // <first factor> ::= [ <addop> ] <factor>                           //
        // <factor> ::= <number> | <var> [^ <number>]* | ( <expression> )    //
        ///////////////////////////////////////////////////////////////////////



class ExParser {
    private:
        vector<TOK> toks;
        char * s;
        int pos;

        bool op(char symbol) {
            if (s[pos] == symbol) {
                std::cout << "s[" << pos << "] == " << symbol << std::endl;
                pos++;
                return true;
            }
            return false;
        }

        bool addop()    { while(op(' ')); return op('-') || op('+'); }
        bool mulop()    { while(op(' ')); return op('*') || op('/'); }
        bool powop()    { while(op(' ')); return op('^'); }
        bool var()      { while(op(' ')); return op('x'); }
        bool rbr_l()    { while(op(' ')); return op('('); }
        bool rbr_r()    { while(op(' ')); return op(')'); }

        bool number()  {
            std::cout << "number" << std::endl;
            while(op(' '));
            bool is_digit, flag = false;
            while (op('0') || op('1') || op('2') || op('3') || op('4') || op('5') || op('6') || op('7') || op('8') || op('9')) {
                flag = true;
            };
            return flag;
        }

        // <factor> ::= <number> | <var> [^ <number>]* | ( <expression> )    //
        bool factor() {
            if (number()) return true;
            if (var()) {
                if(op('^') && number()) return true;
                else return true;
            }

            return rbr_l() && expression() && rbr_r();
        }

        bool first_factor() {
            std::cout << "first_factor" << std::endl;
            addop();
            return factor();
        }

        bool rest() {
            std::cout << "rest" << std::endl;
            while(mulop() && factor());
            return true;
        }

        bool term() {
            std::cout << "term" << std::endl;
            return factor() && rest();
        }

        bool first_term() {
            std::cout << "first_term" << std::endl;
            return first_factor() && rest();
        }

        bool expression() {
            if (!first_term()) return false;
            while(addop() && term());
            return true;
        }

        void parse() {
            expression();
        }
    public:
        ExParser(char * str): s(str), pos(0) {
            parse();
        }
        ~ExParser() {
        }
};

int main(int argc, char ** argv) {

    for (int i = 0; i < argc; i++) {
        std::cout << i << ": " << argv[i] << endl;
    }

    char * str = argv[1];

    ExParser p(str);

    return 0;
}
