#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <string.h>
#include "mn-utils/form.c"

#define NAME_MAX_LEN 36
char user_name[NAME_MAX_LEN] = "";

/* ---------------- Keyboard Input ---------------- */
void open_keyboard() {
    SwkbdState swkbd;
    char buf[NAME_MAX_LEN];
    strncpy(buf, user_name, NAME_MAX_LEN);

    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 3, -1);
    swkbdSetInitialText(&swkbd, buf);
    swkbdSetHintText(&swkbd, "Enter your name");

    SwkbdButton button = swkbdInputText(&swkbd, buf, sizeof(buf));
    if (button != SWKBD_BUTTON_NONE) {
        strncpy(user_name, buf, NAME_MAX_LEN - 1);
        user_name[NAME_MAX_LEN - 1] = '\0';
    }
}

/* ---------------- Mii Selector Callback ---------------- */
void select_mii_callback(Button *btn, void *userdata) {
    static MiiSelectorConf msConf;
    static MiiSelectorReturn msRet;
    static char miiname[36];
    static char miiauthor[30];

    miiSelectorInit(&msConf);
    miiSelectorLaunch(&msConf, &msRet);

    if (miiSelectorChecksumIsValid(&msRet) && !msRet.no_mii_selected) {
        miiSelectorReturnGetName(&msRet, miiname, sizeof(miiname));
        miiSelectorReturnGetAuthor(&msRet, miiauthor, sizeof(miiauthor));

        strncpy(user_name, miiname, NAME_MAX_LEN - 1);
        user_name[NAME_MAX_LEN - 1] = '\0';
        printf("Mii selected: %s by %s\n", miiname, miiauthor);
    } else {
        printf("No Mii selected or invalid checksum.\n");
    }
}

/* ---------------- Main ---------------- */
int main() {
    /* ---------------- Init ---------------- */
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    /* ---------------- Create Form ---------------- */
    Form form;
    init_form(&form, (RelativeBox){0.0f, 0.0f, 1.0f, 1.0f});

    // Title
    add_text(&form, (Text){0.5f, 0.1f, "Signup", true});

    // Name label
    add_text(&form, (Text){0.1f, 0.25f, "Name:", true});

    // Name input box (layer)
    RelativeBox name_box = {0.3f, 0.22f, 0.65f, 0.08f};
    add_layer(&form, (Layer){name_box, 0, true});

    // Name text
    Text name_text = {0.32f, 0.24f, user_name, true};
    add_text(&form, name_text);

    // Select Mii button
    add_button(&form, (Button){
        {0.35f, 0.5f, 0.3f, 0.12f},
        "Select Mii",
        true,
        NULL,
        {0},
        false,
        false,
        select_mii_callback,
        NULL
    });

    /* ---------------- Main Loop ---------------- */
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        // Update buttons
        update_buttons(&form, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT);

        // Check for textbox touch
        TextBox *tb = textbox_touched(&form, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT);
        if (tb) open_keyboard();

        // Update displayed name
        for (int i = 0; i < form.text_count; i++) {
            if (strcmp(form.texts[i].content, name_text.content) == 0) {
                form.texts[i].content = user_name;
            }
        }

        // Render
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bottom, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
        C2D_SceneBegin(bottom);

        draw_form(&form, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT);

        C3D_FrameEnd(0);
    }

    /* ---------------- Cleanup ---------------- */
    free_form(&form);
    C2D_Fini();
    C3D_Fini();
    gfxExit();

    return 0;
}
