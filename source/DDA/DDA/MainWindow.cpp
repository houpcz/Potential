#include "MainWindow.h"
#include "Playground.h"

MainWindow::MainWindow(QWidget *parent)
     : QMainWindow(parent)
{
	setWindowTitle(tr("Motion planning using potencial fields"));

	playground = new Playground(this, 50, 10);

	setCentralWidget(playground);
}

MainWindow::~MainWindow(void)
{
	if(playground != NULL)
		delete playground;
}
