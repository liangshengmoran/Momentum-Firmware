#include "../storage_settings.h"

static void storage_settings_scene_sd_info_dialog_callback(DialogExResult result, void* context) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void storage_settings_scene_sd_info_on_enter(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    SDInfo sd_info;
    FS_Error sd_status = storage_sd_info(app->fs_api, &sd_info);

    scene_manager_set_scene_state(app->scene_manager, StorageSettingsSDInfo, sd_status);

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(dialog_ex, storage_settings_scene_sd_info_dialog_callback);

    if(sd_status != FSE_OK) {
        dialog_ex_set_icon(dialog_ex, 83, 22, &I_WarningDolphinFlip_45x42);
        dialog_ex_set_header(dialog_ex, "SD卡未安装", 64, 3, AlignCenter, AlignTop);
        dialog_ex_set_text(
            dialog_ex, "请尝试重新插入\n或格式化 SD\n卡。", 3, 19, AlignLeft, AlignTop);
        dialog_ex_set_center_button_text(dialog_ex, "确定");
    } else {
        furi_string_printf(
            app->text_string,
            "标签: %s\n类型: %s\n",
            sd_info.label,
            sd_api_get_fs_type_text(sd_info.fs_type));

        if(sd_info.kb_total < 1024) {
            furi_string_cat_printf(app->text_string, "总计: %lu KiB\n", sd_info.kb_total);
        } else if(sd_info.kb_total < 1024 * 1024) {
            furi_string_cat_printf(
                app->text_string, "总计: %.2f MiB\n", (double)sd_info.kb_total / 1024);
        } else {
            furi_string_cat_printf(
                app->text_string, "总计: %.2f GiB\n", (double)sd_info.kb_total / (1024 * 1024));
        }

        if(sd_info.kb_free < 1024) {
            furi_string_cat_printf(app->text_string, "剩余: %lu KiB", sd_info.kb_free);
        } else if(sd_info.kb_free < 1024 * 1024) {
            furi_string_cat_printf(
                app->text_string, "剩余: %.2f MiB", (double)sd_info.kb_free / 1024);
        } else {
            furi_string_cat_printf(
                app->text_string, "剩余: %.2f GiB", (double)sd_info.kb_free / (1024 * 1024));
        }
        furi_string_cat_printf(
            app->text_string,
            "  (%.2f%%)\n", (double)sd_info.kb_free * 100 / sd_info.kb_total);

        furi_string_cat_printf(
            app->text_string,
            "%02X%s %s v%i.%i\nSN:%04lX %02i/%i",
            sd_info.manufacturer_id,
            sd_info.oem_id,
            sd_info.product_name,
            sd_info.product_revision_major,
            sd_info.product_revision_minor,
            sd_info.product_serial_number,
            sd_info.manufacturing_month,
            sd_info.manufacturing_year);

        dialog_ex_set_text(
            dialog_ex, furi_string_get_cstr(app->text_string), 4, 1, AlignLeft, AlignTop);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewDialogEx);
}

bool storage_settings_scene_sd_info_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    FS_Error sd_status = scene_manager_get_scene_state(app->scene_manager, StorageSettingsSDInfo);

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultLeft:
        case DialogExResultCenter:
            consumed = scene_manager_previous_scene(app->scene_manager);
            break;
        case DialogExResultRight:
            scene_manager_next_scene(app->scene_manager, StorageSettingsUnmounted);
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack && sd_status != FSE_OK) {
        consumed = true;
    }

    return consumed;
}

void storage_settings_scene_sd_info_on_exit(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);

    furi_string_reset(app->text_string);
}
