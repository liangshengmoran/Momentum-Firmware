#include "../ibutton_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexWriteId,
    SubmenuIndexWriteCopy,
    SubmenuIndexEdit,
    SubmenuIndexRename,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
};

void ibutton_scene_saved_key_menu_on_enter(void* context) {
    iButton* ibutton = context;
    Submenu* submenu = ibutton->submenu;

    const uint32_t features = ibutton_protocols_get_features(
        ibutton->protocols, ibutton_key_get_protocol_id(ibutton->key));

    submenu_add_item(submenu, "仿真", SubmenuIndexEmulate, ibutton_submenu_callback, ibutton);

    if(features & iButtonProtocolFeatureWriteId) {
        submenu_add_item(
            submenu, "写入 ID", SubmenuIndexWriteId, ibutton_submenu_callback, ibutton);
    }

    if(features & iButtonProtocolFeatureWriteCopy) {
        submenu_add_item(
            submenu,
            "同类型完全写入",
            SubmenuIndexWriteCopy,
            ibutton_submenu_callback,
            ibutton);
    }

    submenu_add_item(submenu, "编辑", SubmenuIndexEdit, ibutton_submenu_callback, ibutton);
    submenu_add_item(submenu, "重命名", SubmenuIndexRename, ibutton_submenu_callback, ibutton);
    submenu_add_item(submenu, "删除", SubmenuIndexDelete, ibutton_submenu_callback, ibutton);
    submenu_add_item(submenu, "信息", SubmenuIndexInfo, ibutton_submenu_callback, ibutton);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(ibutton->scene_manager, iButtonSceneSavedKeyMenu));
    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewSubmenu);
}

bool ibutton_scene_saved_key_menu_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    SceneManager* scene_manager = ibutton->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(scene_manager, iButtonSceneSavedKeyMenu, event.event);
        consumed = true;
        if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(scene_manager, iButtonSceneEmulate);
            dolphin_deed(DolphinDeedIbuttonEmulate);
        } else if(event.event == SubmenuIndexWriteId) {
            ibutton->write_mode = iButtonWriteModeId;
            scene_manager_next_scene(scene_manager, iButtonSceneWrite);
        } else if(event.event == SubmenuIndexWriteCopy) {
            ibutton->write_mode = iButtonWriteModeCopy;
            scene_manager_next_scene(scene_manager, iButtonSceneWrite);
        } else if(event.event == SubmenuIndexEdit) {
            scene_manager_next_scene(scene_manager, iButtonSceneAddValue);
        } else if(event.event == SubmenuIndexRename) {
            scene_manager_next_scene(scene_manager, iButtonSceneSaveName);
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(scene_manager, iButtonSceneDeleteConfirm);
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(scene_manager, iButtonSceneInfo);
        }

    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(
            scene_manager, iButtonSceneSavedKeyMenu, SubmenuIndexEmulate);
        // Event is not consumed
    }

    return consumed;
}

void ibutton_scene_saved_key_menu_on_exit(void* context) {
    iButton* ibutton = context;
    submenu_reset(ibutton->submenu);
}
