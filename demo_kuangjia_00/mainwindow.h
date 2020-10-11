#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStack>
#include <QQueue>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class QCalculatorDec
{
private:
    QString change(QString num);
    QQueue<QString> Split(const QString& exp);          //分离前缀
    QQueue<QString> Transfer(QQueue<QString>& exp);     //将中缀队列转换为后缀队列
    QString Calculate(QQueue<QString>& exp);            //将后缀队列计算出结果

    QString Calculate(QString& l,QString& op,QString& r );
    QString ValidNum(QString str);

public:
    QCalculatorDec();
    QString Result(const QString& exp);
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QCalculatorDec  mDec;
    void abortOperation();
    void connectSlots();//连接信号和槽

    bool waitForOperand;//标记是否等待一个操作数
    void setShortcutKeys();
    QString inToPost(QString infix) throw(const char*);//转换为后缀表达式
    double compute(QString s) throw(const char*);//计算后缀表达式的结果
    QString error;
    QString log;


    Ui::MainWindow *ui;

public:
    explicit MainWindow(QWidget *parent = 0);

    int  MatchingBoth(QString &str1,const char *str2);          //匹配str1和str2,判断str1是否有str2的字符
        int  LastMatchingBoth(QString &str1,const char *str2);      //反向匹配str1和str2
         void handler_clicked();         //处理按键消息
         void handler2_clicked();
    ~MainWindow();




private slots:

         void on_clearBtn_clicked();

         void on_clearAllBtn_clicked();

         void on_equalBtn_clicked();

         void on_signBtn_clicked();

         void on_pointBtn_clicked();

         void digitClicked();

         void operatorClicked();
     void specialOperatorClicked();





    void on_yunsuan_released();

    void on_bijiao_released();


    void on_one_released();

    void on_two_released();

    void on_three_released();

    void on_four_released();

    void on_five_released();

    void on_six_released();

    void on_seven_released();

    void on_eight_released();

    void on_nine_released();
    void on_a_released();
    void on_b_released();
    void on_c_released();
    void on_d_released();
    void on_e_released();
    void on_f_released();
    void on_bina_released();
    void on_dig_released();
    void on_hex_released();


    void on_plus_released();

    void on_minus_released();

    void on_mult_released();

    void on_div_released();

    void on_left_released();

    void on_reght_released();

    void on_epual_released();

    void on_point_released();

    void on_zero_released();

    void on_bakcspace_released();

    void on_clear_released();

    void on_radioButton_released();




    void on_zero_2_released();
    void on_one_2_released();
    void on_two_2_released();
    void on_three_2_released();
    void on_four_2_released();
    void on_five_2_released();
    void on_six_2_released();
    void on_seven_2_released();
    void on_eight_2_released();
    void on_nine_2_released();
    void on_a_2_released();
    void on_b_2_released();
    void on_c_2_released();
    void on_d_2_released();
    void on_e_2_released();
    void on_f_2_released();
    void on_dig_2_released();
    void on_bina_2_released();
    void on_hex_2_released();
    void on_bakcspace_2_released();
    void on_clear_2_released();
    void on_deal_released();
};

#endif // MAINWINDOW_H
