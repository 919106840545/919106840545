#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPushButton>
#include<QMessageBox>
#include<map>
#include<stack>
#include<cmath>
#include<QClipboard>
#include<QApplication>
#include<QDebug>
#include<QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
ui->lineEdit_3->setAlignment(Qt::AlignRight);
ui->cmp_a->setAlignment(Qt::AlignRight);
ui->cmp_b->setAlignment(Qt::AlignRight);
//stack2
ui->lineEdit->setAlignment(Qt::AlignRight);
ui->lineEdit->setText("0");
waitForOperand=true;
setWindowTitle("计算器");
connectSlots();
setShortcutKeys();
//

}

QCalculatorDec::QCalculatorDec()
{

}

QQueue<QString> QCalculatorDec::Split(const QString& exp)          //分离前缀
{
    QQueue<QString> ret;
    QString num="";

    for(int i=0;i<exp.length();i++)
    {
        if( (exp[i]=='.') || ( (exp[i]>='0') && (exp[i]<='9')) || ((exp[i]>='a')&&(exp[i]<='f'))|| exp[i]=='B' ||exp[i]=='D' || exp[i]=='H')    //判断小数点和数字，后缀
        {
            //处理数字的进制
            if(exp[i]=='D')
                continue;
            else if(exp[i]=='B')
            {
                bool x;
                qlonglong temp = num.toLongLong(&x,2);
                num.setNum(temp, 10);
                continue;
            }
            else if(exp[i]=='H')
            {
                bool x;
                qlonglong temp = num.toLongLong(&x,16);

                num.setNum(temp, 10);
                continue;
            }
            else
            num += exp[i];
        }

        else if(exp[i]== '(' || exp[i]== ')' || exp[i]== '*' || exp[i]== '/'  )
        {
            if(!num.isEmpty())
            {
                ret.enqueue(num);        //将数字及后缀入队列
                num.clear();
            }
            ret.enqueue(exp[i]);
        }

        else if(exp[i]== '+' || exp[i]== '-')           // + - 需要特殊处理
        {
            if(i==0)       //表达式开头,说明是正负号
            {
             num+= exp[i];
            }

            else if(exp[i-1]=='(' || exp[i-1]=='+' || exp[i-1]=='-' || exp[i-1]=='*' || exp[i-1]=='/')
            {
             num+= exp[i];
            }
            else        //否则是加减运算符
            {
                if(!num.isEmpty())
                {
                    ret.enqueue(num);        //将数字入队列
                    num.clear();
                }
             ret.enqueue(exp[i]);
            }
        }
    }

    if(!num.isEmpty())         //遍历完成,判断是否还有数字
    {
        ret.enqueue(num);
        num.clear();
    }
return ret;
}

QQueue<QString> QCalculatorDec::Transfer(QQueue<QString>& exp)     //将中缀队列转换为后缀队列
{
    QStack<QString> stack;
    QQueue<QString> ret;
    bool num_ok;
    QString symbol;

    while(!exp.isEmpty())
    {
      symbol = exp.dequeue();   //出队列
      symbol.toDouble(&num_ok);

      if(num_ok==true)          //数字
      {
           stack.push(symbol);
      }

      else if(symbol=="+"||symbol=="-")
      {
          while(!stack.isEmpty() &&(stack.top()!="("))
          {
              ret.enqueue(stack.pop());     //取出栈顶运算符并入队列
          }
          stack.push(symbol);
      }

      else if(symbol=="*"||symbol=="/")
      {
          while(!stack.isEmpty() && (stack.top()!="(") && (stack.top()!="+") && (stack.top()!="-"))
          {
              ret.enqueue(stack.pop());     //取出栈顶运算符并入队列
          }
          stack.push(symbol);
      }

      else if(symbol == "(")
      {
         stack.push(symbol);
      }

      else if(symbol ==")")
      {
          while(!stack.isEmpty() && (stack.top()!="("))
          {
              ret.enqueue(stack.pop());     //取出栈顶运算符并入队列
          }
          if(stack.top()=="(")
            stack.pop();
      }
    }

    while(!stack.isEmpty()&& (stack.top()!="("))         //遍历完成,判断栈里是否为空
    {
       ret.enqueue(stack.pop());     //取出栈顶运算符并入队列
    }return ret;
}

QString QCalculatorDec::ValidNum(QString str)
 {
    QString num;

    if(str.indexOf(".")== -1) //判断是否小数
        return str;

    while(str.length()>1)   //避免0被去掉
    {
         num=str.right(1);
         if(num=="."||num=="0")
         {
             str.chop(1);
             if(num==".")
                 return  str;
         }
         else
             return str;
    }
    return str;
}

QString QCalculatorDec::Calculate(QString& l,QString& op,QString& r )
{
    double left,right,res;
    QString ret="";
    left = l.toDouble();
    right = r.toDouble();
    if(op == "+")
    {
        res = left + right;
    }

    else if(op == "-")
    {
        res = left - right;
    }

    else if(op == "*")
    {
        res = left * right;
    }

    else if(op == "/")
    {
        if( (right>(-0.000000000000001)) && (right<(0.000000000000001)) )   //判断除数为0
            return NULL;
        else
            res = left/right;
    }

    ret.sprintf("%f",res);
    return ret;
}

QString QCalculatorDec::Calculate(QQueue<QString>& exp)            //将后缀队列计算出结果
{
    QStack<QString> stack;
    QString symbol,L,R,op,ret;
    bool num_ok;

    while(!exp.isEmpty())
    {
      symbol = exp.dequeue();   //出队列
      symbol.toDouble(&num_ok);

      if(num_ok==true)      //数字
      {
        stack.push(symbol);
      }
      else                  //运算符
      {
          if(stack.size()<2)
              return "Error";

          R= stack.pop();
          L= stack.pop();
          ret = Calculate(L,symbol,R );
          if(ret==NULL)
              return ret;

          stack.push(ret);
      }
    }
    if(stack.size()==1) //遍历完成,结果只有一个
    {
         return ValidNum(stack.pop());
    }
    else
    {return "Error";
    }
}

QString QCalculatorDec::Result(const QString& exp)
{
      QQueue<QString> q=Split(exp); //分离中缀
      q=Transfer(q);                //转换为后缀
      return Calculate(q);          //返回结果
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_yunsuan_released()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_radioButton_released()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_bijiao_released()
{
    ui->stackedWidget->setCurrentIndex(2);
}


int  MainWindow::LastMatchingBoth(QString& str1,const char* str2)      //反向匹配str1和str2
{
    for(int i=str1.length();i>=0;i--)
    {
        for(unsigned int j=0;j<strlen(str2);j++)
            if(str1[i]==str2[j])
                 return i;
    }
   return -1;
}

int  MainWindow::MatchingBoth(QString& str1,const char* str2)          //匹配str1和str2,判断str1是否有str2的字符
{
    for(int i=0;i<str1.length();i++)
    {
        for(unsigned int j=0;j<strlen(str2);j++)
            if(str1[i]==str2[j])
                 return i;//找到了返回对应下表
    }
   return -1;//没找到返回-1
}

void MainWindow::handler_clicked()      //处理按键消息
{
    static int ClearLine=0;

    QPushButton *btn =dynamic_cast<QPushButton* >(sender()); //获取对象
    QString line = ui->lineEdit_3->text();
    QString text = btn->text();     //获取消息

    if(ClearLine)
    {
        ui->lineEdit_3->setText("");
        line.clear();
        ClearLine=0;
    }if(text>="0"&&text<="9" )    //数字
    {
        QString tmp= line.right(1);
        if(tmp.length() && (tmp[0]==')'||tmp[0] == "B"||tmp[0] == "D"||tmp[0] == "H"))   //数字前面不能为右括号和后缀
        {
            return;
        }
        line+=text;
    }
    else if(text>="a"&&text<="f")   //十六进制字符
    {
        QString tmp= line.right(1);
        if(tmp.length() && (tmp[0]==')'||tmp[0] == "B"||tmp[0] == "D"||tmp[0] == "H"))   //字符前面不能为右括号和后缀
        {
            return;
        }
        int pos= LastMatchingBoth(line,"+-*/.()");   //反向查找
         if(pos!= -1 &&line[pos]=='.' )        //存在小数点
         {
             return ;
         }
         line+=text;
    }
    else if( text == "H")   //十六进制
    {
        QString tmp= line.right(1);
        if(tmp.length())//后缀前只能是数字
        {
            if(MatchingBoth(tmp,"0123456789abcdef")== -1)  //没找到数字
            {
                return;
            }
            int pos= LastMatchingBoth(line,"+-*/().");   //反向查找
             if(pos!= -1 &&line[pos]=='.' )        //存在小数点
             {
                 return ;
        }
            line+=text;
        }

    }
    else if( text == "D")   //十进制
    {
        QString tmp= line.right(1);
        if(tmp.length())//后缀前只能是数字
        {
            if(MatchingBoth(tmp,"0123456789")== -1)  //没找到数字
            {
                return;
            }
            int pos= LastMatchingBoth(line,"+-*/()abcdef");   //反向查找
             if(pos!= -1 &&( line[pos]>='a'&&line[pos]<='f' ))        //存在十六进制字符
             {
                 return ;
             }

        line+=text;}
    }
    else if( text == "B")   //二进制
    {
        QString tmp= line.right(1);
        if(tmp.length())//后缀前只能是数字
        {
            if(MatchingBoth(tmp,"01")== -1)  //没找到数字
            {
                return;
            }
            int pos= LastMatchingBoth(line,"+-*/.()");   //反向查找
             if(pos!= -1 &&line[pos]=='.' )        //存在小数点
             {
                 return ;
             }

        line+=text;}
    }


    else if(text=="." )    //小数点
    {
        QString tmp= line.right(1);
        if(tmp.length()) //小数点前面只能是数字
       {
            if(MatchingBoth(tmp,"0123456789")== -1||MatchingBoth(tmp,"abcdef") !=-1)  //没找到数字
            {
                return;
            }
            int pos= LastMatchingBoth(line,"+-*/()abcdef");   //反向查找
             if(pos!= -1 &&( line[pos]>='a'&&line[pos]<='f' ))        //存在十六进制字符
             {
                 return ;
             }
       }
       else             //小数点前面为空
       {
                return ;
       }

       int pos= LastMatchingBoth(line,"+-*/.()");   //反向查找
        if(pos!= -1 &&line[pos]=='.' )        //一串数字只能有一个小数点
        {
            return ;
        }
         line+=text;
    }

    else if(text=="+"||text=="-")       //加减号
    {
        QString tmp= line.right(1);
        if(tmp.length() )
        if( MatchingBoth(tmp,"()+-*/DBH") == -1)//前面不能为此外的符号
       {
          return ;
       }
       tmp= line.right(2);
       if(tmp.length()==2)          //前面不能连续有两次加减乘除
       {
           if(tmp[0]=='+'||tmp[0]=='-'||tmp[0]=='*'||tmp[0]=='/'||tmp[0]=='(')
                if(tmp[1]=='+'||tmp[1]=='-'||tmp[1]=='*'||tmp[1]=='/')
                            return ;
       }
        line+=text;
    }

    else if(text=="*"||text=="/")       //乘除号
    {
         QString tmp= line.right(1);
         if(tmp.length())       //前面只能为后缀和右括号//不能为:左括号,小数点,加减乘除,
         {
             if(MatchingBoth(tmp,"(.+-*/")!= -1) //查找左括号,小数点,加减乘除
             {
                 return;
             }
         }
         else                   //乘除号前面不能为空
              return;
        if(MatchingBoth(tmp,"HBD)") != -1)
        line+=text;
    }

    else if(text=="(")       //左括号
    {
        QString tmp= line.right(1);
        if(tmp.length())             //前面不能为:右括号,数字,小数点,后缀
        {
            if(MatchingBoth(tmp,")0123456789abcdef.HDB")!= -1) //查找右括号,数字,小数点,后缀
            {
                return;
            }
        }

        tmp= line.right(2);
        if(tmp.length()==2)          //前面不能连续有两次加减乘除
        {
             if(tmp[0]=='+'||tmp[0]=='-'||tmp[0]=='*'||tmp[0]=='/')
                 if(tmp[1]=='+'||tmp[1]=='-'||tmp[1]=='*'||tmp[1]=='/')
                             return ;
        }
         line+=text;
         bracket_cnt++;
    }

    else if(text==")")       //右括号
    {
        QString tmp= line.right(1);
        if(bracket_cnt==0)  //前面没有左括号
           return;

        if(tmp.length())             //前面不能为:加减乘除,小数点,左括号//只能为后缀和右括号
        {
           if(MatchingBoth(tmp,"HBD)")!= -1) //查找加减乘除,小数点,左括号//后缀和右括号
           {
               line+=text;
               bracket_cnt--;

           }
        }
        else                    //右括号前面不能为空
           return;


    }

    else if(text=="退格")       //<-
    {
        if(line.length())
        line.chop(1);
    }

    else if(text=="清除")       //清空
    {
        line.clear();
        bracket_cnt=0;
    }

    else if(text=="="&& line.length())
    {QString tmp= line.right(1);
        if(MatchingBoth(tmp,"HBD)")!= -1)
    {
        QString ret=mDec.Result(line);
        if(ret==NULL)   //除数为0
        {
            line += " : ";
            line +="除数不能为0";
        }
        else if(ret=="wrong")
        {
            line +=" : ";
            line +="TAN(0)，错误";
        }
        else if(ret=="Error")
         {
            line += ":";
            line +="格式出错";
         }
         else
         {
             line += "=";
             line += ret;
         }
        ClearLine =1;
    }}
    ui->lineEdit_3->setText(line);//显示结果
}

void MainWindow::on_one_released()
{
    handler_clicked();
}
void MainWindow::on_two_released()
{
    handler_clicked();
}
void MainWindow::on_three_released()
{
    handler_clicked();
}
void MainWindow::on_four_released()
{
    handler_clicked();
}
void MainWindow::on_five_released()
{
    handler_clicked();
}
void MainWindow::on_six_released()
{
    handler_clicked();
}
void MainWindow::on_seven_released()
{
    handler_clicked();
}
void MainWindow::on_eight_released()
{
    handler_clicked();
}
void MainWindow::on_nine_released()
{
    handler_clicked();
}
void MainWindow::on_a_released()
{
    handler_clicked();
}
void MainWindow::on_b_released()
{
    handler_clicked();
}
void MainWindow::on_c_released()
{
    handler_clicked();
}
void MainWindow::on_d_released()
{
    handler_clicked();
}
void MainWindow::on_e_released()
{
    handler_clicked();
}
void MainWindow::on_f_released()
{
    handler_clicked();
}



void MainWindow::on_bina_released()
{
     handler_clicked();
}
void MainWindow::on_dig_released()
{
     handler_clicked();
}
void MainWindow::on_hex_released()
{
     handler_clicked();
}



void MainWindow::on_plus_released()
{
    handler_clicked();
}

void MainWindow::on_minus_released()
{
    handler_clicked();
}

void MainWindow::on_mult_released()
{
    handler_clicked();
}

void MainWindow::on_div_released()
{
    handler_clicked();
}

void MainWindow::on_left_released()
{
    handler_clicked();
}

void MainWindow::on_reght_released()
{
    handler_clicked();
}

void MainWindow::on_epual_released()
{
    handler_clicked();
}

void MainWindow::on_point_released()
{
    handler_clicked();
}

void MainWindow::on_zero_released()
{
    handler_clicked();
}

void MainWindow::on_bakcspace_released()
{
    handler_clicked();
}

void MainWindow::on_clear_released()
{
    handler_clicked();
}



//stack2
void MainWindow::abortOperation()
{

    ui->lineEdit->setText("0");
    waitForOperand=true;
    QMessageBox::warning(this,"运算错误","错误！");
}



void MainWindow::connectSlots()
{

    QPushButton *digitaBtns[10]={
        ui->digtalBtn0,ui->digtalBtn1,ui->digtalBtn2,ui->digtalBtn3,ui->digtalBtn4,
        ui->digtalBtn5,ui->digtalBtn6,ui->digtalBtn7,ui->digtalBtn8,ui->digtalBtn9
    };

    for(auto btn:digitaBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::digitClicked);

    QPushButton *operatorBtns[8]={
        ui->addBtn,ui->subtractionBtn,ui->mulBtn,ui->divBtn,ui->leftBracketBtn,
        ui->rightBracketBtn,ui->powBtn,ui->percrntBtn

        };
    for(auto btn:operatorBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::operatorClicked);
    QPushButton *specialOperatorBtns[5]={
        ui->sinBtn,ui->cosBtn,ui->tanBtn,ui->lnBtn,ui->lgBtn
    };
    for(auto btn:specialOperatorBtns)
        connect(btn,&QPushButton::clicked,this,&MainWindow::specialOperatorClicked);
}
 void MainWindow::setShortcutKeys()
 {
     Qt::Key key[27]={Qt::Key_0,Qt::Key_1,Qt::Key_2,Qt::Key_3,
                      Qt::Key_4,Qt::Key_5,Qt::Key_6,Qt::Key_7,
                      Qt::Key_8,Qt::Key_9,
                      Qt::Key_Plus,Qt::Key_Minus,Qt::Key_Asterisk,Qt::Key_Slash,
                      Qt::Key_Enter,Qt::Key_Period,Qt::Key_Backspace,Qt::Key_M,
                      Qt::Key_ParenLeft,Qt::Key_ParenRight,Qt::Key_AsciiCircum,
                      Qt::Key_Percent,Qt::Key_S,Qt::Key_C,Qt::Key_T,Qt::Key_N,Qt::Key_G};
     QPushButton *btn[27]={
         ui->digtalBtn0,ui->digtalBtn1,ui->digtalBtn2,ui->digtalBtn3,
         ui->digtalBtn4,ui->digtalBtn5,ui->digtalBtn6,ui->digtalBtn7,
         ui->digtalBtn8,ui->digtalBtn9,
         ui->addBtn,ui->subtractionBtn,ui->mulBtn,ui->divBtn,ui->equalBtn,ui->pointBtn,
         ui->clearAllBtn,ui->signBtn,ui->leftBracketBtn,ui->rightBracketBtn,ui->powBtn,
         ui->percrntBtn,ui->sinBtn,ui->cosBtn,ui->tanBtn,ui->lnBtn,ui->lgBtn
     };
     for(int i=0;i<27;i++)
         btn[i]->setShortcut(QKeySequence(key[i]));
     ui->clearAllBtn->setShortcut(QKeySequence("Ctrl+Backspace"));

 }

void MainWindow::digitClicked()
{
    QPushButton *digitBtn=static_cast<QPushButton*>(sender());

    QString value=digitBtn->text();

    if(ui->lineEdit->text()=="0"&&value=="0")
        return;

    if(waitForOperand)
    {
        ui->lineEdit->setText(value);
        waitForOperand=false;
    }
    else
    {
        ui->lineEdit->setText(ui->lineEdit->text()+value);
    }
}


void MainWindow::operatorClicked()
{
    QPushButton *clickedBtn=qobject_cast<QPushButton*>(sender());
    QString clickedOperator=clickedBtn->text();
    if(waitForOperand==true&&clickedOperator=='(')//首符号为‘（’的到时候
        {
            ui->lineEdit->setText(clickedOperator);
            waitForOperand=false;
        }
    else if(waitForOperand==false)
        ui->lineEdit->setText(ui->lineEdit->text()+clickedOperator);

}
void MainWindow::specialOperatorClicked()
{
    QPushButton *specialOperatorBtn=static_cast<QPushButton*>(sender());
    QString value=specialOperatorBtn->text();
    if(waitForOperand)
    {
        ui->lineEdit->setText(value);
        waitForOperand=false;
    }
    else
    {
        ui->lineEdit->setText(ui->lineEdit->text()+value);
    }
}


void MainWindow::on_equalBtn_clicked()
{

    double result=0.0;
    try
    {
        result=compute(inToPost(ui->lineEdit->text()));
//        QMessageBox::warning(this,"提示",inToPost(ui->lineEdit->text()));//查看后缀表达式
    }
    catch(const char *er)
    {
        error=er;
        abortOperation();
        return;
    }

    ui->lineEdit->setText(ui->lineEdit->text()+'='+QString::number(result));
    log=ui->lineEdit->text();
    waitForOperand=true;
}

void MainWindow::on_clearBtn_clicked()
{
    //将显示字符串退格
    QString str=ui->lineEdit->text();
    if(str!='0'&&waitForOperand==false)
    {
        ui->lineEdit->setText(str.left(str.count()-1));
        waitForOperand=false;
    }
    else
    {
        ui->lineEdit->setText("0");
        waitForOperand=true;
    }



}

void MainWindow::on_clearAllBtn_clicked()
{
    //将当前显示全部归零
    ui->lineEdit->setText("0");
    waitForOperand=true;

}



void MainWindow::on_signBtn_clicked()
{

   QString text=ui->lineEdit->text();
   QChar sign=text[text.size()-1];
   if(sign=='-')
   {
       text.remove(text.size()-1,1);
   }
   else {
       text.append('-');
   }

   ui->lineEdit->setText(text);
}



void MainWindow::on_pointBtn_clicked()
{
    if(waitForOperand==false&&ui->lineEdit->text().data()[ui->lineEdit->text().size()-1].isDigit()==true)
         ui->lineEdit->setText(ui->lineEdit->text()+".");

}

QString MainWindow::inToPost(QString infix) throw(const char*)//将中缀表达式转换为后缀表达式
{
    std::stack<char> stack;
    char current=0;
    QString postfix;
    std::map<char,int> priority;
    priority['+']=0;
    priority['-']=0;
    priority['*']=1;
    priority['/']=1;
    priority['^']=2;
    priority['s']=3;
    priority['c']=3;
    priority['t']=3;
    priority['n']=3;
    priority['g']=3;


    for(int i=0;i<infix.length();i++)
    {
        current=infix[i].toLatin1();
        if(isdigit(current))
        {
            postfix.push_back(current);
            continue;
        }
        switch (current)
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':

            if(infix[i-1]!=')')
            {
                if(infix[i-1].isDigit())
                    postfix.push_back('#');
                else if(infix[i-1]=='%')
                {}
//                    infix.begin();
                else
                    throw "expression is illegality";
            }
            if(stack.empty()==false)
            {
                char tempTop=stack.top();
                while(tempTop!='('&&priority[current]<=priority[tempTop])
                {
                    stack.pop();
                    postfix.push_back(tempTop);
                    if(stack.empty())
                        break;
                    tempTop=stack.top();
                }
            }
            stack.push(current);

            break;

        case's':
        case'c':
        case't':
            if(i>0&&infix[i-1].isDigit())
                throw"表达式非法";
            if(!stack.empty())
            {
                char temTop=stack.top();
                while(temTop!='('&&priority[current]<priority[temTop])
                {
                    stack.pop();
                    postfix.push_back(temTop);
                    if(stack.empty())
                        break;
                    temTop=stack.top();
                }
            }
            stack.push(current);
            i=i+2;
            break;

        case'l':
            if(i>0&&infix[i-1].isDigit())
                throw"表达式非法";
            if(!stack.empty())
            {
                char temTop=stack.top();
                while(temTop!='('&&priority[infix[i+1].toLatin1()]<priority[temTop])
                {
                    stack.pop();
                    postfix.push_back(temTop);
                    if(stack.empty())
                        break;
                    temTop=stack.top();
                }
            }
            stack.push(infix[i+1].toLatin1());
            i=i+1;
            break;

        case'.':
            postfix.push_back(current);
            break;
        case'%':
            postfix.push_back('#');
            postfix.push_back(current);
            break;
        case'(':
            stack.push(current);
            break;
        case')':
             if(infix[i-1]!='%')
                 postfix.push_back('#');//右括号说明前方数字输入完成 标识一下

            char temTop;
            temTop=stack.top();
            while (temTop!='(') {
                stack.pop();
                postfix.push_back(temTop);
                temTop=stack.top();

            }
            stack.pop();
            break;

        default:
            throw "expression has illegality charactor";
            break;
        }
    }

    if(infix[infix.size()-1]!=')')
    {
        if(infix[infix.size()-1].isDigit())
            postfix.push_back('#');
        else if(infix[infix.size()-1]=='%')
        {}
        else
            throw "expression is illegality";

    }
    while(!stack.empty())
    {
        char tempOut=stack.top();
        stack.pop();
        postfix.push_back(tempOut);
    }
    return postfix;

}

double MainWindow::compute(QString s) throw(const char*)//计算后缀表达式
{
    std::stack<double> stack;
    QString str;
    double curr;

    double temNum1;
    double temNum2;
    for(auto i=s.begin();i!=s.end();i++)
    {
        if((*i).isDigit())
        {
            str.push_back(*i);
            continue;
        }
        switch ((*(i)).toLatin1())
        {
        case '+':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2+temNum1);
            break;
        case '-':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2-temNum1);
            break;
        case '*':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2*temNum1);
            break;
        case '/':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2/temNum1);
            break;
        case '^':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(pow(temNum2,temNum1));
            break;
        case '.':
            str.push_back(*i);
            break;
        case'#':
            curr=str.toDouble();
            str.clear();
            stack.push(curr);
            break;
        case'%':
            curr=stack.top();
            stack.pop();
            curr*=0.01;
            stack.push(curr);
            break;
        case's':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::sin(temNum1/180.0*M_PI));
            break;
        case'c':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::cos(temNum1/180.0*M_PI));
            break;
        case't':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::tan(temNum1/180.0*M_PI));
            break;
        case'n':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::log10(temNum1));
            break;
        case'g':
            temNum1=stack.top();
            stack.pop();
            stack.push(std::log(temNum1));
            break;

        default:
            throw "illeaglity";
            break;
        }
    }
    curr=stack.top();
    return curr;
}
//stack3
void MainWindow::handler2_clicked()
    {
        static int ClearLine1=0;
        QPushButton *btn =dynamic_cast<QPushButton* >(sender()); //获取对象
        QString line;
        line.clear();
        if(ui->to_a->isChecked())
        line = ui->cmp_a->text();
        else if(ui->to_b->isChecked())
        line = ui->cmp_b->text();
        QString text = btn->text();     //获取消息

        if(ClearLine1)
        {
            ui->cmp->setText("");
            ui->cmp_a->setText("");
            ui->cmp_b->setText("");
            ui->yueshu->setText("");
            ui->beishu->setText("");
            line.clear();
            ClearLine1=0;
        }if((text>="0"&&text<="9") || (text>="a"&&text<="f"))    //数字
        {
            QString tmp= line.right(1);
            if(tmp.length() && (tmp[0] == "B"||tmp[0] == "D"||tmp[0] == "H"))   //数字前面不能为右括号和后缀
            {
                return;
            }
            line+=text;
        }
        else if( text == "H")   //十六进制
        {
            QString tmp= line.right(1);
            if(tmp.length())//后缀前只能是数字
            {
                if(MatchingBoth(tmp,"0123456789abcdef")== -1)  //没找到数字//或者存在小数点
                {
                    return;
                }
            }
            int pos= LastMatchingBoth(line,"+-*/.()");   //反向查找
             if(pos!= -1 &&line[pos]=='.' )        //数字串中存在小数点
             {
                 return ;
             }
            line+=text;
        }
        else if( text == "D")   //十进制
        {
            QString tmp= line.right(1);
            if(tmp.length())//后缀前只能是数字
            {
                if(MatchingBoth(tmp,"0123456789.")== -1||tmp[0]=='.')  //没找到数字或者前一个是小数点
                {
                    return;
                }
            }
            line+=text;
        }
        else if( text == "B")   //二进制
        {
            QString tmp= line.right(1);
            if(tmp.length())//后缀前只能是数字
            {
                if(MatchingBoth(tmp,"01")== -1 ||MatchingBoth(tmp,".")!=-1)  //没找到数字或者存在小数点
                {
                    return;
                }
            }
            line+=text;
        }


        else if(text=="." )    //小数点
        {
            QString tmp= line.right(1);
            if(tmp.length()) //小数点前面只能是数字
           {
                if(MatchingBoth(tmp,"0123456789")== -1||MatchingBoth(tmp,"abcdef") !=-1)  //没找到数字
                {
                    return;
                }
           }
           else             //小数点前面为空
           {
                    return ;
           }

           int pos= LastMatchingBoth(line,"+-*/.()");   //反向查找
            if(pos!= -1 &&line[pos]=='.' )        //一串数字只能有一个小数点
            {
                return ;
            }
             line+=text;
        }

        else if(text=="退格")       //<-
        {
            if(line.length())
            line.chop(1);
        }

        else if(text=="清除")       //清空
        {
            line.clear();

        }

       else if(text == "输入完成")
        {
            //ClearLine1 = 1;
            QString a,b,yue,bei;

            a = ui->cmp_a->text();
            b = ui->cmp_b->text();
            if(a.isEmpty()||b.isEmpty())//输入框为空
                return;
            QString tmp_a = a.right(1);
            QString tmp_b = b.right(1);
            if(MatchingBoth(tmp_a,"DBH")== -1||MatchingBoth(tmp_b,"DBH")== -1)//没找到后缀
                return;
            //处理数字的进制
            qlonglong a1,b1;
            if(tmp_a == "D")
            {
                a.chop(1);
                a1 = a.toLongLong();
            }
            else if(tmp_a == "B")
            {
                 a.chop(1);
                bool x;
                a1 = a.toLongLong(&x,2);
                a.setNum(a1, 10);
            }
            else if(tmp_a == "H")
            {
               a.chop(1);
               bool x;
               a1 = a.toLongLong(&x,16);
               a.setNum(a1, 10);
            }
            else return;
            if(tmp_b == "D")
            {
                b.chop(1);
                b1 = b.toLongLong();
            }
            else if(tmp_b == "B")
            {
                b.chop(1);
                bool x;
                b1 = b.toLongLong(&x,2);
                b.setNum(b1, 10);
            }
            else if(tmp_b == "H")
            {
               b.chop(1);
               bool x;
               b1 =b.toLongLong(&x,16);
               b.setNum(b1, 10);
            }
            else return;
            //qDebug()<<a1<<" "<<b1;
            qlonglong x;
            QString ans;
            if(a1>b1)
            {
                ui->cmp->setText("大于");
                x = b1;
                for(;x>0;x--)
                {
                    if(a1%x==0 && b1%x==0)
                    {
                        ans.setNum(x,10);
                        break;
                    }
                }
                ui->yueshu->setText(ans);
                x = a1;
                for(;;x++)
                {
                    if(x%b1==0&&x%a1==0)
                    {
                        ans.setNum(x,10);
                        break;
                    }
                }
                ui->beishu->setText(ans);

            }

            else if(a1==b1)
            {
            ui->yueshu->setText(a);
            ui->beishu->setText(a);
            ui->cmp->setText("等于");
            }

            else if(a1<b1)
            {
                ui->cmp->setText("小于");
                x = a1;
                for(;x>0;x--)
                {
                    if(b1%x==0 && a1%x==0)
                    {
                        ans.setNum(x,10);
                        break;
                    }
                }
                ui->yueshu->setText(ans);
                x = b1;
                for(;;x++)
                {
                    if(x%a1==0&&x%b1==0)
                    {
                        ans.setNum(x,10);
                        break;
                    }
                }
                ui->beishu->setText(ans);
            }
            ClearLine1 = 1;
        }
        if(ui->to_a->isChecked())
        ui->cmp_a->setText(line);
        else if(ui->to_b->isChecked())
        ui->cmp_b->setText(line);     //显示结果
    }





void MainWindow::on_zero_2_released()
{
    handler2_clicked();
}
void MainWindow::on_one_2_released()
{
    handler2_clicked();
}
void MainWindow::on_two_2_released()
{
    handler2_clicked();
}

void MainWindow::on_three_2_released()
{
    handler2_clicked();
}

void MainWindow::on_four_2_released()
{
    handler2_clicked();
}

void MainWindow::on_five_2_released()
{
    handler2_clicked();
}

void MainWindow::on_six_2_released()
{
    handler2_clicked();
}

void MainWindow::on_seven_2_released()
{
    handler2_clicked();
}

void MainWindow::on_eight_2_released()
{
    handler2_clicked();
}

void MainWindow::on_nine_2_released()
{
    handler2_clicked();
}

void MainWindow::on_a_2_released()
{
    handler2_clicked();
}

void MainWindow::on_b_2_released()
{
    handler2_clicked();
}

void MainWindow::on_c_2_released()
{
    handler2_clicked();
}

void MainWindow::on_d_2_released()
{
    handler2_clicked();
}

void MainWindow::on_e_2_released()
{
    handler2_clicked();
}

void MainWindow::on_f_2_released()
{
    handler2_clicked();
}

void MainWindow::on_dig_2_released()
{
    handler2_clicked();
}

void MainWindow::on_bina_2_released()
{
    handler2_clicked();
}

void MainWindow::on_hex_2_released()
{
    handler2_clicked();
}

void MainWindow::on_bakcspace_2_released()
{
    handler2_clicked();
}

void MainWindow::on_clear_2_released()
{
    handler2_clicked();
}

void MainWindow::on_deal_released()
{

 handler2_clicked();
}



