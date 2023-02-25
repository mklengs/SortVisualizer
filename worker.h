#ifndef WORKER_H
#define WORKER_H

#include <QMainWindow>
#include<QtCore>
#include<QtGui>
#include<QGraphicsScene>
#include<QGraphicsRectItem>

class WorkerThread : public QObject
{
    Q_OBJECT

public slots:
    void doAlgo(int,std::vector<QGraphicsRectItem *>,int d);

signals:
    void swap(int,int);
    void done();
    void replace(int,int);
    void highlight(int index,Qt::GlobalColor);
    void unhighlight(int index);
    void inc_comparisons();
    void inc_swaps();
private:
    int delay;
    std::vector<QGraphicsRectItem *> columns;
    void selectionsort();
    void bubblesort();
    void insertionsort();
    //void mergesort(std::vector<QGraphicsRectItem *>columns,int,int);
    void quicksort(int,int);
    int partition(int,int);
};

#endif // WORKER_H
