#include"worker.h"
#include<QBrush>
#include <QGraphicsRectItem>
#include<iostream>
#include<stdlib.h>

void WorkerThread::doAlgo(int sortSelect,std::vector<QGraphicsRectItem *> c,int d){
    columns = c;
    delay = d;

    std::cout<<"Recieved do algo signal: "<<sortSelect<<std::endl;
    for(int i = 0; i<columns.size();i++){
        std::cout<<columns[i]->rect().height()<<" ";
    }
    std::cout<<std::endl;

    switch(sortSelect){
    case 0:selectionsort();break;
    case 1:bubblesort();break;
    case 2:insertionsort();break;
    //case 3:mergesort(columns,0,columns.size() - 1);break;
    case 3:quicksort(0,columns.size() - 1);break;
    default:break;
    }
    emit done();
}

void WorkerThread::selectionsort(){
    for(int i = 0; i < columns.size();i++){
        emit highlight(i,Qt::red);

        int min = i;
        for(int j = i+1;j<columns.size();j++){

            if(j-1 > i && j-1 != min)emit highlight(j-1,Qt::cyan);

            emit highlight(j,Qt::red);

            emit inc_comparisons();
            if(min == -1 || columns[j]->rect().height() < columns[min]->rect().height()){
                if(min != i)emit highlight(min,Qt::cyan);
                emit highlight(j,Qt::red);
                min = j;
            }
            Sleep(delay);
        }
        emit highlight(columns.size() - 1,Qt::cyan); //unhighlight last in column

        //if(min == i)continue; //Dont use for proper highlighting
        if(min == -1)std::cout<<"error min == -1"<<std::endl;

        //swap
        emit swap(i,min);
        if(min != i)emit inc_swaps(); //Technically we are still swapping we just wont count it :) since its in place already

        auto temp_p = columns[i];
        columns[i] = columns[min];
        columns[min] = temp_p;

        emit highlight(i,Qt::green);

        if(i != min)emit highlight(min,Qt::cyan);

        if(QThread::currentThread()->isInterruptionRequested()){
            QThread::currentThread()->quit();
            return;
        }
    }
    emit highlight(columns.size() - 1,Qt::green);
}

void WorkerThread::bubblesort(){
    bool sorted = false;
    int iterations = 0;
    while(!sorted){
        sorted = true;
        for(int i = 0;i<columns.size() - 1 - iterations;i++){
            emit highlight(i,Qt::red);
            emit highlight(i-1,Qt::cyan);

            emit inc_comparisons();
            if(columns[i]->rect().height() > columns[i+1]->rect().height()){
                sorted = false;

                emit swap(i,i+1);
                emit inc_swaps();

                auto temp_p = columns[i];
                columns[i] = columns[i+1];
                columns[i+1] = temp_p;

                if(QThread::currentThread()->isInterruptionRequested()){
                    QThread::currentThread()->quit();
                    return;
                }

                Sleep(delay);
            }
        }
        iterations++;
        emit highlight(columns.size() - iterations,Qt::green);
    }
    for(int i = 0;i<columns.size()-iterations;i++){
        emit highlight(i,Qt::green);
    }
}

void WorkerThread::insertionsort(){
    int length = columns.size();
    for(int step = 1;step<length;step++){
        int j = step - 1;

        emit highlight(step,Qt::red);
        emit highlight(step - 1,Qt::cyan);

        while(j >= 0 && columns[j+1]->rect().height() < columns[j]->rect().height()){
            auto temp = columns[j];
            columns[j] = columns[j+1];
            columns[j+1] = temp;

            emit inc_comparisons();
            emit inc_swaps();
            emit swap(j+1,j);
            emit highlight(j+1,Qt::cyan);
            emit highlight(j,Qt::red);
            --j;

            if(QThread::currentThread()->isInterruptionRequested()){
                QThread::currentThread()->quit();
                return;
            }
            Sleep(delay);
        }
        emit inc_comparisons();
        emit highlight(j+1,Qt::cyan);
    }
    for(int i = 0;i<columns.size();i++){
        emit highlight(i,Qt::green);
    }
}

//Maybe merge sort should be done bc in place merge sort is more complex (time wise)
//In-place merge sort is N^2 logn
/*
void WorkerThread::mergesort(std::vector<QGraphicsRectItem *>columns,int start,int end){
    if(end - start <= 0)return;

    int mid = (start + end) / 2;

    mergesort(columns,start,mid);
    mergesort(columns,mid+1,end);

    int left = start;
    int right = mid + 1;

    while(left <= mid && right <= end){
        if(columns[left]->rect().height() <= columns[right]->rect().height()){
            left++;
        }else{
            auto temp = columns[left];
            columns[left] = columns[right];
            columns[right] = temp;
            left++;
            right++;
        }
    }
    for(int i = start; i <= end;i++){
        std::cout<<columns[i]->rect().height()<<" ";
    }
    std::cout<<std::endl;
}*/

void WorkerThread::quicksort(int low,int high){
    if(QThread::currentThread()->isInterruptionRequested()){
        QThread::currentThread()->quit();
        return;
    }
    if(low < high){
        int pi = partition(low,high);
        emit highlight(pi,Qt::green);
        quicksort(low,pi - 1);
        quicksort(pi + 1,high);
    }else{
        emit highlight(low,Qt::green);
        emit highlight(high,Qt::green);
    }
}

int WorkerThread::partition(int low,int high){
    if(QThread::currentThread()->isInterruptionRequested()){
        QThread::currentThread()->quit();
        return -1;
    }

    int pivot = columns[high]->rect().height();
    int pIndex = low;

    emit highlight(high,Qt::red);
    for(int i = low;i<high;i++){
        if(i-1 >= low)emit highlight(i-1,Qt::cyan);
        if(pIndex - 1 >= low)emit highlight(pIndex - 1,Qt::cyan);
        emit highlight(pIndex,Qt::red);
        emit highlight(i,Qt::red);

        emit inc_comparisons();
        if(columns[i]->rect().height() <= pivot){
            emit inc_swaps();

            auto temp = columns[i];
            columns[i] = columns[pIndex];
            columns[pIndex] = temp;

            emit swap(i,pIndex);

            pIndex++;

            Sleep(delay);
        }
    }
    emit highlight(high,Qt::cyan);
    emit highlight(high-1,Qt::cyan);
    if(pIndex - 1 >= low)emit highlight(pIndex - 1,Qt::cyan);

    auto temp = columns[pIndex];
    columns[pIndex] = columns[high];
    columns[high] = temp;

    emit swap(pIndex,high);
    emit inc_swaps();

    return pIndex;
}
