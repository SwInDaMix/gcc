/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : menu_main.c
 ***********************************************************************/

#include <stdint.h>
#include <stdio.h>
#include "menus.h"
#include "stdperiph.h"
#include "periphs.h"
#include "eta_flash.h"

#include "bitmaps/icon_question.hbmp"
STGUI_BITMAP_DECLARE(icon_question, ICON_QUESTION);

#include "bitmaps/icon_flash.hbmp"
STGUI_BITMAP_DECLARE(icon_flash, ICON_FLASH);

#include "bitmaps/icon_stop.hbmp"
STGUI_BITMAP_DECLARE(icon_stop, ICON_STOP);

static uint8_t TestChoice;
static uint8_t TestChoice2;
static bool TestCheckbox;

#include "menus/main.hstgui"

sStGUI_Menu const *Menus_GetMain() { return &STGUI_MENU_TEST_MENU; }

void Dialog_Test(sStGUI_Controller *controller) {
    __extension__ sStGUI_DialogValue _get_value(uint16_t code) {
        sStGUI_DialogValue _dialog_value = { };

        if (code == ActionGroup_TestDialog_DialogVarText) _dialog_value.text = "!Test 123!";

        return _dialog_value;
    }
    sStGUI_DialogController _dialog_controller = {
        .get_value = _get_value
    };
    StGUI_ShowDialog(controller, &_dialog_controller, &STGUI_DIALOG_TEST_DIALOG.base.base, &g_stgui_font_large_bold, &g_stgui_font_small, false);
}

void Dialog_FlashSurfaceCheck(sStGUI_Controller *controller) {
    char const *_step_text = "Erasing chip...";
    char _step_details[32]; *_step_details = '\0';
    char _step_details2[32]; *_step_details2 = '\0';
    uint16_t _progress_step = UINT16_MAX;
    uint16_t _progress_total = 0;

    uint32_t _random_seed = Periph_Random();
    uint32_t _page[SPIFLASH_PAGE_SIZE / sizeof(uint32_t)];

    __extension__ bool _procedure(d_stgui_dialog_procedure_idle procedure_idle, uint16_t *action_code) {
        do procedure_idle(false); while (SpiFlash_IsBusy());
        SpiFlash_WriteEnable(); SpiFlash_EraseChip();
        do procedure_idle(false); while (SpiFlash_IsBusy());

        uint32_t _rnd = _random_seed;
        _step_text = "Generating data...";
        for (uint32_t _page_index = 0; _page_index < SPIFLASH_PAGES_PER_CHIP; _page_index++) {
            uint32_t *_page_ptr = _page;
            uint32_t _cnt = SPIFLASH_PAGE_SIZE / sizeof(uint32_t);
            CRC_SetInitRegister(_rnd);
            CRC_ResetDR();
            while (_cnt--) { _rnd = CRC_CalcCRC(_rnd); *_page_ptr++ = _rnd; }

            do procedure_idle(false); while (SpiFlash_IsBusy());
            _progress_step = UINT8_MAX * _page_index / SPIFLASH_PAGES_PER_CHIP;
            _progress_total = _progress_step >> 1;
            snprintf(_step_details, sizeof(_step_details), "Page %lu/%lu", _page_index, (uint32_t)SPIFLASH_PAGES_PER_CHIP);
            procedure_idle(true);
            SpiFlash_WriteEnable(); SpiFlash_WritePage(_page_index * SPIFLASH_PAGE_SIZE, _page, SPIFLASH_PAGE_SIZE);
        }

        _rnd = _random_seed;
        _step_text = "Verifying data...";
        uint32_t _bad_pages = 0;
        for (uint32_t _page_index = 0; _page_index < SPIFLASH_PAGES_PER_CHIP; _page_index++) {
            do procedure_idle(false); while (SpiFlash_IsBusy());
            _progress_step = UINT8_MAX * _page_index / SPIFLASH_PAGES_PER_CHIP;
            _progress_total = (_progress_step >> 1) + ((UINT8_MAX + 1) >> 1);
            snprintf(_step_details, sizeof(_step_details), "Page %lu/%lu", _page_index + 1, (uint32_t)SPIFLASH_PAGES_PER_CHIP);
            snprintf(_step_details2, sizeof(_step_details), "Bad pages: %lu", _bad_pages);
            procedure_idle(true);
            SpiFlash_ReadData(_page_index * SPIFLASH_PAGE_SIZE, _page, SPIFLASH_PAGE_SIZE);

            uint32_t *_page_ptr = _page;
            uint32_t _cnt = SPIFLASH_PAGE_SIZE / sizeof(uint32_t);
            bool _is_bad = false;
            CRC_SetInitRegister(_rnd);
            CRC_ResetDR();
            while (_cnt--) {
                _rnd = CRC_CalcCRC(_rnd);
                if (*_page_ptr++ != _rnd) _is_bad = true;
            }
            if (_is_bad) _bad_pages++;
        }

        _progress_step = _progress_total = UINT8_MAX;
        _step_text = "Verification done";
        procedure_idle(true);
        return false;
    }
    __extension__ sStGUI_DialogValue _get_value(uint16_t code) {
        sStGUI_DialogValue _dialog_value = { };

        if (code == ActionGroup_FlashSurfaceCheck_StepText) _dialog_value.text = _step_text;
        else if (code == ActionGroup_FlashSurfaceCheck_StepDetails) _dialog_value.text = _step_details;
        else if (code == ActionGroup_FlashSurfaceCheck_StepDetails2) _dialog_value.text = _step_details2;
        else if (code == ActionGroup_FlashSurfaceCheck_StepProgress) _dialog_value.progress = _progress_step;
        else if (code == ActionGroup_FlashSurfaceCheck_TotalProgress) _dialog_value.progress = _progress_total;

        return _dialog_value;
    }

    sStGUI_DialogController _dialog_controller = {
        .procedure = _procedure,
        .get_value = _get_value
    };
    StGUI_ShowDialog(controller, &_dialog_controller, &STGUI_DIALOG_FLASH_SURFACE_CHECK.base.base, &g_stgui_font_large_bold, &g_stgui_font_small, DIALOG_FLAGS_FULL_SCREEN);
}
