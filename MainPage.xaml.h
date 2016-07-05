#pragma once

#include "MainPage.g.h"

namespace ImageEffect
{
	public ref class MainPage sealed
	{
	    public:
		    MainPage();

	    protected:
		    virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	    private:
			Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_bitmap;
			Platform::String^ inputFilename;
			Platform::String^ outputFilename;
			void LoadImage();
			void DisplayProgress();
			void HideProgress();
			void GreyScale_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void Load_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void SobelEffect_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void GaussianBlur_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void Remap_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void Border_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void Save_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
