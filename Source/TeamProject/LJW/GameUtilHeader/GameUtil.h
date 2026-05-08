#pragma once
#include "CoreMinimal.h"

namespace GameUtil
{
	template<typename T>
	T* CreateComponent(
		AActor* Owner, //필수 클래스
		bool isChild = true, //하위 계층 인지
		USceneComponent* InParent = nullptr, //상위 레퍼런스
		FName createName = NAME_None) //이름
	{
		static_assert(TIsDerivedFrom<T, USceneComponent>::IsDerived, "T is Not USceneComponent");
			if (!ensureMsgf(Owner, TEXT("CreateComponent Owner Null")))
			{
				return nullptr;
			}
		FName finalName = createName;
		if (finalName.IsNone())
		{
			//자동으로 이름 생성
			finalName = MakeUniqueObjectName(Owner, T::StaticClass(),
				T::StaticClass()->GetFName());
		}
		T* NewComp = Owner->CreateDefaultSubobject<T>(finalName);
		if (!ensureMsgf(NewComp, TEXT("CreateComponent CreateDefaultSubobject Failed ")))
		{
			return nullptr;
		}
		if (isChild) //하위 계층 인지
		{
			if (InParent == nullptr) //상위 레퍼런스 체크
			{
				InParent = Owner->GetRootComponent();
			}
			if (ensureMsgf(InParent, TEXT("GetRootComponent Parent Null")))
			{
				NewComp->SetupAttachment(InParent);
			}
		}
		else
		{
			Owner->SetRootComponent(NewComp);
		}
		return NewComp;
	} //CreateComponent
	template<typename T>
	T* CreateRootComponet(
		AActor* Owner,
		FName createName = NAME_None)
	{
		T* t = CreateComponent<T>(Owner, false, nullptr, createName);
		return t;
	}
	template<typename T>
	T* CreateComponentWithLocation(
		AActor* Owner,
		FVector vec = FVector::ZeroVector,
		bool isChild = true,
		USceneComponent* InParent = nullptr,
		FName createName = NAME_None)
	{
		T* t = CreateComponent<T>(Owner, isChild, InParent, createName);
		if (ensureMsgf(t, TEXT("GetRootComponent Parent Null")))
		{
			t->SetRelativeLocation(vec);
		}
		return t;
	}
}