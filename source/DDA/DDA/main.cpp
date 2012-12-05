#include <QApplication>

#include "MainWindow.h"
#include "PotentialField.h"
#include "PotentialFieldWorker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	int agentNumber = 10;
	int obstacleNumber = 100;
	bool show1Field = 0;
	bool gpu = true;
	FILE * fr;
	int temp;
	for(int loop1 = 1; loop1 + 1 < argc; loop1 += 2)
	{
		switch(argv[loop1][1])
		{
			case 'a' : agentNumber = atoi(argv[loop1 + 1]); break;
			case 'o' : obstacleNumber = atoi(argv[loop1 + 1]); break;
			case 'f' : show1Field = atoi(argv[loop1 + 1]) > 0; break;
			case 'g' : gpu = atoi(argv[loop1 + 1]) > 0; break;
			case 'i' :
				fr = fopen(argv[loop1 + 1], "r");
				if(fr != NULL)
				{
					fscanf(fr, "#agents %d\n", &agentNumber);
					fscanf(fr, "#obstacles %d\n", &obstacleNumber);
					fscanf(fr, "#gpu %d\n", &temp);
					gpu = temp > 0;
				}
				break;
		}
	}

	fr = fopen("setup.ini", "r");
	if(fr != NULL)
	{
		int temp;
		fscanf(fr, "FIELD_WIDTH = %d\n", &temp);
		PotentialField::SetFieldWidth(temp);
		fscanf(fr, "TILE_WIDTH = %d\n", &temp);
		PotentialField::SetTileWidth(temp);
		fscanf(fr, "THREAD_BLOCK_HEIGHT = %d\n", &temp);
		PotentialFieldWorker::SetThreadBlockHeight(temp);
		fscanf(fr, "THREAD_BLOCK_WIDTH = %d\n", &temp);
		PotentialFieldWorker::SetThreadBlockWidth(temp);
	}

    MainWindow window(agentNumber, obstacleNumber, show1Field, gpu);
    window.show();

    return app.exec();
}