#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "Playground.h"
#include <qwidget.h>
#include <QMainWindow>

class MainWindow : public QMainWindow
 {
     Q_OBJECT

 private :
	Playground * playground;
 public:
	MainWindow(QWidget *parent = 0);
	~MainWindow(void);
};

#endif

