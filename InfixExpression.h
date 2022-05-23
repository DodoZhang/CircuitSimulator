#ifndef INFIXEXPRESSION_H
#define INFIXEXPRESSION_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QList>
#include <QStack>
#include <QMap>
#define IEstring QString
#define IElist QList
#define IEstack QStack
#define IEmap QMap
#define IEGetChar(iter) ((iter)->toLatin1())
#define IEToString(num) QString::number(num)
#define IEMapFind(map, key) ((map).value(key))
#else
#include <string>
#include <list>
#include <stack>
#include <map>
#define IEstring std::string
#define IElist std::list
#define IEstack std::stack
#define IEmap std::map
#define IEGetChar(iter) (*(iter))
#define IEToString(num) std::to_string(num)
#define IEMapFind(map, key) ((map).find(key)->second)
#endif


class InfixExpression
{
public:
    InfixExpression(const IEstring &expression, IEstring *error = nullptr);
    ~InfixExpression();

    bool set(const IEstring &expression, IEstring *error = nullptr);
    const IEstring &get() const;
    IEstring getPostfix() const;
    double calc(const IEmap<IEstring, double> &parameters = IEmap<IEstring, double>());

protected:
    class ExpressionElement
    {
    public:
        enum : uint8_t { Number, Variable, Unary, Binary, Ternary, Function, Bracket } type;
        enum : uint8_t { NAG, NOT } unary;
        enum : uint8_t { ADD, SUB, MULT, DIV, MOD, ET, NET, GT, GET, LT, LET, AND, OR } binary;
        enum : uint8_t { QUESTION, COLON } ternary;
        enum : uint8_t { SIN, COS, TAN, ASIN, ACOS, ATAN, MAX, MIN, ABS, POW, LOG, FLOOR, ROUND, CEIL } function;
        enum : uint8_t { LEFT, RIGHT, COMMA } bracket;
        double number;
        IEstring variable;

    public:
        ExpressionElement() { };
        ExpressionElement(double number) { type = Number; this->number = number; }
        ExpressionElement(IEstring variable) { type = Variable; this->variable = variable; }
        ExpressionElement(typeof (unary) unary) { type = Unary; this->unary = unary; }
        ExpressionElement(typeof (binary) binary) { type = Binary; this->binary = binary; }
        ExpressionElement(typeof (ternary) ternary) { type = Ternary; this->ternary = ternary; }
        ExpressionElement(typeof (function) function) { type = Function; this->function = function; }
        ExpressionElement(typeof (bracket) bracket) { type = Bracket; this->bracket = bracket; }

        int priority() const;
    };

protected:
    IEstring expression;
    IElist<ExpressionElement> *postfix;
};

#endif // INFIXEXPRESSION_H
