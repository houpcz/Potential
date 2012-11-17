#include "MainWindow.h"
#include "Playground.h"

MainWindow::MainWindow(QWidget *parent)
     : QMainWindow(parent)
{
	setWindowTitle(tr("Motion planning using potencial fields"));

	playground = new Playground(this, 300, 120);

	setCentralWidget(playground);
}

MainWindow::~MainWindow(void)
{
	if(playground != NULL)
		delete playground;
}
