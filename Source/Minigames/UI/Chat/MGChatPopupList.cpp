#include "UI/Chat/MGChatPopupList.h"
#include "UI/Chat/MGChatPopup.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

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
		UVerticalBoxSlot* PopupSlot = VerticalBox_Popups->AddChildToVerticalBox(NewPopup);
		if (PopupSlot != nullptr)
		{
			PopupSlot->SetHorizontalAlignment(HAlign_Left);
			PopupSlot->SetPadding(FMargin(0.0f, 2.0f)); 
		}

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