#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/notifications/NotificationManager.h"


void Print_On_Screen(const FColor& Color, const FString& String)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, Color, String);
	}
}

void Print_In_Log(const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *String);
}

EAppReturnType::Type Show_Message_Dialog(EAppMsgType::Type Message_Type, const FString& Message, bool bShow_Message_As_Warning = true)
{
	//Checking to see how the FMessageDialoug should be implemented.
	if (bShow_Message_As_Warning)
	{
		//Displays a message dialog on the screen.
		FText Message_Title = FText::FromString(TEXT("WARNING"));
		//                    "Open" returns an EAppReturnType data type.
		return FMessageDialog::Open(Message_Type, FText::FromString(Message), Message_Title);
	}
	else
	{
		//Display message not as a warning. 
		return FMessageDialog::Open(Message_Type, FText::FromString(Message));
	}
}

void Show_Notification_Information(const FString& Message)
{
	//Customizing the pop up notification message which appears at the bottom right corner of the editor.
	FNotificationInfo Notification_Information(FText::FromString(Message));
	Notification_Information.bUseLargeFont = true;
	Notification_Information.FadeOutDuration = 10.f;

	//Implementation of the custom pop up notification into the editor.
	FSlateNotificationManager::Get().AddNotification(Notification_Information);

}