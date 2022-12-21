#pragma once

#include "Common.h"
#include "EngineFunction.h"
#include "Glacier/ZEntity.h"
#include "Glacier/ZPrimitives.h"

class ZHitman5;
class ZActor;
class ZDynamicObject;
class ZString;
class ZHM5BaseCharacter;
class ZCameraEntity;
class ZSpatialEntity;
class ZEngineAppCommon;
class IRenderDestinationEntity;
class ZInputAction;
class ZHM5InputControl;
class ZEntityRef;
class ZEntityManager;
class IEntityFactory;
struct SMatrix;
class ZGlobalOutfitKit;
class ZItemSpawner;
class ZCharacterSubcontrollerInventory;

class ZHMSDK_API Functions
{
public:
	static EngineFunction<void(ZActor* th)>* ZActor_OnOutfitChanged;
	static EngineFunction<void(ZActor* th)>* ZActor_ReviveActor;
	static EngineFunction<void(ZDynamicObject* th, ZString* a2)>* ZDynamicObject_ToString;
	static EngineFunction<void(ZHM5BaseCharacter* th, bool activate)>* ZHM5BaseCharacter_ActivateRagdoll;
	static EngineFunction<ZCameraEntity*()>* GetCurrentCamera;
	static EngineFunction<void(ZSpatialEntity* th, SMatrix* out)>* ZSpatialEntity_WorldTransform;
	static EngineFunction<void(ZEngineAppCommon* th)>* ZEngineAppCommon_CreateFreeCamera;
	static EngineFunction<TEntityRef<IRenderDestinationEntity>* (ZCameraManager* th, TEntityRef<IRenderDestinationEntity>* result)>* ZCameraManager_GetActiveRenderDestinationEntity;
	static EngineFunction<double(ZInputAction* th, int a2)>* ZInputAction_Analog;
	static EngineFunction<bool(ZInputAction* th, int a2)>* ZInputAction_Digital;
	static EngineFunction<void(ZPlayerRegistry* th, TEntityRef<ZHitman5>* out)>* ZPlayerRegistry_GetLocalPlayer;
	static EngineFunction<ZHM5InputControl*(ZHM5InputManager* th)>* ZHM5InputManager_GetInputControlForLocalPlayer;
	static EngineFunction<void(ZResourceManager* th, int index)>* ZResourceManager_UninstallResource;
	static EngineFunction<void(ZEntityManager* th, ZEntityRef& result, const ZString& debugName, IEntityFactory* factory, const ZEntityRef& parent, void* a6, int64_t a7)>* ZEntityManager_NewEntity;
	static EngineFunction<void(ZSpatialEntity* th)>* ZSpatialEntity_UnknownTransformUpdate;
	static EngineFunction<void(ZEntityManager* th, const ZEntityRef& entity, THashMap<ZRuntimeResourceID, ZEntityRef>& references)>* ZEntityManager_DeleteEntity;
	static EngineFunction<void(ZHitman5* th, TEntityRef<ZGlobalOutfitKit> rOutfitKit, int nCharset, int nVariation, bool unk0, bool unk2)>* ZHitman5_SetOutfit;
	static EngineFunction<void(ZActor* th, TEntityRef<ZGlobalOutfitKit> rOutfitKit, int m_nOutfitCharset, int m_nOutfitVariation, bool bNude)>* ZActor_SetOutfit;
	static EngineFunction<void(ZItemSpawner* th)>* ZItemSpawner_RequestContentLoad;
	static EngineFunction<unsigned long long(ZCharacterSubcontrollerInventory* th, const ZRepositoryID& repId, const ZString& sOnlineInstanceId, void* unknown, unsigned int unknown2)>* ZCharacterSubcontrollerInventory_AddDynamicItemToInventory;
};
