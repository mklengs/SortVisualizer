#include "sort.h"
#include "./ui_sort.h"
#include<QBrush>
#include <QGraphicsRectItem>
#include<iostream>
#include<stdlib.h>
#include"worker.h"

//

Sort::Sort(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Sort)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    //scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->graphicsView->setScene(scene);

    vecCount = 50;
    ui->columnNum->setRange(5,500);
    ui->columnNum->setValue(vecCount);

    delay = 20;
    ui->delay->setRange(0,100);
    ui->delay->setValue(delay);

    ui->Sortbut->setEnabled(false);
    ui->Stopbut->setEnabled(false);

    sortSelect = 0;
    comparisons = 0;
    swaps = 0;

    WorkerThread *worker = new WorkerThread;
    worker->moveToThread(&thread);
    //connect(&thread, &QThread::finished, worker, &QObject::deleteLater); //Dont use so that we can start/stop thread
    connect(this, &Sort::performAlgo, worker, &WorkerThread::doAlgo);
    connect(worker,&WorkerThread::swap,this,&Sort::switchItems);
    connect(worker,&WorkerThread::done,this,&Sort::restoreButtons);
    connect(worker,&WorkerThread::replace,this,&Sort::replaceItems);
    connect(worker,&WorkerThread::highlight,this,&Sort::highlight);
    connect(worker,&WorkerThread::unhighlight,this,&Sort::unhighlight);
    connect(worker,&WorkerThread::inc_comparisons,this,&Sort::inc_comparisons);
    connect(worker,&WorkerThread::inc_swaps,this,&Sort::inc_swaps);
    thread.start();

}

Sort::~Sort()
{
    thread.quit();
    thread.wait();
    delete ui;
}

void Sort::randomizeVector()
{
    columns.clear();
    scene->clear();
    QBrush brush(Qt::cyan);
    QPen pen(Qt::black);

    auto colwidth = ui->graphicsView->size().width() / vecCount;
    int max = ui->graphicsView->size().height() - 5;
    for(int i = 0;i<vecCount;i++){
        int height = std::rand() % max + 5;
        auto r = new QGraphicsRectItem(i*colwidth,0,colwidth,height);
        r->setBrush(brush);
        r->setPen(pen);
        columns.push_back(r);
        scene->addItem(r);
    }
}

void Sort::on_Mixbut_clicked(){
    swaps = 0;
    comparisons = 0;
    QString s = "Swaps: " + QString::number(swaps);
    QString c = "Comparisons " + QString::number(comparisons);
    ui->Swaps->setText(s);
    ui->Comparisons->setText(c);

    randomizeVector();
    ui->Sortbut->setEnabled(true);
}

void Sort::on_Sortbut_clicked()
{
    ui->Sortbut->setEnabled(false);
    ui->Mixbut->setEnabled(false);
    ui->Stopbut->setEnabled(true);

    swaps = 0;
    comparisons = 0;
    QString s = "Swaps: " + QString::number(swaps);
    QString c = "Comparisons " + QString::number(comparisons);
    ui->Swaps->setText(s);
    ui->Comparisons->setText(c);

    if(thread.isFinished()){
        thread.start();
    }

    emit performAlgo(sortSelect,columns,delay);
}

void Sort::restoreButtons(){
    ui->Sortbut->setEnabled(true);
    ui->Mixbut->setEnabled(true);
    ui->Stopbut->setEnabled(false);
}


void Sort::switchItems(int i,int j){
    auto iRect = columns[i]->rect();
    auto minRect = columns[j]->rect();
    auto temp = iRect.left();

    iRect.moveLeft(minRect.left());
    minRect.moveLeft(temp);

    columns[i]->setRect(iRect);
    columns[j]->setRect(minRect);

    auto temp_p = columns[i];
    columns[i] = columns[j];
    columns[j] = temp_p;
}

void Sort::on_SortSelect_currentIndexChanged(int index)
{
    sortSelect = index;
}

void Sort::replaceItems(int i,int j){
    auto jRect = columns[j]->rect();
    auto iRect = columns[i]->rect();

    jRect.moveLeft(iRect.left());
    columns[j]->setRect(jRect);

    columns[i] = columns[j];
}

void Sort::on_Stopbut_clicked()
{
    ui->Stopbut->setEnabled(false);

    thread.quit(); //Issue thread.quit because if it is alive but not executing a sort function, it will never check request interruption
    if(thread.isRunning()){
        thread.requestInterruption();
        thread.wait();
    }
    restoreButtons();
}

void Sort::on_columnNum_valueChanged(int arg1)
{
    vecCount = arg1;
}

void Sort::on_delay_valueChanged(int arg1)
{
    delay = arg1;
}

void Sort::highlight(int index,Qt::GlobalColor c){
    if(index < 0 || index >= columns.size())return;
    QBrush b(c);
    columns[index]->setBrush(b);
}

void Sort::unhighlight(int index){
    if(index < 0 || index >= columns.size())return;
    QBrush b(Qt::cyan);
    columns[index]->setBrush(b);
}

void Sort::inc_comparisons(){
    comparisons++;
    QString c = "Comparisons " + QString::number(comparisons);
    ui->Comparisons->setText(c);
}

void Sort::inc_swaps(){
    swaps++;
    QString s = "Swaps: " + QString::number(swaps);
    ui->Swaps->setText(s);
}
