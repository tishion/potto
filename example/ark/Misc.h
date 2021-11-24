#pragma once
#include <potto/module/pottoobject.hpp>
#include "include/ILogger.h"
#include "include/IPlayer.h"

using namespace Potto;

/// <summary>
/// class Misc
/// 	: public PottoThreadSafeObjectRoot
/// 	, public PottoObjectClass<Misc>
/// 	, public ILogger
/// {
/// public:
/// 	static const PottoUuid& CLSID() { static PottoUuid _clsid_("a85985be-7639-4d06-838d-e9d30d76785e"); return _clsid_; }
/// 	POTTO_ERROR _InternalQueryInterface(const PottoUuid& iid, void** ppvObject)
/// 	{
/// 		return InternalQueryInterface(this, _GetEntries(), iid, ppvObject);
/// 	}
/// 	const static InterfaceEntry* _GetEntries()
/// 	{
/// 		typedef Misc _PottoObjectType;
/// 		static const InterfaceEntry _entries[] =
/// 		{
/// 			{ &Misc::CLSID(), ((ptrdiff_t)(static_cast<ILogger*>((Misc*)0x08)) - 0x08) },
/// 			{ 0, 0 }
/// 		};
/// 		return _entries;
/// 	}
/// 	virtual unsigned long  AddRef() = 0;
/// 	virtual unsigned long  Release() = 0;
/// 	virtual POTTO_ERROR QueryInterface(const PottoUuid& interfaceId, void** pp) = 0;
/// 
/// 	...
/// 	
/// };
/// </summary>
POTTO_MULTITHREADED_OBJECT("a85985be-7639-4d06-838d-e9d30d76785e", Misc)
	POTTO_IMPLEMENT_INTERFACE(ILogger)
	POTTO_IMPLEMENT_INTERFACE(IPlayer)
{
public:
	POTTO_BEGIN_INTERFACE_MAP(Misc)
		POTTO_INTERFACE_ENTRY(ILogger)
		POTTO_INTERFACE_ENTRY(IPlayer)
	POTTO_END_INTERFACE_MAP()

	Misc();

	POTTO_ERROR FinalConstruct();

	void FinalRelease();

	virtual void Info(char* msg) override;


	virtual void Warn(char* msg) override;


	virtual void Error(char* msg) override;


	virtual void Fatal(char* msg) override;


	virtual bool Play(char* file) override;
};

/// <summary>
/// 
/// </summary>
POTTO_CLASS_REGISTER(Misc);

