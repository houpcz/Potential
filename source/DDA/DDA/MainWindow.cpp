#include "MainWindow.h"
#include "Playground.h"

MainWindow::MainWindow(QWidget *parent)
     : QMainWindow(parent)
{
	setWindowTitle(tr("Motion planning using potencial fields"));

	playground = new Playground(this);

	setCentralWidget(playground);
}


MainWindow::~MainWindow(void)
{
	delete playground;
}
