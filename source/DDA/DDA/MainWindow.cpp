#include "MainWindow.h"
#include "Playground.h"
#include "PotentialField.h"

MainWindow::MainWindow(int agentNumber, int obstacleNumber, bool show1Field, bool gpu, QWidget *parent)
     : QMainWindow(parent)
{
	setWindowTitle(tr("Motion planning using potencial fields"));

	PotentialField::SetShow1Field(show1Field);
	playground = new Playground(this, agentNumber, obstacleNumber, gpu);

	setCentralWidget(playground);
}

MainWindow::~MainWindow(void)
{
	if(playground != NULL)
		delete playground;
}
