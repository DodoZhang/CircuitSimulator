#include "InfixExpression.h"

#include <cmath>

InfixExpression::InfixExpression(const IEstring &expression, IEstring *error)
{
    postfix = nullptr;
    if (!set(expression, error)) set("0");
}

InfixExpression::~InfixExpression()
{
    delete postfix;
}

bool InfixExpression::set(const IEstring &expression, IEstring *error)
{
    if (error != nullptr) *error = "";

    //Step 1: 将字符串转换为表达式元素列表
    IElist<ExpressionElement> infix;
    IEstring::const_iterator iter = expression.cbegin();
    //prevElementIsOperand表示前一个「表达式元素」是否是「运算数类型」
    //「运算数类型」的元素包括「数字」「变量」以及「右括号」
    //在「运算数类型」的元素后只能是「二元运算符」「三元运算符」「右括号」或「逗号」
    //在「运算符类型」的元素后可以是「一元运算符」「函数」「左括号」「数字」或「变量」
    bool prevElementIsOperand = false;
    while (iter != expression.cend()) {
        char firstChar = IEGetChar(iter);
        if (firstChar == ' ' || firstChar == '\t' || firstChar == '\n') iter ++; //跳过空白字符
        else if ((firstChar >= '0' && firstChar <= '9') || firstChar == '.')
        {
            //「数字类型」的元素，只包含「数字」
            double number = 0;
            int exponent = 0;
            bool readingDecimalPart = false;
            while (iter != expression.cend())
            {
                char ch = IEGetChar(iter);
                if (ch >= '0' && ch <= '9')
                {
                    number *= 10;
                    number += ch - '0';
                    if (readingDecimalPart) exponent ++;
                }
                else if (ch == '.')
                {
                    if (readingDecimalPart)
                    {
                        //错误：数字中出现一个以上的小数点
                        if (error != nullptr) *error = "More than one decimal point appeared in a number.";
                        return false;
                    } else readingDecimalPart = true;
                } else break;
                iter ++;
            }
            for (; exponent > 0; exponent --) number /= 10;
            infix.push_back(ExpressionElement(number));
            prevElementIsOperand = true;
        }
        else if ((firstChar >= 'A' && firstChar <= 'Z') ||
                 (firstChar >= 'a' && firstChar <= 'z') ||
                 firstChar == '_')
        {
            //「字段类型」的元素，包含「变量」及「函数」
            IEstring str;
            while (iter != expression.cend())
            {
                char ch = IEGetChar(iter);
                if ((ch >= '0' && ch <= '9') ||
                    (ch >= 'A' && ch <= 'Z') ||
                    (ch >= 'a' && ch <= 'z') ||
                    ch == '_') str.push_back(ch);
                else break;
                iter ++;
            }
            if (prevElementIsOperand)
            {
                //错误：在操作数后不可为变量或函数
                if (error != nullptr) *error = "Variable or Function \"" + str + "\" could not append after a operand.";
                return false;
            }
            bool isFunction = true;
            if (str.compare("sin") == 0) infix.push_back(ExpressionElement(ExpressionElement::SIN));
            else if (str.compare("cos") == 0) infix.push_back(ExpressionElement(ExpressionElement::COS));
            else if (str.compare("tan") == 0) infix.push_back(ExpressionElement(ExpressionElement::TAN));
            else if (str.compare("asin") == 0) infix.push_back(ExpressionElement(ExpressionElement::ASIN));
            else if (str.compare("acos") == 0) infix.push_back(ExpressionElement(ExpressionElement::ACOS));
            else if (str.compare("atan") == 0) infix.push_back(ExpressionElement(ExpressionElement::ATAN));
            else if (str.compare("arcsin") == 0) infix.push_back(ExpressionElement(ExpressionElement::ASIN));
            else if (str.compare("arccos") == 0) infix.push_back(ExpressionElement(ExpressionElement::ACOS));
            else if (str.compare("arctan") == 0) infix.push_back(ExpressionElement(ExpressionElement::ATAN));
            else if (str.compare("max") == 0) infix.push_back(ExpressionElement(ExpressionElement::MAX));
            else if (str.compare("min") == 0) infix.push_back(ExpressionElement(ExpressionElement::MIN));
            else if (str.compare("abs") == 0) infix.push_back(ExpressionElement(ExpressionElement::ABS));
            else if (str.compare("power") == 0) infix.push_back(ExpressionElement(ExpressionElement::POW));
            else if (str.compare("pow") == 0) infix.push_back(ExpressionElement(ExpressionElement::POW));
            else if (str.compare("log") == 0) infix.push_back(ExpressionElement(ExpressionElement::LOG));
            else if (str.compare("floor") == 0) infix.push_back(ExpressionElement(ExpressionElement::FLOOR));
            else if (str.compare("int") == 0) infix.push_back(ExpressionElement(ExpressionElement::FLOOR));
            else if (str.compare("round") == 0) infix.push_back(ExpressionElement(ExpressionElement::ROUND));
            else if (str.compare("ceil") == 0) infix.push_back(ExpressionElement(ExpressionElement::CEIL));
            else {
                infix.push_back(ExpressionElement(str));
                isFunction = false;
            }
            if (isFunction)
            {
                char ch = IEGetChar(iter);
                while (ch == ' ' || ch == '\t' || ch == '\n')
                {
                     iter ++;
                     ch = IEGetChar(iter);
                }
                if (ch == '(') infix.push_back(ExpressionElement(ExpressionElement::LEFT));
                else
                {
                    //错误：在函数名后必须为左括号
                    if (error != nullptr) *error = "There must be left bracket after the function name \"" + str + "\".";
                    return false;
                }
                iter ++;
                prevElementIsOperand = false;
            } else prevElementIsOperand = true;
        }
        else
        {
            //「其他类型」的元素，包含「一、二、三元运算符」和「括号」
            if (prevElementIsOperand)
            {
                //「二、三元运算符」「右括号」和「逗号」
                prevElementIsOperand = false;
                if (firstChar == '+') infix.push_back(ExpressionElement(ExpressionElement::ADD));
                else if (firstChar == '-') infix.push_back(ExpressionElement(ExpressionElement::SUB));
                else if (firstChar == '*') infix.push_back(ExpressionElement(ExpressionElement::MULT));
                else if (firstChar == '/') infix.push_back(ExpressionElement(ExpressionElement::DIV));
                else if (firstChar == '%') infix.push_back(ExpressionElement(ExpressionElement::MOD));
                else if (firstChar == '=')
                {
                    iter ++;
                    if (IEGetChar(iter) == '=') infix.push_back(ExpressionElement(ExpressionElement::ET));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator =";
                        return false;
                    }
                }
                else if (firstChar == '!')
                {
                    iter ++;
                    if (IEGetChar(iter) == '=') infix.push_back(ExpressionElement(ExpressionElement::NET));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator !";
                        return false;
                    }
                }
                else if (firstChar == '>')
                {
                    iter ++;
                    char ch = IEGetChar(iter);
                    if ((ch >= '0' && ch <= '9') ||
                        (ch >= 'A' && ch <= 'Z') ||
                        (ch >= 'a' && ch <= 'z') ||
                        ch == ' ' || ch == '\t' || ch == '\n' ||
                        ch == '_' || ch == '.')
                    {
                        infix.push_back(ExpressionElement(ExpressionElement::GT));
                        iter --;
                    }
                    else if (ch == '=') infix.push_back(ExpressionElement(ExpressionElement::GET));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator >";
                        if (error != nullptr) error->push_back(IEGetChar(iter));
                        return false;
                    }
                }
                else if (firstChar == '<')
                {
                    iter ++;
                    char ch = IEGetChar(iter);
                    if ((ch >= '0' && ch <= '9') ||
                        (ch >= 'A' && ch <= 'Z') ||
                        (ch >= 'a' && ch <= 'z') ||
                        ch == ' ' || ch == '\t' || ch == '\n' ||
                        ch == '_' || ch == '.')
                    {
                        infix.push_back(ExpressionElement(ExpressionElement::LT));
                        iter --;
                    }
                    else if (ch == '=') infix.push_back(ExpressionElement(ExpressionElement::LET));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator <";
                        if (error != nullptr) error->push_back(IEGetChar(iter));
                        return false;
                    }
                }
                else if (firstChar == '&')
                {
                    iter ++;
                    if (IEGetChar(iter) == '&') infix.push_back(ExpressionElement(ExpressionElement::AND));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator &";
                        return false;
                    }
                }
                else if (firstChar == '|')
                {
                    iter ++;
                    if (IEGetChar(iter) == '|') infix.push_back(ExpressionElement(ExpressionElement::OR));
                    else
                    {
                        //错误：未知的操作符
                        if (error != nullptr) *error = "Unknow binary operator |";
                        return false;
                    }
                }
                else if (firstChar == '?') infix.push_back(ExpressionElement(ExpressionElement::QUESTION));
                else if (firstChar == ':') infix.push_back(ExpressionElement(ExpressionElement::COLON));
                else if (firstChar == ')')
                {
                     infix.push_back(ExpressionElement(ExpressionElement::RIGHT));
                     prevElementIsOperand = true;
                }
                else if (firstChar == ',') infix.push_back(ExpressionElement(ExpressionElement::COMMA));
                else
                {
                    //错误：未知的操作符
                    if (error != nullptr) *error = "Unknow binary operator ";
                    if (error != nullptr) error->push_back(IEGetChar(iter));
                    return false;
                }
            } else {
                //「一元运算符」和「括号」
                prevElementIsOperand = false;
                if (firstChar == '!') infix.push_back(ExpressionElement(ExpressionElement::NOT));
                else if (firstChar == '-') infix.push_back(ExpressionElement(ExpressionElement::NAG));
                else if (firstChar == '(') infix.push_back(ExpressionElement(ExpressionElement::LEFT));
                else
                {
                    //错误：未知的操作符
                    if (error != nullptr) *error = "Unknow unary operator ";
                    if (error != nullptr) error->push_back(IEGetChar(iter));
                    return false;
                }
            }
            iter ++;
        }
    }

    //Step 2: 检查左右括号及三元表达式是否匹配
    IEstack<typeof (ExpressionElement::type)> typeStack;
    for (IElist<ExpressionElement>::const_iterator iter = infix.cbegin();
         iter != infix.cend(); iter ++)
    {
        if (iter->type == ExpressionElement::Bracket)
        {
            if (iter->bracket == ExpressionElement::LEFT)
                typeStack.push(ExpressionElement::Bracket);
            else if (iter->bracket == ExpressionElement::RIGHT)
            {
                if (typeStack.empty())
                {
                    //错误：存在没有左括号匹配的右括号
                    if (error != nullptr) *error = "There are right bracket(s) which does not match left bracket(s).";
                    return false;
                }
                if (typeStack.top() != ExpressionElement::Bracket)
                {
                    //错误：存在没有冒号匹配的问号
                    if (error != nullptr) *error = "There are question mark(s) which does not match colon(s).";
                    return false;
                }
                typeStack.pop();
            }
            else if (iter->bracket == ExpressionElement::COMMA)
            {
                if (typeStack.empty())
                {
                    //错误：逗号在括号外出现
                    if (error != nullptr) *error = "Comma appeared outside brackets.";
                    return false;
                }
                if (typeStack.top() != ExpressionElement::Bracket)
                {
                    //错误：三元运算符被逗号分割
                    if (error != nullptr) *error = "Ternary operator is separated by comma.";
                    return false;
                }
            }
        }
        else if (iter->type == ExpressionElement::Ternary)
        {
            if (iter->ternary == ExpressionElement::QUESTION)
                typeStack.push(ExpressionElement::Ternary);
            else if (iter->ternary == ExpressionElement::COLON)
            {
                if (typeStack.empty())
                {
                    //错误：存在没有问号匹配的冒号
                    if (error != nullptr) *error = "There are colon(s) which does not match question mark(s).";
                    return false;
                }
                if (typeStack.top() != ExpressionElement::Ternary)
                {
                    //错误：存在没有右括号匹配的左括号
                    if (error != nullptr) *error = "There are left bracket(s) which does not match right bracket(s).";
                    return false;
                }
                typeStack.pop();
            }
        }
    }
    if (!typeStack.empty())
    {
        if (typeStack.top() == ExpressionElement::Bracket)
        {
            //错误：存在没有右括号匹配的左括号
            if (error != nullptr) *error = "There are left bracket(s) which does not match right bracket(s).";
            return false;
        }
        else
        {
            //错误：存在没有冒号匹配的问号
            if (error != nullptr) *error = "There are question mark(s) which does not match colon(s).";
            return false;
        }
    }

    //Step 3:将中缀表达式转换为后缀表达式
    IElist<ExpressionElement> *postfix = new IElist<ExpressionElement>();
    IEstack<ExpressionElement> operatorStack;
    for (IElist<ExpressionElement>::const_iterator iter = infix.cbegin();
         iter != infix.cend(); iter ++)
    {
        switch (iter->type) {
        case ExpressionElement::Number:
        case ExpressionElement::Variable:
            postfix->push_back(*iter);
            break;
        case ExpressionElement::Ternary:
            switch (iter->ternary) {
            case ExpressionElement::QUESTION:
                while (!operatorStack.empty() && operatorStack.top().priority() > iter->priority()) {
                    postfix->push_back(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(*iter);
                break;
            case ExpressionElement::COLON:
                while (operatorStack.top().type != ExpressionElement::Ternary) {
                    postfix->push_back(operatorStack.top());
                    operatorStack.pop();
                }
                break;
            }
            break;
        case ExpressionElement::Bracket:
            switch (iter->bracket) {
            case ExpressionElement::LEFT:
                operatorStack.push(*iter);
                break;
            case ExpressionElement::RIGHT:
                while (operatorStack.top().type != ExpressionElement::Bracket)
                {
                    postfix->push_back(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.pop();
                if (!operatorStack.empty() && operatorStack.top().type == ExpressionElement::Function)
                {
                    postfix->push_back(operatorStack.top());
                    operatorStack.pop();
                }
                break;
            case ExpressionElement::COMMA:
                while (operatorStack.top().type != ExpressionElement::Bracket)
                {
                    postfix->push_back(operatorStack.top());
                    operatorStack.pop();
                }
                break;
            }
            break;
        default:
            while (!operatorStack.empty() && operatorStack.top().priority() >= iter->priority())
            {
                postfix->push_back(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(*iter);
        }
    }
    while (!operatorStack.empty()) {
        postfix->push_back(operatorStack.top());
        operatorStack.pop();
    }

    this->expression = expression;
    if (this->postfix != nullptr) delete this->postfix;
    this->postfix = postfix;
    return true;
}

const IEstring &InfixExpression::get() const
{
    return expression;
}

IEstring InfixExpression::getPostfix() const
{
    if (postfix == nullptr) return "";
    IEstring str = "";
    for (IElist<ExpressionElement>::const_iterator iter = postfix->cbegin();
         iter != postfix->cend(); iter ++)
    {
        switch (iter->type) {
        case ExpressionElement::Number:
            str.append(IEToString(iter->number));
            break;
        case ExpressionElement::Variable:
            str.append(iter->variable);
            break;
        case ExpressionElement::Unary:
            switch (iter->unary) {
            case ExpressionElement::NAG:
                str.push_back('(');
                str.push_back('-');
                str.push_back(')');
                break;
            case ExpressionElement::NOT:
                str.push_back('!');
                break;
            }
            break;
        case ExpressionElement::Binary:
            switch (iter->binary) {
            case ExpressionElement::ADD:
                str.push_back('+');
                break;
            case ExpressionElement::SUB:
                str.push_back('-');
                break;
            case ExpressionElement::MULT:
                str.push_back('*');
                break;
            case ExpressionElement::DIV:
                str.push_back('/');
                break;
            case ExpressionElement::MOD:
                str.push_back('%');
                break;
            case ExpressionElement::ET:
                str.push_back('=');
                str.push_back('=');
                break;
            case ExpressionElement::NET:
                str.push_back('!');
                str.push_back('=');
                break;
            case ExpressionElement::GT:
                str.push_back('>');
                break;
            case ExpressionElement::GET:
                str.push_back('>');
                str.push_back('=');
                break;
            case ExpressionElement::LT:
                str.push_back('<');
                break;
            case ExpressionElement::LET:
                str.push_back('<');
                str.push_back('=');
                break;
            case ExpressionElement::AND:
                str.push_back('&');
                str.push_back('&');
                break;
            case ExpressionElement::OR:
                str.push_back('|');
                str.push_back('|');
                break;
            }
            break;
        case ExpressionElement::Ternary:
            switch (iter->ternary) {
            case ExpressionElement::QUESTION:
                str.push_back('?');
                break;
            case ExpressionElement::COLON:
                str.push_back(':');
                break;
            }
            break;
        case ExpressionElement::Function:
            switch (iter->function) {
            case ExpressionElement::SIN:
                str.append("sin");
                break;
            case ExpressionElement::COS:
                str.append("cos");
                break;
            case ExpressionElement::TAN:
                str.append("tan");
                break;
            case ExpressionElement::ASIN:
                str.append("asin");
                break;
            case ExpressionElement::ACOS:
                str.append("acos");
                break;
            case ExpressionElement::ATAN:
                str.append("atan");
                break;
            case ExpressionElement::MAX:
                str.append("max");
                break;
            case ExpressionElement::MIN:
                str.append("min");
                break;
            case ExpressionElement::ABS:
                str.append("abs");
                break;
            case ExpressionElement::POW:
                str.append("pow");
                break;
            case ExpressionElement::LOG:
                str.append("log");
                break;
            case ExpressionElement::FLOOR:
                str.append("floor");
                break;
            case ExpressionElement::ROUND:
                str.append("round");
                break;
            case ExpressionElement::CEIL:
                str.append("ceil");
                break;
            }
            break;
        case ExpressionElement::Bracket:
            switch (iter->bracket) {
            case ExpressionElement::LEFT:
                str.push_back('(');
                break;
            case ExpressionElement::RIGHT:
                str.push_back(')');
                break;
            case ExpressionElement::COMMA:
                str.push_back(',');
                break;
            }
            break;
        }
        str.push_back(' ');
    }
    return str;
}

double InfixExpression::calc(const IEmap<IEstring, double> &parameters)
{
    IEstack<double> numbers;
    for (IElist<ExpressionElement>::const_iterator iter = postfix->cbegin();
         iter != postfix->cend(); iter ++)
    {
        switch (iter->type) {
        double num1, num2, num3;
        case ExpressionElement::Number:
            numbers.push(iter->number);
            break;
        case ExpressionElement::Variable:
            numbers.push(IEMapFind(parameters, iter->variable));
            break;
        case ExpressionElement::Unary:
            num1 = numbers.top();
            numbers.pop();
            switch (iter->unary) {
            case ExpressionElement::NAG: numbers.push(-num1); break;
            case ExpressionElement::NOT: numbers.push(num1 > 0 ? -1 : 1); break;
            }
            break;
        case ExpressionElement::Binary:
            num2 = numbers.top();
            numbers.pop();
            num1 = numbers.top();
            numbers.pop();
            switch (iter->binary) {
            case ExpressionElement::ADD: numbers.push(num1 + num2); break;
            case ExpressionElement::SUB: numbers.push(num1 - num2); break;
            case ExpressionElement::MULT: numbers.push(num1 * num2); break;
            case ExpressionElement::DIV: numbers.push(num1 / num2); break;
            case ExpressionElement::MOD: numbers.push(num1 - ((int) (num1 / num2)) * num2); break;
            case ExpressionElement::ET: numbers.push(num1 == num2 ? 1 : -1); break;
            case ExpressionElement::NET: numbers.push(num1 != num2 ? 1 : -1); break;
            case ExpressionElement::GT: numbers.push(num1 > num2 ? 1 : -1); break;
            case ExpressionElement::GET: numbers.push(num1 >= num2 ? 1 : -1); break;
            case ExpressionElement::LT: numbers.push(num1 < num2 ? 1 : -1); break;
            case ExpressionElement::LET: numbers.push(num1 <= num2 ? 1 : -1); break;
            case ExpressionElement::AND: numbers.push(num1 > 0 && num2 > 0 ? 1 : -1); break;
            case ExpressionElement::OR: numbers.push(num1 > 0 || num2 > 0 ? 1 : -1); break;
            }
            break;
        case ExpressionElement::Ternary:
            num3 = numbers.top();
            numbers.pop();
            num2 = numbers.top();
            numbers.pop();
            num1 = numbers.top();
            numbers.pop();
            numbers.push(num1 > 0 ? num2 : num3);
            break;
        case ExpressionElement::Function:
            switch (iter->function) {
            case ExpressionElement::SIN:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(sin(num1));
                break;
            case ExpressionElement::COS:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(cos(num1));
                break;
            case ExpressionElement::TAN:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(tan(num1));
                break;
            case ExpressionElement::ASIN:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(asin(num1));
                break;
            case ExpressionElement::ACOS:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(acos(num1));
                break;
            case ExpressionElement::ATAN:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(atan(num1));
                break;
            case ExpressionElement::MAX:
                num2 = numbers.top();
                numbers.pop();
                num1 = numbers.top();
                numbers.pop();
                numbers.push(num1 > num2 ? num1 : num2);
                break;
            case ExpressionElement::MIN:
                num2 = numbers.top();
                numbers.pop();
                num1 = numbers.top();
                numbers.pop();
                numbers.push(num1 < num2 ? num1 : num2);
                break;
            case ExpressionElement::ABS:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(abs(num1));
                break;
            case ExpressionElement::POW:
                num2 = numbers.top();
                numbers.pop();
                num1 = numbers.top();
                numbers.pop();
                numbers.push(pow(num1, num2));
                break;
            case ExpressionElement::LOG:
                num2 = numbers.top();
                numbers.pop();
                num1 = numbers.top();
                numbers.pop();
                numbers.push(log2(num2) / log2(num1));
                break;
            case ExpressionElement::FLOOR:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(floor(num1));
                break;
            case ExpressionElement::ROUND:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(round(num1));
                break;
            case ExpressionElement::CEIL:
                num1 = numbers.top();
                numbers.pop();
                numbers.push(ceil(num1));
                break;
            }
        case ExpressionElement::Bracket: break;
            break;
        }
    }
    return numbers.top();
}

int InfixExpression::ExpressionElement::priority() const
{
    switch (type) {
    case Binary:
        switch (binary) {
        case OR : return 10;
        case AND: return 11;
        case ET :
        case NET: return 12;
        case GT :
        case GET:
        case LT :
        case LET: return 13;
        case ADD:
        case SUB: return 14;
        case MULT:
        case DIV:
        case MOD: return 15;
        }
        break;
    case Unary: return 20;
    case Ternary: return 2;
    case Function: return 30;
    case Bracket:
        switch (bracket) {
        case LEFT : return 0;
        case RIGHT: return 40;
        case COMMA: return 1;
        }
    default: return -1;
    }
}
