#ifndef NGINXPARSER_HPP
#define NGINXPARSER_HPP

#include "Parser.hpp"

class NginxParser : public Parser {
    private:

    public:
        NginxParser(const std::string& fileName) : Parser(fileName) {
        }

        void skipComment (const std::string& str, std::size_t &commentPos) {
            while (str[commentPos]) {
                if (str[commentPos] != '\n') {
                    ++commentPos;
                } else {
                    return ;
                }
            }
        }

        // 매개변수 delimiter의 요소라면, 이를 기준으로 split 후에 첫 단어만 가지고 옵니다.
        // [          listen     5000;]이고, delimit이 " "라면, listen만 가지고 옵니다.
        std::string	getIdentifier(const std::string str, std::size_t& endPos, std::string delimiter)
        {
            size_t wordSize = 0;

            while ((str[endPos] != '\0') && isCharInString(delimiter, str[endPos])) {
                ++endPos;
            }
            size_t begPos = endPos;
            while ((str[endPos] != '\0') && !isCharInString(delimiter, str[endPos])) {
                ++wordSize;
                ++endPos;
            }
            return (str.substr(begPos, wordSize));
        }

        // 매개변수 pos 이후로 브라켓 세트를 만듭니다.
        // 열린 브라켓인지 닫힌 브라켓인지, 각 브라켓의 index는 무엇인지를 함께 담습니다.
        void findBlockSet(const std::string& buf, std::stack<int>st, std::vector<std::pair<std::string, std::size_t> >& vec, std::size_t& pos) {
            while (!(buf[pos] == '\0' || buf[pos] == '{' || buf[pos] == '}')) {
                ++pos;
            }
            // std::cout << "buf[pos]: " << buf[pos] << std::endl;
            if (buf[pos] == '{') {
                st.push(1);
                vec.push_back(std::make_pair("{", pos));
            } else if (buf[pos] == '}') {
                if (st.top() == 1) {
                    st.pop();
                }
                vec.push_back(std::make_pair("}", pos));
            }
            if (st.empty()) {
                return ;
            }
            pos += 1;
            findBlockSet(buf, st, vec, pos);
        }

        // 매개변수 pos 이후로 가장 큰 블록을 읽어옵니다.
        // 내부에 블록이 중첩되어 있더라도 이들을 포함한 큰 블록을 읽어옵니다.
        std::string getBlockContent(const std::string& buf, std::size_t& pos) {
            std::vector<std::pair<std::string, std::size_t> > blockSet;
            std::stack<int> st;

            // std::cout << "buf[getBlockContent]: " << buf << std::endl;
            findBlockSet(buf, st, blockSet, pos);
            if (blockSet.empty()) {
                throw std::string("Error: blockSet is empty!");
            }
            std::size_t blockBeg = blockSet.begin()->second;
            std::size_t blockEnd = (blockSet.end() - 1)->second;
            pos = blockEnd + 1;

            return buf.substr(blockBeg + 1, blockEnd - blockBeg - 1);
        }
};
#endif