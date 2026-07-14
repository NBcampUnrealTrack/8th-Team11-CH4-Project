#include "UI/Chat/MGChatPopupList.h"
#include "UI/Chat/MGChatPopup.h"
#include "Components/VerticalBox.h"

void UMGChatPopupList::AddPopup(const FMGChatType& InChatMessage)
{
	if (IsValid(VerticalBox_Popups) == false || IsValid(PopupClass) == false)
	{
		return;
	}

	while (VerticalBox_Popups->GetChildrenCount() >= MaxPopupCount)
	{
		VerticalBox_Popups->RemoveChildAt(0);
	}

	UMGChatPopup* NewPopup = CreateWidget<UMGChatPopup>(this, PopupClass);
	if (IsValid(NewPopup) == true)
	{
		VerticalBox_Popups->AddChild(NewPopup);
		NewPopup->ShowMessage(InChatMessage);
	}
}

void UMGChatPopupList::ClearAllPopups()
{
	if (IsValid(VerticalBox_Popups) == true)
	{
		VerticalBox_Popups->ClearChildren();
	}
}