#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("Hello, World!\n");

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		circlePosition pos;

		hidCircleRead(&pos);

		printf("\x1b[20;0HCircle Pad: x=%04d y=%04d\n", pos.dx, pos.dy);
	}

	gfxExit();
	return 0;
}
