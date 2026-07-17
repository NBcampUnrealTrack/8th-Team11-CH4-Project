// UW_MinigameIntro.cpp


#include "UI/Loading/UW_MinigameIntro.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UUW_MinigameIntro::Setup(const FMGMinigameInfoRow& Row)
{
	if (IsValid(Text_Title))
	{
		Text_Title->SetText(Row.Title);
	}

	if (IsValid(Text_Subtitle))
	{
		Text_Subtitle->SetText(Row.Subtitle);
	}

	if (IsValid(Text_IllustrationCaption))
	{
		Text_IllustrationCaption->SetText(Row.IllustrationCaption);
	}

	if (IsValid(Text_Goal))
	{
		Text_Goal->SetText(Row.GoalText);
	}

	if (IsValid(Image_Illustration))
	{
		if (UTexture2D* Tex = Row.Illustration.LoadSynchronous())
		{
			Image_Illustration->SetBrushFromTexture(Tex);
		}
	}
	
	if (IsValid(Box_Rules))
	{
		Box_Rules->ClearChildren();

		for (int32 i = 0; i < Row.RuleLines.Num(); ++i)
		{
			UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>();
			if (IsValid(Line))
			{
				const FString LineText = FString::Printf(TEXT("%d. %s"), i + 1, *Row.RuleLines[i].ToString());
				Line->SetText(FText::FromString(LineText));
				Line->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
				Line->SetAutoWrapText(true);
				
				Box_Rules->AddChildToVerticalBox(Line);
			}
		}
	}

	this->RemoveFromParent();
	this->AddToViewport(9999);
}
