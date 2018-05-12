#include "SaveDepth.hpp"

using namespace sl;
using namespace std;

// Sample variables
int current_value;
int step_camera_setting = 1;
CAMERA_SETTINGS camera_settings_ = CAMERA_SETTINGS_BRIGHTNESS;
string str_camera_settings = "BRIGHTNESS";
int count_save = 0;
int mode_PointCloud = 0;
int mode_Depth = 0;
POINT_CLOUD_FORMAT PointCloud_format;
DEPTH_FORMAT Depth_format;
long imgCnt = 0;



std::string getPointCloudFormatName(POINT_CLOUD_FORMAT f) {
	std::string str_;
	switch (f) {
	case POINT_CLOUD_FORMAT_XYZ_ASCII:
		str_ = "XYZ";
		break;
	case  POINT_CLOUD_FORMAT_PCD_ASCII:
		str_ = "PCD";
		break;
	case  POINT_CLOUD_FORMAT_PLY_ASCII:
		str_ = "PLY";
		break;
	case  POINT_CLOUD_FORMAT_VTK_ASCII:
		str_ = "VTK";
		break;
	default:
		break;
	}
	return str_;
}

std::string getDepthFormatName(DEPTH_FORMAT f) {
	std::string str_;
	switch (f) {
	case  DEPTH_FORMAT_PNG:
		str_ = "PNG";
		break;
	case  DEPTH_FORMAT_PFM:
		str_ = "PFM";
		break;
	case  DEPTH_FORMAT_PGM:
		str_ = "PGM";
		break;
	default:
		break;
	}
	return str_;
}

void processKeyEvent(Camera& zed, char &key) {
	switch (key) {
	case 'd':
	case 'D':
		saveDepth(zed, path + prefixDepth + to_string(count_save));
		break;

	case 'n': // Depth format
	case 'N':
	{
		mode_Depth++;
		Depth_format = static_cast<DEPTH_FORMAT> (mode_Depth % 3);
		std::cout << "Depth format: " << getDepthFormatName(Depth_format) << std::endl;
	}
	break;

	case 'p':
	case 'P':
		savePointCloud(zed, path + prefixPointCloud + to_string(count_save));
		break;


	case 'm': // Point cloud format
	case 'M':
	{
		mode_PointCloud++;
		PointCloud_format = static_cast<POINT_CLOUD_FORMAT> (mode_PointCloud % 4);
		std::cout << "Point Cloud format: " << getPointCloudFormatName(PointCloud_format) << std::endl;
	}
	break;

	case 'h': // Print help
	case 'H':
		cout << helpString << endl;
		break;

	case 'z': // Save side by side image
		saveSbSImage(zed, std::string("ZED_image") + std::to_string(count_save) + std::string(".png"));
		break;

	case 'g':
	{
		cv::destroyWindow("Imagel");
		cv::destroyWindow("Depth");
		while (true)
		{
			saveImageSeq(zed);
			char off = cv::waitKey(10);
			if (off == 'q') {
				cv::destroyWindow("Imageleft");
				cv::destroyWindow("ImageRight");
				break;
			}
		}
	}
		break;
	
	case 's':
		switchCameraSettings();
		break;
		// Increase camera settings value ('+' key)
	case '+':
	{
		current_value = zed.getCameraSettings(camera_settings_);
		zed.setCameraSettings(camera_settings_, current_value + step_camera_setting);
		std::cout << str_camera_settings << ": " << current_value + step_camera_setting << std::endl;
	}
	break;

		// Decrease camera settings value ('-' key)
	case '-':
	{
		current_value = zed.getCameraSettings(camera_settings_);
		if (current_value >= 1) {
			zed.setCameraSettings(camera_settings_, current_value - step_camera_setting);
			std::cout << str_camera_settings << ": " << current_value - step_camera_setting << std::endl;
		}
	}
	break;

		// Reset to default parameters
	case 'r':
	{
		std::cout << "Reset all settings to default" << std::endl;
		zed.setCameraSettings(sl::CAMERA_SETTINGS_BRIGHTNESS, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_CONTRAST, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_HUE, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_SATURATION, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_GAIN, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_EXPOSURE, -1, true);
		zed.setCameraSettings(sl::CAMERA_SETTINGS_WHITEBALANCE, -1, true);
	}
	break;
	}
	count_save++;
}

void savePointCloud(Camera& zed, std::string filename) {
	std::cout << "Saving Point Cloud... " << flush;
	bool saved = savePointCloudAs(zed, PointCloud_format, filename.c_str(), true);
	if (saved)
		std::cout << "Done" << endl;
	else
		std::cout << "Failed... Please check that you have permissions to write on disk" << endl;
}

void saveDepth(Camera& zed, std::string filename) {
	float max_value = std::numeric_limits<unsigned short int>::max();
	float scale_factor = max_value / zed.getDepthMaxRangeValue();

	std::cout << "Saving Depth Map... " << flush;
	bool saved = saveDepthAs(zed, Depth_format, filename.c_str(), scale_factor);
	if (saved)
		std::cout << "Done" << endl;
	else
		std::cout << "Failed... Please check that you have permissions to write on disk" << endl;
}

void saveSbSImage(Camera& zed, std::string filename) {
	Resolution image_size = zed.getResolution();

	cv::Mat sbs_image(image_size.height, image_size.width * 2, CV_8UC4);
	cv::Mat left_image(sbs_image, cv::Rect(0, 0, image_size.width, image_size.height));
	cv::Mat right_image(sbs_image, cv::Rect(image_size.width, 0, image_size.width, image_size.height));

	Mat buffer_sl;
	cv::Mat buffer_cv;

	zed.retrieveImage(buffer_sl, VIEW_LEFT);
	buffer_cv = cv::Mat(buffer_sl.getHeight(), buffer_sl.getWidth(), CV_8UC4, buffer_sl.getPtr<sl::uchar1>(MEM_CPU));
	//cv::cvtColor(buffer_cv, buffer_cv, CV_RGBA2RGB);
	//cv::imwrite(filename, buffer_cv);
	buffer_cv.copyTo(left_image);
	zed.retrieveImage(buffer_sl, VIEW_RIGHT);
	buffer_cv = cv::Mat(buffer_sl.getHeight(), buffer_sl.getWidth(), CV_8UC4, buffer_sl.getPtr<sl::uchar1>(MEM_CPU));
	//cv::cvtColor(buffer_cv, buffer_cv, CV_RGBA2RGB);
	//cv::imwrite(filename, buffer_cv);
	buffer_cv.copyTo(right_image);
	
	cv::cvtColor(sbs_image, sbs_image, CV_RGBA2RGB);
	//cv::imshow("sbsimage", sbs_image);
	cv::imwrite(filename, sbs_image);
}

void saveImageSeq(Camera& zed){
	Resolution image_size = zed.getResolution();
	Mat sl_left, sl_right;
	cv::Mat cv_left, cv_right;

	std::stringstream ss;
	ss << std::setfill('0') << std::setw(6) << imgCnt;
	std::string addrImgLeft = addr + "/image_0/" + ss.str() + ".png";
	std::string addrImgRight = addr + "/image_1/" + ss.str() + ".png";
	
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE_STANDARD;
	
	if (zed.grab(runtime_parameters) == SUCCESS)
	{
		zed.retrieveImage(sl_left, VIEW_LEFT, MEM_CPU);
		cv_left = cv::Mat(sl_left.getHeight(), sl_left.getWidth(), CV_8UC4, sl_left.getPtr<sl::uchar1>(MEM_CPU));
		cv::cvtColor(cv_left, cv_left, CV_RGBA2RGB);
		cv::imshow("ImageLeft", cv_left);
		cv::imwrite(addrImgLeft, cv_left);

		zed.retrieveImage(sl_right, VIEW_RIGHT);
		cv_right = cv::Mat(sl_right.getHeight(), sl_right.getWidth(), CV_8UC4, sl_right.getPtr<sl::uchar1>(MEM_CPU));
		cv::cvtColor(cv_right, cv_right, CV_RGBA2RGB);
		cv::imshow("ImageRight", cv_right);
		cv::imwrite(addrImgRight, cv_right);
	}
	imgCnt++;
}

/**
This function toggles between camera settings
**/
void switchCameraSettings() {
	step_camera_setting = 1;
	switch (camera_settings_) {
	case CAMERA_SETTINGS_BRIGHTNESS:
		camera_settings_ = CAMERA_SETTINGS_CONTRAST;
		str_camera_settings = "Contrast";
		std::cout << "Camera Settings: CONTRAST" << std::endl;
		break;

	case CAMERA_SETTINGS_CONTRAST:
		camera_settings_ = CAMERA_SETTINGS_HUE;
		str_camera_settings = "Hue";
		std::cout << "Camera Settings: HUE" << std::endl;
		break;

	case CAMERA_SETTINGS_HUE:
		camera_settings_ = CAMERA_SETTINGS_SATURATION;
		str_camera_settings = "Saturation";
		std::cout << "Camera Settings: SATURATION" << std::endl;
		break;

	case CAMERA_SETTINGS_SATURATION:
		camera_settings_ = CAMERA_SETTINGS_GAIN;
		str_camera_settings = "Gain";
		std::cout << "Camera Settings: GAIN" << std::endl;
		break;

	case CAMERA_SETTINGS_GAIN:
		camera_settings_ = CAMERA_SETTINGS_EXPOSURE;
		str_camera_settings = "Exposure";
		std::cout << "Camera Settings: EXPOSURE" << std::endl;
		break;

	case CAMERA_SETTINGS_EXPOSURE:
		camera_settings_ = CAMERA_SETTINGS_WHITEBALANCE;
		str_camera_settings = "White Balance";
		step_camera_setting = 100;
		std::cout << "Camera Settings: WHITE BALANCE" << std::endl;
		break;

	case CAMERA_SETTINGS_WHITEBALANCE:
		camera_settings_ = CAMERA_SETTINGS_BRIGHTNESS;
		str_camera_settings = "Brightness";
		std::cout << "Camera Settings: BRIGHTNESS" << std::endl;
		break;
	}
}
