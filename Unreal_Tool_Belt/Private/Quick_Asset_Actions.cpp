// Fill out your copyright notice in the Description page of Project Settings.


#include "Quick_Asset_Actions.h"
#include "Debug_Helper.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UQuick_Asset_Actions::Asset_Batch_Duplication(int32 Number_Of_Duplicates)
{
	//Get the selected FAssetData from the selection.
	TArray<FAssetData> Selected_Assets_Data{UEditorUtilityLibrary::GetSelectedAssetData()};
	//Number of Assets which are duplicated.
	uint32 Number_Of_Duplicated_Assets{};

	for(const FAssetData& Selected_Asset_Data : Selected_Assets_Data)
	{
		for(int32 i{}; i < Number_Of_Duplicates; i++)
		{
			/*Generating The Folder Path*/
			
			//Source Asset Path                                   ObjectPath is an FName and needs to be converted to an FString.
			const FString Source_Asset_Path{Selected_Asset_Data.GetObjectPathString()};
			//New Path Name for the new duplicated asset.                 AssetName is an FName and needs to be converted to an FString + Need to append the suffix for the duplicated asset by incrementing the "i" as the suffix Since "i" starts at 0, it needs to be incremented by 1.
			const FString Duplicated_Asset_Path{Selected_Asset_Data.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1)};
			//The folder which the new duplcated asset will be stored in. "Combine" takes care of merging the two strings from ^ into a folder structure (".PackagePath" is the folder name).
			const FString New_Path_Name{FPaths::Combine(Selected_Asset_Data.PackagePath.ToString(), Duplicated_Asset_Path)};
			
			/*Duplicating The Asset*/
			//Returns the pointer for the duplicated asset (Checking if it was successful).
			if(UEditorAssetLibrary::DuplicateAsset(Source_Asset_Path, New_Path_Name))
			{
				//Saving the Asset which was duplicated. Using false because "bOnlyIfIsDirty" saves only when changes are made. The Asset needs to be saved regardless of this.   
				UEditorAssetLibrary::SaveAsset(New_Path_Name, false);
				//pre-incrementing the "Counter" value.
				++Number_Of_Duplicated_Assets;
			}
		}
	}
	//Only "Show_Notification_Information" if the "Counter" is greater than 0.
	if(Number_Of_Duplicated_Assets > 0)
	{
		//Print_On_Screen(FColor::Green, FString("Successfully Duplicated " + FString::FromInt(Counter) + "files"));
		//Calling custom notification pop up message.
		Show_Notification_Information(TEXT("Successfully duplicated " + FString::FromInt(Number_Of_Duplicated_Assets) + " file(s)"));
	}

	//If the called amount of asset duplicates are less than 0 display a warning.
	if(Number_Of_Duplicates <= 0)
	{
		//Print_On_Screen(FColor::Red, FString("Please_Enter_A_Valid_Number"));
		Show_Message_Dialog(EAppMsgType::Ok, TEXT("This is not possible."));
		return;
	}
}

void UQuick_Asset_Actions::Asset_Batch_Renaming(FString New_Name)
{
	//Get the selected FAssetData from the selection.
	TArray<UObject*> Selected_Objects{UEditorUtilityLibrary::GetSelectedAssets()};
	//Number of Assets which are renamed.
	int32 Number_Of_Renamed_Assets{};

	for(UObject* Selected_Object:Selected_Objects)
	{ 
		//If pointer is not valid then start the next loop.
		if(!Selected_Object) continue;

		//Initialize an "FString" variable to store the old name.
		FString Old_Name{Selected_Object->GetName()};

		//Check to see if the selected object has the same name which was entered as the name to update it to. If this is the case display a message on the screen then continue to the next lopp.
		if(Old_Name.Equals(New_Name))
		{
			Show_Message_Dialog(EAppMsgType::Ok, TEXT("The name entered is the current name of the asset."));
			return;
		}

		//Declare the input parameter for renaming the asset which takes into account the number of assets which are being renamed.
		FString Updated_New_Name{};
		if(Number_Of_Renamed_Assets == 0)
		{
			Updated_New_Name = New_Name;
		}
		else
		{
			Updated_New_Name = New_Name + TEXT("_" + FString::FromInt(Number_Of_Renamed_Assets));
		}

		//Rename Asset
		UEditorUtilityLibrary::RenameAsset(Selected_Object, Updated_New_Name);

		//Pre-Increment the "Counter.
		++Number_Of_Renamed_Assets;
	}

	//Display pop-up window at the bottom right corner of the screen which states how many prefixes were added.
	if(Number_Of_Renamed_Assets > 0)
	{
		Show_Notification_Information(TEXT("Successfully renamed " + FString::FromInt(Number_Of_Renamed_Assets) + " asset(s)."));
	}
}

void UQuick_Asset_Actions::Add_Prefix_To_Selection()
{
	//Get selected objects
					 /*GetSelectedAssets is more suitable for this function than GetSelectedAssetData
	                 because UObject is only obtained by using GetSelectedAssets not GetSelectedAssetData.*/
	TArray<UObject*> Selected_Objects{UEditorUtilityLibrary::GetSelectedAssets()};
	//Keeps track of how many prefixes have been added.
	uint32 Number_Of_Prefixes_Added{};

	//Range based for loop which stores the information of the selceted object into "declared local pointer "Selected_Object".
	for(UObject* Selected_Object:Selected_Objects)
	{
		//If pointer is not valid then start the next loop.
		if(!Selected_Object) continue;

		//Use the information of the obtained class to get hold of it's corresponding prefix.
						                  //"Find" returns an FString*
		FString* Prefix_Found{ Prefix_Map.Find(Selected_Object->GetClass()) };

		//If the information obtained from the class doesn't contain an already set prefix display a message on the screen then continue to the next loop. 
		if(!Prefix_Found || Prefix_Found->IsEmpty())
		{
																								 //"GetName" is used to get the FString of the UClass.
			Show_Message_Dialog(EAppMsgType::Ok, TEXT("Prefix for ") + Selected_Object->GetClass()->GetName() + " could not be found.", false);
			continue;
		}

		//Initialize an "FString" variable to store the old name.
		FString Old_Name{Selected_Object->GetName()};

		//Check to see if the selected object already has in its name the prefix which is to be added. If this is the case display a message on the screen then continue to the next lopp.
		if(Old_Name.StartsWith(*Prefix_Found))
		{
			Show_Message_Dialog(EAppMsgType::Ok, TEXT("Prefix for " + Selected_Object->GetClass()->GetName()) + " is already included in the name.");
			continue;
		}

		//if the selected object is a material instance remove the parent "M_" and the suffix "_Inst" from the namespace so the only prefix added is "MI_".
		if(Selected_Object->IsA<UMaterialInstanceConstant>())
		{
			Old_Name.RemoveFromStart(TEXT("M_"));
			Old_Name.RemoveFromEnd(TEXT("_Inst"));
		}

		//Add prefix to the selected object(s).
		const FString New_Name_With_Prefix{*Prefix_Found + Old_Name};
		//Rename Asset
		UEditorUtilityLibrary::RenameAsset(Selected_Object, New_Name_With_Prefix);

		//Pre-Increment the "Counter.
		++Number_Of_Prefixes_Added;
	}

	//Display pop-up window at the bottom right corner of the screen which states how many prefixes were added.
	if(Number_Of_Prefixes_Added > 0)
	{
		Show_Notification_Information(TEXT("Successfully modified " + FString::FromInt(Number_Of_Prefixes_Added) + " asset name(s)."));
	}
}

void UQuick_Asset_Actions::Delete_Unused_Assets()
{
	/*Searching for references within the level to determine wheather an asset is used*/

	//Get hold of the selected assets and store them in a TArray of "FAssetData"
	TArray<FAssetData> Selected_Assets_Data{UEditorUtilityLibrary::GetSelectedAssetData()};
	//A TArray of type "AssetData" to store the unused selected assets. 
	TArray<FAssetData> Unused_Asset_Data{};

	//Fixes redirectors to ensure accurate results are achieved when the call to "Delete_Unused_Assets()" is made.
	void Fix_Up_Redirectors();

	//Ranged based for loop to interate through the selected asstet(s) and retrieve the data which is being saught.
	/*A "const FAssetData&" is declared locally within the range based for loop for the purpose of storing a single
	"Selected_Assets_Data"'s information during each loop of the proceeding ranged based for loop.*/
	for(const FAssetData& Selected_Asset_Data:Selected_Assets_Data)
	{
		//Obtaining references to selected asset(s). If the assets have references then they are being used and should not be deleted.
														/*"FindPackageReferencersForAsset" returns an array so the information here is stored
														in a local array named "Asset_References.*/
		const TArray Asset_References{UEditorAssetLibrary::FindPackageReferencersForAsset(Selected_Asset_Data.GetObjectPathString())};

		//Using the amount of elements (references) of the asset being analyzed in this range based for loop to determine wheather the asset being analyzed in this loop iteration is being used within the level.  
		if(Asset_References.Num() == 0)
		{
			//If the program makes it inside this scope then the asset being analyzed has no references and is ready to be deleted.
			//Add said asset into the elements of the array which will be deleted. This array is decalred above the range based for loop as "TArray<FAssetData> Unused_Asset_Data{};".
			Unused_Asset_Data.Add(Selected_Asset_Data);

			//The range based loop should continue until all the elements in the "Selected_Assets_Data" have been analyzed. Therefore the next loop needs to be initialized.
			continue;
		}
	}
	
	//Delete the selected assets using the "DeleteAssets" fucntion found within the ObjectTools name space. This function handles fixing up redirectors within the editor.
	//Store the number of deleted assets within an int32. "DeleteAssets" return an int32, therefore the value returned may be used on the pop up notification.
	int32 Number_Of_Deleted_Assets{ObjectTools::DeleteAssets(Unused_Asset_Data)};

	//If no assets were deleted the function call was terminated within the editor.
	if(Number_Of_Deleted_Assets == 0) return;

	//Display a message which serves as an notifcation of how many unused assets were deleted.
	Show_Notification_Information(TEXT("Successfully deleted " + FString::FromInt(Number_Of_Deleted_Assets) + " assets from the project"));
}

void UQuick_Asset_Actions::Fix_Up_Redirectors()
{
	//Array which will be filled with the asset redirectors which need be fixed.
	TArray<UObjectRedirector*> Redirectors_To_Fix{};

	//Storing the module in "IAssetRegistry& because only one instance of it can be initialized.  
	                            //"FModuleManager" is the module which will be used to get the redirectors from the editors content browser.
								                 //"LoadModuleChecked" will ensure that only one instance of the module will load at any given time. 
												 //If the module is already loaded and another instance attempts to load, the newly loaded instance will return;
	IAssetRegistry& Asset_Registry{FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get()};

	
	//This will help with filtering the assets which need the redirectors fixed.
	FARFilter Filter{};
	//This will allow access to all the subfolders in the project.
	Filter.bRecursivePaths = true;
	//This will directly determine which folder to target (Content folder).
	Filter.PackagePaths.Emplace("/Game");

	//This class path name will be used for searching for all redirectors.
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	//This array will be filled with the FAssetData of all the redirectors. 
	TArray<FAssetData> Asset_List{};
	
	//Obtains all redirectors in the content browser.
	Asset_Registry.GetAssets(Filter, Asset_List);

	if(Asset_List.Num() == 0) return;

	TArray<FString> Object_Paths{};

	//Range based for loop which will loop through the Array.
	for(const FAssetData& Asset:Asset_List)
	{
		Object_Paths.Add(Asset.GetObjectPathString());
	}

	TArray<UObject*> Objects;
	const bool bAllowed_To_Promt_To_Load_Assets{true};
	const bool bLoad_Redirects{true};
	
	if(AssetViewUtils::LoadAssetsIfNeeded(Object_Paths, Objects, bAllowed_To_Promt_To_Load_Assets, bLoad_Redirects))
	{
		//Casts the elements in the "Objects" array to elements of the type which may fit in a "ObjectRedirector" array.
		TArray<UObjectRedirector*> Redirectors;
		for(UObject* Object:Objects)
		{
			Redirectors.Add(CastChecked<UObjectRedirector>(Object));
		}

		//Loading the asset tools module
		FAssetToolsModule& Asset_Tools_Module{FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"))};
		//Obtrain the redirectors which were cast to type "UObjectRedirectors" in the ranged based for loop performed above.
		Asset_Tools_Module.Get().FixupReferencers(Redirectors);

		Show_Message_Dialog(EAppMsgType::Ok, TEXT("Fixing up redirectors complete."), false);
	}
}