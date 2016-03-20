#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

const int MAX = 1000;
const int MAXOFIDENTIFIER = 25;
const int KEYWORDNUM = 6;
const int UNARYOPERATORNUM = 7;
const int BINOCULAROPERATORNUM = 4;
const int DELIMETERNUM = 7;

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
    char token[MAXOFIDENTIFIER];  //标识符
};

TableNode tokenTable[MAX];
int tableNum = 0;


const char* const keyWords[KEYWORDNUM] = {"begin", "else", "end",
"if", "then","while"};

const char unaryOperators[] = {'+','-','*','/','=','<','>'};

const char* const binocularOperators[] = {"<=",">=",":=","<>"};

const char delimeters[] = {'(', ')' , ',' , ';' , '[' , ']' ,  '"'};


char* Express(int& tableIndex);
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
        cout << "第" << line <<"行" << str << " 变量的长度超过限制！\n";
                   break;
    case POINTERROR:
        cout << "第" << line <<"行" << str << " 小数点错误！\n";
                  break;
    case LENGTHERROROFCONSTANT:
        cout << "第" << line <<"行" << str << " 常量的长度超过限制！\n";
                break;
    case UNKNOWNIDENTIFIER:
        cout << "第"  << line << "行" << str << "未知字符！\n";
    }
}

void Scanner(const char* sourceCode)
{
    int codeLength = strlen(sourceCode);
    int chIndex = 0;
    int line = 1;
    int tableIndex = 0;

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
                        tokenTable[tableIndex].type = KEYWORD;
                        tokenTable[tableIndex].line = line;
                        strcpy(tokenTable[tableIndex].token,str);
                        tableIndex++;
                        break;
                    }

                if (i >= KEYWORDNUM)
                {
                    tokenTable[tableIndex].type = IDENTIFIER;
                    tokenTable[tableIndex].line = line;
                    strcpy(tokenTable[tableIndex].token,str);
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
                tokenTable[tableIndex].line = line;
                strcpy(tokenTable[tableIndex].token,str);
                if (pointFlag == 0)
                    tokenTable[tableIndex].type = INTEGER;
                else
                    tokenTable[tableIndex].type = FLOAT;
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
                    tokenTable[tableIndex].line = line;
                    tokenTable[tableIndex].type = BOPERATOR;
                    strcpy(tokenTable[tableIndex].token,str);
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
                        tokenTable[tableIndex].line = line;
                        tokenTable[tableIndex].type = UOPERATOR;
                        tokenTable[tableIndex].token[0] = sourceCode[chIndex];
                        tokenTable[tableIndex].token[1] = 0;
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
                        tokenTable[tableIndex].line = line;
                        tokenTable[tableIndex].type = DELIMETER;
                        tokenTable[tableIndex].token[0] = sourceCode[chIndex];
                        tokenTable[tableIndex].token[1] = 0;
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

    tableNum = tableIndex;
}

void Emit(char* result,char* leftParameter,char* op,char* rightParameter)
{
    cout << result << "=" << leftParameter << op << rightParameter << endl;
}

char* NewVar()
{
   static int k = 0;

   k++;

   char* tempVar = new char[5];
   tempVar[0] = 't';

   char numtoch[5];
   itoa(k,numtoch,10);
   strcmp(tempVar+1,numtoch);

   return tempVar;
}

char* Factor(int& tableIndex)
{
    char* result = new char[MAXOFIDENTIFIER];

    if (tokenTable[tableIndex].type == IDENTIFIER
        || tokenTable[tableIndex].type == INTEGER
        || tokenTable[tableIndex].type == FLOAT)
    {
        strcpy(result,tokenTable[tableIndex].token);
        tableIndex++;
    }
    else if (strcmp("(",tokenTable[tableIndex].token) == 0)
    {
        tableIndex++;
        strcpy(result, Express(tableIndex));
        if (strcmp(")",tokenTable[tableIndex].token) == 0)
            tableIndex++;
        else
        {
            errorFlag = true;
            cout << "缺少)！" << endl;
        }
    }
    else
    {
        errorFlag = true;
        cout << "缺少(！" << endl;
    }

    return result;
}
char* Term(int& tableIndex)
{
    char* result = new char[MAXOFIDENTIFIER];
    char leftParameter[MAXOFIDENTIFIER];
    char rightParameter[MAXOFIDENTIFIER];
    char op[5];

    strcpy(leftParameter,Factor(tableIndex));

    while (strcmp("*",tokenTable[tableIndex].token) == 0
           || strcmp("/",tokenTable[tableIndex].token) == 0)
    {
        strcpy(op,tokenTable[tableIndex].token);
        tableIndex++;
        strcpy(rightParameter,Factor(tableIndex));
        strcpy(result,NewVar());
        Emit(result,leftParameter,op,rightParameter);
    }

    return result;
}
char* Express(int& tableIndex)
{
    char* result = new char[MAXOFIDENTIFIER];
    char leftParameter[MAXOFIDENTIFIER];
    char rightParameter[MAXOFIDENTIFIER];
    char op[5];

    strcpy(leftParameter,Term(tableIndex));

    while (strcmp("+",tokenTable[tableIndex].token) == 0
           || strcmp("-",tokenTable[tableIndex].token) == 0)
    {
        strcpy(op,tokenTable[tableIndex].token);
        tableIndex++;

        strcpy(rightParameter,Term(tableIndex));

        strcpy(result,NewVar());

        Emit(result,leftParameter,op,rightParameter);
    }

    return result;
}
void Statement(int& tableIndex)
{
    if (tokenTable[tableIndex].type == IDENTIFIER)
    {
        char* leftValue = tokenTable[tableIndex].token;
            tableIndex++;

        if (strcmp(":=",tokenTable[tableIndex].token) == 0)
        {
            tableIndex++;
            char rightValue[MAXOFIDENTIFIER];
            strcpy(rightValue,Express(tableIndex));
            Emit(leftValue,rightValue,"","");
        }
        else
        {
            errorFlag = true;
            cout << "缺少赋值符！" << endl;
        }
    }
}

void Block(int& tableIndex)
{
    Statement(tableIndex);
    while (strcmp(";",tokenTable[tableIndex].token) == 0)
    {
        tableIndex++;
        Statement(tableIndex);
    }
}

void Parser()
{
    int tableIndex = 0;

    if (strcmp("begin",tokenTable[tableIndex].token) == 0)
    {
        tableIndex++;
        Block(tableIndex);

        if (strcmp("end",tokenTable[tableIndex].token) == 0)
            cout << "成功！" << endl;
        else
        {
            errorFlag = true;
            cout << "缺少end!" << endl;
        }
    }
    else
    {
        errorFlag = true;
        cout << "缺少begin!" << endl;
    }
}
int main()
{
    char sourceCode[MAX];
    cout << "请输入源程序（输入结束标志位#）：\n";


    cin.getline(sourceCode,MAX,'#');
    Scanner(sourceCode);

    Parser();

    if (errorFlag)
        return 0;

    return 0;
}
