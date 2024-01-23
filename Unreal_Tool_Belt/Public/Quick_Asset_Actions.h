// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"

#include "Quick_Asset_Actions.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_TOOL_BELT_API UQuick_Asset_Actions : public UAssetActionUtility
{
	GENERATED_BODY()

public:

	UFUNCTION(CallInEditor)
	void Asset_Batch_Duplication(int32 Number_Of_Duplicates);

	UFUNCTION(CallinEditor)
	void Asset_Batch_Renaming(FString New_Name);
	
	UFUNCTION(CallInEditor)
	void Add_Prefix_To_Selection();
	
	UFUNCTION(CallinEditor)
	void Delete_Unused_Assets();

	
	

private:
	
	//A TMap is a more effecient Data Structure to use for fast search when compared to a conventional array (in a sense it is similar to a 2D vector or 2D array).
	TMap<UClass*, FString> Prefix_Map
	{
		{UUserWidget::StaticClass(), TEXT("WBP_")},
		{USkeletalMeshComponent::StaticClass(), TEXT("SK_")},
		{UStaticMesh::StaticClass(), TEXT("SM_")},
		{UBlueprint::StaticClass(), TEXT("BP_")},
		{UTexture::StaticClass(), TEXT("T_")},
		{UTexture2D::StaticClass(), TEXT("T_")},
		{UMaterial::StaticClass(), TEXT("M_")},
		{UMaterialInstanceConstant::StaticClass(),TEXT("MI_")},
		{UMaterialFunctionInterface::StaticClass(), TEXT("MF_")},
		{USoundWave::StaticClass(), TEXT("SW_")},
		{USoundCue::StaticClass(), TEXT("SC_")},
		{UParticleSystem::StaticClass(), TEXT("PS_")},
		{UNiagaraSystem::StaticClass(), TEXT("NS_")},
		{UNiagaraEmitter::StaticClass(), TEXT("NE_")}
	};

	void Fix_Up_Redirectors();
	
};
