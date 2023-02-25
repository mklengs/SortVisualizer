#ifndef SORT_H
#define SORT_H

#include <QMainWindow>
#include<QtCore>
#include<QtGui>
#include<QGraphicsScene>
#include<QGraphicsRectItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Sort; }
QT_END_NAMESPACE

class Sort : public QMainWindow
{
    Q_OBJECT

public:
    Sort(QWidget *parent = nullptr);
    ~Sort();
    void randomizeVector();

private:
    Ui::Sort *ui;
    QGraphicsScene *scene;
    int vecCount;
    std::vector<QGraphicsRectItem *> columns;
    int sortSelect;
    QThread thread;
    double delay;
    int comparisons;
    int swaps;
private slots:
    void on_Mixbut_clicked();
    void on_Sortbut_clicked();
    void switchItems(int,int);
    void restoreButtons(void);
    void replaceItems(int,int);
    void on_SortSelect_currentIndexChanged(int index);
    void highlight(int index,Qt::GlobalColor);
    void unhighlight(int index);
    void on_Stopbut_clicked();
    void on_columnNum_valueChanged(int arg1);
    void on_delay_valueChanged(int arg1);
    void inc_comparisons();
    void inc_swaps();

signals:
    void performAlgo(int,std::vector<QGraphicsRectItem *>,int);
};
#endif // SORT_H
