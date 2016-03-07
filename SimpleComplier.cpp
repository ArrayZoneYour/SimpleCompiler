#include <iostream>
#include <cstring>

using namespace std;

const int MAX = 1000;
const int MAXOFIDENTIFIER = 25;
const int KEYWORDNUM = 30;
const int UNARYOPERATORNUM = 8;
const int BINOCULAROPERATORNUM = 4;
const int DELIMETERNUM = 11;

bool errorFlag = false;

enum ErrorType
{
    LENGTHERROROFIDENTIFIER = 1,
    POINTERROR,
    LENGTHERROROFCONSTANT,
    UNKNOWNIDENTIFIER
};

enum IDType
{
    KEYWORD = 1,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    UOPERATOR,
    BOPERATOR,
    DELIMETER,
    OTHER
};

struct TableNode
{
    int type;     //标识符的类型
    int line;     //标识符所在行数
    char symbol[MAXOFIDENTIFIER];  //标识符
};

TableNode symbolTable[MAX];
int tableIndex = 0;


const char* const keyWords[KEYWORDNUM] = {"and", "array", "begin", "case","char" ,"constant", "do",  "else", "end",
"false","for",  "if", "input", "integer", "not", "of", "or", "output","procedure", "program",
 "read", "real","repeat", "set", "then", "to", "type", "until", "var","while"};

const char unaryOperators[] = {'+','-','*','/','=','#','<','>'};

const char* const binocularOperators[] = {"<=",">=",":=","<>"};

const char delimeters[] = {'(', ')' , ',' , ';' , '.' , '[' , ']' , ':' , '{' , '}' , '"'};

//errorTYpe=1 变量长度错误
//errorType=2 小数点错误
//errorTYpe=3 常量长度错误
void Error(ErrorType errorType,int line,const char* str)
{
    errorFlag = true;

    cout << "Error" << endl;

    switch (errorType)
    {
    case LENGTHERROROFIDENTIFIER:
        cout << "第" << line-1 <<"行" << str << " 变量的长度超过限制！\n";
                   break;
    case POINTERROR:
        cout << "第" << line-1 <<"行" << str << " 小数点错误！\n";
                  break;
    case LENGTHERROROFCONSTANT:
        cout << "第" << line-1 <<"行" << str << " 常量的长度超过限制！\n";
                break;
    case UNKNOWNIDENTIFIER:
        cout << "第"  << line-1 << "行" << str << "未知字符！\n";
    }
}

void Scanner(const char* sourceCode)
{
    int codeLength = strlen(sourceCode);
    int chIndex = 0;
    int line = 1;

    while (chIndex < codeLength)
    {
        //处理空格和Tab
        while (chIndex < codeLength
               && (sourceCode[chIndex] == ' '
               || sourceCode[chIndex] == '\t'))
            chIndex++;
        if (chIndex >= codeLength)
            break;

        while (chIndex < codeLength
               && sourceCode[chIndex] == '\n')
        {
            line++;
            chIndex++;
        }
        if (chIndex >= codeLength)
            break;

        //标识符
        if (isalpha(sourceCode[chIndex])
            || sourceCode[chIndex] == '_')
        {
            char str[100];
            int strLen = 0;
            str[strLen] = sourceCode[chIndex];
            strLen++;
            chIndex++;

            while (chIndex<codeLength
                   && (isalnum(sourceCode[chIndex]) ||
                    sourceCode[chIndex] == '_'))
             {
                 str[strLen] = sourceCode[chIndex];
                 strLen++;
                 chIndex++;
             }
            if (chIndex >= codeLength)
                break;

            str[strLen] = 0;
            if (strLen > MAXOFIDENTIFIER)
                Error(LENGTHERROROFIDENTIFIER,line,str);

            else
            {
                int i;
                for (i = 0;i < KEYWORDNUM;i++)
                    if (strcmp(str,keyWords[i]) == 0)
                    {
                        symbolTable[tableIndex].type = KEYWORD;
                        symbolTable[tableIndex].line = line;
                        strcpy(symbolTable[tableIndex].symbol,str);
                        tableIndex++;
                        break;
                    }

                if (i >= KEYWORDNUM)
                {
                    symbolTable[tableIndex].type = IDENTIFIER;
                    symbolTable[tableIndex].line = line;
                    strcpy(symbolTable[tableIndex].symbol,str);
                    tableIndex++;
                }
            }
        }

        //常数
        else if (isdigit(sourceCode[chIndex]))
        {
            char str[100];
            int strLen = 0;
            int pointFlag = 0;//0表示整数，1表示小数，2表示错误
            str[strLen++] = sourceCode[chIndex++];

            while (chIndex < codeLength
                   && (isdigit(sourceCode[chIndex])
                       || sourceCode[chIndex] == '.'))
            {
                if (sourceCode[chIndex] == '.')
                    pointFlag++;
                str[strLen++] = sourceCode[chIndex];
                chIndex++;
            }
            if (chIndex >= codeLength)
                break;

            str[strLen] = 0;

            if (strLen > 20)
                Error(LENGTHERROROFCONSTANT,line,str);

            if (pointFlag > 1)
                Error(POINTERROR,line,str);
            else
            {
                symbolTable[tableIndex].line = line;
                strcpy(symbolTable[tableIndex].symbol,str);
                if (pointFlag == 0)
                    symbolTable[tableIndex].type = INTEGER;
                else
                    symbolTable[tableIndex].type = FLOAT;
                tableIndex++;
            }
        }

        //操作符、界符。未知字符
        else
        {
            char str[3];
            str[0] = sourceCode[chIndex];
            str[1] = sourceCode[chIndex+1];
            str[2] = 0;

            int i;
            for (i = 0;i < BINOCULAROPERATORNUM;i++)
                if (strcmp(str,binocularOperators[i]) == 0)
                {
                    symbolTable[tableIndex].line = line;
                    symbolTable[tableIndex].type = BOPERATOR;
                    strcpy(symbolTable[tableIndex].symbol,str);
                    tableIndex++;
                    chIndex += 2;
                    break;
                 }

            int j;
            if (i >= BINOCULAROPERATORNUM)
            {
                for (j = 0;j < UNARYOPERATORNUM;j++)
                    if (sourceCode[chIndex] == unaryOperators[j])
                    {
                        symbolTable[tableIndex].line = line;
                        symbolTable[tableIndex].type = UOPERATOR;
                        symbolTable[tableIndex].symbol[0] = sourceCode[chIndex];
                        symbolTable[tableIndex].symbol[1] = 0;
                        tableIndex++;
                        chIndex++;
                        break;
                    }
            }

            int k;
            if (i == BINOCULAROPERATORNUM
                && j == UNARYOPERATORNUM)
            {
                for (k = 0;k < DELIMETERNUM;k++)
                {
                    if (str[0] == delimeters[k])
                    {
                        symbolTable[tableIndex].line = line;
                        symbolTable[tableIndex].type = DELIMETER;
                        symbolTable[tableIndex].symbol[0] = sourceCode[chIndex];
                        symbolTable[tableIndex].symbol[1] = 0;
                        tableIndex++;
                        chIndex++;
                        break;
                    }
                }
            }

            if (i == BINOCULAROPERATORNUM
                && j == UNARYOPERATORNUM
                && k == DELIMETERNUM)
                Error(UNKNOWNIDENTIFIER,line,str);
        }
    }
}

int main()
{
    char sourceCode[MAX];   //存放输入代码的缓冲区
    cout << "请输入源程序（输入结束标志位#）：\n";

    /***********************************从键盘输入****************************/
    cin.getline(sourceCode,MAX,'#');
    Scanner(sourceCode); //调用扫描函数

    if (errorFlag)
        return 0;

    cout << "\n符号表：\n";
    cout <<"单词"<<"      "<<"类型" << endl;
    for (int i = 0;i < tableIndex;i++)
        cout << symbolTable[i].symbol <<"       "<<symbolTable[i].type <<endl;

    return 0;
}
