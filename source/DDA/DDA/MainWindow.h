//----------------------------------------------------------------------------------------
/**
 * \file       MainWindow.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains MainWindow class.
 *
 *  Qt main window class.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "Playground.h"
#include <qwidget.h>
#include <QMainWindow>

class MainWindow : public QMainWindow
 {
     Q_OBJECT

 private :
	Playground * playground;	///< playground, holds all necessary values for simulation
 public:
	MainWindow(int agentNumber, int obstacleNumber, bool show1Field, bool gpu, QWidget *parent = 0);
	~MainWindow(void);
};

#endif

