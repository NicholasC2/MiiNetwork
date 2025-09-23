#pragma once
#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- Colors ---------------- */
#define BACKGROUND_COLOR    C2D_Color32(245, 245, 245, 255)
#define BUTTON_COLOR        C2D_Color32(70, 130, 180, 255)
#define BUTTON_PRESSED      C2D_Color32(60, 110, 160, 255)
#define TEXT_COLOR          C2D_Color32(0, 0, 0, 255)
#define LAYER_COLOR         C2D_Color32(220, 220, 220, 255)
#define TEXTBOX_COLOR       C2D_Color32(255, 255, 255, 255)
#define TEXTBOX_BORDER      C2D_Color32(180, 180, 180, 255)
#define TEXTBOX_ACTIVE      C2D_Color32(200, 230, 255, 255)

/* ---------------- Screen Dimensions ---------------- */
#define TOP_SCREEN_WIDTH    GSP_SCREEN_HEIGHT_TOP
#define TOP_SCREEN_HEIGHT   GSP_SCREEN_WIDTH
#define BOTTOM_SCREEN_WIDTH GSP_SCREEN_HEIGHT_BOTTOM
#define BOTTOM_SCREEN_HEIGHT GSP_SCREEN_WIDTH

/* ---------------- Relative Box ---------------- */
typedef struct { 
    float x, y, w, h; 
} RelativeBox;

/* ---------------- Button ---------------- */
typedef struct Button Button;
typedef void (*ButtonCallback)(Button*, void*);

struct Button {
    RelativeBox box;
    const char *label;
    bool visible;

    C2D_TextBuf buf;
    C2D_Text c2d_text;

    bool pressed;
    bool was_pressed;

    ButtonCallback on_click;
    void *callback_data;
};

/* ---------------- Text ---------------- */
typedef struct {
    float x_percent, y_percent;
    const char *content;
    bool visible;

    C2D_TextBuf buf;
    C2D_Text c2d_text;
} Text;

/* ---------------- TextBox ---------------- */
typedef struct TextBox TextBox;
typedef void (*TextBoxCallback)(TextBox*, void*);

struct TextBox {
    RelativeBox box;
    char *content;
    int max_len;
    bool visible;
    bool active;

    C2D_TextBuf buf;
    C2D_Text c2d_text;

    TextBoxCallback on_edit;
    void *callback_data;
};

/* ---------------- Layer ---------------- */
typedef struct {
    RelativeBox box;
    int z_index;
    bool visible;
} Layer;

/* ---------------- Form ---------------- */
typedef struct {
    RelativeBox box;
    bool visible;

    Button *buttons;
    int button_count;

    Layer *layers;
    int layer_count;

    Text *texts;
    int text_count;

    TextBox *textboxes;
    int textbox_count;
} Form;

/* ---------------- Utils ---------------- */
static inline void to_absolute(RelativeBox box, int sw, int sh, int *x, int *y, int *w, int *h) {
    *x = (int)(box.x * sw);
    *y = (int)(box.y * sh);
    *w = (int)(box.w * sw);
    *h = (int)(box.h * sh);
}

/* ---------------- Form Init ---------------- */
static inline void init_form(Form *f, RelativeBox box) {
    if (!f) return;
    f->box = box;
    f->visible = true;

    f->buttons = NULL;
    f->button_count = 0;

    f->layers = NULL;
    f->layer_count = 0;

    f->texts = NULL;
    f->text_count = 0;

    f->textboxes = NULL;
    f->textbox_count = 0;
}

/* ---------------- Drawing ---------------- */
static inline void draw_rectangle(RelativeBox box, u32 color, int sw, int sh) {
    int x, y, w, h;
    to_absolute(box, sw, sh, &x, &y, &w, &h);
    C2D_DrawRectSolid((float)x, (float)y, 0.0f, (float)w, (float)h, color);
}

static inline void draw_text_obj(const Text *t, int sw, int sh) {
    if (!t || !t->visible || !t->content || !t->buf) return;
    float x = t->x_percent * sw;
    float y = t->y_percent * sh;
    float tw, th;
    C2D_TextGetDimensions(&t->c2d_text, 1.0f, 1.0f, &tw, &th);
    C2D_DrawText(&t->c2d_text, C2D_AlignCenter, x, y - th / 2.0f, 0.5f, 1.0f, 1.0f, TEXT_COLOR);
}

static inline void draw_button(const Button *btn, int sw, int sh) {
    if (!btn || !btn->visible) return;

    u32 color = btn->pressed ? BUTTON_PRESSED : BUTTON_COLOR;
    draw_rectangle(btn->box, color, sw, sh);

    if (btn->label && btn->buf) {
        int x, y, w, h;
        to_absolute(btn->box, sw, sh, &x, &y, &w, &h);
        float cx = x + w / 2.0f;
        float cy = y + h / 2.0f;
        float tw, th;
        C2D_TextGetDimensions(&btn->c2d_text, 0.7f, 0.7f, &tw, &th);
        C2D_DrawText(&btn->c2d_text, C2D_AlignCenter, cx, cy - th / 2.0f, 0.5f, 0.8f, 0.8f, TEXT_COLOR);
    }
}

static inline void draw_layer(const Layer *l, int sw, int sh) {
    if (l && l->visible) draw_rectangle(l->box, LAYER_COLOR, sw, sh);
}

static inline void draw_textbox(const TextBox *tb, int sw, int sh) {
    if (!tb || !tb->visible) return;

    u32 color = tb->active ? TEXTBOX_ACTIVE : TEXTBOX_COLOR;
    draw_rectangle(tb->box, color, sw, sh);

    int x, y, w, h;
    to_absolute(tb->box, sw, sh, &x, &y, &w, &h);
    C2D_DrawRectSolid((float)x, (float)y, 0.5f, (float)w, (float)h, TEXTBOX_BORDER);

    if (tb->content && tb->buf) {
        float cx = x + 5.0f;
        float cy = y + h / 2.0f;
        float tw, th;
        C2D_TextGetDimensions(&tb->c2d_text, 0.7f, 0.7f, &tw, &th);
        C2D_DrawText(&tb->c2d_text, C2D_AlignLeft, cx, cy - th / 2.0f, 0.5f, 0.8f, 0.8f, TEXT_COLOR);
    }
}

static inline void draw_form(const Form *f, int sw, int sh) {
    if (!f || !f->visible) return;

    draw_rectangle(f->box, BACKGROUND_COLOR, sw, sh);

    for (int i = 0; i < f->layer_count; i++) draw_layer(&f->layers[i], sw, sh);
    for (int i = 0; i < f->textbox_count; i++) draw_textbox(&f->textboxes[i], sw, sh);
    for (int i = 0; i < f->button_count; i++) draw_button(&f->buttons[i], sw, sh);
    for (int i = 0; i < f->text_count; i++) draw_text_obj(&f->texts[i], sw, sh);
}

/* ---------------- Add Elements ---------------- */
static inline bool add_button(Form *f, Button b) {
    if (!f) return false;

    if (b.label) {
        b.buf = C2D_TextBufNew(strlen(b.label) + 1);
        C2D_TextParse(&b.c2d_text, b.buf, b.label);
        C2D_TextOptimize(&b.c2d_text);
    } else b.buf = NULL;

    b.pressed = false;
    b.was_pressed = false;

    Button *tmp = realloc(f->buttons, sizeof(Button) * (f->button_count + 1));
    if (!tmp) return false;
    f->buttons = tmp;
    f->buttons[f->button_count++] = b;
    return true;
}

static inline bool add_layer(Form *f, Layer l) {
    if (!f) return false;
    Layer *tmp = realloc(f->layers, sizeof(Layer) * (f->layer_count + 1));
    if (!tmp) return false;
    f->layers = tmp;
    f->layers[f->layer_count++] = l;
    return true;
}

static inline bool add_text(Form *f, Text t) {
    if (!f) return false;

    if (t.content) {
        t.buf = C2D_TextBufNew(strlen(t.content) + 1);
        C2D_TextParse(&t.c2d_text, t.buf, t.content);
        C2D_TextOptimize(&t.c2d_text);
    } else t.buf = NULL;

    Text *tmp = realloc(f->texts, sizeof(Text) * (f->text_count + 1));
    if (!tmp) return false;
    f->texts = tmp;
    f->texts[f->text_count++] = t;
    return true;
}

static inline bool add_textbox(Form *f, TextBox tb) {
    if (!f || !tb.content) return false;

    tb.buf = C2D_TextBufNew(strlen(tb.content) + 1);
    C2D_TextParse(&tb.c2d_text, tb.buf, tb.content);
    C2D_TextOptimize(&tb.c2d_text);
    tb.active = false;

    TextBox *tmp = realloc(f->textboxes, sizeof(TextBox) * (f->textbox_count + 1));
    if (!tmp) return false;
    f->textboxes = tmp;
    f->textboxes[f->textbox_count++] = tb;
    return true;
}

/* ---------------- Free Form ---------------- */
static inline void free_form(Form *f) {
    if (!f) return;

    for (int i = 0; i < f->button_count; i++)
        if (f->buttons[i].buf) C2D_TextBufDelete(f->buttons[i].buf);

    for (int i = 0; i < f->textbox_count; i++)
        if (f->textboxes[i].buf) C2D_TextBufDelete(f->textboxes[i].buf);

    for (int i = 0; i < f->text_count; i++)
        if (f->texts[i].buf) C2D_TextBufDelete(f->texts[i].buf);

    free(f->buttons);
    free(f->textboxes);
    free(f->texts);
    free(f->layers);

    f->buttons = NULL;
    f->textboxes = NULL;
    f->texts = NULL;
    f->layers = NULL;

    f->button_count = f->textbox_count = f->text_count = f->layer_count = 0;
}

/* ---------------- Input Handling ---------------- */
static inline void update_buttons(Form *f, int sw, int sh) {
    if (!f || !f->visible || !f->buttons) return;

    touchPosition t;
    hidTouchRead(&t);
    bool touching = !(t.px == 0 && t.py == 0);

    for (int i = 0; i < f->button_count; i++) {
        Button *b = &f->buttons[i];
        b->was_pressed = b->pressed;
        b->pressed = false;

        if (!b->visible) continue;

        int x, y, w, h;
        to_absolute(b->box, sw, sh, &x, &y, &w, &h);

        if (touching && t.px >= x && t.px <= x + w && t.py >= y && t.py <= y + h)
            b->pressed = true;

        if (!touching && b->was_pressed && b->on_click)
            b->on_click(b, b->callback_data);
    }
}

static inline TextBox* textbox_touched(Form *f, int sw, int sh) {
    if (!f || !f->visible || !f->textboxes) return NULL;

    touchPosition t;
    hidTouchRead(&t);
    bool touching = !(t.px == 0 && t.py == 0);
    if (!touching) return NULL;

    for (int i = 0; i < f->textbox_count; i++) {
        TextBox *tb = &f->textboxes[i];
        if (!tb->visible) continue;

        int x, y, w, h;
        to_absolute(tb->box, sw, sh, &x, &y, &w, &h);

        if (t.px >= x && t.px <= x + w && t.py >= y && t.py <= y + h)
            return tb;
    }
    return NULL;
}
