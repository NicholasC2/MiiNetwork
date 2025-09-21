#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("Mii selector demo.\n");
	printf("Press A to bring up Mii selector with default settings.\n");
	printf("Press B to bring up Mii selector with custom settings.\n");
	printf("Press START to exit.\n");

	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		bool didit = false;
		static MiiSelectorConf msConf;
		static MiiSelectorReturn msRet;
		static char miiname[36];
		static char miiauthor[30];

		if (kDown & KEY_A)
		{
			didit = true;
			miiSelectorInit(&msConf);
			miiSelectorLaunch(&msConf, &msRet);
		}

		if (kDown & KEY_B)
		{
			didit = true;
			miiSelectorInit(&msConf);
			miiSelectorSetTitle(&msConf, "This is a custom title!");
			miiSelectorSetOptions(&msConf, MIISELECTOR_CANCEL|MIISELECTOR_GUESTS|MIISELECTOR_TOP|MIISELECTOR_GUESTSTART);
			miiSelectorSetInitialIndex(&msConf, 1);
			miiSelectorBlacklistUserMii(&msConf, 0);
			miiSelectorLaunch(&msConf, &msRet);
		}

		if (didit)
		{
			if (miiSelectorChecksumIsValid(&msRet))
			{
				if (!msRet.no_mii_selected)
				{
					printf("A Mii was selected.\n");
					miiSelectorReturnGetName(&msRet, miiname, sizeof(miiname));
					miiSelectorReturnGetAuthor(&msRet, miiauthor, sizeof(miiauthor));
					printf("Name: %s\n", miiname);
					printf("Author: %s\n", miiauthor);
					printf("Birthday: Month-%d/Day-%d\n", msRet.mii.mii_details.bday_month, msRet.mii.mii_details.bday_day);
					printf("Sex: %d\n", msRet.mii.mii_details.sex);
					printf("Color: %d\n", msRet.mii.mii_details.shirt_color);
					printf("Favorite: %d\n", msRet.mii.mii_details.favorite);
					printf("Eyebrow: %d\n", msRet.mii.eyebrow_details.style);
					printf("Eyebrow color: %d\n", msRet.mii.eyebrow_details.color);
					printf("Nose: %d\n", msRet.mii.nose_details.style);
				} else
					printf("No Mii was selected.\n");
			} else
				printf("Return checksum invalid.\n");
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
