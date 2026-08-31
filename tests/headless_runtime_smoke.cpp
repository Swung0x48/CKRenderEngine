#include <cstdio>
#include <cstring>

#include "CKContext.h"
#include "CKGlobals.h"
#include "CKPluginManager.h"
#include "CKRenderManager.h"
#include "CKScene.h"
#include "RCK3dEntity.h"
#include "RCKMesh.h"

CKPluginInfo *CKGet_CK2_3D_PluginInfo(int index);

namespace {

int Fail(const char *message)
{
    std::fprintf(stderr, "CKRenderEngine headless smoke failed: %s\n", message);
    return 1;
}

} // namespace

int main()
{
    if (CKStartUp() != CK_OK)
        return Fail("CKStartUp returned an error");

    CKPluginInfo *plugin = CKGet_CK2_3D_PluginInfo(0);
    if (!plugin || !plugin->m_InitInstanceFct || !plugin->m_ExitInstanceFct) {
        CKShutdown();
        return Fail("headless render-engine plugin metadata is incomplete");
    }

    CKContext *context = nullptr;
    if (CKCreateContext(&context, nullptr, 0, 0) != CK_OK || !context) {
        CKShutdown();
        return Fail("could not create a windowless CKContext");
    }

    if (plugin->m_InitInstanceFct(context) != CK_OK) {
        CKCloseContext(context);
        CKShutdown();
        return Fail("render manager initialization failed");
    }

    CKRenderManager *renderManager = context->GetRenderManager();
    if (!renderManager || renderManager->GetRenderDriverCount() != 1) {
        plugin->m_ExitInstanceFct(context);
        CKCloseContext(context);
        CKShutdown();
        return Fail("the NULL rasterizer was not the sole render driver");
    }

    VxDriverDesc *driver = renderManager->GetRenderDriverDescription(0);
    if (!driver || std::strstr(driver->DriverDesc, "NULL Rasterizer") == nullptr) {
        plugin->m_ExitInstanceFct(context);
        CKCloseContext(context);
        CKShutdown();
        return Fail("unexpected headless render driver");
    }

    CKObject *meshObject = context->CreateObject(
        CKCID_MESH, "HeadlessMesh", CK_OBJECTCREATION_DYNAMIC);
    CKObject *entityObject = context->CreateObject(
        CKCID_3DENTITY, "HeadlessEntity", CK_OBJECTCREATION_DYNAMIC);
    CKObject *sceneObject = context->CreateObject(
        CKCID_SCENE, "HeadlessScene", CK_OBJECTCREATION_DYNAMIC);

    RCKMesh *mesh = meshObject && CKIsChildClassOf(meshObject, CKCID_MESH)
        ? static_cast<RCKMesh *>(meshObject) : nullptr;
    RCK3dEntity *entity =
        entityObject && CKIsChildClassOf(entityObject, CKCID_3DENTITY)
            ? static_cast<RCK3dEntity *>(entityObject) : nullptr;
    CKScene *scene = CKScene::Cast(sceneObject);
    if (!mesh || !entity || !scene) {
        plugin->m_ExitInstanceFct(context);
        CKCloseContext(context);
        CKShutdown();
        return Fail("mesh, 3D entity, or scene class registration is missing");
    }

    if (!mesh->SetVertexCount(3)) {
        plugin->m_ExitInstanceFct(context);
        CKCloseContext(context);
        CKShutdown();
        return Fail("registered RCKMesh implementation is not functional");
    }

    scene->AddObjectToScene(entity, FALSE);
    if (!scene->IsObjectHere(entity)) {
        plugin->m_ExitInstanceFct(context);
        CKCloseContext(context);
        CKShutdown();
        return Fail("registered RCK3dEntity could not join a CKScene");
    }

    plugin->m_ExitInstanceFct(context);
    CKCloseContext(context);
    CKShutdown();
    std::puts("CKRenderEngine headless smoke passed");
    return 0;
}
