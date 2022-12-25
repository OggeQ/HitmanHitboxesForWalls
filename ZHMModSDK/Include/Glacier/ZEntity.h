#pragma once

#include "ZString.h"
#include "TArray.h"
#include "Reflection.h"
#include "ZObject.h"
#include "Hooks.h"

class ZEntityType;
class ZActor;
class STypeID;
class ZEntityRef;

class IEntity :
	public IComponentInterface
{
public:
	virtual ~IEntity() {}
};

class ZPinFunctions
{
public:
	void (*trigger)();
	void (*unk1)();
	void (*unk2)();
	void (*unk3)();
	int32_t m_nPin;
};

class ZPinData
{
public:
	int32_t m_unk0x0;
	IType* m_pType;
};

class ZPin
{
public:
	int64_t m_nObjectOffset;
	ZPinFunctions* m_pPinFunctions;
	ZPinData* m_pPinData;
	void* m_unk0x18;
	int32_t m_nPin;
};

class ZEntityPropertyType
{
public:
	ZClassProperty* getPropertyInfo() const
	{
		return reinterpret_cast<ZClassProperty*>(reinterpret_cast<uintptr_t>(this) - 16);
	}
};

class ZEntityProperty
{
public:
	ZEntityPropertyType* m_pType;
	int64_t m_nOffset;
	uint32_t m_nPropertyId;
	uint32_t m_nUnk01;
	uint64_t m_nUnk02;
	uint32_t m_nUnk03;
};

class ZEntityInterface
{
public:
	STypeID* m_pTypeId;
	int64_t m_nOffset;
};

class ZEntityType
{
public:
	uint64_t m_nUnk0x0;
	TArray<ZEntityProperty>* m_pProperties01;
	TArray<ZEntityProperty>* m_pProperties02;
	PAD(0x08);
	TArray<ZEntityInterface>* m_pInterfaces; // 32
	PAD(0x10);
	TArray<ZPin>* m_pInputs;
	TArray<ZPin>* m_pOutputs;
	int64_t m_nLogicalParentEntityOffset;
	int64_t m_nOwningEntityOffset;
	uint64_t m_nEntityId;
};

// Size = 0x18
class ZEntityImpl :
	public IEntity
{
public:
	enum class EEntityFlags
	{
		ENTITYFLAG_INITIALIZED = 1,
		ENTITYFLAG_POSTINITIALIZED = 2,
		ENTITYFLAG_ACTIVATED = 4,
		ENTITYFLAG_PREDELETED = 8,
		ENTITYFLAG_EDITMODE = 16,
		ENTITYFLAG_READONLY_FLAG_SET = 32,
		ENTITYFLAG_READONLY = 64
	};

	virtual ~ZEntityImpl() {}
	virtual ZEntityRef* GetID(ZEntityRef* result) = 0;
	virtual void Activate(int) = 0;
	virtual void Deactivate(int) = 0;
	virtual void ZEntityImpl_unk8() = 0;
	virtual void ZEntityImpl_unk9() = 0;
	virtual void ZEntityImpl_unk10() = 0;
	virtual void ZEntityImpl_unk11() = 0;
	virtual void ZEntityImpl_unk12() = 0;
	virtual void ZEntityImpl_unk13() = 0;
	virtual void ZEntityImpl_unk14() = 0;
	virtual void Start() = 0;
	virtual void ZEntityImpl_unk16() = 0;
	virtual void ZEntityImpl_unk17() = 0;
	virtual void ZEntityImpl_unk18() = 0;
	virtual void ZEntityImpl_unk19() = 0;

public:
	ZEntityType* m_pType;
	uint32_t m_nEntityPtrIndex;
	uint32_t m_nEntityFlags;
};

class ZEntityRef
{
public:
	ZEntityType** m_pEntity = nullptr;

public:
	ZEntityRef()
	{		
	}

	bool operator==(const ZEntityRef&) const = default;
	
	ZEntityImpl* GetBaseEntity()
	{
		if (!m_pEntity || !*m_pEntity)
			return nullptr;

		/*if (!(*m_pEntity)->m_pInterfaces)
			return nullptr;

		for (auto& s_Interface : *(*m_pEntity)->m_pInterfaces)
		{
			if (!s_Interface.m_pTypeId || !s_Interface.m_pTypeId->typeInfo())
				continue;

			// TODO: Cache the type instead.
			if (s_Interface.m_pTypeId->typeInfo()->m_pTypeName == std::string("void"))
			{
				auto s_RealPtr = reinterpret_cast<uintptr_t>(m_pEntity) + s_Interface.m_nOffset;
				return reinterpret_cast<ZEntityImpl*>(s_RealPtr);
			}
		}*/

		auto s_RealPtr = reinterpret_cast<uintptr_t>(m_pEntity) - sizeof(uintptr_t);
		return reinterpret_cast<ZEntityImpl*>(s_RealPtr);
	}

	template <class T>
	T* QueryInterface()
	{
		if (!m_pEntity || !*m_pEntity || !(*m_pEntity)->m_pInterfaces)
			return nullptr;

		if (!*Globals::TypeRegistry)
			return nullptr;

		auto it = (*Globals::TypeRegistry)->m_types.find(ZHMTypeName<T>);

		if (it == (*Globals::TypeRegistry)->m_types.end())
			return nullptr;

		for (auto& s_Interface : *(*m_pEntity)->m_pInterfaces)
		{
			if (s_Interface.m_pTypeId == it->second)
			{
				return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_pEntity) + s_Interface.m_nOffset);
			}
		}

		return nullptr;
	}

	template <class T>
	bool HasInterface() const
	{
		if (!m_pEntity || !*m_pEntity || !(*m_pEntity)->m_pInterfaces)
			return false;

		if (!*Globals::TypeRegistry)
			return false;

		auto it = (*Globals::TypeRegistry)->m_types.find(ZHMTypeName<T>);

		if (it == (*Globals::TypeRegistry)->m_types.end())
			return false;

		for (auto& s_Interface : *(*m_pEntity)->m_pInterfaces)
		{
			if (s_Interface.m_pTypeId == it->second)
			{
				return true;
			}
		}

		return false;
	}

	/*template <class T>
	ZVariant<T> GetProperty(uint32_t p_PropertyId) const
	{
		// TODO: Type checks?
		ZVariant<T> s_Property;
		Hooks::GetPropertyValue->Call(*this, p_PropertyId, s_Property.As<T>());
		return std::move(s_Property);
	}

	template <class T>
	ZVariant<T> GetProperty(const ZString& p_PropertyName) const
	{
		return std::move(GetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size())));
	}

	ZObjectRef GetProperty(uint32_t p_PropertyId) const
	{
		ZObjectRef s_Property;

		if (!(*m_pEntity)->m_pProperties01)
			return std::move(s_Property);

		for (auto& s_PropertyType : *(*m_pEntity)->m_pProperties01)
		{
			if (s_PropertyType.m_nPropertyId != p_PropertyId)
				continue;

			auto* s_Type = s_PropertyType.m_pType->getPropertyInfo()->m_pType;

			auto* s_Data = (*Globals::MemoryManager)->m_pNormalAllocator->AllocateAligned(
				s_Type->typeInfo()->m_nTypeSize,
				s_Type->typeInfo()->m_nTypeAlignment
			);

			// TODO: Re-implement this.
			//Hooks::GetPropertyValue->Call(*this, p_PropertyId, s_Data);

			s_Property.Assign(s_Type, s_Data);

			break;
		}

		return std::move(s_Property);
	}

	ZObjectRef GetProperty(const ZString& p_PropertyName) const
	{
		return std::move(GetProperty(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size())));
	}*/

	template <typename T>
	ZVariant<T> GetProperty(const uint32_t nPropertyID) const
	{
		ZObjectRef property;

		for (uint32_t i = 0; i < m_pEntity[0]->m_pProperties01->size(); ++i)
		{
			ZEntityProperty* entityProperty = &m_pEntity[0]->m_pProperties01->operator[](i);

			if (entityProperty->m_nPropertyId == nPropertyID)
			{
				ZClassProperty* classProperty = entityProperty->m_pType->getPropertyInfo();
				char* property2 = reinterpret_cast<char*>(m_pEntity) + entityProperty->m_nOffset;

				uint16_t typeSize = classProperty->m_pType->typeInfo()->m_nTypeSize;
				uint16_t typeAlignment = classProperty->m_pType->typeInfo()->m_nTypeAlignment;
				auto* data = (*Globals::MemoryManager)->m_pNormalAllocator->AllocateAligned(typeSize, typeAlignment);;

				if (classProperty->m_nFlags & EPropertyInfoFlags::E_HAS_GETTER_SETTER)
				{
					classProperty->get(property2, data, classProperty->m_nOffset);
				}
				else
				{
					classProperty->m_pType->typeInfo()->m_pTypeFunctions->copyConstruct(data, property2);
				}

				property.Assign(classProperty->m_pType, data);

				break;
			}
		}

		return std::move(ZVariant<T>(property));
	}

	template <typename T>
	ZVariant<T> GetProperty(const ZString& p_PropertyName) const
	{
		uint32_t nPropertyID = Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size());

		return std::move(GetProperty<T>(nPropertyID));
	}

	bool SetProperty(uint32_t p_PropertyId, const ZObjectRef& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return Hooks::SetPropertyValue->Call(*this, p_PropertyId, p_Value, p_InvokeChangeHandlers);
	}

	bool SetProperty(const ZString& p_PropertyName, const ZObjectRef& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return SetProperty(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(uint32_t p_PropertyId, const T& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return Hooks::SetPropertyValue->Call(*this, p_PropertyId, ZVariant<T>(p_Value), p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(const ZString& p_PropertyName, const T& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return SetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(uint32_t p_PropertyId, const ZVariant<T>& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return Hooks::SetPropertyValue->Call(*this, p_PropertyId, p_Value, p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(const ZString& p_PropertyName, const ZVariant<T>& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return SetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(uint32_t p_PropertyId, const ZVariantRef<T>& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return Hooks::SetPropertyValue->Call(*this, p_PropertyId, p_Value, p_InvokeChangeHandlers);
	}

	template <class T>
	bool SetProperty(const ZString& p_PropertyName, const ZVariantRef<T>& p_Value, bool p_InvokeChangeHandlers = true)
	{
		return SetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
	}

	void SignalInputPin(const ZString& p_PinName, const ZObjectRef& p_Data = ZObjectRef()) const
	{
		SignalInputPin(Hash::Crc32(p_PinName.c_str(), p_PinName.size()), p_Data);
	}

	void SignalInputPin(uint32_t p_PinId, const ZObjectRef& p_Data = ZObjectRef()) const
	{
		Hooks::SignalInputPin->Call(*this, p_PinId, p_Data);
	}

	void SignalOutputPin(const ZString& p_PinName, const ZObjectRef& p_Data = ZObjectRef()) const
	{
		SignalOutputPin(Hash::Crc32(p_PinName.c_str(), p_PinName.size()), p_Data);
	}

	void SignalOutputPin(uint32_t p_PinId, const ZObjectRef& p_Data = ZObjectRef()) const
	{
		Hooks::SignalOutputPin->Call(*this, p_PinId, p_Data);
	}

	operator bool() const
	{
		return m_pEntity != nullptr && (*m_pEntity) != nullptr;
	}

	struct hasher
	{
		size_t operator()(const ZEntityRef& p_Ref) const noexcept
		{
			return reinterpret_cast<uintptr_t>(p_Ref.m_pEntity);
		}
	};
};

template <typename T>
class TEntityRef
{
public:
	ZEntityRef m_ref;
	T* m_pInterfaceRef = nullptr;

	operator bool() const
	{
		return m_ref && m_pInterfaceRef != nullptr;
	}
};

class ZRepositoryItemEntity :
	public ZEntityImpl
{
public:
	ZRepositoryID m_sId;
};
