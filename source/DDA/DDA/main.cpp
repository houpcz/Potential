#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	int agentNumber = 3;
	int obstacleNumber = 50;
	bool show1Field = 0;
	bool gpu = true;

	for(int loop1 = 1; loop1 + 1 < argc; loop1 += 2)
	{
		switch(argv[loop1][1])
		{
			case 'a' : agentNumber = atoi(argv[loop1 + 1]); break;
			case 'o' : obstacleNumber = atoi(argv[loop1 + 1]); break;
			case 'f' : show1Field = atoi(argv[loop1 + 1]) > 0; break;
			case 'g' : gpu = atoi(argv[loop1 + 1]) > 0; break;
		}
	}

    MainWindow window(agentNumber, obstacleNumber, show1Field, gpu);
    window.show();

    return app.exec();
}