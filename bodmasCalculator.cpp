// g++ -o a.out test1.cpp -I/usr/local/include/boost -L/usr/local/lib

#include <iostream>
#include <string>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

// Define a simple AST (Abstract Syntax Tree) node
struct AstNode
{
    char op;
    int value;
    std::vector<AstNode> children;
    AstNode() : op('\0'), value(0) {} // Default constructor
    AstNode(char op, const AstNode &left, const AstNode &right) : op(op), children{left, right} {}
    AstNode(int value) : op('\0'), value(value) {} // Constructor for terminal value
};

// Define a calculator grammar using Boost Spirit
// standad way of repersenting alangiuage
template <typename Iterator>
struct CalculatorGrammar : boost::spirit::qi::grammar<Iterator, AstNode(), boost::spirit::qi::space_type>
{
    CalculatorGrammar() : CalculatorGrammar::base_type(expression)
    {
        using namespace boost::spirit::qi;

        expression = term[boost::spirit::qi::_val = boost::spirit::qi::_1] >> *(char_('+') > term[boost::spirit::qi::_val = boost::phoenix::construct<AstNode>('+', boost::spirit::qi::_val, boost::spirit::qi::_1)] | char_('-') > term[boost::spirit::qi::_val = boost::phoenix::construct<AstNode>('-', boost::spirit::qi::_val, boost::spirit::qi::_1)]);
        term = factor[boost::spirit::qi::_val = boost::spirit::qi::_1] >> *(char_('*') > factor[boost::spirit::qi::_val = boost::phoenix::construct<AstNode>('*', boost::spirit::qi::_val, boost::spirit::qi::_1)] | char_('/') > factor[boost::spirit::qi::_val = boost::phoenix::construct<AstNode>('/', boost::spirit::qi::_val, boost::spirit::qi::_1)]);
        factor = value | ('[' >> expression >> ']') | ('{' >> expression >> '}') | ('(' >> expression >> ')');
        value = int_;
    }

    boost::spirit::qi::rule<Iterator, AstNode(), boost::spirit::qi::space_type> expression, term, factor;
    boost::spirit::qi::rule<Iterator, int(), boost::spirit::qi::space_type> value;
};

int evaluate(const AstNode &node)
{
    if (node.children.empty())
    {
        return node.value;
    }
    else
    {
        int left = evaluate(node.children[0]);
        int right = evaluate(node.children[1]);
        if (node.op == '+')
        {
            return left + right;
        }
        else if (node.op == '-')
        {
            return left - right;
        }
        else if (node.op == '*')
        {
            return left * right;
        }
        else if (node.op == '/')
        {
            return left / right;
        }
        else
        {
            // Handle other operators as needed
            return 0;
        }
    }
}

int main()
{
    std::string expression;
    std::cout << "Enter a mathematical expression: ";
    std::getline(std::cin, expression);

    using Iterator = std::string::const_iterator;
    CalculatorGrammar<Iterator> grammar;
    AstNode ast;

    Iterator iter = expression.begin();
    Iterator end = expression.end();

    bool success = boost::spirit::qi::phrase_parse(iter, end, grammar, boost::spirit::qi::space, ast);

    if (success && iter == end)
    {
        int result = evaluate(ast);
        std::cout << "Result: " << result << std::endl;
    }
    else
    {
        std::cerr << "Parsing failed!" << std::endl;
    }

    return 0;
}
