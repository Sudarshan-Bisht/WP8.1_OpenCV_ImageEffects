#include "pch.h"
#include "MainPage.xaml.h"

#include <opencv2\imgproc\types_c.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <Robuffer.h>
#include <ppl.h>
#include <ppltasks.h>
#include <stdio.h>
#include <stdlib.h>

using namespace concurrency;
using namespace Platform;
using namespace ImageEffect;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI::Core;


MainPage::MainPage()
{
    InitializeComponent();
}


void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	if (inputFilename == nullptr)
	{
		GreyScale->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		SobelEffect->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		Border->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		Remap->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		Save->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		Reset->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
}


inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
}


byte* GetPointerToPixelData(IBuffer^ buffer)
{
    // Cast to Object^, then to its underlying IInspectable interface.
    Object^ obj = buffer;
    ComPtr<IInspectable> insp(reinterpret_cast<IInspectable*>(obj));

    // Query the IBufferByteAccess interface.
    ComPtr<IBufferByteAccess> bufferByteAccess;
    ThrowIfFailed(insp.As(&bufferByteAccess));

    // Retrieve the buffer data.
    byte* pixels = nullptr;
    ThrowIfFailed(bufferByteAccess->Buffer(&pixels));
    return pixels;
}


void ImageEffect::MainPage::GreyScale_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		outputFilename = outputFilename + "_B&W.jpg";

		// get the pixels from the WriteableBitmap
		byte* pPixels = GetPointerToPixelData(m_bitmap->PixelBuffer);
		int height = m_bitmap->PixelHeight;
		int width = m_bitmap->PixelWidth;

		// create a matrix the size and type of the image
		cv::Mat mat(height, width, CV_8UC4);
		memcpy(mat.data, pPixels, 4 * height*width);

		// convert to grayscale
		cv::Mat intermediateMat;
		cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);

		// convert to BGRA
		cv::cvtColor(intermediateMat, mat, CV_GRAY2BGRA);

		// copy processed image back to the WriteableBitmap
		memcpy(pPixels, mat.data, 4 * height*width);

		// update the WriteableBitmap
		m_bitmap->Invalidate();
	}
}


void ImageEffect::MainPage::SobelEffect_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		outputFilename = outputFilename + "_Edges.jpg";

		int ddepth = CV_16S;
		int scale = 1;
		int delta = 0;
		cv::Mat grad;

		// get the pixels from the WriteableBitmap
		byte* pPixels = GetPointerToPixelData(m_bitmap->PixelBuffer);
		int height = m_bitmap->PixelHeight;
		int width = m_bitmap->PixelWidth;

		// create a matrix the size and type of the image
		cv::Mat mat(height, width, CV_8UC4);
		memcpy(mat.data, pPixels, 4 * height*width);

		// apply blur
		cv::GaussianBlur(mat, mat, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

		// convert to grayscale
		cv::Mat intermediateMat;
		cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);


		// Apply Sobel filter
		/// Generate grad_x and grad_y
		cv::Mat grad_x, grad_y;
		cv::Mat abs_grad_x, abs_grad_y;

		/// Gradient X
		//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
		Sobel(intermediateMat, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
		convertScaleAbs(grad_x, abs_grad_x);

		/// Gradient Y
		//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
		Sobel(intermediateMat, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
		convertScaleAbs(grad_y, abs_grad_y);

		/// Total Gradient (approximate)
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

		// convert to BGRA
		cv::cvtColor(grad, mat, CV_GRAY2BGRA);

		// copy processed image back to the WriteableBitmap
		memcpy(pPixels, mat.data, 4 * height*width);

		// update the WriteableBitmap
		m_bitmap->Invalidate();
	}
}


void ImageEffect::MainPage::GaussianBlur_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		outputFilename = outputFilename + "_Blur.jpg";

		// get the pixels from the WriteableBitmap
		byte* pPixels = GetPointerToPixelData(m_bitmap->PixelBuffer);
		int height = m_bitmap->PixelHeight;
		int width = m_bitmap->PixelWidth;

		// create a matrix the size and type of the image
		cv::Mat mat(height, width, CV_8UC4);
		memcpy(mat.data, pPixels, 4 * height*width);

		// convert to blur
		//cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);
		cv::GaussianBlur(mat, mat, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

		// copy processed image back to the WriteableBitmap
		memcpy(pPixels, mat.data, 4 * height*width);

		// update the WriteableBitmap
		m_bitmap->Invalidate();
	}
}


void ImageEffect::MainPage::Remap_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		outputFilename = outputFilename + "_Mirror.jpg";

		// get the pixels from the WriteableBitmap
		byte* pPixels = GetPointerToPixelData(m_bitmap->PixelBuffer);
		int height = m_bitmap->PixelHeight;
		int width = m_bitmap->PixelWidth;
		cv::Mat dst;
		cv::Mat map_x, map_y;

		// create a matrix the size and type of the image
		cv::Mat src(height, width, CV_8UC4);
		memcpy(src.data, pPixels, 4 * height*width);

		/// Create dst, map_x and map_y with the same size as src:
		dst.create(src.size(), CV_8UC4);
		map_x.create(src.size(), CV_32FC1);
		map_y.create(src.size(), CV_32FC1);

		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				map_x.at<float>(i, j) = src.cols - j;
				map_y.at<float>(i, j) = i;
			}
		}

		// remap pixels
		remap(src, dst, map_x, map_y, CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

		// copy processed image back to the WriteableBitmap
		memcpy(pPixels, dst.data, 4 * height*width);

		// update the WriteableBitmap
		m_bitmap->Invalidate();
	}
}


void ImageEffect::MainPage::Border_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		outputFilename = outputFilename + "_Border.jpg";

		cv::RNG rng(12345);

		// get the pixels from the WriteableBitmap
		byte* pPixels = GetPointerToPixelData(m_bitmap->PixelBuffer);
		int height = m_bitmap->PixelHeight;
		int width = m_bitmap->PixelWidth;
		cv::Mat dst;
		int top, bottom, left, right;
		cv::Scalar value;
		int borderType = cv::BORDER_CONSTANT;

		// create a matrix the size and type of the image
		cv::Mat src(height, width, CV_8UC4);
		memcpy(src.data, pPixels, 4 * height*width);

		// Initialize arguments for the filter
		top = (int)(0.05*src.rows); bottom = (int)(0.05*src.rows);
		left = (int)(0.05*src.cols); right = (int)(0.05*src.cols);

		// Draw border
		value = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		copyMakeBorder(src, dst, top, bottom, left, right, borderType, value);

		// Resize the output so that it gets accommodated to WriteableBitmap
		resize(dst, dst, src.size());

		// copy processed image back to the WriteableBitmap
		memcpy(pPixels, dst.data, 4 * height*width);

		// update the WriteableBitmap
		m_bitmap->Invalidate();

	}
}


void ImageEffect::MainPage::Save_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		if (!(inputFilename == outputFilename + ".jpg"))
		{

			create_task(KnownFolders::CameraRoll->CreateFileAsync(outputFilename, CreationCollisionOption::GenerateUniqueName)).then([this](StorageFile^ file)
			{
				create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([this](IRandomAccessStream^ stream)
				{
					create_task(BitmapEncoder::CreateAsync(BitmapEncoder::JpegEncoderId, stream)).then([this](BitmapEncoder^ encoder)
					{
						IBuffer^ buffer = m_bitmap->PixelBuffer;
						DataReader^ dataReader = DataReader::FromBuffer(buffer);
						Platform::Array<unsigned char, 1>^ pixels = ref new Platform::Array<unsigned char, 1>(buffer->Length);
						dataReader->ReadBytes(pixels);
						encoder->SetPixelData(BitmapPixelFormat::Bgra8, BitmapAlphaMode::Ignore, m_bitmap->PixelWidth, m_bitmap->PixelHeight, 96.0, 96.0, pixels);
						create_task(encoder->FlushAsync());

						Save->Content = "Saved";
					});
				});
			});
		}
	}
}


void ImageEffect::MainPage::Load_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	FileOpenPicker^ openPicker = ref new FileOpenPicker();
	openPicker->ViewMode = PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".jpg");
	openPicker->FileTypeFilter->Append(".jpeg");
	openPicker->FileTypeFilter->Append(".png");

	create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
	{
		if (file)
		{
			inputFilename = file->Name;
			LoadImage();
		}
	});
}

void ImageEffect::MainPage::Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (inputFilename != nullptr)
	{
		LoadImage();
	}
}


void ImageEffect::MainPage::LoadImage()
{
	create_task(KnownFolders::CameraRoll->GetFileAsync(inputFilename)).then([this](StorageFile^ file)
	{
		outputFilename = file->DisplayName;
		create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([this](IRandomAccessStream^ stream)
		{
			m_bitmap = ref new Windows::UI::Xaml::Media::Imaging::WriteableBitmap(1, 1);
			m_bitmap->SetSource(stream);
			image->Source = m_bitmap;
		});
	});

	GreyScale->Visibility = Windows::UI::Xaml::Visibility::Visible;
	SobelEffect->Visibility = Windows::UI::Xaml::Visibility::Visible;
	Border->Visibility = Windows::UI::Xaml::Visibility::Visible;
	Remap->Visibility = Windows::UI::Xaml::Visibility::Visible;
	Save->Visibility = Windows::UI::Xaml::Visibility::Visible;
	Reset->Visibility = Windows::UI::Xaml::Visibility::Visible;
	Text->Text = "Select desired effect, processing takes time...";
	Save->Content = "Save";
}


